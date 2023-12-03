//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MembershipMgrBase.h"
#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
MembershipMgrBase::MembershipMgrBase( P2PEngine& engine )
    : m_Engine( engine )
{
}

//============================================================================
void MembershipMgrBase::onUpdateIdent( VxGUID& onlineId, int64_t timestamp )
{
}

//============================================================================
void MembershipMgrBase::onRemoveIdent( VxGUID& onlineId )
{
}