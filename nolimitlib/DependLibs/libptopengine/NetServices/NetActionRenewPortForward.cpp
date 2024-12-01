//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetActionRenewPortForward.h"
#include "NetServicesMgr.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxParse.h>

//============================================================================
NetActionRenewPortForward::NetActionRenewPortForward( NetServicesMgr& netServicesMgr )
: NetActionBase( netServicesMgr )
{
}

//============================================================================
void NetActionRenewPortForward::doAction( void )
{
	ENetCmdError eResult = m_NetServicesMgr.doRenewPortForward();
	if( eResult != eNetCmdErrorNone )
	{
		LogModule( eLogNetService, LOG_INFO, "NetActionRenewPortForward::doAction result %s ", DescribeNetCmdError( eResult ) );
	}
	else
	{
		LogMsg( LOG_DEBUG, "NetActionRenewPortForward SUCCESS" );
	}
}


