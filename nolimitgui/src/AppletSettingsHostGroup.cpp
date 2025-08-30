//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsHostGroup.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
AppletSettingsHostGroup::AppletSettingsHostGroup( AppCommon& app, QWidget* parent )
    : AppletSettingsHostBase( OBJNAME_APPLET_SETTINGS_HOST_GROUP, app, parent )
{
    setAppletType( eAppletSettingsHostGroup );
    setPluginType( ePluginTypeHostGroup );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    getPluginSettingsWidget()->setupSettingsWidget( eAppletSettingsHostGroup, ePluginTypeHostGroup );

    loadPluginSetting();

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsHostGroup::~AppletSettingsHostGroup()
{
    m_MyApp.activityStateChange( this, false );
}
