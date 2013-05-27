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

#include "networkcookiejar.h"
#include "logger.h"

#include <QDateTime>
#include <QStringList>

NetworkCookieJar::NetworkCookieJar() : QNetworkCookieJar()
{
    Logger::log_message(QString("FUNC_COMPI"));
}

bool NetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> & cookieList, const QUrl & url)
{
    Logger::log_message(QString("FUNC_COMPI"));

	QSettings m_settings;
    m_settings.beginGroup("Cookies/" + url.host());

    for (QList<QNetworkCookie>::const_iterator i = cookieList.begin(); i != cookieList.end(); i++)
    {
        if ((*i).isSessionCookie() || (*i).expirationDate() < QDateTime::currentDateTime())
            m_settings.remove((*i).name());
        else
            m_settings.setValue((*i).name(), QString((*i).toRawForm()));
    }

    m_settings.endGroup();
    m_settings.sync();
    return true;
}

void NetworkCookieJar::removeAllCookies()
{
    Logger::log_message(QString("FUNC_COMPI"));

    QSettings m_settings;

    m_settings.beginGroup("Cookies");
    m_settings.remove("");
    m_settings.endGroup();
    m_settings.sync();
}

QList<QNetworkCookie> NetworkCookieJar::cookiesForUrl(const QUrl & url) const
{
    Logger::log_message(QString("FUNC_COMPI"));

    QList<QNetworkCookie> cookieList;

    QSettings m_settings;
    m_settings.beginGroup("Cookies/" + url.host());

    QStringList keys = m_settings.childKeys();

    for (QStringList::iterator i = keys.begin(); i != keys.end(); i++)
    {
    	QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(m_settings.value(*i).toByteArray());

        for (QList<QNetworkCookie>::const_iterator j = cookies.begin(); j != cookies.end(); j++)
        {
            if ((*j).expirationDate() < QDateTime::currentDateTime())
                continue;

    		cookieList.append(*j);
    	}
    }
    m_settings.endGroup();
    return cookieList;
}
