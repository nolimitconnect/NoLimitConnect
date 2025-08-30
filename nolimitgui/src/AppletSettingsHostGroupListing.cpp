//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsHostGroupListing.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletSettingsHostGroupListing::AppletSettingsHostGroupListing( AppCommon& app, QWidget * parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_HOST_GROUP_LISTING, app, parent )
{
    setAppletType( eAppletSettingsHostGroupListing );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );
}
