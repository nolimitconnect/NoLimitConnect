#ifndef SSLMGR_CMD_DELETE_FILE_H
#define SSLMGR_CMD_DELETE_FILE_H

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

enum ESslMgrCmdDeleteFileState
{
	eCmdDeleteFileStateWaitSendCmd,
	eCmdDeleteFileStateSendCmdResponse,

	eCmdDeleteFileStateSuccess,
	eCmdDeleteFileStateError
};

class SslMgrCmdDeleteFile : public SslMgrCmdBase
{
public:
	SslMgrCmdDeleteFile( SslMgrCmdMgr& cmdMgr, const char * fileName, uint32_t u32CheckSum );

	std::string&				getFileName( void );

	virtual void				enterState( void );
	virtual void				updateState( void );

	virtual int					getDataToWriteToSslMgrServer( char * buf, int bufLen );
	virtual void				handleDataFromSslMgrServer( char * data, int dataLen );
	virtual delayState_e		handleDelays( void );

protected:
	bool						timedOut( void );

	//=== vars ===//
	ESslMgrCmdDeleteFileState	m_eCmdState;
	std::string					m_strFileName;
	uint32_t							m_u32FileCrc;
	std::string					m_strDeleteFileCmd;

};

#endif // SSLMGR_CMD_DELETE_FILE_H

