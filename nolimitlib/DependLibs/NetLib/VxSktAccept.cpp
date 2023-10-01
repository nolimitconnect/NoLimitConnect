//============================================================================
// Copyright (C) 2009 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxSktAccept.h"
#include "VxServerMgr.h"
#include "VxSktBaseMgr.h"
#include "VxSktUtil.h"

#include <CoreLib/VxParse.h>

//============================================================================
VxSktAccept::VxSktAccept()
{
	m_eSktType = eSktTypeTcpAccept;
}

//============================================================================
//! called when socket is accepted
RCODE VxSktAccept::doAccept( VxServerMgr * poMgr, struct sockaddr& oAcceptAddr )
{
	m_SktMgr = poMgr;
	m_eSktCallbackReason = eSktCallbackReasonConnecting;
	// get the peer address
    RCODE rc = VxGetRmtAddress( m_Socket, m_RmtIp );
    if( !rc )
    {
        rc = VxGetLclAddress( m_Socket, m_LclIp );
    }

    if( rc )
    {
        VxCloseSktNow( m_Socket );
        m_Socket = INVALID_SOCKET;
        return rc;
    }

	m_strLclIp = m_LclIp.toStdString();
	m_strRmtIp = m_RmtIp.toStdString();

    LogModule( eLogConnect, LOG_INFO, "VxSktAccept accept Lcl ip %s port %d (0x%4.4x) Rmt ip %s port %d (0x%4.4x) thread 0x%x", 
		m_strLclIp.c_str(),
		m_LclIp.getPort(),
		m_LclIp.getPort(),
		m_strRmtIp.c_str(),
		m_RmtIp.getPort(),
		m_RmtIp.getPort(), VxGetCurrentThreadId() );

	if( VxGetSktStatCallback() )
	{
		VxGetSktStatCallback()->sktConnected3( m_Socket, m_strRmtIp, eSktTypeTcpAccept );
	}

    // tell user we connecting
	//VerifyCodePtr( m_pfnReceive );
	m_pfnReceive( m_ThisSkt, getRxCallbackUserData() );
	// make a useful thread name
	std::string strVxThreadName;
	StdStringFormat( strVxThreadName, "VxSktBaseAccept%d", m_SktNumber );
    LogModule( eLogConnect, LOG_INFO, "VxSktAccept starting rx thread %s for skt %d skt id %d", strVxThreadName.c_str(), m_Socket, m_SktNumber );
	startReceiveThread( strVxThreadName.c_str() );
	return 0;
}

