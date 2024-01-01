//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MemberActiveMgr.h"

#include "MemberActiveCallback.h"
#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
MemberActiveMgr::MemberActiveMgr( P2PEngine& engine )
    : m_Engine( engine )
{
}

//============================================================================
void MemberActiveMgr::updateMemberActive( GroupieId& groupieId, bool memberActive )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "MemberActiveMgr::updateSktIdent invalid skt id" );
        return;
    }

    bool wasUpdated = false;
    lockMemberList();

    bool found{ false };
    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); ++iter )
    {
        if( *iter == groupieId )
        {
            found = true;
            if( !memberActive )
            {
                m_MemberList.erase( iter );
                wasUpdated = true;
            }

            break;
        }
    }

    if( memberActive && !found )
    {
        m_MemberList.emplace_back( groupieId );
        wasUpdated = true;
    }

    unlockMemberList();

    if( wasUpdated )
    {
        if( groupieId.getUserOnlineId() == m_Engine.getMyOnlineId() )
        {
            if( memberActive )
            {
                LogModule( eLogHosts, LOG_VERBOSE, "MemberActiveMgr myself member active with host %s", m_Engine.describeHostedId( groupieId.getHostedId() ).c_str() );
            }
            else
            {
                LogModule( eLogHosts, LOG_VERBOSE, "MemberActiveMgr myself member NOT active with host %s", m_Engine.describeHostedId( groupieId.getHostedId() ).c_str() );
            }

            announceMemberActive( groupieId, memberActive );
        }
    }
}

//============================================================================
bool MemberActiveMgr::isMemberActive( GroupieId& groupieId )
{
    lockMemberList();
    bool isActive = std::find( m_MemberList.begin(), m_MemberList.end(), groupieId ) != m_MemberList.end();
    unlockMemberList();

    return isActive;
}

//============================================================================
void MemberActiveMgr::announceMemberActive( GroupieId& groupieId, bool memberActive )
{
    LogModule( eLogMembership, LOG_INFO, "MemberActiveMgr::announceMemberActive %d %s", memberActive, m_Engine.describeGroupieId( groupieId ).c_str() );
    lockClientList();

    for( auto client : m_MemberClients )
    {
        client->callbackMemberActive( groupieId, memberActive );
    }

    unlockClientList();
}

//============================================================================
void MemberActiveMgr::wantMemberActiveCallbacks( MemberActiveCallback* client, bool enable )
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
                LogMsg( LOG_ERROR, "MemberActiveMgr::wantMemberActiveCallbacks ignored because already in list" );
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
void MemberActiveMgr::callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    if( isOnline )
    {
        return;
    }
    
    std::vector<GroupieId> offlineMemberList;
    lockMemberList();

    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); )
    {
        if( iter->getHostOnlineId() == onlineId  || iter->getUserOnlineId() == onlineId)
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

    for( auto& groupieId : offlineMemberList )
    {
        announceMemberActive( groupieId, false );
    }
}