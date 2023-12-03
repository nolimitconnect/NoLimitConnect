//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PEngine.h"

#include <CoreLib/VxDebug.h>
#include <PktLib/PktsRelay.h>

//============================================================================
void P2PEngine::onPktRelayUserDisconnect( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	PktRelayUserDisconnect* pktRelayErr = (PktRelayUserDisconnect*)pktHdr;
	LogModule( eLogRelay, LOG_VERBOSE, "P2PEngine::onPktRelayUserDisconnect err %s relaying pkt type %d to %s by host %s",
			   DescribeRelayError( pktRelayErr->getRelayError() ), pktRelayErr->getPktType(), 
			   pktRelayErr->getDestOnlineId().toOnlineIdString().c_str(), pktRelayErr->getHostOnlineId().toOnlineIdString().c_str() );
	// TODO manage hosted users online state
}
