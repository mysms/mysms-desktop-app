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

#include "notificationsummarywidget.h"

#include <QApplication>
#include <QVariant>
#include <QWebFrame>
#include <QString>

#include "mainwindow.h"
#include "soundselector.h"
#include "globalsettings.h"

NotificationSummaryWidget::~NotificationSummaryWidget()
{
    disconnect(&m_timeout, SIGNAL(timeout()), this, SLOT(hide()));
}

NotificationSummaryWidget::NotificationSummaryWidget(QWidget *parent) : QWidget(parent)
{
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
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(hide()));

    m_displayWidget.setGeometry(0, 0, width(), height());
    m_displayWidget.setStyleSheet(".QWidget { background-color: rgba(255, 255, 255, 100%); border-width: 0px; border-style: solid; border-radius: 2px; border-color: #CCCCCC; } .QWidget:hover {  border-width: 2px; border-style: solid; border-radius: 2px; border-color: #2ec1e6; }");


    m_numberOfMessagesText.setText(tr("No new messages"));
    m_numberOfMessagesText.setStyleSheet("QLabel { color: #030303; font-weight: bold; font-size: 12px; }");

    m_closeButton.setIcon(QIcon(QPixmap(":/resource/close.png")));
    m_closeButton.setMaximumSize(20,20);
    m_closeButton.setStyleSheet("QPushButton { border: none; }");
    m_closeButton.setToolTip(tr("Close pop-up"));

    connect(&m_closeButton, SIGNAL(clicked()), this, SLOT(hide()));


    m_innerLayout.addWidget(&m_numberOfMessagesText,1, Qt::AlignCenter );
    m_innerLayout.addWidget(&m_closeButton,0, Qt::AlignTop | Qt::AlignRight);

    m_displayWidget.setLayout(&m_innerLayout);

    m_shadow.setBlurRadius(9.0);
    m_shadow.setColor(QColor(0, 0, 0, 160));
    m_shadow.setOffset(0.0,1.0);

    m_displayWidget.setGraphicsEffect(&m_shadow);
    m_containerLayout.addWidget(&m_displayWidget);
    setLayout(&m_containerLayout);
}

void NotificationSummaryWidget::activatePopup()
{
    show();
    startTimer(POPUP_HOVER_FADEOUT_TIME);
}

void NotificationSummaryWidget::deactivatePopup()
{
    hide();
    stopTimer();
}

void NotificationSummaryWidget::startTimer(int time)
{
    m_timeout.start(time);
}

void NotificationSummaryWidget::stopTimer()
{
    m_timeout.stop();
}

void NotificationSummaryWidget::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButtons mouseButtons = event->buttons();

    if(mouseButtons == Qt::LeftButton)
    {
        MainWindow * mainWindow = MainWindow::instance();       

        if (mainWindow->isWindowClosed())
            mainWindow->openWindow();
    }
}
