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
#include "VoiceFeedMgr.h"
#include "VideoFeedMgr.h"

class PluginCamClient : public PluginBase
{
public:
	PluginCamClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginCamClient() = default;

    EMediaModule				getMediaModule( void ) override { return eMediaModuleCamClient; }

    virtual bool                fromGuiStartPluginSession( VxGUID& onlineId = VxGUID::nullVxGUID(), VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
    virtual void				fromGuiStopPluginSession( VxGUID& onlineId = VxGUID::nullVxGUID(), VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
    virtual bool				fromGuiIsPluginInSession( VxGUID& onlineId = VxGUID::nullVxGUID(), VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

	//! user wants to send offer to friend.. return false if cannot connect
	virtual bool				fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;

    virtual void				fromGuiUpdatePluginPermission( EPluginType pluginType, EFriendState pluginPermission ) override;

	bool						stopCamSession(	VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase );

	void						sendVidPkt( VxPktHdr* vidPkt, bool requiresAck );

	void						stopAllSessions( EPluginType pluginType );

protected:
    virtual EPluginAccess       canAcceptNewSession			( VxNetIdent* netIdent ) override;

    virtual void				replaceConnection			( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override;
    virtual void				onConnectionLost			( std::shared_ptr<VxSktBase>& sktBase ) override;
    virtual void				onContactWentOffline		( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override;

    void						onContactOnlineStatusChange( ConnectId& connectId, bool isOnline ) override {};

    virtual void				onPktPluginOfferReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktPluginOfferReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktSessionStartReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktSessionStartReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktSessionStopReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktSessionStopReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktVideoFeedReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedStatus		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedPic			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedPicChunk		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedPicAck		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktVoiceReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVoiceReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				callbackOpusPkt( PktVoiceReq * pktOpusAudio ) override;
    virtual void				callbackAudioOutSpaceAvail( int freeSpaceLenBytes ) override;

    virtual void				callbackVideoJpg( VxGUID& feedId, std::shared_ptr<CamJpgVideo>& jpgVideo ) override;
    virtual void				callbackVideoPktPic( VxGUID& feedId, PktVideoFeedPic * pktVid, int pktsInSequence, int thisPktNum ) override;
    virtual void				callbackVideoPktPicChunk( VxGUID& feedId, PktVideoFeedPicChunk * pktVid, int pktsInSequence, int thisPktNum ) override;

	// override this by plugin to create inherited RxSession
    RxSession *					createRxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId ) override;
	// override this by plugin to create inherited TxSession
    TxSession *					createTxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId ) override;

	bool						requestCamSession(	RxSession *			rxSession,
													bool				bWaitForSuccess = false );
    void						setIsPluginInSession( bool isInSession ) override;

	virtual	void				onNetworkConnectionReady( bool requiresRelay ) override;

	void						stopAllSessions( void );

	void						enableCamServerService( bool enable );

	//=== vars ===//
	PluginSessionMgr			m_PluginSessionMgr;					
	VoiceFeedMgr				m_VoiceFeedMgr;
	VideoFeedMgr				m_VideoFeedMgr;

	bool						m_IsCamServiceEnabled{ false };
};



