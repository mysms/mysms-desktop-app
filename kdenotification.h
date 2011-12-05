/****************************************************************************
**
** mysms App - Send & receive all your SMS on your Notebook, PC or tablet – like on your smartphone
** Copyright (C) 2011 sms.at mobile internet services gmbh
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

#ifndef KDENOTIFICATION_H_
#define KDENOTIFICATION_H_

#include <QtGui>
#include <QNetworkReply>

class KdeNotification : public QObject
 {
     Q_OBJECT
 public:
     KdeNotification(const QString &imageUrl, const QString &title, const QString &body, QObject *parent=0);
     ~KdeNotification();

 public slots:
     void downloadFinished();
     void show();

 private:
     QString m_imageUrl;
     QString m_title;
     QString m_body;

     QNetworkReply *m_networkReply;
     QImage m_image;
 };

#endif /* KDENOTIFICATION_H_ */
