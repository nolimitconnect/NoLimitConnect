#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <IdentListMgrs/IdentListMgrBase.h>
#include <PktLib/ConnectId.h>

#include <vector>
#include <set>
#include <map>
#include <memory>

// maintains a list of online users
class ConnectIdListCallback;
class OnlineStatusCallback;
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

    bool                        isOnline( GroupieId& groupieId );
    bool                        isUserOnline( VxGUID& onlineId );
    bool                        isUserExcluded( const VxGUID& onlineId );

    void                        setNetworkHostId( VxGUID& onlineId )    { m_NetworkHostOnlineId = onlineId; }
    VxGUID                      getNetworkHostId( void )                { return m_NetworkHostOnlineId; }
    bool                        isNetworkHost( VxGUID& onlineId )       { return onlineId == m_NetworkHostOnlineId; }

    bool                        isConnectionInUse( VxGUID& sktConnectId );

    void                        setExcludeConnectId( VxGUID& sktConnectId, bool exclude = true );
    bool                        isConnectIdExcluded( VxGUID& sktConnectId );

    void                        userJoinedHost( VxGUID& sktConnectId, GroupieId& groupieId );
    void                        userLeftHost( VxGUID& sktConnectId, GroupieId& groupieId );

    void                        fromGuiDisconnectFromUser( VxGUID& onlineId );

    bool                        getConnections( HostedId& hostId, std::set<ConnectId>& retConnectIdSet, std::set<ConnectId>& relayConnectIdSet );

    std::shared_ptr<VxSktBase>  findHostConnection( GroupieId& groupieId, bool tryPeerFirst = false );
    std::shared_ptr<VxSktBase>  findRelayMemberConnection( VxGUID& onlineId );
    std::shared_ptr<VxSktBase>  findPeerConnection( VxGUID& onlineId );

    std::shared_ptr<VxSktBase>  findAnyHostOnlineConnection( const VxGUID& onlineId );
    std::shared_ptr<VxSktBase>  findAnyUserOnlineConnection( const VxGUID& onlineId );

    std::shared_ptr<VxSktBase>  findBestHostOnlineConnection( VxGUID& onlineId );
    std::shared_ptr<VxSktBase>  findBestUserOnlineConnection( VxGUID& onlineId );

    virtual bool                findConnectionId( GroupieId& groupieId, VxGUID& retSktConnectId );
    virtual bool                findRelayConnectionId( VxGUID& onlineId, VxGUID& retSktConnectId );
    std::shared_ptr<VxSktBase>  findSktBase( VxGUID& connectId );

    bool                        addConnection( std::shared_ptr<VxSktBase>& sktBase, GroupieId& groupieId );
    void                        addConnection( VxGUID& sktConnectId, GroupieId& groupieId, bool relayed );
    void                        removeConnection( VxGUID& sktConnectId, GroupieId& groupieId );
    void                        addConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason );
    void                        removeConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason );

    void                        disconnectIfIsOnlyUser( GroupieId& groupieId );

    virtual bool                onConnectionLost( std::shared_ptr<VxSktBase>& sktBase );
    virtual bool                onConnectionLost( VxGUID& sktConnectId ); ///< returns false if invalid or is excluded connection
    virtual void                onGroupUserAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, bool relayed );
    void                        onGroupRelayedUserAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent );

    void                        getOnlineMembers( HostedId& hostId, std::vector<VxGUID>& onlineIdList );
    bool                        isMemberOnline( HostedId& hostId, VxGUID& onlineId );

    void                        pktAnnRecieved( VxGUID& sktConnectId, VxGUID onlineId );
    void                        updateOnlineExclusion( VxGUID onlineId, bool excludeFromOnlineStatus, bool isNetworkHost = false );

    void                        wantConnectIdListCallback( ConnectIdListCallback* client, bool enable );
    void                        wantOnlineStatusCallback( OnlineStatusCallback* client, bool enable );

protected:
    void                        announceOnlineStatus( VxGUID& onlineId, bool isOnline );
    void                        announceConnectionStatus( ConnectId& connectId, bool isConnected );
    void                        announceRelayStatus( ConnectId& connectId, bool isRelayed );

    void                        announceConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason, bool enableReason );
    void                        announceConnectionLost( VxGUID& sktConnectId );

    void                        addOnlineConnectionPair( VxGUID& sktConnectId, VxGUID& onlineId );
    void                        removeOnlineConnectionPair( VxGUID& sktConnectId, VxGUID& onlineId );
    void                        removeOnlineConnectionPairs( VxGUID& sktConnectId, std::set<VxGUID>& lostConnUserList );

    void						lockConnectIdClientList( void )         { m_ConnectIdCallbackMutex.lock(); }
    void						unlockConnectIdClientList( void )       { m_ConnectIdCallbackMutex.unlock(); }

    void						lockOnlineStatusClientList( void )      { m_OnlineStatusCallbackMutex.lock(); }
    void						unlockOnlineStatusClientList( void )    { m_OnlineStatusCallbackMutex.unlock(); }

    void						lockOnlineIdList( void )                { m_OnlineIdMutex.lock(); }
    void						unlockOnlineIdList( void )              { m_OnlineIdMutex.unlock(); }

    void						lockExclusionList( void )               { m_ConnectIdExclusionMutex.lock(); }
    void						unlockExclusionList( void )             { m_ConnectIdExclusionMutex.unlock(); }

    std::set<ConnectId>         m_ConnectIdList;
    std::set<ConnectId>         m_RelayedIdList;
    std::map<VxGUID, std::set<EConnectReason>>      m_ConnectReasonList;

    std::vector<ConnectIdListCallback*>    m_ConnectIdCallbackClients;
    VxMutex						m_ConnectIdCallbackMutex;

    std::vector<OnlineStatusCallback*>    m_OnlineStatusCallbackClients;
    VxMutex						m_OnlineStatusCallbackMutex;

    std::set<VxGUID>            m_OnlineIdExclusionList;
    std::set<VxGUID>            m_OnlineIdListList;
    VxMutex						m_OnlineIdMutex;
    std::vector<std::pair<VxGUID, VxGUID>> m_OnlineConnectionPairs; // first connection id second online id
    VxGUID                      m_NetworkHostOnlineId;

    std::set<VxGUID>            m_ConnectIdExclusionList;
    VxMutex						m_ConnectIdExclusionMutex;
 
};

