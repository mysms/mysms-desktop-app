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

#include "kdenotification.h"
#include "mainwindow.h"

#if defined (USE_KDE)
	#include <knotification.h>
#endif


KdeNotification::KdeNotification(const QString &imageUrl, const QString &title, const QString &body, QObject *parent)
    : QObject(parent), m_imageUrl(imageUrl), m_title(title), m_body(body)
{
	if (!imageUrl.isNull()) {
		qDebug() << "load image: " << m_imageUrl;
		m_networkReply = MainWindow::instance()->networkAccessManager()->get(QNetworkRequest(QUrl(m_imageUrl)));
		connect(m_networkReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
	} else {
		show();
	}
}

KdeNotification::~KdeNotification() {
	qDebug() << "destroy";
}

void KdeNotification::downloadFinished() {

	if (m_networkReply->error() != QNetworkReply::NoError) {
	    qDebug() << "Error in" << m_networkReply->url() << ":" << m_networkReply->errorString();
	    return;
	}

	QImage *image = new QImage();
	image->loadFromData(m_networkReply->readAll());

	m_image = image->scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	qDebug() << "image size: " << m_image.size();

	m_networkReply->close();
	disconnect(m_networkReply, SIGNAL(finished()), this, SLOT(downloadFinished()));

	show();
}

void KdeNotification::show() {
#if defined (USE_KDE)
	KNotification *notification = new KNotification("newMessage");
	notification->setText(QString("<b>%1</b>\n%2").arg(m_title).arg(m_body));
	notification->setPixmap(QPixmap::fromImage(m_image));
	notification->sendEvent();
#endif
	delete this;
}
