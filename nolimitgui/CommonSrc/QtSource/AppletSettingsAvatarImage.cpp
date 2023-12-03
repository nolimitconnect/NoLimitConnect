//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsAvatarImage.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletSettingsAvatarImage::AppletSettingsAvatarImage( AppCommon& app, QWidget* parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_AVATAR_IMAGE, app, parent )
{
    setupServiceBaseApplet( eAppletSettingsAvatarImage, ePluginTypeHostPeerUser );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsAvatarImage::~AppletSettingsAvatarImage()
{
    m_MyApp.activityStateChange( this, false );
}
