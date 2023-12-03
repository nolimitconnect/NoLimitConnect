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

class MembershipAvailableCallbackInterface
{
public:
    virtual void				callbackMembershipAvailableAdded( VxGUID& onlineId, VxGUID& sktConnectId, MembershipAvailable& memberAvail ) {};
    virtual void				callbackMembershipAvailableUpdated( VxGUID& onlineId, VxGUID& sktConnectId, MembershipAvailable& memberAvail ) {};
    virtual void				callbackMembershipAvailableRemoved( VxGUID& onlineId ) {};
};

