#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxMutex.h>

class VxGUID;
class P2PEngine;
class MembershipMgrBase
{
public:
    MembershipMgrBase( P2PEngine& engine );
    virtual ~MembershipMgrBase() = default;

    void                        lockList( void ) { m_ListMutex.lock(); }
    void                        unlockList( void ) { m_ListMutex.unlock(); }

    void						lockClientList( void ) { m_ClientMutex.lock(); }
    void						unlockClientList( void ) { m_ClientMutex.unlock(); }

    virtual void                removeConnection( VxGUID& sktConnectId ) = 0;
    virtual void                removeIdent( VxGUID& onlineId ) = 0;
    virtual void                removeAll( void ) = 0;

protected:
    virtual void                onUpdateIdent( VxGUID& onlineId, int64_t timestamp );
    virtual void                onRemoveIdent( VxGUID& onlineId );

    P2PEngine&                  m_Engine;
    VxMutex                     m_ListMutex;
    VxMutex                     m_ClientMutex;
};

