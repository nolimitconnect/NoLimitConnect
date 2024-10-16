//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkEventAvail.h"
#include "NetworkStateMachine.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxSktUtil.h>
#include <NetLib/VxPeerMgr.h>
#include <PktLib/PktAnnounce.h>

//============================================================================
NetworkEventAvail::NetworkEventAvail( NetworkStateMachine& stateMachine, const char* lclIp, bool isCellularNetwork )
: NetworkEventBase( stateMachine )
{
	setNetworkEventType( eNetworkEventTypeAvail );
	if( lclIp )
	{
		m_LclIp = lclIp;
	}
	else
	{
		m_LclIp.clear();
	}

	VxSetLclIpAddress( m_LclIp.c_str() );
	m_bIsCellNetwork = isCellularNetwork;

    LogMsg( LOG_DEBUG, "NetworkEventAvail created with ip %s", lclIp ? lclIp : "" );
}

//============================================================================
void NetworkEventAvail::runNetworkEvent( void )
{
	if( VxIsAppShuttingDown() )
	{
		// do not mess with listen events while shutting down
		return;
	}

	LogModule( eLogNetworkState, LOG_VERBOSE, "NetworkEventAvail::runNetworkEvent start" );
	m_NetworkStateMachine.resolveWebsiteUrls();
	uint16_t prevPort = m_Engine.getNetStatusAccum().getIpPort();
    uint16_t listenPort = m_Engine.getEngineSettings().getTcpIpPort();
	bool ipv6 = m_Engine.getNetStatusAccum().getUseIpv6();
	EFirewallTestType firewallTestType = m_Engine.getNetStatusAccum().getFirewallTestType();
    m_Engine.getNetStatusAccum().setIpPort( listenPort );

	if( VxIsIpValid( m_LclIp ) )
	{
		m_Engine.getNetStatusAccum().setLocalIpAddress( m_LclIp );
	}

	if( listenPort != m_PktAnn.getOnlinePort() )
    {
        m_PktAnn.setOnlinePort( listenPort );
        m_Engine.getToGui().toGuiUpdateMyIdent( &m_PktAnn );
    }

    if( !m_Engine.getPeerMgr().isListening( ipv6 ) || listenPort != prevPort )
    {
        m_Engine.getPeerMgr().startListening( ipv6, listenPort, eFirewallTestAssumeNoFirewall != firewallTestType );
    }

    LogModule( eLogNetworkState, LOG_VERBOSE, "NetworkEventAvail::runNetworkEvent done" );
}


