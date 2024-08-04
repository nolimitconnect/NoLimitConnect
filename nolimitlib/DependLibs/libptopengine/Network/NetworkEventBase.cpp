//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkEventBase.h"
#include "NetworkStateMachine.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
NetworkEventBase::NetworkEventBase( NetworkStateMachine& stateMachine )
: m_Engine( stateMachine.getEngine() )
, m_PktAnn( stateMachine.getEngine().getMyPktAnnounce() )
, m_NetServicesMgr( stateMachine.getEngine().getNetServicesMgr() )
, m_NetworkStateMachine( stateMachine )
, m_eNetworkEventType( eNetworkEventTypeUnknown )
{
}

//============================================================================
bool NetworkEventBase::checkForAbortOrShutdown( void )
{
	return m_NetworkStateMachine.checkForAbortOrShutdown();
}
