//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateLost.h"
#include "NetworkStateMachine.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
NetworkStateLost::NetworkStateLost( NetworkStateMachine& stateMachine )
: NetworkStateBase( stateMachine )
{
	setNetworkStateType( eNetworkStateTypeLost );
}

//============================================================================
void NetworkStateLost::enterNetworkState( void )
{
	m_Engine.getToGui().toGuiNetworkState( eNetworkStateTypeLost );
}

//============================================================================
void NetworkStateLost::runNetworkState( void )
{
	while( false == m_NetworkStateMachine.checkAndHandleNetworkEvents() )
	{
		VxSleep( 500 );
	}
}


