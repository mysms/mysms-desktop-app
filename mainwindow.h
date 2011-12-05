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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QWebView>
#include <QWebPage>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *instance();
    ~MainWindow();
    QNetworkAccessManager *networkAccessManager();
    QSystemTrayIcon *systemTrayIcon();

protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void openExternalUrl(QUrl url);
    void saveSettings();
    void createActions();
    void createTrayIcon();
    void quit();

private:
    static MainWindow *m_instance;
    explicit MainWindow(QWidget *parent = 0);

    QSettings m_settings;
    QWebView m_webview;
    QIcon m_icon;

    Qt::WindowStates m_savedWindowState;
    QAction *m_quitAction;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

private slots:
    void addJsObjects();

};

class WebPage : public QWebPage {
    Q_OBJECT
protected slots:
    QString userAgentForUrl(const QUrl& url) const;
};

#endif // MAINWINDOW_H
