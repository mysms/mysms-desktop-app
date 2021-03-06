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

#include "updatedialog.h"
#include "globalsettings.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QDir>

UpdateDialog::UpdateDialog(QString installerFileName, QWidget *parent) : QWidget(parent)
{        

    m_installerFileName = installerFileName;

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    m_updateText.setText(tr("There is a new version of mysms available"));

    QPixmap icon (":/resource/update.png");

    m_updatePic.setPixmap(icon);    

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Install"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Continue"));

    m_mainLayout.setSizeConstraint(QLayout::SetNoConstraint);

    m_contentLayout.addWidget(&m_updateText);
    m_contentLayout.addWidget(buttonBox, 0, Qt::AlignCenter);
    m_mainLayout.addWidget(&m_updatePic, 0, Qt::AlignLeft);
    m_mainLayout.addLayout(&m_contentLayout);

    setLayout(&m_mainLayout);

    this->setMinimumSize(280, 100);
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            sizeHint(),
            QApplication::desktop()->availableGeometry(0)
    ));
    setWindowTitle(tr("Update version"));
    setWindowIcon(QIcon(":/resource/icon.png"));

    connect(buttonBox,  SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox,  SIGNAL(rejected()), this, SLOT(reject()));
}

UpdateDialog::~UpdateDialog()
{
}

void UpdateDialog::accept()
{
    QProcess * myProgress = new QProcess;
    connect(myProgress, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(myProgress, SIGNAL(started()), this, SLOT(processStarted()));

    myProgress->start(m_installerFileName);
    hide();
}

void UpdateDialog::reject()
{
    hide();
}

void UpdateDialog::processError(QProcess::ProcessError err)
{
    switch(err)
    {	case QProcess::FailedToStart:   QMessageBox::information(0,"FailedToStart","FailedToStart"); break;
        case QProcess::Crashed:         QMessageBox::information(0,"Crashed","Crashed");             break;
        case QProcess::Timedout:        QMessageBox::information(0,"FailedToStart","FailedToStart"); break;
        case QProcess::WriteError:		QMessageBox::information(0,"Timedout","Timedout");           break;
        case QProcess::ReadError:		QMessageBox::information(0,"ReadError","ReadError");		 break;
        case QProcess::UnknownError:	QMessageBox::information(0,"UnknownError","UnknownError");   break;
        default:                        break;
    }
}

void UpdateDialog::processStarted()
{
    QApplication::quit();
}
