#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxGUID.h>

class IgnoredHostInfo
{
public:
    IgnoredHostInfo() = default;
    IgnoredHostInfo( VxGUID& onlineId, VxGUID& thumbId, std::string hostUrlIpv4, std::string hostUrlIpv6, std::string hostTitle, std::string hostDescription, int64_t timestampMs );

    IgnoredHostInfo( const IgnoredHostInfo& rhs );

    virtual ~IgnoredHostInfo() = default;

    IgnoredHostInfo& operator=( const IgnoredHostInfo& rhs );

    void                        setOnlineId( VxGUID& onlineId )             { m_OnlineId = onlineId; }
    VxGUID&                     getOnlineId()                               { return m_OnlineId; }
    void                        setThumbId( VxGUID& thumbId )               { m_ThumbId = thumbId; }
    VxGUID&                     getThumbId( void )                            { return m_ThumbId; }
    void                        setHostUrl( bool ipv6, std::string& hostUrl )      { ipv6 ? m_HostUrlIpv6 = hostUrl : m_HostUrlIpv4 = hostUrl; }
    std::string&                getHostUrl( bool ipv6 )                            { return ipv6 ? m_HostUrlIpv6 : m_HostUrlIpv4; }
    void                        setHostTitle( std::string& hosTitle )           { m_HostTitle = hosTitle; }
    std::string&                getHostTitle( void )                            { return m_HostTitle; }
    void                        setHostDescription( std::string& hosDescription ) { m_HostDescription = hosDescription; }
    std::string&                getHostDescription( void )                      { return m_HostDescription; }
    void                        setTimestampMs( int64_t timestampMs )           { m_TimestampMs = timestampMs; }
    int64_t                     getTimestampMs( void )                          { return m_TimestampMs; }

protected:
    VxGUID                      m_OnlineId;
    VxGUID                      m_ThumbId;
    std::string                 m_HostUrlIpv4;
    std::string                 m_HostUrlIpv6;
    std::string                 m_HostTitle;
    std::string                 m_HostDescription;
    int64_t                     m_TimestampMs{ 0 };
};