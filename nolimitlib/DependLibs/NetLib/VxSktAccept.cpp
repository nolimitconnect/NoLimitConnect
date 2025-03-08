//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxSktAccept.h"
#include "VxServerMgr.h"
#include "VxSktBaseMgr.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxSktUtil.h>

#if !defined(TARGET_OS_WINDOWS)
#include <sys/signal.h>
#endif // !defined(TARGET_OS_WINDOWS)

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

#if !defined(TARGET_OS_WINDOWS)
    signal(SIGPIPE, SIG_IGN);
#endif // !defined(TARGET_OS_WINDOWS)

	m_strLclIp = m_LclIp.toString();
	m_strRmtIp = m_RmtIp.toString();

    LogModule( eLogConnect, LOG_INFO, "VxSktAccept accept Lcl ip %s port %d (0x%4.4x) Rmt ip %s port %d (0x%4.4x) thread 0x%x", 
		m_strLclIp.c_str(),
		m_LclIp.getPort(),
		m_LclIp.getPort(),
		m_strRmtIp.c_str(),
		m_RmtIp.getPort(),
		m_RmtIp.getPort(), VxGetCurrentThreadId() );

	if( VxGetSktStatCallback() )
	{
		VxGetSktStatCallback()->sktConnected4( m_Socket, m_strRmtIp, eSktTypeTcpAccept, eConnectReasonUnknown );
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

