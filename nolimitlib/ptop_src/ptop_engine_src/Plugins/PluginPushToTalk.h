#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
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

#include "PluginSessionMgr.h"
#include "PluginBase.h"
#include "PushToTalkFeedMgr.h"

#include <PktLib/VxCommon.h>

class PluginPushToTalk : public PluginBase
{
public:
	PluginPushToTalk( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginPushToTalk() = default;

	virtual bool				fromGuiMakePluginOffer( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;
	virtual bool				fromGuiOfferReply( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;

	virtual bool				fromGuiIsPluginInSession( VxNetIdent* netIdent = nullptr,  int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
	virtual void				fromGuiStartPluginSession( VxNetIdent* netIdent = nullptr, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
	virtual void				fromGuiStopPluginSession( VxNetIdent* netIdent = nullptr,  int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

	virtual bool				fromGuiInstMsg(	VxNetIdent*	netIdent, const char* pMsg ) override;
	virtual bool				fromGuiPushToTalk( VxNetIdent* netIdent, bool enableTalk ) override;

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

    virtual void				onSessionStart				( PluginSessionBase* poSession, bool pluginIsLocked ) override;
    virtual void				onSessionEnded				( PluginSessionBase* poSession, bool pluginIsLocked, EOfferResponse offerResponse ) override;

protected:
    virtual void				callbackOpusPkt				( void * userData, PktVoiceReq * pktOpusAudio ) override;
    virtual void				callbackAudioOutSpaceAvail	( int freeSpaceLen ) override;

	PluginSessionMgr			m_PluginSessionMgr;
	PushToTalkFeedMgr			m_PushToTalkFeedMgr;
};



