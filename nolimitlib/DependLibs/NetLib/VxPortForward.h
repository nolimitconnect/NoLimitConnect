#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <stdint.h>
#include <string>
#include <vector>

class VxPortForward
{
public:
	VxPortForward() = default;
	~VxPortForward() = default;

	static void setEnablePortForward( bool enable );
	static bool getEnablePortForward( void );


	static bool addPortForward( bool ipv6, std::string ipAddr, uint16_t externPort );
	static bool removePortForward( bool ipv6, uint16_t port );

	static bool listPortForward( bool ipv6 = false );


	static bool m_ForwardEnable;
};
