//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsVideoPhone.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletSettingsVideoPhone::AppletSettingsVideoPhone( AppCommon& app, QWidget* parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_VIDEO_PHONE, app, parent )
{
	ui.setupUi( getContentItemsFrame() );
	setAppletType( eAppletSettingsVideoPhone );
	setPluginType( ePluginTypeVideoPhone );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsVideoPhone::~AppletSettingsVideoPhone()
{
    m_MyApp.activityStateChange( this, false );
}
