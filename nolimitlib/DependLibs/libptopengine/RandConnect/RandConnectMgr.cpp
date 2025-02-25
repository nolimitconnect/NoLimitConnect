//============================================================================
// Copyright (C) 2024 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "RandConnectMgr.h"

#include "RandConnectCallback.h"
#include <Membership/MemberActiveMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <algorithm>

//============================================================================
void RandConnectMgr::onEngineStartup( void )
{
    GetPtoPEngine().getMemberActiveMgr().wantMemberActiveCallbacks( this, true );
}

//============================================================================
void RandConnectMgr::onEngineShutdown( void )
{
    GetPtoPEngine().getMemberActiveMgr().wantMemberActiveCallbacks( this, false );
}

//============================================================================
void RandConnectMgr::updateRandConnectStatus( GroupieId& groupieId, VxGUID& toUserOnlineId, enum ERandAction randAction )
{
    GroupieId toUserGroupie( groupieId );
    toUserGroupie.setUserOnlineId( toUserOnlineId );
    updateRandConnectStatus( groupieId, randAction );
    updateRandConnectStatus( toUserGroupie, randAction );
}

//============================================================================
void RandConnectMgr::updateRandConnectStatus( GroupieId& groupieId, enum ERandAction randAction )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "RandConnectMgr::updateSktIdent invalid skt id" );
        return;
    }

    bool wasUpdated = false;
    lockMemberList();

    bool found{ false };
    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); ++iter )
    {
        if( iter->first == groupieId )
        {
            found = true;
            if( randAction == eRandActionNone )
            {
                m_MemberList.erase( iter );
                wasUpdated = true;
            }
            else if( iter->second != randAction )
            {
                iter->second = randAction;
                wasUpdated = true;
            }

            break;
        }
    }

    if( randAction != eRandActionNone && !found )
    {
        m_MemberList.emplace_back( std::make_pair( groupieId, randAction ) );
    }

    unlockMemberList();

    if( wasUpdated )
    {
        announceRandConnect( groupieId, randAction );
    }
}

//============================================================================
void RandConnectMgr::announceRandConnect( GroupieId& groupieId, enum ERandAction randAction )
{
    lockClientList();

    for( auto& client : m_MemberClients )
    {
        client->callbackRandConnect( groupieId, randAction );
    }

    unlockClientList();
}

//============================================================================
void RandConnectMgr::wantRandConnectCallbacks( RandConnectCallback* client, bool enable )
{
    lockClientList();

    bool found{ false };
    for( auto iter = m_MemberClients.begin(); iter != m_MemberClients.end(); ++iter )
    {
        if( *iter == client )
        {
            found = true;
            if( !enable )
            {
                m_MemberClients.erase( iter );
            }
            else
            {
                LogMsg( LOG_ERROR, "RandConnectMgr::wantRandConnectCallbacks ignored because already in list" );
            }

            break;
        }
    }
    
    if( !found && enable )
    {
        m_MemberClients.emplace_back( client );
    }

    unlockClientList();
}

//============================================================================
void RandConnectMgr::callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    if( isOnline )
    {
        return;
    }
    
    std::vector<std::pair<GroupieId,ERandAction>> offlineMemberList;
    lockMemberList();

    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); )
    {
        if( iter->first.getHostOnlineId() == onlineId  || iter->first.getUserOnlineId() == onlineId )
        {
            offlineMemberList.emplace_back( *iter );
            iter = m_MemberList.erase( iter );
        }
        else
        {
            ++iter;
        }
    }

    unlockMemberList();

    for( auto& randPair : offlineMemberList )
    {
        announceRandConnect( randPair.first, eRandActionNone );
    }
}

//============================================================================
void RandConnectMgr::callbackMemberActive( GroupieId& groupieId, bool isActive )
{
    if( groupieId.getHostType() != eHostTypeRandomConnect )
    {
        return;
    }

    ERandAction randAction = isActive ? eRandActionDeselectUser : eRandActionNone;
    lockMemberList();
    auto iter = std::find_if( m_MemberList.begin(), m_MemberList.end(),
                              [&]( const std::pair<GroupieId, enum ERandAction>& randPair ) { return randPair.first == groupieId; } );
    bool foundMember = iter != m_MemberList.end();
    if( !foundMember )
    {
        m_MemberList.emplace_back( std::make_pair(groupieId, randAction ) );
    }

    unlockMemberList();
    if( !foundMember )
    {
        announceRandConnect( groupieId, randAction );
    }
}
