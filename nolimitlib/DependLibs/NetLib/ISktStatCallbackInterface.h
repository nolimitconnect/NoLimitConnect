#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include <GuiInterface/IDefs.h>
#include "VxSktUtil.h"

class ISktStatCallbackInterface
{
public:
	virtual void                sktConnected( SOCKET skt ) = 0;
	virtual void                sktConnected2( SOCKET skt, std::string ipAddr ) = 0;
	virtual void                sktConnected3( SOCKET skt, std::string ipAddr, ESktType sktType ) = 0;
	virtual void                sktSetRemoteAddr( SOCKET skt, std::string ipAddr ) = 0;
	virtual void                sktSetType( SOCKET skt, ESktType sktType ) = 0;
	virtual void                sktClosed( SOCKET skt ) = 0;
};