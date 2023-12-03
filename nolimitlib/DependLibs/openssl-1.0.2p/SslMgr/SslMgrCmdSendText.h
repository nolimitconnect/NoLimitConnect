#ifndef SSLMGR_CMD_SEND_TEXT_H
#define SSLMGR_CMD_SEND_TEXT_H

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

enum ESslMgrCmdSendTextState
{
	eCmdSendTextStateWaitSendTextCmd,
	eCmdSendTextStateSendTextResponse,

	eCmdSendTextStateGetListSuccess,
	eCmdSendTextStateGetListError
};

class SslMgrCmdSendText : public SslMgrCmdBase
{
public:
	SslMgrCmdSendText( SslMgrCmdMgr& cmdMgr );

	void						setCmdText( const char * text );
	std::string&				getCmdText( void );

	virtual int					getDataToWriteToSslMgrServer( char * buf, int bufLen );
	virtual void				handleDataFromSslMgrServer( char * data, int dataLen );

protected:
	bool						timedOut( void );

	//=== vars ===//
	ESslMgrCmdSendTextState	m_eCmdState;
	std::string					m_strCmdText;
};

#endif // SSLMGR_CMD_SEND_TEXT_H

