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
  Logger::log_message(QString(__func__));

  if (m_instance == 0)
  {
      m_instance = new MainWindow();      
  }  

  return m_instance;
}

void MainWindow::drop()
{
    Logger::log_message(QString(__func__));

    if (m_instance != 0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

MainWindow::~MainWindow()
{
    Logger::log_message(QString(__func__));

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
    Logger::log_message(QString(__func__));  

    m_updatedialog = NULL;
    m_tabdialog = NULL;
    m_notificationPopupManager = new NotificationPopupManager();
    m_usersettings = userSettings::getInstance();

    if (m_usersettings->areSettingsAvailable())
        m_usersettings->loadSettings();
    else
        m_usersettings->setDefaultData();

    m_tabdialog = new TabDialog;

    m_icon = QIcon(":/resource/icon.png");

    QFontDatabase::addApplicationFont(":/resource/opensans.ttf");
    QFontDatabase::addApplicationFont(":/resource/opensans-bold.ttf");

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

    m_webview.settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);    
    m_webview.settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
    m_webview.settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    m_webview.settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    m_webview.settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    m_webview.setContextMenuPolicy(Qt::NoContextMenu);


    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);
   // QWebSettings::enablePersistentStorage(QApplication::applicationDirPath());
    QWebSettings::enablePersistentStorage();


    m_networkCookieJar = new NetworkCookieJar();

    m_webview.setPage(new WebPage());        
    m_webview.page()->networkAccessManager()->setCookieJar(m_networkCookieJar);
    m_webview.page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);


    QNetworkProxyQuery npq(QUrl("http://www.google.com"));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);   

    if ((listOfProxies.size() > 0) && (listOfProxies[0].type() != QNetworkProxy::NoProxy))
    {
        QNetworkProxy::setApplicationProxy(listOfProxies[0]);        

        Logger::log_message(QString("proxy there"));

    }

    setCentralWidget(&m_webview);

    connect(&m_webview, SIGNAL(linkClicked(QUrl)), SLOT(openExternalUrl(QUrl)));
    connect(m_webview.page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJsObjects()));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));    
    connect(&m_ClickTimer, SIGNAL(timeout()), this, SLOT(singleClick()));

    QNetworkAccessManager *oldManager = m_webview.page()->networkAccessManager();
    QNetworkAccessManagerCustom *newManager = new QNetworkAccessManagerCustom(oldManager, this);
    m_webview.page()->setNetworkAccessManager(newManager);  


#ifdef WEBINSPECTOR
    m_webview.page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    m_inspector = new QWebInspector();
    m_inspector->setPage(m_webview.page());
    m_inspector->setVisible(true);
    m_inspector->show();
#endif

    Logger::log_message(QString("load page"));

    loadPage();
}

void MainWindow::checkVersion()
{
    Logger::log_message(QString(__func__));

    connect(&m_genPurposeNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileReplyFinished(QNetworkReply*)));
    m_genPurposeNetworkManager.get(QNetworkRequest(QUrl(XML_VERSION_FILE_PATH)));
}

void MainWindow::fileReplyFinished(QNetworkReply * networkReply)
{
    Logger::log_message(QString(__func__));

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
    Logger::log_message(QString(__func__));

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
    Logger::log_message(QString(__func__));

    return m_webview.page()->networkAccessManager();
}

QSystemTrayIcon *MainWindow::systemTrayIcon()
{
    Logger::log_message(QString(__func__));

	return m_trayIcon;
}

void MainWindow::changeEvent(QEvent *event)
{
    Logger::log_message(QString(__func__));

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
    Logger::log_message(QString(__func__));

    userSettingsData data = m_usersettings->GetUserSettingsData();

    if(data.commonTabData.showInTaskbarAfterExitSelector)
        showMinimized();
    else
        hide();

    raise();        

    if (event != NULL)
    {
        event->ignore();                       
        //QMainWindow::closeEvent(event);
    }
}

void MainWindow::openWindow()
{
    Logger::log_message(QString(__func__));

    m_notificationPopupManager->clearAllMessages();
    m_notificationPopupManager->setNotificationModeOverview(false);

    if (isHidden() || isMinimized())
    {
        raise();
        show();
        setWindowState(m_savedWindowState | Qt::WindowActive);
    }

    m_trayIcon->hide();           // workaround -> after activation of the main window, the next Trigger event of the system tray doesn´t occur
    m_trayIcon->show();

    activateWindow();
}

bool MainWindow::isWindowClosed()
{
    Logger::log_message(QString(__func__));

    return ((isHidden()) || (isMinimized()) || (!isActiveWindow()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
     Logger::log_message(QString(__func__));

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
    Logger::log_message(QString(__func__));

    Logger::log_message(QString("icon activated"));

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
    Logger::log_message(QString(__func__));

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
     Logger::log_message(QString(__func__));

     m_jsInterface = new JsInterface(m_notificationPopupManager, m_networkCookieJar);

     m_webview.page()->mainFrame()->addToJavaScriptWindowObject(QString(APPLICATION_NAME), m_jsInterface );
 }

void MainWindow::saveSettings()
{
     Logger::log_message(QString(__func__));

     m_settings.setValue(SettingGeometry, geometry());
     m_settings.setValue(SettingWindowState, isMaximized());
     m_settings.sync();
}

void MainWindow::createActions()
{
     Logger::log_message(QString(__func__));

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
     Logger::log_message(QString(__func__));

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
    Logger::log_message(QString(__func__));

    m_tabdialog->show();
    m_tabdialog->activateWindow();
}

void MainWindow::refresh()
{
    Logger::log_message(QString(__func__));
    m_webview.reload();
}

void MainWindow::quit()
{
    Logger::log_message(QString(__func__));

    saveSettings();
    qApp->quit();
}

QString WebPage::userAgentForUrl(const QUrl &url ) const
{
//    Logger::log_message(QString(__func__));

    QString ua = QWebPage::userAgentForUrl(url);    

    if (url.toString().indexOf("analytics") >= 0)
        return ua.replace(QRegExp("^\\S*\\s(\\([^\\)]*\\)).*$"), "mysms/" + QtSingleApplication::applicationVersion() + " \\1" );
    else
        return ua;

    return ua;

}
