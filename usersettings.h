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


#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QSettings>

class TCommonTabData
{
public:
    TCommonTabData(){}

    bool startExecSelector;
    bool showInTaskbarAfterExitSelector;
};

class TNotificationTabData
{
public:
    bool displayNotificationsSelector;
    bool privacyModeNotificationsSelector;
};

class TNetworkTabData
{
public:
    int portNumber;
    bool alternativePortsSelector;
    int proxyRecognitionComboIndex;
    QString hostEdit;
    QString portEdit;
    bool proxyAuth;
    QString userEdit;
    QString passwordEdit;
    bool uPnpSelector;
};

class TShortcutsTabData
{
public:
    bool mysmsOpenAutoEnableSelector;
    bool mysmsOpenAutoShiftSelector;
    bool mysmsOpenAutoCtrlSelector;
    bool mysmsOpenAutoAltSelector;
    QString mysmsOpenAutoKeyEdit;

    bool mysmsSendEnableSelector;
    bool mysmsSendShiftSelector;
    bool mysmsSendCtrlSelector;
    bool mysmsSendAltSelector;
    QString mysmsSendKeyEdit;
};

class TSoundTabData
{
public:
    bool singleMessageSelector;
    bool groupMessageSelector;

    bool messageSoundOnlyFirst;
    bool messageSoundEnable;

    int  singleSoundIndex;
    int  groupSoundIndex;
} ;

class UserSettingsData
{


public:
    void setTabData(UserSettingsData const & passedUserSettingsData);

    TCommonTabData commonTabData;
    TNotificationTabData notificationTabData;
    TNetworkTabData networkTabData;
    TShortcutsTabData shortcutsTabData;
    TSoundTabData soundTabData;
};

class UserSettings
{
public:
    static UserSettings* getInstance();
    void saveSettings(UserSettingsData const & passedUserSettingsData);
    void loadSettings();
    void removeObsoleteVersionSettings();
    void setDefaultData();
    bool areSettingsAvailable();

    UserSettingsData getUserSettingsData() const;

private:
    UserSettings();
    ~UserSettings();

    void setStartup();
    void loadStartup();

    static UserSettings* m_userSettings;
private:
    QSettings m_settings;
    UserSettingsData m_userSettingsData;
};

#endif // USERSETTINGS_H
