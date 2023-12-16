//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseHostClient.h"
#include "PluginMgr.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>
#include <ptop_src/ptop_engine_src/UserJoinMgr/UserJoinMgr.h>
#include <NetLib/VxPeerMgr.h>

#include <CoreLib/VxPtopUrl.h>
#include <NetLib/VxSktBase.h>
#include <PktLib/SearchParams.h>
#include <PktLib/PktsHostJoin.h>
#include <PktLib/PktsGroupie.h>

//============================================================================
PluginBaseHostClient::PluginBaseHostClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
    : PluginBase( engine, pluginMgr, myIdent, pluginType )
    , m_ConnectionMgr(engine.getConnectionMgr())
    , m_HostClientMgr(engine, pluginMgr, myIdent, *this)
{
}

//============================================================================
void PluginBaseHostClient::fromGuiAnnounceHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    if( !ptopUrlIpv4.empty() || !ptopUrlIpv6.empty() )
    {
        //VxGUID hostGuid;
        //EHostJoinStatus joinStatus = m_ConnectionMgr.lookupOrQueryId( hostType, url.c_str(), hostGuid, this);
    }
    else
    {
        m_Engine.getToGui().toGuiHostAnnounceStatus( hostType, sessionId, eHostAnnounceInvalidUrl );
    }
}

//============================================================================
void PluginBaseHostClient::fromGuiJoinHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    if(  !ptopUrlIpv4.empty() || !ptopUrlIpv6.empty()  )
    {
        //VxGUID hostGuid;
        //EHostJoinStatus joinStatus = m_ConnectionMgr.lookupOrQueryId( hostType, url.c_str(), hostGuid, this);
    }
    else
    {
        m_Engine.getToGui().toGuiHostJoinStatus( hostType, sessionId, eHostJoinInvalidUrl );
    }
}

//============================================================================
void PluginBaseHostClient::fromGuiLeaveHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    if(  !ptopUrlIpv4.empty() || !ptopUrlIpv6.empty()  )
    {
        sendLeaveHost( hostType, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
    }
}

//============================================================================
void PluginBaseHostClient::fromGuiUnJoinHost( EHostType hostType, VxGUID& sessionId,std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    if( ptopUrlIpv4.empty() || !ptopUrlIpv6.empty()  )
    {
        sendUnJoinHost( hostType, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
    }
    else
    {
        m_Engine.getToGui().toGuiHostJoinStatus( hostType, sessionId, eHostJoinInvalidUrl );
    }
}

//============================================================================
void PluginBaseHostClient::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable )
{
    std::string url = searchParams.getSearchUrl();
    if( !url.empty() )
    {
        //VxGUID hostGuid; // TODO
        //EHostJoinStatus joinStatus = m_ConnectionMgr.lookupOrQueryId( hostType, url.c_str(), hostGuid, this);
    }
    else
    {
        m_Engine.getToGui().toGuiHostSearchStatus( hostType, searchParams.getSearchSessionId(), eHostSearchInvalidUrl );
        m_Engine.getToGui().toGuiHostSearchComplete( hostType, searchParams.getSearchSessionId() );
    }
}

//============================================================================
bool PluginBaseHostClient::fromGuiRequestPluginThumb( VxNetIdent* netIdent, VxGUID& thumbId )
{
    if( netIdent && thumbId.isVxGUIDValid() )
    {
        std::shared_ptr<VxSktBase> sktBase( nullptr );
        m_PluginMgr.pluginApiSktConnectTo( getPluginType(), netIdent, 0, sktBase );
        if( sktBase && sktBase->isConnected() )
        {
            // the netIdent from gui is not the same one as in big list
            BigListInfo* bigListInfo = m_Engine.getBigListMgr().findBigListInfo( netIdent->getMyOnlineId() );
            if( bigListInfo )
            {
                return ptopEngineRequestPluginThumb( sktBase, bigListInfo->getVxNetIdent(), thumbId );
            }
        }
    }

    return false;
}

//============================================================================
bool PluginBaseHostClient::ptopEngineRequestPluginThumb( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, VxGUID& thumbId )
{
    return m_ThumbXferMgr.requestPluginThumb( sktBase, netIdent, thumbId );
}

//============================================================================
void PluginBaseHostClient::sendLeaveHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    VxGUID hostOnlineId = m_Engine.getOnlineIdFromUrl( ptopUrlIpv4, ptopUrlIpv6 );
    if( hostOnlineId.isVxGUIDValid() )
    {
        GroupieId groupieId( m_Engine.getMyOnlineId(), hostOnlineId, hostType );
        sendLeaveHost( groupieId );
    }
}

//============================================================================
bool PluginBaseHostClient::sendLeaveHost( GroupieId& groupieId )
{
    LogModule( eLogHosts, LOG_VERBOSE, "PluginBaseHostClient::sendLeaveHost groupie %s my online id %s",
               groupieId.describeGroupieId().c_str(), m_Engine.getMyOnlineId().describeVxGUID().c_str());
    bool pktSent{ false };
    std::shared_ptr<VxSktBase> sktBase =  m_Engine.getConnectIdListMgr().findHostConnection( groupieId );
    if( sktBase )
    {
        PktHostLeaveReq leaveReq;
        leaveReq.setGroupieId( groupieId );
        leaveReq.setPluginType( HostTypeToClientPlugin( groupieId.getHostType() ) );
        if( m_Engine.getMyOnlineId() == groupieId.getHostOnlineId() )
        {
            // is ourself
            pktSent = txPacket( m_Engine.getMyOnlineId(), m_Engine.getSktLoopback(), &leaveReq );
        }
        else
        {
            pktSent = txPacket( groupieId.getHostOnlineId(), sktBase, &leaveReq );
            if( pktSent )
            {
                m_Engine.getUserJoinMgr().onUserLeftHost( groupieId );
            }
        }
    }

    return pktSent;
}

//============================================================================
void PluginBaseHostClient::sendUnJoinHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    VxGUID hostOnlineId = m_Engine.getOnlineIdFromUrl( ptopUrlIpv4, ptopUrlIpv6 );
    if( hostOnlineId.isVxGUIDValid() )
    {
        GroupieId groupieId( m_Engine.getMyOnlineId(), hostOnlineId, hostType );
        sendUnJoinHost( groupieId );
    }
}

//============================================================================
bool PluginBaseHostClient::sendUnJoinHost( GroupieId& groupieId )
{
    bool pktSent{ false };
    std::shared_ptr<VxSktBase> sktBase = m_Engine.getConnectIdListMgr().findHostConnection( groupieId );
    if( sktBase )
    {
        PktHostUnJoinReq leaveReq;
        leaveReq.setHostType( groupieId.getHostType() );
        leaveReq.setPluginType( HostTypeToClientPlugin( groupieId.getHostType() ) );
        if( m_Engine.getMyOnlineId() == groupieId.getHostOnlineId() )
        {
            // is ourself
            pktSent = txPacket( m_Engine.getMyOnlineId(), m_Engine.getSktLoopback(), &leaveReq );
        }
        else
        {
            pktSent = txPacket( groupieId.getHostOnlineId(), sktBase, &leaveReq );
            if( pktSent )
            {
                m_Engine.getUserJoinMgr().onUserLeftHost( groupieId );
            }
        }
    }

    return pktSent;
}

//============================================================================
bool PluginBaseHostClient::queryUserListFromHost( GroupieId& groupieId )
{
    bool pktSent{ false };
    std::shared_ptr<VxSktBase> sktBase = m_Engine.getConnectIdListMgr().findHostConnection( groupieId );
    if( sktBase )
    {
        PktGroupieSearchReq pktReq;
        VxGUID sessionId;
        sessionId.initializeWithNewVxGUID();

        pktReq.setSearchSessionId( sessionId );
        pktReq.setHostType( groupieId.getHostType() );
        if( m_Engine.getMyOnlineId() == groupieId.getHostOnlineId() )
        {
            // is ourself
            pktSent = txPacket( m_Engine.getMyOnlineId(), m_Engine.getSktLoopback(), &pktReq );
        }
        else
        {
            pktSent = txPacket( groupieId.getHostOnlineId(), sktBase, &pktReq );
        }
    }

    return pktSent;
}

//============================================================================
void PluginBaseHostClient::onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{

    m_HostClientMgr.onUserJoinedHost( groupieId, sktBase, netIdent );
}

//============================================================================
void PluginBaseHostClient::onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    m_HostClientMgr.onUserLeftHost( groupieId, sktBase, netIdent );
}

//============================================================================
void PluginBaseHostClient::onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    m_HostClientMgr.onUserUnJoinedHost( groupieId, sktBase, netIdent );
}

//============================================================================
void PluginBaseHostClient::onGroupRelayedUserAnnounce( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    m_HostClientMgr.onGroupRelayedUserAnnounce( groupieId, sktBase, netIdent );
}

//============================================================================
void PluginBaseHostClient::onPktHostJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostJoinReply %s", pktHdr->describePktHdr().c_str() );
    PktHostJoinReply* pktReply = (PktHostJoinReply*)pktHdr;
    GroupieId userGroupieId = pktReply->getGroupieId();
    LogModule( eLogMembership, LOG_VERBOSE, "PluginBaseHostClient::onPktHostJoinReply %s", m_Engine.describeGroupieId(userGroupieId).c_str() );
    m_Engine.getConnectIdListMgr().addConnection( sktBase->getSocketId(), userGroupieId, netIdent->getMyOnlineId() != userGroupieId.getUserOnlineId() );
}

//============================================================================
void PluginBaseHostClient::onPktHostLeaveReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostLeaveReply %s", pktHdr->describePktHdr().c_str() );
    PktHostLeaveReply* pktReply = (PktHostLeaveReply*)pktHdr;
    GroupieId userGroupieId = pktReply->getGroupieId();
    LogModule( eLogMembership, LOG_VERBOSE, "PluginBaseHostClient::onPktHostLeaveReply %s", m_Engine.describeGroupieId(userGroupieId).c_str() );
    m_Engine.getConnectIdListMgr().removeConnection( sktBase->getSocketId(), userGroupieId );
}

//============================================================================
void PluginBaseHostClient::onPktHostUnJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostUnJoinReply" );
}

//============================================================================
void PluginBaseHostClient::onPktHostOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostOfferReply" );
}

//============================================================================
void PluginBaseHostClient::onPktHostInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostInfoReply" );
    m_Engine.getHostedListMgr().onPktHostInfoReply( sktBase, pktHdr, netIdent, this );
}

//============================================================================
void PluginBaseHostClient::onPktHostSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostSearchReply" );
}

//============================================================================
void PluginBaseHostClient::onPktGroupieInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktGroupieInfoReply" );
    m_Engine.getGroupieListMgr().onPktGroupieInfoReply( sktBase, pktHdr, netIdent, this );
}

//============================================================================
void PluginBaseHostClient::onPktGroupieAnnReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktGroupieAnnReply" );
    m_Engine.getGroupieListMgr().onPktGroupieAnnReply( sktBase, pktHdr, netIdent, this );
}

//============================================================================
void PluginBaseHostClient::onPktGroupieSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktGroupieSearchReply" );
     m_Engine.getGroupieListMgr().onPktGroupieSearchReply( sktBase, pktHdr, netIdent, this );
}

//============================================================================
void PluginBaseHostClient::onPktGroupieMoreReply ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktGroupieMoreReply" );
    m_Engine.getGroupieListMgr().onPktGroupieMoreReply( sktBase, pktHdr, netIdent, this );
}

//============================================================================
void PluginBaseHostClient::onPktHostUserListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostUserListReply" );
}

//============================================================================
void PluginBaseHostClient::onPktHostUserListMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogModule( eLogPkt, LOG_VERBOSE, "PluginBaseHostClient::onPktHostUserListMoreReply" );
}

