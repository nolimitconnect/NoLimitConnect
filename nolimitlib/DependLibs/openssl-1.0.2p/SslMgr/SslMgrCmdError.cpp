//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SslMgrCmdError.h"
#include "SslWrapper.h"

//============================================================================
SslMgrCmdError::SslMgrCmdError(SslMgrCmdMgr& cmdMgr )
: SslMgrCmdBase( cmdMgr )
{
	setCmdType( eSslMgrCmdIdle );
}

//============================================================================
void SslMgrCmdError::enterState( void )
{
	if( m_strErrorText.length() )
	{
		m_SslWrapper.getSslCallback().sslMgrCmdError( m_eCmdTypeWithError, m_ErrorCode, m_strErrorText.c_str() );
	}
}

//============================================================================
void SslMgrCmdError::setErrorText( const char * errorText )
{
	m_strErrorText = errorText;
}

//============================================================================
void SslMgrCmdError::setCmdTypeWithError( ESslMgrCmdType eCmdType )
{
	m_eCmdTypeWithError = eCmdType;
}
