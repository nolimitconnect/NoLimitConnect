#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones 
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
#include <CoreLib/VxMutex.h>

#include <map>

class UrlInfo
{
public:
    UrlInfo() = default;
    ~UrlInfo() = default;

    void                        updateOnlineId( VxGUID& onlineId );
    void                        updateUrl( bool ipv6 );

    std::string                 m_UrlIpv4;
    std::string                 m_UrlIpv6;
    std::string                 m_IpAddrIpv4;
    std::string                 m_IpAddrIpv6;
    VxGUID                      m_OnlineId;
    uint16_t                    m_Port{ 0 };
    uint64_t                    m_ModifiedTimestamp{ 0 };
};

class UrlMgr
{
public:
    UrlMgr() = default;
    ~UrlMgr() = default;

    void                        setMyOnlineNodeUrl( bool ipv6, std::string& myNodeUrl );

    std::string                 resolveUrl( bool ipv6, std::string& hostUrl );
    void                        updateUrlCache( bool ipv6, std::string& hostUrl, VxGUID& onlineId );

    bool                        lookupOnlineId( std::string& hostUrl, VxGUID& onlineId );

protected:
    bool                        addUrl( bool ipv6, std::string& hostUrl );
    bool                        addUrlAndOnlineId( bool ipv6, std::string& hostUrl, VxGUID& onlineId );
    bool                        fillUrlInfo( bool ipv6, std::string& hostUrl, UrlInfo& urlInfo );

    VxMutex                     m_UrlMutex;
    std::map<std::string, UrlInfo> m_UrlMap;
    std::string                 m_MyNodeUrlIpv4;
    std::string                 m_MyNodeUrlIpv6;
    UrlInfo                     m_MyUrlInfo;
};

UrlMgr& GetUrlMgrInstance();