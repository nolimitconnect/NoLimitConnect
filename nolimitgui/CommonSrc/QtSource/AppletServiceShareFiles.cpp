//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceShareFiles.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceShareFiles::AppletServiceShareFiles( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_SHARE_FILES, app, parent )
{
    setupServiceBaseApplet( eAppletServiceShareFiles, ePluginTypeFileShareServer );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletServiceShareFiles::~AppletServiceShareFiles()
{
    m_MyApp.activityStateChange( this, false );
}
