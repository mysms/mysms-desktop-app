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

#include <QDateTime>

#if defined (USE_KDE)
	#include "kdenotification.h"
#endif

JsInterface::JsInterface(NotificationPopupManager * pnotificationPopupManager,  NetworkCookieJar * pnetworkCookieChar, QObject *parent) : QObject(parent)
{   
    notificationPopupManager = pnotificationPopupManager;
    networkCookieJar = pnetworkCookieChar;
}

JsInterface::~JsInterface()
{
}

void JsInterface::showNotification(const QString & imageUrl, const QString &title, const QString &body, double eventId, int messageId, QString address, double date)
{
    bool isGroupMessage = false;

    if ((address != NULL) && (address.length() > 0) && (address[0] == '#'))
        isGroupMessage = true;

    QDateTime dateVal = QDateTime::fromMSecsSinceEpoch (  date );

#if defined (USE_KDE)	
	KdeNotification *notification = new KdeNotification(imageUrl, title, body);
#else    
    notificationPopupManager->newMessageReceived(imageUrl, title, body, isGroupMessage, messageId, address, dateVal);
#endif
}

void JsInterface::setBadgeCounter(const int badgeCounter)
{
    notificationPopupManager->setBadgeCounter(badgeCounter);
}

void JsInterface::openExternalUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl::fromEncoded(url.toLocal8Bit()));
}

void JsInterface::invalidateOAuthLogin()
{
    networkCookieJar->removeAllCookies();
}

void JsInterface::openSettings()
{
    MainWindow::instance()->settings();
}
