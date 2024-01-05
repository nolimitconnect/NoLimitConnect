//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ConnectionMgr.h"
#include "ConnectedInfo.h"
#include "HandshakeInfo.h"

#include <BigListLib/BigListMgr.h>
#include <BigListLib/BigListInfo.h>
#include <Network/NetworkMgr.h>
#include <Network/NetworkStateMachine.h>
#include <P2PEngine/P2PEngine.h>

#include <UrlMgr/UrlMgr.h>

#include <NetLib/VxSktConnect.h>
#include <NetLib/VxPeerMgr.h>
#include <NetLib/VxSktCrypto.h>
#include <NetLib/VxSktUtil.h>

#include <PktLib/PktsRelay.h>

#include <CoreLib/VxTime.h>
#include <CoreLib/VxUrl.h>

//============================================================================
ConnectionMgr::ConnectionMgr( P2PEngine& engine )
    : m_Engine( engine )
    , m_BigListMgr( engine.getBigListMgr() )
    , m_AllList( engine )
    , m_PeerMgr( engine.getPeerMgr() )
{
}

//============================================================================
std::string ConnectionMgr::getDefaultHostUrl( EHostType hostType )
{
    std::string hostUrl("");
    lockConnectionList();
    auto iter = m_DefaultHostUrlList.find( hostType );
    if( iter != m_DefaultHostUrlList.end() )
    {
        hostUrl = iter->second;
    }

    unlockConnectionList();
    return hostUrl;
}

//============================================================================
void ConnectionMgr::setDefaultHostOnlineId( EHostType hostType, VxGUID& hostOnlineId )
{
    lockConnectionList();
    m_DefaultHostIdList[hostType] = hostOnlineId;
    unlockConnectionList();
}

//============================================================================
bool ConnectionMgr::getDefaultHostOnlineId( EHostType hostType, VxGUID& retHostOnlineId )
{
    bool result = false;
    retHostOnlineId.clearVxGUID();

    lockConnectionList();
    auto iter = m_DefaultHostIdList.find( hostType );
    if( iter != m_DefaultHostIdList.end() )
    {
        retHostOnlineId = iter->second;
        result = true;
    }

    unlockConnectionList();
    return result;
}

//============================================================================
EHostAnnounceStatus ConnectionMgr::lookupOrQueryAnnounceId( EHostType hostType, VxGUID& sessionId, std::string hostUrlIpv4, 
    VxGUID& hostGuid, IConnectRequestCallback* callback, EConnectReason connectReason )
{
    EHostAnnounceStatus hostStatus = eHostAnnounceUnknown;
    if( urlCacheOnlineIdLookup( hostUrlIpv4, hostGuid ) )
    {
        hostStatus = eHostAnnounceQueryIdSuccess;
        if( hostType != eHostTypeUnknown )
        {
            std::string emptyUrl;
            m_Engine.getHostUrlListMgr().updateHostUrl( hostType, hostGuid, hostUrlIpv4, emptyUrl );
        }
    }
    else if( getQueryIdFailedCount( hostType ) > 2 )
    {
        // dont keep hammering server if is sending an error
        hostStatus = eHostAnnounceQueryIdFailed;
    }
    else
    {
        hostStatus = eHostAnnounceQueryIdInProgress;
        std::string myUrl = m_Engine.getMyOnlineUrl( false );
        m_Engine.getRunUrlAction().runUrlAction( sessionId, eNetCmdQueryHostOnlineIdReq, hostUrlIpv4.c_str(), myUrl.c_str(), this, callback, hostType, connectReason );
    }

    return hostStatus;
}

//============================================================================
EHostJoinStatus ConnectionMgr::lookupOrQueryJoinId( VxGUID& sessionId, std::string hostUrl, VxGUID& hostGuid, IConnectRequestCallback* callback, EConnectReason connectReason )
{
    EHostJoinStatus joinStatus = eHostJoinUnknown;
    if( urlCacheOnlineIdLookup( hostUrl, hostGuid ) )
    {
        joinStatus = eHostJoinQueryIdSuccess;
    }
    else
    {
        joinStatus = eHostJoinQueryIdInProgress;
        std::string myUrl = m_Engine.getMyOnlineUrl( false );
        m_Engine.getRunUrlAction().runUrlAction( sessionId, eNetCmdQueryHostOnlineIdReq, 
            m_Engine.getUrlMgr().resolveUrl(false, hostUrl).c_str(), myUrl.c_str(), this, callback, eHostTypeUnknown, connectReason );
    }

    return joinStatus;
}

//============================================================================
EHostSearchStatus ConnectionMgr::lookupOrQuerySearchId( VxGUID& sessionId, std::string hostUrl, VxGUID& hostGuid, IConnectRequestCallback* callback, EConnectReason connectReason )
{
    EHostSearchStatus joinStatus = eHostSearchUnknown;
    if( urlCacheOnlineIdLookup( hostUrl, hostGuid ) )
    {
        joinStatus = eHostSearchQueryIdSuccess;
    }
    else
    {
        joinStatus = eHostSearchQueryIdInProgress;
        std::string myUrl = m_Engine.getMyOnlineUrl(false);
        m_Engine.getRunUrlAction().runUrlAction( sessionId, eNetCmdQueryHostOnlineIdReq, hostUrl.c_str(), myUrl.c_str(), this, callback, eHostTypeUnknown, connectReason );
    }

    return joinStatus;
}


//============================================================================
void ConnectionMgr::onSktConnectedWithPktAnn( std::shared_ptr<VxSktBase>& sktBase, BigListInfo * bigListInfo )
{
    std::vector<HandshakeInfo> shakeList;
    std::vector<HandshakeInfo> timedOutList;
    m_HandshakeMutex.lock();
    m_HandshakeList.getAndRemoveHandshakeInfo( sktBase->getSocketId(), bigListInfo->getMyOnlineId(), shakeList, timedOutList );
    m_HandshakeMutex.unlock();

    if( !timedOutList.empty() )
    {
        for( HandshakeInfo& shakeInfo : timedOutList )
        {
            shakeInfo.onHandshakeTimeout();
        }
    }

    if( !shakeList.empty() )
    {
        lockConnectionList();
        ConnectedInfo* connectInfo = m_AllList.getOrAddConnectedInfo( sktBase->getSocketId(), bigListInfo );
        if( nullptr == connectInfo )
        {
            LogMsg( LOG_ERROR, "ConnectionMgr get connection info FAILED" );
        }
        else
        {
            for( HandshakeInfo& shakeInfo : shakeList )
            {
                connectInfo->addConnectReason( shakeInfo );
            }
        }

        // if we do the onContactConnected() while locked then may deadlock if doneWithConnection is called in onContactConnected
        unlockConnectionList();

        if( sktBase->isConnected() )
        {
            for( auto& shakeInfo : shakeList )
            {
                shakeInfo.onContactConnected();
            }
        }
        else
        {
            // lost connection already
            VxGUID socketId = sktBase->getSocketId();
            for( auto iter = shakeList.begin(); iter != shakeList.end(); )
            {
                if( iter->getSocketId() == socketId )
                {
                    shakeList.erase( iter );
                }
                else
                {
                    ++iter;
                }
            }
        }
    }
}

//============================================================================
void ConnectionMgr::onSktDisconnected( std::shared_ptr<VxSktBase>& sktBase )
{
    m_HandshakeMutex.lock();
    m_HandshakeList.onSktDisconnected( sktBase->getSocketId() );
    m_HandshakeMutex.unlock();

    lockConnectionList();
    m_AllList.onSktDisconnected( sktBase->getSocketId() );
    unlockConnectionList();
}

//============================================================================
void ConnectionMgr::callbackInternetStatusChanged( EInternetStatus internetStatus )
{
    bool internetBecameAvailable = m_InternetStatus == eInternetNoInternet &&
        internetStatus != eInternetNoInternet;
    lockConnectionList();
    m_InternetStatus = internetStatus;
    unlockConnectionList();
    if( internetBecameAvailable )
    {
        onInternetAvailable();
    }
}

//============================================================================
void ConnectionMgr::callbackNetAvailStatusChanged( ENetAvailStatus netAvalilStatus )
{
    bool networkBecameAvailable = ( m_NetAvailStatus == eNetAvailNoInternet ) && 
        ( netAvalilStatus != eNetAvailNoInternet );
    lockConnectionList();
    m_NetAvailStatus = netAvalilStatus;
    unlockConnectionList();
    if( networkBecameAvailable )
    {
        onNoLimitNetworkAvailable();
    }
}

//============================================================================
void ConnectionMgr::onInternetAvailable( void )
{

}

//============================================================================
void ConnectionMgr::onNoLimitNetworkAvailable( void )
{

}

//============================================================================
void ConnectionMgr::resetDefaultHostUrl( EHostType hostType )
{
    m_DefaultHostIdList[hostType] = VxGUID::nullVxGUID();
    m_DefaultHostUrlList[hostType] = "";
    m_DefaultHostRequiresOnlineId[hostType] = "";
    m_DefaultHostQueryIdFailed[hostType] = std::make_pair(eRunTestStatusUnknown, 0);
}

//============================================================================
void ConnectionMgr::applyDefaultHostUrl( EHostType hostType, std::string& hostUrlIn )
{
    std::string emptyUrl;

    if( m_Engine.getIsMyHostServiceEnabled( hostType ) )
    {
        // I am the host
        std::string myUrlIpv4 = m_Engine.getMyOnlineUrl( false );
        lockConnectionList();
        m_DefaultHostUrlList[ hostType ] = myUrlIpv4;
        m_DefaultHostIdList[ hostType ] = m_Engine.getMyOnlineId();
        unlockConnectionList();

        updateUrlCache( myUrlIpv4, m_Engine.getMyOnlineId() );
        m_Engine.getUrlMgr().updateUrlCache( false, myUrlIpv4, m_Engine.getMyOnlineId() );
        m_Engine.getHostUrlListMgr().updateHostUrl( hostType, m_Engine.getMyOnlineId(), myUrlIpv4, emptyUrl );
        return;
    }

    std::string hostUrl = m_Engine.getUrlMgr().resolveUrl( false, hostUrlIn );

    lockConnectionList();
    m_DefaultHostUrlList[hostType] = hostUrl;
    unlockConnectionList();

    VxUrl parsedUrl( hostUrl.c_str() );
    if( parsedUrl.validateUrl( false ) )
    {
        bool needOnlineId = true;
        VxGUID onlineId = parsedUrl.getOnlineId();
        if( onlineId.isVxGUIDValid() )
        {
            needOnlineId = false;

            lockConnectionList();
            m_DefaultHostIdList[hostType] = onlineId;  
            unlockConnectionList();
            updateUrlCache( hostUrl, onlineId );   
            m_Engine.getUrlMgr().updateUrlCache( false, hostUrl, onlineId );
            if( hostType != eHostTypeUnknown )
            {
                std::string resolvedUrl = m_Engine.getUrlMgr().resolveUrl( false, hostUrl );
                m_Engine.getHostUrlListMgr().updateHostUrl( hostType, onlineId, resolvedUrl, emptyUrl );
            }
        }

        if( needOnlineId )
        {
            lockConnectionList();
            m_DefaultHostRequiresOnlineId[hostType] = hostUrl;
            unlockConnectionList();

            std::string myUrl = m_Engine.getMyOnlineUrl( false );
            static VxGUID sessionId;
            VxGUID::generateNewVxGUID( sessionId );
            m_Engine.getRunUrlAction().runUrlAction( sessionId, eNetCmdQueryHostOnlineIdReq, hostUrl.c_str(), myUrl.c_str(), this, nullptr, hostType );
        }
    }
}

//============================================================================
void ConnectionMgr::callbackQueryIdSuccess( UrlActionInfo& actionInfo, VxGUID onlineId )
{
    std::string emptyUrl;

    if( eHostTypeUnknown != actionInfo.getHostType() )
    {
        lockConnectionList();
        m_DefaultHostIdList[actionInfo.getHostType()] = onlineId;
        m_DefaultHostRequiresOnlineId[actionInfo.getHostType()] = "";
        unlockConnectionList();

        if( eHostTypeNetwork == actionInfo.getHostType() && onlineId.isVxGUIDValid() )
        {
            // exclude network host from updating online status to gui because is just temporary
            // also set the network host id so that we do not block packet announce from network host
            // connection test does not need PktAnnounce so should not need to be excluded
            m_Engine.getConnectIdListMgr().updateOnlineExclusion( onlineId, true, true );
        }
    }

    std::string hostUrl = actionInfo.getRemoteUrl();
    updateUrlCache( hostUrl, onlineId );
    if( actionInfo.getHostType() != eHostTypeUnknown )
    {
        std::string resolvedUrl = m_Engine.getUrlMgr().resolveUrl( false, hostUrl );
        m_Engine.getHostUrlListMgr().updateHostUrl( actionInfo.getHostType(), onlineId, resolvedUrl, emptyUrl );
    }

    if( actionInfo.getConnectReqInterface() )
    {
        actionInfo.getConnectReqInterface()->onUrlActionQueryIdSuccess( actionInfo.getSessionId(), hostUrl, onlineId, actionInfo.getConnectReason() );
    }

    LogMsg( LOG_VERBOSE, "ConnectionMgr: Success query host %s for online id is %s",  hostUrl.c_str(),
        onlineId.toOnlineIdString().c_str());
}

//============================================================================
void ConnectionMgr::callbackActionFailed( UrlActionInfo& actionInfo, ERunTestStatus eStatus, ENetCmdError netCmdError )
{
    if( eHostTypeUnknown != actionInfo.getHostType() )
    {
        int failedCount = getQueryIdFailedCount( actionInfo.getHostType() );
        lockConnectionList();
        m_DefaultHostQueryIdFailed[actionInfo.getHostType()] = std::make_pair(eStatus, failedCount);
        unlockConnectionList();
    }

    std::string hostUrl = actionInfo.getRemoteUrl();
    if( actionInfo.getConnectReqInterface() )
    {
        actionInfo.getConnectReqInterface()->onUrlActionQueryIdFail( actionInfo.getSessionId(), hostUrl, eStatus, actionInfo.getConnectReason() );
    }

    LogMsg( LOG_ERROR, "ConnectionMgr: query host %s for id failed %s %s",  hostUrl.c_str(),
        DescribeRunTestStatus( eStatus ), DescribeNetCmdError( netCmdError ));
}

//============================================================================
void ConnectionMgr::setQueryIdFailedCount( EHostType hostType, int failedCount )
{
    lockConnectionList();
    auto iter = m_DefaultHostQueryIdFailed.find( hostType );
    if( iter != m_DefaultHostQueryIdFailed.end() )
    {
        iter->second.second = failedCount;
    }

    unlockConnectionList();
}

//============================================================================
int ConnectionMgr::getQueryIdFailedCount( EHostType hostType )
{
    int failedCount = 0;
    lockConnectionList();
    auto iter = m_DefaultHostQueryIdFailed.find( hostType );
    if( iter != m_DefaultHostQueryIdFailed.end() )
    {
        failedCount = iter->second.second;
    }

    unlockConnectionList();
    return failedCount;
}

//============================================================================
EConnectStatus ConnectionMgr::requestConnection( VxGUID& sessionId, std::string url, VxGUID onlineId, IConnectRequestCallback* callback, std::shared_ptr<VxSktBase>& retSktBase, EConnectReason connectReason )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectionMgr::requestConnection must have valid online id" );
        return eConnectStatusBadParam;
    }

    if( onlineId == m_Engine.getMyOnlineId() )
    {
        LogMsg( LOG_DEBUG, "ConnectionMgr::requestConnection %s Loopback Socket", DescribeConnectReason( connectReason ) );
        retSktBase = m_Engine.getSktLoopback();
        int64_t timeMs = GetGmtTimeMs();
        retSktBase->setLastActiveTimeMs( timeMs );
        retSktBase->setLastSessionTimeMs( timeMs );
        retSktBase->setLastImAliveTimeRxMs( timeMs );

        if( callback )
        {
            callback->onContactConnected( sessionId, retSktBase, onlineId, connectReason );
        }

        return eConnectStatusReady;
    }

    LogMsg( LOG_DEBUG, "ConnectionMgr::requestConnection %s url %s", DescribeConnectReason( connectReason ), url.c_str() );
    std::shared_ptr<VxSktBase> sktBase( nullptr );

    if( !IsConnectReasonTemporary( connectReason ) )
    {
        // first see if we already have a connection to the requested onlineId
        bool isDisconnected = false;

        // see if we already have a connection for a different reason
        bool isOnline = m_Engine.getConnectIdListMgr().isUserOnline( onlineId );
        if( isOnline )
        {
            sktBase = m_Engine.getConnectIdListMgr().findAnyHostOnlineConnection( onlineId );
        }

        if( !sktBase )
        {
            lockConnectionList();
            ConnectedInfo* connectInfo = m_AllList.getAnyConnectedInfo( onlineId );
            if( connectInfo )
            {
                sktBase = connectInfo->getSktBase();
                if( sktBase )
                {
                    if( sktBase->isConnected() )
                    {
                        uint64_t timeNow = GetTimeStampMs();
                        HandshakeInfo shakeInfo( sktBase, sessionId, onlineId, callback, connectReason, timeNow );
                        connectInfo->addConnectReason( shakeInfo );
                        isDisconnected = false;
                    }
                    else
                    {
                        isDisconnected = true;
                    }
                }
            }

            unlockConnectionList();
        }

        if( sktBase )
        {
            if( eConnectReasonUnknown == sktBase->getConnectReason() )
            {
                // set the connect reason so is marked as temporary connection and not announced to gui
                sktBase->setConnectReason( connectReason );
            }

            m_Engine.getConnectIdListMgr().addConnectionReason( sktBase->getSocketId(), connectReason );
        }

        if( isDisconnected && sktBase )
        {
            onSktDisconnected( sktBase );
            sktBase = nullptr;
        }
        else if( sktBase && callback )
        {
            callback->onContactConnected( sessionId, sktBase, onlineId, connectReason );
        }
    }

    if( sktBase )
    {
        retSktBase = sktBase;
        return eConnectStatusReady;
    }
    else
    {
        return attemptConnection( sessionId, url, onlineId, callback, retSktBase, connectReason );
    }

    return eConnectStatusUnknown;
}

//============================================================================
EConnectStatus ConnectionMgr::attemptConnection( VxGUID& sessionId, std::string url, VxGUID& onlineId, IConnectRequestCallback* callback, std::shared_ptr<VxSktBase>& retSktBase, EConnectReason connectReason )
{
    EConnectStatus connectStatus = eConnectStatusConnecting;
    VxUrl connectUrl( url.c_str() );
    if( onlineId.isVxGUIDValid() && connectUrl.validateUrl( false ) )
    {
        connectStatus = directConnectTo( url, onlineId, callback, retSktBase, sessionId, connectReason );
        if( connectStatus == eConnectStatusConnectSuccess )
        {
            // connected but waiting for PktAnnounce reply
            connectStatus = eConnectStatusReady;
        }
    }
    else
    {
        connectStatus = eConnectStatusBadAddress;
    }

    return connectStatus;
}

//============================================================================
void ConnectionMgr::doneWithConnection( VxGUID socketId, VxGUID sessionId, VxGUID onlineId, IConnectRequestCallback* callback, EConnectReason connectReason )
{
    LogModule( eLogHostConnect, LOG_DEBUG, "HostBaseMgr::doneWithConnection %s", DescribeConnectReason( connectReason ));
    m_HandshakeMutex.lock();
    m_HandshakeList.removeHandshakeInfo( socketId, sessionId );
    m_HandshakeMutex.unlock();

    bool sktDisconnected{ false };
    std::shared_ptr<VxSktBase> sktBase( nullptr );
    lockConnectionList();
    ConnectedInfo* connectInfo = m_AllList.getConnectedInfo( socketId, onlineId );
    if( connectInfo )
    {
        sktDisconnected = connectInfo->removeConnectReason( sessionId, callback, connectReason, sktBase );
    }

    unlockConnectionList();
    if( sktBase )
    {
        m_Engine.getConnectIdListMgr().removeConnectionReason( sktBase->getSocketId(), connectReason );
    }

    if( sktDisconnected && sktBase )
    {
        // this has to be done after list is unlocked or mutex delock can occur
        // let the normal socket disconnected code do the work of removing the connection
        sktBase->closeSkt( eSktCloseConnectReasonsEmpty );
    }
}

//============================================================================
void ConnectionMgr::doneWithConnection( VxGUID sessionId, VxGUID onlineId, IConnectRequestCallback* callback, EConnectReason connectReason )
{
    LogModule( eLogHostConnect, LOG_DEBUG, "HostBaseMgr::doneWithConnection %s", DescribeConnectReason( connectReason ) );
    m_HandshakeMutex.lock();
    m_HandshakeList.removeHandshakeSession( sessionId );
    m_HandshakeMutex.unlock();

    bool sktDisconnected{ false };
    std::vector<std::shared_ptr<VxSktBase>> sktList;
    lockConnectionList();
    m_AllList.removeConnectedReason( sessionId, onlineId, callback, connectReason, sktList );
    unlockConnectionList();
    for( auto sktBase : sktList )
    {
        m_Engine.getConnectIdListMgr().removeConnectionReason( sktBase->getSocketId(), connectReason );
    }

    for( auto sktBase : sktList )
    {
        // this has to be done after list is unlocked or mutex deadlock can occur
        // let the normal socket disconnected code do the work of removing the connection
        sktBase->closeSkt( ConnectReasonToCloseReason( connectReason ) );
    }
}

//============================================================================
void ConnectionMgr::updateUrlCache( std::string& hostUrl, VxGUID& onlineId )
{
    if( !hostUrl.empty() && onlineId.isVxGUIDValid() )
    {
        // there should be only one online id per ip and port however the ip may change
        // only keep the latest url
        m_Engine.getUrlMgr().updateUrlCache( false, hostUrl, onlineId );
        lockConnectionList();
        for( auto iter = m_UrlCache.begin(); iter != m_UrlCache.end(); ++iter )
        {
            if( iter->second == onlineId )
            {
                if( iter->first == hostUrl )
                {
                    // already in map
                    unlockConnectionList();
                    return;
                }

                m_UrlCache.erase( iter );
                break;
            }
        }

        m_UrlCache[hostUrl] = onlineId;
        unlockConnectionList();
    }
    else
    {
        LogMsg( LOG_ERROR, "ConnectionMgr::updateUrlCache empty url" );
    }
}

//============================================================================
bool ConnectionMgr::urlCacheOnlineIdLookup( std::string& hostUrl, VxGUID& onlineId )
{
    onlineId.clearVxGUID();
    bool foundId = m_Engine.getUrlMgr().lookupOnlineId( hostUrl, onlineId );
    if( foundId )
    {
        LogMsg( LOG_VERBOSE, "ConnectionMgr::urlCacheOnlineIdLookup found online Id in cache url %s", hostUrl.c_str() );
        return foundId;
    }

    // it may be part of the url .. if so no lookup required
    VxUrl testUrl( hostUrl );
    if( testUrl.hasValidOnlineId() )
    {
        onlineId = testUrl.getOnlineId();
        foundId = true;
    }

    if( !foundId )
    {
        // not part of url.. see if is in cache
        lockConnectionList();
        auto iter = m_UrlCache.find( hostUrl );
        if( iter != m_UrlCache.end() )
        {
            onlineId = iter->second;
            foundId = true;
        }

        unlockConnectionList();
    }

    return foundId;
}

//============================================================================
EConnectStatus ConnectionMgr::directConnectTo(  std::string                 url,
                                                VxGUID&                     onlineId,
                                                IConnectRequestCallback*    callback,
                                                std::shared_ptr<VxSktBase>& retSktBase,
                                                VxGUID                      sessionId,
                                                EConnectReason              connectReason,
                                                int					        iConnectTimeoutMs,
                                                bool                        ipv6 )
{
    VxUrl connectUrl( url.c_str() );
    std::string ipAddr = connectUrl.getHostString();
    uint16_t port = connectUrl.getPort();
    if( !ipAddr.empty() && port )
    {
        return directConnectTo( ipAddr, port, onlineId, retSktBase, callback, sessionId, connectReason, iConnectTimeoutMs, ipv6 );
    }
    else
    {
        return eConnectStatusBadParam;
    }
}

//============================================================================-
EConnectStatus ConnectionMgr::directConnectTo(  VxConnectInfo&		        connectInfo,
                                                std::shared_ptr<VxSktBase>& ppoRetSkt,		// return pointer to socket if not null
                                                VxGUID                      sessionId, 
                                                int					        iConnectTimeoutMs,// how long to attempt connect
                                                bool				        bUseUdpIp,
                                                bool				        bUseLanIp,
                                                IConnectRequestCallback*    callback,
                                                EConnectReason              connectReason,
                                                bool                        ipv6 )
{
    ppoRetSkt = nullptr;

    std::string ipAddr;

    if( bUseLanIp
        && connectInfo.getLanIPv4().isValid() )
    {
        ipAddr = connectInfo.getLanIPv4().toStdString();
    }
    else
    {
        connectInfo.m_DirectConnectId.getIpAddress( ipv6, ipAddr );
    }

    return directConnectTo( ipAddr, connectInfo.getOnlinePort(), connectInfo.getMyOnlineId(), ppoRetSkt, callback, sessionId, connectReason, iConnectTimeoutMs );
}

//============================================================================-
EConnectStatus ConnectionMgr::directConnectTo(  std::string                 ipAddr,
                                                uint16_t                    port,
                                                VxGUID                      onlineId,
                                                std::shared_ptr<VxSktBase>& retSktBase,
                                                IConnectRequestCallback*    callback,
                                                VxGUID                      sessionId, 
                                                EConnectReason              connectReason,
                                                int					        iConnectTimeoutMs,
                                                bool                        ipv6 )
{
    EConnectStatus connectStatus = eConnectStatusConnecting;

    lockConnectionList();
    std::shared_ptr<VxSktBase> sktBase = m_PeerMgr.connectTo(	ipAddr.c_str(),			// remote ip or url 
                                                                port,	                // port to connect to
                                                                iConnectTimeoutMs );	// milli seconds before connect attempt times out
    if( sktBase )
    {
        if( sktBase->getConnectReason() == eConnectReasonUnknown )
        {
            sktBase->setConnectReason( connectReason );
        }

        // it is possible that the rx thread closes the socket immediately so keep checking the connection
        if( !sktBase->isConnected() )
        {
            LogModule( eLogConnect, LOG_ERROR, "ConnectionMgr::directConnectTo: connection to %s:%d was closed abruptly by rx thread", ipAddr.c_str(), port );
            unlockConnectionList();
            return eConnectStatusSendPktAnnFailed;
        }

        // generate encryption keys
        LogModule( eLogSktData, LOG_VERBOSE, "NetworkMgr::DirectConnectTo: connect success.. generating tx key %s:%d %s", sktBase->getRemoteIp().c_str(), port, onlineId.toHexString().c_str() );

        GenerateTxConnectionKey( sktBase, sktBase->getRemoteIp(), port, onlineId, m_Engine.getNetworkMgr().getNetworkKey() );

        if (!sktBase->isConnected())
        {
            LogModule(eLogConnect, LOG_ERROR, "ConnectionMgr::directConnectTo: connection to %s:%d was closed abruptly by rx thread after tx key generated", ipAddr.c_str(), port);
            unlockConnectionList();
            return eConnectStatusSendPktAnnFailed;
        }

        LogModule( eLogSktData, LOG_VERBOSE, "NetworkMgr::DirectConnectTo: connect success.. generating rx key" );

        GenerateRxConnectionKey( sktBase, &m_Engine.getMyPktAnnounce().m_DirectConnectId, m_Engine.getNetworkMgr().getNetworkKey() );

        if( !sktBase->isConnected() )
        {
            LogModule(eLogConnect, LOG_ERROR, "ConnectionMgr::directConnectTo: connection to %s:%d was closed abruptly by rx thread after rx key generated", ipAddr.c_str(), port);
            unlockConnectionList();
            return eConnectStatusSendPktAnnFailed;
        }

        LogModule( eLogSktData, LOG_VERBOSE, "NetworkMgr::DirectConnectTo: connect success.. sending announce" );

        if( false == sendMyPktAnnounce( onlineId, sktBase, true, false, false ) )
        {
            LogModule( eLogConnect, LOG_DEBUG, "NetworkMgr::DirectConnectTo: connect failed sending announce" );
            unlockConnectionList();
            return eConnectStatusSendPktAnnFailed;
        }

        if( !sktBase->isConnected() )
        {
            LogModule( eLogConnect, LOG_ERROR, "ConnectionMgr::directConnectTo: connection to %s:%d was closed after pkt announce sent by rx thread", ipAddr.c_str(), port);
            unlockConnectionList();
            return eConnectStatusSendPktAnnFailed;
        }

        m_Engine.getConnectIdListMgr().addConnectionReason( sktBase->getSocketId(), connectReason );
        m_HandshakeMutex.lock();
        m_HandshakeList.addHandshake(sktBase, sessionId, onlineId, callback, connectReason);
        m_HandshakeMutex.unlock();
        if( !sktBase->isConnected() )
        {
            LogModule( eLogConnect, LOG_ERROR, "ConnectionMgr::directConnectTo: connection to %s:%d was closed after handshake set by rx thread", ipAddr.c_str(), port);
            m_HandshakeMutex.lock();
            m_HandshakeList.removeHandshake( sktBase );
            m_HandshakeMutex.unlock();

            unlockConnectionList();
            return eConnectStatusSendPktAnnFailed;
        }

        connectStatus = eConnectStatusHandshaking;
        retSktBase = (std::shared_ptr<VxSktBase>&)sktBase;
        
    }
    else
    {
        connectStatus = eConnectStatusConnectFailed;

        //LogMsg( LOG_INFO, "ConnectionMgr::directConnectTo: connect FAIL to %s:%d", strIpAddress.c_str(), connectInfo.getOnlinePort() );
        LogModule( eLogConnect, LOG_DEBUG, "ConnectionMgr::DirectConnectTo: failed" );
    }

    LogModule( eLogConnect, LOG_DEBUG, "ConnectionMgr::DirectConnectTo: done" );
    unlockConnectionList();
    return connectStatus;
}

//============================================================================
void ConnectionMgr::addConnectRequestToQue( VxConnectInfo& connectInfo, EConnectReason connectReason, bool addToHeadOfQue, bool replaceExisting )
{
    ConnectReqInfo connectRequest( connectInfo, connectReason );
    addConnectRequestToQue( connectRequest, addToHeadOfQue, replaceExisting );
}

//============================================================================
void ConnectionMgr::addConnectRequestToQue( ConnectReqInfo& connectRequest, bool addToHeadOfQue, bool replaceExisting  )
{
    m_NetConnectorMutex.lock();
    if( m_IdentsToConnectList.size() )
    {
        // remove any previous that have the same id and reason
        std::vector<ConnectReqInfo>::iterator iter;
        for( iter = m_IdentsToConnectList.begin(); iter != m_IdentsToConnectList.end(); ++iter )
        {
            if( ( (*iter).getMyOnlineId() == connectRequest.getMyOnlineId() )
                && ( (*iter).getConnectReason() == connectRequest.getConnectReason() ) )
            {
                if( replaceExisting )
                {
                    m_IdentsToConnectList.erase( iter );
                    break;
                }
                else
                {
                    m_NetConnectorMutex.unlock();
                    return;
                }
            }
        }
    }

    if( addToHeadOfQue )
    {
        m_IdentsToConnectList.insert( m_IdentsToConnectList.begin(), connectRequest );
    }
    else
    {
        m_IdentsToConnectList.push_back( connectRequest );
    }

    m_NetConnectorMutex.unlock();
    m_WaitForConnectWorkSemaphore.signal();
}

//============================================================================
bool ConnectionMgr::connectToContact(	VxConnectInfo&		connectInfo, 
                                        std::shared_ptr<VxSktBase>&		    ppoRetSkt,
                                        VxGUID&             sessionId,
                                        bool&				retIsNewConnection )
{
    bool gotConnected	= false;
    retIsNewConnection	 = false;
    if( connectInfo.getMyOnlineId() == m_Engine.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "ConnectionMgr::connectToContact: cannot connect to ourself" );  
        return false;
    }

#ifdef DEBUG_CONNECTIONS
    LogMsg( LOG_SKT, "connectToContact: id %s %s",  
        connectInfo.getOnlineName(),
        connectInfo.getMyOnlineId().describeVxGUID().c_str() );
#endif // DEBUG_CONNECTIONS

    lockConnectionList();
    ConnectedInfo * rmtUserConnectInfo = m_AllList.getAnyConnectedInfo( connectInfo.getMyOnlineId() );
    if( rmtUserConnectInfo )
    {
#ifdef DEBUG_CONNECTIONS
        std::string strId;
        connectInfo.getMyOnlineId(strId);
        LogMsg( LOG_SKT, "connectToContact: User is already connected %s id %s", 
            m_Engine.knownContactNameFromId( connectInfo.getMyOnlineId() ),
            strId.c_str() );
#endif // DEBUG_CONNECTIONS

        ppoRetSkt = rmtUserConnectInfo->getSktBase();
        unlockConnectionList();
        gotConnected = true;
    }
    else
    {
        unlockConnectionList();
        if( connectUsingTcp( connectInfo, ppoRetSkt, sessionId ) )
        {
            gotConnected		= true;
            retIsNewConnection	= true;
        }
    }

    return gotConnected;
}

//============================================================================
bool ConnectionMgr::connectUsingTcp( VxConnectInfo&	connectInfo, std::shared_ptr<VxSktBase>& ppoRetSkt, VxGUID& sessionId )
{
    ppoRetSkt = nullptr;
    std::shared_ptr<VxSktBase> sktBase( nullptr );
    if( false == connectInfo.m_DirectConnectId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "connectUsingTcp: User invalid online id" );
        return false;
    }


    bool requiresRelay = connectInfo.requiresRelay();
    std::string strDirectConnectIp;
    connectInfo.m_DirectConnectId.getIpAddress( false, strDirectConnectIp );
 
#if ENABLE_COMPONENT_NEARBY
    if( ( connectInfo.getMyOnlineIPv4() == m_Engine.getMyPktAnnounce().getMyOnlineIPv4())
        && connectInfo.getMyOnlineIPv4().isValid()
        && connectInfo.getLanIPv4().isValid() )
    {
        if( connectInfo.getMyOnlinePort() == m_Engine.getMyPktAnnounce().getMyOnlinePort() )
        {
            LogMsg( LOG_ERROR, "ERROR connectUsingTcp: attempting connect to our ip and port for different id %s", connectInfo.getOnlineName() );
            return false;
        }

        // probably on same network so use local ip
        if( eConnectStatusConnectSuccess == directConnectTo(	connectInfo, 
                                                                sktBase, 
                                                                sessionId,
                                                                LAN_CONNECT_TIMEOUT,
                                                                false, 
                                                                true ) )
        {
#ifdef DEBUG_CONNECTIONS
            connectInfo.getLanIPv4().toStdString( strDirectConnectIp );
            LogMsg( LOG_SKT, "connectUsingTcp: SUCCESS skt %d LAN connect to %s ip %s port %d",
                sktBase->m_SktNumber,
                connectInfo.getOnlineName(),
                strDirectConnectIp.c_str(),
                connectInfo.m_DirectConnectId.getPort() );
#endif // DEBUG_CONNECTIONS
            ppoRetSkt = sktBase;
            return true;
        }
#ifdef DEBUG_CONNECTIONS
        else
        {
            LogMsg( LOG_SKT, "connectUsingTcp: FAIL LAN connect to %s ip %s port %d",
                connectInfo.getOnlineName(),
                strDirectConnectIp.c_str(),
                connectInfo.m_DirectConnectId.getPort() );
        }
#endif // DEBUG_CONNECTIONS
    }

#ifdef DEBUG_CONNECTIONS
    std::string debugClientOnlineId;
    connectInfo.getMyOnlineId(debugClientOnlineId);
    LogMsg( LOG_INFO, "connectUsingTcp %s id %s", 
        connectInfo.getOnlineName(),
        debugClientOnlineId.c_str() );
#endif // DEBUG_CONNECTIONS
#endif // ENABLE_COMPONENT_NEARBY

    // verify proxy if proxy required

    if( requiresRelay )
    {
	    #if ENABLE_IPV6
		    return tryIPv6Connect( connectInfo, ppoRetSkt );
	    #else
		    return false; // no ipv6 support
	    #endif // SUPPORT_IPV6
    }
    else
    {
#ifdef DEBUG_CONNECTIONS
        LogMsg( LOG_INFO, "P2PEngine::connectUsingTcp: Attempting direct connect to %s ip %s port %d",
            m_Engine.knownContactNameFromId( connectInfo.getMyOnlineId() ),
            strDirectConnectIp.c_str(),
            connectInfo.m_DirectConnectId.getPort() );
#endif // DEBUG_CONNECTIONS
        if( eConnectStatusConnectSuccess == directConnectTo( connectInfo, sktBase, sessionId, DIRECT_CONNECT_TIMEOUT ) )
        {
            // direct connection success
#ifdef DEBUG_CONNECTIONS
            LogMsg( LOG_SKT, "connectUsingTcp: SUCCESS skt %d direct connect to %s ip %s port %d",
                sktBase->m_SktNumber,
                m_Engine.knownContactNameFromId( connectInfo.getMyOnlineId() ),
                strDirectConnectIp.c_str(),
                connectInfo.m_DirectConnectId.getPort() );
#endif // DEBUG_CONNECTIONS
            ppoRetSkt = sktBase;

            return true;
        }
        else
        {
#ifdef DEBUG_CONNECTIONS
            LogMsg( LOG_SKT, "directConnectTo: FAIL LAN connect to %s ip %s port %d",
                connectInfo.getOnlineName(),
                strDirectConnectIp.c_str(),
                connectInfo.m_DirectConnectId.getPort() );
#endif // DEBUG_CONNECTIONS
#if ENABLE_IPV6
            return tryIPv6Connect( connectInfo, ppoRetSkt );
#else
            return false; // no ipv6 support
#endif // SUPPORT_IPV6
        }
    }

    if( sktBase )
    {
        ppoRetSkt = sktBase;
        return true;
    }

	#if ENABLE_IPV6
		return tryIPv6Connect( connectInfo, ppoRetSkt );
	#else
		return false; // no ipv6 support
	#endif // SUPPORT_IPV6
}

//============================================================================
bool ConnectionMgr::tryIPv6Connect(	VxConnectInfo& connectInfo, std::shared_ptr<VxSktBase>& ppoRetSkt )
{
    bool connectSuccess = false;
    if( m_Engine.getMyPktAnnounce().getMyOnlineIPv6().isValid()
        && connectInfo.getMyOnlineIPv6().isValid() )
    {
        std::string ipv6;
        ipv6 = connectInfo.getMyOnlineIPv6().toStdString();
        // not likely to succeed so just see if we can get a socket
        SOCKET skt = ::VxConnectToIPv6( ipv6.c_str(), connectInfo.getOnlinePort() );
        if( INVALID_SOCKET != skt )
        {
            ppoRetSkt = m_PeerMgr.createConnectionUsingSocket( skt, ipv6.c_str(), connectInfo.getOnlinePort() );
            connectSuccess = ( nullptr != ppoRetSkt );
        }
    }

    return connectSuccess;
}

//============================================================================
//! encrypt and send my PktAnnounce to someone of whom we have no recored except from anchor announce
bool ConnectionMgr::sendMyPktAnnounce(  VxGUID&				destinationId,
                                        std::shared_ptr<VxSktBase>&			sktBase, 
                                        bool				requestAnnReply,
                                        bool				requestReverseConnection,
                                        bool				requestSTUN )
{
    PktAnnounce pktAnn;
    m_Engine.copyMyPktAnnounce(pktAnn);
    pktAnn.setAppAliveTimeSec( GetApplicationAliveSec() );

    pktAnn.setIsPktAnnReplyRequested( requestAnnReply );
    pktAnn.setIsPktAnnRevConnectRequested( requestReverseConnection );
    pktAnn.setIsPktAnnStunRequested( requestSTUN );

    BigListInfo * poInfo = m_Engine.getBigListMgr().findBigListInfo( destinationId, true );	// id of friend to look for
    if( poInfo )
    {
        EFriendState eMyFriendshipToHim = poInfo->getMyFriendshipToHim();
        EFriendState eHisFriendshipToMe = poInfo->getHisFriendshipToMe();

        pktAnn.setMyFriendshipToHim( eMyFriendshipToHim );
        pktAnn.setHisFriendshipToMe( eHisFriendshipToMe );

        if( eMyFriendshipToHim != eFriendStateAnonymous || eHisFriendshipToMe != eFriendStateAnonymous )
        {
            LogModule( eLogConnect, LOG_DEBUG, "ConnectionMgr::sendMyPktAnnounce myFriendship %s hisFriendship %s",
                    DescribeFriendState( eMyFriendshipToHim ), DescribeFriendState( eHisFriendshipToMe ) );
        }
    }

    return txPacket( destinationId, sktBase, &pktAnn );	
}

//============================================================================
bool ConnectionMgr::txPacket(	VxGUID&				        destinationId, 
                                std::shared_ptr<VxSktBase>&	sktBase, 
                                VxPktHdr*			        poPkt )
{
    bool bSendSuccess = false;
    poPkt->setSrcOnlineId( m_Engine.getMyOnlineId() );
    vx_assert( poPkt->getPktType() == PKT_TYPE_ANNOUNCE );

    if( 0 == (poPkt->getPktLength() & 0xf ) )
    {
        if( sktBase->isConnected() && sktBase->isTxEncryptionKeySet() )
        {
            sktBase->m_u8TxSeqNum++;
            poPkt->setPktSeqNum( sktBase->m_u8TxSeqNum );
            RCODE rc = sktBase->txPacket( destinationId, poPkt, false );
            if( 0 == rc )
            {
                bSendSuccess = true;
            }
            else
            {
                LogMsg( LOG_ERROR, "ConnectionMgr::txPacket: %s error %d", sktBase->describeSktType().c_str(), rc );
            }
        }
        else
        {
            if( false == sktBase->isConnected() )
            {
                LogMsg( LOG_ERROR, "P2PEngine::txSystemPkt: error skt %d not connected", sktBase->m_SktNumber );
            }
            else
            {
                LogMsg( LOG_ERROR, "P2PEngine::txSystemPkt: error skt %d has no encryption key", sktBase->m_SktNumber );
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "P2PEngine::txPluginPkt: Invalid system Packet length %d type %d", 
            poPkt->getPktLength(),
            poPkt->getPktType() );
    }

    return bSendSuccess;
}

//============================================================================
void ConnectionMgr::handleConnectSuccess( BigListInfo * bigListInfo, std::shared_ptr<VxSktBase>& skt, bool isNewConnection, EConnectReason connectReason )
{
    if( 0 != bigListInfo )
    {
        int64_t timeNow = GetGmtTimeMs();
        bigListInfo->setTimeLastConnectAttemptMs( timeNow );

        if( eConnectReasonRandomConnectJoin == connectReason )
        {
            m_Engine.getToGui().toGuiScanResultSuccess( eScanTypeRandomConnect, bigListInfo );
        }
    }
}

//============================================================================
void ConnectionMgr::closeConnection( ESktCloseReason closeReason, VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase, BigListInfo* poInfo )
{
    if( nullptr == poInfo )
    {
        poInfo = m_Engine.getBigListMgr().findBigListInfo( onlineId );
    }

    if( nullptr == poInfo )
    {
        LogMsg( LOG_ERROR, "Failed to find info for %s %s", onlineId.toHexString().c_str(), poInfo->getOnlineName() );
        sktBase->closeSkt( eSktCloseFindBigInfoFail );
        return;
    }

    ConnectedInfo* poRmtUserConnectInfo = m_AllList.getConnectedInfo( sktBase->getSocketId(), onlineId );
    if( poRmtUserConnectInfo )
    {
        /*
        if( poRmtUserConnectInfo->isRelayServer()
            || poRmtUserConnectInfo->isRelayClient() )
        {
            PktRelayUserDisconnect pktRelayDisconnect;
            pktRelayDisconnect.setSrcOnlineId( m_Engine.getMyPktAnnounce().getMyOnlineId() );
            pktRelayDisconnect.m_UserId = onlineId;
            skt->txPacket( onlineId, &pktRelayDisconnect );
        }
        else 
        {
            skt->closeSkt( 236 );
        }
        */

        sktBase->closeSkt( closeReason );
    }
    else
    {
        LogMsg( LOG_ERROR, "Failed to find ConnectedInfo for %s %s", onlineId.toHexString().c_str(), poInfo->getOnlineName() );
        sktBase->closeSkt( eSktCloseFindConnectedInfoFail );
    }
}

//============================================================================
bool ConnectionMgr::doConnectRequest( ConnectReqInfo& connectRequest, bool ignoreToSoonToConnectAgain )
{
    int64_t timeNow = GetGmtTimeMs();
    VxConnectInfo& connectInfo = connectRequest.getConnectInfo();
    if( false == m_Engine.getNetworkStateMachine().isP2POnline() )
    {
         LogMsg( LOG_ERROR, "ConnectionMgr::doConnectRequest when not online" );
    }

    P2PConnectList& connectedList = m_Engine.getConnectList();
    connectedList.connectListLock();
    RcConnectInfo *	rcInfo = connectedList.findConnection( connectRequest.getMyOnlineId(), true );
    if( rcInfo )
    {
        // already connected
        BigListInfo * bigInfo = m_Engine.getBigListMgr().findBigListInfo( connectInfo.getMyOnlineId() );
        if( bigInfo )
        {
            connectedList.connectListUnlock();
            bigInfo->setTimeLastTcpContactMs( timeNow );
            bigInfo->setTimeLastConnectAttemptMs( timeNow );
            connectRequest.setTimeLastConnectAttemptMs( timeNow );
            handleConnectSuccess( bigInfo, rcInfo->getSkt(), false, connectRequest.getConnectReason() );
            return true;
        }
    }

    connectedList.connectListUnlock();

    if( ( false == ignoreToSoonToConnectAgain )
        && connectRequest.isTooSoonToAttemptConnectAgain() )
    {
#ifdef DEBUG_CONNECTIONS
        LogMsg( LOG_INFO, "ConnectionMgr::doConnectRequest: to soon to connect again %s", m_Engine.describeContact( connectRequest ).c_str() );
#endif // DEBUG_CONNECTIONS
        return false;
    }

    connectRequest.setTimeLastConnectAttemptMs( timeNow );
    BigListInfo * bigListInfo = m_Engine.getBigListMgr().findBigListInfo( connectInfo.getMyOnlineId() );
    if( bigListInfo )
    {
        bigListInfo->setTimeLastConnectAttemptMs( timeNow );
    }

    std::shared_ptr<VxSktBase> retSktBase;
    bool isNewConnection = false;
    if( m_Engine.connectToContact( connectInfo, retSktBase, isNewConnection, connectRequest.getConnectReason() ) )
    {
        // handle success connect
#ifdef DEBUG_CONNECTIONS
        LogMsg( LOG_INFO, "ConnectionMgr::doConnectRequest: success  %s", m_Engine.describeContact( connectInfo ).c_str() );
#endif // DEBUG_CONNECTIONS
        if( 0 == bigListInfo )
        {
            // when connected should have created a big list entry when got back a packet announce
            bigListInfo = m_Engine.getBigListMgr().findBigListInfo( connectInfo.getMyOnlineId() );
        }

        if( bigListInfo )
        {
            handleConnectSuccess( bigListInfo, retSktBase, isNewConnection, connectRequest.getConnectReason() );
        }
#ifdef DEBUG_CONNECTIONS
        else
        {
            LogMsg( LOG_INFO, "ConnectionMgr::doConnectRequest: No BigList for connected  %s", m_Engine.describeContact( connectInfo ).c_str() );
        }
#endif // DEBUG_CONNECTIONS

        return true;
    }

    // handle fail connect
#ifdef DEBUG_CONNECTIONS
    LogMsg( LOG_INFO, "ConnectionMgr::doConnectRequest: connect fail  %s", m_Engine.describeContact( connectInfo ).c_str() );
#endif // DEBUG_CONNECTIONS
    return false;
}
