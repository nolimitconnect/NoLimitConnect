//============================================================================
// Copyright (C) 2019 Brett R. Jones
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

#include "PluginRandomConnectClient.h"
#include "PluginMgr.h"
#include "P2PSession.h"
#include "RxSession.h"
#include "TxSession.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxFileUtil.h>

//============================================================================
PluginRandomConnectClient::PluginRandomConnectClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseHostClient( engine, pluginMgr, myIdent, pluginType )
{
    setPluginType( ePluginTypeClientRandomConnect );
}
//============================================================================
void PluginRandomConnectClient::fromGuiAnnounceHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrl )
{
    std::string url = !ptopUrl.empty() ? ptopUrl : "";
    m_HostClientMgr.fromGuiAnnounceHost( hostType, sessionId, url );
}

//============================================================================
void PluginRandomConnectClient::fromGuiJoinHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrl )
{
    std::string url = !ptopUrl.empty() ? ptopUrl : "";
    m_HostClientMgr.fromGuiJoinHost( hostType, sessionId, url );
}

//============================================================================
void PluginRandomConnectClient::fromGuiLeaveHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrl )
{
    std::string url = !ptopUrl.empty() ? ptopUrl : "";
    m_HostClientMgr.fromGuiLeaveHost( hostType, sessionId, url );
}

//============================================================================
void PluginRandomConnectClient::fromGuiUnJoinHost( EHostType hostType, VxGUID& sessionId, std::string& ptopUrl )
{
    std::string url = !ptopUrl.empty() ? ptopUrl : "";
    m_HostClientMgr.fromGuiUnJoinHost( hostType, sessionId, url );
}

//============================================================================
void PluginRandomConnectClient::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable )
{
    m_HostClientMgr.fromGuiSearchHost( hostType, searchParams, enable );
}

//============================================================================
void PluginRandomConnectClient::onPktHostJoinReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginRandomConnectClient got join request" );
}

//============================================================================
void PluginRandomConnectClient::onPktHostJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginRandomConnectClient got join reply" );
    m_HostClientMgr.onPktHostJoinReply( sktBase, pktHdr,  netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginRandomConnectClient got search reply" );
    m_HostClientMgr.onPktHostSearchReply( sktBase, pktHdr,  netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginRandomConnectClient got join offer request" );
}

//============================================================================
void PluginRandomConnectClient::onPktHostOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    LogMsg( LOG_DEBUG, "PluginRandomConnectClient got join offer reply" );
}

//============================================================================
void PluginRandomConnectClient::onPktHostInviteSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    handlePktHostInviteSearchReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostInviteMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    handlePktHostInviteMoreReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostLeaveReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostLeaveReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostUserListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserListReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostUserListMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserListMoreReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostUserInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserInfoReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginRandomConnectClient::onPktHostUserStatusReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserStatusReply( sktBase, pktHdr, netIdent );
}
