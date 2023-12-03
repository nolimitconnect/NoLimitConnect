//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "RelayMgr.h"
#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/HostJoinMgr/HostJoinMgr.h>
#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>

#include <NetLib/VxSktBase.h>
#include <PktLib/PktsRelay.h>

//============================================================================
RelayMgr::RelayMgr( P2PEngine& engine )
	: m_Engine( engine )
{
}

//============================================================================
bool RelayMgr::handleRelayPkt( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
	// verify src is someone who has connected
	BigListInfo* srcBigInfo = m_Engine.getBigListMgr().findBigListInfo( srcOnlineId );
	if( !srcBigInfo )
	{
		// this is someone we have never encountered
		VxReportHack( eHackerLevelSevere, eHackerReasonInvalidPkt, sktBase, "attempted relay pkt %s", srcOnlineId.toOnlineIdString().c_str() );
		sktBase->closeSkt( eSktCloseHackLevetSevere );
		return true;
	}

	if( srcBigInfo->isIgnored() )
	{
		VxReportHack( eHackerLevelSuspicious, eHackerReasonAccessDenied, sktBase, "ignored user %s %s attempted relay pkt", 
					  srcBigInfo->getOnlineName(), srcOnlineId.toOnlineIdString().c_str() );

		if( !m_Engine.getConnectIdListMgr().isConnectionInUse( sktBase->getSocketId() ) )
		{
			sktBase->closeSkt( eSktCloseBlockedUser );
		}

		return true;
	}
	
	if( srcBigInfo->isIgnored() )
	{
		VxReportHack( eHackerLevelSuspicious, eHackerReasonAccessDenied, sktBase, "ignored user %s %s attempted relay pkt", 
					  srcBigInfo->getOnlineName(), srcOnlineId.toOnlineIdString().c_str() );

		if( !m_Engine.getConnectIdListMgr().isConnectionInUse( sktBase->getSocketId() ) )
		{
			sktBase->closeSkt( eSktCloseBlockedUser );
		}

		return true;
	}

	VxGUID destOnlineId = pktHdr->getDestOnlineId();
	if( !isJoinedToRelayHost( srcOnlineId ) )
	{
		sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrSrcNotJoined );
		return true;
	}

	// verify dest is someone who has connected
	BigListInfo* destBigInfo = m_Engine.getBigListMgr().findBigListInfo( destOnlineId );
	if( !destBigInfo )
	{
		// this is not someone recently connected but if was not friend or admin they would not have been restored on startup
		sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrUserNotOnline );
		return true;
	}

	if( !isJoinedToRelayHost( destOnlineId ) )
	{
		sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrDestNotJoined );
		return true;
	}

    std::shared_ptr<VxSktBase> sktBaseRelay = m_Engine.getConnectIdListMgr().findRelayMemberConnection( destOnlineId );
	if( !sktBaseRelay )
	{
		sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrUserNotOnline );
		return true;
	}
	else if( sktBaseRelay->getIsPeerPktAnnSet() && sktBaseRelay->getPeerOnlineId() != destOnlineId )
	{
		LogModule( eLogRelay, LOG_ERROR, "handleRelayPkt wrong socket found" );
		sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrUserNotOnline );
		return true;
	}

	if( pktHdr->getPktType() == PKT_TYPE_ANNOUNCE )
	{
		PktAnnounce* pktAnn = (PktAnnounce*)pktHdr;
		if( !onRelayPktAnnounce( sktBase, pktAnn, srcBigInfo, destBigInfo ) )
		{
			sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrInvalidPktAnn );
			return true;
		}
	}

	if( 0 != sktBaseRelay->txPacket( destOnlineId, pktHdr ) )
	{
        LogModule( eLogRelay, LOG_VERBOSE, "handleRelayPkt FAILED sent relay pkt %s srcId %s %s destId %s %s", pktHdr->describePkt().c_str(),
			srcOnlineId.toOnlineIdString().c_str(), sktBase->getPeerOnlineName().c_str(), destOnlineId.toOnlineIdString().c_str(), sktBaseRelay->getPeerOnlineName().c_str() );

		sendRelayError( pktHdr, srcOnlineId, destOnlineId, sktBase, eRelayErrUserNotOnline );
		return true;
	}

    LogModule( eLogRelay, LOG_VERBOSE, "handleRelayPkt sent relay pkt %s srcId %s %s destId %s %s", pktHdr->describePkt().c_str(),
		srcOnlineId.toOnlineIdString().c_str(), sktBase->getPeerOnlineName().c_str(), destOnlineId.toOnlineIdString().c_str(), sktBaseRelay->getPeerOnlineName().c_str() );
	return true;
}

//============================================================================
bool RelayMgr::isJoinedToRelayHost( VxGUID& onlineId )
{
	return m_Engine.getHostJoinMgr().isUserJoinedToRelayHost( onlineId );
}

//============================================================================
bool RelayMgr::onRelayPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, BigListInfo* srcBigInfo, BigListInfo* destBigInfo )
{
	LogModule( eLogRelay, LOG_VERBOSE, "RelayMgr::onRelayPktAnnounce from %s %s to %s %s", 
			srcBigInfo->getOnlineName(), srcBigInfo->getMyOnlineId().toOnlineIdString().c_str(),
			destBigInfo->getOnlineName(), destBigInfo->getMyOnlineId().toOnlineIdString().c_str() );
	// TODO extra validation
	return true;
}

//============================================================================
bool RelayMgr::sendRelayError( VxPktHdr* pktHdr, VxGUID& srcOnlineId, VxGUID& destOnlineId, std::shared_ptr<VxSktBase>& sktBase, ERelayErr relayErr )
{
	PktRelayUserDisconnect pktReply;
	pktReply.setSrcOnlineId( m_Engine.getMyOnlineId() );

	pktReply.setPktType( pktHdr->getPktType() );
	pktReply.setDestUserOnlineId( destOnlineId );
	pktReply.setHostOnlineId( m_Engine.getMyOnlineId() );
	pktReply.setRelayError( relayErr );

	return 0 == sktBase->txPacket( srcOnlineId, &pktReply );
}
