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

#include "PluginSessionMgr.h"
#include "PluginBase.h"
#include "PushToTalkFeedMgr.h"

#include <PktLib/VxCommon.h>

class PluginPushToTalk : public PluginBase
{
public:
	PluginPushToTalk( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginPushToTalk() = default;

	virtual bool				fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;
	virtual bool				fromGuiOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;

	virtual bool				fromGuiIsPluginInSession( VxGUID& onlineId = VxGUID::nullVxGUID(),  int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
	virtual void				fromGuiStartPluginSession( VxGUID& onlineId = VxGUID::nullVxGUID(), int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
	virtual void				fromGuiStopPluginSession( VxGUID& onlineId = VxGUID::nullVxGUID(),  int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

	virtual bool				fromGuiInstMsg( VxGUID& onlineId, const char* pMsg ) override;
	virtual bool				fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk ) override;

	virtual void				replaceConnection			( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override;
protected:
	virtual void				onPktPluginOfferReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktPluginOfferReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktChatReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktPushToTalkReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktPushToTalkReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktPushToTalkStart		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktPushToTalkStop         ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	
	virtual void				onPktSessionStopReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

	virtual void				onPktVoiceReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
	virtual void				onPktVoiceReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual	void				onContactWentOffline		( VxNetIdent*	netIdent, std::shared_ptr<VxSktBase>& sktBase ) override;
    virtual	void				onConnectionLost			( std::shared_ptr<VxSktBase>& sktBase ) override;

	void						onContactOnlineStatusChange( VxGUID& onlineId, bool isOnline ) override {};

    virtual void				onSessionStart				( PluginSessionBase* poSession, bool pluginIsLocked ) override;
    virtual void				onSessionEnded				( PluginSessionBase* poSession, bool pluginIsLocked, EOfferResponse offerResponse ) override;

protected:
    virtual void				callbackOpusPkt				( PktVoiceReq * pktOpusAudio ) override;
    virtual void				callbackAudioOutSpaceAvail	( int freeSpaceLen ) override;

	PluginSessionMgr			m_PluginSessionMgr;
	PushToTalkFeedMgr			m_PushToTalkFeedMgr;
};



