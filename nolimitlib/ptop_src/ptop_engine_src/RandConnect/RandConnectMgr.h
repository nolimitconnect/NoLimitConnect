#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <ConnectIdListMgr/OnlineStatusCallback.h>
#include <Membership/MemberActiveCallback.h>

#include <CoreLib/VxMutex.h>

#include <PktLib/GroupieId.h>
#include <PktLib/PktsRandConnectDefs.h>

#include <vector>

class RandConnectCallback;
class P2PEngine;

class RandConnectMgr : public OnlineStatusCallback, public MemberActiveCallback
{
public:
    RandConnectMgr() = default;
    virtual ~RandConnectMgr() = default;

    void                        onEngineStartup( void );
    void                        onEngineShutdown( void );

    void                        wantRandConnectCallbacks( RandConnectCallback* client, bool enable );

    void				        callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline ) override;
    void				        callbackMemberActive( GroupieId& onlineId, bool isActive ) override;

    void                        updateRandConnectStatus( GroupieId& groupieId, VxGUID& toUserOnlineId, enum ERandAction randAction );

protected:

    void                        lockMemberList( void )              { m_MemberListMutex.lock(); }
    void                        unlockMemberList( void )            { m_MemberListMutex.unlock(); }

    void                        lockClientList( void )              { m_MemberClientsMutex.lock(); }
    void                        unlockClientList( void )            { m_MemberClientsMutex.unlock(); }

    void                        updateRandConnectStatus( GroupieId& groupieId, enum ERandAction randAction );

    virtual void                announceRandConnect( GroupieId& groupieId, enum ERandAction randAction );
    
    VxMutex                     m_MemberListMutex;
    std::vector<std::pair<GroupieId, ERandAction>>      m_MemberList;

    VxMutex                     m_MemberClientsMutex;
    std::vector<RandConnectCallback*> m_MemberClients;
};

