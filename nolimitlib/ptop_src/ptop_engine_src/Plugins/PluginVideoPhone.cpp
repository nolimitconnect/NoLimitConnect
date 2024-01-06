//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginVideoPhone.h"
#include "P2PSession.h"
#include "PluginMgr.h"

#include <PktLib/PktsVideoFeed.h>
#include <PktLib/PktChatReq.h>

#include <GuiInterface/IToGui.h>
#include <P2PEngine/P2PEngine.h>

#include <memory.h>

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif //_MSC_VER

//#define DEBUG_AUTOPLUGIN_LOCK 1

//============================================================================
PluginVideoPhone::PluginVideoPhone( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBase( engine, pluginMgr, myIdent, pluginType )
, m_PluginSessionMgr( engine, *this, pluginMgr )
, m_VoiceFeedMgr( engine, *this, m_PluginSessionMgr )
, m_VideoFeedMgr( engine, *this, m_PluginSessionMgr )
{
	m_ePluginType = ePluginTypeVideoPhone;
}

//============================================================================
bool PluginVideoPhone::fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	bool result = false;
	P2PSession* poSession = nullptr;
	VxGUID& lclSessionId = offerInfo.getOfferId();
	PluginBase::AutoPluginLock pluginMutexLock( this );
	if( lclSessionId.isVxGUIDValid() )
	{
		poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionBySessionId( lclSessionId, true );
		if( poSession )
		{
			#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_ERROR, "PluginVideoPhone already in session");
			#endif // DEBUG_AUTOPLUGIN_LOCK
			// assume some error in logic
			m_PluginSessionMgr.removeSessionBySessionId( true, lclSessionId );
		}
	}
	else
	{
		poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionByOnlineId(onlineId, true );
		if( poSession )
		{
#ifdef DEBUG_AUTOPLUGIN_LOCK
			LogMsg( LOG_ERROR, "PluginVideoPhone already in session");
#endif // DEBUG_AUTOPLUGIN_LOCK
			// assume some error in logic
			m_PluginSessionMgr.removeRxSessionByOnlineId( onlineId, true );
		}
	}

		
	result = m_PluginSessionMgr.fromGuiMakePluginOffer(	true, onlineId, offerInfo );

	return result;
}

//============================================================================
bool PluginVideoPhone::fromGuiOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::fromGuiOfferReply %d", offerResponse );
#endif // DEBUG_AUTOPLUGIN_LOCK
	return m_PluginSessionMgr.fromGuiOfferReply( false, onlineId, offerInfo );
}

//============================================================================
bool PluginVideoPhone::fromGuiIsPluginInSession( VxGUID& onlineId, int pvUserData, VxGUID lclSessionId )
{
	return m_PluginSessionMgr.fromGuiIsPluginInSession( false, onlineId, pvUserData, lclSessionId );
}

//============================================================================
void PluginVideoPhone::fromGuiStartPluginSession( VxGUID& onlineId, int pvUserData, VxGUID lclSessionId )
{
	m_VoiceFeedMgr.fromGuiStartPluginSession( false, eAppModuleVideoPhone, onlineId );
	m_VideoFeedMgr.fromGuiStartPluginSession( false, eAppModuleVideoPhone, getEngine().getMyOnlineId() );
}

//============================================================================
void PluginVideoPhone::fromGuiStopPluginSession( VxGUID& onlineId, int pvUserData, VxGUID lclSessionId )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::fromGuiStopPluginSession %s start", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	m_VoiceFeedMgr.fromGuiStopPluginSession( false, eAppModuleVideoPhone, onlineId );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleVideoPhone, onlineId );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleVideoPhone, getEngine().getMyOnlineId() );
	m_PluginSessionMgr.fromGuiStopPluginSession( false, onlineId, pvUserData, lclSessionId );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::fromGuiStopPluginSession %s done", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
bool PluginVideoPhone::fromGuiInstMsg( VxGUID& onlineId, 
										const char*	pMsg )
{
	PluginBase::AutoPluginLock pluginMutexLock( this );
	P2PSession* poSession = m_PluginSessionMgr.findP2PSessionByOnlineId( onlineId, true );
	if( poSession )
	{
		PktChatReq oPkt;
		oPkt.addMsg( pMsg );
		return m_PluginMgr.pluginApiTxPacket( m_ePluginType, onlineId, poSession->getSkt(), &oPkt );
	}
	else
	{
		LogMsg( LOG_ERROR, "PluginVideoPhone::fromGuiInstMsg session not found" );
		return false;
	}

	return false;
}

//============================================================================
void PluginVideoPhone::onPktChatReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktChatReq * pkt = (PktChatReq *)pktHdr;
	IToGui::getToGui().toGuiInstMsg( netIdent, m_ePluginType, (const char*)pkt->getDataPayload() );
}

//============================================================================
void PluginVideoPhone::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onPktPluginOfferReq %s start", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	m_PluginSessionMgr.onPktPluginOfferReq( sktBase, pktHdr, netIdent );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onPktPluginOfferReq %s done", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PluginVideoPhone::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onPktPluginOfferReply %s start", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	m_PluginSessionMgr.onPktPluginOfferReply( sktBase, pktHdr, netIdent );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onPktPluginOfferReply %s done", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PluginVideoPhone::onPktVideoFeedReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	//LogMsg( LOG_INFO, "PluginVideoPhone::onPktVideoFeedReq %s start\n", netIdent->getOnlineName() );
	m_VideoFeedMgr.onPktVideoFeedReq( sktBase, pktHdr, netIdent );
	//LogMsg( LOG_INFO, "PluginVideoPhone::onPktVideoFeedReq %s done\n", netIdent->getOnlineName() );
}

//============================================================================
void PluginVideoPhone::onPktVideoFeedStatus( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedStatus( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVideoPhone::onPktVideoFeedPic( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	//LogMsg( LOG_INFO, "PluginVideoPhone::onPktVideoFeedPic %s start\n", netIdent->getOnlineName() );
	m_VideoFeedMgr.onPktVideoFeedPic( sktBase, pktHdr, netIdent );
	//LogMsg( LOG_INFO, "PluginVideoPhone::onPktVideoFeedPic %s done\n", netIdent->getOnlineName() );
}

//============================================================================
void PluginVideoPhone::onPktVideoFeedPicChunk( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedPicChunk( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVideoPhone::onPktVideoFeedPicAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedPicAck( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVideoPhone::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onPktSessionStopReq %s start", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	m_PluginSessionMgr.onPktSessionStopReq( sktBase, pktHdr, netIdent );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onPktSessionStopReq %s done", netIdent->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PluginVideoPhone::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	//LogMsg( LOG_INFO, "PluginVideoPhone::onPktVoiceReq %s start\n", netIdent->getOnlineName() );
	m_VoiceFeedMgr.onPktVoiceReq( sktBase, pktHdr, netIdent );
	//LogMsg( LOG_INFO, "PluginVideoPhone::onPktVoiceReq %s done\n", netIdent->getOnlineName() );
}

//============================================================================
void PluginVideoPhone::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVideoPhone::callbackOpusPkt( PktVoiceReq * pktOpusAudio )
{
	m_VoiceFeedMgr.callbackOpusPkt( pktOpusAudio );
}

//============================================================================
void PluginVideoPhone::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	m_VoiceFeedMgr.callbackAudioOutSpaceAvail( freeSpaceLen );
}

//============================================================================
void PluginVideoPhone::callbackVideoPktPic( VxGUID& onlineId, PktVideoFeedPic * pktVid, int pktsInSequence, int thisPktNum )
{
	m_VideoFeedMgr.callbackVideoPktPic( onlineId, pktVid, pktsInSequence, thisPktNum );
}

//============================================================================
void PluginVideoPhone::callbackVideoPktPicChunk( VxGUID& onlineId, PktVideoFeedPicChunk * pktVid, int pktsInSequence, int thisPktNum )
{
	m_VideoFeedMgr.callbackVideoPktPicChunk( onlineId, pktVid, pktsInSequence, thisPktNum );
}

//============================================================================
void PluginVideoPhone::onSessionStart( PluginSessionBase* session, bool pluginIsLocked )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onSessionStart %s start", session->getSendToId()->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::onSessionStart( session, pluginIsLocked );; // mark user session time so contact list is sorted with latest used on top
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onSessionStart %s starting voice feed", session->getSendToId()->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	m_VoiceFeedMgr.fromGuiStartPluginSession( pluginIsLocked, eAppModuleVideoPhone, session->getSendToId() );
	// in order to get my video packets to send out the ident has to be myself
	m_VideoFeedMgr.fromGuiStartPluginSession( pluginIsLocked, eAppModuleVideoPhone, getEngine().getMyOnlineId() );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onSessionStart %s done\n", session->getSendToId()->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PluginVideoPhone::onSessionEnded( PluginSessionBase* session, bool pluginIsLocked, EOfferResponse offerResponse )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onSessionStart %s STOP voice feed", session->getSendToId()->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
	m_VoiceFeedMgr.fromGuiStopPluginSession( pluginIsLocked, eAppModuleVideoPhone, session->getSendToId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( pluginIsLocked, eAppModuleVideoPhone, session->getSendToId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( pluginIsLocked, eAppModuleVideoPhone, getEngine().getMyOnlineId() );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_INFO, "PluginVideoPhone::onSessionStart %s done", session->getSendToId()->getOnlineName() );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PluginVideoPhone::replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt )
{
	m_PluginSessionMgr.replaceConnection( netIdent, poOldSkt, poNewSkt );
}

//============================================================================
void PluginVideoPhone::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )
{
	m_PluginSessionMgr.onConnectionLost( sktBase );
}

//============================================================================
void PluginVideoPhone::onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
	m_VoiceFeedMgr.fromGuiStopPluginSession( false, eAppModuleVideoPhone, netIdent->getMyOnlineId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleVideoPhone, netIdent->getMyOnlineId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleVideoPhone, getEngine().getMyOnlineId() );
	m_PluginSessionMgr.onContactWentOffline( netIdent, sktBase );
}
