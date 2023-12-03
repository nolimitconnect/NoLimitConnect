//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateOnlineThroughRelay.h"
#include "NetworkStateMachine.h"
#include <ptop_src/ptop_engine_src/NetServices/NetServicesMgr.h>

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
NetworkStateOnlineThroughRelay::NetworkStateOnlineThroughRelay( NetworkStateMachine& stateMachine )
: NetworkStateBase( stateMachine )
{
	setNetworkStateType( eNetworkStateTypeOnlineThroughRelay );
}

//============================================================================
void NetworkStateOnlineThroughRelay::enterNetworkState( void )
{
	m_Engine.getToGui().toGuiNetworkState( eNetworkStateTypeOnlineThroughRelay );
}

//============================================================================
void NetworkStateOnlineThroughRelay::runNetworkState( void )
{
	while( false == m_NetworkStateMachine.checkAndHandleNetworkEvents() )
	{
		if( m_PktAnn.requiresRelay() 
			&& ( false == m_NetworkStateMachine.isRelayServiceConnected() ) )
		{
			m_NetworkStateMachine.changeNetworkState( eNetworkStateTypeWaitForRelay );
			return;
		}

		VxSleep( 2000 );
	}
}



