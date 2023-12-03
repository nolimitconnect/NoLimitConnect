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

#include <CoreLib/VxDebug.h>

//============================================================================

//============================================================================
void MembershipAvailable::setMembershipState( EHostType hostType, EMembershipState membership )
{
	switch( hostType )
	{
	case eHostTypeNetwork:
		m_NetworkMembership = membership;
		break;
	case eHostTypeConnectTest:
		m_ConnectTestMembership = membership;
		break;
	case eHostTypeGroup:
		m_GroupMembership = membership;
		break;
	case eHostTypeChatRoom:
		m_ChatRoomMembership = membership;
		break;
	case eHostTypeRandomConnect:
		m_RandomConnectMembership = membership;
		break;
	default:
		vx_assert( false );
	}
}


