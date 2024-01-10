#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsBase.h"

#include "PluginSettingsWidget.h"
#include "PermissionWidget.h"

#include <GuiInterface/IToGui.h>
#include <PluginSettings/PluginSettingMgr.h>

#include "ui_AppletSettingsHostBase.h"
 
class AppletSettingsHostBase : public AppletSettingsBase
{
	Q_OBJECT
public:
    AppletSettingsHostBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletSettingsHostBase() = default;
            
    PluginSettingsWidget*       getPluginSettingsWidget() { return ui.m_PluginSettingsWidget; }

protected slots:
    void                        slotHostRequirementsButtonClicked();
    void                        slotApplyServiceSettings();

protected:
    void                        loadPluginSetting();
    bool                        savePluginSetting();

    Ui::AppletSettingsHostBaseUi ui;
};


