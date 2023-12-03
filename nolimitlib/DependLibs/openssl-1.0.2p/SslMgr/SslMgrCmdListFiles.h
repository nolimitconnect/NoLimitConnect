#ifndef SSLMGR_CMD_LIST_FILES_H
#define SSLMGR_CMD_LIST_FILES_H

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

enum ESslMgrCmdListState
{
	eCmdListStateWaitSendCmd,
	eCmdListStateSendCmdResponse,
	eCmdListStateWaitFileList,

	eCmdListStateSuccess,
	eCmdListStateError
};

class SslMgrCmdListFiles : public SslMgrCmdBase
{
public:
	SslMgrCmdListFiles( SslMgrCmdMgr& cmdMgr );

	virtual void				updateState( void );

	virtual int					getDataToWriteToSslMgrServer( char * buf, int bufLen );
	virtual void				handleDataFromSslMgrServer( char * data, int dataLen );
	virtual delayState_e		handleDelays( void );

protected:
	bool						timedOut( void );
	bool						checkAccumForFileList( void );

	//=== vars ===//
	ESslMgrCmdListState		m_eCmdState;
};

#endif // SSLMGR_CMD_LIST_FILES_H

