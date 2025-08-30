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
#include <PluginSettings/PluginSettingMgr.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletHostNetworkStatusUi;
}
QT_END_NAMESPACE
 
class AppletHostNetworkStatus : public AppletBase
{
	Q_OBJECT
public:
    AppletHostNetworkStatus( AppCommon& app, QWidget* parent );
	virtual ~AppletHostNetworkStatus();

    PermissionWidget*           getNetworkHostPermissionWidget();
    PermissionWidget*           getConnectionTestWidget();

protected slots:
    void                        slotHostRequirementsButtonClicked();
    void                        slotUpdateStatusTimeout();

protected:
    Ui::AppletHostNetworkStatusUi& ui;

    QTimer*                     m_UpdateStatusTimer;
};


