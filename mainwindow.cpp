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
#include "qnetworkaccessmanagercustom.h"
#include "notificationpopupmanager.h"
#include "updatedialog.h"
#include "globalsettings.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFontDatabase>
#include <QMessageBox>
#include <QWebFrame>
#include <QWindowStateChangeEvent>
#include <QShortcut>
#include <QKeySequence>

#include <QNetworkDiskCache>
#include <QNetworkProxyQuery>
#include <QDesktopServices>
#include <QDesktopWidget>

#if QT_VERSION >= 0x050000
  #include <QStandardPaths>
  #include <QtWidgets>
#endif

static const QString SettingGeometry =     "geometry";
static const QString SettingWindowState =  "state";
static const QString SettingCloseInfo =    "closeinfo";
static const QString SettingLocale =       "locale";
static const QString SettingNotificationDisabledUntil = "notificationDisabled";
static const int versionStringLength = 5;
MainWindow *MainWindow::m_instance = 0;

MainWindow *MainWindow::instance()
{
  if (m_instance == 0)
  {
      m_instance = new MainWindow();      
  }  

  return m_instance;
}

void MainWindow::drop()
{
    if (m_instance != 0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

MainWindow::~MainWindow()
{
    delete m_notificationPopupManager;
    m_notificationPopupManager = NULL;
    delete m_trayIconMenu;
    m_trayIconMenu = NULL;
    delete m_trayIcon;
    m_trayIcon = NULL;
    delete m_quitAction;
    m_quitAction = NULL;
    delete m_refreshAction;
    m_refreshAction = NULL;
    delete m_settingsAction;
    m_settingsAction = NULL;
    delete m_jsInterface;
    m_jsInterface = NULL;

#ifdef WEBINSPECTOR
    delete m_inspector;
    m_inspector = NULL;
#endif
}

void MainWindow::updateBadgeCounter(const int badgeCounter)
{

    QIcon icon;
    if (badgeCounter > 9)
        icon = QIcon(":/resource/icon-9.png");
    else if (badgeCounter == 0)
        icon = m_icon;
    else
        icon = QIcon(QString(":/resource/icon-%1.png").arg(badgeCounter));

    QWidget::setWindowIcon(icon);
    m_trayIcon->setIcon(icon);

#if defined(Q_OS_WIN) && QT_VERSION >= 0x050000
    if (badgeCounter == 0) {
        m_taskBarButton->clearOverlayIcon();
    } else {
        m_taskBarButton->setOverlayIcon(icon);
    }
#endif
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{        
    m_userSettingsDialog = NULL;
    m_notificationPopupManager = new NotificationPopupManager();
    m_lastClickTime = 0;

    UserSettings *userSettings = UserSettings::getInstance();

    if (userSettings->areSettingsAvailable())
        userSettings->loadSettings();
    else
        userSettings->setDefaultData();

    m_userSettingsDialog = new UserSettingsDialog();

    m_icon = QIcon(":/resource/icon.png");

    QFontDatabase::addApplicationFont(":/resource/opensans.ttf");
    QFontDatabase::addApplicationFont(":/resource/opensans-bold.ttf");
    QApplication::font().setFamily("Open Sans");

    QWidget::setWindowIcon(m_icon);
    setWindowTitle(tr("mysms"));

#if defined(Q_OS_WIN) && QT_VERSION >= 0x050000
    m_taskBarButton = new QWinTaskbarButton(this);
    m_taskBarButton->setWindow(this->windowHandle());
#endif

    createActions();
    createTrayIcon();

    bool skipResize = false;

    if (m_settings.contains(SettingWindowState))
    {
        if (m_settings.value(SettingWindowState).toBool())      // maximized
        {
            setGeometry(m_settings.value(SettingGeometry).toRect());
            showMaximized();
            skipResize = true;
        }
    }

    if (!(skipResize))
    {
        if (m_settings.contains(SettingGeometry))
        {
            QDesktopWidget* desktopWidget = QApplication::desktop();
            QRect clientRect = desktopWidget->availableGeometry();
            int nrOfScreens = desktopWidget->numScreens();

            if (nrOfScreens > 1)
            {
               QRect clientRect2 = desktopWidget->availableGeometry(1);
               clientRect.setRight(clientRect2.right());
            }

            QRect storedRect = m_settings.value(SettingGeometry).toRect();

            if (storedRect.right() > clientRect.right())
                showMaximized();
            else
                setGeometry(m_settings.value(SettingGeometry).toRect());
        }
        else
            resize(qMin(QApplication::desktop()->width(), 960), qMin(QApplication::desktop()->height(), 800));
    }

#ifdef CACHE_CLEAR
    QWebSettings::clearMemoryCaches();
    m_webview.settings()->clearMemoryCaches();
#endif

    QWebSettings::globalSettings()->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
#if defined(Q_OS_WIN) && QT_VERSION < 0x050000
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
#else
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);
#endif
    QWebSettings::enablePersistentStorage();

    m_webview.settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
    m_webview.settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);

    QNetworkAccessManagerCustom *networkAccessManager = new QNetworkAccessManagerCustom(this);
    m_webview.page()->setNetworkAccessManager(networkAccessManager);
    m_webview.page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
#if QT_VERSION >= 0x050000
    diskCache->setCacheDirectory(QStandardPaths::standardLocations(QStandardPaths::CacheLocation).at(0));
#else
    diskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
#endif
    networkAccessManager->setCache(diskCache);


    m_networkCookieJar = new NetworkCookieJar();
    networkAccessManager->setCookieJar(m_networkCookieJar);

    QNetworkProxyQuery npq(QUrl("http://www.google.com"));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);   

    if ((listOfProxies.size() > 0) && (listOfProxies[0].type() != QNetworkProxy::NoProxy))
    {
        QNetworkProxy::setApplicationProxy(listOfProxies[0]);        
    }

    setCentralWidget(&m_webview);

    connect(&m_webview, SIGNAL(linkClicked(QUrl)), SLOT(openExternalUrl(QUrl)));
    connect(m_webview.page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJsObjects()));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));    
    connect(&m_clickDelayTimer, SIGNAL(timeout()), this, SLOT(singleClick()));

#ifdef WEBINSPECTOR
    m_webview.page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    m_inspector = new QWebInspector();
    m_inspector->setPage(m_webview.page());
    m_inspector->setVisible(true);
    m_inspector->show();
#endif

    loadPage();
}

void MainWindow::checkVersion()
{
#ifdef UPDATE_SUPPORTED
    QNetworkReply *networkReply = networkAccessManager()->get(QNetworkRequest(QUrl(UPDATE_VERSION_FILE_URL)));
    connect(networkReply, SIGNAL(finished()), this, SLOT(downloadVersionFileFinished()));
#endif
}

void MainWindow::downloadVersionFileFinished()
{
    QNetworkReply *networkReply = (QNetworkReply *)sender();
    bool updateAvailable = false;

    if (networkReply->error() != QNetworkReply::NoError) {
        qDebug() << "error occured on downloading version file" << networkReply->url() << ":" << networkReply->errorString();
    } else {

        QString version;
        QString data = (QString)networkReply->readAll();
        QXmlStreamReader xml(data);

        while(!xml.atEnd())
        {
            xml.readNextStartElement();
            if(xml.name() == "Program_Version" && !xml.isEndElement())
            {
                version = xml.readElementText();
                break;
            }
        }

        if (!version.isEmpty() && (version.length() == versionStringLength) && (version[1] == '.') && (version[3] == '.'))
        {
            int majorServerVersion  = version[0].digitValue();
            int middleServerVersion = version[2].digitValue();
            int minorServerVersion  = version[4].digitValue();

            if (VERSION_MAJOR < majorServerVersion)
                updateAvailable = true;
            else if (VERSION_MAJOR == majorServerVersion)
            {
                if (VERSION_MIDDLE < middleServerVersion)
                    updateAvailable = true;
                else if (VERSION_MIDDLE == middleServerVersion)
                {
                    if (VERSION_MINOR < minorServerVersion)
                        updateAvailable = true;
                }
            }
        }
    }

    disconnect(networkReply, SIGNAL(finished()));
    networkReply->deleteLater();

    if (updateAvailable)
    {
        networkReply = networkAccessManager()->get(QNetworkRequest(QUrl(UPDATE_INSTALLER_URL)));
        connect(networkReply, SIGNAL(finished()), this, SLOT(downloadInstallerFinished()));
    }
}

void MainWindow::downloadInstallerFinished()
{
    QNetworkReply *networkReply = (QNetworkReply *)sender();
    if (networkReply->error() != QNetworkReply::NoError) {
        qDebug() << "error occured on downloading version file" << networkReply->url() << ":" << networkReply->errorString();
    } else {
        QString installerFileName = QDir::tempPath() + "/" + UPDATE_INSTALLER_FILE_NAME;
        QFile localFile(installerFileName);

        if (localFile.open(QIODevice::WriteOnly))
        {
            localFile.write(networkReply->readAll());
            localFile.close();

            UpdateDialog *updateDialog = new UpdateDialog(installerFileName);
            updateDialog->show();
        }
    }

    disconnect(networkReply, SIGNAL(finished()));
    networkReply->deleteLater();
}

void MainWindow::loadPage(QString address)
{
    QUrl url;

    if (m_settings.contains(SettingLocale))
        url = QUrl::fromUserInput(QString(SERVER_URL) + "/?locale=" + m_settings.value(SettingLocale).toString() + address);
    else
        url = QUrl::fromUserInput(QString(SERVER_URL) + address);

    if (m_webview.url() != url)
        m_webview.load(url);
}

QNetworkAccessManager *MainWindow::networkAccessManager()
{
    return m_webview.page()->networkAccessManager();
}

QSystemTrayIcon *MainWindow::systemTrayIcon()
{
    return m_trayIcon;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent *qwsc_event = dynamic_cast<QWindowStateChangeEvent*>(event);

        m_savedWindowState = qwsc_event->oldState();

        if(!(this->isWindowClosed()))
        {
            m_notificationPopupManager->clearAllMessages();
            m_notificationPopupManager->setNotificationModeOverview(false);     
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::closeWindow(QCloseEvent *event)
{
    UserSettingsData data = UserSettings::getInstance()->getUserSettingsData();

    if (data.commonTabData.showInTaskbarAfterExitSelector)
        showMinimized();
    else
        hide();

    if (event != NULL)
    {
        event->ignore();                       
    }
}

void MainWindow::openWindow()
{
    m_notificationPopupManager->clearAllMessages();
    m_notificationPopupManager->setNotificationModeOverview(false);

    if (isHidden() || isMinimized())
    {
        raise();
        show();
        setWindowState(m_savedWindowState);
    }

#ifdef Q_OS_WIN
    m_trayIcon->hide();           // workaround -> after activation of the main window, the next Trigger event of the system tray doesn´t occur
    m_trayIcon->show();
#endif

    activateWindow();
}

bool MainWindow::isWindowClosed()
{
    return ((isHidden()) || (isMinimized()) || (!isActiveWindow()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
     if (m_trayIcon->isVisible())
     {
         if (!m_settings.contains(SettingCloseInfo))
         {
             m_settings.setValue(SettingCloseInfo, true);
             QMessageBox::information(this, tr("Systray"),
                                      tr("The program will keep running in the "
                                         "system tray. To terminate the program, "
                                         "choose <b>Quit</b> in the context menu "
                                         "of the system tray entry."));
         }

         m_savedWindowState = windowState();
         closeWindow(event);
     }
}

void MainWindow::singleClick()
{
    m_clickDelayTimer.stop();

    if (!(isWindowClosed()))
    {
        m_savedWindowState = windowState();
        closeWindow(NULL);
    }
    else
        m_notificationPopupManager->setNotificationModeOverview(true);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    qint64 time;

    switch (reason)
    {
        case QSystemTrayIcon::DoubleClick:
            m_clickDelayTimer.stop();
            if (isWindowClosed())
                openWindow();
#ifdef Q_OS_LINUX
            else
                closeWindow(NULL);
#endif
            break;

        case QSystemTrayIcon::Trigger:
            if (isWindowClosed()) {
                time = QDateTime::currentMSecsSinceEpoch();
                if (time - m_lastClickTime > POPUP_HOVER_FADEOUT_TIME) {
                    m_lastClickTime = time;
                    m_notificationPopupManager->setNotificationModeOverview(false);
                    m_clickDelayTimer.start(POPUP_DELAY_TIME);
                } else {
                    openWindow();
                }
            } else {
                m_notificationPopupManager->setNotificationModeOverview(false);
                m_clickDelayTimer.start(POPUP_DELAY_TIME);
            }
            break;

        case QSystemTrayIcon::Context:
                updateTrayIconMenu();
                break;

        default:
            break;
    }
}

void MainWindow::openExternalUrl(QUrl url)
{
    if (url.toString().startsWith(QString(SERVER_URL)))
    {
		QRegExp rx("locale\\=(\\w\\w)");

        if (rx.indexIn(url.toString(), 0) > 0)
        {
			QString locale = rx.capturedTexts().value(1);
			m_settings.setValue(SettingLocale, locale);
		}

		m_webview.load(url);
    }
    else
		QDesktopServices::openUrl(url);
}

void MainWindow::addJsObjects()
{
     m_jsInterface = new JsInterface(m_notificationPopupManager, m_networkCookieJar);

     m_webview.page()->mainFrame()->addToJavaScriptWindowObject(QString(APPLICATION_NAME), m_jsInterface );
 }

void MainWindow::saveSettings()
{
     m_settings.setValue(SettingGeometry, geometry());
     m_settings.setValue(SettingWindowState, isMaximized());
     m_settings.sync();
}

void MainWindow::createActions()
{
     m_quitAction = new QAction(tr("&Quit"), this);
     connect(m_quitAction, SIGNAL(triggered()), this, SLOT(quit()));

     m_refreshAction = new QAction(tr("&Refresh"), this);
     connect(m_refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));

     m_settingsAction = new QAction(tr("&Settings"), this);
     connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(settings()));

     m_notificationDisable30m = new QAction(tr("hide for 30 minutes"), this);
     connect(m_notificationDisable30m, SIGNAL(triggered()), this, SLOT(disableNotifications()));

     m_notificationDisable1h = new QAction(tr("hide for 1 hour"), this);
     connect(m_notificationDisable1h, SIGNAL(triggered()), this, SLOT(disableNotifications()));

     m_notificationDisable3h = new QAction(tr("hide for 3 hour"), this);
     connect(m_notificationDisable3h, SIGNAL(triggered()), this, SLOT(disableNotifications()));

     m_notificationDisable8h = new QAction(tr("hide for 8 hour"), this);
     connect(m_notificationDisable8h, SIGNAL(triggered()), this, SLOT(disableNotifications()));

     m_notificationDisabled = new QAction(tr("Notifications disabled"), this);
     m_notificationDisabled->setIcon(QIcon(QPixmap(":/resource/notification-disabled.png")));
     connect(m_notificationDisabled, SIGNAL(triggered()), this, SLOT(enableNotifications()));

    /* rfu
     QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+Shift+o"), this);
     connect(shortcut, SIGNAL(activated()), this, SLOT(shortcut_Ctrl_O()));
    */
}

void MainWindow::createTrayIcon()
{
     m_trayIconMenu = new QMenu(this);

     m_notificationMenu = new QMenu(tr("Notifications"), this);
     m_notificationMenu->addAction(m_notificationDisable30m);
     m_notificationMenu->addAction(m_notificationDisable1h);
     m_notificationMenu->addAction(m_notificationDisable3h);
     m_notificationMenu->addAction(m_notificationDisable8h);
     m_trayIconMenu->addMenu(m_notificationMenu);
     m_trayIconMenu->addAction(m_notificationDisabled);
     m_trayIconMenu->addSeparator();

     m_trayIconMenu->addAction(m_settingsAction);
     m_trayIconMenu->addAction(m_refreshAction);
     m_trayIconMenu->addAction(m_quitAction);

     m_trayIcon = new QSystemTrayIcon(this);
     m_trayIcon->setContextMenu(m_trayIconMenu);
     m_trayIcon->setIcon(m_icon);          
     m_trayIcon->show();
}

void MainWindow::updateTrayIconMenu()
{
    if (isNotificationDisabled())
    {
        m_notificationMenu->menuAction()->setVisible(false);
        m_notificationDisabled->setVisible(true);
    } else {
        m_notificationMenu->menuAction()->setVisible(true);
        m_notificationDisabled->setVisible(false);
    }
}

void MainWindow::settings()
{
    m_userSettingsDialog->show();
    m_userSettingsDialog->activateWindow();
}

void MainWindow::refresh()
{
    // workaround for webkit crash if offline application cache is enabled and reload (m_webview.reload()) is done
    QUrl url = m_webview.url();
    m_webview.load(QUrl::fromUserInput("about:blank"));
    m_webview.load(url);
}

void MainWindow::quit()
{
    saveSettings();
    QWebSettings::clearMemoryCaches();
    qApp->quit();
}

bool MainWindow::isNotificationDisabled()
{
    return m_settings.contains(SettingNotificationDisabledUntil)
            && m_settings.value(SettingNotificationDisabledUntil).toLongLong() > QDateTime::currentMSecsSinceEpoch();
}

void MainWindow::disableNotifications()
{
    qint64 timespan = 30 * 60 * 1000;
    if (sender() == m_notificationDisable1h) {
       timespan = 60 * 60 * 1000;
    } else if (sender() == m_notificationDisable3h) {
        timespan = 3 * 60 * 60 * 1000;
    } else if (sender() == m_notificationDisable8h) {
        timespan = 8 * 60 * 60 * 1000;
    }

    qDebug() << "disable notifications for " << timespan << " millis";

    m_settings.setValue(SettingNotificationDisabledUntil, QDateTime::currentMSecsSinceEpoch() + timespan);
    m_settings.sync();
}

void MainWindow::enableNotifications()
{
    m_settings.remove(SettingNotificationDisabledUntil);
    m_settings.sync();
}

QString WebPage::userAgentForUrl(const QUrl &url ) const
{
    QString ua = QWebPage::userAgentForUrl(url);    

    if (url.toString().indexOf("analytics") >= 0)
        return ua.replace(QRegExp("^\\S*\\s(\\([^\\)]*\\)).*$"), "mysms/" + QtSingleApplication::applicationVersion() + " \\1" );

    return ua;
}
