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

#include "Membership.h"
#include "MembershipMgrBase.h"

#include <vector>

class MembershipAvailableCallbackInterface;
class MembershipAvailableMgr : public MembershipMgrBase
{
public:
    MembershipAvailableMgr() = delete;
    MembershipAvailableMgr( P2PEngine& engine );
    virtual ~MembershipAvailableMgr() = default;

    void                        addMemberClient( MembershipAvailableCallbackInterface* client, bool enable );

    virtual void                updateMemberAvail( VxGUID& sktConnectId, VxGUID& onlineId, MembershipAvailable& memberAvail );

    virtual void                removeConnection( VxGUID& sktConnectId ) override;
    virtual void                removeIdent( VxGUID& onlineId ) override;
    virtual void                removeAll( void ) override;

    std::vector< std::pair<VxGUID, std::pair<VxGUID, MembershipAvailable>>>& getMemberList()         { return m_MemberList; };

protected:
    virtual void                removeConnection( VxGUID& onlineId, VxGUID& sktConnectId );

    virtual void                onMembershipAdded( VxGUID& onlineId, VxGUID& sktConnectId, MembershipAvailable& memberAvail );
    virtual void                onMembershipUpdated( VxGUID& onlineId, VxGUID& sktConnectId, MembershipAvailable& memberAvail );
    virtual void                onMembershipRemoved( VxGUID& onlineId );

    virtual void                announceMembershipAdded( VxGUID& onlineId, VxGUID& sktConnectId, MembershipAvailable& memberAvail );
    virtual void                announceMembershipUpdated( VxGUID& onlineId, VxGUID& sktConnectId, MembershipAvailable& memberAvail );
    virtual void                announceMembershipRemoved( VxGUID& onlineId );
    

    // online id, skt id, membership
    std::vector< std::pair<VxGUID, std::pair<VxGUID, MembershipAvailable>>> m_MemberList;

    std::vector<MembershipAvailableCallbackInterface*> m_MemberClients;
};

