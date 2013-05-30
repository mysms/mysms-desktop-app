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

#ifndef NOTIFICATIONPOPUPMANAGER_H
#define NOTIFICATIONPOPUPMANAGER_H

#include <QtCore>
#include <QNetworkReply>
#include <QDateTime>
#include "notificationpopup.h"
#include "usersettings.h"
#include "notificationsummarywidget.h"

class NotificationPopupManager : QObject
{
    Q_OBJECT

public slots:
    void removeFirst(NotificationPopup *widget);
    void setNotificationModeOverview(bool enable = false);
    void downloadFinished(int messageId);

public:
    NotificationPopupManager();
    ~NotificationPopupManager();

    void append(NotificationPopup *widget);    
    void newMessageReceived(const QString &imageUrl, QString headerText, QString messageText, bool isGroupMessage, int messageId, QString address, QDateTime date);
    void clearAllMessages();
    void setBadgeCounter(const int badgeCounter);
    void restoreBadgeCounter();

private:
    void playPopupSound();
    void setWidgetGraphicPos(NotificationPopup* widget, int widgetPos);
    int getSoundNeeded(bool firstPopup, bool isGroupMessage, bool popupMode);
    int getSingleOrGroupIndex(bool isGroupMessage);

private:   
    QImage* icon;
    QNetworkReply *m_networkReply;
    QQueue<NotificationPopup *> * notificationWidgetQueue;

    NotificationSummaryWidget m_notificationSummaryWidget;

    int m_deltaY;
    int m_startX;
    int m_startY;
    int m_width;
    int m_height;
    int m_maxNotificationPopups;
    userSettingsData currentUserSettings;
    int m_storedBadgeCounter;
    bool m_notificationOverview;
};

#endif // NOTIFICATIONPOPUPMANAGER_H
