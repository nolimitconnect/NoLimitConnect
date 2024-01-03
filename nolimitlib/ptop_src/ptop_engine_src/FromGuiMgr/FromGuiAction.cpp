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

	case eFromGuiTypeAppStartup:
		return "FromGuiType App Startup";
	
	case eFromGuiTypeSetUserSpecificDir:
		return "FromGuiType Set User Specific Dir";
	
	case eFromGuiTypeSetUserXferDir:
		return "FromGuiType Set User Xfer Dir";
			
	case eFromGuiTypeiUserLoggedOn:
		return "FromGuiType User Logged On";

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
FromGuiStartupDirectoryAction::FromGuiStartupDirectoryAction( P2PEngine& engine, EFromGuiType fromGuiType, std::string& dir1 )
	: FromGuiActionBase( engine, fromGuiType )
{
	m_Dir1 = dir1;
}

//============================================================================	
FromGuiStartupDirectoryAction::FromGuiStartupDirectoryAction( P2PEngine& engine, EFromGuiType fromGuiType, std::string& dir1, std::string& dir2 )
	: FromGuiActionBase( engine, fromGuiType )
{
	m_Dir1 = dir1;
	m_Dir2 = dir2;
}

//============================================================================	
void FromGuiStartupDirectoryAction::executeAction( void )
{
	switch( m_FromGuiType )
	{

	case eFromGuiTypeAppStartup:
		LogModule( eLogStartup, LOG_VERBOSE, "fromGuiAppStartup %s %s", m_Dir1.c_str(), m_Dir2.c_str() );
		m_Engine.fromGuiAppStartup( m_Dir1, m_Dir2, true );
		break;

	case eFromGuiTypeSetUserSpecificDir:
		LogModule( eLogStartup, LOG_VERBOSE, "fromGuiSetUserSpecificDir %s", m_Dir1.c_str() );
		m_Engine.fromGuiSetUserSpecificDir( m_Dir1, true );
		break;

	case eFromGuiTypeSetUserXferDir:
		LogModule( eLogStartup, LOG_VERBOSE, "fromGuiSetUserXferDir %s", m_Dir1.c_str() );
		m_Engine.fromGuiSetUserXferDir( m_Dir1, true );
		break;

	default:
		onGuiActionError();
	}
}

//============================================================================
FromGuiUserLogon::FromGuiUserLogon( P2PEngine& engine, EFromGuiType fromGuiType, VxNetIdent* myIdent )
	: FromGuiActionBase( engine, fromGuiType )
{
	m_MyIdent = myIdent;
}

//============================================================================	
void FromGuiUserLogon::executeAction( void )
{
	m_Engine.fromGuiUserLoggedOn( m_MyIdent, true );
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
