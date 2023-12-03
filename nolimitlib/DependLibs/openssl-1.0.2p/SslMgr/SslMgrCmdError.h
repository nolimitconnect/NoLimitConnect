#ifndef SSLMGR_CMD_ERROR_H
#define SSLMGR_CMD_ERROR_H

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

class SslMgrCmdError : public SslMgrCmdBase
{
public:
	SslMgrCmdError( SslMgrCmdMgr& cmdMgr );

	virtual void				enterState( void );
	virtual bool				getIsCmdComplete( void )				{ return true; }

	virtual void				setErrorText( const char * errorText );
	virtual void				setErrorCode( int errCode )				{ m_ErrorCode = errCode; }
	virtual void				setCmdTypeWithError( ESslMgrCmdType eCmdType );

	//=== vars ===//
	std::string					m_strErrorText;
	ESslMgrCmdType				m_eCmdTypeWithError;
	int							m_ErrorCode;
};

#endif // SSLMGR_CMD_ERROR_H

