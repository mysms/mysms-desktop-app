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

static const QString SettingGeometry =     "geometry";
static const QString SettingWindowState =  "state";
static const QString SettingCloseInfo =    "closeinfo";
static const QString SettingLocale =       "locale";
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

    if (m_updatedialog != NULL)
    {
        delete m_updatedialog;
        m_updatedialog = NULL;
    }

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

void MainWindow::updateBadgeCounter(QIcon icon)
{
    m_icon = icon;

    QWidget::setWindowIcon(icon);
    m_trayIcon->setIcon(icon);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{        
    m_updatedialog = NULL;
    m_tabdialog = NULL;
    m_notificationPopupManager = new NotificationPopupManager();

    UserSettings *userSettings = UserSettings::getInstance();

    if (userSettings->areSettingsAvailable())
        userSettings->loadSettings();
    else
        userSettings->setDefaultData();

    m_tabdialog = new TabDialog;

    m_icon = QIcon(":/resource/icon.png");

    QFontDatabase::addApplicationFont(":/resource/opensans.ttf");
    QFontDatabase::addApplicationFont(":/resource/opensans-bold.ttf");
    QApplication::font().setFamily("Open Sans");

    QWidget::setWindowIcon(m_icon);
    setWindowTitle(tr("mysms"));

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
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);
    QWebSettings::enablePersistentStorage();

    m_webview.settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
#if defined(Q_OS_WIN)
    m_webview.settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, false);
#else
    m_webview.settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
#endif
    m_webview.settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
    m_webview.settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    m_webview.setContextMenuPolicy(Qt::NoContextMenu);

    QNetworkAccessManagerCustom *networkAccessManager = new QNetworkAccessManagerCustom(this);
    m_webview.page()->setNetworkAccessManager(networkAccessManager);
    m_webview.page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
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
    connect(&m_ClickTimer, SIGNAL(timeout()), this, SLOT(singleClick()));

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
    connect(&m_genPurposeNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileReplyFinished(QNetworkReply*)));
    m_genPurposeNetworkManager.get(QNetworkRequest(QUrl(XML_VERSION_FILE_PATH)));
}

void MainWindow::fileReplyFinished(QNetworkReply * networkReply)
{
    QString version;
    QString data=(QString)networkReply->readAll();
    QXmlStreamReader xml(data);

    while(!xml.atEnd())
    {
        xml.readNextStartElement();
        if(xml.name()=="Program_Version" && !xml.isEndElement())
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

        QString currentVersionString = QString::number(VERSION_MAJOR) + "." + QString::number(VERSION_MIDDLE) + "." + QString::number(VERSION_MINOR);

        if (VERSION_MAJOR < majorServerVersion)
            m_updatedialog = new updateDialog();
        else if (VERSION_MAJOR == majorServerVersion)
        {
            if (VERSION_MIDDLE < middleServerVersion)
                m_updatedialog = new updateDialog();
            else if (VERSION_MIDDLE == middleServerVersion)
            {
                if (VERSION_MINOR < minorServerVersion)
                    m_updatedialog = new updateDialog();
            }
        }
    }
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

    if(data.commonTabData.showInTaskbarAfterExitSelector)
        showMinimized();
    else
        hide();

    raise();        

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
        setWindowState(m_savedWindowState | Qt::WindowActive);
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
    m_ClickTimer.stop();

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
    switch (reason)
    {
        case QSystemTrayIcon::DoubleClick:
            m_ClickTimer.stop();
            if (isWindowClosed())
                openWindow();
            break;

        case QSystemTrayIcon::Trigger:
            m_notificationPopupManager->setNotificationModeOverview(false);
            m_ClickTimer.start(POPUP_START_TIME);
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

    /* rfu
     QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+Shift+o"), this);
     connect(shortcut, SIGNAL(activated()), this, SLOT(shortcut_Ctrl_O()));
    */
}

void MainWindow::createTrayIcon()
{
     m_trayIconMenu = new QMenu(this);

     m_trayIconMenu->addAction(m_settingsAction);
     m_trayIconMenu->addAction(m_refreshAction);
     m_trayIconMenu->addAction(m_quitAction);     

     m_trayIcon = new QSystemTrayIcon(this);
     m_trayIcon->setContextMenu(m_trayIconMenu);
     m_trayIcon->setIcon(m_icon);          
     m_trayIcon->show();
}

void MainWindow::settings()
{
    m_tabdialog->show();
    m_tabdialog->activateWindow();
}

void MainWindow::refresh()
{
    m_webview.reload();
}

void MainWindow::quit()
{
    saveSettings();
    qApp->quit();
}

QString WebPage::userAgentForUrl(const QUrl &url ) const
{
    QString ua = QWebPage::userAgentForUrl(url);    

    if (url.toString().indexOf("analytics") >= 0)
        return ua.replace(QRegExp("^\\S*\\s(\\([^\\)]*\\)).*$"), "mysms/" + QtSingleApplication::applicationVersion() + " \\1" );

    return ua;
}
