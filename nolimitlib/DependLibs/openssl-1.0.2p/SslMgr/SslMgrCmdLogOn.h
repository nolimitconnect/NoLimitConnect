#ifndef SSLMGR_CMD_LOG_ON_H
#define SSLMGR_CMD_LOG_ON_H

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

enum ESvrLoginState
{
	eSvrLoginStateWaitGetUser,
	eSvrLoginStateGetUserResponse,
	eSvrLoginStateWaitGetPass,
	eSvrLoginStateGetPassResponse,
	eSvrLoginStateWaitGetInfo,
	eSvrLoginStateGetInfoResponse,
	eSvrLoginStateSuccess,
	eSvrLoginStateError
};

class SslMgrCmdLogOn : public SslMgrCmdBase
{
public:
	SslMgrCmdLogOn( SslMgrCmdMgr& cmdMgr );

	virtual void				enterState( void );
	virtual int					getDataToWriteToSslMgrServer( char * buf, int bufLen );
	virtual void				handleDataFromSslMgrServer( char * data, int dataLen );
	virtual delayState_e		handleDelays( void );

	void						setUserName( const char * userName );
	void						setPassword( const char * pass );

protected:
	bool						timedOut( void );
	bool						accumulatedOkPlusAssigned( void );
	bool						accumulatedOkPlusUnassigned( void );

	//=== vars ===//
	std::string					m_strUserName;
	std::string					m_strPassword;
	ESvrLoginState				m_eSvrLoginState;
};

#endif // SSLMGR_CMD_LOG_ON_H

