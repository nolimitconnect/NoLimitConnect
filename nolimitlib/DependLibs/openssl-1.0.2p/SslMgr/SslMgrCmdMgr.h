#ifndef SSLMGR_CMD_MGR_H
#define SSLMGR_CMD_MGR_H

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
#include "SslMgrCmdIdle.h"

#include <CoreLib/VxMutex.h>

#include <vector>

class SslWrapper;

class SslMgrCmdMgr
{
public:
	SslMgrCmdMgr( SslWrapper& wrapper );
    
    ~SslMgrCmdMgr();

	SslWrapper&					getSslWrapper( void )					{ return m_SslWrapper; }

	virtual void				LogMsg( unsigned long u32LogMsgType, const char * logMsg, ... );
	
	virtual bool				haveAllSslMgrCmdsCompleted( void );
	
	void						beginSslMgrCmds( void );
	void						updateSslMgrCmds( void );
	void						endSslMgrCmds( void );
	void						handleDelays( void );

	int							getDataToWriteToSslMgrServer( char * buf, int bufSize );
	void						handleDataFromSslMgrServer( char * buf, int dataSize );

	void						addErrorCmd( ESslMgrCmdType eCmdType, int errCode, const char * errMsg );

	virtual void				sslMgrCmdLogOn( const char * userName, const char * pass );
	virtual void				sslMgrCmdLogOff( bool logOffRightNow = false, bool fromSslMgrThread = false );
	virtual void				sslMgrCmdSendFile( const char * userFileName );
	virtual void				sslMgrCmdListFiles( void );
	virtual void				sslMgrCmdGetFile( const char * userFileName, bool deleteFromSslMgrIfSuccess = true );
	virtual void				sslMgrCmdDeleteFile( const char * userFileName, uint32_t u32CheckSum );
	virtual void				sslMgrCmdSendText( const char * textToSendToServer );
	virtual void				deleteAllQuedCommands( void );


	//=== vars ===//
	std::vector<SslMgrCmdBase *>	m_SslMgrCmds;
	SslWrapper&					m_SslWrapper;
	VxMutex						m_CmdMutex;

	SslMgrCmdIdle				m_SslMgrCmdIdle;
	SslMgrCmdBase *				m_SslMgrCurCmd;

};

#endif // SSLMGR_CMD_MGR_H
