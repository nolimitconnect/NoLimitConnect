//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsShareFiles.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletSettingsNoUserSettings.h"

//============================================================================
AppletSettingsShareFiles::AppletSettingsShareFiles( AppCommon& app, QWidget* parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_SHARE_FILES, app, parent )
, ui(*(new Ui::AppletSettingsNoUserSettingsUi))
{
	ui.setupUi( getContentItemsFrame() );
	setAppletType( eAppletSettingsShareFiles );
	setPluginType( ePluginTypeFileShareServer );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsShareFiles::~AppletSettingsShareFiles()
{
    m_MyApp.activityStateChange( this, false );
}
