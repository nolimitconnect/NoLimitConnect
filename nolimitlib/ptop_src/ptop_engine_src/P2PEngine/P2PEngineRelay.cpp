//============================================================================
// Copyright (C) 2009 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
