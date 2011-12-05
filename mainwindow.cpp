#include "mainwindow.h"
#include "networkcookiejar.h"
#include "jsinterface.h"
#include "qtsingleapplication.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QMessageBox>
#include <QWebFrame>
#include <QWindowStateChangeEvent>

const QString SettingGeometry = "geometry";
const QString SettingCloseInfo = "closeinfo";
const QString SettingLocale = "locale";
const QString Url = "https://app.mysms.com";

MainWindow *MainWindow::m_instance = 0;

MainWindow *MainWindow::instance() {
  if (m_instance == 0) m_instance = new MainWindow();
  return m_instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    m_icon = QIcon(":/resource/icon.png");

    QWidget::setWindowIcon(m_icon);
    setWindowTitle("mysms App");

    createActions();
    createTrayIcon();

    if (m_settings.contains(SettingGeometry)) {
        setGeometry(m_settings.value(SettingGeometry).toRect());
    } else {
        resize(qMin(QApplication::desktop()->width(), 960), qMin(QApplication::desktop()->height(), 800));
    }

    m_webview.settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
    m_webview.settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    m_webview.settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::enablePersistentStorage();

    m_webview.setPage(new WebPage());
    m_webview.page()->networkAccessManager()->setCookieJar(new NetworkCookieJar());
    m_webview.page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);

    m_webview.setContextMenuPolicy(Qt::NoContextMenu);

    setCentralWidget(&m_webview);

    connect(&m_webview, SIGNAL(linkClicked(QUrl)), SLOT(openExternalUrl(QUrl)));
    connect(m_webview.page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJsObjects()));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    if (m_settings.contains(SettingLocale)) {
    	m_webview.load(QUrl(Url + "/?locale=" + m_settings.value(SettingLocale).toString()));
    } else {
    	m_webview.load(QUrl(Url));
    }
}

MainWindow::~MainWindow() {}

QNetworkAccessManager *MainWindow::networkAccessManager() {
	return m_webview.page()->networkAccessManager();
}

QSystemTrayIcon *MainWindow::systemTrayIcon() {
	return m_trayIcon;
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *qwsc_event = dynamic_cast<QWindowStateChangeEvent*>(event);
        m_savedWindowState = qwsc_event->oldState();
        if (isMinimized()) {
            QTimer::singleShot(0, this, SLOT(hide()));
            event->ignore();
        } else {
            event->accept();
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
     if (m_trayIcon->isVisible()) {
         if (!m_settings.contains(SettingCloseInfo)) {
             m_settings.setValue(SettingCloseInfo, true);
             QMessageBox::information(this, tr("Systray"),
                                      tr("The program will keep running in the "
                                         "system tray. To terminate the program, "
                                         "choose <b>Quit</b> in the context menu "
                                         "of the system tray entry."));
         }
         m_savedWindowState = windowState();
         hide();
         event->ignore();
     }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            if (isHidden()) {
                show();
                raise();
                setWindowState(m_savedWindowState | Qt::WindowActive);
            } else {
                m_savedWindowState = windowState();
                hide();
            }
            break;
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::MiddleClick:
            break;
        default:
            break;
    }
}


void MainWindow::openExternalUrl(QUrl url) {
	if (url.toString().startsWith(Url)) {
		QRegExp rx("locale\\=(\\w\\w)");
		if (rx.indexIn(url.toString(), 0) > 0) {
			QString locale = rx.capturedTexts().value(1);
			m_settings.setValue(SettingLocale, locale);
		}
		m_webview.load(url);
	} else {
		QDesktopServices::openUrl(url);
	}
}

void MainWindow::addJsObjects() {
     m_webview.page()->mainFrame()->addToJavaScriptWindowObject(QString("mysms"), new JsInterface());
 }

void MainWindow::saveSettings() {
     m_settings.setValue(SettingGeometry, geometry());
     m_settings.sync();
}

void MainWindow::createActions() {
     m_quitAction = new QAction(tr("&Quit"), this);
     connect(m_quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::createTrayIcon() {
     m_trayIconMenu = new QMenu(this);
     m_trayIconMenu->addAction(m_quitAction);

     m_trayIcon = new QSystemTrayIcon(this);
     m_trayIcon->setContextMenu(m_trayIconMenu);
     m_trayIcon->setIcon(m_icon);
     m_trayIcon->show();
}

void MainWindow::quit() {
	saveSettings();
	qApp->quit();
}

QString WebPage::userAgentForUrl(const QUrl &url ) const {
    QString ua = QWebPage::userAgentForUrl(url);
    if (url.toString().indexOf("analytics") >= 0) {
        return ua.replace(QRegExp("^\\S*\\s(\\([^\\)]*\\)).*$"), "mysms/" + QtSingleApplication::applicationVersion() + " \\1" );
    } else {
        return ua;
    }
}
