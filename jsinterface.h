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

#ifndef JSINTERFACE_H
#define JSINTERFACE_H

#include "notificationpopupmanager.h"
#include "networkcookiejar.h"

#include <QtGui>

class JsInterface : public QObject
 {
     Q_OBJECT
 public:
     JsInterface(NotificationPopupManager * pnotificationPopupManager, NetworkCookieJar * pnetworkCookieChar, QObject *parent=0);
     ~JsInterface();

 private:
     NotificationPopupManager * notificationPopupManager;
     NetworkCookieJar * networkCookieJar;

 public slots:
     void showNotification(const QString & imageUrl, const QString &title, const QString &body, const double eventId = 0, const int messageId = 0, QString address = "", double date = 0);
     void setBadgeCounter(const int badgeCounter);
     void openExternalUrl(const QString &url);    
     void openSettings();
     void invalidateOAuthLogin();
 };
#endif // JSINTERFACE_H
