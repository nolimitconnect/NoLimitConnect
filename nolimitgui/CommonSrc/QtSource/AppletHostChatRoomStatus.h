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

#include "AppletSettingsHostBase.h"
#include "PluginSettingsWidget.h"
#include "PermissionWidget.h"

#include <GuiInterface/IToGui.h>
#include <ptop_src/ptop_engine_src/PluginSettings/PluginSettingMgr.h>

#include "ui_AppletHostNetworkStatus.h"
 
class AppletHostChatRoomStatus : public AppletBase
{
	Q_OBJECT
public:
    AppletHostChatRoomStatus( AppCommon& app, QWidget* parent );
	virtual ~AppletHostChatRoomStatus();

    PermissionWidget*           getChatRoomHostPermissionWidget()   { return ui.m_HostPermissionWidget; }
    PermissionWidget*           getConnectionTestWidget()           { return ui.m_ConnectTestPermissionWidget; }
    PermissionWidget*           getGroupHostPermissionWidget()      { return ui.m_AdditionalPermissionWidget; }
    PermissionWidget*           getRandomConnectPermissionWidget()  { return ui.m_RandomConnectPermissionWidget; }

protected slots:
    void                        slotHostRequirementsButtonClicked();
    void                        slotUpdateStatusTimeout();

protected:
    Ui::AppletHostNetworkStatusUi     ui;

    QTimer*                     m_UpdateStatusTimer;
};


