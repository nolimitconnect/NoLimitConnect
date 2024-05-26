//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetHostSetting.h"

//============================================================================
NetHostSetting& NetHostSetting::operator =( const NetHostSetting& rhs )
{
	if( this != &rhs )
	{
		m_NetHostSettingName		= rhs.m_NetHostSettingName;
		m_NetworkName				= rhs.m_NetworkName;
		m_NetworkHostUrl		    = rhs.m_NetworkHostUrl;
        m_NetConnectTestUrl         = rhs.m_NetConnectTestUrl;
        m_NetRandomConnectUrl       = rhs.m_NetRandomConnectUrl;
        m_GroupHostUrl              = rhs.m_GroupHostUrl;
        m_ChatRoomHostUrl           = rhs.m_ChatRoomHostUrl;
        m_ExternIpAddr              = rhs.m_ExternIpAddr;
        m_FirewallType              = rhs.m_FirewallType;
        m_UseUpnp                   = rhs.m_UseUpnp;
        m_UseIpv6                   = rhs.m_UseIpv6;
        m_TcpPort	                = rhs.m_TcpPort;
	}

	return *this;
}

//============================================================================
bool NetHostSetting::operator == ( const NetHostSetting& rhs ) const
{
    return 	m_NetHostSettingName == rhs.m_NetHostSettingName &&
        m_NetworkName == rhs.m_NetworkName &&
        m_NetworkHostUrl == rhs.m_NetworkHostUrl &&
        m_NetConnectTestUrl == rhs.m_NetConnectTestUrl &&
        m_NetRandomConnectUrl == rhs.m_NetRandomConnectUrl &&
        m_GroupHostUrl == rhs.m_GroupHostUrl &&
        m_ChatRoomHostUrl == rhs.m_ChatRoomHostUrl &&
        m_ExternIpAddr == rhs.m_ExternIpAddr &&
        m_FirewallType == rhs.m_FirewallType &&
        m_UseUpnp == rhs.m_UseUpnp &&
        m_UseIpv6 == rhs.m_UseIpv6 &&
        m_TcpPort == rhs.m_TcpPort;
}

//============================================================================
bool NetHostSetting::operator != ( const NetHostSetting& rhs ) const
{
    return !( *this == rhs );
}