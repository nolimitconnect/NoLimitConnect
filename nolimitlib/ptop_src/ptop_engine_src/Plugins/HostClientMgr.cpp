//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "HostClientMgr.h"
#include "PluginBase.h"

#include <GuiInterface/IToGui.h>
#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>
#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/BaseInfo/BaseSessionInfo.h>
#include <ptop_src/ptop_engine_src/UserJoinMgr/UserJoinMgr.h>
#include <ptop_src/ptop_engine_src/UserOnlineMgr/UserOnlineMgr.h>

#include <PktLib/PktsHostJoin.h>
#include <PktLib/PktsHostSearch.h>
#include <PktLib/PluginIdList.h>
#include <PktLib/PktsHostUser.h>

#include <CoreLib/VxTime.h>
#include <NetLib/VxSktBase.h>

//============================================================================
HostClientMgr::HostClientMgr( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, PluginBase& pluginBase )
    : HostClientSearchMgr( engine, pluginMgr, myIdent, pluginBase )
{
}

//============================================================================
void HostClientMgr::onPktHostJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostJoinReply* hostReply = ( PktHostJoinReply* )pktHdr;
    if( hostReply->isValidPkt() )
    {
        if( eHostTypeUnknown == hostReply->getHostType() || hostReply->getHostType() != getHostType() )
        {
            LogMsg( LOG_ERROR, "HostClientMgr::onPktHostJoinReply invalid host type" );
            return;
        }

        GroupieId groupieId( hostReply->getGroupieId() );
        if( !m_Engine.getConnectIdListMgr().addConnection( sktBase, groupieId ) )
        {
            LogMsg( LOG_ERROR, "HostClientMgr::onPktHostJoinReply addConnection failed" );
            return;
        }

        HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, hostReply->getSessionId() );

        if( ePluginAccessOk == hostReply->getAccessState() )
        {
            m_Engine.getBigListMgr().updateMemberFriendship( groupieId.getUserOnlineId() );
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinSuccess );
            BaseSessionInfo sessionInfo( hostUserSessionId );
            onUserJoinHostGranted( groupieId, sktBase, netIdent, sessionInfo );
        }
        else if( ePluginAccessLocked == hostReply->getAccessState() )
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinFailPermission );
            if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
            {
                m_Engine.getConnectionMgr().doneWithConnection( hostReply->getSessionId(), groupieId.getUserOnlineId(), this, HostTypeToConnectJoinReason( hostReply->getHostType() ) );
            }
        }
        else
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinFail, DescribePluginAccess( hostReply->getAccessState() ) );
            if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
            {
                m_Engine.getConnectionMgr().doneWithConnection( hostReply->getSessionId(), groupieId.getUserOnlineId(), this, HostTypeToConnectJoinReason( hostReply->getHostType() ) );
            }
        }
    }
    else
    {
        onInvalidRxedPacket( sktBase, pktHdr, netIdent );     
    }
}

//============================================================================
void HostClientMgr::onPktHostLeaveReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostLeaveReply* hostReply = ( PktHostLeaveReply* )pktHdr;
    if( hostReply->isValidPkt() )
    {
        if( eHostTypeUnknown == hostReply->getHostType() || hostReply->getHostType() != getHostType() )
        {
            LogMsg( LOG_ERROR, "HostClientMgr::onPktHostLeaveReply invalid host type" );
        }

        GroupieId groupieId( hostReply->getGroupieId() );
        m_Engine.getConnectIdListMgr().removeConnection( sktBase->getSocketId(), groupieId );
        HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, hostReply->getSessionId() );

        if( ePluginAccessOk == hostReply->getAccessState() )
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinUnJoinSuccess );
            BaseSessionInfo sessionInfo( hostUserSessionId );
            onUserLeftHost( groupieId, sktBase, netIdent, sessionInfo );
        }
        else if( ePluginAccessLocked == hostReply->getAccessState() )
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinFailPermission );
            if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
            {
                m_Engine.getConnectionMgr().doneWithConnection( hostReply->getSessionId(), groupieId.getUserOnlineId(), this, HostTypeToConnectJoinReason( hostReply->getHostType() ) );
            }
        }
        else
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinFail, DescribePluginAccess( hostReply->getAccessState() ) );
            if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
            {
                m_Engine.getConnectionMgr().doneWithConnection( hostReply->getSessionId(), groupieId.getUserOnlineId(), this, HostTypeToConnectJoinReason( hostReply->getHostType() ) );
            }
        }
    }
    else
    {
        onInvalidRxedPacket( sktBase, pktHdr, netIdent );
    }
}

//============================================================================
void HostClientMgr::onPktHostUnJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostUnJoinReply* hostReply = ( PktHostUnJoinReply* )pktHdr;
    if( hostReply->isValidPkt() )
    {
        if( eHostTypeUnknown == hostReply->getHostType() || hostReply->getHostType() != getHostType() )
        {
            LogMsg( LOG_ERROR, "HostClientMgr::onPktHostUnJoinReply invalid host type" );
        }

        GroupieId groupieId( hostReply->getGroupieId() );
        m_Engine.getConnectIdListMgr().removeConnection( sktBase->getSocketId(), groupieId );
        HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, hostReply->getSessionId() );

        if( ePluginAccessOk == hostReply->getAccessState() )
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinUnJoinSuccess );
            BaseSessionInfo sessionInfo( hostUserSessionId );
            onUserUnJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
        }
        else if( ePluginAccessLocked == hostReply->getAccessState() )
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinFailPermission );
            if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
            {
                m_Engine.getConnectionMgr().doneWithConnection( hostReply->getSessionId(), groupieId.getUserOnlineId(), this, HostTypeToConnectJoinReason( hostReply->getHostType() ) );
            }
        }
        else
        {
            m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinFail, DescribePluginAccess( hostReply->getAccessState() ) );
            if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
            {
                m_Engine.getConnectionMgr().doneWithConnection( hostReply->getSessionId(), groupieId.getUserOnlineId(), this, HostTypeToConnectJoinReason( hostReply->getHostType() ) );
            }
        }
    }
    else
    {
        onInvalidRxedPacket( sktBase, pktHdr, netIdent );
    }
}

//============================================================================
void HostClientMgr::onPktHostSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostSearchReply* hostReply = ( PktHostSearchReply* )pktHdr;
    ECommErr commErr = hostReply->getCommError();
    if( 0 == hostReply->getTotalMatches() )
    {
        if( eCommErrNone != commErr )
        {
            if( eCommErrPluginNotEnabled == commErr )
            {
                m_Engine.getToGui().toGuiHostSearchStatus( hostReply->getHostType(), netIdent->getMyOnlineId(), eHostSearchPluginDisabled, hostReply->getCommError() );
            }
            else if( eCommErrPluginPermission == commErr )
            {
                m_Engine.getToGui().toGuiHostSearchStatus( hostReply->getHostType(), netIdent->getMyOnlineId(), eHostSearchFailPermission, hostReply->getCommError() );
            }
            else
            {
                m_Engine.getToGui().toGuiHostSearchStatus( hostReply->getHostType(), netIdent->getMyOnlineId(), eHostSearchFail, hostReply->getCommError() );
            }
        }
        else
        {
            m_Engine.getToGui().toGuiHostSearchStatus( hostReply->getHostType(), netIdent->getMyOnlineId(), eHostSearchNoMatches, hostReply->getCommError() );
        }

        LogModule( eLogHostSearch, LOG_DEBUG, "HostClientMgr::onPktHostSearchReply no matches" );
        stopHostSearch( hostReply->getHostType(), hostReply->getSearchSessionId(), sktBase, netIdent->getMyOnlineId() );
    }
    else
    {
        startHostDetailSession( hostReply, sktBase, netIdent );
    }
}

//============================================================================
void HostClientMgr::onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    m_ServerListMutex.lock();
    m_ServerList.insert( groupieId );
    m_ServerListMutex.unlock();

    m_Engine.getUserJoinMgr().onUserJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
    m_Engine.getUserOnlineMgr().onUserJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
    m_Engine.getThumbMgr().queryThumbIfNeeded( sktBase, netIdent, sessionInfo.getHostPluginType() );
}

//============================================================================
void HostClientMgr::onUserJoinHostGranted( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    m_ServerListMutex.lock();
    m_ServerList.insert( groupieId );
    m_ServerListMutex.unlock();

    m_Engine.getUserJoinMgr().onUserJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
    m_Engine.getUserOnlineMgr().onUserJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
    m_Engine.getThumbMgr().queryThumbIfNeeded( sktBase, netIdent, sessionInfo.getHostPluginType() );

    if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
    {
        // request a list of everybody because we just joined
        clearUserInfoRequests();

        PktHostUserListReq pktReq;
        pktReq.setHostType( sessionInfo.getHostType() );
        pktReq.setHostOnlineId( sessionInfo.getHostOnlineId() );
        pktReq.setSearchSessionId( sessionInfo.getSessionId() );

        LogModule( eLogHostConnect, LOG_INFO, "HostClientMgr::onUserJoinHostGranted to me" );
        if( !m_Plugin.txPacket( netIdent, sktBase, &pktReq ) )
        {
            LogModule( eLogHostConnect, LOG_INFO, "HostClientMgr::txPkt PktHostUserListReq failed" );
        }
    }
    else
    {
        // request info about the person who just joined
        PktHostUserInfoReq pktReq;
        pktReq.setGroupieId( groupieId );

        LogModule( eLogHostConnect, LOG_INFO, "HostClientMgr::onUserJoinHostGranted to other user" );
        if( !m_Plugin.txPacket( netIdent, sktBase, &pktReq ) )
        {
            LogModule( eLogHostConnect, LOG_INFO, "HostClientMgr::txPkt PktHostUserInfoReq failed" );
        }
    }
}

//============================================================================
void HostClientMgr::onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    m_ServerListMutex.lock();
    m_ServerList.erase( groupieId );
    m_ServerListMutex.unlock();

    m_Engine.getUserJoinMgr().onUserLeftHost( groupieId, sktBase, netIdent, sessionInfo );
    m_Engine.getUserOnlineMgr().onUserLeftHost( groupieId, sktBase, netIdent, sessionInfo );
}

//============================================================================
void HostClientMgr::onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    m_ServerListMutex.lock();
    m_ServerList.erase( groupieId );
    m_ServerListMutex.unlock();

    m_Engine.getUserJoinMgr().onUserUnJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
    m_Engine.getUserOnlineMgr().onUserUnJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
}

//============================================================================
void HostClientMgr::sendHostSearchToNetworkHost( VxGUID& sessionId, SearchParams& searchParams, EConnectReason connectReason )
{
    // save announce pkt in announce session list
    std::string url = m_ConnectionMgr.getDefaultHostUrl( eHostTypeNetwork );
    if( url.empty() )
    {
        LogMsg( LOG_VERBOSE, "HostServerMgr network host url is empty" );
        return;
    }

    addSearchSession( sessionId, searchParams );
    connectToHost( eHostTypeNetwork, sessionId, url, connectReason );
}

//============================================================================
void HostClientMgr::addPluginRxSession( VxGUID& sessionId, PluginIdList& pluginIdList )
{
    removePluginRxSession( sessionId );
    m_PluginRxListMutex.lock();
    m_PluginRxList[sessionId] = pluginIdList;
    m_PluginRxListMutex.unlock();
}

//============================================================================
void HostClientMgr::removePluginRxSession( VxGUID& sessionId )
{
    m_PluginRxListMutex.lock();
    auto iter = m_PluginRxList.find( sessionId );
    if( iter != m_PluginRxList.end() )
    {
        m_PluginRxList.erase( iter );
    }

    m_PluginRxListMutex.unlock();
}

//============================================================================
void HostClientMgr::removeSession( VxGUID& sessionId, EConnectReason connectReason )
{
    if( isSearchConnectReason( connectReason ) )
    {
        removeSearchSession( sessionId );
    }

    HostClientSearchMgr::removeSession( sessionId, connectReason );
}

//============================================================================
void HostClientMgr::onContactDisconnected( VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& onlineId, EConnectReason connectReason )
{
    GroupieId groupieId( m_Engine.getMyOnlineId(), onlineId, getHostType() );
    m_ServerList.erase( groupieId );
    removeContact( onlineId );
}

//============================================================================
bool HostClientMgr::onConnectToHostSuccess( EHostType hostType, VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& onlineId, EConnectReason connectReason )
{
    bool result{ false };
    if( isSearchConnectReason( connectReason ) )
    {
        m_Engine.getToGui().toGuiHostSearchStatus( hostType, sessionId, eHostSearchConnectSuccess );
    }

    if( hostType == eHostTypeNetwork &&
        ( connectReason == eConnectReasonChatRoomSearch ||
            connectReason == eConnectReasonGroupSearch ||
            connectReason == eConnectReasonRandomConnectSearch ) )
    {
        m_Engine.getToGui().toGuiHostSearchStatus( hostType, sessionId, eHostSearchSendingSearchRequest );

        m_SearchParamsMutex.lock();
        auto iter = m_SearchParamsList.find( sessionId );
        if( iter != m_SearchParamsList.end() )
        {
            SearchParams& searchParams = iter->second;
            PktHostSearchReq searchReq;
            searchReq.setHostType( searchParams.getHostType() );
            searchReq.setPluginType( m_Plugin.getPluginType() );
            searchReq.setSearchSessionId( sessionId );
            PktBlobEntry& blobEntry = searchReq.getBlobEntry();
            bool result = searchParams.addToBlob( blobEntry );
            searchReq.calcPktLen();
            // unlock before txPacket else in looback mode can cause a deadlock
            m_SearchParamsList.erase( iter );
            m_SearchParamsMutex.unlock();

            if( result && searchReq.isValidPkt() )
            {
                if( !m_Plugin.txPacket( onlineId, sktBase, &searchReq, false, m_Plugin.getDestinationPluginOverride( hostType ) ) )
                {
                    LogModule( eLogHostSearch, LOG_DEBUG, "HostClientMgr::onConnectToHostSuccess failed send PktHostSearchReq" );
                }
                else
                {
                    result = true;
                }
            }
            else
            {
                LogMsg( LOG_ERROR, "HostServerMgr PktHostSearchReq is invalid" );
            }

            // not done with connection.. wait for search results
        }
        else
        {
            m_SearchParamsMutex.unlock();
            LogMsg( LOG_ERROR, "HostServerMgr Search Params Not Found" );
            stopHostSearch( hostType, sessionId, sktBase, onlineId );
        }     
    }
    else
    {
        HostBaseMgr::onConnectToHostSuccess( hostType, sessionId, sktBase, onlineId, connectReason );
    }

    return result;
}

//============================================================================
void HostClientMgr::startHostDetailSession( PktHostSearchReply* hostReply, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    EHostType hostType = hostReply->getHostType();
    VxGUID sessionId = hostReply->getSearchSessionId();
    int pluginIdCnt = hostReply->getTotalMatches();
    ECommErr commErr = hostReply->getCommError();
    if( eCommErrNone != commErr )
    {
        LogModule( eLogHostSearch, LOG_DEBUG, "HostClientMgr::startHostDetailSession comm error %s", DescribeCommError( commErr ) );
        if( eCommErrPluginNotEnabled == commErr )
        {
            m_Engine.getToGui().toGuiHostSearchStatus( hostType, netIdent->getMyOnlineId(), eHostSearchPluginDisabled );
        }

        stopHostSearch( hostReply->getHostType(), hostReply->getSearchSessionId(), sktBase, netIdent->getMyOnlineId() );
        return;
    }

    if( !sessionId.isVxGUIDValid() )
    {
        LogModule( eLogHostSearch, LOG_DEBUG, "HostClientMgr::startHostDetailSession session id invalid");
    }

    bool result = sessionId.isVxGUIDValid() && pluginIdCnt > 0 && eCommErrNone == commErr;
    if( result )
    {
        // insert ids and send first request for plugin settings
        PluginIdList pluginIdList;
        PktBlobEntry& blobEntry = hostReply->getBlobEntry();
        blobEntry.resetRead();
        for( int i = 0; i < pluginIdCnt; i++ )
        {
            PluginId pluginId;
            if( blobEntry.getValue( pluginId ) )
            {
                pluginIdList.addPluginId( pluginId );
            }
            else
            {
                LogModule( eLogHostSearch, LOG_DEBUG, "HostClientMgr::startHostDetailSession error getting plug id at index %d", i);
                result = false;
                break;
            }
        }

        if( result )
        {
            addPluginRxSession( sessionId, pluginIdList );
        }
    }

    if( !result )
    {
        LogModule( eLogHostSearch, LOG_DEBUG, "HostClientMgr::startHostDetailSession failed");
        stopHostSearch( hostReply->getHostType(), hostReply->getSearchSessionId(), sktBase, netIdent->getMyOnlineId() );
    }
}

//============================================================================
bool HostClientMgr::stopHostSearch( EHostType hostType, VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& onlineId )
{
    m_Engine.getToGui().toGuiHostSearchStatus( hostType, onlineId, eHostSearchCompleted );
    m_Engine.getToGui().toGuiHostSearchComplete( hostType, onlineId );

    removeSearchSession( sessionId );
    EConnectReason connectReason = getSearchConnectReason(hostType);
    m_Engine.getConnectionMgr().doneWithConnection( sessionId, onlineId, this, connectReason );
    return true;
}

//============================================================================
void HostClientMgr::onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    VxGUID sessionId;
    sessionId.initializeWithNewVxGUID();

    HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, sessionId );
    BaseSessionInfo sessionInfo( hostUserSessionId );
    onUserJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
}

//============================================================================
void HostClientMgr::onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    VxGUID sessionId;
    sessionId.initializeWithNewVxGUID();

    HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, sessionId );
    BaseSessionInfo sessionInfo( hostUserSessionId );
    onUserLeftHost( groupieId, sktBase, netIdent, sessionInfo );
}

//============================================================================
void HostClientMgr::onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    VxGUID sessionId;
    sessionId.initializeWithNewVxGUID();

    HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, sessionId );
    BaseSessionInfo sessionInfo( hostUserSessionId );
    onUserUnJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
}

//============================================================================
void HostClientMgr::onGroupRelayedUserAnnounce( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    LogMsg( LOG_VERBOSE, "HostClientMgr::onGroupRelayedUserAnnounce from %s id %s hosted by %s id %s groupieId %s",
            netIdent->getOnlineName(), netIdent->getMyOnlineId().describeVxGUID().c_str(), sktBase->getPeerOnlineName().c_str(),
        sktBase->getPeerOnlineId().describeVxGUID().c_str(), groupieId.describeGroupieId().c_str());

    m_Engine.getToGui().toGuiHostJoinStatus( groupieId.getHostType(), groupieId.getUserOnlineId(), eHostJoinSuccess );
    VxGUID sessionId;
    sessionId.initializeWithNewVxGUID();

    HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, sessionId );
    BaseSessionInfo sessionInfo( hostUserSessionId );
    onUserJoinedHost( groupieId, sktBase, netIdent, sessionInfo );
}

//============================================================================
void HostClientMgr::onPktHostUserInfoReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_ERROR, "HostClientMgr::onPktHostUserInfoReq" ); // should not recieve this.. should we do hack attempt?
}

//============================================================================
void HostClientMgr::onPktHostUserStatusReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserStatusReq" );

}

//============================================================================
void HostClientMgr::onPktHostUserStatusReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserStatusReply" );

}

//============================================================================
void HostClientMgr::onPktHostUserListReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserListReq" );

}

//============================================================================
void HostClientMgr::onPktHostUserListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserListReply" );
    PktHostUserListReply* pktReply = (PktHostUserListReply*)pktHdr;
    if( pktReply->isValidPkt() )
    {
        PktBlobEntry& blobEntry = pktReply->getBlobEntry();
        blobEntry.resetRead();
        bool writeResult{ true };
        for( int i = 0; i < pktReply->getHostUserCountThisPkt(); i++ )
        {
            VxGUID onlineId;
            if( !blobEntry.getValue( onlineId ) )
            {
                LogMsg( LOG_ERROR, "HostClientMgr::onPktHostUserListReply failed read online id" );
                break;
            }

            if( onlineId == m_Engine.getMyOnlineId() )
            {
                continue;
            }

            requestHostUserInfo( sktBase, netIdent, onlineId, pktReply->getSearchSessionId() );
        }
    }
}

//============================================================================
void HostClientMgr::onPktHostUserListMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserListMoreReq" );
}

//============================================================================
void HostClientMgr::onPktHostUserListMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserListMoreReply" );
}

//============================================================================
void HostClientMgr::requestHostUserInfo( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdentHost, VxGUID& onlineId, VxGUID& sessionId )
{
    if( onlineId == m_Engine.getMyOnlineId() )
    {
        return;
    }

    BigListInfo* bigListInfo = m_Engine.getBigListMgr().findBigListInfo( onlineId );
    if( bigListInfo )
    {
        announceUserInfo( sktBase, bigListInfo, sessionId );
        return;
    }

    m_ClientMutex.lock();
    m_UserInfoReqList.addGuid( onlineId );
    m_ClientMutex.unlock();

    // space out requests to avoid flooding the network
    if( shouldRequestUserInfo() )
    {
        sendNextUserInfoRequest( sktBase, netIdentHost, onlineId, sessionId );
    }
}

//============================================================================
void HostClientMgr::sendNextUserInfoRequest( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdentHost, VxGUID& onlineId, VxGUID& sessionId )
{
    PktHostUserInfoReq pktReq;
    GroupieId groupieId( onlineId, getHostId() );
    pktReq.setGroupieId( groupieId );
    pktReq.setSessionId( sessionId );

    m_ClientMutex.lock();
    m_SentUserInfoReqTime = GetGmtTimeMs();
    m_ClientMutex.unlock();

    if( !m_Plugin.txPacket( netIdentHost, sktBase, &pktReq ) )
    {
        clearUserInfoRequests();
        LogModule( eLogClients, LOG_VERBOSE, "HostClientMgr::requestHostUserInfo failed send" );
    }
}

//============================================================================
bool HostClientMgr::shouldRequestUserInfo( void )
{
    return !m_SentUserInfoReqTime;
}

//============================================================================
void HostClientMgr::clearUserInfoRequests( void )
{
    m_ClientMutex.lock();
    m_UserInfoReqList.clearList();
    m_SentUserInfoReqTime = 0;
    m_ClientMutex.unlock();
}

//============================================================================
void HostClientMgr::onPktHostUserInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "HostClientMgr::onPktHostUserInfoReply" );
    PktHostUserInfoReply* pktReply = (PktHostUserInfoReply*)pktHdr;
    if( pktReply->isValidPkt() )
    {
        m_ClientMutex.lock();
        m_SentUserInfoReqTime = 0;
        m_UserInfoReqList.removeGuid( pktReply->getGroupieId().getUserOnlineId() );
        bool userInfoReqListEmpty = m_UserInfoReqList.isEmpty();
        VxGUID nextOnlineId = m_UserInfoReqList.getAnyGuid();
        m_ClientMutex.unlock();

        PktBlobEntry& blobEntry = pktReply->getBlobEntry();
        blobEntry.resetRead();
        PktAnnounce pktAnn;
        bool readResult = pktAnn.extractFromBlob( blobEntry );
        if( readResult )
        {
            announceUserInfo( sktBase, &pktAnn, pktReply->getSessionId(), pktReply->getHostType() );
        }

        if( !userInfoReqListEmpty )
        {
            sendNextUserInfoRequest( sktBase, netIdent, nextOnlineId, pktReply->getSessionId() );
        }
    }
}

//============================================================================
void HostClientMgr::announceUserInfo( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, VxGUID& sessionId, EHostType hostType )
{
    if( m_Engine.getMyOnlineId() == pktAnn->getMyOnlineId() )
    {
        LogMsg( LOG_VERBOSE, "HostClientMgr::announceUserInfo got myself.. ERROR ?" );
        return;
    }

    if( m_Engine.getBigListMgr().isUserIgnored( pktAnn->getMyOnlineId() ) )
    {
        LogMsg( LOG_VERBOSE, "HostClientMgr::announceUserInfo ignored user %s %s", pktAnn->getOnlineName(), pktAnn->getMyOnlineId().toOnlineIdString().c_str() );
        return;
    }

    if( IsHostARelayForUsers( hostType ) )
    {
        // this is someone not in our database so start with guest friendships because is a member of the host we joined to
    
        BigListInfo* bigListInfo = 0;
        EPktAnnUpdateType updateType = m_Engine.getBigListMgr().updatePktAnn( pktAnn, &bigListInfo, hostType );		
        if( !bigListInfo || !bigListInfo->isValidNetIdent() )
        {
            LogMsg( LOG_ERROR, "HostClientMgr::announceUserInfo INVALID" );
            return;
        }

        if( !m_Engine.getUserOnlineMgr().isUserOnline( pktAnn->getMyOnlineId() ) )
        {
            // we need to exchange PktAnn to get current friendship from peer user
            // if possible connect directly
            if( pktAnn->canDirectConnectToUser() )
            {
                std::shared_ptr<VxSktBase> sktBase;
                VxGUID sessionId;
                sessionId.initializeWithNewVxGUID();

                m_Engine.getConnectionMgr().connectUsingTcp( pktAnn->getConnectInfo(), sktBase, sessionId );
            }
            else
            {
                // send our pkt ann through host to peer user and request response pkt ann
                m_Engine.getConnectionMgr().sendMyPktAnnounce( pktAnn->getMyOnlineId(), sktBase, true, false, false );
            }
        }

        pktAnn = bigListInfo;
    }

    GroupieId groupieId( pktAnn->getMyOnlineId(), getHostId() );
    HostUserSessionId hostUserSessionId( sktBase->getSocketId(), groupieId, sessionId );
    BaseSessionInfo sessionInfo( hostUserSessionId );
    sessionInfo.setOnlineState( eOnlineStateOnline );
    sessionInfo.setJoinState( eJoinStateJoinIsGranted );
    onUserJoinedHost( groupieId, sktBase, pktAnn, sessionInfo );
}

