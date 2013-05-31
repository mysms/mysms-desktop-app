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

#include "usersettings.h"

#include <QApplication>
#include <QFile>

static const char * const settingsGroup = "SettingsMysms";
static const char * const settingsRegistry = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char * const defaultSettingsWritten = "DefaultSettingsWritten";

UserSettings* UserSettings::m_userSettings = NULL;

UserSettings::UserSettings()
{}

UserSettings* UserSettings::getInstance()
{
    if (m_userSettings == NULL )
    {
        m_userSettings = new UserSettings();
    }

    return m_userSettings;
}

void UserSettingsData::setTabData(UserSettingsData const & passedUserSettingsData)
{
    commonTabData       = passedUserSettingsData.commonTabData;
    networkTabData      = passedUserSettingsData.networkTabData;
    soundTabData        = passedUserSettingsData.soundTabData;
    shortcutsTabData    = passedUserSettingsData.shortcutsTabData;
    notificationTabData = passedUserSettingsData.notificationTabData;
}

void UserSettings::saveSettings(UserSettingsData const & passedUserSettingsData)
{
    m_settings.beginGroup(settingsGroup);

    m_userSettingsData.setTabData(passedUserSettingsData);

    setStartup();
    //settings.setValue("userSettings.commonTabData.startExecSelector", m_userSettingsData.commonTabData.startExecSelector);
    m_settings.setValue("userSettings.commonTabData.showInTaskbarAfterExitSelector", m_userSettingsData.commonTabData.showInTaskbarAfterExitSelector);


    m_settings.setValue("userSettings.notificationTabData.displayNotificationsSelector", m_userSettingsData.notificationTabData.displayNotificationsSelector );
    m_settings.setValue("userSettings.notificationTabData.privacyModeNotificationsSelector", m_userSettingsData.notificationTabData.privacyModeNotificationsSelector );


    m_settings.setValue("userSettings.soundTabData.messageSoundEnable", m_userSettingsData.soundTabData.messageSoundEnable);
    m_settings.setValue("userSettings.soundTabData.messageSoundOnlyFirst", m_userSettingsData.soundTabData.messageSoundOnlyFirst);
    m_settings.setValue("userSettings.soundTabData.groupMessageSelector", m_userSettingsData.soundTabData.groupMessageSelector);
    m_settings.setValue("userSettings.soundTabData.singleMessageSelector", m_userSettingsData.soundTabData.singleMessageSelector);
    m_settings.setValue("userSettings.soundTabData.singleSoundIndex", m_userSettingsData.soundTabData.singleSoundIndex);
    m_settings.setValue("userSettings.soundTabData.groupSoundIndex", m_userSettingsData.soundTabData.groupSoundIndex);


    m_settings.setValue("userSettings.shortcutsTabData.mysmsOpenAutoEnableSelector", m_userSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsOpenAutoShiftSelector", m_userSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsOpenAutoCtrlSelector", m_userSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsOpenAutoAltSelector", m_userSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsOpenAutoKeyEdit", m_userSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit);

    m_settings.setValue("userSettings.shortcutsTabData.mysmsSendEnableSelector", m_userSettingsData.shortcutsTabData.mysmsSendEnableSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsSendShiftSelector", m_userSettingsData.shortcutsTabData.mysmsSendShiftSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsSendCtrlSelector", m_userSettingsData.shortcutsTabData.mysmsSendCtrlSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsSendAltSelector", m_userSettingsData.shortcutsTabData.mysmsSendAltSelector);
    m_settings.setValue("userSettings.shortcutsTabData.mysmsSendKeyEdit", m_userSettingsData.shortcutsTabData.mysmsSendKeyEdit);


    m_settings.setValue("userSettings.networkTabData.portNumber", m_userSettingsData.networkTabData.portNumber);
    m_settings.setValue("userSettings.networkTabData.alternativePortsSelector", m_userSettingsData.networkTabData.alternativePortsSelector);
    m_settings.setValue("userSettings.networkTabData.proxyRecognitionComboIndex", m_userSettingsData.networkTabData.proxyRecognitionComboIndex);
    m_settings.setValue("userSettings.networkTabData.hostEdit", m_userSettingsData.networkTabData.hostEdit);
    m_settings.setValue("userSettings.networkTabData.portEdit", m_userSettingsData.networkTabData.portEdit);
    m_settings.setValue("userSettings.networkTabData.proxyAuth", m_userSettingsData.networkTabData.proxyAuth);
    m_settings.setValue("userSettings.networkTabData.userEdit", m_userSettingsData.networkTabData.userEdit);
    m_settings.setValue("userSettings.networkTabData.passwordEdit", m_userSettingsData.networkTabData.passwordEdit);
    m_settings.setValue("userSettings.networkTabData.uPnpSelector", m_userSettingsData.networkTabData.uPnpSelector);

    m_settings.setValue(defaultSettingsWritten, true);

    m_settings.endGroup();
    m_settings.sync();
}

void UserSettings::setStartup()
{
    bool settingToStore = m_userSettingsData.commonTabData.startExecSelector;

    loadStartup();

#ifdef Q_OS_WIN
    QSettings settingsTmp(settingsRegistry, QSettings::NativeFormat);
    if (m_userSettingsData.commonTabData.startExecSelector != settingToStore)       // val in registry is different
    {
        m_userSettingsData.commonTabData.startExecSelector = settingToStore;

        if (m_userSettingsData.commonTabData.startExecSelector)
        {
            QString pathToExec = QApplication::applicationDirPath();

            pathToExec.replace("/", "\\");
            settingsTmp.setValue("mysms", "\"" + pathToExec + "\\mysms.exe\" min");
        }
        else
        {
            settingsTmp.remove("mysms");
        }
    }
#endif
}

void UserSettings::loadStartup()
{
#ifdef Q_OS_WIN
    QSettings settingsTmp(settingsRegistry, QSettings::NativeFormat);
    QString value = settingsTmp.value("mysms").toString();

    if (value.isEmpty())
        m_userSettingsData.commonTabData.startExecSelector = false;
    else
        m_userSettingsData.commonTabData.startExecSelector = true;
#else
    m_userSettingsData.commonTabData.startExecSelector = false;
#endif
}

void UserSettings::loadSettings()
{
    m_settings.beginGroup(settingsGroup);

    loadStartup();
    m_userSettingsData.commonTabData.showInTaskbarAfterExitSelector = m_settings.value("userSettings.commonTabData.showInTaskbarAfterExitSelector").toBool();

    m_userSettingsData.notificationTabData.displayNotificationsSelector = m_settings.value("userSettings.notificationTabData.displayNotificationsSelector").toBool();
    m_userSettingsData.notificationTabData.privacyModeNotificationsSelector = m_settings.value("userSettings.notificationTabData.privacyModeNotificationsSelector").toBool();

    m_userSettingsData.soundTabData.messageSoundEnable = m_settings.value("userSettings.soundTabData.messageSoundEnable").toBool();
    m_userSettingsData.soundTabData.messageSoundOnlyFirst = m_settings.value("userSettings.soundTabData.messageSoundOnlyFirst").toBool();
    m_userSettingsData.soundTabData.groupMessageSelector = m_settings.value("userSettings.soundTabData.groupMessageSelector").toBool();
    m_userSettingsData.soundTabData.singleMessageSelector = m_settings.value("userSettings.soundTabData.singleMessageSelector").toBool();
    m_userSettingsData.soundTabData.singleSoundIndex = m_settings.value("userSettings.soundTabData.singleSoundIndex").toInt();
    m_userSettingsData.soundTabData.groupSoundIndex = m_settings.value("userSettings.soundTabData.groupSoundIndex").toInt();

    m_userSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector = m_settings.value("userSettings.shortcutsTabData.mysmsOpenAutoEnableSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector = m_settings.value("userSettings.shortcutsTabData.mysmsOpenAutoShiftSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector = m_settings.value("userSettings.shortcutsTabData.mysmsOpenAutoCtrlSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector = m_settings.value("userSettings.shortcutsTabData.mysmsOpenAutoAltSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit = m_settings.value("userSettings.shortcutsTabData.mysmsOpenAutoKeyEdit").toString();

    m_userSettingsData.shortcutsTabData.mysmsSendEnableSelector = m_settings.value("userSettings.shortcutsTabData.mysmsSendEnableSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendShiftSelector = m_settings.value("userSettings.shortcutsTabData.mysmsSendShiftSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendCtrlSelector = m_settings.value("userSettings.shortcutsTabData.mysmsSendCtrlSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendAltSelector = m_settings.value("userSettings.shortcutsTabData.mysmsSendAltSelector").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendKeyEdit = m_settings.value("userSettings.shortcutsTabData.mysmsSendKeyEdit").toString();

    m_userSettingsData.networkTabData.portNumber = m_settings.value("userSettings.networkTabData.portNumber").toInt();
    m_userSettingsData.networkTabData.alternativePortsSelector = m_settings.value("userSettings.networkTabData.alternativePortsSelector").toBool();
    m_userSettingsData.networkTabData.proxyRecognitionComboIndex = m_settings.value("userSettings.networkTabData.proxyRecognitionComboIndex").toInt();
    m_userSettingsData.networkTabData.hostEdit = m_settings.value("userSettings.networkTabData.hostEdit").toString();
    m_userSettingsData.networkTabData.portEdit = m_settings.value("userSettings.networkTabData.portEdit").toString();
    m_userSettingsData.networkTabData.proxyAuth = m_settings.value("userSettings.networkTabData.proxyAuth").toBool();
    m_userSettingsData.networkTabData.userEdit = m_settings.value("userSettings.networkTabData.userEdit").toString();
    m_userSettingsData.networkTabData.passwordEdit = m_settings.value("userSettings.networkTabData.passwordEdit").toString();
    m_userSettingsData.networkTabData.uPnpSelector = m_settings.value("userSettings.networkTabData.uPnpSelector").toBool();

    m_settings.endGroup();
}

UserSettingsData UserSettings::getUserSettingsData() const
{
    return m_userSettingsData;
}

void UserSettings::setDefaultData()
{
    UserSettingsData data;

    removeObsoleteVersionSettings();
    loadStartup();

    data.commonTabData.showInTaskbarAfterExitSelector = false; // after exit show in taskbar

    data.notificationTabData.displayNotificationsSelector = true;      // show notifications
    data.notificationTabData.privacyModeNotificationsSelector = false; // message text

    data.soundTabData.messageSoundEnable = false;             // open app play sound
    data.soundTabData.messageSoundOnlyFirst = false;          // sound only for first unread message
    data.soundTabData.groupMessageSelector = true;            // sound for group enabled
    data.soundTabData.singleMessageSelector = true;           // sound for single message enabled
    data.soundTabData.singleSoundIndex = 0;                   // take first sound
    data.soundTabData.groupSoundIndex = 0;                    // take first sound

    data.shortcutsTabData.mysmsOpenAutoEnableSelector = false;  // shortcuts
    data.shortcutsTabData.mysmsOpenAutoShiftSelector = false;
    data.shortcutsTabData.mysmsOpenAutoCtrlSelector = false;
    data.shortcutsTabData.mysmsOpenAutoAltSelector = false;
    data.shortcutsTabData.mysmsOpenAutoKeyEdit = "";

    data.shortcutsTabData.mysmsSendEnableSelector = false;  // shortcuts
    data.shortcutsTabData.mysmsSendShiftSelector = false;
    data.shortcutsTabData.mysmsSendCtrlSelector = false;
    data.shortcutsTabData.mysmsSendAltSelector = false;
    data.shortcutsTabData.mysmsSendKeyEdit = "";

    data.networkTabData.portNumber = 0;
    data.networkTabData.alternativePortsSelector = false;
    data.networkTabData.proxyRecognitionComboIndex = 0;
    data.networkTabData.hostEdit = "";
    data.networkTabData.portEdit = "";
    data.networkTabData.proxyAuth = false;
    data.networkTabData.userEdit = "";
    data.networkTabData.passwordEdit = "";
    data.networkTabData.uPnpSelector = false;

    saveSettings(data);
}

void UserSettings::removeObsoleteVersionSettings()
{
    QFile::remove("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\mysms App.lnk");
}

bool UserSettings::areSettingsAvailable()
{
     bool ret = false;

     m_settings.beginGroup(settingsGroup);
     ret = m_settings.value(defaultSettingsWritten).toBool();
     m_settings.endGroup();

     return ret;
}
