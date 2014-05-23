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

#include "webview.h"

#include <QAction>
#include <QFileDialog>
#include <QNetworkRequest>
#include <QNetworkReply>

WebView::WebView(QWidget *parent) : QWebView(parent)
{
    QWebPage *page = this->page();
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(downloadRequested(QNetworkRequest)));

    page->action(QWebPage::Reload)->setVisible(false);
    page->action(QWebPage::OpenFrameInNewWindow)->setVisible(false);
    page->action(QWebPage::OpenImageInNewWindow)->setVisible(false);
    page->action(QWebPage::CopyImageUrlToClipboard)->setVisible(false);
}

QWebView * WebView::createWindow(QWebPage::WebWindowType type)
{
    QWebView *webView = new QWebView;
    QWebPage *page = new QWebPage(webView);

    if (type == QWebPage::WebModalDialog)
        webView->setWindowModality(Qt::ApplicationModal);

    webView->setAttribute(Qt::WA_DeleteOnClose, true);

    webView->setPage(page);
    connect(page, SIGNAL(windowCloseRequested()), this, SLOT(closeMyWindow()));
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(downloadRequested(QNetworkRequest)));

    page->action(QWebPage::Reload)->setVisible(false);
    page->action(QWebPage::OpenFrameInNewWindow)->setVisible(false);
    page->action(QWebPage::OpenImageInNewWindow)->setVisible(false);
    page->action(QWebPage::CopyImageUrlToClipboard)->setVisible(false);

    return webView;
}

void WebView::closeMyWindow()
{
    QWebView *webView = ((QWebView*)sender()->parent());
    webView->close();
}

void WebView::downloadRequested(const QNetworkRequest &request)
{

#if QT_VERSION >= 0x050000
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#else
    QString dir =  QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif

    QString defaultFileName = request.url().fileName();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), dir + "/" + defaultFileName);
    if (fileName.isEmpty())
        return;

    QNetworkRequest newRequest = request;
    newRequest.setAttribute(QNetworkRequest::User, fileName);

    QNetworkAccessManager *networkManager = this->page()->networkAccessManager();
    QNetworkReply *reply = networkManager->get(newRequest);
    connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void WebView::downloadFinished()
{
    QNetworkReply *reply = ((QNetworkReply*)sender());
    QNetworkRequest request = reply->request();
    QVariant v = request.attribute(QNetworkRequest::User);
    QString fileName = v.toString();
    QFile file(fileName);
    if (file.open(QFile::ReadWrite)) {
        file.write(reply->readAll());
        file.close();
    }
}
