//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceStoryboard.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceStoryboard::AppletServiceStoryboard( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_SHARE_STORYBOARD, app, parent )
{
    setupServiceBaseApplet( eAppletServiceStoryboard, ePluginTypeStoryboardServer );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletServiceStoryboard::~AppletServiceStoryboard()
{
    m_MyApp.activityStateChange( this, false );
}
