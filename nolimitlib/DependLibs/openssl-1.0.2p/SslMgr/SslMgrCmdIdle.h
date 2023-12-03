#ifndef SSLMGR_CMD_IDLE_H
#define SSLMGR_CMD_IDLE_H

//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SslMgrCmdBase.h"

class SslMgrCmdIdle : public SslMgrCmdBase
{
public:
	SslMgrCmdIdle( SslMgrCmdMgr& cmdMgr );

	virtual bool				getIsCmdComplete( void )				{ return true; }
};

#endif // SSLMGR_CMD_IDLE_H

