//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "EngineSettings.h"
#include "EngineSettingsDefaultValues.h"

#include <CoreLib/VxDebug.h>
#include <NetLib/VxGetRandomPort.h>
#include <NetLib/NetHostSettingDefs.h>

#define MY_SETTINGS_DBVERSION 1
#define MY_SETTINGS_KEY "RCKEY"

//============================================================================
EngineSettings::EngineSettings()
: VxSettings( "EngineSettingsDb" )
, m_Initialized( false )
, m_CachedNetworkName("")
{
}

//============================================================================
EngineSettings::~EngineSettings()
{
	engineSettingsShutdown();
}

//============================================================================
RCODE EngineSettings::engineSettingsStartup( std::string& strDbFileName )
{
	if( m_Initialized )
	{
		engineSettingsShutdown();
	}

	RCODE rc = dbStartup(MY_SETTINGS_DBVERSION, strDbFileName.c_str());
	if( 0 == rc )
	{
		m_Initialized = true;
	}
    else
    {
        LogMsg( LOG_VERBOSE, "P2PEngine::%s failed db %s", __func__, strDbFileName.c_str() );
        vx_assert( false );
    }

	return rc;
}

//============================================================================
void EngineSettings::engineSettingsShutdown( void )
{
	if( m_Initialized )
	{
		dbShutdown();
		m_Initialized = false;
	}
}

//============================================================================
void EngineSettings::getNetHostSettings( NetHostSetting& netSettings )
{
    vx_assert( m_Initialized );
    std::string strValue;
    getNetworkKey( strValue );
    netSettings.setNetworkKey( strValue.c_str() );

    getNetworkHostUrl( strValue );
    netSettings.setNetworkHostUrl( strValue.c_str() );

    getConnectTestUrl( strValue );
    netSettings.setConnectTestUrl( strValue.c_str() );

    getRandomConnectUrl( strValue );
    netSettings.setRandomConnectUrl( strValue.c_str() );

    getGroupHostUrl( strValue );
    netSettings.setGroupHostUrl( strValue.c_str() );

    getChatRoomHostUrl( strValue );
    netSettings.setChatRoomHostUrl( strValue.c_str() );

    uint16_t u16Port = getTcpIpPort();
    netSettings.setTcpPort( u16Port );

    bool useIpv6 = getUseIpv6();
    netSettings.setUseIpv6( useIpv6 );

    getUserSpecifiedExternIpAddr( strValue, useIpv6 );
    netSettings.setUserSpecifiedExternIpAddr( strValue.c_str() );

    bool useUpnp = getUseUpnpPortForward();
    netSettings.setUseUpnpPortForward( useUpnp );

    int32_t firewallType = 0;
    EFirewallTestType firewallEnum = getFirewallTestSetting();
    switch( firewallEnum )
    {
    case eFirewallTestAssumeNoFirewall:
        firewallType = 1;
        break;

    case eFirewallTestAssumeFirewalled:
        firewallType = 2;
        break;

    case eFirewallTestUrlConnectionTest:
    default:
        firewallType = 0;
        break;
    }

    netSettings.setFirewallTestType( firewallType );
}

//============================================================================
void EngineSettings::setNetHostSettings( NetHostSetting& netSettings )
{
    vx_assert( m_Initialized );
    bool ipv6 = netSettings.getUseIpv6();
    setNetworkKey( netSettings.getNetworkKey() );
    setNetworkHostUrl( netSettings.getNetworkHostUrl() );
    setConnectTestUrl( netSettings.getConnectTestUrl() );
    setRandomConnectUrl( netSettings.getRandomConnectUrl() );
    setGroupHostUrl( netSettings.getGroupHostUrl() );
    setChatRoomHostUrl( netSettings.getChatRoomHostUrl() );
    setTcpIpPort( netSettings.getTcpPort() );
    setUserSpecifiedExternIpAddr( netSettings.getUserSpecifiedExternIpAddr(), ipv6 );
    setUseIpv6( ipv6 );
    setUseUpnpPortForward( netSettings.getUseUpnpPortForward() );

    int32_t firewallType = netSettings.getFirewallTestType();
    EFirewallTestType firewallEnum =  eFirewallTestUrlConnectionTest;
    switch( firewallType )
    {
    case 1:
        firewallEnum = eFirewallTestAssumeNoFirewall;
        break;

    case 2:
        firewallType = eFirewallTestAssumeFirewalled;
        break;

    default:
        break;
    }

    setFirewallTestSetting( firewallEnum ); 
}

//============================================================================
void EngineSettings::getNetSettings( NetSettings& netSettings )
{
    vx_assert( m_Initialized );
    getNetHostSettings( netSettings );
    
    uint16_t u16MulticastPort = 0;
	getMulticastPort( u16MulticastPort );
	netSettings.setMyMulticastPort( u16MulticastPort );

	uint32_t usrCnt;
	uint32_t sysCnt;
	getMaxRelaysInUse( usrCnt, sysCnt );
	netSettings.setUserRelayPermissionCount( (int)usrCnt );
	netSettings.setSystemRelayPermissionCount( (int)sysCnt );
	netSettings.setAllowMulticastBroadcast( getAllowMulticastBroadcast() );
	netSettings.setAllowUserLocation( getAllowMulticastBroadcast() );
}

//============================================================================
void EngineSettings::setNetSettings( NetSettings& netSettings )
{
    vx_assert( m_Initialized );
    setNetHostSettings( netSettings );

	setMulticastPort( netSettings.getMyMulticastPort() );
	setMulticastEnable( netSettings.getMulticastEnable() );
	setUseUpnpPortForward( netSettings.getUseUpnpPortForward() );
	
	setMaxRelaysInUse( netSettings.getUserRelayPermissionCount(), netSettings.getSystemRelayPermissionCount() );
}

//============================================================================
EFriendViewType EngineSettings::getWhichContactsToView( void )
{
    vx_assert( m_Initialized );
    
	int32_t whichContactView = eFriendViewEverybody;
	getIniValue( MY_SETTINGS_KEY, "WhichContactsToView", whichContactView, eFriendViewEverybody );
    
	return (EFriendViewType)whichContactView;
}

//============================================================================
void EngineSettings::setWhichContactsToView( EFriendViewType eViewType )
{
    vx_assert( m_Initialized );
    
	int32_t whichContactView = (int32_t)eViewType;
	setIniValue( MY_SETTINGS_KEY, "WhichContactsToView", whichContactView );
    
}

//============================================================================
uint16_t EngineSettings::getTcpIpPort( bool bGetRandomIfDoesntExist )
{
    vx_assert( m_Initialized );
    uint16_t u16IpPort;
    
	getIniValue( MY_SETTINGS_KEY, "TcpIpPort", u16IpPort, 0 );
    
	if( 0 == u16IpPort )
	{
        if( bGetRandomIfDoesntExist )
        {
            u16IpPort = VxGetRandomTcpPort();
        }
        
        if( !u16IpPort )
        {
            u16IpPort = NET_DEFAULT_NETSERVICE_PORT;
        }

		setTcpIpPort( u16IpPort );
	}

    return u16IpPort;
}

//============================================================================
void EngineSettings::setTcpIpPort( uint16_t u16IpPort )
{
    vx_assert( m_Initialized );
    
    setIniValue( MY_SETTINGS_KEY, "TcpIpPort", u16IpPort );
    
}

//============================================================================
void EngineSettings::getMulticastPort( uint16_t& u16IpPort )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "UdpIpPort", u16IpPort, DEFAULT_UDP_PORT );
    
}

//============================================================================
void EngineSettings::setMulticastPort( uint16_t u16IpPort )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "UdpIpPort", u16IpPort );
    
}

//============================================================================
void EngineSettings::setMulticastEnable( bool enableMulticast )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "MulticastEnable", enableMulticast );
    
}

//============================================================================
void EngineSettings::getMulticastEnable( bool& enableMulticast )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "MulticastEnable", enableMulticast, false );
    
}

//============================================================================
void EngineSettings::getConnectTestUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "ConnectTestUrl", strWebsiteUrl, NET_DEFAULT_CONNECT_TEST_URL_IPV4 );
    
}

//============================================================================
void EngineSettings::setConnectTestUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "ConnectTestUrl", strWebsiteUrl );
    
}

//============================================================================
void EngineSettings::getNetworkHostUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "NetHostUrl", strWebsiteUrl, NET_DEFAULT_NET_HOST_URL_IPV4 );
    
}

//============================================================================
void EngineSettings::setNetworkHostUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "NetHostUrl", strWebsiteUrl );
    
}

//============================================================================
void EngineSettings::setRandomConnectUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
    setIniValue( MY_SETTINGS_KEY, "RandConnectUrl", strWebsiteUrl );
    
}

//============================================================================
void EngineSettings::getRandomConnectUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
    getIniValue( MY_SETTINGS_KEY, "RandConnectUrl", strWebsiteUrl, NET_DEFAULT_RANDOM_CONNECT_URL_IPV4 );
    
}

//============================================================================
void EngineSettings::setGroupHostUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
    setIniValue( MY_SETTINGS_KEY, "GroupHostUrl", strWebsiteUrl );
    
}

//============================================================================
void EngineSettings::getGroupHostUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
    getIniValue( MY_SETTINGS_KEY, "GroupHostUrl", strWebsiteUrl, NET_DEFAULT_GROUP_HOST_URL_IPV4 );
    
}

//============================================================================
void EngineSettings::setChatRoomHostUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
    setIniValue( MY_SETTINGS_KEY, "ChatRoomHostUrl", strWebsiteUrl );
    
}

//============================================================================
void EngineSettings::getChatRoomHostUrl( std::string& strWebsiteUrl )
{
    vx_assert( m_Initialized );
    
    getIniValue( MY_SETTINGS_KEY, "ChatRoomHostUrl", strWebsiteUrl, NET_DEFAULT_CHAT_ROOM_HOST_URL_IPV4 );
    
}

//============================================================================
void EngineSettings::setUserSpecifiedExternIpAddr( std::string& externIp, bool ipv6 )
{
    vx_assert( m_Initialized );
    if( ipv6 )
    {
        m_CachedExternIpv6 = externIp;
    }
    else
    {
        m_CachedExternIpv4 = externIp;
    }

    
    setIniValue( MY_SETTINGS_KEY, ipv6 ? "ExternIpAddrIpv6" : "ExternIpAddrIpv4", externIp );
    
}

//============================================================================
void EngineSettings::getUserSpecifiedExternIpAddr( std::string& externIp, bool ipv6)
{
    vx_assert( m_Initialized );
    if( ipv6 && !m_CachedExternIpv6.empty() )
    {
        externIp = m_CachedExternIpv6;
        return;
    }

    if( !ipv6 && !m_CachedExternIpv4.empty() )
    {
        externIp = m_CachedExternIpv4;
        return;
    }

    
    getIniValue( MY_SETTINGS_KEY, ipv6 ? "ExternIpAddrIpv6" : "ExternIpAddrIpv4", externIp, "" );
    
    if( ipv6 )
    {
        m_CachedExternIpv6 = externIp;
    }
    else
    {
        m_CachedExternIpv4 = externIp;
    }
}

//============================================================================
std::string EngineSettings::getUserSpecifiedExternIpAddr( bool ipv6  )
{
    vx_assert( m_Initialized );
    std::string externIp;
    getUserSpecifiedExternIpAddr( externIp, ipv6 );
    return externIp;
}

//============================================================================
void EngineSettings::setUseIpv6( bool useIpv6 )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "UseIpv6", useIpv6 );
    m_HasUseIpv6BeenCached = true;
    m_CachedUseIpv6 = useIpv6;
    
}

//============================================================================
bool EngineSettings::getUseIpv6( void )
{
    vx_assert( m_Initialized );
    if( m_HasUseIpv6BeenCached )
    {
        return m_CachedUseIpv6;
    }

	bool useIpv6 = false;
    
	getIniValue( MY_SETTINGS_KEY, "UseIpv6", useIpv6, false );
    m_HasUseIpv6BeenCached = true;
    m_CachedUseIpv6 = useIpv6;
    
	return useIpv6;
}

//============================================================================
void EngineSettings::setUseUpnp( bool useUpnp )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "UseUpnp", useUpnp );
    
}

//============================================================================
bool EngineSettings::getUseUpnp( void )
{
    vx_assert( m_Initialized );
	bool useUpnp = false;
    
	getIniValue( MY_SETTINGS_KEY, "UseUpnp", useUpnp, false );
    
	return useUpnp;
}

//============================================================================
void EngineSettings::setExcludeMeFromNetHostList( bool excludeFromHostList )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "ExcludeFromHostList", excludeFromHostList );
    
}

//============================================================================
bool EngineSettings::getExcludeMeFromNetHostList( void )
{
    vx_assert( m_Initialized );
	bool excludeMe = false;
    
	getIniValue( MY_SETTINGS_KEY, "ExcludeFromHostList", excludeMe, false );
    
	return excludeMe;
}

//============================================================================
void EngineSettings::getNetworkKey( std::string& strNetworkName )
{
    vx_assert( m_Initialized );
	if( m_CachedNetworkName.length() )
	{
		strNetworkName = m_CachedNetworkName;
	}
	else
	{
        
		getIniValue( MY_SETTINGS_KEY, "NetworkName", strNetworkName, NET_DEFAULT_NETWORK_NAME );
		m_CachedNetworkName = strNetworkName;
        
    }
}

//============================================================================
void EngineSettings::setNetworkKey( std::string& strNetworkName )
{
    vx_assert( m_Initialized );
    
	m_CachedNetworkName = strNetworkName;
	setIniValue( MY_SETTINGS_KEY, "NetworkName", strNetworkName );
    
}

//============================================================================
bool EngineSettings::getUseUpnpPortForward( void )
{
    vx_assert( m_Initialized );
	bool bUseUpnp = true;
    
	getIniValue( MY_SETTINGS_KEY, "UseUpnp", bUseUpnp, true );
    

	return bUseUpnp;
}

//============================================================================
void EngineSettings::setUseUpnpPortForward( bool bUseUpnpPortForward )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "UseUpnp", bUseUpnpPortForward );
    
}

//============================================================================
bool EngineSettings::getUseNatPortForward( void )
{
    vx_assert( m_Initialized );
	bool bUseNat = true;
    
	getIniValue( MY_SETTINGS_KEY, "UseNat", bUseNat, false );
    
	return bUseNat;
}

//============================================================================
void EngineSettings::setUseNatPortForward( bool bUseNatPortForward )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "UseNat", bUseNatPortForward );
    
}

//============================================================================
EFirewallTestType EngineSettings::getFirewallTestSetting( void )
{
    vx_assert( m_Initialized );
    if( m_HaveCachedFirewallTestType )
    {
        return m_CachedFirewallTestType;
    }

	uint16_t u16Setting;
    
	getIniValue( MY_SETTINGS_KEY, "FirewallTest", u16Setting, 0 );
    m_CachedFirewallTestType = ( EFirewallTestType )u16Setting;
    m_HaveCachedFirewallTestType = true;
    

	return m_CachedFirewallTestType;
}

//============================================================================
void EngineSettings::setFirewallTestSetting( EFirewallTestType eFirewallTestType )
{
    vx_assert( m_Initialized );
	uint16_t u16Setting = (uint16_t)eFirewallTestType;
    
	setIniValue( MY_SETTINGS_KEY, "FirewallTest", u16Setting );
    m_CachedFirewallTestType = eFirewallTestType;
    m_HaveCachedFirewallTestType = true;
    
}

//=========================================================================
//=== file share settings ===//
//============================================================================
void EngineSettings::getCompletedDnldsDir( std::string& strCompletedDir )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "CompletedDnldsDir", strCompletedDir, "" );
    
}

//============================================================================
void EngineSettings::setCompletedDnldsDir( std::string& strCompletedDir )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "CompletedDnldsDir", strCompletedDir );
    
}

//============================================================================
void EngineSettings::getIncompleteDnldsDir( std::string& strIncompleteDir )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "IncompleteDnldsDir", strIncompleteDir, "" );
    
}

//============================================================================
void EngineSettings::setIncompleteDnldsDir( std::string& strIncompleteDir )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "IncompleteDnldsDir", strIncompleteDir );
    
}

//============================================================================
void EngineSettings::getDnldsBandwidth( uint32_t& u32Bandwidth )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "DnldsBandwidth", u32Bandwidth, 0 );
    
}

//============================================================================
void EngineSettings::setDnldsBandwidth( uint32_t& u32Bandwidth )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "DnldsBandwidth", u32Bandwidth );
    
}

//============================================================================
void EngineSettings::getMaxDownloadingFiles( uint32_t& u32MaxDownloadingFiles )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "MaxDownloadingFiles", u32MaxDownloadingFiles, DEFAULT_MAX_DOWNLOADING_FILES );
    
}

//============================================================================
void EngineSettings::setMaxDownloadingFiles( uint32_t& u32MaxDownloadingFiles )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "MaxDownloadingFiles", u32MaxDownloadingFiles );
    
}

//============================================================================
void EngineSettings::getSharedFilesDirs( std::vector<std::string>& strSharedDirs )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "SharedDirs", strSharedDirs );
    
}

//============================================================================
void EngineSettings::setSharedFilesDirs( std::vector<std::string>& strSharedDirs )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "SharedDirs", strSharedDirs );
    
}

//============================================================================
void EngineSettings::getUpldsBandwidth( uint32_t& u32Bandwidth )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "UpldsBandwidth", u32Bandwidth, 0 );
    
}

//============================================================================
void EngineSettings::setUpldsBandwidth( uint32_t& u32Bandwidth )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "UpldsBandwidth", u32Bandwidth );
    
}

//============================================================================
void EngineSettings::getMaxUploadingFiles( uint32_t& u32MaxUploadingFiles )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "MaxUploadingFiles", u32MaxUploadingFiles, DEFAULT_MAX_UPLOADING_FILES );
    
}

//============================================================================
void EngineSettings::setMaxUploadingFiles( uint32_t& u32MaxUploadingFiles )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "MaxUploadingFiles", u32MaxUploadingFiles );
    
}

//============================================================================
void EngineSettings::setMaxRelaysInUse( uint32_t userRelays, uint32_t anonRelays )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "MaxUserRelays", userRelays );
	setIniValue( MY_SETTINGS_KEY, "MaxSystemRelays", anonRelays );
    
}

//============================================================================
void EngineSettings::getMaxRelaysInUse( uint32_t& userRelays, uint32_t& anonRelays )
{
    vx_assert( m_Initialized );
    
	getIniValue( MY_SETTINGS_KEY, "MaxUserRelays", userRelays, DEFAULT_USER_RELAYS );
	getIniValue( MY_SETTINGS_KEY, "MaxSystemRelays", anonRelays, DEFAULT_ANON_RELAYS );
    
}

//============================================================================
void EngineSettings::setAllowUserLocation( bool allowUserLocation )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "AllowUserLocation", allowUserLocation );
    
}

//============================================================================
bool EngineSettings::getAllowUserLocation( void )
{
    vx_assert( m_Initialized );
	bool allowUserLocation = true;
    
	getIniValue( MY_SETTINGS_KEY, "AllowUserLocation", allowUserLocation, true );
    
	return allowUserLocation;
}

//============================================================================
void EngineSettings::setAllowMulticastBroadcast( bool allowBroadcast )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "AllowBroadcast", allowBroadcast );
    
}

//============================================================================
bool EngineSettings::getAllowMulticastBroadcast( void )
{
    vx_assert( m_Initialized );
	bool allowBroadcast = true;
    
	getIniValue( MY_SETTINGS_KEY, "AllowBroadcast", allowBroadcast, false );
    
	return allowBroadcast;
}

//============================================================================
void EngineSettings::setLastFirewallPort( uint16_t u16IpPort )
{
    vx_assert( m_Initialized );
    
	setIniValue( MY_SETTINGS_KEY, "firewallPort", u16IpPort );
    
}

//============================================================================
uint16_t EngineSettings::getLastFirewallPort( void )
{
    vx_assert( m_Initialized );
	uint16_t u16IpPort;
    
	getIniValue( MY_SETTINGS_KEY, "firewallPort", u16IpPort, 0 );
    
	return u16IpPort;
}

//============================================================================
std::string EngineSettings::fromGuiQueryDefaultUrl( EHostType hostType )
{
    vx_assert( m_Initialized );
    std::string hostUrl( "" );
    switch( hostType )
    {
    case eHostTypeNetwork:
        getNetworkHostUrl( hostUrl );
        break;
    case eHostTypeConnectTest:
        getConnectTestUrl( hostUrl );
        break;
    case eHostTypeGroup:
        getGroupHostUrl( hostUrl );
        break;
    case eHostTypeChatRoom:
        getChatRoomHostUrl( hostUrl );
        break;
    case eHostTypeRandomConnect:
        getRandomConnectUrl( hostUrl );
        break;

    default:
        break;
    }

    return hostUrl;
}

//============================================================================
bool EngineSettings::fromGuiSetDefaultUrl( enum EHostType hostType, std::string& hostUrl )
{
    vx_assert( m_Initialized );
    if( hostUrl.empty() )
    {
        return false;
    }

    bool result{ false };
    switch( hostType )
    {
    case eHostTypeNetwork:
        setNetworkHostUrl( hostUrl );
        result = true;
        break;
    case eHostTypeConnectTest:
        setConnectTestUrl( hostUrl );
        result = true;
        break;
    case eHostTypeGroup:
        setGroupHostUrl( hostUrl );
        result = true;
        break;
    case eHostTypeChatRoom:
        setChatRoomHostUrl( hostUrl );
        result = true;
        break;
    case eHostTypeRandomConnect:
        setRandomConnectUrl( hostUrl );
        result = true;
        break;

    default:
        break;
    }

    return result;
}
