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

#include "UrlMgr.h"

#include <NetLib/VxSktUtil.h>
#include <string.h>

//============================================================================
UrlMgr& GetUrlMgrInstance()
{
    static UrlMgr g_UrlMgr;
    return g_UrlMgr;
}

//============================================================================
void UrlInfo::updateOnlineId( VxGUID& onlineId )
{
    if( m_OnlineId == onlineId )
    {
        // no changes needed
        return;
    }

    m_OnlineId = onlineId;

    updateUrl( false );
    updateUrl( true );
}

//============================================================================
void UrlInfo::updateUrl( bool ipv6 )
{
    std::string url( "ptop://" );
    if( ipv6 ? m_UrlIpv6.size() > 7 && 0 == strncmp( m_UrlIpv6.c_str(), "http://", 7 ) :  m_UrlIpv4.size() > 7 && 0 == strncmp( m_UrlIpv4.c_str(), "http://", 7 ) )
    {
        url = "http://";
    }

    std::string& ipAddr = ipv6 ? m_IpAddrIpv6 : m_IpAddrIpv4;

    url += ipAddr;
    url += ":";
    url += std::to_string( m_Port );
    url += "/";
    url += m_OnlineId.toOnlineIdString();

    ipv6 ? m_UrlIpv6 = url : m_UrlIpv4 = url;
}

//============================================================================
bool UrlMgr::lookupOnlineId( std::string& hostUrl, VxGUID& onlineId )
{
    m_UrlMutex.lock();
    auto iter = m_UrlMap.find( hostUrl );
    if( iter != m_UrlMap.end() )
    {
        if( iter->second.m_OnlineId.isVxGUIDValid() )
        {
            onlineId = iter->second.m_OnlineId;
            m_UrlMutex.unlock();
            return true;
        }
    }

    m_UrlMutex.unlock();
    return false;
}

//============================================================================
void UrlMgr::setMyOnlineNodeUrl( bool ipv6, std::string& myNodeUrl )
{
    if( fillUrlInfo( ipv6, myNodeUrl, m_MyUrlInfo ) )
    {
        ipv6 ? m_MyNodeUrlIpv6 = myNodeUrl : m_MyNodeUrlIpv4 = myNodeUrl;
        m_UrlMutex.lock();
        for( auto iter = m_UrlMap.begin(); iter != m_UrlMap.end(); ++iter )
        {
            if( !iter->second.m_OnlineId.isVxGUIDValid() )
            {
                if( iter->second.m_Port == m_MyUrlInfo.m_Port && iter->second.m_IpAddrIpv6 == m_MyUrlInfo.m_IpAddrIpv6 || iter->second.m_IpAddrIpv4 == m_MyUrlInfo.m_IpAddrIpv4  )
                {
                    iter->second.updateOnlineId( m_MyUrlInfo.m_OnlineId );
                }
            }
        }

        m_UrlMutex.unlock();
    }
}

//============================================================================
std::string UrlMgr::resolveUrl( bool ipv6, std::string& hostUrl )
{
    if( hostUrl.empty() )
    {
        return hostUrl;
    }

    std::string url;
    m_UrlMutex.lock();
    auto iter = m_UrlMap.find( hostUrl );
    if( iter != m_UrlMap.end() )
    {
        url = ipv6 ? iter->second.m_UrlIpv6 : iter->second.m_UrlIpv4;
        m_UrlMutex.unlock();
        return url;
    }

    m_UrlMutex.unlock();

    UrlInfo urlInfo;
    if( fillUrlInfo( ipv6, hostUrl, urlInfo ) )
    {
        url = ipv6 ? urlInfo.m_UrlIpv6 : urlInfo.m_UrlIpv4;

        m_UrlMutex.lock();
        m_UrlMap[hostUrl] = urlInfo;
        m_UrlMutex.unlock();
    }

    return url;
}

//============================================================================
void UrlMgr::updateUrlCache( bool ipv6, std::string& hostUrl, VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        return;
    }

    std::string ipAddr;
    uint16_t tcpPort{ 0 };

    bool result = VxResolveUrl( hostUrl, tcpPort, ipAddr, ipv6 );

    bool foundUrl = false;
    m_UrlMutex.lock();
    auto iter = m_UrlMap.find( hostUrl );
    if( iter != m_UrlMap.end() )
    {
        foundUrl = true;
        if( onlineId != iter->second.m_OnlineId )
        {
            iter->second.updateOnlineId( onlineId );
        }
    }

    for( auto iter = m_UrlMap.begin(); iter != m_UrlMap.end(); ++iter )
    {
        if( !iter->second.m_OnlineId.isVxGUIDValid() && iter->second.m_Port == tcpPort && ipv6 ? iter->second.m_IpAddrIpv6 == ipAddr : iter->second.m_IpAddrIpv4 == ipAddr)
        {
            iter->second.updateOnlineId( onlineId );
        }
    }

    m_UrlMutex.unlock();

    if( !foundUrl )
    {
        addUrlAndOnlineId( ipv6, hostUrl, onlineId );
    }
}

//============================================================================
bool UrlMgr::addUrl( bool ipv6, std::string& hostUrl )
{
    bool urlAdded{ false };
    UrlInfo urlInfo;
    if( fillUrlInfo( ipv6, hostUrl, urlInfo ) )
    {
        m_UrlMutex.lock();
        m_UrlMap[hostUrl] = urlInfo;
        m_UrlMutex.unlock();
        urlAdded = true;
    }

    return urlAdded;
}

//============================================================================
bool UrlMgr::addUrlAndOnlineId( bool ipv6, std::string& hostUrl, VxGUID& onlineId )
{
    bool urlAdded{ false };
    UrlInfo urlInfo;
    if( fillUrlInfo( ipv6, hostUrl, urlInfo ) )
    {
        urlInfo.updateOnlineId( onlineId );
        m_UrlMutex.lock();
        m_UrlMap[hostUrl] = urlInfo;
        m_UrlMutex.unlock();
        urlAdded = true;
    }

    return urlAdded;
}

//============================================================================
bool UrlMgr::fillUrlInfo( bool ipv6, std::string& hostUrl, UrlInfo& urlInfo )
{
    std::string strHost;
    std::string strFile;
    uint16_t tcpPort{ 0 };

    bool result = VxSplitHostAndFile( hostUrl.c_str(), strHost, strFile, tcpPort );
    if( result )
    {
        InetAddress inetAddr;
        urlInfo.m_Port = tcpPort;
        if( VxIsIPv4Address( strHost.c_str() ) )
        {
            urlInfo.m_IpAddrIpv4 = strHost;
        }
        else if( VxIsIPv6Address( strHost.c_str() ) )
        {
            urlInfo.m_IpAddrIpv4 = strHost;
        }
        else if( VxResolveUrl( strHost.c_str(), tcpPort, inetAddr, ipv6 ) )
        {
           ipv6 ?  urlInfo.m_IpAddrIpv6 = inetAddr.toStdString() : urlInfo.m_IpAddrIpv4 = inetAddr.toStdString();
        }
        else
        {
            return false;
        }

        bool hadOnlineId = false;
        if( !strFile.empty() && strFile[0] == '!' )
        {
            // possibly has online id
            VxGUID onlineId;
            onlineId.fromOnlineIdString( strFile.c_str() );
            if( onlineId.isVxGUIDValid() )
            {
                urlInfo.m_OnlineId = onlineId;
                hadOnlineId = true;
            }
        }
        else if( urlInfo.m_Port == m_MyUrlInfo.m_Port && ipv6 ? urlInfo.m_IpAddrIpv6 == m_MyUrlInfo.m_IpAddrIpv6 : urlInfo.m_IpAddrIpv4 == m_MyUrlInfo.m_IpAddrIpv4 )
        {
            urlInfo.m_OnlineId = m_MyUrlInfo.m_OnlineId;
            strFile = urlInfo.m_OnlineId.toOnlineIdString();
            hadOnlineId = true;
        }

        std::string url( "ptop://" );
        if( hostUrl.size() > 7 && 0 == strncmp( hostUrl.c_str(), "http://", 7 ) )
        {
            url = "http://";
        }

        url += ipv6 ? urlInfo.m_IpAddrIpv6 : urlInfo.m_IpAddrIpv4;
        url += ":";
        url += std::to_string( tcpPort );
        if( hadOnlineId )
        {
            url += "/";
            url += strFile;
        }

        ipv6 ? urlInfo.m_UrlIpv6 = url : urlInfo.m_UrlIpv4 = url;
    }

    return result;
}
