//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "HostUrlInfo.h"

//============================================================================
HostUrlInfo::HostUrlInfo( EHostType hostType, VxGUID& onlineId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, int64_t timestamp )
    : m_HostType( hostType )
    , m_OnlineId( onlineId )
    , m_HostUrlIpv4( hostUrlIpv4 )
    , m_HostUrlIpv6( hostUrlIpv6 )
    , m_TimestampMs( timestamp )
{
}

//============================================================================
HostUrlInfo::HostUrlInfo( const HostUrlInfo& rhs )
    : m_HostType( rhs.m_HostType )
    , m_OnlineId( rhs.m_OnlineId )
    , m_HostUrlIpv4( rhs.m_HostUrlIpv4 )
    , m_HostUrlIpv6( rhs.m_HostUrlIpv6 )
    , m_TimestampMs( rhs.m_TimestampMs )
{
}

//============================================================================
HostUrlInfo& HostUrlInfo::operator=( const HostUrlInfo& rhs ) 
{	
	if( this != &rhs )
	{
        m_HostType = rhs.m_HostType;
        m_OnlineId = rhs.m_OnlineId;
        m_HostUrlIpv4 = rhs.m_HostUrlIpv4;
        m_HostUrlIpv6 = rhs.m_HostUrlIpv6;
        m_TimestampMs = rhs.m_TimestampMs;
    }

	return *this;
}
