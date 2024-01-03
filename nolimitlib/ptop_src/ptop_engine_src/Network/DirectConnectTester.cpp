//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "DirectConnectTester.h"
#include "NetworkStateMachine.h"

#include <P2PEngine/P2PEngine.h>
#include <NetServices/NetServicesMgr.h>

namespace
{
	void MyPortOpenCallback( void * userData, ENetCmdError eAppErr, std::string& myExternalIp )
	{
		if( userData )
		{
			((DirectConnectTester *)userData)->myPortOpenCallback( eAppErr, myExternalIp );
		}
	}
}

//============================================================================
DirectConnectTester::DirectConnectTester( NetworkStateMachine& stateMachine )
: m_NetworkStateMachine( stateMachine )
, m_Engine( stateMachine.getEngine() )
, m_NetworkMgr( stateMachine.getNetworkMgr() )
, m_NetServicesMgr( stateMachine.getEngine().getNetServicesMgr() )
, m_bTestIsRunning( false )
{
}

//============================================================================
void DirectConnectTester::testCanDirectConnect( void )
{
	while( m_bTestIsRunning )
	{
		LogMsg( LOG_INFO, "DirectConnectTester::testCanDirectConnect: waiting for prev test to complete" );
		VxSleep( 400 );
	}

	m_bTestIsRunning = true;
	m_TestResults.m_eNetCmdErr = eNetCmdErrorNone;

	m_NetServicesMgr.setMyPortOpenResultCallback( MyPortOpenCallback, this );

	m_NetServicesMgr.addNetActionIsMyPortOpenToQueue();
}

//============================================================================
bool DirectConnectTester::isDirectConnectTestComplete( void )
{
	return !m_bTestIsRunning;
}

//============================================================================
bool DirectConnectTester::isTestResultCanDirectConnect( void )
{
	return m_TestResults.getCanDirectConnect();
}

//============================================================================
void DirectConnectTester::myPortOpenCallback( ENetCmdError eAppErr, std::string& myExternalIp )
{
	m_TestResults.m_eNetCmdErr = eAppErr;
	m_TestResults.m_MyIpAddr = myExternalIp;

	m_bTestIsRunning = false;
}