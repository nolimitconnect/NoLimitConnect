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

class PluginBaseMultimedia : public PluginBase
{
public:
	PluginBaseMultimedia( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginBaseMultimedia() = default;

    EHostType                   getHostType( void ) override        { return m_HostType; }
    HostedId&                   getHostedId( void )                 { return m_HostedId; }

    virtual void				fromGuiUserLoggedOn( void ) override;

	virtual bool				fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;
    virtual bool				fromGuiOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo ) override;

    virtual bool				fromGuiIsPluginInSession( VxGUID& onlineId = VxGUID::nullVxGUID(), VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
    virtual bool                fromGuiStartPluginSession( VxGUID& onlineId = VxGUID::nullVxGUID(), VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;
    virtual void				fromGuiStopPluginSession( VxGUID& onlineId = VxGUID::nullVxGUID(), VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

	virtual bool				fromGuiSendAsset( AssetBaseInfo& assetInfo ) override;
    virtual bool				fromGuiMultiSessionAction( VxGUID& onlineId, EMSessionAction mSessionAction, int pos0to100000, VxGUID lclSessionId = VxGUID::nullVxGUID() ) override;

    virtual bool				fromGuiTodGameActionSend( VxGUID& onlineId, ETodGameAction todGameAction ) override;

    void                        broadcastToClients( VxPktHdr* pktHdr, VxGUID& requesterOnlineId, std::shared_ptr<VxSktBase>& sktBaseRequester, bool includeRequester = true ) override;
    void                        broadcastToClients( VxPktHdr* pktHdr, VxGUID& excludedOnlineId ) override;

protected:

    virtual void				onPktPluginOfferReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktPluginOfferReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktSessionStopReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktVoiceReq				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVoiceReply				( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktVideoFeedReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedStatus		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedPic			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedPicChunk		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktVideoFeedPicAck		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktAssetSendReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktAssetSendReply			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktAssetChunkReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktAssetChunkReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktAssetSendCompleteReq	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktAssetSendCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktAssetXferErr			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktMultiSessionReq		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktMultiSessionReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktTodGameStats			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktTodGameAction			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktTodGameValue			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				callbackOpusPkt( PktVoiceReq * pktOpusAudio ) override;
    virtual void				callbackAudioOutSpaceAvail( int freeSpaceLen ) override;

	//virtual void				callbackVideoJpg( VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen, int motion0to100000 );
    virtual void				callbackVideoPktPic( VxGUID& onlineId, PktVideoFeedPic * pktVid, int pktsInSequence, int thisPktNum ) override;
    virtual void				callbackVideoPktPicChunk( VxGUID& onlineId, PktVideoFeedPicChunk * pktVid, int pktsInSequence, int thisPktNum ) override;

    virtual void				onSessionStart( PluginSessionBase* session, bool pluginIsLocked ) override;
    virtual void				onSessionEnded( PluginSessionBase* session, bool pluginIsLocked, EOfferResponse offerResponse ) override;

protected:
	virtual void				onContactWentOnline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override;
	virtual void				onContactWentOffline( VxNetIdent*	netIdent, std::shared_ptr<VxSktBase>& sktBase ) override;

    void						onContactOnlineStatusChange( ConnectId& connectId, bool isOnline ) override {};

	virtual void				replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override;
	virtual void				onConnectionLost( std::shared_ptr<VxSktBase>& sktBase ) override;

    EConnectReason              getHostAnnounceConnectReason( void );
    EConnectReason              getHostJoinConnectReason( void );
    EConnectReason              getHostSearchConnectReason( void );

	//=== vars ===
	PluginSessionMgr			m_PluginSessionMgr;
	VoiceFeedMgr				m_VoiceFeedMgr;
	VideoFeedMgr				m_VideoFeedMgr;

    AssetXferMgr                m_AssetXferMgr;
    EHostType                   m_HostType{ eHostTypeUnknown };
    HostedId                    m_HostedId;
};



