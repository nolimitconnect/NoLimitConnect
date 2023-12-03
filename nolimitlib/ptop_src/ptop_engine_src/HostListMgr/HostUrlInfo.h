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

#include <GuiInterface/IDefs.h>

#include <CoreLib/VxGUID.h>

#include <string>

class HostUrlInfo
{
public:
	HostUrlInfo() = default;
	HostUrlInfo( const HostUrlInfo& rhs );
    HostUrlInfo( EHostType hostType, VxGUID& onlineId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, int64_t timestamp = 0 );
    virtual ~HostUrlInfo() = default;

	HostUrlInfo&				operator=( const HostUrlInfo& rhs ); 

    virtual void				setOnlineId( VxGUID& onlineId )                     { m_OnlineId = onlineId; }
    virtual VxGUID&             getOnlineId( void )                                 { return m_OnlineId; }

    virtual void			    setHostType( enum EHostType friendshipToHim )       { m_HostType = friendshipToHim; }
    virtual EHostType	        getHostType( void )                                 { return m_HostType; }

    virtual void			    setHostUrl( bool ipv6, std::string hostUrl )        { ipv6 ? m_HostUrlIpv6 = hostUrl : m_HostUrlIpv4 = hostUrl; }
    virtual std::string&	    getHostUrl( bool ipv6 )                             { return ipv6 ? m_HostUrlIpv6 : m_HostUrlIpv4; }

    virtual void			    setTimestamp( int64_t timestampMs )                 { m_TimestampMs = timestampMs; }
    virtual int64_t             getTimestamp( void )                                { return m_TimestampMs; }

protected:
	//=== vars ===//
    EHostType                   m_HostType{ eHostTypeUnknown };
    VxGUID                      m_OnlineId;
    std::string                 m_HostUrlIpv4;
    std::string                 m_HostUrlIpv6;
    int64_t                     m_TimestampMs{ 0 };
};
