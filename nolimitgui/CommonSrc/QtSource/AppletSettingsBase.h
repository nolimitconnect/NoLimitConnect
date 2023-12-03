#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "PluginSettingsWidget.h"
#include "PermissionWidget.h"

#include <GuiInterface/IToGui.h>
#include <ptop_src/ptop_engine_src/PluginSettings/PluginSettingMgr.h>
 
class AppletSettingsBase : public AppletBase
{
	Q_OBJECT
public:
    AppletSettingsBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletSettingsBase() = default;

    virtual PluginSettingsWidget*   getPluginSettingsWidget() = 0;
    virtual PermissionWidget*       getConnectionTestWidget() = 0;

protected slots:
    void                            slotApplyServiceSettings();

protected:
    void                            connectServiceWidgets();
    void                            loadPluginSetting();
    void                            savePluginSetting();
    void                            loadUiFromSetting();
    void                            saveUiToSetting();

    PluginSetting                   m_PluginSetting;
    EFriendState                    m_OrigPermissionLevel = eFriendStateIgnore;
    EFriendState                    m_OrigRelayPermission = eFriendStateIgnore;
    EFriendState                    m_OrigConnectTestPermission = eFriendStateIgnore;
};


