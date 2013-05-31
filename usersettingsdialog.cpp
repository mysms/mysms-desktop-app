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

#include <QtGui>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>

#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QSound>
#include <QApplication>
#include <QModelIndexList>
#include <QApplication>

#include "usersettings.h"
#include "usersettingsdialog.h"
#include "soundselector.h"

 UserSettingsDialog::UserSettingsDialog( QWidget *parent) : QDialog(parent)
 {
     m_tabWidget.addTab(&m_commonTab, tr("Common Settings"));
     m_tabWidget.addTab(&m_notificationTab, tr("Notifications"));
     m_tabWidget.addTab(&m_soundTab, tr("Sounds"));
//   m_tabWidget.addTab(&m_networkTab, tr("Network"));
//     m_tabWidget.addTab(&m_shortCutsTab, tr("Shortcuts"));    rfu

     m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

     connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

     m_mainLayout.setSizeConstraint(QLayout::SetNoConstraint);
     m_mainLayout.addWidget(&m_tabWidget);
     m_mainLayout.addWidget(m_buttonBox);
     setLayout(&m_mainLayout);

     setWindowTitle(tr("Settings"));
 }

 UserSettingsDialog::~UserSettingsDialog()
 {}

 void UserSettingsDialog::accept()
 {
     UserSettings *settings = UserSettings::getInstance();

     UserSettingsData data;

     data.commonTabData.startExecSelector = m_commonTab.m_startExecSelector.isChecked();
     data.commonTabData.showInTaskbarAfterExitSelector = m_commonTab.m_showInTaskbarAfterExitSelector.isChecked();

     data.notificationTabData.displayNotificationsSelector = m_notificationTab.m_displayNotificationsSelector.isChecked();
     data.notificationTabData.privacyModeNotificationsSelector = m_notificationTab.m_privacyModeNotificationsSelector.isChecked();

     data.soundTabData.messageSoundEnable =  m_soundTab.m_messageSoundEnable.isChecked();
     data.soundTabData.messageSoundOnlyFirst = m_soundTab.m_messageSoundOnlyFirst.isChecked();
     data.soundTabData.groupMessageSelector = (m_soundTab.m_groupMessageSelector->checkState() == Qt::Checked);
     data.soundTabData.singleMessageSelector = (m_soundTab.m_singleMessageSelector->checkState() == Qt::Checked);
     data.soundTabData.singleSoundIndex = m_soundTab.m_singleCachedComboIndex;
     data.soundTabData.groupSoundIndex = m_soundTab.m_groupCachedComboIndex; //mSoundTab->soundSelectorCombo->currentIndex();

     /* rfu
     data.shortcutsTabData.mysmsOpenAutoEnableSelector = m_shortCutsTab.mysmsOpenAutoEnableSelector->isChecked();
     data.shortcutsTabData.mysmsOpenAutoShiftSelector = m_shortCutsTab.mysmsOpenAutoShiftSelector->isChecked();
     data.shortcutsTabData.mysmsOpenAutoCtrlSelector = m_shortCutsTab.mysmsOpenAutoCtrlSelector->isChecked();
     data.shortcutsTabData.mysmsOpenAutoAltSelector = m_shortCutsTab.mysmsOpenAutoAltSelector->isChecked();
     data.shortcutsTabData.mysmsOpenAutoKeyEdit = m_shortCutsTab.mysmsOpenAutoKeyEdit->text();

     data.shortcutsTabData.mysmsSendEnableSelector = m_shortCutsTab.mysmsSendEnableSelector->isChecked();
     data.shortcutsTabData.mysmsSendShiftSelector = m_shortCutsTab.mysmsSendShiftSelector->isChecked();
     data.shortcutsTabData.mysmsSendCtrlSelector = m_shortCutsTab.mysmsSendCtrlSelector->isChecked();
     data.shortcutsTabData.mysmsSendAltSelector = m_shortCutsTab.mysmsSendAltSelector->isChecked();
     data.shortcutsTabData.mysmsSendKeyEdit = m_shortCutsTab.mysmsSendKeyEdit->text();


     data.networkTabData.portNumber = m_networkTab.portNumber->text().toInt();
     data.networkTabData.alternativePortsSelector = m_networkTab.alternativePortsSelector->isChecked();
     data.networkTabData.proxyRecognitionComboIndex = m_networkTab.proxyRecognitionCombo->currentIndex();
     data.networkTabData.hostEdit = m_networkTab.hostEdit->text();
     data.networkTabData.portEdit = m_networkTab.portEdit->text();
     data.networkTabData.proxyAuth = m_networkTab.proxyAuth->isChecked();
     data.networkTabData.userEdit = m_networkTab.userEdit->text();
     data.networkTabData.passwordEdit = m_networkTab.passwordEdit->text();
     data.networkTabData.uPnpSelector = m_networkTab.uPnpSelector->isChecked();
     */

     settings->saveSettings(data);

     QApplication::setQuitOnLastWindowClosed(false);
     close();
 }

 CommonTab::CommonTab( QWidget *parent): QWidget(parent)
 {
     UserSettingsData settingsData = UserSettings::getInstance()->getUserSettingsData();

     m_startExecSelector.setText(tr("Run mysms on Windows startup"));
     m_startExecSelector.setChecked(settingsData.commonTabData.startExecSelector);

     m_showInTaskbarAfterExitSelector.setText(tr("Show mysms in taskbar"));
     m_showInTaskbarAfterExitSelector.setChecked(settingsData.commonTabData.showInTaskbarAfterExitSelector);

     m_tabLayout.addWidget(&m_startExecSelector);
     m_tabLayout.addWidget(&m_showInTaskbarAfterExitSelector);
     m_tabLayout.addStretch();

     setLayout(&m_tabLayout);
 }

 NotificationTab::NotificationTab( QWidget *parent) : QWidget(parent)
 {
     UserSettingsData settingsData = UserSettings::getInstance()->getUserSettingsData();

     m_displayNotificationsSelector.setText(tr("Show notifications"));
     m_privacyModeNotificationsSelector.setText(tr("Privacy Mode - message text is not displayed"));

     m_displayNotificationsSelector.setChecked(settingsData.notificationTabData.displayNotificationsSelector);
     m_privacyModeNotificationsSelector.setChecked(settingsData.notificationTabData.privacyModeNotificationsSelector);
     m_privacyModeNotificationsSelector.setEnabled(m_displayNotificationsSelector.isChecked());

     connect(&m_displayNotificationsSelector, SIGNAL( toggled(bool) ), this, SLOT(onCheckBoxChange(bool)));

     m_tabLayout.addWidget(&m_displayNotificationsSelector);
     m_tabLayout.addWidget(&m_privacyModeNotificationsSelector);
     m_tabLayout.addStretch();

     setLayout(&m_tabLayout);
 }

 void NotificationTab::onCheckBoxChange(bool state)
 {
     m_privacyModeNotificationsSelector.setEnabled(state);
 }

 SoundTab::~SoundTab()
 {     
 }

 SoundTab::SoundTab( QWidget *parent): QWidget(parent)
 {
     UserSettingsData settingsData = UserSettings::getInstance()->getUserSettingsData();

     m_soundLabel.setText(tr("Play sound, if ..."));

     m_singleMessageSelector = new QListWidgetItem(tr("Single message"), &m_applicationsListBox);
     m_groupMessageSelector  = new QListWidgetItem(tr("Group message"), &m_applicationsListBox);

     if (settingsData.soundTabData.singleMessageSelector)
        m_singleMessageSelector->setCheckState( Qt::Checked );
     else
        m_singleMessageSelector->setCheckState( Qt::Unchecked );

     if (settingsData.soundTabData.groupMessageSelector)
        m_groupMessageSelector->setCheckState( Qt::Checked );
     else
        m_groupMessageSelector->setCheckState( Qt::Unchecked );

     connect(&m_applicationsListBox,  SIGNAL( itemSelectionChanged() ), this, SLOT(onApplicationsListBoxChanged()));
     connect(&m_applicationsListBox,  SIGNAL( itemClicked(QListWidgetItem*)), this, SLOT(onApplicationsListBoxChanged()));

     m_soundLabelRight.setText(tr("Select sound to play"));

     SoundSelector * soundSelector = SoundSelector::getInstance();

     for (int i = 0; i < PLAYLISTSIZE; i++)
     {
         m_soundSelectorCombo.addItem(soundSelector->getSoundTitle(i));
     }

     m_singleCachedComboIndex =  settingsData.soundTabData.singleSoundIndex;
     m_groupCachedComboIndex = settingsData.soundTabData.groupSoundIndex;

     connect(&m_soundSelectorCombo, SIGNAL (currentIndexChanged(int)), this, SLOT (onSoundSelectorChanged(int)));

     m_applicationsListBox.item(0)->setSelected(true);

     m_leftLayout.addWidget(&m_soundLabel);
     m_leftLayout.addWidget(&m_applicationsListBox);
     m_leftLayout.addStretch();

     m_buttonPlaySound.setIcon(QIcon(QPixmap(":/resource/play.png")));
     m_buttonPlaySound.setStyleSheet("QPushButton { border: none; }");
     connect(&m_buttonPlaySound, SIGNAL(clicked()), this, SLOT(playSound()));

     m_soundLabelPlay.setText(tr("Play this sound"));

     m_centerLayout.addWidget(&m_buttonPlaySound);
     m_centerLayout.addWidget(&m_soundLabelPlay);
     m_centerLayout.setAlignment(Qt::AlignJustify);

     m_messageSoundOnlyFirst.setText(tr("Play sound only for first unread message"));
     m_messageSoundEnable.setText(tr("Ring if application is open"));

     m_messageSoundOnlyFirst.setChecked(settingsData.soundTabData.messageSoundOnlyFirst);
     m_messageSoundEnable.setChecked(settingsData.soundTabData.messageSoundEnable);

     m_rightLayout.addWidget(&m_soundLabelRight);
     m_rightLayout.addWidget(&m_soundSelectorCombo);
     m_rightLayout.addLayout(&m_centerLayout);
     m_rightLayout.addStretch();

     m_upperLayout.addLayout(&m_leftLayout);
     m_upperLayout.addLayout(&m_rightLayout);
     m_upperLayout.addStretch();

     m_mainLayout.addLayout(&m_upperLayout);
     m_mainLayout.addWidget(&m_messageSoundOnlyFirst);
     m_mainLayout.addWidget(&m_messageSoundEnable);
     m_mainLayout.addStretch();

     setLayout(&m_mainLayout);
 }

 void SoundTab::onSoundSelectorChanged(int idx)
 {
     QList<QListWidgetItem *>  widgetItems = m_applicationsListBox.selectedItems();

     if (widgetItems.count() == 1)
     {
         if (widgetItems.at(0)->text() == tr("Single message"))            // Singlemessage
         {
            m_singleCachedComboIndex = idx;
         }
         else
         {
            m_groupCachedComboIndex = idx;
         }
     }
 }

 void SoundTab::onApplicationsListBoxChanged()
 {
     QList<QListWidgetItem *>  widgetItems = m_applicationsListBox.selectedItems();

     if (widgetItems.count() == 1)
     {
         if (widgetItems.at(0)->text() == tr("Single message"))            // Singlemessage
         {
             if (m_soundSelectorCombo.count() > m_singleCachedComboIndex)
             {
                m_soundSelectorCombo.setCurrentIndex(m_singleCachedComboIndex);
             }
         }
         else
         {
             if (m_soundSelectorCombo.count() > m_groupCachedComboIndex)
             {
                m_soundSelectorCombo.setCurrentIndex(m_groupCachedComboIndex);
             }
         }
     }
 }

 void SoundTab::playSound()
 {
     int idx = m_soundSelectorCombo.currentIndex();

     SoundSelector * soundSelector = SoundSelector::getInstance();
     soundSelector->playSound(idx);
 }

/* rfu

 NetworkTab::NetworkTab( QWidget *parent): QWidget(parent)
 {
     userSettingsData settingsData = userSettings::getInstance()->GetUserSettingsData();

     QHBoxLayout *  firstLineLayout = new QHBoxLayout;

     QLabel * labelPortLeft = new QLabel(tr("Use port"));
     portNumber = new QLineEdit();
     QLabel * labelPortRight = new QLabel(tr("for incoming connections"));

     portNumber->setText(QString::number(settingsData.networkTabData.portNumber));

     firstLineLayout->addWidget(labelPortLeft);
     firstLineLayout->addWidget(portNumber);
     firstLineLayout->addWidget(labelPortRight);
     firstLineLayout->addStretch();

     alternativePortsSelector = new QCheckBox(tr("Use ports 80 and 443 als alternative for incoming connections"));

     alternativePortsSelector->setChecked(settingsData.networkTabData.alternativePortsSelector);

     proxyRecognitionCombo = new QComboBox();
     proxyRecognitionCombo->addItem(tr("Auto proxy detection"));

     if (proxyRecognitionCombo->count() > settingsData.networkTabData.proxyRecognitionComboIndex)
         proxyRecognitionCombo->setCurrentIndex(settingsData.networkTabData.proxyRecognitionComboIndex);

     QHBoxLayout *  secondLineLayout = new QHBoxLayout;

     QLabel * labelHost = new QLabel(tr("Host:"));
     hostEdit = new QLineEdit();
     QLabel * labelPort = new QLabel(tr("Port:"));
     portEdit = new QLineEdit();

     hostEdit->setText(settingsData.networkTabData.hostEdit);
     portEdit->setText(settingsData.networkTabData.portEdit);

     secondLineLayout->addWidget(labelHost);
     secondLineLayout->addWidget(hostEdit);
     secondLineLayout->addWidget(labelPort);
     secondLineLayout->addWidget(portEdit);
     secondLineLayout->addStretch();

     proxyAuth = new QCheckBox(tr("Allow proxy authentication"));

     proxyAuth->setChecked(settingsData.networkTabData.proxyAuth);

     QHBoxLayout *  thirdLineLayout = new QHBoxLayout;

     QLabel * labelUser = new QLabel(tr("User:"));
     userEdit = new QLineEdit();
     QLabel * labelPassword = new QLabel(tr("Password:"));
     passwordEdit = new QLineEdit();

     userEdit->setText(settingsData.networkTabData.userEdit);
     passwordEdit->setText(settingsData.networkTabData.passwordEdit);

     thirdLineLayout->addWidget(labelUser);
     thirdLineLayout->addWidget(userEdit);
     thirdLineLayout->addWidget(labelPassword);
     thirdLineLayout->addWidget(passwordEdit);
     thirdLineLayout->addStretch();

     uPnpSelector = new QCheckBox(tr("Use uPnP"));

     uPnpSelector->setChecked(settingsData.networkTabData.uPnpSelector);

     QVBoxLayout *  ownerLayout = new QVBoxLayout;

     ownerLayout->addLayout(firstLineLayout);
     ownerLayout->addWidget(alternativePortsSelector);
     ownerLayout->addWidget(proxyRecognitionCombo);
     ownerLayout->addLayout(secondLineLayout);
     ownerLayout->addWidget(proxyAuth);
     ownerLayout->addLayout(thirdLineLayout);
     ownerLayout->addWidget(uPnpSelector);
     ownerLayout->addStretch();

     setLayout(ownerLayout);
 }


 ShortcutsTab::ShortcutsTab( QWidget *parent): QWidget(parent)
 {
     userSettingsData settingsData = userSettings::getInstance()->GetUserSettingsData();

     QLabel * systemInfo = new QLabel(tr("Hint: Shortcuts are valid for the whole system."));

     mysmsOpenAutoEnableSelector = new QCheckBox(tr("Open mysms"));                    
     mysmsOpenAutoShiftSelector = new QCheckBox(tr("Shift"));
     mysmsOpenAutoCtrlSelector = new QCheckBox(tr("Ctrl"));
     mysmsOpenAutoAltSelector = new QCheckBox(tr("Alt"));
     mysmsOpenAutoKeyEdit = new QLineEdit();

     mysmsSendEnableSelector = new QCheckBox(tr("Send a new message"));
     mysmsSendShiftSelector = new QCheckBox(tr("Shift"));
     mysmsSendCtrlSelector = new QCheckBox(tr("Ctrl"));
     mysmsSendAltSelector = new QCheckBox(tr("Alt"));
     mysmsSendKeyEdit = new QLineEdit();

     mysmsOpenAutoEnableSelector->setChecked(settingsData.shortcutsTabData.mysmsOpenAutoEnableSelector);
     mysmsOpenAutoShiftSelector->setChecked(settingsData.shortcutsTabData.mysmsOpenAutoShiftSelector);
     mysmsOpenAutoCtrlSelector->setChecked(settingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector);
     mysmsOpenAutoAltSelector->setChecked(settingsData.shortcutsTabData.mysmsOpenAutoAltSelector);
     mysmsOpenAutoKeyEdit->setText(settingsData.shortcutsTabData.mysmsOpenAutoKeyEdit);

     mysmsSendEnableSelector->setChecked(settingsData.shortcutsTabData.mysmsSendEnableSelector);
     mysmsSendShiftSelector->setChecked(settingsData.shortcutsTabData.mysmsSendShiftSelector);
     mysmsSendCtrlSelector->setChecked(settingsData.shortcutsTabData.mysmsSendCtrlSelector);
     mysmsSendAltSelector->setChecked(settingsData.shortcutsTabData.mysmsSendAltSelector);
     mysmsSendKeyEdit->setText(settingsData.shortcutsTabData.mysmsSendKeyEdit);

     QVBoxLayout *  firstRowLayout = new QVBoxLayout;
     firstRowLayout->addWidget(mysmsOpenAutoEnableSelector);
     firstRowLayout->addWidget(mysmsSendEnableSelector);

     QVBoxLayout *  secondRowLayout = new QVBoxLayout;
     secondRowLayout->addWidget(mysmsOpenAutoShiftSelector);
     secondRowLayout->addWidget(mysmsSendShiftSelector);

     QVBoxLayout *  thirdRowLayout = new QVBoxLayout;
     thirdRowLayout->addWidget(mysmsOpenAutoCtrlSelector);
     thirdRowLayout->addWidget(mysmsSendCtrlSelector);

     QVBoxLayout *  fourthRowLayout = new QVBoxLayout;
     fourthRowLayout->addWidget(mysmsOpenAutoAltSelector);
     fourthRowLayout->addWidget(mysmsSendAltSelector);

     QVBoxLayout *  fifthRowLayout = new QVBoxLayout;
     fifthRowLayout->addWidget(mysmsOpenAutoKeyEdit);
     fifthRowLayout->addWidget(mysmsSendKeyEdit);

     QHBoxLayout *  col = new QHBoxLayout;
     col->addLayout(firstRowLayout);
     col->addLayout(secondRowLayout);
     col->addLayout(thirdRowLayout);
     col->addLayout(fourthRowLayout);
     col->addLayout(fifthRowLayout);

     QVBoxLayout *  ownerLayout = new QVBoxLayout;
     ownerLayout->addLayout(col);
     ownerLayout->addWidget(systemInfo);     
     ownerLayout->addStretch();
     setLayout(ownerLayout);
 }
*/
