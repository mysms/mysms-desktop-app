/****************************************************************************
**
** mysms - Send & receive all your SMS on your Notebook, PC or tablet – like on your smartphone
** Copyright (C) 2013 Up To Eleven
** All rights reserved.
**
**
** This library is free software.  either version 2.1 of the License, or (at your option) any later version.
** you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation;
** either version 2.1 of the License, or (at your option) any later version.
**
**
** This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
** Without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License along with this library;
** if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
**
****************************************************************************/

#include "notificationpopup.h"

#include <QSound>
#include <QApplication>
#include <QVariant>
#include <QWebFrame>
#include <QString>

#include "mainwindow.h"
#include "logger.h"
#include "soundselector.h"
#include "globalSettings.h"

NotificationPopup::~NotificationPopup()
{}

NotificationPopup::NotificationPopup(QPixmap pixmapIcon, QString headerText, QString messageText, bool isGroupMessage, int messageId, QString address, QDateTime date, QWidget *parent) : QWidget(parent)
{
    Logger::log_message(QString(__func__));

    m_isGroupMessage = isGroupMessage;
    m_messageId = messageId;
    m_address = address;
    m_date = date;
    m_markedRemoveFromOverview = false;

    setWindowFlags(
        #ifdef Q_OS_MAC
            Qt::SubWindow | // This type flag is the second point
        #else
            Qt::Tool |
        #endif
            Qt::FramelessWindowHint |  Qt::WindowSystemMenuHint |  Qt::WindowStaysOnTopHint
    );

    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);   // set the parent widget's background to translucent
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_displayWidget.setGeometry(0, 0, width(), height());
    m_displayWidget.setStyleSheet(".QWidget { background-color: rgba(255, 255, 255, 100%); border-width: 0px; border-style: solid; border-radius: 10px; border-color: #CCCCCC; } .QWidget:hover {  border-width: 2px; border-style: solid; border-radius: 10px; border-color: #2ec1e6; }");    
    m_displayWidget.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_closeButton.setIcon(QIcon(QPixmap(":/resource/close.png")));
    m_closeButton.setMaximumSize(20,20);
    m_closeButton.setStyleSheet("QPushButton { border: none;  }");
    m_closeButton.setToolTip(tr("Close pop-up"));
    connect(&m_closeButton, SIGNAL(clicked()), this, SLOT(fadeOut()));

    m_readButton.setIcon(QIcon(QPixmap(":/resource/read.png")));
    m_readButton.setMaximumSize(20,20);
    m_readButton.setStyleSheet("QPushButton { border: none;   }");
    m_readButton.setToolTip(tr("Mark message as read"));
    connect(&m_readButton, SIGNAL(clicked()), this, SLOT(messageRead()));

    m_delButton.setIcon(QIcon(QPixmap(":/resource/del.png")));
    m_delButton.setMaximumSize(20,20);
    m_delButton.setStyleSheet("QPushButton { border: none;  }");
    m_delButton.setToolTip(tr("Delete message"));
    connect(&m_delButton, SIGNAL(clicked()), this, SLOT(messageDelete()));

    m_icon.setPixmap(pixmapIcon);
    m_icon.setStyleSheet("QLabel { border-width: 1px; Margin-left: 3px;  }");

    messageText.remove("\n");
    messageText.remove("\t");

    if (messageText.length() > POPUP_MESSAGE_MAX_SIZE)
    {
        messageText.resize(POPUP_MESSAGE_MAX_SIZE);
        messageText += "...";
    }  

    headerText.remove("\n");
    headerText.remove("\t");

    if (headerText.length() > POPUP_TEXT_MAX_SIZE)
    {
        headerText.resize(POPUP_TEXT_MAX_SIZE);
        headerText += "...";
    }    

    m_header.setWordWrap(false);
    m_header.setMaximumWidth(230);
    m_header.setText(headerText);
    m_header.setStyleSheet("QLabel { color: #030303; Margin-left: 3px;   font: bold Arial; font-size: 13px; }");

    //m_message.setMaximumSize(300, 52);
    m_message.setMaximumWidth(300);
    m_message.setMinimumWidth(300);

    m_message.setText(messageText);
    m_message.setWordWrap(true);    
    m_message.setStyleSheet("QLabel { color: #030303; Margin-left: 3px;  font-size: 13px; font: Arial; }");

    m_dateDisp.setText(m_date.toString("hh:mm:ss"));
    m_dateDisp.setStyleSheet("QLabel { color: #0f7ea6; font-size: 12px; }");
    m_dateDisp.setAlignment(Qt::AlignTop);

    m_ovLayout.addWidget(&m_icon, 0, Qt::AlignTop);

    m_messageDateLayout.addWidget(&m_message,  1, Qt::AlignLeft | Qt::AlignTop);
    m_messageDateLayout.addWidget(&m_dateDisp, 0, Qt::AlignRight);
    m_buttonLayout.addWidget(&m_header, 1, Qt::AlignLeft);
    m_buttonLayout.addWidget(&m_readButton);
    m_buttonLayout.addWidget(&m_delButton);
    m_buttonLayout.addWidget(&m_closeButton);
    m_headerButtonDateLayout.addLayout(&m_buttonLayout);
    m_headerButtonDateLayout.addLayout(&m_messageDateLayout);
    m_ovLayout.addLayout(&m_headerButtonDateLayout);
    m_displayWidget.setLayout(&m_ovLayout);

    m_shadow.setBlurRadius(9.0);
    m_shadow.setColor(QColor(0, 0, 0, 160));
    m_shadow.setOffset(0.0,1.0);

    m_displayWidget.setGraphicsEffect(&m_shadow);
    m_containerLayout.addWidget(&m_displayWidget);
    setLayout(&m_containerLayout);
}

void NotificationPopup::setFaceImage(QPixmap pixmapIcon)
{
    Logger::log_message(QString(__func__));

    m_icon.setPixmap(pixmapIcon);
}

int NotificationPopup::getMessageId() const
{
   Logger::log_message(QString(__func__));

    return m_messageId;
}

void NotificationPopup::fadeOut()
{
    Logger::log_message(QString(__func__));

    m_timeout.stop();
    emit deleted(this);
}

void NotificationPopup::messageDelete()
{
    Logger::log_message(QString(__func__));

    QString mycall = QString("messageService.deleteMessage(" + QString::number(m_messageId) + ",'" + m_address + "'," + QString("false") + ")");
    MainWindow::instance()->m_webview.page()->mainFrame()->evaluateJavaScript(mycall);

    m_markedRemoveFromOverview = true;
    fadeOut();
}

bool NotificationPopup::isItemMarkedToRemoveFromOverview() const
{
    Logger::log_message(QString(__func__));

    return m_markedRemoveFromOverview;
}

void NotificationPopup::messageRead()
{
    Logger::log_message(QString(__func__));

    QString mycall = QString("messageService.markMessageRead(%1, '%2')").arg(QString::number(m_messageId), m_address);
    MainWindow::instance()->m_webview.page()->mainFrame()->evaluateJavaScript(mycall);

    m_markedRemoveFromOverview = true;
    fadeOut();
}

bool NotificationPopup::isGroupNotification() const
{
   Logger::log_message(QString(__func__));

   return m_isGroupMessage;
}

void NotificationPopup::activatePopup()
{
    Logger::log_message(QString(__func__));

    show();
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(fadeOut()));
    startTimer(POPUP_FADEOUT_TIME);
}

void NotificationPopup::startTimer(int time)
{
    Logger::log_message(QString(__func__));

    m_timeout.start(time);
}

void NotificationPopup::stopTimer()
{
    Logger::log_message(QString(__func__));

    m_timeout.stop();
}

void NotificationPopup::mousePressEvent(QMouseEvent *event)
{
    Logger::log_message(QString(__func__));

    Qt::MouseButtons mouseButtons = event->buttons();

    if(mouseButtons == Qt::LeftButton)
    {
        MainWindow * mainWindow = MainWindow::instance();        

        Logger::log_message(m_address);

        QString urlEnc;

        if(m_isGroupMessage)
            urlEnc = m_address.replace("#", "%23");
        else
            urlEnc = m_address;

        Logger::log_message(m_address);
        Logger::log_message(urlEnc);

        mainWindow->loadPage("/#messages:" + urlEnc);

        if (mainWindow->isWindowClosed())
            mainWindow->openWindow();
    }
}
