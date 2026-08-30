//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginChatRoomClient.h"
#include "PluginMgr.h"

#include <GuiInterface/IToGui.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include <PktLib/PktAdminAvail.h>
#include <PktLib/PktsHostJoin.h>

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif //_MSC_VER

//============================================================================
PluginChatRoomClient::PluginChatRoomClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseMultimedia( engine, pluginMgr, myIdent, pluginType )
, m_HostClientMgr(engine, pluginMgr, myIdent, *this)
, m_CalendarClientMgr(engine, myIdent, *this)
{
	setPluginType( ePluginTypeClientChatRoom );
}

//============================================================================
void PluginChatRoomClient::fromGuiAnnounceHost( HostedId& adminId, VxGUID& sessionId, std::string& ptopUrl )
{
    m_HostClientMgr.fromGuiAnnounceHost( adminId, sessionId, ptopUrl );
}

//============================================================================
void PluginChatRoomClient::fromGuiJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& ptopUrl )
{
    m_HostClientMgr.fromGuiJoinHost( adminId, sessionId, ptopUrl );
}

//============================================================================
void PluginChatRoomClient::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable )
{
    m_HostClientMgr.fromGuiSearchHost( hostType, searchParams, enable );
}

//============================================================================
void PluginChatRoomClient::onPktHostJoinReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if(LogEnabled(eLogChatRoom))LogModule( eLogChatRoom, LOG_DEBUG, "PluginChatRoomClient got join request" );
}

//============================================================================
void PluginChatRoomClient::onPktHostJoinReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if( LogEnabled( eLogChatRoom ) )LogModule( eLogChatRoom, LOG_DEBUG, "PluginChatRoomClient got join reply" );
    m_HostClientMgr.onPktHostJoinReply( sktBase, pktHdr,  netIdent );

    // fetch the calendar now that we have a live connection to the host -- mirrors
    // PluginBaseHostClient::onPktHostJoinReply ( PluginChatRoomClient duplicates rather than
    // inherits that class, see the class comment in PluginChatRoomClient.h, so this hook has to
    // be duplicated here too ). only for our OWN join, not a relayed announce of someone else
    // joining, and only when access was actually granted.
    PktHostJoinReply* pktReply = (PktHostJoinReply*)pktHdr;
    if( pktReply->isValidPktPrefix() && ePluginAccessOk == pktReply->getAccessState() )
    {
        GroupieId userGroupieId = pktReply->getGroupieId();
        if( m_Engine.getMyOnlineId() == userGroupieId.getUserOnlineId() )
        {
            VxGUID hostOnlineId = userGroupieId.getHostOnlineId();
            m_CalendarClientMgr.sendCalendarEventListReq( sktBase, hostOnlineId );
        }
    }
}

//============================================================================
void PluginChatRoomClient::onPktHostSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if( LogEnabled( eLogChatRoom ) )LogModule( eLogChatRoom, LOG_DEBUG, "PluginChatRoomClient got search reply" );
    m_HostClientMgr.onPktHostSearchReply( sktBase, pktHdr,  netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if( LogEnabled( eLogChatRoom ) )LogModule( eLogChatRoom, LOG_DEBUG, "PluginChatRoomClient got join offer request" );
}

//============================================================================
void PluginChatRoomClient::onPktHostOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if( LogEnabled( eLogChatRoom ) )LogModule( eLogChatRoom, LOG_DEBUG, "PluginChatRoomClient got join offer reply" );
}

//============================================================================
void PluginChatRoomClient::onPktHostInviteSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    handlePktHostInviteSearchReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostInviteMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    handlePktHostInviteMoreReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostLeaveReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostLeaveReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostUserListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserListReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostUserListMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserListMoreReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostUserInfoReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserInfoReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktHostUserStatusReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostClientMgr.onPktHostUserStatusReply( sktBase, pktHdr, netIdent );
}

//=== calendar -- duplicated from PluginBaseHostClient, see class comment in PluginChatRoomClient.h ===//

//============================================================================
void PluginChatRoomClient::fromGuiCalendarEventUpdate( HostedId& adminId, CalendarEventInfo& eventInfo )
{
    VxGUID hostOnlineId = adminId.getHostOnlineId();
    m_CalendarClientMgr.sendCalendarEventUpdateReq( hostOnlineId, adminId.getHostType(), eventInfo );
}

//============================================================================
void PluginChatRoomClient::fromGuiCalendarEventCancel( HostedId& adminId, VxGUID& eventId )
{
    VxGUID hostOnlineId = adminId.getHostOnlineId();
    m_CalendarClientMgr.sendCalendarEventCancelReq( hostOnlineId, adminId.getHostType(), eventId );
}

//============================================================================
void PluginChatRoomClient::fromGuiCalendarRefresh( HostedId& adminId )
{
    VxGUID hostOnlineId = adminId.getHostOnlineId();
    GroupieId groupieId( m_Engine.getMyOnlineId(), adminId );
    std::shared_ptr<VxSktBase> sktBase = m_Engine.getConnectIdListMgr().findHostConnection( groupieId );
    if( sktBase )
    {
        m_CalendarClientMgr.sendCalendarEventListReq( sktBase, hostOnlineId );
    }
    else if( LogEnabled( eLogCalendar ) )
    {
        LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] PluginChatRoomClient::%s no live connection to host %s, refresh not sent",
            GetApplicationAliveMs(), __func__, hostOnlineId.toHexString().c_str() );
    }
}

//============================================================================
void PluginChatRoomClient::onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    if( m_Engine.getMyOnlineId() == groupieId.getUserOnlineId() )
    {
        VxGUID hostOnlineId = groupieId.getHostOnlineId();
        m_CalendarClientMgr.clearHostCache( hostOnlineId );
    }
}

//============================================================================
void PluginChatRoomClient::onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    if( m_Engine.getMyOnlineId() == groupieId.getUserOnlineId() )
    {
        VxGUID hostOnlineId = groupieId.getHostOnlineId();
        m_CalendarClientMgr.clearHostCache( hostOnlineId );
    }
}

//============================================================================
void PluginChatRoomClient::onPktCalendarEventListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if(LogEnabled(eLogPkt)) LogModule( eLogPkt, LOG_VERBOSE, "PluginChatRoomClient::%s", __func__ );
    m_CalendarClientMgr.onPktCalendarEventListReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktCalendarEventUpdateReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if(LogEnabled(eLogPkt)) LogModule( eLogPkt, LOG_VERBOSE, "PluginChatRoomClient::%s", __func__ );
    m_CalendarClientMgr.onPktCalendarEventUpdateReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginChatRoomClient::onPktCalendarEventCancelReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    if(LogEnabled(eLogPkt)) LogModule( eLogPkt, LOG_VERBOSE, "PluginChatRoomClient::%s", __func__ );
    m_CalendarClientMgr.onPktCalendarEventCancelReply( sktBase, pktHdr, netIdent );
}

//============================================================================
int64_t PluginChatRoomClient::getActiveCalendarEventExpiresTime( VxGUID& hostOnlineId, int64_t nowMs )
{
    return m_CalendarClientMgr.getActiveEventContentExpiresTime( hostOnlineId, nowMs );
}

//============================================================================
void PluginChatRoomClient::onThreadOncePerMinute( void )
{
    m_CalendarClientMgr.extendActiveEventExpiryTimes( GetGmtTimeMs() );
}
