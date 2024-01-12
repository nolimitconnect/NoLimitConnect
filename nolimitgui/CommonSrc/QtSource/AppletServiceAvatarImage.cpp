//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceAvatarImage.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceAvatarImage::AppletServiceAvatarImage( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_AVATAR_IMAGE, app, parent )
{
    setupServiceBaseApplet( eAppletServiceAvatarImage, ePluginTypeHostPeerUser );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletServiceAvatarImage::~AppletServiceAvatarImage()
{
    m_MyApp.activityStateChange( this, false );
}
