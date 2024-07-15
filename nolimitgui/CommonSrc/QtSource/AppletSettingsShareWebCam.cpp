//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsShareWebCam.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletSettingsNoUserSettings.h"

//============================================================================
AppletSettingsShareWebCam::AppletSettingsShareWebCam( AppCommon& app, QWidget* parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_SHARE_WEB_CAM, app, parent )
, ui(*(new Ui::AppletSettingsNoUserSettingsUi))
{
	ui.setupUi( getContentItemsFrame() );
	setAppletType( eAppletSettingsWebCamServer );
	setPluginType( ePluginTypeCamServer );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsShareWebCam::~AppletSettingsShareWebCam()
{
    m_MyApp.activityStateChange( this, false );
}
