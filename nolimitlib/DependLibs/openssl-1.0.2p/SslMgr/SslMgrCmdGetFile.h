#ifndef SSLMGR_CMD_GET_FILE_H
#define SSLMGR_CMD_GET_FILE_H

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

#include <CoreLib/VxDefs.h>


enum ESslMgrCmdGetFileState
{
	eCmdGetFileStateWaitSendCmd,
	eCmdGetFileStateWaitOk,
	eCmdGetFileStateWaitFileLen,
	eCmdGetFileStateWaitCheckSum,
	eCmdGetFileStateRxFileBytes,

	eCmdGetFileStateSuccess,
	eCmdGetFileStateError
};

class SslMgrCmdGetFile : public SslMgrCmdBase
{
public:
	SslMgrCmdGetFile( SslMgrCmdMgr& cmdMgr, bool deleteFromSslMgrIfSuccess );

	void						setFileName( const char * fileName );
	std::string&				getFileName( void );
	std::string&				getDownloadCompleteFileName( void );
	uint32_t							getFileCheckSum( void );

	virtual void				enterState( void );
	virtual void				exitState( void );

	virtual int					getDataToWriteToSslMgrServer( char * buf, int bufLen );
	virtual void				handleDataFromSslMgrServer( char * data, int dataLen );
	virtual delayState_e		handleDelays( void );
	bool						deleteFromSslMgrIfSuccess( void )			{ return m_bDeleteFromSslMgrIfSuccess; }

protected:
	bool						writeSslStreamToFile( char * data, int dataLen );
	bool						timedOut( void );
	void						doFileWriteError( void );
	void						checkForFileLen( void );
	void						checkForCheckSum( void );
	void						startFileReceive( void );
	void						checkForFileComplete( void );
	bool						isFileFinishedDownloading( void );
	void						finishFileReceive( void );
	bool						verifyChecksum( void );

	//=== vars ===//
	ESslMgrCmdGetFileState		m_eCmdState;
	std::string					m_strFileName;

	uint32_t							m_u32FileLength;
	uint32_t							m_u32FileCrc;
	std::string					m_strDownloadFileName;
	FILE *						m_hFile;
	uint32_t							m_u32FileOffs;
	bool						m_bDeleteFromSslMgrIfSuccess;
};

#endif // SSLMGR_CMD_GET_FILE_H

