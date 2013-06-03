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

#include "notificationpopupmanager.h"

#include <QtCore>
#include <QApplication>
#include <QDesktopWidget>
#include <QSound>

#include "mainwindow.h"
#include "globalsettings.h"
#include "soundselector.h"

NotificationPopupManager::NotificationPopupManager() : m_maxNotificationPopups(POPUP_NUMBER)
{
    icon = new QImage(":/resource/icon.png");

    m_downloadFinishedConnected = false;
    m_networkReplyMap = new QHash<QNetworkReply*, int>();
    m_notificationPopupQueue = new QQueue<NotificationPopup*>();

    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect clientRect = desktopWidget->availableGeometry();

    m_width  = 400;
    m_height = 80;

    m_startX = clientRect.left() + clientRect.width() - m_width;
    m_startY = clientRect.top() + clientRect.height() - m_height;

    m_storedBadgeCounter = 0;
    m_notificationOverview = false;
}

NotificationPopupManager::~NotificationPopupManager()
{
    delete icon;

    m_notificationPopupQueue->clear();
    delete m_notificationPopupQueue;
}

void NotificationPopupManager::restoreBadgeCounter()
{
    setBadgeCounter(m_storedBadgeCounter);
}

void NotificationPopupManager::setBadgeCounter(const int badgeCounter)
{
    m_storedBadgeCounter = badgeCounter;

    if (badgeCounter == 0)
    {
        setNotificationModeOverview(false);
        clearAllMessages();
    }

    QIcon *icon;
    if (badgeCounter > 9)
        icon = new QIcon(":/resource/icon-9.png");
    else if (badgeCounter == 0)
        icon = new QIcon(":/resource/icon.png");
    else
        icon = new QIcon(QString(":/resource/icon-%1.png").arg(badgeCounter));

    MainWindow::instance()->updateBadgeCounter(*icon);

    delete icon;
}

void NotificationPopupManager::setWidgetGraphicPos(NotificationPopup* widget, int widgetPos)
{
    int heightOffset = 0;
    int currentNrOfPopups = m_notificationPopupQueue->count();
    int nrOfVisiblePopups = 0;

    if ((currentNrOfPopups > 0) && (widgetPos != 0))
    {
        for(int i = 0; i < currentNrOfPopups; i++)
        {
            if (m_notificationPopupQueue->at(i)->isVisible())
            {
                heightOffset += (m_notificationPopupQueue->at(i)->sizeHint().height() - 10); // overlap popups a bit - they have enough margin
                nrOfVisiblePopups++;

                if (nrOfVisiblePopups == widgetPos)
                    break;
            }
        }
    }

    QDesktopWidget* desktopWidget = QApplication::desktop();
    widget->setGeometry(m_startX, desktopWidget->availableGeometry().height() - widget->sizeHint().height() - heightOffset , m_width, widget->sizeHint().height());
}

void NotificationPopupManager::downloadFinished(QNetworkReply *networkReply)
{
    int messageId = m_networkReplyMap->take(networkReply);

    if (messageId == 0)
        return;

    qDebug() << "download finished for message id";

    for (int i = 0; i < m_notificationPopupQueue->count(); i++)
    {
        if (m_notificationPopupQueue->at(i)->getMessageId() == messageId)
        {

            NotificationPopup *notification = m_notificationPopupQueue->at(i);

            if (networkReply->error() == QNetworkReply::NoError)
            {
                QImage *image = new QImage();
                image->loadFromData(networkReply->readAll());

                QImage imageScale = image->scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                networkReply->close();
                networkReply->deleteLater();

                notification->setFaceImage(QPixmap::fromImage(imageScale));
            }
            break;
        }
    }
}

void NotificationPopupManager::newMessageReceived(const QString &imageUrl, QString headerText, QString messageText, bool isGroupMessage, int messageId, QString address, QDateTime date)
{   
    qDebug() << "new message arrived from" << headerText << "message:" << messageText;

    UserSettingsData userSettingsData = UserSettings::getInstance()->getUserSettingsData();

    int playSoundIndex = getSoundNeeded(((m_notificationPopupQueue->count() == 0) && (m_storedBadgeCounter == 0)) , isGroupMessage, MainWindow::instance()->isWindowClosed());

    if (playSoundIndex != no_sound)
    {
        SoundSelector * soundSelector = SoundSelector::getInstance();

        soundSelector->playSound(playSoundIndex);
    }

    if (MainWindow::instance()->isWindowClosed())       // only show notifications when main window is minimized
    {        
        if (userSettingsData.notificationTabData.displayNotificationsSelector)
        {
            if (userSettingsData.notificationTabData.privacyModeNotificationsSelector)
                messageText = "";

            qDebug() << "show new notification popup";

            if (!imageUrl.isNull())
            {
                if (!m_downloadFinishedConnected) {
                    connect(MainWindow::instance()->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
                    m_downloadFinishedConnected = true;
                }

                QNetworkReply *networkReply = MainWindow::instance()->networkAccessManager()->get(QNetworkRequest(QUrl(imageUrl)));
                m_networkReplyMap->insert(networkReply, messageId);
            }
            append(new NotificationPopup(QPixmap::fromImage(icon->scaled(0, 0, Qt::KeepAspectRatio, Qt::SmoothTransformation)), headerText, messageText, isGroupMessage, messageId, address, date));
        }        
    }
}

int NotificationPopupManager::getSingleOrGroupIndex(bool isGroupMessage)
{

    UserSettingsData userSettingsData = UserSettings::getInstance()->getUserSettingsData();

    int playSoundPopup = no_sound;

    if (isGroupMessage && userSettingsData.soundTabData.groupMessageSelector)
        playSoundPopup = userSettingsData.soundTabData.groupSoundIndex;
    else if (!isGroupMessage  && userSettingsData.soundTabData.singleMessageSelector)
        playSoundPopup = userSettingsData.soundTabData.singleSoundIndex;

    return playSoundPopup;
}

int NotificationPopupManager::getSoundNeeded(bool firstPopup, bool isGroupMessage, bool popupMode)
{

    UserSettingsData userSettingsData = UserSettings::getInstance()->getUserSettingsData();

    int playSoundPopup = no_sound;

    if (popupMode)
    {
        if (userSettingsData.soundTabData.messageSoundOnlyFirst)
        {
            if (firstPopup)
                playSoundPopup = getSingleOrGroupIndex(isGroupMessage);
        }
        else
            playSoundPopup = getSingleOrGroupIndex(isGroupMessage);
    }
    else
    {
        if (userSettingsData.soundTabData.messageSoundEnable)
            playSoundPopup = getSingleOrGroupIndex(isGroupMessage);
    }

    return playSoundPopup;
}

void NotificationPopupManager::clearAllMessages()
{
    while(m_notificationPopupQueue->count() > 0)
    {
        delete m_notificationPopupQueue->first();
        m_notificationPopupQueue->dequeue();
    }
}

void NotificationPopupManager::setNotificationModeOverview(bool enable)
{
    int nrOfItems = m_notificationPopupQueue->count();

    for (int i = 0; i < nrOfItems; i++)
    {
        m_notificationPopupQueue->at(i)->deactivatePopup();
    }

    if (m_notificationPopupQueue->count() == 0)
    {
        m_notificationSummaryWidget.deactivatePopup();
    }

    if (enable)     // show last items
    {
        int idxFirst = nrOfItems - m_maxNotificationPopups;

        if (idxFirst < 0)
            idxFirst = 0;

        for (int i = idxFirst; i < nrOfItems; i++)
        {
            setWidgetGraphicPos(m_notificationPopupQueue->at(i), i - idxFirst);
            m_notificationPopupQueue->at(i)->activatePopup();
        }

        if (m_notificationPopupQueue->count() == 0)
        {
            m_notificationSummaryWidget.setGeometry(m_startX, m_startY, m_width, m_height);
            m_notificationSummaryWidget.activatePopup();
        }
    }
}

void NotificationPopupManager::append(NotificationPopup* widget)
{
    if (m_notificationPopupQueue->count() == 0)
    {
        m_notificationSummaryWidget.deactivatePopup();
    }

    connect(widget, SIGNAL(deleted(NotificationPopup*)), this, SLOT(removeFirst(NotificationPopup*)));    

    int currentNrOfPopups = m_notificationPopupQueue->count();

    int nrOfVisiblePopups = 0;
    int firstIndexOfVisiblePopup = 0;

    for (int i = 0; i < currentNrOfPopups; i++)
    {
        if (m_notificationPopupQueue->at(i)->isVisible())
        {
            if (nrOfVisiblePopups == 0)
                firstIndexOfVisiblePopup = i;
            nrOfVisiblePopups++;
        }
    }

    int widgetPos = 0;

    if (nrOfVisiblePopups < m_maxNotificationPopups)
    {
        widgetPos = nrOfVisiblePopups % m_maxNotificationPopups;
        setWidgetGraphicPos(widget, widgetPos);
        widget->activatePopup();
    }
    else
    {        
        widgetPos = m_maxNotificationPopups - 1;

        m_notificationPopupQueue->at(firstIndexOfVisiblePopup)->fadeOut();
        setWidgetGraphicPos(widget, widgetPos);

        widget->activatePopup();
    }

    m_notificationPopupQueue->enqueue(widget);
}

void NotificationPopupManager::removeFirst(NotificationPopup *widget)
{
    widget->hide();

    int currentNrOfPopups = m_notificationPopupQueue->count();

    int a = 0;

    for (int i = 0; i < currentNrOfPopups; i++)
    {
      if (m_notificationPopupQueue->at(i)->isVisible())
      {
          setWidgetGraphicPos(m_notificationPopupQueue->at(i), a);
          m_notificationPopupQueue->at(i)->repaint();
          a++;
      }
    }

    if (widget->isItemMarkedToRemoveFromOverview())
    {
        m_notificationPopupQueue->removeOne(widget);
        widget->deleteLater();
    }
}
