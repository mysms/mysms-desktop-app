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

userSettings* userSettings::muserSettings = NULL;

userSettings::userSettings()
{}

userSettings* userSettings::getInstance()
{
    if (muserSettings == NULL )
    {
        muserSettings = new userSettings();
    }

    return muserSettings;
}

void userSettingsData::SetTabData(userSettingsData const & passedUserSettingsData)
{
    commonTabData       = passedUserSettingsData.commonTabData;
    networkTabData      = passedUserSettingsData.networkTabData;
    soundTabData        = passedUserSettingsData.soundTabData;
    shortcutsTabData    = passedUserSettingsData.shortcutsTabData;
    notificationTabData = passedUserSettingsData.notificationTabData;
}

void userSettings::saveSettings(userSettingsData const & passedUserSettingsData)
{
    settings.beginGroup(settingsGroup);

    muserSettingsData.SetTabData(passedUserSettingsData);

    setStartup();
    //settings.setValue("muserSettingsData.commonTabData.startExecSelector", muserSettingsData.commonTabData.startExecSelector);
    settings.setValue("muserSettingsData.commonTabData.showInTaskbarAfterExitSelector", muserSettingsData.commonTabData.showInTaskbarAfterExitSelector);


    settings.setValue("muserSettingsData.notificationTabData.displayNotificationsSelector", muserSettingsData.notificationTabData.displayNotificationsSelector );
    settings.setValue("muserSettingsData.notificationTabData.privacyModeNotificationsSelector", muserSettingsData.notificationTabData.privacyModeNotificationsSelector );


    settings.setValue("muserSettingsData.soundTabData.messageSoundEnable", muserSettingsData.soundTabData.messageSoundEnable);
    settings.setValue("muserSettingsData.soundTabData.messageSoundOnlyFirst", muserSettingsData.soundTabData.messageSoundOnlyFirst);
    settings.setValue("muserSettingsData.soundTabData.groupMessageSelector", muserSettingsData.soundTabData.groupMessageSelector);
    settings.setValue("muserSettingsData.soundTabData.singleMessageSelector", muserSettingsData.soundTabData.singleMessageSelector);
    settings.setValue("muserSettingsData.soundTabData.singleSoundIndex", muserSettingsData.soundTabData.singleSoundIndex);
    settings.setValue("muserSettingsData.soundTabData.groupSoundIndex", muserSettingsData.soundTabData.groupSoundIndex);


    settings.setValue("muserSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector", muserSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector", muserSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector", muserSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector", muserSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit", muserSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit);

    settings.setValue("muserSettingsData.shortcutsTabData.mysmsSendEnableSelector", muserSettingsData.shortcutsTabData.mysmsSendEnableSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsSendShiftSelector", muserSettingsData.shortcutsTabData.mysmsSendShiftSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsSendCtrlSelector", muserSettingsData.shortcutsTabData.mysmsSendCtrlSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsSendAltSelector", muserSettingsData.shortcutsTabData.mysmsSendAltSelector);
    settings.setValue("muserSettingsData.shortcutsTabData.mysmsSendKeyEdit", muserSettingsData.shortcutsTabData.mysmsSendKeyEdit);


    settings.setValue("muserSettingsData.networkTabData.portNumber", muserSettingsData.networkTabData.portNumber);
    settings.setValue("muserSettingsData.networkTabData.alternativePortsSelector", muserSettingsData.networkTabData.alternativePortsSelector);
    settings.setValue("muserSettingsData.networkTabData.proxyRecognitionComboIndex", muserSettingsData.networkTabData.proxyRecognitionComboIndex);
    settings.setValue("muserSettingsData.networkTabData.hostEdit", muserSettingsData.networkTabData.hostEdit);
    settings.setValue("muserSettingsData.networkTabData.portEdit", muserSettingsData.networkTabData.portEdit);
    settings.setValue("muserSettingsData.networkTabData.proxyAuth", muserSettingsData.networkTabData.proxyAuth);
    settings.setValue("muserSettingsData.networkTabData.userEdit", muserSettingsData.networkTabData.userEdit);
    settings.setValue("muserSettingsData.networkTabData.passwordEdit", muserSettingsData.networkTabData.passwordEdit);
    settings.setValue("muserSettingsData.networkTabData.uPnpSelector", muserSettingsData.networkTabData.uPnpSelector);

    settings.setValue(defaultSettingsWritten, true);

    settings.endGroup();
    settings.sync();
}

void userSettings::setStartup()
{
    QSettings settingsTmp(settingsRegistry, QSettings::NativeFormat);

    bool settingToStore = muserSettingsData.commonTabData.startExecSelector;

    loadStartup();

    if (muserSettingsData.commonTabData.startExecSelector != settingToStore)       // val in registry is different
    {
        muserSettingsData.commonTabData.startExecSelector = settingToStore;

        if (muserSettingsData.commonTabData.startExecSelector)
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
}

void userSettings::loadStartup()
{
    QSettings settingsTmp(settingsRegistry, QSettings::NativeFormat);
    QString value = settingsTmp.value("mysms").toString();

    if (value.isEmpty())
        muserSettingsData.commonTabData.startExecSelector = false;
    else
        muserSettingsData.commonTabData.startExecSelector = true;
}

void userSettings::loadSettings()
{
    settings.beginGroup(settingsGroup);            

    loadStartup();
    //muserSettingsData.commonTabData.startExecSelector = settings.value("muserSettingsData.commonTabData.startExecSelector").toBool();
    muserSettingsData.commonTabData.showInTaskbarAfterExitSelector = settings.value("muserSettingsData.commonTabData.showInTaskbarAfterExitSelector").toBool();

    muserSettingsData.notificationTabData.displayNotificationsSelector = settings.value("muserSettingsData.notificationTabData.displayNotificationsSelector").toBool();
    muserSettingsData.notificationTabData.privacyModeNotificationsSelector = settings.value("muserSettingsData.notificationTabData.privacyModeNotificationsSelector").toBool();

    muserSettingsData.soundTabData.messageSoundEnable = settings.value("muserSettingsData.soundTabData.messageSoundEnable").toBool();
    muserSettingsData.soundTabData.messageSoundOnlyFirst = settings.value("muserSettingsData.soundTabData.messageSoundOnlyFirst").toBool();
    muserSettingsData.soundTabData.groupMessageSelector = settings.value("muserSettingsData.soundTabData.groupMessageSelector").toBool();
    muserSettingsData.soundTabData.singleMessageSelector = settings.value("muserSettingsData.soundTabData.singleMessageSelector").toBool();
    muserSettingsData.soundTabData.singleSoundIndex = settings.value("muserSettingsData.soundTabData.singleSoundIndex").toInt();
    muserSettingsData.soundTabData.groupSoundIndex = settings.value("muserSettingsData.soundTabData.groupSoundIndex").toInt();

    muserSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsOpenAutoEnableSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsOpenAutoShiftSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsOpenAutoCtrlSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsOpenAutoAltSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit = settings.value("muserSettingsData.shortcutsTabData.mysmsOpenAutoKeyEdit").toString();

    muserSettingsData.shortcutsTabData.mysmsSendEnableSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsSendEnableSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsSendShiftSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsSendShiftSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsSendCtrlSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsSendCtrlSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsSendAltSelector = settings.value("muserSettingsData.shortcutsTabData.mysmsSendAltSelector").toBool();
    muserSettingsData.shortcutsTabData.mysmsSendKeyEdit = settings.value("muserSettingsData.shortcutsTabData.mysmsSendKeyEdit").toString();

    muserSettingsData.networkTabData.portNumber = settings.value("muserSettingsData.networkTabData.portNumber").toInt();
    muserSettingsData.networkTabData.alternativePortsSelector = settings.value("muserSettingsData.networkTabData.alternativePortsSelector").toBool();
    muserSettingsData.networkTabData.proxyRecognitionComboIndex = settings.value("muserSettingsData.networkTabData.proxyRecognitionComboIndex").toInt();
    muserSettingsData.networkTabData.hostEdit = settings.value("muserSettingsData.networkTabData.hostEdit").toString();
    muserSettingsData.networkTabData.portEdit = settings.value("muserSettingsData.networkTabData.portEdit").toString();
    muserSettingsData.networkTabData.proxyAuth = settings.value("muserSettingsData.networkTabData.proxyAuth").toBool();
    muserSettingsData.networkTabData.userEdit = settings.value("muserSettingsData.networkTabData.userEdit").toString();
    muserSettingsData.networkTabData.passwordEdit = settings.value("muserSettingsData.networkTabData.passwordEdit").toString();
    muserSettingsData.networkTabData.uPnpSelector = settings.value("muserSettingsData.networkTabData.uPnpSelector").toBool();

    settings.endGroup();
}

userSettingsData userSettings::GetUserSettingsData() const
{
    return muserSettingsData;
}

void userSettings::setDefaultData()
{
    userSettingsData data;

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

void userSettings::removeObsoleteVersionSettings()
{
    QFile::remove("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\mysms App.lnk");
}

bool userSettings::areSettingsAvailable()
{
     bool ret = false;

     settings.beginGroup(settingsGroup);
     ret = settings.value(defaultSettingsWritten).toBool();
     settings.endGroup();

     return ret;
}
