#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <CoreLib/VxThread.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/AppErr.h>

class NetworkStateMachine;
class NetworkMgr;
class NetServicesMgr;
class P2PEngine;

class DirectConnectTestResults
{
public:
	DirectConnectTestResults() = default;

	DirectConnectTestResults( const DirectConnectTestResults& rhs )
	{
		*this = rhs;
	}

	DirectConnectTestResults& operator =( const DirectConnectTestResults& rhs )
	{
		if( this != &rhs )
		{
			m_MyIpAddr			= rhs.m_MyIpAddr;
			m_eNetCmdErr		= rhs.m_eNetCmdErr;
		}
		return *this;
	}

	bool						getCanDirectConnect( void )							{ return ( eNetCmdErrorNone == m_eNetCmdErr ) ? true : false ; }

	ENetCmdError				m_eNetCmdErr{ eNetCmdErrorUnknown };
	std::string					m_MyIpAddr{ "" };
};

class DirectConnectTester
{
public:
	DirectConnectTester( NetworkStateMachine& stateMachine );
	virtual ~DirectConnectTester() = default;
	DirectConnectTester() = delete; // don't allow default constructor
	DirectConnectTester( const DirectConnectTester& ) = delete; // don't allow copy constructor

	DirectConnectTestResults&	getDirectConnectTestResults( void )		{ return m_TestResults; }

	void						testCanDirectConnect( void );

	bool						isDirectConnectTestComplete( void );
	bool						isTestResultCanDirectConnect( void );

	void						myPortOpenCallback( ENetCmdError eAppErr, std::string& myExternalIp );
protected:

	//=== vars ===//
	NetworkStateMachine&		m_NetworkStateMachine;
	P2PEngine&					m_Engine;
	NetworkMgr&					m_NetworkMgr;
	NetServicesMgr&				m_NetServicesMgr;

	bool						m_bTestIsRunning{ false };
	DirectConnectTestResults	m_TestResults;
};


