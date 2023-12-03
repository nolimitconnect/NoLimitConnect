#ifndef SSLMGR_CMD_SEND_FILE_H
#define SSLMGR_CMD_SEND_FILE_H

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

enum ESslMgrCmdSendFileState
{
	eCmdSendFileStateWaitSendCmd,
	eCmdSendFileStateWaitOkSend,
	eCmdSendFileStateTxFileBytes,

	eCmdSendFileStateConfirmServerOk,

	eCmdSendFileStateSuccess,
	eCmdSendFileStateError
};

class SslMgrCmdSendFile : public SslMgrCmdBase
{
public:
	SslMgrCmdSendFile( SslMgrCmdMgr& cmdMgr );
    ~SslMgrCmdSendFile();

	void						setFileName( const char * fileName );
	std::string&				getFileName( void );

	virtual void				enterState( void );
	virtual void				exitState( void );

	virtual int					getDataToWriteToSslMgrServer( char * buf, int bufLen );
	virtual void				handleDataFromSslMgrServer( char * data, int dataLen );
	virtual delayState_e		handleDelays( void );

protected:
	void						beginFileSend( void );

	int							writeToSslStreamFromFile( char * sslBuf, int bufLen );
	bool						timedOut( void );
	void						doFileReadError( void );


	void						checkForFileComplete( void );
	bool						isFileFinishedUploading( void );
	void						finishFileSend( void );

	//=== vars ===//
	ESslMgrCmdSendFileState	m_eCmdState;
	std::string					m_strFileName;
	uint32_t							m_u32FileLength;
	uint32_t							m_u32FileCrc;

	FILE *						m_hFile;
	uint32_t							m_u32FileOffs;
	std::string					m_strJustFileName;

};

#endif // SSLMGR_CMD_SEND_FILE_H

