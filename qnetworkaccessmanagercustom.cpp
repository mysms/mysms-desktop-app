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

#include "qnetworkaccessmanagercustom.h"
#include "networkreplyfilter.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QMessageBox>
#include <QApplication>

#include "globalsettings.h"

QNetworkAccessManagerCustom::QNetworkAccessManagerCustom(QObject *parent) : QNetworkAccessManager(parent)
{}

QNetworkReply * QNetworkAccessManagerCustom::createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
    if (op == QNetworkAccessManager::GetOperation)
    {
        QString path = req.url().toString();

        // qtwebkit doesn't support font-face correctly - so return empty request
        if (path.contains(QString("fonts.googleapis.com")))
        {
            QNetworkRequest request(req);
            request.setUrl(QUrl("data:text/css,"));

            return QNetworkAccessManager::createRequest (op, request, outgoingData);
        }

        if (path.endsWith(QString(".nocache.manifest"))) {
            QNetworkReply *reply = QNetworkAccessManager::createRequest (op, req, outgoingData);
            return new NetworkReplyFilter(reply, this);
        }
    }

    QNetworkRequest request = req;
    if (request.attribute(QNetworkRequest::CacheLoadControlAttribute) == QNetworkRequest::PreferNetwork &&
            request.url().toString() != UPDATE_INSTALLER_URL)
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    return QNetworkAccessManager::createRequest (op, request, outgoingData);
}
