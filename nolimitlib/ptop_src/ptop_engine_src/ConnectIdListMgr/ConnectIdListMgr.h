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

#include <ptop_src/ptop_engine_src/IdentListMgrs/IdentListMgrBase.h>
#include <PktLib/ConnectId.h>

#include <vector>
#include <set>
#include <map>
#include <memory>

// maintains a list of online users
class ConnectIdListCallbackInterface;
class P2PEngine;
class PktAnnounce;
class VxNetIdent;
class VxSktBase;

class ConnectIdListMgr : public IdentListMgrBase
{
public:
    ConnectIdListMgr() = delete;
    ConnectIdListMgr( P2PEngine& engine );
    virtual ~ConnectIdListMgr() = default;
    
    bool                        isDirectConnected( VxGUID& onlineId );
    bool                        isRelayed( VxGUID& onlineId );
    bool                        isHosted( VxGUID& onlineId );

    bool                        isOnline( VxGUID& onlineId ) { return isDirectConnected( onlineId ) || isRelayed( onlineId ); }
    bool                        isOnline( GroupieId& groupieId );

    void                        userJoinedHost( VxGUID& sktConnectId, GroupieId& groupieId );
    void                        userLeftHost( VxGUID& sktConnectId, GroupieId& groupieId );

    bool                        getConnections( HostedId& hostId, std::set<ConnectId>& retConnectIdSet, std::set<ConnectId>& relayConnectIdSet );

    std::shared_ptr<VxSktBase> findHostConnection( GroupieId& groupieId, bool tryPeerFirst = false );
    std::shared_ptr<VxSktBase> findRelayMemberConnection( VxGUID& onlineId );
    std::shared_ptr<VxSktBase> findPeerConnection( VxGUID& onlineId );

    std::shared_ptr<VxSktBase> findAnyOnlineConnection( VxGUID& onlineId );
    std::shared_ptr<VxSktBase> findBestOnlineConnection( VxGUID& onlineId );

    virtual bool                findConnectionId( GroupieId& groupieId, VxGUID& retSktConnectId );
    virtual bool                findRelayConnectionId( VxGUID& onlineId, VxGUID& retSktConnectId );
    std::shared_ptr<VxSktBase> findSktBase( VxGUID& connectId );

    void                        addConnection( VxGUID& sktConnectId, GroupieId& groupieId, bool relayed );
    void                        removeConnection( VxGUID& sktConnectId, GroupieId& groupieId );
    void                        addConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason );
    void                        removeConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason );

    void                        disconnectIfIsOnlyUser( GroupieId& groupieId );

    virtual void                onConnectionLost( VxGUID& sktConnectId );
    virtual void                onGroupUserAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, bool relayed );
    void                        onGroupRelayedUserAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );

    void                        wantConnectIdListCallback( ConnectIdListCallbackInterface* client, bool enable );

    void                        getOnlineMembers( HostedId& hostId, std::vector<VxGUID>& onlineIdList );
    bool                        isMemberOnline( HostedId& hostId, VxGUID& onlineId );

protected:
    void                        announceOnlineStatus( VxGUID& onlineId, bool isOnline );
    void                        announceConnectionStatus( ConnectId& connectId, bool isConnected );
    void                        announceRelayStatus( ConnectId& connectId, bool isRelayed );

    void                        announceConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason, bool enableReason );
    void                        announceConnectionLost( VxGUID& sktConnectId );

    void						lockClientList( void ) { m_ClientCallbackMutex.lock(); }
    void						unlockClientList( void ) { m_ClientCallbackMutex.unlock(); }

    std::set<ConnectId>         m_ConnectIdList;
    std::set<ConnectId>         m_RelayedIdList;
    std::map<VxGUID, std::set<EConnectReason>>      m_ConnectReasonList;
    std::vector<ConnectIdListCallbackInterface*>    m_CallbackClients;
    VxMutex						m_ClientCallbackMutex;
};

