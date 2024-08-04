//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkEventLost.h"
#include "NetworkStateMachine.h"

//============================================================================
NetworkEventLost::NetworkEventLost( NetworkStateMachine& stateMachine )
: NetworkEventBase( stateMachine )
{
	setNetworkEventType( eNetworkEventTypeLost );
}

//============================================================================
void NetworkEventLost::runNetworkEvent( void )
{
	m_NetworkStateMachine.changeNetworkState( eNetworkStateTypeLost );
}



