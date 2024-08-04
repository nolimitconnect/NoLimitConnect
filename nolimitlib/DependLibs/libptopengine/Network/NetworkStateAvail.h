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

#include "NetworkStateBase.h"

class DirectConnectTester;

class NetworkStateAvail : public NetworkStateBase
{
public:
	NetworkStateAvail( NetworkStateMachine& stateMachine );

	virtual void				enterNetworkState( void );
	void						runNetworkState( void );

protected:
	//=== vars ===//
	DirectConnectTester&		m_DirectConnectTester;

};


