//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletStoryboardServerViewMine.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>

//============================================================================
AppletStoryboardServerViewMine::AppletStoryboardServerViewMine( AppCommon& app, QWidget* parent )
    : AppletStoryboardClient( app, parent )
{
}

//============================================================================
AppletStoryboardServerViewMine::~AppletStoryboardServerViewMine()
{
    m_MyApp.activityStateChange( this, false );
    wantActivityCallbacks( false );
}

//============================================================================
void AppletStoryboardServerViewMine::setAppletType( EApplet applet )
{
    AppletStoryboardClient::setAppletType( eAppletStoryboardServerViewMine );
}
