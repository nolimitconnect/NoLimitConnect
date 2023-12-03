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
#include <CoreLib/VxGUID.h>

class MembershipAvailable
{
public:
    MembershipAvailable() = default;

	void						setTimestamp( uint64_t timestamp )		{ m_TimestampMs = timestamp; }
	uint64_t					getTimestamp( void )					{ return m_TimestampMs; }

    void                        setCanPushToTalk( bool canPtt )			{ m_CanPushToTalk = canPtt; }
	bool                        getCanPushToTalk( void )				{ return m_CanPushToTalk; }

    void                        setMembershipState( EHostType hostType, EMembershipState membership );

protected:
	uint64_t					m_TimestampMs{ 0 };
    bool                        m_CanPushToTalk{ false };
	EMembershipState			m_NetworkMembership{ eMembershipStateNone };
	EMembershipState			m_ConnectTestMembership{ eMembershipStateNone };
	EMembershipState			m_GroupMembership{ eMembershipStateNone };
	EMembershipState			m_ChatRoomMembership{ eMembershipStateNone };
	EMembershipState			m_RandomConnectMembership{ eMembershipStateNone };
};

class MembershipHosted
{
public:
	uint64_t					m_TimestampMs{ 0 };
	EMembershipState			m_MembershipHosted{ eMembershipStateNone };
	EHostType					m_HostType{ eHostTypeUnknown };
};
