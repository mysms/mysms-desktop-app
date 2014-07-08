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

#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QDateTime>
#include <QPushButton>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>

#define no_sound (-1)

class NotificationPopup : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationPopup(QPixmap pixmapIcon, QString headerText, QString messageText, bool isGroupMessage, int messageId, QString address, QDateTime date, QWidget *parent = 0);
    ~NotificationPopup();

    void activatePopup();
    void deactivatePopup();
    void activateFadeOut();
    void deactivateFadeOut();
    void setFaceImage(QPixmap pixmapIcon);
    int  getMessageId() const;
    bool isItemMarkedToRemoveFromOverview() const;
    bool isGroupNotification() const;

private:

    void stopTimer();
    void startTimer(int time);

    QTimer m_timeout;

    // notification information
    bool        m_isGroupMessage;
    int         m_messageId;
    QDateTime   m_date;
    QString     m_address;
    bool        m_markedRemoveFromOverview;

    // graphic elements
    QWidget      m_displayWidget;
    QPushButton  m_closeButton;
    QPushButton  m_readButton;
    QPushButton  m_delButton;
    QLabel       m_icon;
    QLabel       m_header;
    QLabel       m_message;
    QLabel       m_dateDisp;
    QGridLayout  m_gridLayout;    
    QGraphicsDropShadowEffect m_shadow;
    QHBoxLayout  m_containerLayout;

    QHBoxLayout  m_ovLayout;

    QVBoxLayout  m_headerButtonDateLayout;
    QHBoxLayout  m_buttonLayout;
    QVBoxLayout  m_messageDateLayout;

signals:
    void deleted(NotificationPopup*);
    void hover(NotificationPopup*);
    void unhover(NotificationPopup*);

public slots:
    void fadeOut();
    void messageDelete();
    void messageRead();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // NOTIFICATIONPOPUP_H
