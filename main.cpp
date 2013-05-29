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

#include "mainwindow.h"
#include "qtsingleapplication.h"
#include "logger.h"
#include "usersettings.h"
#include "globalsettings.h"

#include <QGraphicsView>
#include <QGraphicsWebView>
#include <QTranslator>
#include <QString>
#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[])
{    
    Logger::open_file();
    Logger::log_message(QString(__func__));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    QtSingleApplication::setGraphicsSystem(QLatin1String("raster"));
#endif

    QtSingleApplication::setOrganizationName(ORGANIZATION_NAME);
    QtSingleApplication::setApplicationName(APPLICATION_NAME);
    QtSingleApplication::setApplicationVersion(QString::number(VERSION_MAJOR) + "." + QString::number(VERSION_MIDDLE) + "." + QString::number(VERSION_MINOR));
    QtSingleApplication app(argc, argv);

    if (app.isRunning()) {
        return 0;
    }

    QTranslator translator;
    if (translator.load("mysms", ":/translation"))
        app.installTranslator(&translator);

    MainWindow *w = MainWindow::instance();
    app.setActivationWindow(w);

    if ((argc == 2) && (QString(argv[1]) == QString("min")))
        w->closeWindow(NULL);
    else
        w->show();

    w->checkVersion();

    int res = app.exec();
    w->drop();
    return res;
}
