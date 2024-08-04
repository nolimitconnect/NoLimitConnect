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

#include "NetworkEventBase.h"

#include <string>

class NetworkEventAvail : public NetworkEventBase
{
public:
	NetworkEventAvail( NetworkStateMachine& stateMachine, const char* lclIp, bool isCellularNetwork );

	const char*				getLclIp( void )							{ return m_LclIp.c_str();  }
	bool						getIsCellNetwork( void )					{ return m_bIsCellNetwork; }

	virtual void				runNetworkEvent( void );

protected:
	//=== vars ===//
	std::string					m_LclIp; 
	bool						m_bIsCellNetwork;
};


