//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsHostNetwork.h"

#include "ActivityInformation.h"
#include "AppCommon.h"
#include "GuiHelpers.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
AppletSettingsHostNetwork::AppletSettingsHostNetwork( AppCommon& app, QWidget* parent )
: AppletSettingsHostBase( OBJNAME_APPLET_SETTINGS_HOST_NETWORK, app, parent )
{
    setAppletType( eAppletSettingsHostNetwork );
    setPluginType( ePluginTypeHostNetwork );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    getPluginSettingsWidget()->setupSettingsWidget( eAppletSettingsHostNetwork, ePluginTypeHostNetwork );

    loadPluginSetting();

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsHostNetwork::~AppletSettingsHostNetwork()
{
    m_MyApp.activityStateChange( this, false );
}
