//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateTestConnection.h"
#include "NetworkStateMachine.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
NetworkStateTestConnection::NetworkStateTestConnection( NetworkStateMachine& stateMachine )
: NetworkStateBase( stateMachine )
, m_DirectConnectTester( stateMachine.getDirectConnectTester() )
{
	setNetworkStateType( eNetworkStateTypeTestConnection );
}

//============================================================================
void NetworkStateTestConnection::enterNetworkState( void )
{
	m_Engine.getToGui().toGuiNetworkState( eNetworkStateTypeTestConnection );
}

//============================================================================
void NetworkStateTestConnection::runNetworkState( void )
{
	DirectConnectTestResults& testResults = m_DirectConnectTester.getDirectConnectTestResults();

	if( false == m_NetworkStateMachine.shouldAbort() )
	{
		m_DirectConnectTester.testCanDirectConnect();
		if( ( eNetCmdErrorNone != testResults.m_eNetCmdErr )
			|| ( false == testResults.getCanDirectConnect() ) )
		{
			m_NetworkStateMachine.setPktAnnounceWithCanDirectConnect( testResults.m_MyIpAddr, true );
			m_Engine.getMyPktAnnounce().setRequiresRelay( true );
			m_NetworkStateMachine.changeNetworkState( eNetworkStateTypeWaitForRelay );
		}
		else
		{
			m_NetworkStateMachine.setPktAnnounceWithCanDirectConnect( testResults.m_MyIpAddr, testResults.getCanDirectConnect() ? false : true );
			m_NetworkStateMachine.changeNetworkState( eNetworkStateTypeOnlineDirect );
		}
	}
}




