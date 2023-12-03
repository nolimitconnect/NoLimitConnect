//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginNetworkHost.h"
#include "PluginMgr.h"
#include "P2PSession.h"
#include "RxSession.h"
#include "TxSession.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxFileUtil.h>
#include <PktLib/PktsHostInvite.h>

//============================================================================
PluginNetworkHost::PluginNetworkHost( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseHostService( engine, pluginMgr, myIdent, pluginType )
{
    setPluginType( ePluginTypeHostNetwork );
}

//============================================================================
void PluginNetworkHost::onPktHostInviteAnnReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktHostInviteAnnounceReq* hostAnn = ( PktHostInviteAnnounceReq*)pktHdr;
    std::string identName = netIdent && netIdent->getOnlineName() ? netIdent->getOnlineName() : "";
    if( eHostTypeChatRoom == hostAnn->getHostType() )
    {
        if( netIdent->getMyOnlineId() != m_Engine.getMyOnlineId() )
        {
            LogMsg( LOG_VERBOSE, "PluginNetworkHost got chat room announce from %s", identName.c_str() );
        }

        updateHostSearchList( eHostTypeChatRoom, hostAnn, netIdent, sktBase );
    }
    else if( eHostTypeGroup == hostAnn->getHostType() )
    {
        if( netIdent->getMyOnlineId() != m_Engine.getMyOnlineId() )
        {
            LogMsg( LOG_VERBOSE, "PluginNetworkHost got group announce from %s", identName.c_str() );
        }

        updateHostSearchList( eHostTypeGroup, hostAnn, netIdent, sktBase );
    }
    else if( eHostTypeRandomConnect == hostAnn->getHostType() )
    {
        if( netIdent->getMyOnlineId() != m_Engine.getMyOnlineId() )
        {
            LogMsg( LOG_VERBOSE, "PluginNetworkHost got random connect announce from %s", identName.c_str() );
        }

       updateHostSearchList( eHostTypeRandomConnect, hostAnn, netIdent, sktBase );
    }
    else if( eHostTypeNetwork == hostAnn->getHostType() )
    {
        // for now we are the only network host so ignore
        LogMsg( LOG_VERBOSE, "PluginNetworkHost got network announce.. ignored" );
    }
    else if( eHostTypeConnectTest == hostAnn->getHostType() )
    {
        if( netIdent->getMyOnlineId() != m_Engine.getMyOnlineId() )
        {
             LogMsg( LOG_VERBOSE, "PluginNetworkHost got connect test announce from %s", identName.c_str() );
        }

        updateHostSearchList( eHostTypeConnectTest, hostAnn, netIdent, sktBase );
    }
    else
    {
        LogMsg( LOG_VERBOSE, "PluginNetworkHost unknown announce %d from %s", hostAnn->getHostType(), identName.c_str() );
    }
}

//============================================================================
void PluginNetworkHost::updateHostSearchList( EHostType hostType, PktHostInviteAnnounceReq* hostAnn, VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
    m_HostServerMgr.updateHostSearchList( hostType, hostAnn, netIdent, sktBase );
}

//============================================================================
void PluginNetworkHost::fromGuiSendAnnouncedList( EHostType hostType, VxGUID& sessionId )
{
    m_HostServerMgr.fromGuiSendAnnouncedList( hostType, sessionId );
}

//============================================================================
void PluginNetworkHost::fromGuiListAction( EListAction listAction )
{
    m_HostServerMgr.fromGuiListAction( listAction );
}

//============================================================================
void PluginNetworkHost::onPktHostInviteSearchReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostServerMgr.onPktHostInviteSearchReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginNetworkHost::onPktHostInviteSearchReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    handlePktHostInviteSearchReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginNetworkHost::onPktHostInviteMoreReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_HostServerMgr.onPktHostInviteMoreReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginNetworkHost::onPktHostInviteMoreReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    handlePktHostInviteMoreReply( sktBase, pktHdr, netIdent );
}
