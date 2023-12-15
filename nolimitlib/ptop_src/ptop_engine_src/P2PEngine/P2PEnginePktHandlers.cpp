//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PEngine.h"

#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>
#include <ptop_src/ptop_engine_src/Plugins/PluginBase.h>
#include <ptop_src/ptop_engine_src/Plugins/PluginMgr.h>
#include <ptop_src/ptop_engine_src/Network/NetConnector.h>
#include <ptop_src/ptop_engine_src/Membership/Membership.h>

#include <NetLib/VxSktCrypto.h>
#include <NetLib/VxSktBase.h>

#include <PktLib/PktTcpPunch.h>
#include <PktLib/PktsPing.h>
#include <PktLib/PktsMembership.h>

#include <memory.h>

//============================================================================
void P2PEngine::handlePkt( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	// relay packets will have a destination id other than ourself
	
	if( pktHdr->getDestOnlineId() != getMyOnlineId() )
	{
		getRelayMgr().handleRelayPkt( sktBase, pktHdr );
	}
	else
	{
		PktHandlerBase::handlePkt( sktBase, pktHdr );
	}
}

//============================================================================
void P2PEngine::onPktUnhandled( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogMsg( LOG_ERROR, "onPktUnhandled pkt type %d", pktHdr->getPktType() );
}

//============================================================================
void P2PEngine::onPktInvalid( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogMsg( LOG_ERROR, "onPktInvalid pkt type %d", pktHdr->getPktType() );
}

//============================================================================
void P2PEngine::onPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	if( !sktBase || !sktBase->isConnected() )
	{
		LogMsg( LOG_ERROR, "%s null or invalid param  ", __func__ );
		return;
	}

	bool ipv6 = sktBase->getIsIpv6Connection();
	PktAnnounce* pktAnn = (PktAnnounce *)pktHdr;
	VxGUID contactOnlineId = pktAnn->getMyOnlineId();
	if( contactOnlineId == getMyOnlineId() )
	{
		// it is ourself
        LogMsg( LOG_ERROR, "onPktAnnounce Cannot send a packet to ourself  " );

        std::string rmAddr = sktBase->getRemoteIpAddress() ? sktBase->getRemoteIpAddress() : "";
        std::string ourAddr = getNetStatusAccum().getExternalIpAddress( ipv6 );
        if( rmAddr.empty() || sktBase->getRemoteIpAddress() != ourAddr )
        {
            // remote attack.. serious
            hackerOffense( eHackerLevelSevere, eHackerReasonPktOnlineIdMeFromAnotherIp, pktAnn, sktBase, "rxed same as our online id from another " );
            sktBase->closeSkt( eSktClosePktOnlineIdMeFromAnotherIp );
        }
        else
        {
            hackerOffense( eHackerLevelSuspicious, eHackerReasonPktOnlineIdMeFromMyIp, pktAnn, sktBase, "rxed same as our online from our address " );
            sktBase->closeSkt( eSktClosePktOnlineIdMeFromMyIp );
        }

		return;
	}

	if( getConnectIdListMgr().isUserExcluded( contactOnlineId ) && !getConnectIdListMgr().isNetworkHost( contactOnlineId ) )
	{
		if( !getConnectIdListMgr().isConnectionInUse( sktBase->getSocketId() ) )
		{
			sktBase->closeSkt( eSktCloseBlockedUser );
		}

		return;
	}

	bool isFirstAnnounce = false;
	if( false == sktBase->m_TxCrypto.isKeyValid() )
	{
		// setup tx crypto
		isFirstAnnounce = true;
		std::string networkName;
		m_EngineSettings.getNetworkKey( networkName );
		GenerateTxConnectionKey( sktBase, &pktAnn->m_DirectConnectId, networkName.c_str() );
	}
	else if( !sktBase->getIsPeerPktAnnSet() )
	{
		isFirstAnnounce = true;
	}

	pktAnn->reversePermissions();
	pktAnn->setTimeLastTcpContactMs( GetGmtTimeMs() );

	bool isHostedUserPktAnnounce{ false };
	bool isPeerUpdatePktAnnounce{ false };
	bool isRelayedPktAnnounce{ false };
	if( !isFirstAnnounce )
	{
		// detect if was announced by host instead of user
		if( pktAnn->getHostOnlineId() == sktBase->getPeerOnlineId() && pktAnn->getMyOnlineId() != sktBase->getPeerOnlineId() && IsHostARelayForUsers( pktAnn->getHostType() ) )
		{
			isHostedUserPktAnnounce = true;
		}
		else if( pktAnn->getMyOnlineId() == sktBase->getPeerOnlineId() )
		{
			isPeerUpdatePktAnnounce  = true;
		}
		else
		{
			isRelayedPktAnnounce = true;
		}
	}

	BigListInfo * bigListInfo = 0;
	EHostType hostType{ eHostTypeUnknown };
	EPktAnnUpdateType pktAnnUpdateType = m_BigListMgr.updatePktAnn( pktAnn, &bigListInfo, hostType, false, !isHostedUserPktAnnounce );		
	if( !bigListInfo->isValidNetIdent() )
	{
		LogMsg( LOG_ERROR, "PktAnnounce updatePktAnn INVALID" );
		return;
	}

	if( ePktAnnUpdateTypeIgnored == pktAnnUpdateType )
	{
		LogModule( eLogConnect, LOG_VERBOSE, "Ignoring %s ip %s id %s",
			pktAnn->getOnlineName(),
            sktBase->getRemoteIp().c_str(),
			contactOnlineId.toOnlineIdString().c_str() );
        // if is the first announce and ignored we can close the connection
        // if not first announce then was relayed through host and do not close the connection
        if( isFirstAnnounce )
		{
			m_NetConnector.closeConnection( eSktCloseUserIgnored, contactOnlineId, sktBase );
            getConnectList().onConnectionLost( sktBase );
		}

		return;
	}

	if( isFirstAnnounce && IsConnectReasonJoin( sktBase->getConnectReason() ) )
	{
		getBigListMgr().updateMemberFriendship( bigListInfo );
	}

	bool pktAnnReplyRequested = pktAnn->getIsPktAnnReplyRequested();
	bool reverseConnectionRequested = pktAnn->getIsPktAnnRevConnectRequested();
	if( pktAnn->getTTL() > 0 )
	{
		pktAnn->setTTL( pktAnn->getTTL() - 1 );
		pktAnn->setIsPktAnnReplyRequested( false );
		pktAnn->setIsPktAnnStunRequested( false );
	}

	if( pktAnnReplyRequested )
	{
        LogModule( eLogConnect, LOG_VERBOSE, "P2PEngine::onPktAnnounce from %s %s at %s pktAnn reply requested", 
				   pktAnn->getOnlineName(), pktAnn->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
		if( !m_NetConnector.sendMyPktAnnounce( pktAnn->getMyOnlineId(),
				sktBase,
				false,
				false,
				false ) )
		{
			LogModule( eLogConnect, LOG_VERBOSE, "P2PEngine::onPktAnnounce from %s at %s send pktAnn reply failed",
					   pktAnn->getOnlineName(), pktAnn->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
			sktBase->closeSkt( eSktClosePktAnnSendFail );
            getConnectList().onConnectionLost( sktBase );
			return;
		}
	}

	bool updateOk{ false };

	if( isFirstAnnounce )
	{
		updateOk = onFirstPktAnnounce( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
	}
	else
	{
		if( sktBase->getIsPeerPktAnnSet() )
		{
			// detect if was announced by host instead of user
			if( isHostedUserPktAnnounce )
			{
				updateOk = onHostedUserPktAnnounce( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
			}
			else if( isPeerUpdatePktAnnounce )
			{
				updateOk = onConnectionPktAnnounceUpdated( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
			}
			else
			{
				updateOk = onRelayedUserPktAnnounce( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
			}
	

			//LogModule( eLogConnect, LOG_VERBOSE, "P2PEngine::onPktAnnounce %s %s through relay %s %s ip %s",
			//		   pktAnn->getOnlineName(),
   //                    pktAnn->getMyOnlineId().toOnlineIdString().c_str(),
   //                    sktBase->getPeerOnlineName().c_str(),
			//		   sktBase->getPeerOnlineId().toOnlineIdString().c_str(),
			//		   sktBase->getRemoteIp().c_str() );
		}
		else
		{
			updateOk = onUnexpectedPktAnnounce( sktBase, pktAnn, pktAnnUpdateType, bigListInfo );
			//LogMsg( LOG_ERROR, "P2PEngine::onPktAnnounce not first PktAnd and peer PktAnn not set ip %s",
			//		sktBase->getRemoteIp().c_str() );
			// what should we do here. Hacker attempt or bad programming?
		}
	}

	if( !updateOk )
	{
		 LogModule( eLogConnect, LOG_VERBOSE, "P2PEngine::onPktAnnounce from %s %s at %s failed to update", 
					pktAnn->getOnlineName(), pktAnn->getMyOnlineId().toOnlineIdString().c_str(), sktBase->getRemoteIp().c_str() );
		 sktBase->closeSkt( eSktClosePktAnnUpdateFailed ); // should we close? TODO investigate failed PktAnn update failed
		 return; 
	}

    LogMsg( LOG_VERBOSE, "P2PEngine::onPktAnnounce of %s %s by %s %s at %s skt id %s",
            pktAnn->getOnlineName(), pktAnn->getMyOnlineId().toOnlineIdString().c_str(),
            sktBase->getPeerOnlineName().c_str(), sktBase->getPeerOnlineId().toOnlineIdString().c_str(),
			sktBase->getRemoteIp().c_str(), sktBase->getSocketId().toHexString().c_str() );

#if ENABLE_STUN_REVERSE_CONNECT
    if( sktBase->isConnected() && isFirstAnnounce && pktAnn->getIsPktAnnRevConnectRequested() )
	{
		LogModule( eLogConnect, LOG_VERBOSE, "P2PEngine::onPktAnnounce from %s at %s reverse connect requested", pktAnn->getOnlineName(), sktBase->getRemoteIp().c_str() );
		std::shared_ptr<VxSktBase> poNewSkt;
		m_NetConnector.directConnectTo( pktAnn->getConnectInfo(), poNewSkt, eConnectReasonReverseConnectRequested );
		if( poNewSkt )
		{
			LogModule( eLogConnect, LOG_VERBOSE, "sendMyPktAnnounce 6" );
            if( m_NetConnector.sendMyPktAnnounce(   pktAnn->getMyOnlineId(),
													poNewSkt,
                                                    true,
                                                    false,
                                                    false ) )
            {
				if( poNewSkt->setPeerPktAnn( *pktAnn ) )
				{
					getConnectList().addConnection( poNewSkt, bigListInfo, ( ePktAnnUpdateTypeContactIsSame == pktAnnUpdateType ) );
					getConnectionMgr().onSktConnectedWithPktAnn( sktBase, bigListInfo );
				}
				else
				{
					getConnectList().addConnection( poNewSkt, bigListInfo, ( ePktAnnUpdateTypeContactIsSame == pktAnnUpdateType ) );
				}
            }
            else
            {
                poNewSkt->closeSkt( eSktClosePktAnnSendFail );
				return;
            }
		}
	}
#endif // ENABLE_STUN_REVERSE_CONNECT

	// send ping request to keep connection alive
	if( sktBase->isConnected() )
	{

		PktPingReq pktPingReq;
		pktPingReq.setSrcOnlineId( m_PktAnn.getSrcOnlineId() );
		if( 0 != sktBase->txPacket( bigListInfo->getMyOnlineId(), &pktPingReq ) )
		{
			sktBase->closeSkt( eSktClosePktPingReqSendFail );
            getConnectList().onConnectionLost( sktBase );
			return;
		}
	}
	else
	{
		getConnectList().onConnectionLost( sktBase );
	}
}

//============================================================================
void P2PEngine::onPktAnnList( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAnnList" );
}
	
//============================================================================
void P2PEngine::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPluginOfferReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPluginOfferReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktChatReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktChatReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktChatReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktChatReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVoiceReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVoiceReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVideoFeedReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVideoFeedReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVideoFeedStatus( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVideoFeedStatus" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVideoFeedPic( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVideoFeedPic" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVideoFeedPicChunk( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVideoFeedPicChunk" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktVideoFeedPicAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktVideoFeedPicAck" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileGetReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileGetReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileGetReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileGetReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileSendReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileSendReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileListReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileListReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileListReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileChunkReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileChunkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileChunkReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileSendCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileSendCompleteReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileSendCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileSendCompleteReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileGetCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileGetCompleteReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileGetCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileGetCompleteReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileShareErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileShareErr" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetGetReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetGetReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetGetReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetGetReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetSendReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetSendReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetChunkReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetChunkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetChunkReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetGetCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetGetCompleteReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetGetCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetGetCompleteReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetSendCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetSendCompleteReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetSendCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetSendCompleteReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktAssetXferErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktAssetXferErr" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktMultiSessionReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktMultiSessionReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktMultiSessionReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktMultiSessionReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktSessionStartReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktSessionStartReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktSessionStartReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktSessionStartReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktSessionStopReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktSessionStopReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktSessionStopReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktMyPicSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktMyPicSendReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktMyPicSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktMyPicSendReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktWebServerPicChunkTx( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktWebServerPicChunkTx" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktWebServerPicChunkAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktWebServerPicChunkAck" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktWebServerGetChunkTx( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktWebServerGetChunkTx" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktWebServerGetChunkAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktWebServerGetChunkAck" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktWebServerPutChunkTx( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktWebServerPutChunkTx" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktWebServerPutChunkAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktWebServerPutChunkAck" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTodGameStats( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTodGameStats" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTodGameAction( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTodGameAction" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTodGameValue( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTodGameValue" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTcpPunch( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTcpPunch" );

	PktTcpPunch * pktPunch = ( PktTcpPunch * )pktHdr;
	std::shared_ptr<VxSktBase> poNewSkt;
	if( 0 == m_NetConnector.directConnectTo( pktPunch->m_ConnectInfo, poNewSkt, eConnectReasonPktTcpPunch ) )
	{
		LogMsg( LOG_INFO, "P2PEngine:: TcpPunch SUCCESS" );
		if( nullptr != poNewSkt )
		{
			LogMsg( LOG_INFO, "sendMyPktAnnounce 7" ); 
			m_NetConnector.sendMyPktAnnounce(	pktPunch->m_ConnectInfo.getMyOnlineId(), 
												poNewSkt,
												true,
												false,
												false );
		}
	}
	else
	{
		LogMsg( LOG_INFO, "P2PEngine:: TcpPunch FAIL" );
	}
}

//============================================================================
void P2PEngine::onPktPingReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPingReq" );

	PktPingReq * pktPingReq = (PktPingReq *)pktHdr;
	PktPingReply pktPingReply;
	pktPingReply.setSrcOnlineId( m_PktAnn.getMyOnlineId() );
	pktPingReply.setTimestamp( pktPingReq->getTimestamp() );
	sktBase->txPacket( pktPingReq->getSrcOnlineId(), &pktPingReply );
}

//============================================================================
void P2PEngine::onPktPingReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPingReply" );

	PktPingReply * pktPingReply = (PktPingReply *)pktHdr;
	int64_t timeDiffTmp = GetGmtTimeMs() - pktPingReply->getTimestamp();
	uint16_t timeDiff = timeDiffTmp > 30000 ? 30000 : (uint16_t)timeDiffTmp;

	std::string onlineName;
	BigListInfo * bigListInfo = m_BigListMgr.findBigListInfo( pktHdr->getSrcOnlineId() );
	if( 0 != bigListInfo )
	{
		onlineName = bigListInfo->getOnlineName();
		bigListInfo->setPingTimeMs( timeDiff );
	}
	else
	{
		pktHdr->getSrcOnlineId().toHexString( onlineName );
	}
	
	LogMsg( LOG_DEBUG, "Ping %s Time ms %d", onlineName.c_str(), timeDiff );
}


//============================================================================
void P2PEngine::onPktImAliveReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktImAliveReq" );

	sktBase->setLastImAliveTimeTxMs(  GetGmtTimeMs() );
	PktImAliveReply pktImAliveReply;
	pktImAliveReply.setSrcOnlineId( m_PktAnn.getMyOnlineId() );
	pktImAliveReply.setDestOnlineId( pktHdr->getSrcOnlineId() );

	sktBase->txPacketWithDestId( &pktImAliveReply );
}

//============================================================================
void P2PEngine::onPktImAliveReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktImAliveReply" );

	sktBase->setLastImAliveTimeRxMs(  GetGmtTimeMs() );
}

//============================================================================
void P2PEngine::onPktBlobSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobSendReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktBlobSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobSendReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktBlobChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobChunkReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktBlobChunkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobChunkReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktBlobSendCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobSendCompleteReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktBlobSendCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobSendCompleteReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktBlobXferErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktBlobXferErr" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostJoinReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostJoinReq" );
    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostJoinReply" );
    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostLeaveReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostLeaveReq" );
	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostLeaveReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostLeaveReply" );
	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUnJoinReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUnJoinReq" );
	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUnJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUnJoinReply" );
	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostSearchReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostSearchReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostSearchReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostOfferReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostOfferReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFriendOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFriendOfferReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFriendOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFriendOfferReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbGetReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbGetReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbGetReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbGetReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbSendReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbSendReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbChunkReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbChunkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbChunkReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbGetCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbGetCompleteReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbGetCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbGetCompleteReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbSendCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbSendCompleteReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbSendCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbSendCompleteReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktThumbXferErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktThumbXferErr" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

// offers
//============================================================================
void P2PEngine::onPktOfferSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferSendReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktOfferSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferSendReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktOfferChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferChunkReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktOfferChunkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferChunkReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktOfferSendCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferSendCompleteReq" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktOfferSendCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferSendCompleteReply" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktOfferXferErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktOfferXferErr" );

    m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktPushToTalkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPushToTalkReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktPushToTalkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPushToTalkReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktPushToTalkStart( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPushToTalkStart" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktPushToTalkStop( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktPushToTalkStop" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}


//============================================================================
void P2PEngine::onPktMembershipReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktMembershipReq" );

	VxNetIdent* netIdent = pktHdr->getSrcOnlineId() == getMyOnlineId() ? getMyNetIdent() : m_BigListMgr.findBigListInfo( pktHdr->getSrcOnlineId() );
	if( netIdent && !netIdent->isIgnored() && sktBase && sktBase->isConnected() )
	{
		PktAnnounce pktAnn;
		copyMyPktAnnounce( pktAnn );
		EFriendState myFriendshipToHim = netIdent->getMyFriendshipToHim();
		PktMembershipReply pktReply;
		pktReply.setCanPushToTalk( pktAnn.getPluginPermission( ePluginTypePushToTalk ) != eFriendStateIgnore && myFriendshipToHim >= pktAnn.getPluginPermission( ePluginTypePushToTalk ) );
		pktReply.setHostMembership( eHostTypeNetwork, getMembershipState( pktAnn, netIdent, ePluginTypeHostNetwork, myFriendshipToHim ) );
		pktReply.setHostMembership( eHostTypeConnectTest, getMembershipState( pktAnn, netIdent, ePluginTypeHostConnectTest, myFriendshipToHim ) );
		pktReply.setHostMembership( eHostTypeGroup, getMembershipState( pktAnn, netIdent, ePluginTypeHostGroup, myFriendshipToHim ) );
		pktReply.setHostMembership( eHostTypeChatRoom, getMembershipState( pktAnn, netIdent, ePluginTypeHostChatRoom, myFriendshipToHim ) );
        pktReply.setHostMembership( eHostTypeRandomConnect, getMembershipState( pktAnn, netIdent, ePluginTypeHostRandomConnect, myFriendshipToHim ) );

        sktBase->txPacket( netIdent->getMyOnlineId(), &pktReply );
	}
}

//============================================================================
void P2PEngine::onPktMembershipReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktMembershipReply" );

	PktMembershipReply* pktReply = ( PktMembershipReply* )pktHdr;
	if( pktReply && pktReply->isValidPkt() )
	{
		VxNetIdent* netIdent = pktHdr->getSrcOnlineId() == getMyOnlineId() ? getMyNetIdent() : m_BigListMgr.findBigListInfo( pktHdr->getSrcOnlineId() );
		if( netIdent && !netIdent->isIgnored() && sktBase && sktBase->isConnected() )
		{
			MembershipAvailable membership;
			membership.setCanPushToTalk( pktReply->getCanPushToTalk() );
			membership.setMembershipState( eHostTypeNetwork, pktReply->getHostMembership( eHostTypeNetwork ) );
			membership.setMembershipState( eHostTypeConnectTest, pktReply->getHostMembership( eHostTypeConnectTest ) );
			membership.setMembershipState( eHostTypeGroup, pktReply->getHostMembership( eHostTypeGroup ) );
			membership.setMembershipState( eHostTypeChatRoom, pktReply->getHostMembership( eHostTypeChatRoom ) );
			membership.setMembershipState( eHostTypeRandomConnect, pktReply->getHostMembership( eHostTypeRandomConnect ) );
		}
	}
}

//============================================================================
EMembershipState P2PEngine::getMembershipState( PktAnnounce& myPktAnn, VxNetIdent* netIdent, EPluginType pluginType, EFriendState myFriendshipToHim )
{
	EMembershipState membershipState{ eMembershipStateNone };
	if( myFriendshipToHim == eFriendStateIgnore )
	{
		membershipState = eMembershipStateJoinDenied;
	}
	else if( myPktAnn.getPluginPermission( pluginType ) != eFriendStateIgnore )
	{
		if( ePluginTypeHostGroup == pluginType || ePluginTypeHostChatRoom == pluginType )
		{
			// look up joined state from group manager
			PluginBase* pluginBase = m_PluginMgr.findPlugin( ePluginTypeHostGroup );
			if( pluginBase )
			{
				membershipState = pluginBase->getMembershipState( netIdent );
			}
		}
		else
		{
			if( myFriendshipToHim >= myPktAnn.getPluginPermission( pluginType ) )
			{
				membershipState = eMembershipStateJoined;
			}
			else
			{
				membershipState = eMembershipStateCanBeRequested;
			}
		}
	}
	else
	{
		membershipState = eMembershipStateJoinDenied;
	}

	return membershipState;
}

//============================================================================
void P2PEngine::onPktHostInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInfoReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInfoReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInviteAnnReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInviteAnnReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInviteAnnReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInviteAnnReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInviteSearchReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInviteSearchReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInviteSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInviteSearchReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInviteMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInviteMoreReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostInviteMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostInviteMoreReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieInfoReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieInfoReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieAnnReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieAnnReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieAnnReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieAnnReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieSearchReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieSearchReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieSearchReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieMoreReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktGroupieMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktGroupieMoreReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoInfoReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoInfoReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoAnnReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoAnnReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoAnnReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoAnnReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoSearchReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoSearchReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoSearchReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoMoreReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktFileInfoMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktFileInfoMoreReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserInfoReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserInfoReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserStatusReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserStatusReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserStatusReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserStatusReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserListReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserListReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserListReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserListMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserListMoreReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktHostUserListMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserListMoreReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTestConnTestReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktHostUserListMoreReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTestConnTestReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTestConnTestReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTestConnPingReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTestConnPingReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktTestConnPingReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktTestConnPingReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktQueryHostUrlReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktQueryHostUrlReq" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}

//============================================================================
void P2PEngine::onPktQueryHostUrlReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	LogModule( eLogPkt, LOG_VERBOSE, "P2PEngine::onPktQueryHostUrlReply" );

	m_PluginMgr.handleNonSystemPackets( sktBase, pktHdr );
}
