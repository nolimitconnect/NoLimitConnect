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

class NetworkStateWaitForRelay : public NetworkStateBase
{
public:
	NetworkStateWaitForRelay( NetworkStateMachine& stateMachine );

	virtual void				enterNetworkState( void );
	void						runNetworkState( void );
};

