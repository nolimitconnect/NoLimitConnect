//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "HostedListMgr.h"
#include "HostedListCallbackInterface.h"

#include <P2PEngine/P2PEngine.h>
#include <BigListLib/BigListInfo.h>
#include <HostServerJoinMgr/HostServerJoinMgr.h>
#include <UserJoinMgr/UserJoinMgr.h>

#include <Plugins/PluginMgr.h>
#include <Plugins/PluginBase.h>
#include <Plugins/PluginBaseHostService.h>

#include <CoreLib/VxPtopUrl.h>
#include <NetLib/VxSktBase.h>
#include <PktLib/PktsHostInvite.h>
#include <PktLib/PktsHostInfo.h>
#include <PktLib/PktsGroupie.h>

//============================================================================
HostedListMgr::HostedListMgr( P2PEngine& engine )
    : m_Engine( engine )
{
}

//============================================================================
RCODE HostedListMgr::hostedListMgrStartup( std::string& dbFileName )
{
    RCODE rc = m_HostedInfoListDb.hostedListDbStartup( HOSTED_LIST_DB_VERSION, dbFileName.c_str() );
    m_HostedInfoList.clear();
    m_HostedInfoListDb.getAllHosteds( m_HostedInfoList );
    return rc;
}

//============================================================================
RCODE HostedListMgr::hostedListMgrShutdown( void )
{
    return m_HostedInfoListDb.hostedListDbShutdown();
}

//============================================================================
void HostedListMgr::updateHosted( EHostType hostType, VxGUID& onlineId, std::string& hosted, int64_t timestampMs )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "HostedListMgr::updateDirectConnectIdent invalid id" );
        return;
    }

    /*
    bool wasUpdated = false;
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter )
    {
        if( iter->getHostType() == hostType && iter->getOnlineId() == onlineId )
        {
            iter->setHosted( hosted );
            if( timestampMs )
            {
                iter->setTimestamp( timestampMs );
                m_HostedInfoListDb.saveHosted( *iter );
            }
            
            wasUpdated = true;
            break;
        }
    }

    if( !wasUpdated )
    {
        HostedInfo hostedInfo( hostType, onlineId, hosted, timestampMs );
        m_HostedInfoList.push_back( hostedInfo );
        if( timestampMs )
        {
            m_HostedInfoListDb.saveHosted( hostedInfo );
        }
    }

    unlockList();
    */
}

//============================================================================
void HostedListMgr::updateHostedList( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
    if( !netIdent || !sktBase )
    {
        LogMsg( LOG_ERROR, "HostedListMgr::updateHosteds null netIdent or sktBase" );
        return;
    }

    if( netIdent->requiresRelay() )
    {
        removeClosedPortIdent( netIdent->getMyOnlineId() );
    }
    else
    {     
        VxGUID onlineId = netIdent->getMyOnlineId();
        if( !onlineId.isVxGUIDValid() )
        {
            LogMsg( LOG_ERROR, "HostedListMgr::updateHostedList invalid id" );
            return;
        }

        std::string nodeUrlIpv4 = netIdent->getMyOnlineUrl( false );
        for( int i = eHostTypeUnknown + 1; i < eMaxHostType; ++i )
        {
            EHostType hostType = ( EHostType )i;
            if( netIdent->canRequestJoin( hostType ) )
            {
                updateAndRequestInfoIfNeeded( false, hostType, onlineId, nodeUrlIpv4, netIdent, sktBase );
            }
        }
    }
}

//============================================================================
void HostedListMgr::removeClosedPortIdent( VxGUID& onlineId )
{
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); )
    {
        if( iter->getAdminOnlineId() == onlineId )
        {
            iter = m_HostedInfoList.erase( iter );
        }
        else
        {
            ++iter;
        }
    }

    unlockList();
    m_HostedInfoListDb.removeClosedPortIdent( onlineId );
}

//============================================================================
void HostedListMgr::removeHostedInfo( EHostType hostType, VxGUID& onlineId )
{
    bool wasRemoved{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); )
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getHostType() == hostType )
        {
            m_HostedInfoList.erase( iter );
            wasRemoved = true;
            break;
        }
        else
        {
            ++iter;
        }
    }

    unlockList();
    if( wasRemoved )
    {
        m_HostedInfoListDb.removeHostedInfo( hostType, onlineId );
        announceHostInfoRemoved( hostType, onlineId );
    }
}

//============================================================================
void HostedListMgr::clearHostedInfoList( void )
{
    m_HostedInfoList.clear();
}

//============================================================================
void HostedListMgr::wantHostedListCallback( HostedListCallbackInterface* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "HostServerJoinMgr::wantHostedListCallback invalid param" );
        return;
    }

    lockClientList();
    for( auto iter = m_HostedInfoListClients.begin(); iter != m_HostedInfoListClients.end(); ++iter )
    {
        if( *iter == client )
        {
            iter = m_HostedInfoListClients.erase( iter );
            break;
        }
    }

    if( enable )
    {
        m_HostedInfoListClients.push_back( client );
    }

    unlockClientList();
}

//============================================================================
void HostedListMgr::announceHostInfoUpdated( HostedInfo* hostedInfo )
{
    if( hostedInfo )
    {
        lockClientList();
        for( auto client : m_HostedInfoListClients )
        {
            client->callbackHostedInfoListUpdated( hostedInfo );
        }

        unlockClientList();
    }
    else
    {
        LogMsg( LOG_ERROR, "HostServerJoinMgr::announceHostInfoUpdated invalid param" );
    }
}

//============================================================================
void HostedListMgr::announceHostInfoRemoved( EHostType hostType, VxGUID& hostOnlineId )
{
    // removeFromDatabase( hostOnlineId, hostType, false );
    lockClientList();
    for( auto client : m_HostedInfoListClients )
    {
        client->callbackHostedInfoListRemoved( hostOnlineId, hostType );
    }

    unlockClientList();
}

//============================================================================
void HostedListMgr::announceHostInfoSearchResult( HostedInfo* hostedInfo, VxGUID& sessionId )
{
    if( hostedInfo )
    {
        lockClientList();
        for( auto client : m_HostedInfoListClients )
        {
            client->callbackHostedInfoListSearchResult( hostedInfo, sessionId );
        }

        unlockClientList();
    }
    else
    {
        LogMsg( LOG_ERROR, "HostServerJoinMgr::announceHostInfoSearchResult invalid param" );
    }
}


//============================================================================
void HostedListMgr::announceHostInfoSearchComplete( EHostType hostType, VxGUID& sessionId )
{
    if( hostType != eHostTypeUnknown )
    {
        lockClientList();
        for( auto client : m_HostedInfoListClients )
        {
            client->callbackHostedInfoListSearchComplete( hostType, sessionId );
        }

        unlockClientList();
    }
    else
    {
        LogMsg( LOG_ERROR, "HostServerJoinMgr::announceHostInfoSearchResult invalid param" );
    }
}

//============================================================================
void HostedListMgr::updateAndRequestInfoIfNeeded( bool ipv6, EHostType hostType, VxGUID& onlineId, std::string& nodeUrl, VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
    bool requiresSendHostInfoRequest{ false };
    bool requiresAnnounceUpdate{ false };
    bool wasFound{ false };
    bool urlChanged{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter)
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getHostType() == hostType )
        {
            wasFound = true;
            iter->setConnectedTimestamp( sktBase->getLastActiveTimeMs() );
            if( iter->getHostInfoTimestamp() < netIdent->getHostOrThumbModifiedTime( hostType ) )
            {
                requiresSendHostInfoRequest = true;
            }

            if( nodeUrl != iter->getHostInviteUrl( ipv6 ) )
            {
                urlChanged = true;
                iter->setHostInviteUrl( ipv6, nodeUrl );
                if( !requiresSendHostInfoRequest && iter->isValidForGui() )
                {
                    requiresAnnounceUpdate = true;
                }
            }       

            if( iter->shouldSaveToDb() )
            {
                if( urlChanged )
                {
                    m_HostedInfoListDb.saveHosted( *iter );
                }
                else
                {
                    m_HostedInfoListDb.updateLastConnected( hostType, onlineId, iter->getConnectedTimestamp() );
                }
            }

            if( requiresAnnounceUpdate )
            {
                announceHostInfoUpdated( &( *iter ) );
            }
        }
    }

    if( !wasFound )
    {
        requiresSendHostInfoRequest = true;
        VxGUID thumbId = netIdent->getHostThumbId( hostType, true );

        std::string otherNodeUrl = netIdent->getMyOnlineUrl( !ipv6 );
        
        HostedInfo hostedInfo( hostType, onlineId, ipv6 ? otherNodeUrl : nodeUrl, ipv6 ? nodeUrl : otherNodeUrl, thumbId );

        hostedInfo.setConnectedTimestamp( sktBase->getLastActiveTimeMs() );
        m_HostedInfoList.push_back( hostedInfo );
    }

    unlockList();

    if( requiresSendHostInfoRequest )
    {
        requestHostedInfo( hostType, onlineId, netIdent, sktBase );
    }
}

//============================================================================
bool HostedListMgr::updateLastConnected( EHostType hostType, VxGUID& onlineId, int64_t lastConnectedTime )
{
    bool result{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter)
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getHostType() == hostType )
        {
            iter->setConnectedTimestamp( lastConnectedTime );
            result = true;

            if( iter->shouldSaveToDb() )
            {
                m_HostedInfoListDb.updateLastConnected( hostType, onlineId, iter->getConnectedTimestamp() );
            }
        }
    }

    unlockList();
    return result;
}

//============================================================================
bool HostedListMgr::updateLastJoined( EHostType hostType, VxGUID& onlineId, int64_t lastJoinedTime )
{
    bool result{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter)
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getHostType() == hostType )
        {
            int64_t oldJoinedTime = iter->getJoinedTimestamp();
            iter->setJoinedTimestamp( lastJoinedTime );
            result = true;

            if( iter->shouldSaveToDb() )
            {
                m_HostedInfoListDb.updateLastConnected( hostType, onlineId, lastJoinedTime );
            }
            else if( oldJoinedTime )
            {
                m_HostedInfoListDb.removeHostedInfo( hostType, onlineId );
            }

            if( iter->isValidForGui() )
            {
                announceHostInfoUpdated( &( *iter ) );
            }
        }
    }

    unlockList();
    return result;
}

//============================================================================
bool HostedListMgr::updateIsFavorite( EHostType hostType, VxGUID& onlineId, bool isFavorite )
{
    bool result{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter)
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getHostType() == hostType )
        {
            bool wasFavorite = iter->getIsFavorite();
            iter->setIsFavorite( isFavorite );
            result = true;

            if( iter->shouldSaveToDb() )
            {
                m_HostedInfoListDb.updateIsFavorite( hostType, onlineId, isFavorite );
            }
            else if( wasFavorite )
            {
                m_HostedInfoListDb.removeHostedInfo( hostType, onlineId );
            }

            if( iter->isValidForGui() )
            {
                announceHostInfoUpdated( &( *iter ) );
            }
        }
    }

    unlockList();
    return result;
}

//============================================================================
bool HostedListMgr::getIsFavorite( VxGUID& onlineId )
{
    bool result{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter)
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getIsFavorite() )
        {
            result = true;
            break;
        }
    }

    unlockList();
    return result;
}

//============================================================================
bool HostedListMgr::updateHostTitleAndDescription( EHostType hostType, VxGUID& onlineId, std::string& title, std::string& description, int64_t lastDescUpdateTime, VxNetIdent* netIdent )
{
    bool result{ false };
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter)
    {
        if( iter->getAdminOnlineId() == onlineId && iter->getHostType() == hostType )
        {
            result = true;
            iter->setHostTitle( title );
            iter->setHostDescription( description );
            iter->setHostInfoTimestamp( lastDescUpdateTime );
            VxGUID thumbId = netIdent->getHostThumbId( hostType, true );
            iter->setThumbId( thumbId );

            if( iter->shouldSaveToDb() )
            {
                m_HostedInfoListDb.updateHostTitleAndDescription( hostType, onlineId, title, description, lastDescUpdateTime, thumbId );
            }

            if( iter->isValidForGui() )
            {
                announceHostInfoUpdated( &( *iter ) );
            }
        }
    }

    unlockList();
    return result;
}

//============================================================================
bool HostedListMgr::requestHostedInfo( EHostType hostType, VxGUID& onlineId, VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
    bool result{ false };
    // only hosts that announce to network respond to Host Info requests
    if( HostShouldAnnounceToNetwork( hostType ) )
    {
        PktHostInfoReq pktReq;
        pktReq.setPluginNum( ( uint8_t )HostTypeToHostPlugin( hostType ) );
        pktReq.getSessionId().initializeWithNewVxGUID();
        pktReq.setSrcOnlineId( m_Engine.getMyOnlineId() );
        pktReq.setDestOnlineId( netIdent->getMyOnlineId() );

        result = sktBase->txPacket( netIdent->getMyOnlineId(), &pktReq);
    }

    return result;
}

//============================================================================
void HostedListMgr::onPktHostInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, PluginBase* plugin )
{
    bool result{ false };
    PktHostInfoReply* pktReply = (PktHostInfoReply *)pktHdr;

    EHostType hostType = pktReply->getHostType();
    std::string hostTitle;
    std::string hostDesc;
    int64_t lastModifiedTime{ 0 };
    ECommErr commErr = pktReply->getCommError();
    if( eCommErrNone == commErr )
    {
        if( pktReply->getHostTitleAndDescription( hostTitle, hostDesc, lastModifiedTime ) )
        {
            if( !hostTitle.empty() && !hostDesc.empty() && lastModifiedTime )
            {
                result = true;
            }
            else
            {
                LogMsg( LOG_ERROR, "HostedListMgr::onPktHostInfoReply INVALID host info %s", netIdent->getOnlineName() );
            }
        }
        else
        {
            LogMsg( LOG_ERROR, "HostedListMgr::onPktHostInfoReply extract host info FAILED %s", netIdent->getOnlineName() );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "HostedListMgr::onPktHostInfoReply error %s %s", DescribeCommError( commErr ), netIdent->getOnlineName() );
    }


    if( result )
    {
        LogMsg( LOG_VERBOSE, "HostedListMgr::onPktHostInfoReply success title %s desc %s", hostTitle.c_str(), hostDesc.c_str() );
        updateHostTitleAndDescription( hostType, netIdent->getMyOnlineId(), hostTitle, hostDesc, lastModifiedTime, netIdent );
    }
}

//============================================================================
bool HostedListMgr::fromGuiQueryMyHostedInfo( EHostType hostType, std::vector<HostedInfo>& hostedInfoList )
{
    hostedInfoList.clear();
    PluginBase* pluginBase = m_Engine.getPluginMgr().getPlugin( HostTypeToHostPlugin( hostType ) );
    if( pluginBase && pluginBase->isPluginEnabled() )
    {
        HostedInfo hostedInfo;
        if( pluginBase->getHostedInfo( hostedInfo ) )
        {
            hostedInfoList.push_back( hostedInfo );
        }
    }

    return !hostedInfoList.empty();
}

//============================================================================
bool HostedListMgr::fromGuiQueryHostedInfoList( EHostType hostType, std::vector<HostedInfo>& hostedInfoList, VxGUID& hostIdIfNullThenAll )
{
    hostedInfoList.clear();
    lockList();
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter )
    {
        if( iter->getHostType() == hostType )
        {
            addToListInJoinedTimestampOrder( hostedInfoList, *iter );
            hostedInfoList.push_back( *iter );
        }
    }

    unlockList();

    return !hostedInfoList.empty();
}

//============================================================================
bool HostedListMgr::fromGuiQueryHostListFromNetworkHost( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll )
{
    if( netHostUrl.isValid() )
    {
        m_SearchHostType = hostType;
        m_SearchSpecificOnlineId = hostIdIfNullThenAll;
        m_SearchSessionId.initializeWithNewVxGUID();

        // add ourself to host list if we have enabled hosting of the given host type
        EPluginType pluginType = HostTypeToHostPlugin( hostType );
        if( m_Engine.getMyPktAnnounce().isPluginEnabled( pluginType ) )
        {
            PluginBaseHostService* plugin = dynamic_cast< PluginBaseHostService*>( m_Engine.getPluginMgr().findPlugin( pluginType ));
            HostedInfo myHostedInfo;
            if( plugin && plugin->getHostedInfo( myHostedInfo ) )
            {
                // so we can work with join our own host always start out as needing request join
                GroupieId groupieId( m_Engine.getMyOnlineId(), m_Engine.getMyOnlineId(), hostType );
                m_Engine.getHostJoinMgr().changeJoinState( groupieId, eJoinStateNone );
                m_Engine.getUserJoinMgr().changeJoinState( groupieId, eJoinStateNone );
                hostSearchResult( hostType, m_SearchSessionId, m_Engine.getSktLoopback(), m_Engine.getMyPktAnnounce().getVxNetIdent(), myHostedInfo );
            }
        }

        m_Engine.getFromGuiMgr().fromGuiQueryHostListFromNetworkHost( netHostUrl, hostType, hostIdIfNullThenAll, m_SearchSessionId );

        return true;
    }

    return false;
}

//============================================================================
bool HostedListMgr::fromGuiQueryGroupiesFromHosted( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& onlineIdIfNullThenAll )
{
    if( netHostUrl.isValid() )
    {
        m_SearchHostType = hostType;
        m_SearchSpecificOnlineId = onlineIdIfNullThenAll;
        m_SearchSessionId.initializeWithNewVxGUID();
        std::shared_ptr<VxSktBase> sktBase( nullptr );
        EConnectReason connectReason{ eConnectReasonUnknown };
        switch( hostType )
        {
        case eHostTypeGroup:
            connectReason = eConnectReasonGroupHostedUserListSearch;
            break;
        case eHostTypeChatRoom:
            connectReason = eConnectReasonChatRoomHostedUserListSearch;
            break;
        case eHostTypeRandomConnect:
            connectReason = eConnectReasonRandomConnectHostedUserListSearch;
            break;
        default:
            break;
        }

        if( connectReason != eConnectReasonUnknown )
        {
            m_Engine.getConnectionMgr().requestConnection( m_SearchSessionId, netHostUrl.getUrl(), netHostUrl.getOnlineId(), this, sktBase, eConnectReasonGroupHostedUserListSearch );
            return true;
        }
        else
        {
            LogMsg( LOG_ERROR, "HostedListMgr::fromGuiQueryGroupiesFromHosted invalid host type" );
        }
    }

    return false;
}

//============================================================================
void HostedListMgr::connectToHostAttempt( HostedId adminId, std::string& ptopUrlAttempted, bool ipv6 )
{
    bool found = false;
    bool updated = false;
    HostedInfo updatedHostedInfo;
    lockList();
    for( auto hostInfo : m_HostedInfoList )
    {
        if( hostInfo.getAdminId() == adminId )
        {
            found = true;
            if( hostInfo.getHostInviteUrl( ipv6 ).empty() )
            {
                hostInfo.setHostInviteUrl( ipv6, ptopUrlAttempted );
                updatedHostedInfo = hostInfo;
            }

            break;
        }
    }

    if( !found )
    {
        updatedHostedInfo = HostedInfo( adminId, ptopUrlAttempted, ipv6 );
        m_HostedInfoList.emplace_back( updatedHostedInfo );
    }

    unlockList();

    if( !found || updated )
    {
        announceHostInfoUpdated( &updatedHostedInfo );
    }
}

//============================================================================
void HostedListMgr::connectToHostSuccess( HostedId adminId )
{
    bool found{ false };
    HostedInfo updatedHostedInfo;
    lockList();
    for( auto hostInfo : m_HostedInfoList )
    {
        if( hostInfo.getAdminId() == adminId )
        {
            found = true;
            hostInfo.setConnectedTimestamp( GetGmtTimeMs() );
            updatedHostedInfo = hostInfo;
            break;
        }
    }

    unlockList();
    if( found )
    {
        announceHostInfoUpdated( &updatedHostedInfo );
    }
    else
    {
        LogMsg( LOG_ERROR, "HostedListMgr::connectToHostSuccess host not found %s", m_Engine.describeHostedId( adminId ).c_str() );
        vx_assert( false );
    }
}

//============================================================================
void HostedListMgr::joinedToHostSuccess( HostedId adminId )
{
    bool found{ false };
    HostedInfo updatedHostedInfo;
    lockList();
    for( auto hostInfo : m_HostedInfoList )
    {
        if( hostInfo.getAdminId() == adminId )
        {
            found = true;
            hostInfo.setJoinedTimestamp( GetGmtTimeMs() );
            updatedHostedInfo = hostInfo;
            break;
        }
    }

    unlockList();
    if( found )
    {
        announceHostInfoUpdated( &updatedHostedInfo );
    }
    else
    {
        LogMsg( LOG_ERROR, "HostedListMgr::joinedToHostSuccess host not found %s", m_Engine.describeHostedId( adminId ).c_str() );
        vx_assert( false );
    }
}

//============================================================================
bool HostedListMgr::onContactConnected( VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, EConnectReason connectReason )
{
    if( eConnectReasonNetworkHostListSearch == connectReason )
    {
        LogModule( eLogHostSearch, LOG_DEBUG, "HostedListMgr::onContactConnected sending PktHostInviteSearchReq" );

        PktHostInviteSearchReq pktReq;
        pktReq.setPluginNum( ( uint8_t )ePluginTypeHostNetwork );
        pktReq.setSrcOnlineId( m_Engine.getMyOnlineId() );
        pktReq.setSearchSessionId( sessionId );
        pktReq.setHostType( m_SearchHostType );
        pktReq.setSpecificOnlineId( m_SearchSpecificOnlineId );
        return 0 == sktBase->txPacket( onlineId, &pktReq );
    }
    else if( eConnectReasonGroupHostedUserListSearch == connectReason || eConnectReasonChatRoomHostedUserListSearch == connectReason || eConnectReasonRandomConnectHostedUserListSearch == connectReason )
    {
        EHostType hostType{ eHostTypeUnknown };
        EPluginType pluginType{ ePluginTypeInvalid };

        switch( connectReason )
        {
        case eConnectReasonGroupHostedUserListSearch:
            hostType = eHostTypeGroup;
            pluginType = ePluginTypeHostGroup;
            break;
        case eConnectReasonChatRoomHostedUserListSearch:
            hostType = eHostTypeChatRoom;
            pluginType = ePluginTypeHostChatRoom;
            break;
        case eConnectReasonRandomConnectHostedUserListSearch:
            hostType = eHostTypeRandomConnect;
            pluginType = ePluginTypeHostRandomConnect;
            break;
        default:
            break;
        }

        if( hostType != eHostTypeUnknown && pluginType != ePluginTypeInvalid )
        {
            PktGroupieSearchReq pktReq;
            LogModule( eLogHostSearch, LOG_DEBUG, "HostedListMgr::onContactConnected sending PktGroupieSearchReq" );

            pktReq.setPluginNum( ( uint8_t )pluginType );
            pktReq.setSrcOnlineId( m_Engine.getMyOnlineId() );
            pktReq.setSearchSessionId( sessionId );
            pktReq.setHostType( hostType );
            pktReq.setSpecificOnlineId( m_SearchSpecificOnlineId );
            return 0 == sktBase->txPacket( onlineId, &pktReq );
        }
        else
        {
            LogMsg( LOG_ERROR, "HostedListMgr::onContactConnected invalid param" );
        }
    }

    return false;
}

//============================================================================
void HostedListMgr::addToListInJoinedTimestampOrder( std::vector<HostedInfo>& hostedInfoList, HostedInfo& hostedInfo )
{
    bool wasInserted{ false };
    int64_t joinedTimestamp = hostedInfo.getJoinedTimestamp();
    int64_t connectedTimestamp = hostedInfo.getConnectedTimestamp();
    if( !hostedInfoList.empty() && (joinedTimestamp || connectedTimestamp) )
    {
        for( auto iter = hostedInfoList.begin(); iter != hostedInfoList.end(); ++iter )
        {
            if( joinedTimestamp )
            {
                if( joinedTimestamp > iter->getJoinedTimestamp() )
                {
                    hostedInfoList.insert( iter, hostedInfo );
                    wasInserted = true;
                }
            }
            else if( connectedTimestamp )
            {
                // skip those with joined timestamp and put those with connected timestamp in connected time order
                if( iter->getJoinedTimestamp() )
                {
                    continue;
                }
                else if( connectedTimestamp > iter->getConnectedTimestamp() )
                {
                    hostedInfoList.insert( iter, hostedInfo );
                    wasInserted = true;
                }
            }

            if( wasInserted )
            {
                break;
            }
        }
    }

    if( !wasInserted )
    {
        hostedInfoList.push_back( hostedInfo );
    }
}

//============================================================================
void HostedListMgr::hostSearchResult( EHostType hostType, VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, HostedInfo& hostedInfo )
{
    HostedInfo resultInfo;
    if( updateHostInfo( hostType, hostedInfo, netIdent, sktBase, true, &resultInfo ) )
    {
        announceHostInfoSearchResult( &resultInfo, searchSessionId );
    }
}

//============================================================================
void HostedListMgr::hostSearchCompleted( EHostType hostType, VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, ECommErr commErr )
{
    if( commErr )
    {
        LogMsg( LOG_ERROR, "HostedListMgr::hostSearchCompleted with error %s from %s", DescribeCommError( commErr ), sktBase->describeSktConnection().c_str() );
    }
    else
    {
        LogMsg( LOG_VERBOSE, "HostedListMgr::hostSearchCompleted with no errors" );
    }

    m_Engine.getConnectionMgr().doneWithConnection( searchSessionId, netIdent->getMyOnlineId(), this, eConnectReasonNetworkHostListSearch );
    announceHostInfoSearchComplete( hostType, searchSessionId );
}

//============================================================================
void HostedListMgr::onHostInviteAnnounceAdded( EHostType hostType, HostedInfo& hostedInfo, VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
    LogMsg( LOG_VERBOSE, "HostedListMgr::onHostInviteAnnounceAdded %s from %s ", DescribeHostType( hostType), netIdent->getOnlineName() );
    updateHostInfo( hostType, hostedInfo, netIdent, sktBase );
}

//============================================================================
void HostedListMgr::onHostInviteAnnounceUpdated( EHostType hostType, HostedInfo& hostedInfo, VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase, bool infoChanged )
{
    LogMsg( LOG_VERBOSE, "HostedListMgr::onHostInviteAnnounceUpdated %s from %s ", DescribeHostType( hostType ), netIdent->getOnlineName() );
    updateHostInfo( hostType, hostedInfo, netIdent, sktBase, infoChanged );
}

//============================================================================
// returns true if retHostedInfo was filled
bool HostedListMgr::updateHostInfo( EHostType hostType, HostedInfo& hostedInfo, VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase, bool infoChanged, HostedInfo* retResultInfo )
{
    VxPtopUrl ptopUrlIpv4( hostedInfo.getHostInviteUrl( false ) );
    VxPtopUrl ptopUrlIpv6( hostedInfo.getHostInviteUrl( true ) );
    bool ipv6{ false };
    VxGUID onlineId;

    if( ptopUrlIpv4.isValid() )
    {
        onlineId = ptopUrlIpv4.getOnlineId();
    }
    else if( ptopUrlIpv6.isValid() )
    {
        onlineId = ptopUrlIpv6.getOnlineId();
        ipv6 = true;
    }
    
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "HostedListMgr::hostSearchResult INVALID url" );
        return false;
    }

    bool filledResultInfo = false;
    bool needsIdentityReq = false;
    if( !netIdent )
    {
        VxNetIdent hostIdent;
        BigListInfo* bigListInfo = m_Engine.getBigListMgr().findBigListInfo( onlineId );
        if( bigListInfo )
        {
            netIdent = bigListInfo->getVxNetIdent();
        }
        else
        {
            needsIdentityReq = true;
            m_Engine.getHostUrlListMgr().requestIdentity( hostedInfo.getHostInviteUrl( ipv6 ) );
        }
    }

    LogMsg( LOG_VERBOSE, "HostedListMgr::hostSearchResult title %s desc %s time %lld host url %s", 
        hostedInfo.getHostTitle().c_str(), hostedInfo.getHostDescription().c_str(), hostedInfo.getHostInfoTimestamp(), hostedInfo.getHostInviteUrl( ipv6 ).c_str() );

    bool alreadyExisted{ false };
    bool hostedInfoUpdated{ false };
    HostedInfo updatedHostedInfo;

    lockList();
    // if exists see if needs update
    for( auto iter = m_HostedInfoList.begin(); iter != m_HostedInfoList.end(); ++iter )
    {
        if( iter->getAdminId() == hostedInfo.getAdminId() )
        {
            alreadyExisted = true;
            if( sktBase )
            {
                iter->setConnectedTimestamp( sktBase->getLastActiveTimeMs() );
            }

            if( iter->getHostInviteUrl( ipv6 ) != hostedInfo.getHostInviteUrl( ipv6 ) || infoChanged )
            {
                // url has changed. just update
                iter->setHostInviteUrl( ipv6, hostedInfo.getHostInviteUrl( ipv6 ) );
                // update our url list also
                m_Engine.getHostUrlListMgr().updateHostUrl( hostType, hostedInfo.getAdminOnlineId(), hostedInfo.getHostInviteUrl( false ), hostedInfo.getHostInviteUrl( true ) );
                if( iter->shouldSaveToDb() )
                {
                    m_HostedInfoListDb.updateHostUrl( ipv6, iter->getHostType(), iter->getAdminOnlineId(), hostedInfo.getHostInviteUrl( ipv6 )  );
                }
                // TODO do we need to update if just url changed ?
            }

            if( hostedInfo.getHostInfoTimestamp() > iter->getHostInfoTimestamp() || infoChanged )
            {
                iter->setHostInfoTimestamp( hostedInfo.getHostInfoTimestamp() );
                iter->setHostTitle( hostedInfo.getHostTitle() );
                iter->setHostDescription( hostedInfo.getHostDescription() );
                updatedHostedInfo = *iter;
                hostedInfoUpdated = true;
                if( iter->shouldSaveToDb() )
                {
                    m_HostedInfoListDb.saveHosted( *iter );
                }
            }
            else
            {
                // in theory they should be same if we are up to date.. check anyway
                if( iter->getHostTitle() != hostedInfo.getHostTitle() || iter->getHostDescription() != hostedInfo.getHostDescription() || infoChanged )
                {
                    LogMsg( LOG_VERBOSE, "HostedListMgr::hostSearchResult title or description is different" );
                    iter->setHostTitle( hostedInfo.getHostTitle() );
                    iter->setHostDescription( hostedInfo.getHostDescription() );
                    updatedHostedInfo = *iter;
                    hostedInfoUpdated = true;
                    if( iter->shouldSaveToDb() )
                    {
                        m_HostedInfoListDb.updateHostTitleAndDescription( iter->getHostType(), iter->getAdminOnlineId(), hostedInfo.getHostTitle(), hostedInfo.getHostDescription(), iter->getHostInfoTimestamp(), iter->getThumbId() );
                    }
                }
            }

            if( !needsIdentityReq && netIdent && !netIdent->canRequestJoin( hostType ) )
            {
                // clear hostedInfoUpdated.. if cannot possibly join dont announce it
                hostedInfoUpdated = false;
            }

            if( retResultInfo )
            {
                *retResultInfo = *iter;
                filledResultInfo = true;
            }

            break;
        }
    }

    if( !alreadyExisted )
    {
        if( sktBase )
        {
            hostedInfo.setConnectedTimestamp( sktBase->getLastActiveTimeMs() );
        }

        if( retResultInfo )
        {
            *retResultInfo = hostedInfo;
            filledResultInfo = true;
        }

        m_HostedInfoList.push_back( hostedInfo );
        updatedHostedInfo = hostedInfo;
        hostedInfoUpdated = true;
    }

    unlockList();

    if( hostedInfoUpdated )
    {
        announceHostInfoUpdated( &updatedHostedInfo );
        return true;
    }

    return filledResultInfo;
}
