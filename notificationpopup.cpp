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

#include <QApplication>
#include <QVariant>
#include <QWebFrame>
#include <QString>

#include "mainwindow.h"
#include "soundselector.h"
#include "globalsettings.h"

NotificationPopup::~NotificationPopup()
{
    disconnect(&m_timeout, SIGNAL(timeout()), this, SLOT(fadeOut()));
}

NotificationPopup::NotificationPopup(QPixmap pixmapIcon, QString headerText, QString messageText, bool isGroupMessage, int messageId, QString address, QDateTime date, QWidget *parent) : QWidget(parent)
{
    m_isGroupMessage = isGroupMessage;
    m_messageId = messageId;
    m_address = address;
    m_date = date;
    m_markedRemoveFromOverview = false;

    setWindowFlags(
        #ifdef Q_OS_UNIX
            Qt::SubWindow | Qt::X11BypassWindowManagerHint | // This type flag is the second point
        #else
            Qt::Tool |
        #endif
            Qt::FramelessWindowHint |  Qt::WindowSystemMenuHint |  Qt::WindowStaysOnTopHint
    );

    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);   // set the parent widget's background to translucent
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(fadeOut()));

    m_displayWidget.setGeometry(0, 0, width(), height());
    m_displayWidget.setStyleSheet(".QWidget { background-color: rgba(255, 255, 255, 100%); border-width: 0px; border-style: solid; border-radius: 2px; border-color: #CCCCCC; } .QWidget:hover {  border-width: 2px; border-style: solid; border-radius: 2px; border-color: #2ec1e6; }");
    m_displayWidget.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_displayWidget.installEventFilter(this);

    m_closeButton.setIcon(QIcon(QPixmap(":/resource/close.png")));
    m_closeButton.setMaximumSize(21,21);
    m_closeButton.setStyleSheet("QPushButton { border: none; }");
    m_closeButton.setToolTip(tr("Close pop-up"));
    m_closeButton.installEventFilter(this);
    m_closeButton.setCursor(Qt::PointingHandCursor);
    connect(&m_closeButton, SIGNAL(clicked()), this, SLOT(fadeOut()));

    m_readButton.setIcon(QIcon(QPixmap(":/resource/read.png")));
    m_readButton.setMaximumSize(21,21);
    m_readButton.setStyleSheet("QPushButton { border: none; }");
    m_readButton.setToolTip(tr("Mark message as read"));
    m_readButton.installEventFilter(this);
    m_readButton.setCursor(Qt::PointingHandCursor);
    connect(&m_readButton, SIGNAL(clicked()), this, SLOT(messageRead()));

    m_delButton.setIcon(QIcon(QPixmap(":/resource/del.png")));
    m_delButton.setMaximumSize(21,21);
    m_delButton.setStyleSheet("QPushButton { border: none; }");
    m_delButton.setToolTip(tr("Delete message"));
    m_delButton.installEventFilter(this);
    m_delButton.setCursor(Qt::PointingHandCursor);
    connect(&m_delButton, SIGNAL(clicked()), this, SLOT(messageDelete()));

    m_icon.setPixmap(pixmapIcon);
    m_icon.setStyleSheet("QLabel { border-width: 1px }");

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
    m_header.setStyleSheet("QLabel { color: #030303; margin-left: 3px; font-weight: bold; font-size: 13px; }");

    m_message.setMaximumWidth(300);
    m_message.setMinimumWidth(300);

    m_message.setText(messageText);
    m_message.setWordWrap(true);    
    m_message.setStyleSheet("QLabel { color: #030303; margin-left: 3px;  font-size: 13px; }");

    m_dateDisp.setText(m_date.toString(QLocale::system().timeFormat(QLocale::ShortFormat)));
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
    m_icon.setPixmap(pixmapIcon);
}

int NotificationPopup::getMessageId() const
{
    return m_messageId;
}

void NotificationPopup::fadeOut()
{
    m_timeout.stop();
    emit deleted(this);
}

void NotificationPopup::messageDelete()
{
    QString mycall = QString("messageService.deleteMessage(" + QString::number(m_messageId) + ",'" + m_address + "'," + QString("false") + ")");
    MainWindow::instance()->m_webview->page()->mainFrame()->evaluateJavaScript(mycall);

    m_markedRemoveFromOverview = true;
    fadeOut();
}

bool NotificationPopup::isItemMarkedToRemoveFromOverview() const
{
    return m_markedRemoveFromOverview;
}

void NotificationPopup::messageRead()
{
    QString mycall = QString("messageService.markMessageRead(%1, '%2')").arg(QString::number(m_messageId), m_address);
    MainWindow::instance()->m_webview->page()->mainFrame()->evaluateJavaScript(mycall);

    m_markedRemoveFromOverview = true;
    fadeOut();
}

bool NotificationPopup::isGroupNotification() const
{
   return m_isGroupMessage;
}

void NotificationPopup::activatePopup()
{
    show();
    startTimer(POPUP_FADEOUT_TIME);
}

void NotificationPopup::deactivatePopup()
{
    hide();
    stopTimer();
}

void NotificationPopup::activateFadeOut()
{
    startTimer(POPUP_HOVER_FADEOUT_TIME);
}

void NotificationPopup::deactivateFadeOut()
{
    stopTimer();
}

void NotificationPopup::startTimer(int time)
{
    m_timeout.start(time);
}

void NotificationPopup::stopTimer()
{
    m_timeout.stop();
}

bool NotificationPopup::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::Enter)
    {
        if (obj == &m_displayWidget)
            emit hover(this);
        else if (obj == &m_closeButton)
            m_closeButton.setIcon(QIcon(QPixmap(":/resource/close-hover.png")));
        else if (obj == &m_readButton)
            m_readButton.setIcon(QIcon(QPixmap(":/resource/read-hover.png")));
        else if (obj == &m_delButton)
            m_delButton.setIcon(QIcon(QPixmap(":/resource/del-hover.png")));
    }
    else if (event->type() == QEvent::Leave)
    {
        if (obj == &m_displayWidget)
            emit unhover(this);
        else if (obj == &m_closeButton)
            m_closeButton.setIcon(QIcon(QPixmap(":/resource/close.png")));
        else if (obj == &m_readButton)
            m_readButton.setIcon(QIcon(QPixmap(":/resource/read.png")));
        else if (obj == &m_delButton)
            m_delButton.setIcon(QIcon(QPixmap(":/resource/del.png")));
    }
    return QObject::eventFilter(obj, event);
}

void NotificationPopup::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButtons mouseButtons = event->buttons();

    if(mouseButtons == Qt::LeftButton)
    {
        MainWindow *mainWindow = MainWindow::instance();

        mainWindow->loadPage("/#messages:" + m_address.replace("#", "%23"));

        if (mainWindow->isWindowClosed())
            mainWindow->openWindow();
    }
}
