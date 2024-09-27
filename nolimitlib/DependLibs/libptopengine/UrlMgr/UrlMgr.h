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

    std::string                 m_Url;
    std::string                 m_IpAddr;

    VxGUID                      m_OnlineId;
    uint16_t                    m_Port{ 0 };
    uint64_t                    m_ModifiedTimestamp{ 0 };
};

class UrlMgr
{
public:
    UrlMgr() = default;
    ~UrlMgr() = default;

    void                        setMyOnlineNodeUrl( std::string& myNodeUrl );

    std::string                 resolveUrl( std::string& hostUrl );
    void                        updateUrlCache( std::string& hostUrl, VxGUID& onlineId );

    bool                        lookupOnlineId( std::string& hostUrl, VxGUID& onlineId );

protected:
    bool                        addUrl( std::string& hostUrl );
    bool                        addUrlAndOnlineId( std::string& hostUrl, VxGUID& onlineId );
    bool                        fillUrlInfo( std::string& hostUrl, UrlInfo& urlInfo );

    VxMutex                     m_UrlMutex;
    std::map<std::string, UrlInfo> m_UrlMap;
    std::string                 m_MyNodeUrlIpv4;
    std::string                 m_MyNodeUrlIpv6;
    UrlInfo                     m_MyUrlInfo;
};

UrlMgr& GetUrlMgrInstance();
