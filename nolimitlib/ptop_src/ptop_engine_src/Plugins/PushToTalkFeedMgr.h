#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <CoreLib/MediaCallbackInterface.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxGUIDList.h>

#include <memory>

class P2PEngine;
class PluginBase;
class PluginMgr;
class PluginSessionMgr;
class VxNetIdent;
class VxSktBase;
class VxPktHdr;
class PktPushToTalkReq;

class PushToTalkFeedMgr
{
public:
	PushToTalkFeedMgr( P2PEngine& engine, PluginBase& plugin, PluginSessionMgr& sessionMgr );

	virtual bool				fromGuiPushToTalk( VxNetIdent* netIdent, bool enableTalk, std::shared_ptr<VxSktBase>& sktBase );

	virtual void				onPktPushToTalkReq					( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
	virtual void				onPktPushToTalkReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
	virtual void				onPktPushToTalkStart				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
	virtual void				onPktPushToTalkStop                 ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );

	virtual void				onPktVoiceReq						( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
	virtual void				onPktVoiceReply						( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );

    virtual void				callbackOpusPkt( void * userData, PktVoiceReq* pktOpusAudio );
	virtual void				callbackAudioOutSpaceAvail( int freeSpaceLen );

	virtual bool				addPushToTalkUser( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase, bool rxOnly = false );
	virtual bool				removePushToTalkUser( VxGUID& onlineId, bool txOnly = false );
	virtual void				onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase );

	virtual bool				sendPushToTalkStart( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase );
	virtual bool				sendPushToTalkStop( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase );
	virtual bool				sendPushToTalkReq( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase );

protected:
	bool						enableAudioCapture( bool enable, VxNetIdent* netIdent, EAppModule appModule, std::shared_ptr<VxSktBase>& sktBase );
	void						updatePushToTalkStatus( VxGUID& onlineId );

    P2PEngine&                  m_Engine;
	PluginBase&					m_Plugin;
	PluginMgr&					m_PluginMgr;
	PluginSessionMgr&			m_SessionMgr;

	VxGUIDList					m_TxOnlineIdList;
	VxGUIDList					m_RxOnlineIdList;

	bool						m_AudioPktsRequested{ false };
	bool						m_MixerInputRequesed{ false };
};

