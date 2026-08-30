//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginChatRoomHost.h"
#include "PluginMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <BigListLib/BigListInfo.h>

#include <NetLib/VxSktBase.h>

#include <PktLib/PktAdminAvail.h>
#include <PktLib/PktsHostJoin.h>
#include <PktLib/PktsHostSearch.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
PluginChatRoomHost::PluginChatRoomHost( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
    : PluginBaseHostService( engine, pluginMgr, myIdent, pluginType )
{
    setPluginType( ePluginTypeHostChatRoom );
}

//============================================================================
void PluginChatRoomHost::pluginStartup( void )
{
    PluginBaseHostService::pluginStartup();
}

//============================================================================
EMembershipState PluginChatRoomHost::getMembershipState( VxNetIdent* netIdent )
{
    if( eFriendStateIgnore == getPluginPermission() )
    {
        return eMembershipStateJoinDenied;
    }

    return m_HostServerMgr.getMembershipState( netIdent, getHostType() );
}

//============================================================================
void PluginChatRoomHost::fromGuiAdminViewHost( EPluginType pluginType, bool adminIsViewing )
{
    if( pluginType != getPluginType() )
    {
        return;
    }

    // persist ( not just broadcast ) -- consulted by CalendarMgr::extendActiveEventExpiryTimes,
    // see event-calendar design notes
    setAdminIsViewing( adminIsViewing );
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] PluginChatRoomHost::%s admin %s own hosted room", GetApplicationAliveMs(), __func__, adminIsViewing ? "joined" : "exited" );

    GroupieId groupieId( m_Engine.getMyOnlineId(), m_Engine.getMyOnlineId(), eHostTypeChatRoom );
    PktAdminAvail pktAdminAvail;
    pktAdminAvail.setAdminAvailable( adminIsViewing );
    pktAdminAvail.setAdminGroupieId( groupieId );

    VxGUID excludeId;
    broadcastToClients( &pktAdminAvail, excludeId );
}
