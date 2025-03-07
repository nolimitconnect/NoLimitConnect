//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ConnectIdListMgr.h"
#include "ConnectIdListCallback.h"

#include <BaseInfo/BaseSessionInfo.h>
#include <BigListLib/BigListInfo.h>
#include <HostServerJoinMgr/HostServerJoinMgr.h>

#include <Membership/MemberActiveMgr.h>
#include <P2PEngine/P2PEngine.h>
#include <Plugins/PluginBase.h>
#include <Plugins/PluginMgr.h>

#include <UserJoinMgr/UserJoinMgr.h>
#include <UserOnlineMgr/UserOnlineMgr.h>

#include <CoreLib/VxDebug.h>
#include <NetLib/VxPeerMgr.h>
#include <NetLib/VxSktBase.h>

#include <PktLib/PktsRandConnectDefs.h>

#include <algorithm>

//============================================================================
ConnectIdListMgr::ConnectIdListMgr( P2PEngine& engine )
    : IdentListMgrBase( engine )
{
    setIdentListType( eUserViewTypeOnline );
}

//============================================================================
bool ConnectIdListMgr::getConnections( HostedId& hostId, std::set<ConnectId>& directConnectIdSet, std::set<ConnectId>& relayConnectIdSet )
{
    directConnectIdSet.clear();
    relayConnectIdSet.clear();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast<ConnectId&>(connectId).getHostedId() == hostId )
        {
            directConnectIdSet.insert( connectId );
        }
    }

    for( auto& connectId : m_RelayedIdList )
    {
        if( const_cast<ConnectId&>(connectId).getHostedId() == hostId )
        {
            relayConnectIdSet.insert( connectId );
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return !directConnectIdSet.empty() || !relayConnectIdSet.empty();
}

//============================================================================
bool ConnectIdListMgr::isDirectConnected( VxGUID& onlineId )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return true;
    }

    bool isOnlined = false;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast<ConnectId&>(connectId).getUserOnlineId() == onlineId )
        {
            isOnlined = true;
            break;
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return isOnlined;
}

//============================================================================
bool ConnectIdListMgr::isRelayed( VxGUID& onlineId )
{
    bool isRelayed = false;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_RelayedIdList )
    {
        if( const_cast<ConnectId&>(connectId).getUserOnlineId() == onlineId )
        {
            isRelayed = true;
            break;
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return isRelayed;
}

//============================================================================
bool ConnectIdListMgr::isHosted( VxGUID& onlineId )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return true;
    }

    bool isHosted = false;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        ConnectId& noConstConnectId = const_cast< ConnectId& >( connectId );
        if( noConstConnectId.getUserOnlineId() == onlineId && IsHostARelayForUsers( noConstConnectId.getHostType() ) )
        {
            isHosted = true;
            break;
        }
    }

    if( !isHosted )
    {
        for( auto& connectId : m_RelayedIdList )
        {
            ConnectId& noConstConnectId = const_cast<ConnectId&>(connectId);
            if( noConstConnectId.getUserOnlineId() == onlineId && IsHostARelayForUsers( noConstConnectId.getHostType() ) )
            {
                isHosted = true;
                break;
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return isHosted;
}

//============================================================================
bool ConnectIdListMgr::isOnline( GroupieId& groupieId )
{
    bool isOnlined = false;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast< ConnectId& >( connectId ).getGroupieId() == groupieId )
        {
            isOnlined = true;
            break;
        }
    }

    if( !isOnlined )
    {
        for( auto& connectId : m_RelayedIdList )
        {
            if( const_cast<ConnectId&>(connectId).getGroupieId() == groupieId )
            {
                isOnlined = true;
                break;
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return isOnlined;
}

//============================================================================
bool ConnectIdListMgr::addConnection( std::shared_ptr<VxSktBase>& sktBase, GroupieId& groupieId )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::addConnection groupie id invalid" );
        return false;
    }

    if( sktBase->isConnected() )
    {
        bool isRelayed = groupieId.getUserOnlineId() != sktBase->getPeerOnlineId();
        addConnection( sktBase->getSocketId(), groupieId, isRelayed );
        return true;
    }
    else
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::addConnection socket is no longer connected" );
    }

    return false;
}

//============================================================================
void ConnectIdListMgr::addConnection( VxGUID& sktConnectId, GroupieId& groupieId, bool relayed )
{
    if( !groupieId.isValid() || !sktConnectId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::addConnection invalid id" );
        return;
    }

    VxGUID onlineId = groupieId.getUserOnlineId();
    bool wasOnline = isUserOnline( onlineId );
    bool becameOnline{ false };
    if( !wasOnline && !isUserExcluded( onlineId ) )
    {
        lockOnlineIdList();
        m_OnlineIdListList.insert( onlineId );
        unlockOnlineIdList();
        becameOnline = true;
    }
    
    addOnlineConnectionPair( sktConnectId, onlineId );

    ConnectId connectId( sktConnectId, groupieId );

    LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::%s relayed ? %d ConnectId %s", __func__, relayed, connectId.describeConnectId().c_str() );

    if( relayed )
    {
        #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
        #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        lockConnectIdList();
        #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
        #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        bool isInList = m_RelayedIdList.find( connectId ) != m_RelayedIdList.end();
        if( !isInList )
        {
            // new connection
            m_RelayedIdList.insert( connectId );
        }

        #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
        #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        unlockConnectIdList();

        announceRelayStatus(connectId, true);
    }
    else
    {
        #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
        #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        lockConnectIdList();
        #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
        #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        bool isInList = m_ConnectIdList.find( connectId ) != m_ConnectIdList.end();
        if( !isInList )
        {
            // new connection
            m_ConnectIdList.insert( connectId );
        }

        #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
        #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        unlockConnectIdList();

        announceConnectionStatus( connectId, true );
    }

    if( becameOnline )
    {
        announceOnlineStatus( onlineId, true );
    }
}

//============================================================================
void ConnectIdListMgr::removeConnection( VxGUID& sktConnectId, GroupieId& groupieId )
{
    if( !groupieId.isValid() || !sktConnectId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::removeConnection invalid id" );
        return;
    }

    bool wasRemovedFromDirectConnect = false;
    bool wasRemovedFromRelayed = false;
    VxGUID& onlineId = groupieId.getUserOnlineId();
    ConnectId connectId( sktConnectId, groupieId );

    bool wasOnline = isUserOnline( onlineId );

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::removeConnection start skt id %s cnt users %d direct %d relayed %d pairs %d", 
            sktConnectId.toHexString().c_str(), m_OnlineIdListList.size(), m_ConnectIdList.size(), m_RelayedIdList.size(),
            m_OnlineConnectionPairs.size() );

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::removeConnection ConnectId %s", connectId.describeConnectId().c_str() );

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    auto iter = m_ConnectIdList.find( connectId );
    if( iter != m_ConnectIdList.end() )
    {
        m_ConnectIdList.erase( iter );
        wasRemovedFromDirectConnect = true;
    }

    auto iterRelayed = m_RelayedIdList.find( connectId );
    if( iterRelayed != m_RelayedIdList.end() )
    {
        m_RelayedIdList.erase( iterRelayed );
        wasRemovedFromRelayed = true;
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    if( wasRemovedFromDirectConnect )
    {
        announceConnectionStatus( connectId, false );
    }

    if( wasRemovedFromRelayed )
    {
        announceRelayStatus( connectId, false );
    }

    removeOnlineConnectionPair( sktConnectId, onlineId );

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::removeConnection end skt id %s cnt users %d direct %d relayed %d pairs %d", 
            sktConnectId.toHexString().c_str(), m_OnlineIdListList.size(), m_ConnectIdList.size(), m_RelayedIdList.size(),
            m_OnlineConnectionPairs.size() );

    if( wasOnline )
    {
        if( !findAnyUserOnlineConnection( onlineId ) )
        {
            bool wasRemoved{ false };
            lockOnlineIdList();
            auto iter = m_OnlineIdListList.find( onlineId );
            if( iter != m_OnlineIdListList.end() )
            {
                m_OnlineIdListList.erase( iter );
                wasRemoved = true;
            }

            unlockOnlineIdList();
            if( wasRemoved )
            {
                announceOnlineStatus( onlineId, false );
            }
        }
    }
}

//============================================================================
void ConnectIdListMgr::addConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason )
{
    lockConnectReasonList();
    auto iter = m_ConnectReasonList.find( sktConnectId );
    if( iter != m_ConnectReasonList.end() )
    {
        iter->second.insert( connectReason );
    }
    else
    {
        std::set<EConnectReason> reasonSet{ connectReason };
        m_ConnectReasonList[sktConnectId] = reasonSet;
    }

    unlockConnectReasonList();
    announceConnectionReason( sktConnectId, connectReason, true );
}

//============================================================================
void ConnectIdListMgr::removeConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason )
{
    bool wasRemoved{ false };
    lockConnectReasonList();
    auto iter = m_ConnectReasonList.find( sktConnectId );
    if( iter != m_ConnectReasonList.end() )
    {
        auto iterReason = iter->second.find( connectReason );
        if( iterReason != iter->second.end() )
        {
            iter->second.erase( iterReason );
        }
    }

    unlockConnectReasonList();
    if( wasRemoved )
    {
        announceConnectionReason( sktConnectId, connectReason, false );
    } 
}

//============================================================================
bool ConnectIdListMgr::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )
{
    if( sktBase->isTempConnection() )
    {
        return false;
    }

    return onConnectionLost( sktBase->getSocketId() );
}

//============================================================================
bool ConnectIdListMgr::onConnectionLost( VxGUID& sktConnectId )
{
    if( !sktConnectId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::onConnectionLost invalid id" );
        return false;
    }

    if( isConnectIdExcluded( sktConnectId ) )
    {
        setExcludeConnectId( sktConnectId, false );
        return false;
    }

    if( !m_OnlineIdListList.size() && !m_ConnectIdList.size() && !m_RelayedIdList.size() && !m_OnlineConnectionPairs.size() )
    {
        // nothing to remove
        return false;
    }

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::onConnectionLost start skt id %s cnt users %d direct %d relayed %d pairs %d", 
            sktConnectId.toHexString().c_str(), m_OnlineIdListList.size(), m_ConnectIdList.size(), m_RelayedIdList.size(),
            m_OnlineConnectionPairs.size() );
  
    std::set<VxGUID> userList;
    std::set<ConnectId> lostConnectList;
    std::set<ConnectId> lostRelayList;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    auto iter = m_ConnectIdList.begin();
    while( iter != m_ConnectIdList.end() )
    {
        ConnectId& connectId = const_cast<ConnectId&>(*iter);
        if( connectId.getSocketId() == sktConnectId )
        {
            userList.insert( connectId.getUserOnlineId() );
            userList.insert( connectId.getHostOnlineId() );
            lostConnectList.insert( connectId );
            iter = m_ConnectIdList.erase( iter );
        }
        else
        {
            ++iter;
        }
    }

    auto iterRelay = m_RelayedIdList.begin();
    while( iterRelay != m_RelayedIdList.end() )
    {
        ConnectId& connectId = const_cast<ConnectId&>(*iterRelay);
        if( connectId.getSocketId() == sktConnectId )
        {
            userList.insert( connectId.getUserOnlineId() );
            userList.insert( connectId.getHostOnlineId() );
            lostRelayList.insert( connectId );
            iterRelay = m_RelayedIdList.erase( iterRelay );
        }
        else
        {
            ++iterRelay;
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    for( auto& connectId : lostConnectList )
    {
        announceConnectionStatus( const_cast< ConnectId& >( connectId ), false );
    }

    for( auto& connectId : lostRelayList )
    {
        announceRelayStatus( const_cast<ConnectId&>(connectId), false );
    }

    removeOnlineConnectionPairs( sktConnectId, userList );

    for( auto& onlineId : userList )
    {
        if( onlineId == m_Engine.getMyOnlineId() )
        {
            continue;
        }

        if( !findAnyUserOnlineConnection( onlineId ) )
        {
            bool wasRemoved{ false };
            lockOnlineIdList();
            auto iter = m_OnlineIdListList.find( onlineId );
            if( iter != m_OnlineIdListList.end() )
            {
                m_OnlineIdListList.erase( iter );
                wasRemoved = true;
            }

            unlockOnlineIdList();
            if( wasRemoved )
            {
                announceOnlineStatus( const_cast<VxGUID&>(onlineId), false );
            }
        }
    }

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::onConnectionLost end skt id %s cnt users %d direct %d relayed %d pairs %d", 
            sktConnectId.toHexString().c_str(), m_OnlineIdListList.size(), m_ConnectIdList.size(), m_RelayedIdList.size(),
            m_OnlineConnectionPairs.size() );

    announceConnectionLost( sktConnectId );
    return true;
}

//============================================================================
void ConnectIdListMgr::userJoinedHost( VxGUID& sktConnectId, GroupieId& groupieId )
{
    if( !groupieId.isValid() || !sktConnectId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::userJoinedHost invalid id" );
        return;
    }

    LogMsg( LOG_ERROR, "ConnectIdListMgr::userJoinedHost skt id %s groupie %s", 
            sktConnectId.toHexString().c_str(), groupieId.describeGroupieId().c_str() );

    addConnection( sktConnectId, groupieId, false );
}

//============================================================================
void ConnectIdListMgr::userLeftHost( VxGUID& sktConnectId, GroupieId& groupieId )
{
    if( !groupieId.isValid() || !sktConnectId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::userJoinedHost invalid id" );
        return;
    }

    LogMsg( LOG_ERROR, "ConnectIdListMgr::userLeftHost skt id %s groupie %s", 
            sktConnectId.toHexString().c_str(), groupieId.describeGroupieId().c_str() );

    removeConnection( sktConnectId, groupieId );
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findHostConnection( GroupieId& groupieId, bool tryPeerFirst )
{
    // host connection can only be a direct connection
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::%s invalid id", __func__ );
        return std::shared_ptr<VxSktBase>();
    }

    std::shared_ptr<VxSktBase> sktBase( nullptr );
    if( groupieId.isValid() )
    {
        if( groupieId.getHostOnlineId() == m_Engine.getMyOnlineId() && groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
        {
            return m_Engine.getSktLoopback();
        }

        if( tryPeerFirst )
        {
            sktBase = findPeerConnection( groupieId.getUserOnlineId() );
        }

        if( !sktBase )
        {
            VxGUID connectId;
            if( findConnectionId( groupieId, connectId ) )
            {
                sktBase = findSktBase( connectId );
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::%s invalid groupieId", __func__ );
    }

    return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findAnyHostConnection( EHostType hostType )
{
    if( !IsHostARelayForUsers( hostType ) )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::%s invalid host type %d", __func__, hostType );
        return std::shared_ptr<VxSktBase>();
    }

    std::shared_ptr<VxSktBase> sktBase( nullptr );
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectIdConst : m_ConnectIdList )
    {
        ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
        //if( connectId.getUserOnlineId() == onlineId && IsHostARelayForUsers( connectId.getHostType() ) )
        if( connectId.getHostType() == hostType )
        {
            sktBase = findSktBase( connectId.getSocketId() );
            if( sktBase && sktBase->isConnected() )
            {
                break;
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findRelayMemberConnection( VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::findUserConnection invalid id" );
        return std::shared_ptr<VxSktBase>();
    }

    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return m_Engine.getSktLoopback();
    }

    std::shared_ptr<VxSktBase> sktBase( nullptr );
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectIdConst : m_ConnectIdList )
    {
        ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
        //if( connectId.getUserOnlineId() == onlineId && IsHostARelayForUsers( connectId.getHostType() ) )
        if( connectId.getUserOnlineId() == onlineId )
        {
            sktBase = findSktBase( connectId.getSocketId() );
            if( sktBase && sktBase->isConnected() )
            {
                break;
            }
        }
    }

    if( !sktBase )
    {
        for( auto& connectIdConst : m_RelayedIdList )
        {
            ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
            //if( connectId.getUserOnlineId() == onlineId && IsHostARelayForUsers( connectId.getHostType() ) )
            if( connectId.getUserOnlineId() == onlineId )
            {
                sktBase = findSktBase( connectId.getSocketId() );
                if( sktBase && sktBase->isConnected() )
                {
                    break;
                }
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    if( sktBase )
    {
        LogModule( eLogRelay, LOG_VERBOSE, "ConnectIdListMgr::findRelayMemberConnection found connection %s %s for online id %s", 
                   sktBase->getSocketIdText().c_str(), sktBase->describeSktConnection().c_str(), onlineId.toOnlineIdString().c_str() );
    }
    else
    {
        LogModule( eLogRelay, LOG_VERBOSE, "ConnectIdListMgr::findRelayMemberConnection no connection for online id %s", 
                   onlineId.toOnlineIdString().c_str() );
    }

    return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findPeerConnection( VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::findPeerConnection invalid id" );
        return nullptr;
    }

    std::shared_ptr<VxSktBase> sktBase( nullptr );
    GroupieId groupieIdDirect( onlineId, onlineId, eHostTypePeerUser );

    VxGUID connectId;
    if( findConnectionId( groupieIdDirect, connectId ) )
    {
        sktBase = findSktBase( connectId );
    }
    else if( findRelayConnectionId( onlineId, connectId ) )
    {
        sktBase = findSktBase( connectId );
    }

    return sktBase;
}

//============================================================================
bool ConnectIdListMgr::findConnectionId( GroupieId& groupieId, VxGUID& retSktConnectId )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::findConnectionId invalid id" );
        return false;
    }

    bool foundConnection = false;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast< ConnectId& >( connectId ).getGroupieId() == groupieId )
        {
            retSktConnectId = const_cast< ConnectId& >( connectId ).getSocketId();
            foundConnection = retSktConnectId.isVxGUIDValid();
            break;
        }
    }

    if( !foundConnection )
    {
        for( auto& connectId : m_RelayedIdList )
        {
            if( const_cast<ConnectId&>(connectId).getGroupieId() == groupieId )
            {
                retSktConnectId = const_cast<ConnectId&>(connectId).getSocketId();
                foundConnection = retSktConnectId.isVxGUIDValid();
                break;
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return foundConnection;
}

//============================================================================
bool ConnectIdListMgr::findRelayConnectionId( VxGUID& onlineId, VxGUID& retSktConnectId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::findRelayConnectionId invalid id" );
        return false;
    }

    bool foundConnection = false;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_RelayedIdList )
    {
        if( const_cast<ConnectId&>(connectId).getUserOnlineId() == onlineId )
        {
            retSktConnectId = const_cast<ConnectId&>(connectId).getSocketId();
            foundConnection = retSktConnectId.isVxGUIDValid();
            break;
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return foundConnection;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findSktBase( VxGUID& connectId )
{
    #if defined(DEBUG_SKT_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockSktBaseMgr", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    m_Engine.getPeerMgr().lockSktBaseMgr();
    #if defined(DEBUG_SKT_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockSktBaseMgr done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<VxSktBase> sktBase = m_Engine.getPeerMgr().findSktBase( connectId );
    if( sktBase )
    {
        sktBase = sktBase->isConnected() ? sktBase : nullptr;
    }

    #if defined(DEBUG_SKT_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockSktBaseMgr", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    m_Engine.getPeerMgr().unlockSktBaseMgr();
    return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findAnyHostOnlineConnection( VxGUID onlineId )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return m_Engine.getSktLoopback();
    }

    std::shared_ptr<VxSktBase> sktBase( nullptr );
    if( isUserExcluded( onlineId ) )
    {
        return sktBase;
    }

    std::set<VxGUID> sktConnectIdList;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast<ConnectId& >( connectId ).getGroupieId().getHostOnlineId() == onlineId )
        {
            sktConnectIdList.insert( const_cast< ConnectId& >( connectId ).getSocketId() );
        }
    }

    if( sktConnectIdList.empty() )
    {
        for( auto& connectId : m_RelayedIdList )
        {
            if( const_cast<ConnectId&>(connectId).getGroupieId().getHostOnlineId() == onlineId )
            {
                sktConnectIdList.insert( const_cast<ConnectId&>(connectId).getSocketId() );
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    for( auto sktConnectId : sktConnectIdList )
    {
        sktBase = findSktBase( sktConnectId );
        if( sktBase )
        {
            break;
        }
    }

    if( sktConnectIdList.empty() )
    {
        // check connection pairs
        lockOnlineIdList();
        // make a list of who used the connection and remove the connections from the list
        for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); iter++ )
        {
            if( iter->second == onlineId )
            {
                sktConnectIdList.insert( iter->first );
            }
        }

        unlockOnlineIdList();
    }

    return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findAnyUserOnlineConnection( VxGUID onlineId, EHostType hostType )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return m_Engine.getSktLoopback();
    }

    std::set<VxGUID> sktConnectIdList;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast<ConnectId& >( connectId ).getGroupieId().getUserOnlineId() == onlineId )
        {
            if( eHostTypeUnknown == hostType || connectId.getHostType() == hostType )
            {
                sktConnectIdList.insert( const_cast< ConnectId& >( connectId ).getSocketId() );
            }
        }
    }

    if( sktConnectIdList.empty() )
    {
        for( auto& connectId : m_RelayedIdList )
        {
            if( const_cast<ConnectId&>(connectId).getGroupieId().getUserOnlineId() == onlineId )
            {
                if( eHostTypeUnknown == hostType || connectId.getHostType() == hostType )
                {
                    sktConnectIdList.insert( const_cast<ConnectId&>(connectId).getSocketId() );
                }
            }
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    if( sktConnectIdList.empty() )
    {
        // check connection pairs
        lockOnlineIdList();
        // make a list of who used the connection and remove the connections from the list
        for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); iter++ )
        {
            if( iter->second == onlineId )
            {
                sktConnectIdList.insert( iter->first );
            }
        }

        unlockOnlineIdList();
    }

    std::shared_ptr<VxSktBase> sktBase( nullptr );
    for( auto sktConnectId : sktConnectIdList )
    {
        sktBase = findSktBase( sktConnectId );
        if( sktBase )
        {
            break;
        }
    }

    return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findBestHostOnlineConnection( VxGUID& onlineId )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return m_Engine.getSktLoopback();
    }

    std::vector<ConnectId> connectIdList;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast< ConnectId& >( connectId ).getGroupieId().getHostOnlineId() == onlineId )
        {
            connectIdList.emplace_back( connectId );
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    VxGUID sktConnectId;
    // first check for direct connection
    GroupieId directGroupieId( onlineId, onlineId, eHostTypePeerUser );
    for( auto& connectId : connectIdList )
    {
        if( connectId.getGroupieId() == directGroupieId )
        {
            sktConnectId = connectId.getSocketId();
            break;
        }
    }

    if( sktConnectId.isVxGUIDValid() )
    {
        return findSktBase( sktConnectId );
    }

    return findAnyHostOnlineConnection( onlineId );
}


//============================================================================
std::shared_ptr<VxSktBase> ConnectIdListMgr::findBestUserOnlineConnection( VxGUID& onlineId, EPluginType pluginType )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return m_Engine.getSktLoopback();
    }

    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return m_Engine.getSktLoopback();
    }

    EHostType hostType{ eHostTypeUnknown };
    if( IsPluginARelayForUser( pluginType ) )
    {
        hostType = PluginTypeToHostType( pluginType );
    }
 

    std::vector<ConnectId> connectIdList;
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectId : m_ConnectIdList )
    {
        if( const_cast< ConnectId& >( connectId ).getGroupieId().getUserOnlineId() == onlineId )
        {
            connectIdList.push_back( connectId );
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();

    VxGUID sktConnectId;
    // first check for direct connection
    GroupieId directGroupieId( onlineId, onlineId, eHostTypePeerUser );
    for( auto& connectId : connectIdList )
    {
        if( connectId.getGroupieId() == directGroupieId )
        {
            sktConnectId = connectId.getSocketId();
            break;
        }
    }

    if( sktConnectId.isVxGUIDValid() )
    {
        return findSktBase( sktConnectId );
    }

    return findAnyUserOnlineConnection( onlineId, hostType );
}

//============================================================================
void ConnectIdListMgr::wantConnectIdListCallback( ConnectIdListCallback* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "ConnectIdListMgr::wantConnectIdListCallback null client" );
        vx_assert( false );
        return;
    }

    lockConnectIdClientList();
    bool foundClient{ false };
    for( auto iter = m_ConnectIdCallbackClients.begin(); iter != m_ConnectIdCallbackClients.end(); ++iter )
    {
        if( *iter == client )
        {
            foundClient = true;
            if( !enable )
            {
                m_ConnectIdCallbackClients.erase( iter );
            }

            break;
        }
    }

    if( !foundClient && enable )
    {
        m_ConnectIdCallbackClients.emplace_back( client );
    }

    unlockConnectIdClientList();
}

//============================================================================
void ConnectIdListMgr::announceOnlineStatus( VxGUID& onlineId, bool isOnline )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::announceOnlineStatus was myself online ? %d",isOnline );
        return;
    }

    LogModule( eLogUserEvent, LOG_VERBOSE, "ConnectIdListMgr::announceOnlineStatus online %d user %s",
                isOnline, m_Engine.describeUser( onlineId ).c_str() );

    m_Engine.getMemberActiveMgr().callbackOnlineStatusChange( onlineId, isOnline );
    m_Engine.getUserJoinMgr().callbackOnlineStatusChange( onlineId, isOnline );

    lockConnectIdClientList();

    for( auto& client : m_ConnectIdCallbackClients )
    {
        if( client )
        {
            client->callbackOnlineStatusChange( onlineId, isOnline );
        }
        else
        {
            LogMsg( LOG_ERROR, "ConnectIdListMgr::announceOnlineStatus null client" );
        }    
    }

    unlockConnectIdClientList();

    lockOnlineStatusClientList();

    for( auto& client : m_OnlineStatusCallbackClients )
    {
        if( client )
        {
            client->callbackOnlineStatusChange( onlineId, isOnline );
        }
        else
        {
            LogMsg( LOG_ERROR, "ConnectIdListMgr::announceOnlineStatus null client" );
        }    
    }

    unlockOnlineStatusClientList();

    m_Engine.getPluginMgr().onContactOnlineStatusChange( onlineId, isOnline );
    if( !isOnline )
    {
        m_Engine.getHostJoinMgr().onUserOffline( onlineId );
        m_Engine.getGroupieListMgr().onUserOffline( onlineId );
        m_Engine.getUserOnlineMgr().onUserOffline( onlineId );
    }
}

//============================================================================
void ConnectIdListMgr::announceConnectionStatus( ConnectId& connectId, bool isConnected )
{
    lockConnectIdClientList();

    for( auto& client : m_ConnectIdCallbackClients )
    {
        if( client )
        {
            client->callbackConnectionStatusChange( connectId, isConnected );
        }
        else
        {
            LogMsg( LOG_ERROR, "ConnectIdListMgr::announceConnectionStatus null client" );
        }
    }

    unlockConnectIdClientList();
}

//============================================================================
void ConnectIdListMgr::announceRelayStatus( ConnectId& connectId, bool isRelayed )
{
    lockConnectIdClientList();

    for( auto& client : m_ConnectIdCallbackClients )
    {
        if( client )
        {
            client->callbackRelayStatusChange( connectId, isRelayed );
        }
        else
        {
            LogMsg( LOG_ERROR, "ConnectIdListMgr::announceRelayStatus null client" );
        }
    }

    unlockConnectIdClientList();
}

//============================================================================
void ConnectIdListMgr::announceConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason, bool enableReason )
{
    lockConnectIdClientList();

    for( auto& client : m_ConnectIdCallbackClients )
    {
        if( client )
        {
            client->callbackConnectionReason( sktConnectId, connectReason, enableReason );
        }
        else
        {
            LogMsg( LOG_ERROR, "ConnectIdListMgr::announceConnectionReason null client" );
        }
    }

    unlockConnectIdClientList();
}

//============================================================================
void ConnectIdListMgr::announceConnectionLost( VxGUID& sktConnectId )
{
    lockConnectIdClientList();

    for( auto& client : m_ConnectIdCallbackClients )
    {
        if( client )
        {
            client->callbackConnectionLost( sktConnectId );
        }
        else
        {
            LogMsg( LOG_ERROR, "ConnectIdListMgr::announceConnectionLost null client" );
        }
    }

    unlockConnectIdClientList();
}

//============================================================================
void ConnectIdListMgr::onGroupUserAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, bool relayed )
{
    if( relayed )
    {
        onGroupRelayedUserAnnounce( pktAnn, sktBase, netIdent );
        return;
    }

    VxGUID onlineId = netIdent->getMyOnlineId();
    VxGUID connectionId = sktBase->getSocketId();
    // if relayed then the peer id should be the host that relayed the packet
    VxGUID hostOnlineId = sktBase->getPeerOnlineId();
    if( onlineId.isVxGUIDValid() && hostOnlineId.isVxGUIDValid() && connectionId.isVxGUIDValid() )
    {
        if( onlineId == hostOnlineId )
        {
            EHostType hostType{ eHostTypeUnknown };

            #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
                LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
            #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            lockConnectIdList();
            #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
                LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
            #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            for( auto& connectIdConst : m_ConnectIdList )
            {
                ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
                if( IsHostARelayForUsers( connectId.getHostType() ) )
                {
                    if( connectId.getHostedId().getHostOnlineId() == hostOnlineId )
                    {
                        hostType = connectId.getHostType();
                        hostOnlineId = connectId.getHostedId().getHostOnlineId();
                        break;
                    }
                }
            }

            #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
                LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
            #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            unlockConnectIdList();

            if( hostType != eHostTypeUnknown )
            {
                LogMsg( LOG_VERBOSE, "ConnectIdListMgr::onGroupUserAnnounce %s from host %s",
                    netIdent->getOnlineName(), sktBase->getPeerOnlineName().c_str() );

                GroupieId groupieId( onlineId, hostOnlineId, hostType );
                addConnection( connectionId, groupieId, relayed );
                m_Engine.getUserOnlineMgr().onUserOnline( groupieId, sktBase, netIdent );
            }
            else
            {
                LogMsg( LOG_WARNING, "ConnectIdListMgr::onGroupUserAnnounce hostId not found" );
            }
        }
        else
        {
            LogMsg( LOG_WARNING, "ConnectIdListMgr::onGroupUserAnnounce onlineId != hostOnlineId" );
        }
    }
    else
    {
        LogMsg( LOG_WARNING, "ConnectIdListMgr::onGroupUserAnnounce invalid id" );
    }
}

//============================================================================
void ConnectIdListMgr::onGroupRelayedUserAnnounce( PktAnnounce* pktAnn, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    VxGUID onlineId = netIdent->getMyOnlineId();
    VxGUID socketId = sktBase->getSocketId();
    // if relayed then the peer id should be the host that relayed the packet
    VxGUID hostOnlineId = sktBase->getPeerOnlineId();

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::onGroupRelayedUserAnnounce from %s id %s hosted by %s id %s",
            netIdent->getOnlineName(), onlineId.describeVxGUID().c_str(), sktBase->getPeerOnlineName().c_str(), hostOnlineId.toOnlineIdString().c_str());
    if( onlineId.isVxGUIDValid() && hostOnlineId.isVxGUIDValid() && socketId.isVxGUIDValid() )
    {
        if( onlineId != hostOnlineId )
        {
            std::set<EHostType> hostTypes;

            #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
                LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
            #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            lockConnectIdList();
            #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
                LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
            #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            for( auto& connectIdConst : m_ConnectIdList )
            {
                ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
                if( connectId.getSocketId() == socketId && IsHostARelayForUsers( connectId.getHostType() ) )
                {
                    if( connectId.getHostedId().getHostOnlineId() == hostOnlineId )
                    {
                        hostTypes.insert( connectId.getHostType() );
                    }
                }
            }

            #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
                LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
            #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
            unlockConnectIdList();

            if( !hostTypes.empty() )
            {
                for( auto hostType : hostTypes )
                {
                    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::onGroupRelayedUserAnnounce %s from host %s %s",
                        netIdent->getOnlineName(), DescribeHostType( hostType ), sktBase->getPeerOnlineName().c_str() );

                    GroupieId groupieId( onlineId, hostOnlineId, hostType );

                    // add the connection first so if user is added it's online status is valid
                    addConnection( socketId, groupieId, true );
                    // next make the user up todate
                    m_Engine.getUserOnlineMgr().onUserOnline( groupieId, sktBase, netIdent );
                    // finally add the user group join info
                    PluginBase* plugin = m_Engine.getPluginMgr().findPlugin( HostTypeToClientPlugin( hostType ) );
                    if( plugin )
                    {
                        plugin->onGroupRelayedUserAnnounce( groupieId, sktBase, netIdent );
                    }
                    else
                    {
                        LogMsg( LOG_VERBOSE, "ConnectIdListMgr::onGroupRelayedUserAnnounce %s from host %s %s Faield to get plugin",
                            netIdent->getOnlineName(), DescribeHostType( hostType ), sktBase->getPeerOnlineName().c_str() );
                    }
                }
            }
            else
            {
                LogMsg( LOG_WARNING, "ConnectIdListMgr::onGroupRelayedUserAnnounce hostId not found" );
            }
        }
        else
        {
            LogMsg( LOG_WARNING, "ConnectIdListMgr::onGroupRelayedUserAnnounce onlineId != hostOnlineId" );
        }
    }
    else
    {
        LogMsg( LOG_WARNING, "ConnectIdListMgr::onGroupRelayedUserAnnounce invalid id" );
    }
}

//============================================================================
void ConnectIdListMgr::getOnlineMembers( HostedId& hostId, std::vector<VxGUID>& onlineIdList )
{
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectIdConst : m_ConnectIdList )
    {
        ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
        if( connectId.getHostedId() == hostId )
        {
            onlineIdList.emplace_back( connectId.getUserOnlineId() );
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
}

//============================================================================
bool ConnectIdListMgr::isMemberOnline( HostedId& hostId, VxGUID& onlineId )
{
    bool isOnline{ false };
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    lockConnectIdList();
    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s lockConnectIdList done", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    for( auto& connectIdConst : m_ConnectIdList )
    {
        ConnectId& connectId = const_cast<ConnectId&>(connectIdConst);
        if( connectId.getHostedId() == hostId && connectId.getUserOnlineId() == onlineId )
        {
            isOnline = true;
            break;
        }
    }

    #if defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
        LogMsg( LOG_DEBUG, "ConnectIdListMgr::%s unlockConnectIdList", __func__ );
    #endif // defined(DEBUG_CONNECT_ID_LIST_MGR_LOCK)
    unlockConnectIdList();
    return isOnline;
}

//============================================================================
void ConnectIdListMgr::pktAnnRecieved( VxGUID& sktConnectId, VxGUID onlineId )
{
    bool shouldAnnounce{ false };
    if( !sktConnectId.isVxGUIDValid() || !onlineId.isVxGUIDValid() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::pktAnnRecieved BAD PARAM" );
        vx_assert( false );
        return;
    }

    if( isUserExcluded( onlineId ) )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::pktAnnRecieved excluded %s", onlineId.toOnlineIdString().c_str() );
        return;
    }

    LogMsg( LOG_VERBOSE, "ConnectIdListMgr::pktAnnRecieved skt connect id %s user %s", sktConnectId.toHexString().c_str(), onlineId.toOnlineIdString().c_str() );

    lockOnlineIdList();
    auto iter = std::find_if( m_OnlineConnectionPairs.begin(), m_OnlineConnectionPairs.end(),
                              [&]( const std::pair< VxGUID, VxGUID>& element ) { return element.first == sktConnectId && element.second == onlineId; } );
    if( iter == m_OnlineConnectionPairs.end() )
    {
        m_OnlineConnectionPairs.emplace_back( std::make_pair( sktConnectId, onlineId ) );
        if( m_OnlineIdListList.find( onlineId ) == m_OnlineIdListList.end() )
        {
            shouldAnnounce = true;
        }
    }

    unlockOnlineIdList();

    if( shouldAnnounce )
    {
        lockOnlineIdList();
        bool wasOnline = m_OnlineIdListList.find( onlineId ) != m_OnlineIdListList.end();
        if( !wasOnline )
        {
            m_OnlineIdListList.insert( onlineId );
        }

        unlockOnlineIdList();

        if( !wasOnline )
        {
            announceOnlineStatus( onlineId, true );
        }
    }
}

//============================================================================
void ConnectIdListMgr::removeOnlineConnectionPairs( VxGUID& sktConnectId, std::set<VxGUID>& lostConnUserList )
{
    if( !sktConnectId.isVxGUIDValid() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::doOnlineIdConnectionLost BAD PARAM" );
        vx_assert( false );
        return;
    }

    std::set<VxGUID> onlineIdList;
    lockOnlineIdList();
    // make a list of who used the connection and remove the connections from the list
    for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); )
    {
        if( iter->first == sktConnectId )
        {
            if( iter->second != m_Engine.getMyOnlineId() )
            {
                onlineIdList.insert( iter->second );
            }
            
            LogModule( eLogUserEvent, LOG_VERBOSE, "ConnectIdListMgr::removeOnlineConnectionPairs removed skt id %s user %s ",
                   sktConnectId.toHexString().c_str(), m_Engine.describeUser( iter->second ).c_str() );
            iter = m_OnlineConnectionPairs.erase( iter );
        }
        else
        {
            iter++;
        }
    }

    // for each user if there is no more connections to user then add to lostConnectiononIdList
    for( auto& onlineId : onlineIdList )
    {
        if( onlineId == m_Engine.getMyOnlineId() )
        {
            continue;
        }

        auto iter = std::find_if( m_OnlineConnectionPairs.begin(), m_OnlineConnectionPairs.end(),
                              [&]( const std::pair< VxGUID, VxGUID>& element ) { return element.second == onlineId; } );
        if( iter == m_OnlineConnectionPairs.end() )
        {
            lostConnUserList.insert( onlineId );
        }
    }

    unlockOnlineIdList();
}

//============================================================================
void ConnectIdListMgr::addOnlineConnectionPair( VxGUID& sktConnectId, VxGUID& onlineId )
{
    bool wasFound{ false };
    lockOnlineIdList();
    for( auto& connectionPair : m_OnlineConnectionPairs )
    {
        if( connectionPair.first == sktConnectId && connectionPair.second == onlineId )
        {
            wasFound = true;
            break;
        }
    }

    if( !wasFound )
    {
        LogModule( eLogUserEvent, LOG_VERBOSE, "ConnectIdListMgr::addOnlineConnectionPair added skt id %s user %s ",
                   sktConnectId.toHexString().c_str(), m_Engine.describeUser( onlineId ).c_str() );
        m_OnlineConnectionPairs.emplace_back( std::make_pair( sktConnectId, onlineId ) );
    }

    unlockOnlineIdList();
}

//============================================================================
void ConnectIdListMgr::removeOnlineConnectionPair( VxGUID& sktConnectId, VxGUID& onlineId )
{
    lockOnlineIdList();
    // make a list of who used the connection and remove the connections from the list
    for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); )
    {
        if( iter->first == sktConnectId && iter->second == onlineId )
        {
            LogModule( eLogUserEvent, LOG_VERBOSE, "ConnectIdListMgr::removeOnlineConnectionPair removed skt id %s user %s ",
                   sktConnectId.toHexString().c_str(), m_Engine.describeUser( onlineId ).c_str() );
            iter = m_OnlineConnectionPairs.erase( iter );
        }
        else
        {
            iter++;
        }
    }

    unlockOnlineIdList();
}

//============================================================================
bool ConnectIdListMgr::isUserOnline( VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::isUserOnline BAD PARAM" );
        vx_assert( false );
        return false;
    }

    lockOnlineIdList();
    bool isOnline = m_OnlineIdListList.find( onlineId ) != m_OnlineIdListList.end();
    unlockOnlineIdList();

    return isOnline;
}


//============================================================================
void ConnectIdListMgr::updateOnlineExclusion( VxGUID onlineId, bool excludeFromOnlineStatus, bool isNetworkHost )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::updateOnlineExclusion BAD PARAM" );
        vx_assert( false );
        return;
    }

    lockOnlineIdList();
    if( isNetworkHost )
    {
        m_NetworkHostOnlineId = onlineId;
    }

    auto iter = m_OnlineIdExclusionList.find( onlineId );
    if( excludeFromOnlineStatus )
    {
        if( iter == m_OnlineIdExclusionList.end() )
        {
            m_OnlineIdExclusionList.insert( onlineId );
        }
    }
    else
    {
        if( iter != m_OnlineIdExclusionList.end() )
        {
            m_OnlineIdExclusionList.erase( iter );
        }
    }

    unlockOnlineIdList();
}

//============================================================================
bool ConnectIdListMgr::isUserExcluded( VxGUID onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::isUserExcluded BAD PARAM" );
        vx_assert( false );
        return true;
    }

    lockOnlineIdList();
    bool isExcluded = m_OnlineIdExclusionList.find( onlineId ) != m_OnlineIdExclusionList.end();
    unlockOnlineIdList();

    return isExcluded;
}

//============================================================================
bool ConnectIdListMgr::isConnectionInUse( VxGUID& sktConnectId )
{
    if( !sktConnectId.isVxGUIDValid() )
    {
        LogModule( eLogConnect, LOG_VERBOSE, "ConnectIdListMgr::isConnectionInUse BAD PARAM" );
        vx_assert( false );
        return false;
    }

    bool sktInUse{ false };
    lockOnlineIdList();
    auto iter = std::find_if( m_OnlineConnectionPairs.begin(), m_OnlineConnectionPairs.end(),
                              [&]( const std::pair< VxGUID, VxGUID>& element ) { return element.first == sktConnectId; } );
    if( iter != m_OnlineConnectionPairs.end() )
    {
         sktInUse = true;
    }

    unlockOnlineIdList();

    return sktInUse;
}

//============================================================================
void ConnectIdListMgr::disconnectIfIsOnlyUser( GroupieId& groupieId )
{
    VxGUID userOnlineId = groupieId.getUserOnlineId();

    std::set<VxGUID> userSktConnectIds;
    std::vector<VxGUID> toDisconnectIds;

    lockOnlineIdList();
    for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); ++iter )
    {
        if( iter->second == userOnlineId )
        {
            userSktConnectIds.insert( iter->first );
        }
    }

    for( auto sktConnectId : userSktConnectIds )
    {
        bool inUseByOther{ false };
        for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); ++iter )
        {
            if( iter->first == sktConnectId && iter->second != userOnlineId )
            {
                inUseByOther = true;
                break;
            }
        }

        if( !inUseByOther )
        {
            toDisconnectIds.emplace_back( sktConnectId );
        }
    }

    unlockOnlineIdList();

    for( auto& sktConnectId : toDisconnectIds )
    {
        m_Engine.getPeerMgr().closeConnection( sktConnectId, eSktCloseNotNeeded );
    }
}

//============================================================================
void ConnectIdListMgr::setExcludeConnectId( VxGUID& sktConnectId, bool exclude )
{
    lockExclusionList();

    auto iter = m_ConnectIdExclusionList.find( sktConnectId );
    if( exclude )
    {
        if( iter == m_ConnectIdExclusionList.end() )
        {
            m_ConnectIdExclusionList.insert( sktConnectId );
        }
    }
    else
    {
        if( iter != m_ConnectIdExclusionList.end() )
        {
            m_ConnectIdExclusionList.erase( iter );
        }
    }

    unlockExclusionList();
}

//============================================================================
bool ConnectIdListMgr::isConnectIdExcluded( VxGUID& sktConnectId )
{
    lockExclusionList();
    bool isExcluded = m_ConnectIdExclusionList.find( sktConnectId ) != m_ConnectIdExclusionList.end();
    unlockExclusionList();

    return isExcluded;
}


//============================================================================
void ConnectIdListMgr::fromGuiDisconnectFromUser( VxGUID& userOnlineId )
{
	std::set<VxGUID> userSktConnectIds;
    std::vector<VxGUID> toDisconnectIds;

    lockOnlineIdList();
    for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); ++iter )
    {
        if( iter->second == userOnlineId )
        {
            userSktConnectIds.insert( iter->first );
        }
    }

    for( auto sktConnectId : userSktConnectIds )
    {
        bool inUseByOther{ false };
        for( auto iter = m_OnlineConnectionPairs.begin(); iter != m_OnlineConnectionPairs.end(); ++iter )
        {
            if( iter->first == sktConnectId && iter->second != userOnlineId )
            {
                inUseByOther = true;
                break;
            }
        }

        if( !inUseByOther )
        {
            toDisconnectIds.emplace_back( sktConnectId );
        }
    }

    unlockOnlineIdList();

    for( auto& sktConnectId : toDisconnectIds )
    {
        m_Engine.getPeerMgr().closeConnection( sktConnectId, eSktCloseClosedByUser );
    }
}
