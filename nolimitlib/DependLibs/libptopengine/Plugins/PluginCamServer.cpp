//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginCamServer.h"
#include "PluginMgr.h"
#include "TxSession.h"
#include "RxSession.h"

#include <libjpg/VxJpgLib.h>
#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>
#include <GuiInterface/IToGui.h>

#include <NetLib/VxSktBase.h>

#include <PktLib/PktsVideoFeed.h>
#include <PktLib/PktsSession.h>
#include <PktLib/PktsPluginOffer.h>

#include <memory.h>

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif //_MSC_VER

//============================================================================
PluginCamServer::PluginCamServer( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBase( engine, pluginMgr, myIdent, pluginType )
, m_PluginSessionMgr( engine, *this, pluginMgr )
, m_VoiceFeedMgr( engine, *this, m_PluginSessionMgr )
, m_VideoFeedMgr( engine, *this, m_PluginSessionMgr )
{
	m_MediaSessionId.initializeWithNewVxGUID();
	m_ePluginType = ePluginTypeCamServer;
}

//============================================================================
void PluginCamServer::setIsPluginInSession( bool isInSession )
{
	setIsServerInSession( isInSession );
	IToGui::getIToGui().toGuiPluginStatus( m_ePluginType, isInSession ? 1 : 0, isInSession ? m_PluginSessionMgr.getSessionCount() : -1 );
}

//============================================================================
// override this by plugin to create inherited RxSession
RxSession * PluginCamServer::createRxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
	return new RxSession( sktBase, onlineId, getPluginType() );
}

//============================================================================
// override this by plugin to create inherited TxSession
TxSession * PluginCamServer::createTxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
	return new TxSession( sktBase, onlineId, getPluginType() );
}

//============================================================================
void PluginCamServer::callbackVideoJpgSmall( VxGUID& feedId, uint8_t * jpgData, uint32_t jpgDataLen, int motion0to100000 )
{
	//LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s session count %d", __func__, m_PluginSessionMgr.getSessionCount() );
	m_PluginMgr.pluginApiPlayVideoFrame( m_ePluginType, jpgData, jpgDataLen, m_MyIdent, motion0to100000 );
}

//============================================================================
void PluginCamServer::sendVidPkt( VxPktHdr* vidPkt, bool requiresAck )
{
	// LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s %d sessions", __func__, m_PluginSessionMgr.getSessionCount() );

	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_PluginSessionMgr.getSessions();
	PluginBase::AutoPluginLock pluginMutexLock( this );
	for( auto sessionPair : sessionList )
	{
		PluginSessionBase* sessionBase = sessionPair.second;
		if( sessionBase->isTxSession() )
		{
			TxSession * poSession = (TxSession *)sessionBase;
			if(  !requiresAck  || (10 > poSession->getOutstandingAckCnt() ) )
			{
				bool sent = m_PluginMgr.pluginApiTxPacket(	m_ePluginType, 
												poSession->getSendToId(), 
												poSession->getSkt(), 
												vidPkt ); 
				if( sent )
				{
					if( requiresAck )
					{
						poSession->setOutstandingAckCnt( poSession->getOutstandingAckCnt() + 1 );
					}
				}
				else
				{
					LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s %s outstanding failed to send", __func__, 
								poSession->getSendToId().toOnlineIdString().c_str() );
				}
			}
			else
			{
				LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s %s outstanding ack count %d", __func__, 
							poSession->getSendToId().toOnlineIdString().c_str(), poSession->getOutstandingAckCnt() );
			}
		}
	}
}

//============================================================================
void PluginCamServer::callbackOpusPkt( PktVoiceReq * pktOpusAudio )
{
	m_VoiceFeedMgr.callbackOpusPkt( pktOpusAudio );
}

//============================================================================
void PluginCamServer::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	m_VoiceFeedMgr.callbackAudioOutSpaceAvail( freeSpaceLen );
}

//============================================================================
void PluginCamServer::callbackVideoPktPic( VxGUID& feedId, PktVideoFeedPic * pktVid, int pktsInSequence, int thisPktNum )
{
	sendVidPkt( pktVid, true );
}

//============================================================================
void PluginCamServer::callbackVideoPktPicChunk( VxGUID& feedId, PktVideoFeedPicChunk * pktVid, int pktsInSequence, int thisPktNum )
{
	sendVidPkt( pktVid, false );
}

//============================================================================
//! called to start service or session with remote friend
void PluginCamServer::fromGuiStartPluginSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	if( onlineId == m_Engine.getMyOnlineId() )
	{
		LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::fromGuiStartPluginSession is ME" );
		enableCamServerService( true );
	}
	else
	{
		LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::fromGuiStartPluginSession is NOT ME" );
		AutoPluginLock pluginMutexLock( this );
		RxSession * rxSession = m_PluginSessionMgr.findRxSessionByOnlineId( onlineId, true );
		if( !rxSession )
		{
			std::shared_ptr<VxSktBase> sktBase = m_Engine.getConnectIdListMgr().findBestUserOnlineConnection( onlineId );
			if( sktBase && sktBase->isConnected() )
			{
				rxSession = m_PluginSessionMgr.findOrCreateRxSessionWithOnlineId( onlineId, sktBase, true, lclSessionId );
			}
		}

		if( rxSession )
		{
			if( lclSessionId.isVxGUIDValid() )
			{
				rxSession->setLclSessionId( lclSessionId );
			}
			else
			{
				lclSessionId = rxSession->getLclSessionId();
			}

			requestCamSession( rxSession, false );

			m_VideoFeedMgr.fromGuiStartPluginSession( true, eAppModuleCamServer, onlineId );
			m_VoiceFeedMgr.fromGuiStartPluginSession( true, eAppModuleCamServer, onlineId );
			setIsPluginInSession( true );
		}
		else
		{
			LogMsg( LOG_INFO, "PluginCamServer::fromGuiStartPluginSession could not connect to %s", m_Engine.describeUser( onlineId ).c_str() );
		}
	}
}

//============================================================================
//! called to stop service or session with remote friend
void PluginCamServer::fromGuiStopPluginSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::fromGuiStopPluginSession" );
	PluginBase::AutoPluginLock pluginMutexLock( this );
	bool isMyself = ( onlineId == m_MyIdent->getMyOnlineId() );
	if( isMyself )
	{
		m_Engine.setHasSharedWebCam(false);
		m_VoiceFeedMgr.fromGuiStopPluginSession( true, eAppModuleCamServer, onlineId );
		// don't want video capture anymore
		m_VideoFeedMgr.fromGuiStopPluginSession( true, eAppModuleCamServer, onlineId );
		if( true == fromGuiIsPluginInSession() )
		{
			setIsPluginInSession(false);

			PktVideoFeedStatus oPkt;
			oPkt.setFeedStatus( eFeedStatusOffline );

			std::map<VxGUID, PluginSessionBase*>&	sessionList = m_PluginSessionMgr.getSessions();
			for( auto iter = sessionList.begin(); iter != sessionList.end(); )
			{
				PluginSessionBase* sessionBase = iter->second;
				if( sessionBase->isTxSession() )
				{
					PluginSessionBase* sessionBase = iter->second;
					if( sessionBase->isTxSession() )
					{
						TxSession * poSession = (TxSession *)sessionBase;
						if( poSession->getSkt() )
						{
							oPkt.setLclSessionId( poSession->getLclSessionId() );
							oPkt.setRmtSessionId( poSession->getRmtSessionId() );
							m_PluginMgr.pluginApiTxPacket( m_ePluginType, poSession->getSendToId(), poSession->getSkt(), &oPkt );
							iter = sessionList.erase( iter );
							delete poSession;
							break;
						}
						else
						{
							++iter;
						}
					}
					else
					{
						++iter;
					}
				}
			}
		}
	}
	else
	{
		m_VoiceFeedMgr.fromGuiStopPluginSession( true, eAppModuleCamServer, onlineId );
		PktSessionStopReq oPkt;

		RxSession * poSession = (RxSession *)m_PluginSessionMgr.findRxSessionByOnlineId( onlineId, true );
		if( poSession )
		{
			oPkt.setLclSessionId( poSession->getLclSessionId() );
			oPkt.setRmtSessionId( poSession->getRmtSessionId() );
			m_PluginMgr.pluginApiTxPacket( m_ePluginType, poSession->getSendToId(), poSession->getSkt(), &oPkt );
		}

		m_PluginSessionMgr.removeRxSessionByOnlineId( onlineId, true );
	}
}

//============================================================================
bool PluginCamServer::fromGuiIsPluginInSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	// for cam server we really want to know if server is running
	return getIsServerInSession();
}

//============================================================================
EPluginAccess PluginCamServer::canAcceptNewSession( VxNetIdent* netIdent )
{
	EFriendState eHisPermissionToMe = netIdent->getHisFriendshipToMe();
	EFriendState eMyPermissionToHim = netIdent->getMyFriendshipToHim();

	if( (eFriendStateIgnore == eHisPermissionToMe) ||
		(eFriendStateIgnore == eMyPermissionToHim) )
	{
		return ePluginAccessIgnored;
	}
	EFriendState ePermissionLevel = this->m_MyIdent->getPluginPermission(m_ePluginType);
	if( eFriendStateIgnore == ePermissionLevel )
	{
		return ePluginAccessDisabled;
	}
	if( ePermissionLevel > eMyPermissionToHim )
	{
		return ePluginAccessLocked;
	}

	return ePluginAccessOk;
}

//============================================================================
bool PluginCamServer::fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	std::shared_ptr<VxSktBase> sktBase = m_Engine.getConnectIdListMgr().findBestUserOnlineConnection( onlineId );
	if( !sktBase || !sktBase->isConnected() )
	{
		return false;
	}

	VxGUID& lclSessionId = offerInfo.getOfferId();
	LogModule( eLogWebCam, LOG_INFO, " PluginCamServer::%s %s", __func__, m_Engine.describeUser( onlineId ).c_str() );
	PluginBase::AutoPluginLock pluginMutexLock( this );

	PktPluginOfferReq pktReq;
	pktReq.setPluginType( getPluginType() );
	pktReq.setLclSessionId( lclSessionId );
	pktReq.calcPktLen();
    // force session to be created so have session to lookup on reply
	RxSession * rxSession = (RxSession *)m_PluginSessionMgr.findOrCreateRxSessionWithSessionId( lclSessionId, sktBase, onlineId, true );
    if( rxSession )
    {
        if( true == m_PluginMgr.pluginApiTxPacket(	m_ePluginType,
                                                    onlineId,
                                                    sktBase,
                                                    &pktReq ) )
        {
            LogMsg( LOG_INFO, " PluginCamServer::fromGuiMakePluginOffer success");
            return true;
        }
        else
        {
            LogMsg( LOG_INFO, " PluginCamServer::fromGuiMakePluginOffer failed to send pkt");
        }
    }
    else
    {
        LogMsg( LOG_ERROR, " PluginCamServer::fromGuiMakePluginOffer failed to create session");
    }


	return false;
}

//============================================================================
bool PluginCamServer::requestCamSession( RxSession* rxSession, bool	bWaitForSuccess )
{
	PktSessionStartReq pktReq;
	pktReq.setLclSessionId( rxSession->getLclSessionId() );
	bool bSuccess = m_PluginMgr.pluginApiTxPacket(	m_ePluginType, 
													rxSession->getSendToId(), 
													rxSession->getSkt(), 
													&pktReq );
	if( ( true == bSuccess ) && bWaitForSuccess )
	{
		bSuccess = false;
		bool bResponseReceived = rxSession->waitForResponse( 9000 );
		if( bResponseReceived )
		{
			if( rxSession->getIsSessionStarted() )
			{
				bSuccess = true;
			}
		}
	}
	
	return bSuccess;
}

//============================================================================
bool PluginCamServer::stopCamSession( VxNetIdent* netIdent,	std::shared_ptr<VxSktBase>& sktBase )
{
	LogModule( eLogWebCam, LOG_VERBOSE, "PluginCamServer::stopCamSession");
	PktSessionStopReq pktReq;
	bool bSuccess = m_PluginMgr.pluginApiTxPacket(	m_ePluginType, 
													netIdent->getMyOnlineId(), 
													sktBase, 
													&pktReq );
	m_PluginSessionMgr.removeRxSessionByOnlineId( netIdent->getMyOnlineId(), false );

	return bSuccess;
}

//============================================================================
//! packet with remote users offer
void PluginCamServer::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	if( !isAccessAllowed( netIdent, true, "onPktPluginOfferReq" ) )
	{
		return;
	}

	LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::onPktPluginOfferReq" );
	PktPluginOfferReq * pktOfferReq = ( PktPluginOfferReq * )pktHdr;
	OfferBaseInfo offerInfo;
	if( offerInfo.extractFromBlob( pktOfferReq->getBlobEntry() ) )
	{
		PktPluginOfferReply pktReply;
		pktReply.setLclSessionId( pktOfferReq->getLclSessionId() );
		pktReply.setRmtSessionId( pktOfferReq->getRmtSessionId() );
		pktReply.setPluginType( getPluginType() );
		offerInfo.addToBlob( pktReply.getBlobEntry() );
		pktReply.calcPktLen();

		PluginBase::AutoPluginLock pluginMutexLock( this );
		if( getIsServerInSession() && (ePluginAccessOk == canAcceptNewSession( netIdent )) )
		{
			TxSession* txSession = (TxSession*)m_PluginSessionMgr.findOrCreateTxSessionWithOnlineId( netIdent->getMyOnlineId(), sktBase, true );
			pktReply.setLclSessionId( txSession->getLclSessionId() );
			pktReply.setOfferResponse( eOfferResponseAccept );
			if( !m_RequestedVidPkts )
			{
				m_RequestedVidPkts = true;
				m_Engine.getMediaProcessor().wantMediaInput( m_Engine.getMyOnlineId(), eMediaInputVideoPkts, this, eAppModuleCamServer, netIdent->getMyOnlineId(), true );
			}
		}
		else
		{
			LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::onPktPluginOfferReq REJECTED in session %d canAcceptNewSession %d",
				getIsServerInSession(), canAcceptNewSession( netIdent ) );
			pktReply.setOfferResponse( eOfferResponseReject );
		}

		m_PluginMgr.pluginApiTxPacket( m_ePluginType,
			netIdent->getMyOnlineId(),
			sktBase,
			&pktReply );
	}
	else
	{
		LogMsg( LOG_ERROR, "PluginCamServer::onPktPluginOfferReq failed extract blob" );
	}
}

//============================================================================
//! packet with remote users reply to offer
void PluginCamServer::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktPluginOfferReply* pktReply = (PktPluginOfferReply*)pktHdr;
	EOfferResponse offerResponse = pktReply->getOfferResponse();

	LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s from %s is %s", __func__, netIdent->getOnlineName(), DescribeOfferResponse( offerResponse ) );

	OfferBaseInfo offerInfo;
	if( offerInfo.extractFromBlob( pktReply->getBlobEntry() ) )
	{
		IToGui::getIToGui().toGuiRxedOfferReply( pktReply->getSrcOnlineId(), offerInfo);
	}

	PluginBase::AutoPluginLock pluginMutexLock( this );
	if( offerResponse == eOfferResponseAccept )
	{
		RxSession * rxSession = (RxSession *)m_PluginSessionMgr.findOrCreateRxSessionWithOnlineId( pktReply->getSrcOnlineId(), sktBase, true );
		rxSession->setOfferResponse( offerResponse );
	}
}

//============================================================================
void PluginCamServer::onPktSessionStartReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	if( !isAccessAllowed( netIdent, true, "onPktSessionStartReq" ) )
	{
		LogModule( eLogWebCam, LOG_DEBUG, "PluginCamServer::%s access not allowed from %s", __func__, netIdent->getOnlineName() );
		return;
	}

	LogModule( eLogWebCam, LOG_VERBOSE, "PluginCamServer::%s from %s", __func__, netIdent->getOnlineName() );
	PktSessionStartReply pktReply;
	PluginBase::AutoPluginLock pluginMutexLock( this );
	if( getIsServerInSession() && ( ePluginAccessOk == canAcceptNewSession( netIdent ) ) ) 
	{
		pktReply.setOfferResponse(eOfferResponseAccept);
        TxSession * txSession = (TxSession *)m_PluginSessionMgr.findOrCreateTxSessionWithOnlineId( netIdent->getMyOnlineId(), sktBase, true );
        if( 0 == txSession )
        {
            LogMsg( LOG_ERROR, "PluginCamServer::%s failed to create or find session", __func__ );
            pktReply.setOfferResponse( eOfferResponseReject );
        }
		else
		{
			IToGui::getIToGui().toGuiPluginStatus( m_ePluginType, 1, m_PluginSessionMgr.getTxSessionCount( true ) );
			if( !m_RequestedVidPkts )
			{
				m_RequestedVidPkts = true;
				m_Engine.getMediaProcessor().wantMediaInput( m_Engine.getMyOnlineId(), eMediaInputVideoPkts, this, eAppModuleCamServer, m_MediaSessionId, true );
			}
		}
	}
	else
	{
		pktReply.setOfferResponse( eOfferResponseReject );
	}

	bool sent = m_PluginMgr.pluginApiTxPacket(	m_ePluginType, 
												netIdent->getMyOnlineId(), 
												sktBase, 
												&pktReply ); 
	if( !sent )
	{
		LogModule( eLogWebCam, LOG_VERBOSE, "PluginCamServer::%s failed send to %s", __func__, netIdent->getOnlineName() );
	}
}

//============================================================================
void PluginCamServer::onPktSessionStartReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktSessionStartReply * poPkt = (PktSessionStartReply *)pktHdr;
	PluginBase::AutoPluginLock pluginMutexLock( this );
	RxSession * poSession = (RxSession *)m_PluginSessionMgr.findRxSessionByOnlineId( netIdent->getMyOnlineId(), true );
	if( poSession )
	{
		LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::onPktSessionStartReply %d", poPkt->getOfferResponse() );
		if( eOfferResponseAccept == poPkt->getOfferResponse() )
		{
			poSession->setIsSessionStarted( true );
		}
		else
		{
			poSession->setIsSessionStarted( false );
		}

		poSession->signalResponseRecieved();
	}
}

//============================================================================
void PluginCamServer::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.removeTxSessionByOnlineId( netIdent->getMyOnlineId(), false );
	updateTxSessionCount();
}

//============================================================================
void PluginCamServer::onPktSessionStopReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogModule( eLogWebCam, LOG_ERROR, "PluginCamServer::onPktSessionStopReply" );
}

//============================================================================
void PluginCamServer::onPktVideoFeedReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
}

//============================================================================
void PluginCamServer::onPktVideoFeedStatus( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktVideoFeedStatus * pktVideoStatus = ( PktVideoFeedStatus * )pktHdr;
	PluginBase::AutoPluginLock pluginMutexLock( this );
	TxSession * txSession = (TxSession *)m_PluginSessionMgr.findTxSessionByOnlineId( true, netIdent->getMyOnlineId() );
	if( txSession )
	{
		LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s from %s %d", __func__, netIdent->getOnlineName(), pktVideoStatus->getFeedStatus() );
		if( eFeedStatusOnline != pktVideoStatus->getFeedStatus() )
		{
			m_PluginSessionMgr.endPluginSession( netIdent->getMyOnlineId(), true );
			m_PluginSessionMgr.removeTxSessionByOnlineId( netIdent->getMyOnlineId(), true );
			if( getIsServerInSession() )
			{
				IToGui::getIToGui().toGuiPluginStatus( m_ePluginType, 1, m_PluginSessionMgr.getTxSessionCount( true ) );
			}
		}
	}

	RxSession* rxSession = (RxSession*)m_PluginSessionMgr.findRxSessionByOnlineId( netIdent->getMyOnlineId(), true );
	if( rxSession )
	{
		LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::%s from %s %d", __func__, netIdent->getOnlineName(), pktVideoStatus->getFeedStatus() );
		if( eFeedStatusOnline != pktVideoStatus->getFeedStatus() )
		{
			//IToGui::getIToGui().toGuiRxedOfferReply( netIdent,
			//	m_ePluginType,
			//	0,				// plugin defined data
			//	(eFeedStatusBusy == pktVideoStatus->getFeedStatus()) ? eOfferResponseBusy : eOfferResponseEndSession,
			//	0,
			//	0,
			//	pktVideoStatus->getRmtSessionId(),
			//	pktVideoStatus->getLclSessionId() );

			m_PluginSessionMgr.endPluginSession( netIdent->getMyOnlineId(), true );
			m_PluginSessionMgr.removeRxSessionByOnlineId( netIdent->getMyOnlineId(), true );
		}
	}
}

//============================================================================
void PluginCamServer::onPktVideoFeedPic( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogModule( eLogWebCam, LOG_VERBOSE, "PluginCamServer::%s from %s", __func__, netIdent->getOnlineName() );
	PktVideoFeedPicAck oPkt;
	m_PluginMgr.pluginApiTxPacket( m_ePluginType, netIdent->getMyOnlineId(), sktBase, &oPkt ); 

	PktVideoFeedPic * pktVideoFeedPic = ( PktVideoFeedPic * )pktHdr;
	if( pktVideoFeedPic->getTotalDataLen() == pktVideoFeedPic->getThisDataLen() )
	{
		m_Engine.getMediaProcessor().processFriendVideoFeed( netIdent->getMyOnlineId(), pktVideoFeedPic->getDataPayload(),
															 pktVideoFeedPic->getTotalDataLen(), pktVideoFeedPic->getMotionDetect() );

		PluginBase::AutoPluginLock pluginMutexLock( this );
		RxSession * poSession = (RxSession *)m_PluginSessionMgr.findRxSessionByOnlineId( netIdent->getMyOnlineId(), true );
		if( poSession )
		{
			if( poSession->getVideoFeedPkt() )
			{
				delete poSession->getVideoFeedPkt();
				poSession->setVideoFeedPkt( NULL );
			}
		}
	}
	else
	{
		// picture was too big for one packet
		PluginBase::AutoPluginLock pluginMutexLock( this );

		RxSession * poSession = (RxSession *)m_PluginSessionMgr.findRxSessionByOnlineId( netIdent->getMyOnlineId(), true );
		if( poSession )
		{
			if( poSession->getVideoFeedPkt() )
			{
				delete poSession->getVideoFeedPkt();
				poSession->setVideoFeedPkt( NULL );
			}
		}
		else
		{
			poSession = (RxSession *)m_PluginSessionMgr.findOrCreateRxSessionWithOnlineId( netIdent->getMyOnlineId(), sktBase, true );
			LogModule( eLogWebCam, LOG_INFO, "PluginCamServer::onPktVideoFeedPic: creating rx session because could not be found");
		}

		PktVideoFeedPic * poPic = ( PktVideoFeedPic * ) new char[ sizeof( PktVideoFeedPic ) + 16 + pktVideoFeedPic->getTotalDataLen() ];
		poPic->setThisDataLen( pktVideoFeedPic->getThisDataLen() );
		memcpy( poPic, pktVideoFeedPic, pktVideoFeedPic->getPktLength() );
		poSession->setVideoFeedPkt( poPic );

		// wait for rest of picture to arrive
	}
}

//============================================================================
void PluginCamServer::onPktVideoFeedPicChunk( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktVideoFeedPicChunk * poPktPicChunk = ( PktVideoFeedPicChunk * )pktHdr;

	PluginBase::AutoPluginLock pluginMutexLock( this );

	RxSession * poSession = (RxSession *)m_PluginSessionMgr.findRxSessionByOnlineId( netIdent->getMyOnlineId(), true );
	if( !poSession )
	{
		// this may occur normally with dropped frames due to slow acknowledgment
		//LogMsg( LOG_ERROR, "PluginCamServer::onPktVideoFeedPicChunk: could not find RxSession\n");
		return;
	}

	PktVideoFeedPic * poPktCastPic = poSession->getVideoFeedPkt();
	if( poPktCastPic && 
		(poPktCastPic->getTotalDataLen() >=  (poPktCastPic->getThisDataLen() + poPktPicChunk->getThisDataLen() ) ) )
	{
		memcpy( &poPktCastPic->getDataPayload()[ poPktCastPic->getThisDataLen() ], 
				poPktPicChunk->getDataPayload(), 
				poPktPicChunk->getThisDataLen() );
		poPktCastPic->setThisDataLen( poPktCastPic->getThisDataLen() + poPktPicChunk->getThisDataLen() );
		if( poPktCastPic->getThisDataLen() >= poPktCastPic->getTotalDataLen() )
		{
			// all of picture arrived
			PktVideoFeedPicAck oPkt;
			m_PluginMgr.pluginApiTxPacket(	m_ePluginType, 
				netIdent->getMyOnlineId(), 
				sktBase, 
				&oPkt ); 

			m_PluginMgr.pluginApiPlayVideoFrame(	m_ePluginType, 
				poPktCastPic->getDataPayload(), 
				poPktCastPic->getTotalDataLen(), 
				netIdent,
				poPktCastPic->getMotionDetect() );
			
			delete poSession->getVideoFeedPkt();
			poSession->setVideoFeedPkt( NULL );
		}
	}
}

//============================================================================
void PluginCamServer::onPktVideoFeedPicAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PluginBase::AutoPluginLock pluginMutexLock( this );

	TxSession * txSession = (TxSession *)m_PluginSessionMgr.findTxSessionByOnlineId( true, netIdent->getMyOnlineId() );
	if( !txSession )
	{
		LogModule( eLogWebCam, LOG_ERROR, "PluginCamServer::onPktVideoFeedPicAck: could not find TxSession");
		return;
	}

	txSession->decrementOutstandingAckCnt();
}

//============================================================================
void PluginCamServer::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginCamServer::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginCamServer::replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt )
{
	m_PluginSessionMgr.replaceConnection( netIdent, poOldSkt, poNewSkt );
}

//============================================================================
void PluginCamServer::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )
{
	m_PluginSessionMgr.onConnectionLost( sktBase );
	updateTxSessionCount();
}

//============================================================================
void PluginCamServer::onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
	m_PluginSessionMgr.onContactWentOffline( netIdent, sktBase );
	updateTxSessionCount();
}

//============================================================================
void PluginCamServer::onNetworkConnectionReady( bool requiresRelay )
{
	if( isPluginEnabled() )
	{
		// automatically start web cam on startup if enabled
		enableCamServerService( true );
	}
}

//============================================================================
void PluginCamServer::fromGuiUpdatePluginPermission( EPluginType pluginType, EFriendState pluginPermission )
{
	if( pluginType == getPluginType() )
	{
		enableCamServerService( eFriendStateIgnore != pluginPermission );
	}
}

//============================================================================
void PluginCamServer::stopAllSessions( void )
{
	if( true == fromGuiIsPluginInSession() )
	{
		setIsPluginInSession( false );

		// tell everyone we are no longer online
		PktVideoFeedStatus oPkt;
		oPkt.setFeedStatus( eFeedStatusOffline );

		std::map<VxGUID, PluginSessionBase*>& sessionList = m_PluginSessionMgr.getSessions();
		for( auto iter = sessionList.begin(); iter != sessionList.end(); )
		{
			PluginSessionBase* sessionBase = iter->second;
			if( sessionBase->isTxSession() )
			{
				TxSession* poSession = (TxSession*)sessionBase;
				if( poSession->getSkt() )
				{
					oPkt.setLclSessionId( poSession->getLclSessionId() );
					oPkt.setRmtSessionId( poSession->getRmtSessionId() );
					m_PluginMgr.pluginApiTxPacket( m_ePluginType, poSession->getSendToId(), poSession->getSkt(), &oPkt );
					iter = sessionList.erase( iter );
					delete poSession;
				}
				else
				{
					++iter;
				}
			}
			else
			{
				++iter;
			}
		}
	}

	m_VideoFeedMgr.stopAllSessions( eAppModuleCamServer, getPluginType() );
	m_VoiceFeedMgr.stopAllSessions( eAppModuleCamServer, getPluginType() );
	updateTxSessionCount();
}

//============================================================================
void PluginCamServer::enableCamServerService( bool enable )
{
	if( m_IsCamServiceEnabled == enable )
	{
		return;
	}

	m_IsCamServiceEnabled = enable;

	if( m_IsCamServiceEnabled )
	{
		m_Engine.setHasSharedWebCam( true );
		// request video capture
		m_VideoFeedMgr.fromGuiStartPluginSession( false, eAppModuleCamServer, m_Engine.getMyOnlineId() );
		m_VoiceFeedMgr.fromGuiStartPluginSession( false, eAppModuleCamServer, m_Engine.getMyOnlineId() );
		setIsPluginInSession( true );
	}
	else
	{
		// stop video capture
		m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleCamServer, m_Engine.getMyOnlineId() );
		m_VoiceFeedMgr.fromGuiStopPluginSession( false, eAppModuleCamServer, m_Engine.getMyOnlineId() );
		m_Engine.setHasSharedWebCam( false );
		stopAllSessions();
		setIsPluginInSession( false );
	}
}

//============================================================================
void PluginCamServer::updateTxSessionCount( void )
{
	int txSessionCnt = m_PluginSessionMgr.getTxSessionCount();
	if( getIsServerInSession() )
	{
		IToGui::getIToGui().toGuiPluginStatus( m_ePluginType, 1, txSessionCnt );
	}

	if( 0 == txSessionCnt && m_RequestedVidPkts )
	{
		m_RequestedVidPkts = false;
		m_Engine.getMediaProcessor().wantMediaInput( m_Engine.getMyOnlineId(), eMediaInputVideoPkts, this, eAppModuleCamServer, m_MediaSessionId, false );
	}
}
