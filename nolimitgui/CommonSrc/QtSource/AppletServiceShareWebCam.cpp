//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceShareWebCam.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceShareWebCam::AppletServiceShareWebCam( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_SHARE_WEB_CAM, app, parent )
{
    setupServiceBaseApplet( eAppletServiceShareWebCam, ePluginTypeCamServer );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletServiceShareWebCam::~AppletServiceShareWebCam()
{
    m_MyApp.activityStateChange( this, false );
}
