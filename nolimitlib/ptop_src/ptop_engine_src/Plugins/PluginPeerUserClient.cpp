//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginPeerUserClient.h"
#include "PluginMgr.h"
#include "P2PSession.h"
#include "RxSession.h"
#include "TxSession.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/P2PEngine/P2PConnectList.h>
#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>

#include <NetLib/VxSktBase.h>
#include <CoreLib/VxFileUtil.h>

//============================================================================
PluginPeerUserClient::PluginPeerUserClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseHostClient( engine, pluginMgr, myIdent, pluginType )
{
    setPluginType( ePluginTypeClientPeerUser );
}

//============================================================================
void PluginPeerUserClient::fromGuiAnnounceHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    m_HostClientMgr.fromGuiAnnounceHost( hostType, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void PluginPeerUserClient::fromGuiJoinHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    m_HostClientMgr.fromGuiJoinHost( hostType, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void PluginPeerUserClient::fromGuiUnJoinHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrlIpv4, std::string& ptopUrlIpv6 )
{
    m_HostClientMgr.fromGuiUnJoinHost( hostType, sessionId, ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void PluginPeerUserClient::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable )
{
    m_HostClientMgr.fromGuiSearchHost( hostType, searchParams, enable );
}

//============================================================================
void PluginPeerUserClient::onPktHostJoinReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginPeerUserClient got join request" );
}

//============================================================================
void PluginPeerUserClient::onPktHostJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginPeerUserClient got join reply" );
    m_HostClientMgr.onPktHostJoinReply( sktBase, pktHdr,  netIdent );
}

//============================================================================
void PluginPeerUserClient::onPktHostSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginPeerUserClient got search reply" );
    m_HostClientMgr.onPktHostSearchReply( sktBase, pktHdr,  netIdent );
}

//============================================================================
void PluginPeerUserClient::onPktHostOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginPeerUserClient got join offer request" );
}

//============================================================================
void PluginPeerUserClient::onPktHostOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginPeerUserClient got join offer reply" );
}
