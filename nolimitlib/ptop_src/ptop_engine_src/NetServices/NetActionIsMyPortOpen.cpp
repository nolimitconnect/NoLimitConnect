//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetActionIsMyPortOpen.h"
#include "NetServicesMgr.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxParse.h>

//============================================================================
NetActionIsMyPortOpen::NetActionIsMyPortOpen( NetServicesMgr& netServicesMgr )
: NetActionBase( netServicesMgr )
{
}

//============================================================================
void NetActionIsMyPortOpen::doAction( void )
{
	std::string myIp = "";
	ENetCmdError eResult = m_NetServicesMgr.doIsMyPortOpen( myIp, false );
	LogModule( eLogIsPortOpenTest, LOG_INFO, "NetActionIsMyPortOpen::doAction result %s ip %s", DescribeNetCmdError( eResult ), myIp.c_str() );
}


