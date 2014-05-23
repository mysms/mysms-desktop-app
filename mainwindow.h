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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "jsinterface.h"
#include "usersettings.h"
#include "usersettingsdialog.h"
#include "updatedialog.h"
#include "notificationpopup.h"
#include "notificationpopupmanager.h"
#include "webview.h"
#include "networkcookiejar.h"

#include <QtGui>
#include <QWebView>
#include <QUrl>
#include <QWebPage>
#include <QWebInspector>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMainWindow>

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QWidget>
#endif

#include <QtCore>

#if defined(Q_OS_WIN) && QT_VERSION >= 0x050000
  #include <QWinTaskbarButton>
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *instance();
    static void drop();
    QNetworkAccessManager *networkAccessManager();
    QSystemTrayIcon *systemTrayIcon();

    WebView m_webview;

    bool isWindowClosed();
    void openWindow();
    void closeWindow(QCloseEvent *event);
    void loadPage(QString address = "");
    void checkVersion();
    void updateBadgeCounter(const int badgeCounter);

protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);


public slots:
    void settings();

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void openExternalUrl(QUrl url);
    void saveSettings();
    void createActions();
    void createTrayIcon();
    void quit();
    void refresh();    
    void downloadVersionFileFinished();
    void downloadInstallerFinished();
    void singleClick();

private:    
    static MainWindow *m_instance;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSettings m_settings;
    QIcon m_icon;
    Qt::WindowStates m_savedWindowState;

    QTimer m_clickDelayTimer;
    qint64 m_lastClickTime;
    QAction *m_quitAction;
    QAction *m_refreshAction;
    QAction *m_settingsAction;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;
    UserSettingsDialog  *m_userSettingsDialog;

    NotificationPopupManager *m_notificationPopupManager;

    QWebInspector *m_inspector;
    NetworkCookieJar *m_networkCookieJar;
    JsInterface *m_jsInterface;

#if defined(Q_OS_WIN) && QT_VERSION >= 0x050000
    QWinTaskbarButton *m_taskBarButton;
#endif

private slots:
    void addJsObjects();
};

class WebPage : public QWebPage {
    Q_OBJECT
protected slots:
    QString userAgentForUrl(const QUrl& url) const;



};

#endif // MAINWINDOW_H
