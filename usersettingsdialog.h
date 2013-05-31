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

#ifndef USERSETTINGSDIALOG_H
 #define USERSETTINGSDIALOG_H

 #include <QCheckBox>

 #include <QDialog>
 #include <QTableWidget>
 #include <QDialogButtonBox>
 #include <QFileInfo>
 #include <QListWidget>
 #include <QComboBox>
 #include <QVBoxLayout>
 #include <QLabel>
 #include <QPushButton>

 class QDialogButtonBox;
 class QFileInfo;
 class QTabWidget;

 class CommonTab : public QWidget
 {
     Q_OBJECT

 public:
     CommonTab(QWidget *parent = 0);

     QCheckBox      m_startExecSelector;
     QCheckBox      m_showInTaskbarAfterExitSelector;

     QVBoxLayout    m_tabLayout;
 };

 class NotificationTab : public QWidget
 {
     Q_OBJECT


private slots:
     void onCheckBoxChange(bool state);

 public:
     NotificationTab( QWidget *parent = 0);

     QCheckBox m_privacyModeNotificationsSelector;
     QCheckBox m_displayNotificationsSelector;

     QVBoxLayout    m_tabLayout;
 };

  /*

 class ShortcutsTab : public QWidget
 {
     Q_OBJECT

 public:
     ShortcutsTab( QWidget *parent = 0);

     QCheckBox *mysmsSendEnableSelector;
     QCheckBox *mysmsSendShiftSelector;
     QCheckBox *mysmsSendCtrlSelector;
     QCheckBox *mysmsSendAltSelector;
     QLineEdit *mysmsSendKeyEdit;

     QCheckBox *mysmsOpenAutoEnableSelector;
     QCheckBox *mysmsOpenAutoShiftSelector;
     QCheckBox *mysmsOpenAutoCtrlSelector;
     QCheckBox *mysmsOpenAutoAltSelector;
     QLineEdit *mysmsOpenAutoKeyEdit;

 };




 class NetworkTab : public QWidget
 {
     Q_OBJECT

 public:
     NetworkTab( QWidget *parent = 0);

     QLineEdit *portNumber;
     QCheckBox *alternativePortsSelector;
     QComboBox *proxyRecognitionCombo;
     QLineEdit *hostEdit;
     QLineEdit *portEdit;
     QCheckBox *proxyAuth;
     QLineEdit *userEdit;
     QLineEdit *passwordEdit;
     QCheckBox *uPnpSelector;
 };

 */

 class SoundTab : public QWidget
 {
     Q_OBJECT

 private slots:
     void playSound();
     void onApplicationsListBoxChanged();
     void onSoundSelectorChanged(int idx);

 public:
     SoundTab( QWidget *parent = 0);
     ~SoundTab();

     QListWidgetItem *  m_singleMessageSelector;
     QListWidgetItem *  m_groupMessageSelector;
     QComboBox          m_soundSelectorCombo;
     QListWidget        m_applicationsListBox;
     QCheckBox          m_messageSoundOnlyFirst;
     QCheckBox          m_messageSoundEnable;


     QLabel             m_soundLabel;
     QLabel             m_soundLabelRight;
     QVBoxLayout        m_leftLayout;
     QHBoxLayout        m_centerLayout;
     QVBoxLayout        m_rightLayout;
     QHBoxLayout        m_upperLayout;
     QVBoxLayout        m_mainLayout;

     QPushButton        m_buttonPlaySound;
     QLabel             m_soundLabelPlay;

     int m_groupCachedComboIndex;
     int m_singleCachedComboIndex;
 };

 class UserSettingsDialog : public QDialog
 {
     Q_OBJECT

 private slots:
     void accept();

 public:
     UserSettingsDialog(QWidget *parent = 0);
     ~UserSettingsDialog();

 private:
     QTabWidget         m_tabWidget;
     QDialogButtonBox  * m_buttonBox;

     CommonTab          m_commonTab;
     NotificationTab    m_notificationTab;
     SoundTab           m_soundTab;
    // NetworkTab         m_networkTab;
    // ShortcutsTab       m_shortCutsTab;

     QVBoxLayout        m_mainLayout;
 };

 #endif // USERSETTINGSDIALOG_H
