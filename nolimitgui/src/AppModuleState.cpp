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
void AppModuleState::toGuiSetIsAppModuleRunning( EMediaModule mediaModule, bool isRunning )
{ 
    m_IsRunning[ mediaModule ] = isRunning; 
}

//============================================================================
bool AppModuleState::toGuiGetIsAppModuleRunning( EMediaModule mediaModule )
{ 
    return m_IsRunning[ mediaModule ]; 
}

//============================================================================
bool AppModuleState::toGuiRunModule( EMediaModule mediaModule )
{
     m_IsRunning[ mediaModule ] = true; 
     return true;
}

//============================================================================
bool AppModuleState::toGuiStopModule( EMediaModule mediaModule )
{
    if( eMediaModulePlayerNlc == mediaModule )
    {
        IMediaPlayerRequests::getNlcPlayer().fromGuiStopModule(mediaModule);
        return true;
    }

    return false;
}

