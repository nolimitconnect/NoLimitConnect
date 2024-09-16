#pragma once
//============================================================================
// Copyright (C) 2003 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InetAddress.h"

#include <string>

bool VxResolveHostToIp(	const char* pUrl,			// web name to resolve
						std::string & strRetIp,		// return dotted string 
						uint16_t& u16RetPort );			// return port

bool VxResolveHostToIp(	const char* pHostOnly,			//web host name to resolve
					    uint16_t u16Port,
						InetAddress& oRetIp );

bool VxResolveHostToIps(	const char* pHostOnly,			//web host name to resolve
							uint16_t u16Port,
							std::vector<InetAddress>& aoRetIps );


