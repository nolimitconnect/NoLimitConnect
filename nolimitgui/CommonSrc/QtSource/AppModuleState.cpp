//============================================================================
// Copyright (C) 2024 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppModuleState.h"

#include "AppCommon.h"
#include "GuiPlayerMgr.h"

#include <GuiInterface/IMediaPlayerRequests.h>
#include <MediaPlayerNlc.h>

AppModuleState::AppModuleState( void )
{
    memset( m_IsRunning, 0, sizeof( m_IsRunning ) );
}

//============================================================================
AppModuleState::~AppModuleState()
{
}

//============================================================================
void AppModuleState::toGuiSetIsAppModuleRunning( EAppModule appModule, bool isRunning )
{ 
    m_IsRunning[ appModule ] = isRunning; 
}

//============================================================================
bool AppModuleState::toGuiGetIsAppModuleRunning( EAppModule appModule )
{ 
    return m_IsRunning[ appModule ]; 
}

//============================================================================
bool AppModuleState::toGuiRunModule( EAppModule appModule )
{
     m_IsRunning[ appModule ] = true; 
     return true;
}

//============================================================================
bool AppModuleState::toGuiStopModule( EAppModule appModule )
{
    if( eAppModulePlayerNlc == appModule )
    {
        IMediaPlayerRequests::getNlcPlayer().fromGuiStopModule(appModule);
        return true;
    }

    return false;
}

