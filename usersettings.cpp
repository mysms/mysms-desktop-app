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

static const char * const settingsGroup = "Settings";
static const char * const settingsRegistry = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char * const settingsInitialized = "initialized";

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

    setAutorunEnabled(m_userSettingsData.commonTabData.startExecSelector);

    m_settings.setValue("showInTaskbarAfterExit", m_userSettingsData.commonTabData.showInTaskbarAfterExitSelector);


    m_settings.setValue("notification.displayNotifications", m_userSettingsData.notificationTabData.displayNotificationsSelector );
    m_settings.setValue("notification.privacyModeNotifications", m_userSettingsData.notificationTabData.privacyModeNotificationsSelector );


    m_settings.setValue("sound.messageSoundEnable", m_userSettingsData.soundTabData.messageSoundEnable);
    m_settings.setValue("sound.messageSoundOnlyFirst", m_userSettingsData.soundTabData.messageSoundOnlyFirst);
    m_settings.setValue("sound.groupMessage", m_userSettingsData.soundTabData.groupMessageSelector);
    m_settings.setValue("sound.singleMessage", m_userSettingsData.soundTabData.singleMessageSelector);
    m_settings.setValue("sound.singleSoundIndex", m_userSettingsData.soundTabData.singleSoundIndex);
    m_settings.setValue("sound.groupSoundIndex", m_userSettingsData.soundTabData.groupSoundIndex);


    m_settings.setValue("shortcuts.openAutoEnable", m_userSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector);
    m_settings.setValue("shortcuts.openAutoShift", m_userSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector);
    m_settings.setValue("shortcuts.openAutoCtrl", m_userSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector);
    m_settings.setValue("shortcuts.openAutoAlt", m_userSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector);
    m_settings.setValue("shortcuts.openAutoKey", m_userSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit);

    m_settings.setValue("shortcuts.mysmsSendEnable", m_userSettingsData.shortcutsTabData.mysmsSendEnableSelector);
    m_settings.setValue("shortcuts.mysmsSendShift", m_userSettingsData.shortcutsTabData.mysmsSendShiftSelector);
    m_settings.setValue("shortcuts.mysmsSendCtrl", m_userSettingsData.shortcutsTabData.mysmsSendCtrlSelector);
    m_settings.setValue("shortcuts.mysmsSendAlt", m_userSettingsData.shortcutsTabData.mysmsSendAltSelector);
    m_settings.setValue("shortcuts.sendKey", m_userSettingsData.shortcutsTabData.mysmsSendKeyEdit);


    m_settings.setValue("network.portNumber", m_userSettingsData.networkTabData.portNumber);
    m_settings.setValue("network.alternativePorts", m_userSettingsData.networkTabData.alternativePortsSelector);
    m_settings.setValue("network.proxyRecognition", m_userSettingsData.networkTabData.proxyRecognitionComboIndex);
    m_settings.setValue("network.host", m_userSettingsData.networkTabData.hostEdit);
    m_settings.setValue("network.port", m_userSettingsData.networkTabData.portEdit);
    m_settings.setValue("network.proxyAuth", m_userSettingsData.networkTabData.proxyAuth);
    m_settings.setValue("network.user", m_userSettingsData.networkTabData.userEdit);
    m_settings.setValue("network.password", m_userSettingsData.networkTabData.passwordEdit);
    m_settings.setValue("network.uPnp", m_userSettingsData.networkTabData.uPnpSelector);

    m_settings.setValue(settingsInitialized, true);

    m_settings.endGroup();
    m_settings.sync();
}

void UserSettings::setAutorunEnabled(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings settingsTmp(settingsRegistry, QSettings::NativeFormat);
    if (enabled)
    {
        QString pathToExec = QApplication::applicationDirPath();

        pathToExec.replace("/", "\\");
        settingsTmp.setValue("mysms", "\"" + pathToExec + "\\mysms.exe\" min");
    }
    else
    {
        settingsTmp.remove("mysms");
    }
#endif
}

bool UserSettings::autorunEnabled()
{
#ifdef Q_OS_WIN
    QSettings settingsTmp(settingsRegistry, QSettings::NativeFormat);
    QString value = settingsTmp.value("mysms").toString();
    return value.isEmpty() ? false : true;
#else
    return false;
#endif
}

void UserSettings::loadSettings()
{
    m_settings.beginGroup(settingsGroup);

    m_userSettingsData.commonTabData.startExecSelector = autorunEnabled();
    m_userSettingsData.commonTabData.showInTaskbarAfterExitSelector = m_settings.value("showInTaskbarAfterExitSelector").toBool();

    m_userSettingsData.notificationTabData.displayNotificationsSelector = m_settings.value("notification.displayNotifications").toBool();
    m_userSettingsData.notificationTabData.privacyModeNotificationsSelector = m_settings.value("notification.privacyModeNotifications").toBool();

    m_userSettingsData.soundTabData.messageSoundEnable = m_settings.value("sound.messageSoundEnable").toBool();
    m_userSettingsData.soundTabData.messageSoundOnlyFirst = m_settings.value("sound.messageSoundOnlyFirst").toBool();
    m_userSettingsData.soundTabData.groupMessageSelector = m_settings.value("sound.groupMessage").toBool();
    m_userSettingsData.soundTabData.singleMessageSelector = m_settings.value("sound.singleMessage").toBool();
    m_userSettingsData.soundTabData.singleSoundIndex = m_settings.value("sound.singleSoundIndex").toInt();
    m_userSettingsData.soundTabData.groupSoundIndex = m_settings.value("sound.groupSoundIndex").toInt();

    m_userSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector = m_settings.value("shortcuts.openAutoEnable").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector = m_settings.value("shortcuts.openAutoShift").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector = m_settings.value("shortcuts.openAutoCtrl").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector = m_settings.value("shortcuts.openAutoAlt").toBool();
    m_userSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit = m_settings.value("shortcuts.openAutoKey").toString();

    m_userSettingsData.shortcutsTabData.mysmsSendEnableSelector = m_settings.value("shortcuts.sendEnable").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendShiftSelector = m_settings.value("shortcuts.sendShift").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendCtrlSelector = m_settings.value("shortcuts.sendCtrl").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendAltSelector = m_settings.value("shortcuts.sendAlt").toBool();
    m_userSettingsData.shortcutsTabData.mysmsSendKeyEdit = m_settings.value("shortcuts.sendKeyEdit").toString();

    m_userSettingsData.networkTabData.portNumber = m_settings.value("network.portNumber").toInt();
    m_userSettingsData.networkTabData.alternativePortsSelector = m_settings.value("network.alternativePorts").toBool();
    m_userSettingsData.networkTabData.proxyRecognitionComboIndex = m_settings.value("network.proxyRecognition").toInt();
    m_userSettingsData.networkTabData.hostEdit = m_settings.value("network.host").toString();
    m_userSettingsData.networkTabData.portEdit = m_settings.value("network.port").toString();
    m_userSettingsData.networkTabData.proxyAuth = m_settings.value("network.proxyAuth").toBool();
    m_userSettingsData.networkTabData.userEdit = m_settings.value("network.user").toString();
    m_userSettingsData.networkTabData.passwordEdit = m_settings.value("network.password").toString();
    m_userSettingsData.networkTabData.uPnpSelector = m_settings.value("network.uPnp").toBool();

    m_settings.endGroup();
}

UserSettingsData UserSettings::getUserSettingsData() const
{
    return m_userSettingsData;
}

void UserSettings::setDefaultData()
{
    UserSettingsData data;

    data.commonTabData.startExecSelector = autorunEnabled();
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

bool UserSettings::areSettingsAvailable()
{
     bool ret = false;

     m_settings.beginGroup(settingsGroup);
     ret = m_settings.value(settingsInitialized).toBool();
     m_settings.endGroup();

     return ret;
}
