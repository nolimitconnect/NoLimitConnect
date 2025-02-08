//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <PktLib/VxCommon.h>

#include "PluginTruthOrDare.h"
#include "PluginMgr.h"

#include "TodGameSession.h"

#include <GuiInterface/IToGui.h>
#include <P2PEngine/P2PEngine.h>

#include <PktLib/PktsTodGame.h>
#include <PktLib/PktChatReq.h>

#include <memory.h>
#ifdef _MSC_VER
# pragma warning( disable: 4355 ) //'this' : used in base member initializer list
#endif // _MSC_VER

//============================================================================
PluginTruthOrDare::PluginTruthOrDare( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseMultimedia( engine, pluginMgr, myIdent, pluginType )
, m_PluginSessionMgr( engine, *this, pluginMgr )
, m_VoiceFeedMgr( engine, *this, m_PluginSessionMgr )
, m_VideoFeedMgr( engine, *this, m_PluginSessionMgr )
{
	setPluginType( ePluginTypeTruthOrDare );
}

//============================================================================
P2PSession* PluginTruthOrDare::createP2PSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
    return new TodGameSession( sktBase, onlineId, getPluginType() );
}

//============================================================================
bool PluginTruthOrDare::fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	VxGUID& lclSessionId = offerInfo.getOfferId();
	PluginBase::AutoPluginLock pluginMutexLock( this );
	P2PSession* poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionBySessionId( lclSessionId, true );

	if( poSession )
	{
		LogMsg( LOG_ERROR, "PluginTruthOrDare already in session");
		// assume some error in logic
		m_PluginSessionMgr.removeSessionBySessionId( true, onlineId );
	}
		
	return m_PluginSessionMgr.fromGuiMakePluginOffer( true, onlineId, offerInfo );
}

//============================================================================
bool PluginTruthOrDare::fromGuiOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	return m_PluginSessionMgr.fromGuiOfferReply( false, onlineId, offerInfo );
}

//============================================================================
bool PluginTruthOrDare::fromGuiIsPluginInSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	return m_PluginSessionMgr.fromGuiIsPluginInSession( false, onlineId, lclSessionId );
}

//============================================================================
//! called to start service or session with remote friend
void PluginTruthOrDare::fromGuiStartPluginSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	m_VoiceFeedMgr.fromGuiStartPluginSession( false, eAppModuleTruthOrDare, onlineId );
	m_VideoFeedMgr.fromGuiStartPluginSession( false, eAppModuleTruthOrDare, onlineId );
}

//============================================================================
//! called to stop service or session with remote friend
void PluginTruthOrDare::fromGuiStopPluginSession( VxGUID& onlineId, VxGUID lclSessionId )
{
	m_VoiceFeedMgr.fromGuiStopPluginSession( false, eAppModuleTruthOrDare, onlineId );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleTruthOrDare, onlineId );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleTruthOrDare, getEngine().getMyOnlineId() );
	m_PluginSessionMgr.fromGuiStopPluginSession( false, onlineId, lclSessionId );
}

//============================================================================
bool PluginTruthOrDare::fromGuiInstMsg( VxGUID& onlineId, const char* msg )
{
	LogMsg( LOG_ERROR, "PluginTruthOrDare::fromGuiInstMsg" );
	PluginBase::AutoPluginLock pluginMutexLock( this );
	P2PSession* poSession = (P2PSession*)m_PluginSessionMgr.findP2PSessionByOnlineId( onlineId, true );
	if( poSession )
	{
		PktChatReq pkt;
		pkt.addMsg( msg );
		return m_PluginMgr.pluginApiTxPacket( m_ePluginType, onlineId, poSession->getSkt(), &pkt );
	}
	else
	{
		LogMsg( LOG_ERROR, "PluginTruthOrDare::fromGuiInstMsg session not found" );
		return false;
	}
}

//============================================================================
bool PluginTruthOrDare::fromGuiSetGameValueVar( VxGUID& onlineId, int32_t varId, int32_t varValue )
{
	PluginBase::AutoPluginLock pluginMutexLock( this );
	PluginSessionBase* poSession = m_PluginSessionMgr.findPluginSessionByOnlineId( onlineId, true );
	if( poSession )
	{
		PktTodGameValue pktGameValue;
		pktGameValue.setValue( (ETodGameVarId) varId, varValue );
		return m_PluginMgr.pluginApiTxPacket( m_ePluginType, onlineId, poSession->getSkt(), &pktGameValue );
	}

	return false;
}

//============================================================================
bool PluginTruthOrDare::fromGuiSetGameActionVar( VxGUID& onlineId, int32_t actionId, int32_t actionValue )
{
	PluginBase::AutoPluginLock pluginMutexLock( this );
	PluginSessionBase* poSession = m_PluginSessionMgr.findPluginSessionByOnlineId( onlineId, true );
	if( poSession )
	{
		PktTodGameAction pktGameAction;
		pktGameAction.setAction( (ETodGameAction) actionId, actionValue );
		return m_PluginMgr.pluginApiTxPacket( m_ePluginType, onlineId, poSession->getSkt(), &pktGameAction );
	}

	return false;
}

//============================================================================
void PluginTruthOrDare::onPktChatReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktChatReq * pkt = (PktChatReq *)pktHdr;
	IToGui::getIToGui().toGuiInstMsg( netIdent, m_ePluginType, (const char*)pkt->getDataPayload() );
}

//============================================================================
void PluginTruthOrDare::onPktTodGameStats( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktTodGameStats * poPkt = (PktTodGameStats *)pktHdr;
	for( int i = 0; i < eMaxTodGameStatId; ++i )
	{
		IToGui::getIToGui().toGuiSetGameValueVar( m_ePluginType, netIdent->getMyOnlineId(), i, poPkt->getVar((ETodGameVarId)i) );
	}

	IToGui::getIToGui().toGuiSetGameActionVar( m_ePluginType, netIdent->getMyOnlineId(), eTodGameActionSendStats, 1 );
}

//============================================================================
void PluginTruthOrDare::onPktTodGameAction( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktTodGameAction * poPkt = (PktTodGameAction *)pktHdr;
	IToGui::getIToGui().toGuiSetGameActionVar( m_ePluginType, netIdent->getMyOnlineId(), poPkt->getActionVarId(), poPkt->getActionVarValue() );
}

//============================================================================
void PluginTruthOrDare::onPktTodGameValue( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	PktTodGameValue * poPkt = (PktTodGameValue *)pktHdr;
	IToGui::getIToGui().toGuiSetGameValueVar( m_ePluginType, netIdent->getMyOnlineId(), poPkt->getValueVarId(), poPkt->getValueVar() );
}

//============================================================================
void PluginTruthOrDare::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.onPktPluginOfferReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.onPktPluginOfferReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVideoFeedReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVideoFeedStatus( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedStatus( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVideoFeedPic( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedPic( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVideoFeedPicChunk( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedPicChunk( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVideoFeedPicAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VideoFeedMgr.onPktVideoFeedPicAck( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::callbackOpusPkt( PktVoiceReq * pktOpusAudio )
{
	m_VoiceFeedMgr.callbackOpusPkt( pktOpusAudio );
}

//============================================================================
void PluginTruthOrDare::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	m_VoiceFeedMgr.callbackAudioOutSpaceAvail( freeSpaceLen );
}

//============================================================================
void PluginTruthOrDare::callbackVideoPktPic( VxGUID& onlineId, PktVideoFeedPic * pktVid, int pktsInSequence, int thisPktNum )
{
	m_VideoFeedMgr.callbackVideoPktPic( onlineId, pktVid, pktsInSequence, thisPktNum );
}

//============================================================================
void PluginTruthOrDare::callbackVideoPktPicChunk( VxGUID& onlineId, PktVideoFeedPicChunk * pktVid, int pktsInSequence, int thisPktNum ) 
{
	m_VideoFeedMgr.callbackVideoPktPicChunk( onlineId, pktVid, pktsInSequence, thisPktNum );
}

//============================================================================
void PluginTruthOrDare::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_PluginSessionMgr.onPktSessionStopReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	m_VoiceFeedMgr.onPktVoiceReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginTruthOrDare::onSessionStart( PluginSessionBase* session, bool pluginIsLocked )
{
	PluginBase::onSessionStart( session, pluginIsLocked ); // mark user session time so contact list is sorted with latest used on top
	m_VoiceFeedMgr.fromGuiStartPluginSession( pluginIsLocked, eAppModuleTruthOrDare, session->getSendToId() );
	// in order to get my video packets to send out the ident has to be myself
	m_VideoFeedMgr.fromGuiStartPluginSession( pluginIsLocked, eAppModuleTruthOrDare, getEngine().getMyOnlineId() );
}

//============================================================================
void PluginTruthOrDare::onSessionEnded( PluginSessionBase* session, bool pluginIsLocked, EOfferResponse offerResponse )
{
	m_VoiceFeedMgr.fromGuiStopPluginSession( pluginIsLocked, eAppModuleTruthOrDare, session->getSendToId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( pluginIsLocked, eAppModuleTruthOrDare, session->getSendToId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( pluginIsLocked, eAppModuleTruthOrDare, getEngine().getMyOnlineId() );
}

//============================================================================
void PluginTruthOrDare::replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt )
{
	m_PluginSessionMgr.replaceConnection( netIdent, poOldSkt, poNewSkt );
}

//============================================================================
void PluginTruthOrDare::onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
	m_VoiceFeedMgr.fromGuiStopPluginSession( false, eAppModuleTruthOrDare, netIdent->getMyOnlineId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleTruthOrDare, netIdent->getMyOnlineId() );
	m_VideoFeedMgr.fromGuiStopPluginSession( false, eAppModuleTruthOrDare, getEngine().getMyOnlineId() );
	m_PluginSessionMgr.onContactWentOffline( netIdent, sktBase );
}

//============================================================================
void PluginTruthOrDare::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )
{
	m_PluginSessionMgr.onConnectionLost( sktBase );
}



