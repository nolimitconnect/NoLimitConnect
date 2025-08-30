//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceAboutMe.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceAboutMe::AppletServiceAboutMe( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_ABOUT_ME, app, parent )
{
    setupServiceBaseApplet( eAppletServiceAboutMe, ePluginTypeAboutMePageServer );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletServiceAboutMe::~AppletServiceAboutMe()
{
    m_MyApp.activityStateChange( this, false );
}
