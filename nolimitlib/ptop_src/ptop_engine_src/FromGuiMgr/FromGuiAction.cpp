//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FromGuiAction.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

//============================================================================
FromGuiActionBase::FromGuiActionBase( P2PEngine& engine, EFromGuiType fromGuiType )
	: m_Engine( engine )
	, m_FromGuiType( fromGuiType )
{
}

//============================================================================	
void FromGuiActionBase::onGuiActionError( void )
{
	LogMsg( LOG_ERROR, "FromGuiActionBase::onUnknownActionError unknown action %d", (int)m_FromGuiType );
}

//============================================================================	
std::string FromGuiActionBase::describeGuiAction( void )
{
	switch( m_FromGuiType )
	{
	case eFromGuiTypeNone:
		return "FromGuiType None";

	case eFromGuiAnnounceHost:
		return "FromGuiType AnnounceHost";

	case eFromGuiJoinHost:
		return "FromGuiType JoinHost";

	case eFromGuiLeaveHost:
		return "FromGuiType LeaveHost";

	case eFromGuiUnJoinHost:
		return "FromGuiType UnJoinHost";

	case eFromGuiJoinLastJoinedHost:
		return "FromGuiType JoinLastJoinedHost";

	case eFromGuiSearchHost:
		return "FromGuiType SearchHost";

	default:
		return "FromGuiType Unknown";
	}
}

//============================================================================
FromGuiHostAction::FromGuiHostAction( P2PEngine& engine, EFromGuiType fromGuiType, HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 )
	: FromGuiActionBase( engine, fromGuiType )
	, m_AdminId( adminId )
	, m_SessionId( sessionId )
	, m_HostUrlIpv4( hostUrlIpv4 )
	, m_HostUrlIpv6( hostUrlIpv6 )
{
}

//============================================================================	
void FromGuiHostAction::executeAction( void )
{
	switch( m_FromGuiType )
	{

	case eFromGuiAnnounceHost:
		m_Engine.fromGuiAnnounceHost( m_AdminId, m_SessionId, m_HostUrlIpv4, m_HostUrlIpv6, true );
		break;

	case eFromGuiJoinHost:
		m_Engine.fromGuiJoinHost( m_AdminId, m_SessionId, m_HostUrlIpv4, m_HostUrlIpv6, true );
		break;

	case eFromGuiLeaveHost:
		m_Engine.fromGuiLeaveHost( m_AdminId, m_SessionId, m_HostUrlIpv4, m_HostUrlIpv6, true );
		break;

	case eFromGuiUnJoinHost:
		m_Engine.fromGuiUnJoinHost( m_AdminId, m_SessionId, m_HostUrlIpv4, m_HostUrlIpv6, true );
		break;

	default:
		onGuiActionError();
	}
}

//============================================================================	
FromGuiJoinLastHostAction::FromGuiJoinLastHostAction( P2PEngine& engine, EFromGuiType fromGuiType, HostedId& adminId, VxGUID& sessionId )
	: FromGuiActionBase( engine, fromGuiType )
	, m_AdminId( adminId )
	, m_SessionId( sessionId )
{
}

//============================================================================	
void FromGuiJoinLastHostAction::executeAction( void )
{
	m_Engine.fromGuiJoinLastJoinedHost( m_AdminId, m_SessionId, true );
}

//============================================================================
FromGuiSearchHostAction::FromGuiSearchHostAction( P2PEngine& engine, EFromGuiType fromGuiType, EHostType hostType, SearchParams& searchParams, bool enable )
	: FromGuiActionBase( engine, fromGuiType )
	, m_HostType( hostType )
	, m_SearchParams( searchParams )
	, m_Enable( enable )
{
}

//============================================================================	
void FromGuiSearchHostAction::executeAction( void )
{
	m_Engine.fromGuiSearchHost( m_HostType, m_SearchParams,  m_Enable, true );
}
