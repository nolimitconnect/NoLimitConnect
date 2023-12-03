#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBase.h"
#include "PluginSessionMgr.h"
#include "VoiceFeedMgr.h"
#include "VideoFeedMgr.h"

class PluginVideoPhone : public PluginBase
{
public:
	PluginVideoPhone( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginVideoPhone() = default;

	virtual bool				fromGuiMakePluginOffer( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;
	virtual bool				fromGuiOfferReply( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;

	virtual bool				fromGuiIsPluginInSession( VxNetIdent* netIdent     = nullptr,  int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
	virtual void				fromGuiStartPluginSession( VxNetIdent* netIdent 	= nullptr, 	int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
	virtual void				fromGuiStopPluginSession( VxNetIdent* netIdent 	= nullptr, 	int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

	virtual bool				fromGuiInstMsg(	VxNetIdent* netIdent, const char* pMsg ) override;

protected:
	virtual void				onPktPluginOfferReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktPluginOfferReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktChatReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktVoiceReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktVoiceReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktVideoFeedReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktVideoFeedStatus		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktVideoFeedPic			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktVideoFeedPicChunk		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktVideoFeedPicAck		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktSessionStopReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				callbackOpusPkt( void * userData, PktVoiceReq * pktOpusAudio ) override;
    virtual void				callbackAudioOutSpaceAvail( int freeSpaceLen ) override;

    virtual void				callbackVideoPktPic( void * userData, VxGUID& onlineId, PktVideoFeedPic * pktVid, int pktsInSequence, int thisPktNum ) override;
    virtual void				callbackVideoPktPicChunk( void * userData, VxGUID& onlineId, PktVideoFeedPicChunk * pktVid, int pktsInSequence, int thisPktNum ) override;

    virtual void				onSessionStart( PluginSessionBase* session, bool pluginIsLocked ) override;
    virtual void				onSessionEnded( PluginSessionBase* session, bool pluginIsLocked, EOfferResponse offerResponse ) override;

    virtual void				replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override;
    virtual void				onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override;
    virtual void				onConnectionLost( std::shared_ptr<VxSktBase>& sktBase ) override;


	//=== vars ===//
protected:
	PluginSessionMgr			m_PluginSessionMgr;
	VoiceFeedMgr				m_VoiceFeedMgr;
	VideoFeedMgr				m_VideoFeedMgr;
};



