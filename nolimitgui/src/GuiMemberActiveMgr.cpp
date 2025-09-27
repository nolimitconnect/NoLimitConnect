//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiMemberActiveMgr.h"

#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiMemberActiveCallback.h"
#include "GuiUserMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <Membership/MemberActiveMgr.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
GuiMemberActiveMgr::GuiMemberActiveMgr()
    : QObject()
{
}

//============================================================================
void GuiMemberActiveMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalMemberActive(GroupieId,bool)), this, SLOT(slotInternalMemberActive(GroupieId,bool)), Qt::QueuedConnection );

    GetPtoPEngine().getMemberActiveMgr().wantMemberActiveCallbacks( this, true );
}

//============================================================================
bool GuiMemberActiveMgr::isMemberActive( GroupieId& groupieId )
{
    if( VxGetShowMyselfInLists() && groupieId.getUserOnlineId() == GetPtoPEngine().getMyOnlineId() && groupieId.getHostOnlineId() == GetPtoPEngine().getMyOnlineId() )
    {
        // is myself and allowed
        return true;
    }

    auto iter = std::find(m_MemberList.begin(), m_MemberList.end(), groupieId);
    return iter != m_MemberList.end();
}

//============================================================================
bool GuiMemberActiveMgr::isMemberOfHostType( EHostType hostType, VxGUID& onlineId )
{
    for( auto groupieId : m_MemberList )
    {
        if( hostType == groupieId.getHostType() && groupieId.getUserOnlineId() == onlineId )
        {
            if( groupieId.getHostOnlineId() != onlineId )
            {
                return true;
            }
        }
    }

    return false;
}

//============================================================================
bool GuiMemberActiveMgr::isActiveMemberOfAny( VxGUID& onlineId )
{
    for( auto groupieId : m_MemberList )
    {
        if( groupieId.getUserOnlineId() == onlineId || groupieId.getHostOnlineId() == onlineId )
        {
            return true;
        }
    }

    return false;
}

//============================================================================
void GuiMemberActiveMgr::getActiveMembers( HostedId& hostId, std::set<VxGUID>& memberList )
{
    memberList.clear();
    for( auto groupieId : m_MemberList )
    {
        if( hostId == groupieId.getHostedId() )
        {
            memberList.insert( groupieId.getUserOnlineId() );
        }
    }
}

//============================================================================
void GuiMemberActiveMgr::callbackMemberActive( GroupieId& groupieId, bool isActive )
{
    if( !isActive && groupieId.getUserOnlineId() == GetAppInstance().getMyOnlineId() )
    {
        LogMsg( LOG_WARN, "GuiMemberActiveMgr::%s member is now inactive", __func__ );
    }

    emit signalInternalMemberActive( groupieId, isActive );
}

//============================================================================
void GuiMemberActiveMgr::slotInternalMemberActive( GroupieId groupieId, bool isActive )
{
    updateMemberActive( groupieId, isActive );
}

//============================================================================
void GuiMemberActiveMgr::updateMemberActive( GroupieId& groupieId, bool isActive )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "MemberActiveMgr::updateMemberActive invalid groupieId %s", GetAppInstance().describeGroupieId(groupieId).c_str());
        return;
    }

    LogModule( eLogMembership, LOG_VERBOSE, "MemberActiveMgr::updateMemberActive groupieId %s active %d", GetAppInstance().describeGroupieId( groupieId ).c_str(), isActive );

    bool wasMember = isMemberOfHostType( groupieId.getHostType(), groupieId.getUserOnlineId() );

    bool wasUpdated = false;
    bool wasFound = false;
    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); ++iter )
    {
        if( *iter == groupieId )
        {
            wasFound = true;
            if( !isActive )
            {
                m_MemberList.erase( iter );
                wasUpdated = true;
            }

            break;
        }
    }

    if( !wasFound && isActive )
    {
        m_MemberList.emplace_back( groupieId );
        wasUpdated = true;
    }

    if( wasUpdated )
    {
        announceMemberActive( groupieId, isActive );
        bool isMember = isMemberOfHostType( groupieId.getHostType(), groupieId.getUserOnlineId() );
        if( wasMember != isMember )
        {
            announceMemberIsJoinedToHost( groupieId.getUserOnlineId(), groupieId.getHostType(), isMember );
        }
    }
}

//============================================================================
void GuiMemberActiveMgr::wantMemberActiveCallback( GuiMemberActiveCallback* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "GuiMemberActiveMgr null client" );
        return;
    }

    bool wasFound = false;
    for( auto iter = m_MemberClients.begin(); iter != m_MemberClients.end(); ++iter )
    {
        if( *iter == client )
        {
            wasFound = true;
            if( !enable )
            {
                m_MemberClients.erase( iter );
            }

            break;
        }
    }

    if( enable && !wasFound )
    {
        m_MemberClients.emplace_back( client );
    }
}

//============================================================================
void GuiMemberActiveMgr::announceMemberActive( GroupieId& groupieId, bool isActive )
{
    for( auto& client : m_MemberClients )
    {
        client->callbackGuiMemberActive( groupieId, isActive );
    }
}

//============================================================================
void GuiMemberActiveMgr::announceMemberIsJoinedToHost( VxGUID& onlineId, EHostType host, bool isJoined )
{
    GuiUser* guiUser = GetAppInstance().getUserMgr().getUser( onlineId );
    if( guiUser )
    {
        guiUser->setIsJoined( host, isJoined );
    }

    for( auto& client : m_MemberClients )
    {
        client->callbackGuiMemberIsJoinedToHost( onlineId, host, isJoined );
    }
}

//============================================================================
GroupieId GuiMemberActiveMgr::getJoinedAdminGroupieId( EHostType hostType, VxGUID& onlineId )
{
    bool wasFound{ false };
    GroupieId groupieId;
    for( auto& memberGroupieId : m_MemberList )
    {
        if( memberGroupieId.getHostType() == hostType )
        {
            if( memberGroupieId.getUserOnlineId() == onlineId )
            {
                wasFound = true;
                groupieId = memberGroupieId;
                break;
            }
        }
    }

    if( !wasFound )
    {
        LogMsg( LOG_VERBOSE, "GuiMemberActiveMgr::%s not found", __func__ );
    }

    return groupieId;
}
