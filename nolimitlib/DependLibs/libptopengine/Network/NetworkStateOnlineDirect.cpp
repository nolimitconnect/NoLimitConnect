//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateOnlineDirect.h"
#include "NetworkStateMachine.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
NetworkStateOnlineDirect::NetworkStateOnlineDirect( NetworkStateMachine& stateMachine )
: NetworkStateBase( stateMachine )
{
	setNetworkStateType( eNetworkStateTypeOnlineDirect );
}

//============================================================================
void NetworkStateOnlineDirect::enterNetworkState( void )
{
	m_Engine.getToGui().toGuiNetworkState( eNetworkStateTypeOnlineDirect );
}

//============================================================================
void NetworkStateOnlineDirect::runNetworkState( void )
{
	//int loopCnt = 0;
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



