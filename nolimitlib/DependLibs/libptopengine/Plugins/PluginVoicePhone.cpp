//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginVoicePhone.h"
#include "PluginMgr.h"

#include "P2PSession.h"

#include <GuiInterface/IToGui.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <PktLib/PktVoiceReq.h>
#include <PktLib/PktVoiceReply.h>
#include <PktLib/PktChatReq.h>

#include <memory.h>
#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif

//============================================================================
PluginVoicePhone::PluginVoicePhone( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBase( engine, pluginMgr, myIdent, pluginType )
, m_PluginSessionMgr( engine, *this, pluginMgr )
, m_VoiceFeedMgr( engine, *this, m_PluginSessionMgr )
{
	m_ePluginType = ePluginTypeVoicePhone;
}

//============================================================================
//! user wants to send offer to friend.. return false if cannot connect
bool PluginVoicePhone::fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	P2PSession* poSession = nullptr;
	VxGUID& lclSessionId = offerInfo.getOfferId();
	PluginBase::AutoPluginLock pluginMutexLock( this );
	if( lclSessionId.isVxGUIDValid() )
	{
		poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionBySessionId( lclSessionId, true  );
	}
	else
	{
		poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionByOnlineId( onlineId, true );
	}

	if( poSession )
	{
		LogMsg( LOG_ERROR, "PluginVoicePhone already in session");
		// assume some error in logic
		m_PluginSessionMgr.removeSessionBySessionId( true, onlineId );
	}

	return m_PluginSessionMgr.fromGuiMakePluginOffer( true, onlineId, offerInfo );
}

//============================================================================
bool PluginVoicePhone::fromGuiIsPluginInSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	return m_PluginSessionMgr.fromGuiIsPluginInSession( false, onlineId, lclSessionId );
}

//============================================================================
//! called to start service or session with remote friend
bool PluginVoicePhone::fromGuiStartPluginSession( VxGUID& onlineId, VxGUID )
{
	m_VoiceFeedMgr.enableAudioCapture( true, onlineId );
	m_VoiceFeedMgr.enableAudioReceive( true, onlineId );
	return true;
}

//============================================================================
//! called to stop service or session with remote friend
void PluginVoicePhone::fromGuiStopPluginSession( VxGUID& onlineId, VxGUID )
{
	m_VoiceFeedMgr.enableAudioCapture( false, onlineId );
	m_VoiceFeedMgr.enableAudioReceive( false, onlineId );
	m_PluginSessionMgr.fromGuiStopPluginSession( false, onlineId );
}

//============================================================================
//! handle reply to offer
bool PluginVoicePhone::fromGuiOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	return m_PluginSessionMgr.fromGuiOfferReply( false, onlineId, offerInfo );
}

//============================================================================
bool PluginVoicePhone::fromGuiInstMsg( VxGUID& onlineId, const char* pMsg )
{
	PluginBase::AutoPluginLock pluginMutexLock( this );
	P2PSession* poSession = m_PluginSessionMgr.findP2PSessionByOnlineId( onlineId, true );
	if( poSession )
	{
		PktChatReq oPkt;
		oPkt.addMsg( pMsg );
		return m_PluginMgr.pluginApiTxPacket( m_ePluginType, onlineId, poSession->getSkt(), &oPkt );
	}

	return false;
}

//============================================================================
void PluginVoicePhone::callbackOpusPkt( PktVoiceReq * pktOpusAudio )
{
	m_VoiceFeedMgr.callbackOpusPkt( pktOpusAudio );
}

//============================================================================
void PluginVoicePhone::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	m_VoiceFeedMgr.callbackAudioOutSpaceAvail( freeSpaceLen );
}

//============================================================================
void PluginVoicePhone::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.onPktPluginOfferReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVoicePhone::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.onPktPluginOfferReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVoicePhone::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.onPktSessionStopReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVoicePhone::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVoicePhone::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginVoicePhone::onPktChatReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktChatReq* poPkt = (PktChatReq *)pktHdr;
	PluginBase::AutoPluginLock pluginMutexLock( this );
	P2PSession* poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionByOnlineId( netIdent->getMyOnlineId(), true );
	if( poSession )
	{
		IToGui::getIToGui().toGuiInstMsg( netIdent, m_ePluginType, (const char*)poPkt->getDataPayload() );
	}
}

//============================================================================
void PluginVoicePhone::onSessionStart( PluginSessionBase* session, bool pluginIsLocked )
{
	PluginBase::onSessionStart( session, pluginIsLocked ); // mark user session time so contact list is sorted with latest used on top
	m_VoiceFeedMgr.enableAudioCapture( true, session->getSendToId() );
	m_VoiceFeedMgr.enableAudioReceive( true, session->getSendToId() );
}

//============================================================================
void PluginVoicePhone::onSessionEnded( PluginSessionBase* session, bool pluginIsLocked, EOfferResponse offerResponse )
{
	m_VoiceFeedMgr.enableAudioCapture( false, session->getSendToId() );
	m_VoiceFeedMgr.enableAudioReceive( false, session->getSendToId() );
}

//============================================================================
void PluginVoicePhone::replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt )
{
	m_PluginSessionMgr.replaceConnection( netIdent, poOldSkt, poNewSkt );
}

//============================================================================
void PluginVoicePhone::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )
{
	m_PluginSessionMgr.onConnectionLost( sktBase );
}

//============================================================================
void PluginVoicePhone::onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
	m_VoiceFeedMgr.enableAudioCapture( false, netIdent->getMyOnlineId() );
	m_VoiceFeedMgr.enableAudioReceive( false, netIdent->getMyOnlineId() );
	m_PluginSessionMgr.onContactWentOffline( netIdent, sktBase );
}

