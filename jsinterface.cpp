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

#include "jsinterface.h"
#include "mainwindow.h"
#include "logger.h"

#include <QDateTime>

#if defined (USE_KDE)
	#include "kdenotification.h"
#endif

JsInterface::JsInterface(NotificationPopupManager * pnotificationPopupManager,  NetworkCookieJar * pnetworkCookieChar, QObject *parent) : QObject(parent)
{   
    Logger::log_message(QString(__func__));

    notificationPopupManager = pnotificationPopupManager;
    networkCookieJar = pnetworkCookieChar;
}

JsInterface::~JsInterface()
{
    Logger::log_message(QString(__func__));
}

void JsInterface::showNotification(const QString & imageUrl, const QString &title, const QString &body, double eventId, int messageId, QString address, double date)
{
    Logger::log_message(QString(__func__));

    bool isGroupMessage = false;

    if ((address != NULL) && (address.length() > 0) && (address[0] == '#'))
        isGroupMessage = true;

    QDateTime dateVal = QDateTime::fromMSecsSinceEpoch (  date );

#if defined (USE_KDE)	
	KdeNotification *notification = new KdeNotification(imageUrl, title, body);
#else    
    notificationPopupManager->newMessageReceived(imageUrl, title, body, isGroupMessage, messageId, address, dateVal);
    // MainWindow::instance()->systemTrayIcon()->showMessage(title, body, QSystemTrayIcon::NoIcon, 15000);
#endif
}

void JsInterface::setBadgeCounter(const int badgeCounter)
{
    Logger::log_message(QString(__func__));

    notificationPopupManager->setBadgeCounter(badgeCounter);
}

void JsInterface::openExternalUrl(const QString &url)
{
    Logger::log_message(QString(__func__));

    QDesktopServices::openUrl(QUrl::fromEncoded(url.toLocal8Bit()));
}

void JsInterface::invalidateOAuthLogin()
{
    Logger::log_message(QString(__func__));

    networkCookieJar->removeAllCookies();
}

void JsInterface::openSettings()
{
    Logger::log_message(QString(__func__));

    MainWindow::instance()->settings();
}
