//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateWaitForRelay.h"
#include "NetworkStateMachine.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
NetworkStateWaitForRelay::NetworkStateWaitForRelay( NetworkStateMachine& stateMachine )
	: NetworkStateBase( stateMachine )
{
	setNetworkStateType( eNetworkStateTypeWaitForRelay );
}

//============================================================================
void NetworkStateWaitForRelay::enterNetworkState( void )
{
	m_Engine.getToGui().toGuiNetworkState( eNetworkStateTypeWaitForRelay );
}

//============================================================================
void NetworkStateWaitForRelay::runNetworkState( void )
{
	while( false == m_NetworkStateMachine.checkAndHandleNetworkEvents() )
	{
		if( m_PktAnn.requiresRelay() &&  m_NetworkStateMachine.isRelayServiceConnected() )
		{
			m_NetworkStateMachine.changeNetworkState( eNetworkStateTypeOnlineThroughRelay );
			return;
		}

		LogModule( eLogNetworkState, LOG_VERBOSE, "NetworkStateWaitForRelay waiting for relay connect" );
		VxSleep( 2000 );
	}
}




