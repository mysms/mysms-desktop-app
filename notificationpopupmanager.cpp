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

    notificationWidgetQueue = new QQueue<NotificationPopup*>();

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

    notificationWidgetQueue->clear();
    delete notificationWidgetQueue;    
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
    int currentNrOfPopups = notificationWidgetQueue->count();
    int nrOfVisiblePopups = 0;

    if ((currentNrOfPopups > 0) && (widgetPos != 0))
    {
        for(int i = 0; i < currentNrOfPopups; i++)
        {
            if (notificationWidgetQueue->at(i)->isVisible())
            {
                heightOffset += (notificationWidgetQueue->at(i)->sizeHint().height() - 10); // overlap popups a bit - they have enough margin
                nrOfVisiblePopups++;

                if (nrOfVisiblePopups == widgetPos)
                    break;
            }
        }
    }

    QDesktopWidget* desktopWidget = QApplication::desktop();
    widget->setGeometry(m_startX, desktopWidget->availableGeometry().height() - widget->sizeHint().height() - heightOffset , m_width, widget->sizeHint().height());
}

void NotificationPopupManager::downloadFinished(int messageId)
{
    if (m_networkReply->error() != QNetworkReply::NoError)
    {
        return;
    }

    QImage *image = new QImage();
    image->loadFromData(m_networkReply->readAll());

    QImage imageScale = image->scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_networkReply->close();

    for (int i = 0; i < notificationWidgetQueue->count(); i++)
    {
        if (notificationWidgetQueue->at(i)->getMessageId() == messageId)
        {
            notificationWidgetQueue->at(i)->setFaceImage(QPixmap::fromImage(imageScale));
        }
    }
}

void NotificationPopupManager::newMessageReceived(const QString &imageUrl, QString headerText, QString messageText, bool isGroupMessage, int messageId, QString address, QDateTime date)
{   
    qDebug() << "new message arrived from" << headerText << "message:" << messageText;

    currentUserSettings = userSettings::getInstance()->GetUserSettingsData();

    int playSoundIndex = getSoundNeeded(((notificationWidgetQueue->count() == 0) && (m_storedBadgeCounter == 0)) , isGroupMessage, MainWindow::instance()->isWindowClosed());

    if (playSoundIndex != no_sound)
    {
        SoundSelector * soundSelector = SoundSelector::getInstance();

        soundSelector->playSound(playSoundIndex);
    }

    if (MainWindow::instance()->isWindowClosed())       // only show notifications when main window is minimized
    {        
        if (currentUserSettings.notificationTabData.displayNotificationsSelector)
        {
            if (currentUserSettings.notificationTabData.privacyModeNotificationsSelector)
                messageText = "";

            qDebug() << "show new notification popup";

            if (!imageUrl.isNull())
            {
                QSignalMapper* signalMapper = new QSignalMapper(this);
                m_networkReply = MainWindow::instance()->networkAccessManager()->get(QNetworkRequest(QUrl(imageUrl)));
                connect(m_networkReply, SIGNAL(finished()), signalMapper, SLOT(map()));
                signalMapper->setMapping(m_networkReply, messageId);
                connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(downloadFinished(int)));
            }
            append(new NotificationPopup(QPixmap::fromImage(icon->scaled(0, 0, Qt::KeepAspectRatio, Qt::SmoothTransformation)), headerText, messageText, isGroupMessage, messageId, address, date));
        }        
    }
}

int NotificationPopupManager::getSingleOrGroupIndex(bool isGroupMessage)
{
    int playSoundPopup = no_sound;

    if (isGroupMessage && currentUserSettings.soundTabData.groupMessageSelector)
        playSoundPopup = currentUserSettings.soundTabData.groupSoundIndex;
    else if (!isGroupMessage  && currentUserSettings.soundTabData.singleMessageSelector)
        playSoundPopup = currentUserSettings.soundTabData.singleSoundIndex;

    return playSoundPopup;
}

int NotificationPopupManager::getSoundNeeded(bool firstPopup, bool isGroupMessage, bool popupMode)
{
    int playSoundPopup = no_sound;

    if (popupMode)
    {
        if (currentUserSettings.soundTabData.messageSoundOnlyFirst)
        {
            if (firstPopup)
                playSoundPopup = getSingleOrGroupIndex(isGroupMessage);
        }
        else
            playSoundPopup = getSingleOrGroupIndex(isGroupMessage);
    }
    else
    {
        if (currentUserSettings.soundTabData.messageSoundEnable)
            playSoundPopup = getSingleOrGroupIndex(isGroupMessage);
    }

    return playSoundPopup;
}

void NotificationPopupManager::clearAllMessages()
{
    while(notificationWidgetQueue->count() > 0)
    {
        delete notificationWidgetQueue->first();
        notificationWidgetQueue->dequeue();
    }
}

void NotificationPopupManager::setNotificationModeOverview(bool enable)
{
    int nrOfItems = notificationWidgetQueue->count();

    for (int i = 0; i < nrOfItems; i++)
    {
        notificationWidgetQueue->at(i)->deactivatePopup();
    }

    if (notificationWidgetQueue->count() == 0)
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
            setWidgetGraphicPos(notificationWidgetQueue->at(i), i - idxFirst);
            notificationWidgetQueue->at(i)->activatePopup();
        }

        if (notificationWidgetQueue->count() == 0)
        {
            m_notificationSummaryWidget.setGeometry(m_startX, m_startY, m_width, m_height);
            m_notificationSummaryWidget.activatePopup();
        }
    }
}

void NotificationPopupManager::append(NotificationPopup* widget)
{
    if (notificationWidgetQueue->count() == 0)
    {
        m_notificationSummaryWidget.deactivatePopup();
    }

    connect(widget, SIGNAL(deleted(NotificationPopup*)), this, SLOT(removeFirst(NotificationPopup*)));    

    int currentNrOfPopups = notificationWidgetQueue->count();

    int nrOfVisiblePopups = 0;
    int firstIndexOfVisiblePopup = 0;

    for (int i = 0; i < currentNrOfPopups; i++)
    {
        if (notificationWidgetQueue->at(i)->isVisible())
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

        notificationWidgetQueue->at(firstIndexOfVisiblePopup)->fadeOut();
        setWidgetGraphicPos(widget, widgetPos);

        widget->activatePopup();
    }

    notificationWidgetQueue->enqueue(widget);
}

void NotificationPopupManager::removeFirst(NotificationPopup *widget)
{
    widget->hide();

    int currentNrOfPopups = notificationWidgetQueue->count();

    int a = 0;

    for (int i = 0; i < currentNrOfPopups; i++)
    {
      if (notificationWidgetQueue->at(i)->isVisible())
      {
          setWidgetGraphicPos(notificationWidgetQueue->at(i), a);
          notificationWidgetQueue->at(i)->repaint();
          a++;
      }
    }

    if (widget->isItemMarkedToRemoveFromOverview())
    {
        notificationWidgetQueue->removeOne(widget);
        widget->deleteLater();
    }
}
