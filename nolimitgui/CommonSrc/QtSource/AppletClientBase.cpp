//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletClientBase.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "GuiHostSession.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletClientBase::AppletClientBase( const char* objName, AppCommon& app, QWidget* parent )
    : AppletHostBase( objName, app, parent )
{
}

//============================================================================
void AppletClientBase::onJointButtonClicked( GuiHostSession* hostSession )
{
    if( hostSession )
    {
        manageHostSession( hostSession, true );
    }
    else
    {
        LogMsg( LOG_ERROR, "AppletClientBase::onJointButtonClicked null hostSession" );
    }
}
