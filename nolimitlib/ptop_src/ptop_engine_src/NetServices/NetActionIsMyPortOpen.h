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

#include "NetActionBase.h"

#include <string>

class NetServicesMgr;
class VxSktConnectSimple;

class NetActionIsMyPortOpen : public NetActionBase
{
public:
	NetActionIsMyPortOpen( NetServicesMgr& netServicesMgr );
	virtual ~NetActionIsMyPortOpen() = default;

	virtual ENetActionType		getNetActionType( void )			{ return eNetActionIsPortOpen; }
	virtual void				doAction( void );

protected:
	void						buildIsMyPortOpenUrl( VxSktConnectSimple * netServConn, std::string& strHttpUrl );
};

