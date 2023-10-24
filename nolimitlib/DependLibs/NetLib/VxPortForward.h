#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

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