#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
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

#include "HostServerSearchMgr.h"
#include <ptop_src/ptop_engine_src/User/UserList.h>

#include <PktLib/PktsHostInvite.h>
#include <PktLib/PluginId.h>

class HostServerMgr : public HostServerSearchMgr
{
public:
    HostServerMgr( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, PluginBase& pluginBase );
	virtual ~HostServerMgr() = default;

    virtual void				fromGuiListAction( EListAction listAction ) override;

    virtual void                removeSession( VxGUID& sessionId, EConnectReason connectReason = eConnectReasonUnknown ) override;
    virtual void                sendHostAnnounceToNetworkHost( VxGUID& sessionId, PktHostInviteAnnounceReq& hostAnnounce, EConnectReason connectReason );

    virtual EJoinState	        getJoinState( VxNetIdent* netIdent, EHostType hostType ) override;
    virtual EMembershipState	getMembershipState( VxNetIdent* netIdent, EHostType hostType ) override;

    virtual void				onJoinRequested( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID sessionId, EHostType hostType );
    virtual void				onUserJoined( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID sessionId, GroupieId& groupieId );
    virtual void				onUserLeftHost( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID sessionId, EHostType hostType );
    virtual void				onUserUnJoined( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID sessionId, GroupieId& groupieId );

    virtual void                onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );
    virtual void                onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    virtual void                onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );
    virtual void                onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    virtual void                onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );
    virtual void                onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );

    virtual void                onGroupDirectUserAnnounce( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );

    virtual void				onPktHostUserInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserStatusReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserStatusReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

    virtual void				onPktHostUserListReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserListMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;
    virtual void				onPktHostUserListMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

protected:
    virtual bool                onConnectToHostSuccess( EHostType hostType, VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& onlineId, EConnectReason connectReason ) override;
    virtual void                onContactDisconnected( VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& onlineId, EConnectReason connectReason = eConnectReasonUnknown ) override;

    virtual void                onClientJoined( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );
    virtual bool                addClient( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );
    virtual bool                removeClient( VxGUID& onlineId );

    virtual void                addAnnounceSession( VxGUID& sessionId, PktHostInviteAnnounceReq* hostAnn );
    virtual void                removeAnnounceSession( VxGUID& sessionId );

    virtual bool                isMemberOnline( VxGUID& onlineId );

    VxMutex                     m_ServerMutex;
    VxGUIDList                  m_ClientList;
    std::map<VxGUID, PktHostInviteAnnounceReq*> m_AnnList;
    VxMutex                     m_AnnListMutex;
};

