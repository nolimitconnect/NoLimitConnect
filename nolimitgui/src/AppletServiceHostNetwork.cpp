//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceHostNetwork.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceHostNetwork::AppletServiceHostNetwork( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_HOST_NETWORK, app, parent )
{
    setupServiceBaseApplet( eAppletServiceHostNetwork, ePluginTypeHostNetwork );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
void AppletServiceHostNetwork::loadFromSettings()
{

}

//============================================================================
void AppletServiceHostNetwork::saveToSettings()
{

}

