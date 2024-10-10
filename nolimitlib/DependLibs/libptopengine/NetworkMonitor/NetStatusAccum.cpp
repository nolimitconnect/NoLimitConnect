//============================================================================
// Copyright (C) 2020 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetStatusAccum.h"

#include <P2PEngine/P2PEngine.h>
#include <UrlMgr/UrlMgr.h>
#include <Plugins/PluginMgr.h>

#include <GuiInterface/IToGui.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxSktUtil.h>
#include <NetLib/VxPeerMgr.h>

//============================================================================
NetStatusAccum::NetStatusAccum( P2PEngine& engine )
    : m_Engine(engine)
{
}

//============================================================================
void NetStatusAccum::resetConnectionTestStatus( void )
{
    m_ConnectionTestAvail = false;
    m_DirectConnectTested = false;
    m_RequriesRelay = true;

    m_IsExternalIpValid = false;

    onNetStatusChange();
}

//============================================================================
void NetStatusAccum::resetNetworkStatusAll( void )
{
    resetConnectionTestStatus();

    m_InternetAvail = false;
    m_NetworkHostAvail = false;

    m_ConnectedToRelay = false;
    m_GroupListHostAvail = false;
    m_GroupHostAvail = false;
    m_IsConnectedGroupHost = false;

    onNetStatusChange();
}

//============================================================================
void NetStatusAccum::fromGuiNetworkSettingsChanged( void )
{
    resetNetworkStatusAll();
}

//============================================================================
void NetStatusAccum::onNetStatusChange( void )
{
    ENetAvailStatus netAvailStatus = eNetAvailNoInternet;
    EInternetStatus internetStatus = m_InternetAvail ? eInternetInternetAvailable : eInternetNoInternet;
    if( eFirewallTestAssumeNoFirewall == m_FirewallTestType )
    {
        internetStatus = eInternetAssumeDirectConnect;
        netAvailStatus = eNetAvailP2PAvail; // must be at least P2P available or will not accept incomming accept sockets

        bool ipv6 = m_Engine.getEngineSettings().getUseIpv6();
        if( m_ExternAddr.empty() )
        {
            setExternalIpAddress( m_Engine.getEngineSettings().getUserSpecifiedExternIpAddr( ipv6 ) );
        }

        if( isDirectConnectTested() )
        {
            // direct connect tested is set when first packet announce is recieved when have fixed ip
            if( m_Engine.isNetworkHostEnabled() )
            {
                // we are the host
                netAvailStatus = eNetAvailDirectGroupHost; // 7 bars green
            }
            else
            {
                m_NetAvailStatus = eNetAvailFullOnlineDirectConnect;  // 5 bars green
                if( m_GroupHostAvail )
                {
                    netAvailStatus = eNetAvailDirectGroupHost; // 7 bars green
                }
            }
        }
        else
        {
            if( m_Engine.isNetworkHostEnabled() )
            {
                // we are the host
                netAvailStatus = eNetAvailRelayGroupHost; // 6 bars orange
            }
            else
            {
                netAvailStatus = eNetAvailFullOnlineWithRelay; // 4 bars orange
                if( m_GroupHostAvail )
                {
                    netAvailStatus = eNetAvailRelayGroupHost; // 6 bars orange
                }
            }
        }
    }
    else if( eFirewallTestAssumeFirewalled == m_FirewallTestType )
    {
        if( m_InternetAvail )
        {
            internetStatus = eInternetRequiresRelay;
        }
    }
    else if( eFirewallTestUrlConnectionTest == m_FirewallTestType )
    {
        if( m_InternetAvail )
        {
            if( m_ConnectionTestAvail )
            {
                internetStatus = eInternetTestHostAvailable;
                if( m_DirectConnectTested )
                {
                    if( m_RequriesRelay )
                    {
                        internetStatus = eInternetRequiresRelay;
                    }
                    else
                    {
                        internetStatus = eInternetCanDirectConnect;
                    }
                }
            }
            else
            {
                internetStatus = eInternetTestHostUnavailable;
            }
        }
    }

    if( m_InternetStatus != internetStatus )
    {
        m_AccumMutex.lock();
        m_InternetStatus = internetStatus;
        m_AccumMutex.unlock();

        m_AccumCallbackMutex.lock();
        for( auto callback : m_CallbackList )
        {
            callback->callbackInternetStatusChanged( internetStatus );
        }

        m_AccumCallbackMutex.unlock();

        LogModule( eLogNetAccessStatus, LOG_VERBOSE, "Internet Status %s", DescribeInternetStatus( internetStatus ) );
    }

    if( !( eFirewallTestAssumeNoFirewall == m_FirewallTestType ) )
    {
        if( m_NetworkHostAvail )
        {
            netAvailStatus = eNetAvailHostAvail;
            if( m_DirectConnectTested )
            {
                netAvailStatus = eNetAvailP2PAvail;
                if( requiresRelay() )
                {
                    if( m_ConnectedToRelay )
                    {
                        netAvailStatus = eNetAvailFullOnlineWithRelay;
                    }
                    else
                    {
                        netAvailStatus = eNetAvailOnlineButNoRelay;
                    }
                }
                else
                {
                    netAvailStatus = eNetAvailFullOnlineDirectConnect;
                }

                if( eNetAvailFullOnlineDirectConnect == netAvailStatus )
                {
                    // fully connected
                    if( m_GroupHostAvail )
                    {
                        netAvailStatus = eNetAvailDirectGroupHost;
                    }
                }

                if( eNetAvailFullOnlineWithRelay == netAvailStatus )
                {
                    // fully connected
                    if( m_GroupHostAvail )
                    {
                        netAvailStatus = eNetAvailRelayGroupHost;
                    }
                }
            }
        }
    }

    if( m_NetAvailStatus != netAvailStatus )
    {
        m_AccumMutex.lock();
        m_NetAvailStatus = netAvailStatus;
        m_AccumMutex.unlock();

        m_AccumCallbackMutex.lock();
        for( auto callback : m_CallbackList )
        {
            callback->callbackNetAvailStatusChanged( netAvailStatus );
        }

        m_AccumCallbackMutex.unlock();

        if( eNetAvailFullOnlineDirectConnect == netAvailStatus )
        {
            // just in case direct connect test was skipped by assume no firewall
            m_Engine.getMyPktAnnounce().setRequiresRelay( false );
        }

        LogModule( eLogNetAccessStatus, LOG_VERBOSE, "Net Avail Status %s", DescribeNetAvailStatus( netAvailStatus ) );
        m_Engine.getToGui().toGuiNetAvailableStatus( netAvailStatus );
    }   
}

//============================================================================
void NetStatusAccum::addNetStatusCallback( NetAvailStatusCallbackInterface* callbackInt )
{
    if( callbackInt )
    {
        ENetAvailStatus netAvailStatus = getNetAvailStatus();
        m_AccumCallbackMutex.lock();
        bool alreadyExist = false;
        for( auto callback : m_CallbackList )
        {
            if( callback == callbackInt )
            {
                alreadyExist = true;
                break;
            }
        }

        if( !alreadyExist )
        {
            m_CallbackList.push_back( callbackInt );
            callbackInt->callbackNetAvailStatusChanged( netAvailStatus );
        }

        m_AccumCallbackMutex.unlock();
    }
}

//============================================================================
void NetStatusAccum::removeNetStatusCallback( NetAvailStatusCallbackInterface* callbackInt )
{
    if( callbackInt )
    {
        m_AccumCallbackMutex.lock();
        for( auto iter = m_CallbackList.begin(); iter != m_CallbackList.begin(); ++iter )
        {
            if( *iter == callbackInt )
            {
                m_CallbackList.erase( iter );
                break;
            }
        }

        m_AccumCallbackMutex.unlock();
    }
}

//============================================================================
void NetStatusAccum::setInternetAvail( bool avail )
{
    if( avail != m_InternetAvail )
    {
        m_InternetAvail = avail;
        LogModule( eLogNetAccessStatus, LOG_VERBOSE, "Internet available %d", avail );

        onNetStatusChange();
    }
}

//============================================================================
void NetStatusAccum::setNetHostAvail( bool avail )
{
    if( avail != m_NetworkHostAvail )
    {
        m_NetworkHostAvail = avail;
        LogModule( eLogNetAccessStatus, LOG_VERBOSE, "Network Host available %d", avail );
        onNetStatusChange();
    }
}

//============================================================================
void NetStatusAccum::setConnectionTestAvail( bool avail )
{
    if( avail != m_ConnectionTestAvail )
    {
        m_ConnectionTestAvail = avail;
        LogModule( eLogNetAccessStatus, LOG_VERBOSE, "Connection Test available %d", avail );
        onNetStatusChange();
    }
}

//============================================================================
void NetStatusAccum::setDirectConnectTested( bool isTested, bool requiresRelay, std::string& myExternalIp )
{
    if( !VxIsIpValid( myExternalIp ) )
    {
        LogMsg( LOG_ERROR, "NetStatusAccum::setDirectConnectTested invalid external Ip" );
        return;
    }

    if( !isTested )
    {
        LogMsg( LOG_ERROR, "NetStatusAccum::setDirectConnectTested isTested should not be false" );
        return;
    }

    bool requresRelayChanged{ false };
    bool directConnectTestedChanged{ false };
    bool externIpChanged{ false };

    std::string prevExternIp = getExternalIpAddress();
    if( prevExternIp != myExternalIp )
    {
        externIpChanged = true;
        setExternalIpAddress( myExternalIp );
    }

    if( m_RequriesRelay != requiresRelay )
    {
        requresRelayChanged = true;
        m_RequriesRelay = requiresRelay;
    }

    if( m_DirectConnectTested != isTested )
    {
        directConnectTestedChanged = true;
        m_ConnectionTestAvail = isTested;
        m_DirectConnectTested = isTested;
    }

    if( requresRelayChanged || directConnectTestedChanged || externIpChanged )
    {
        // if assumed no firewall then network ready was called when extern ip was set
        if( m_Engine.getEngineSettings().getFirewallTestSetting() != eFirewallTestAssumeNoFirewall )
        {
            // no need to wait because open port is assumed
            m_Engine.onNetworkConnectionReady( requiresRelay, myExternalIp, m_IpPort );
        } 

        onNetStatusChange();
    }
}

//============================================================================
void NetStatusAccum::setConnectToRelay( bool connectedToRelay )
{
    if( connectedToRelay != m_ConnectedToRelay )
    {
        m_ConnectedToRelay = connectedToRelay;
        LogModule( eLogNetAccessStatus, LOG_VERBOSE, "Connected to relay %d", connectedToRelay );
        onNetStatusChange();
    }
}

//============================================================================
void NetStatusAccum::setFirewallTestType( EFirewallTestType firewallTestType )
{
    if( firewallTestType != m_FirewallTestType )
    {
        m_AccumMutex.lock();
        m_FirewallTestType = firewallTestType;
        m_AccumMutex.unlock();

        onNetStatusChange();
    }
}

//============================================================================
void NetStatusAccum::getNodeUrl( std::string& retNodeUrl )
{
    retNodeUrl = "";

    if( isInternetAvailable() )
    {
        std::string ipAddr = getExternalIpAddress();
        EIpAddrType addrType = VxGetIpAddrType( ipAddr.c_str() );
        if( addrType != eIpAddrTypeUnknown && VxIsPortValid( m_IpPort ) )
        {
            VxMakePtopUrl( ipAddr, m_IpPort, retNodeUrl );
        }
    }
}

//============================================================================
uint16_t NetStatusAccum::getIpPort( void )
{
    uint16_t port = 0;
    m_AccumMutex.lock();
    port = m_IpPort;
    m_AccumMutex.unlock();
    return port;
}

//============================================================================
void NetStatusAccum::setJoinedHost( EHostType hostType, std::string hostUrl, VxGUID& connectId )
{
    m_AccumMutex.lock();
    auto item = m_HostConnectionMap.find( hostType );
    if( item != m_HostConnectionMap.end() ) 
    {
        item->second = HostConnection( hostType, hostUrl, connectId );
    }
    else 
    {
        m_HostConnectionMap.emplace( std::make_pair( hostType, HostConnection( hostType, hostUrl, connectId ) ) );
    }

    m_AccumMutex.unlock();
}

//============================================================================
bool NetStatusAccum::isConnectedToHost( EHostType hostType )
{
    bool isConnected = false;
    m_AccumMutex.lock();
    auto item = m_HostConnectionMap.find( hostType );
    if( item != m_HostConnectionMap.end() )
    {
        if( item->second.getConnectionId().isVxGUIDValid() )
        {
            isConnected = true;
        }
    }

    m_AccumMutex.unlock();
    return isConnected;
}

//============================================================================
void NetStatusAccum::onConnectionLost( VxGUID& connectId )
{
    m_AccumMutex.lock();
    for( auto& hostConn : m_HostConnectionMap )
    {
        if( hostConn.second.getConnectionId() == connectId )
        {
            hostConn.second.clearConnectionId();
        }
    }
    
    m_AccumMutex.unlock();
}

//============================================================================
std::string NetStatusAccum::getConnectedHostUrl( EHostType hostType )
{
    std::string hostUrl;
    m_AccumMutex.lock();
    auto item = m_HostConnectionMap.find( hostType );
    if( item != m_HostConnectionMap.end() )
    {
        if( item->second.getConnectionId().isVxGUIDValid() )
        {
            hostUrl = item->second.getHostUrl();
        }
    }

    m_AccumMutex.unlock();
    return hostUrl;
}

//============================================================================
void NetStatusAccum::setIpPort( uint16_t ipPort )
{
    if( VxIsPortValid( ipPort ) )
    {
        bool changedPort{false};
        m_AccumMutex.lock();
        if( m_IpPort != ipPort )
        {
            m_IpPort = ipPort;
            changedPort = true;
        }

        m_AccumMutex.unlock();
        if( changedPort )
        {
            m_Engine.getPeerMgr().setListenPort( ipPort );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "NetStatusAccum::setIpPort invalid port %s", ipPort );
    }
}

//============================================================================
void NetStatusAccum::setExternalIpAddress( std::string ipAddr )
{
    if( VxIsIpValid( ipAddr ) )
    {
        bool changedIp{false};
        m_AccumMutex.lock();
        if( m_ExternAddr != ipAddr )
        {
            m_ExternAddr = ipAddr;
            changedIp = true;
        }

        m_AccumMutex.unlock();
        if( changedIp )
        {
            m_Engine.lockAnnouncePktAccess();
            m_Engine.getMyPktAnnounce().setOnlineIpAddress( ipAddr.c_str() );
            std::string myNodeUrl = m_Engine.getMyPktAnnounce().getMyOnlineUrl();
            m_Engine.unlockAnnouncePktAccess();

            m_Engine.getUrlMgr().setMyOnlineNodeUrl( myNodeUrl );
            m_Engine.getPluginMgr().onMyOnlineUrlIsValid( true );
            if( m_Engine.getEngineSettings().getFirewallTestSetting() == eFirewallTestAssumeNoFirewall )
            {
                // no need to wait because open port is assumed
                m_Engine.onNetworkConnectionReady( false, ipAddr, m_IpPort );
            }          
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "NetStatusAccum::%s invalid ip %s", __func__, ipAddr.c_str() );
    }
}

//============================================================================
std::string NetStatusAccum::getExternalIpAddress( void )
{
    m_AccumMutex.lock();
    std::string ipAddr = m_ExternAddr;
    m_AccumMutex.unlock();
    return ipAddr;
}

//============================================================================
void NetStatusAccum::setLocalIpAddress( std::string ipAddr )
{
    if( VxIsIpValid( ipAddr ) )
    {
        bool changedIp{false};
        m_AccumMutex.lock();
        if( m_LocalAddr != ipAddr )
        {
            m_LocalAddr = ipAddr;
            changedIp = true;
        }

        m_AccumMutex.unlock();
    }
    else
    {
        LogMsg( LOG_ERROR, "NetStatusAccum::%s invalid ip %s", __func__, ipAddr.c_str() );
    }
}

//============================================================================
std::string NetStatusAccum::getLocalIpAddress( void )
{
    m_AccumMutex.lock();
    std::string ipAddr = m_LocalAddr;
    m_AccumMutex.unlock();
    return ipAddr;
}

//============================================================================
void NetStatusAccum::setUseFixedIp( std::string externIp )
{
    m_AccumMutex.lock();
    m_HasFixedIpAddr = true;
    m_ExternAddr = externIp;
    m_AccumMutex.unlock();
}

//============================================================================
void NetStatusAccum::setUseIpv6( bool useIpv6, uint16_t ipPort )
{
    // this will only get called on startup or engine settings changed
    // use it to get default local addresses
    bool addrChanged{ false };
    bool portChanged{ false };
    bool useIpv6Changed{ false };
    
    if( m_LocalAddrIpv6.empty() )
    {
        if( VxGetDefaultLocalIp( true, m_LocalAddrIpv6 ) )
        {        
            addrChanged |= true;
        }
    }

    if( m_LocalAddrIpv4.empty() )
    {
        if( VxGetDefaultLocalIp( false, m_LocalAddrIpv4 ) )
        {
            addrChanged |= true;
        }
    }

    std::string useThisLocaIpAddr;
    lockAccum();
    if( ipPort != m_IpPort )
    {
        m_IpPort = ipPort;
        portChanged = true;
    }

    if( useIpv6 != m_UseIpv6 )
    {
        m_UseIpv6 = useIpv6;
        useIpv6Changed = true;
    }

    if( useIpv6 && ( m_LocalAddr.empty() || ( !m_LocalAddrIpv6.empty() && m_LocalAddr != m_LocalAddrIpv6 ) ) )
    {
        m_LocalAddr = m_LocalAddrIpv6;
        addrChanged |= true;
    }
    else if( m_LocalAddr.empty() || ( !m_LocalAddrIpv4.empty() && m_LocalAddr != m_LocalAddrIpv4 )  )
    {
        m_LocalAddr = m_LocalAddrIpv4;
        addrChanged |= true;
    }

    useThisLocaIpAddr = m_LocalAddr;
    unlockAccum();

    // we want to open port and start listening as soon as possible in startup
    if( addrChanged || portChanged || useIpv6Changed )
    {
        if( portChanged )
        {
            m_Engine.getMyPktAnnounce().setOnlinePort( ipPort );
            m_Engine.setPktAnnLastModTime( GetTimeStampMs() );
        }

        if( !useThisLocaIpAddr.empty() )
        {
            m_Engine.getPeerMgr().setLocalIp( useThisLocaIpAddr );
            m_Engine.getPeerMgr().startListening( useIpv6, ipPort );
            if( addrChanged )
            {
                // if we have a local ip address we have internet
                setInternetAvail( true );
            }
        }
    }
}

//============================================================================
bool NetStatusAccum::isHostResolved( EHostType hostType )
{
    VxGUID retHostOnlineId;
    if( m_Engine.getConnectionMgr().getDefaultHostOnlineId( hostType, retHostOnlineId ) )
    {
        return retHostOnlineId.isVxGUIDValid();
    }

    return false;
}

//============================================================================
bool NetStatusAccum::getResolvedHost( EHostType hostType, std::string& retHostUrl )
{
    VxGUID hostOnlineId;
    if( m_Engine.getConnectionMgr().getDefaultHostOnlineId( hostType, hostOnlineId ) && hostOnlineId.isVxGUIDValid() )
	{
        return m_Engine.getHostUrlListMgr().getResolvedHostUrl( hostType, hostOnlineId, retHostUrl );
	} 

    return false;
}