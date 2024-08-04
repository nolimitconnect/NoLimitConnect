#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxGUID.h>

#include <map>

class NetAvailStatusCallbackInterface
{
public:
    virtual void				callbackInternetStatusChanged( EInternetStatus internetStatus ) = 0;
    virtual void				callbackNetAvailStatusChanged( ENetAvailStatus netAvalilStatus ) = 0;
};


class HostConnection
{
public:
    HostConnection() = delete;
    HostConnection( EHostType hostType, std::string hostUrl, VxGUID& connectId )
        : m_HostType( hostType )
        , m_HostUrl( hostUrl )
        , m_ConnectId( connectId )
    {
    }

    HostConnection( const HostConnection& rhs )
        : m_HostType( rhs.m_HostType )
        , m_HostUrl( rhs.m_HostUrl )
        , m_ConnectId( rhs.m_ConnectId )
    {
    }

    HostConnection& operator =( const HostConnection& rhs )
    {
        if( this != &rhs )
        {
            m_HostType = rhs.m_HostType;
            m_HostUrl = rhs.m_HostUrl;
            m_ConnectId = rhs.m_ConnectId;
        }

        return *this;
    }

    void                        setConnectionId( VxGUID&  connectId )           { m_ConnectId = connectId; }
    VxGUID&                     getConnectionId( void )                         { return m_ConnectId; }
    void                        clearConnectionId( void )                       { m_ConnectId.clearVxGUID(); }

    std::string&                getHostUrl( void )                              { return m_HostUrl; }

protected:
    EHostType                   m_HostType{ eHostTypeUnknown };
    std::string                 m_HostUrl;
    VxGUID&                     m_ConnectId;
};

class P2PEngine;

// network state accumulator
class NetStatusAccum
{
public:
    NetStatusAccum( P2PEngine& toGui );
    virtual ~NetStatusAccum() = default;

    void                        addNetStatusCallback( NetAvailStatusCallbackInterface* callbackInt );
    void                        removeNetStatusCallback( NetAvailStatusCallbackInterface* callbackInt );

    void                        fromGuiNetworkSettingsChanged( void );

    void                        resetNetStatus( void );

    void                        setIsFixedIpAddress( bool fixedIpAddr ) { m_FixedIpAddr = fixedIpAddr; }
    bool                        isFixedIpAddress( void )                { return m_FixedIpAddr; }

    void                        setInternetAvail( bool avail );
    void                        setNetHostAvail( bool avail );
    void                        setConnectionTestAvail( bool avail );
    void                        setDirectConnectTested( bool isTested, bool requiresRelay, std::string& myExternalIp );

    void                        setConnectToRelay( bool connectedToRelay );

    void                        setFirewallTestType( EFirewallTestType firewallTestType );
    void                        setWebsiteUrlsResolved( bool resolved );

    bool                        isInternetAvailable( void )         { return m_InternetAvail; };
    bool                        isNetHostAvailable( void )          { return m_NetworkHostAvail; };
    bool                        isNetHostOnlineIdAvailable( void )  { return m_NetHostIdAvail; };
    bool                        isDirectConnectTested( void )       { return m_DirectConnectTested; };
    bool                        isP2PAvailable( void )              { return m_FixedIpAddr || ( m_DirectConnectTested && (!m_RequriesRelay || m_ConnectedToRelay) ); };
    bool                        isRxPortOpen( void )                { return m_FixedIpAddr || ( m_DirectConnectTested && !m_RequriesRelay ); };
    bool                        requiresRelay( void )               { return m_RequriesRelay; };
    void                        getNodeUrl( bool ipv6, std::string& retNodeUrl );

    void                        setIpPort( uint16_t ipPort);
    uint16_t                    getIpPort( void );

    void                        setExternalIpAddress( bool ipv6 , std::string ipAddr );
    std::string                 getExternalIpAddress( bool ipv6 );

    void                        setLanIpAddress( bool ipv6, std::string ipAddr );
    std::string                 getLanIpAddress( bool ipv6 );

    EInternetStatus             getInternetStatus( void )           { return m_InternetStatus; }
    ENetAvailStatus             getNetAvailStatus( void )           { m_AccumMutex.lock(); ENetAvailStatus status = m_NetAvailStatus;  m_AccumMutex.unlock(); return status;  }

    void                        setNearbyAvailable( bool avail )    { m_NearbyAvailable = avail; }
    bool                        getNearbyAvailable( void )          { return m_NearbyAvailable && (!m_LanIpAddrIpv4.empty() || !m_LanIpAddrIpv6.empty() ); }

    void                        setJoinedHost( EHostType hostType, std::string hostUrl, VxGUID& connectId );
    bool                        isConnectedToHost( EHostType hostType );
    std::string                 getConnectedHostUrl( EHostType hostType );
    void                        onConnectionLost( VxGUID& connectId );

protected:
    void                        onNetStatusChange( void );

    P2PEngine&					m_Engine;
    VxMutex                     m_AccumMutex;
    VxMutex                     m_AccumCallbackMutex;
    std::vector<NetAvailStatusCallbackInterface*> m_CallbackList;

    bool                        m_NearbyAvailable{ false };

    bool                        m_FixedIpAddr{ false };

    bool                        m_InternetAvail{ false };
    bool                        m_NetworkHostAvail{ false };
    bool                        m_ConnectionTestAvail{ false };
    bool                        m_DirectConnectTested{ false };
    bool                        m_RequriesRelay{ false };
    bool                        m_ConnectedToRelay{ false };
    bool                        m_GroupListHostAvail{ false };
    bool                        m_GroupHostAvail{ false };
    bool                        m_IsConnectedGroupHost{ false };
    bool                        m_IsExternalIpValid{ false };

    uint16_t                    m_IpPort{ 0 };
    std::string                 m_ExternAddrIpv6;
    std::string                 m_ExternAddrIpv4;

    std::string                 m_LanIpAddrIpv6;
    std::string                 m_LanIpAddrIpv4;

    bool                        m_NetHostIdAvail{ false };
    VxGUID                      m_NetNostOnlineId;

    EFirewallTestType           m_FirewallTestType{ eFirewallTestUrlConnectionTest };
    EInternetStatus             m_InternetStatus{ eInternetNoInternet };
    ENetAvailStatus             m_NetAvailStatus{ eNetAvailNoInternet };
    bool                        m_WebsiteUrlsResolved{ false };
    std::map<EHostType, HostConnection> m_HostConnectionMap;
};
