//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityInformation.h"
#include "AppletSettingsHostRandomConnect.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "GuiHelpers.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
AppletSettingsHostRandomConnect::AppletSettingsHostRandomConnect( AppCommon& app, QWidget* parent )
: AppletSettingsHostBase( OBJNAME_APPLET_SETTINGS_HOST_RANDOM_CONNECT, app, parent )
{
    setAppletType( eAppletSettingsHostRandomConnect );
    setPluginType( ePluginTypeHostRandomConnect );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    getPluginSettingsWidget()->setupSettingsWidget( eAppletSettingsHostRandomConnect, ePluginTypeHostRandomConnect );

    loadPluginSetting();

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsHostRandomConnect::~AppletSettingsHostRandomConnect()
{
    m_MyApp.activityStateChange( this, false );
}
