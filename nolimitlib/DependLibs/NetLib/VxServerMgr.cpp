//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxSktUtil.h>
#include "VxSktAccept.h"
#include "VxServerMgr.h"
#include <P2PEngine/P2PEngine.h>

#include "VxPortForward.h"

#include <CoreLib/VxParse.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/AppErr.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxThread.h>

#include <stdio.h>
#include <memory.h>
#include <time.h>

#if defined(TARGET_OS_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif // defined(TARGET_OS_WINDOWS)

//#define DEBUG_VXSERVER_MGR
#define DISABLE_WATCHDOG 1

int VxServerMgr::m_iAcceptMgrCnt = 0;				// number of accept managers that have been created

//============================================================================
namespace
{
    const int INACTIVE_LISTEN_RESTART_MS = 15 * 60 * 1000; // if no listen activity for 15 minutes reopen listen socke

    void * VxServerMgrVxThreadFunc( void * pvContext )
	{
        if( pvContext )
        {
            VxThread* vxThread = (VxThread*)pvContext;
            vxThread->setIsThreadRunning( true );
            std::string threadName = vxThread->getThreadName();
            if( !threadName.empty() )
            {
                bool ipv6 = threadName.find( "IPv6" ) != std::string::npos;
                VxServerMgr* poMgr = (VxServerMgr*)vxThread->getThreadUserParam();
                if( poMgr )
                {
                    LogModule( eLogAcceptConn, LOG_INFO, "#### VxServerMgr: Mgr id %d Listen port %d thread started thread 0x%x", poMgr->m_iMgrId, poMgr->getListenPort(), VxGetCurrentThreadId() );
                    poMgr->listenForConnectionsToAccept( ipv6, vxThread );
                    // quitting
                    LogModule( eLogAcceptConn, LOG_INFO, "#### VxServerMgr: Mgr id %d Listen port %d thread 0x%x tid %d quiting", poMgr->m_iMgrId, poMgr->getListenPort(), VxGetCurrentThreadId(), vxThread->getThreadTid() );
                }
            }
            else
            {
                 LogMsg( LOG_ERROR, "#### thread  thread 0x%x tid %d has empty thread name", VxGetCurrentThreadId(), vxThread->getThreadTid() );
            }

            //! VxThread calls this just before exit
            vxThread->threadAboutToExit();
        }
        return nullptr;
	}
}

//============================================================================
VxServerMgr::VxServerMgr()
: VxSktBaseMgr()
{
#if defined(TARGET_OS_ANDROID)
    m_IsAndroidOs = true;
#endif // defined(TARGET_OS_ANDROID)

	m_iAcceptMgrCnt++;
	m_iMgrId = m_iAcceptMgrCnt;
	m_eSktMgrType = eSktMgrTypeTcpAccept;
}

//============================================================================
void VxServerMgr::sktMgrStartup( void )
{
	startListeningThreads();
}

//============================================================================
void VxServerMgr::sktMgrShutdown( void )
{
	stopListeningThreads();
	VxSktBaseMgr::sktMgrShutdown();
}

//============================================================================
RCODE VxServerMgr::startListeningThreads( void )
{
    std::string ipv4ThreadName;
    StdStringFormat( ipv4ThreadName, "VxServerMgr%dIPv4", m_iMgrId );
    RCODE rc = m_ListenThreadIpv4.startThread( (VX_THREAD_FUNCTION_T)VxServerMgrVxThreadFunc, this, ipv4ThreadName.c_str() );
#if ENABLE_IPV6
    if( 0 == rc )
    {
        std::string ipv6ThreadName;
        StdStringFormat( ipv4ThreadName, "VxServerMgr%dIPv6", m_iMgrId );
        rc = m_ListenThreadIpv4.startThread( (VX_THREAD_FUNCTION_T)VxServerMgrVxThreadFunc, this, ipv4ThreadName.c_str() );
    }
#endif // ENABLE_IPV6
    return rc;
}

//============================================================================
void VxServerMgr::stopListeningThreads( void )
{
    setIsReadyToAcceptConnections( false, false );
    m_ListenThreadIpv4.abortThreadRun( true );
    listenSettingsUpdated( false, true );
    setIsReadyToAcceptConnections( false , false );
#if ENABLE_IPV6
    m_ListenThreadIpv4.abortThreadRun( true );
    listenSettingsUpdated( true, true );
    setIsReadyToAcceptConnections( true, false );
#endif // ENABLE_IPV6
     
    if( m_ListenThreadIpv4.isThreadRunning() )
    {
        m_ListenThreadIpv4.killThread();
        SOCKET skt = getListenSkt( false );
        if( INVALID_SOCKET != skt )
        {
            ::VxCloseSktNow( skt );
        }
    }

#if ENABLE_IPV6

    if( m_ListenThreadIpv6.isThreadRunning() )
    {
        m_ListenThreadIpv6.killThread();
        SOCKET skt = getListenSkt( true );
        if( INVALID_SOCKET != skt )
        {
            ::VxCloseSktNow( skt );
        }
    }
#endif // ENABLE_IPV6
}

//============================================================================
std::shared_ptr<VxSktBase> VxServerMgr::makeNewAcceptSkt( bool ipv6 )				
{ 
    std::shared_ptr<VxSktBase> sharedSkt( new VxSktAccept() );
    sharedSkt->setIsIpv6Connection( ipv6 );
	sharedSkt->setThisSkt( sharedSkt ); // so skt can do callbacks without look up in manager
	return sharedSkt;
}

//============================================================================
bool VxServerMgr::isListening( bool ipv6 )							
{ 
    if( !m_u16ListenPort || !getListenEnable( ipv6 ) )
    {
        return false;
    }

    return ipv6 ? m_ListenThreadIpv6.isThreadRunning() : m_ListenThreadIpv4.isThreadRunning();
}

//============================================================================
RCODE VxServerMgr::acceptConnection( bool ipv6, VxThread* poVxThread, SOCKET oListenSkt )
{
    if( VxIsAppShuttingDown() )
    {
        return -1;
    }

	RCODE rc = 0;
	if( INVALID_SOCKET == oListenSkt )
	{
        LogModule( eLogAcceptConn, LOG_ERROR, "VxServerMgr::acceptConnection INVALID LISTEN SOCKET thread 0x%x", VxGetCurrentThreadId() );
		return -2;
	}

	if( shouldListenAbort( ipv6 ) || !getListenEnable(ipv6) || getIsListenParamsChanged(ipv6) )
	{
        LogModule( eLogAcceptConn, LOG_ERROR, "VxServerMgr::acceptConnection aborted accept thread 0x%x", VxGetCurrentThreadId() );
        return -3;
	}

    static int dumpAcceptCnt = 0;
    dumpAcceptCnt++;
    if( dumpAcceptCnt > 20 )
    {
        dumpAcceptCnt = 0;
        LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr: start acceptConnection skt %d rc %d thread 0x%x", oListenSkt, VxGetLastError(), VxGetCurrentThreadId() );
    }

	// perform accept
	// setup address
    struct sockaddr_storage acceptAddr;
    socklen_t acceptAddrLen = VxSktAddrInit( ipv6, acceptAddr );

    // NOTE: in android the return to blocking on listen doesn't work so we just set it once before start listening so accept does not get hung
    SOCKET oAcceptSkt = accept( oListenSkt, (sockaddr *) &acceptAddr, &acceptAddrLen);

    if( shouldListenAbort( ipv6 ) || !getListenEnable(ipv6) || getIsListenParamsChanged(ipv6) )
	{
        LogModule( eLogAcceptConn, LOG_ERROR, "VxServerMgr::acceptConnection aborted accept2 thread 0x%x", VxGetCurrentThreadId() );
        return -4;
	}

static int acceptErrCnt = 0;
static int dumpSktStatsCnt = 0;
    if( INVALID_SOCKET == oAcceptSkt )
    {
		rc = VxGetLastError();
#if defined(TARGET_OS_WINDOWS)
        if( rc == WSAEWOULDBLOCK )
        {
            rc = EAGAIN;
        }
#endif // defined(TARGET_OS_WINDOWS)

        if( rc )
        {
            acceptErrCnt++;
            if( acceptErrCnt > 300 )
            {
                acceptErrCnt = 0;
                dumpSktStatsCnt++;
                if( rc != EAGAIN )
                {
                    LogModule( eLogAcceptConn, LOG_DEBUG, "VxServerMgr::acceptConnection: listen port %d skt %d error %d thread 0x%x", m_u16ListenPort, oListenSkt, rc, VxGetCurrentThreadId() );
                }

                if( dumpSktStatsCnt > 10 )
                {
                    dumpSktStatsCnt = 0;
                    dumpSocketStats("full dump", true);
                }
                else
                {
                    dumpSocketStats();
                }
            }
        }
        else
        {
            acceptErrCnt = 0;
        }

		if( 0 == rc )
		{
			// not sure how it happens but seems to get in a loop where the clear doesn't clear and there is no error
			// so sleep just in case so doesn't eat up all the CPU
            LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr: no rc acceptConnection skt %d rc %d thread 0x%x", oListenSkt, VxGetLastError(), VxGetCurrentThreadId() );
			VxSleep( 500 );
			return -1;
		}
        else if( EAGAIN == rc )
        {
            // windows non blocking operation could not be done immediate error
            VxSleep( 200 );
            static int intRetry1Cnt = 0;
            intRetry1Cnt++;
            if( intRetry1Cnt > 20 )
            {
                intRetry1Cnt = 0;
                LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr: non blocking operation  acceptConnection skt %d rc %d thread 0x%x", oListenSkt, VxGetLastError(), VxGetCurrentThreadId() );
            }

            return 0;
        }
 		else
		{
            LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr: other error acceptConnection skt %d rc %d thread 0x%x", oListenSkt, VxGetLastError(), VxGetCurrentThreadId() );
			VxSleep( 200 );
			return rc;
		}
    }
    else
    {
        acceptErrCnt = 0;
    }

    LogModule( eLogAcceptConn, LOG_DEBUG, "VxServerMgr::acceptConnection: listen skt %d accepted skt %d thread 0x%x", oListenSkt, oAcceptSkt, VxGetCurrentThreadId() );
	if( poVxThread->isAborted() || VxIsAppShuttingDown() ) 
	{
		return -1;
	}

	// valid accept socket
	if( m_aoSkts.size() >= m_u32MaxConnections )
	{
        LogMsg( LOG_ERROR, "VxServerMgr: reached max connections %d thread 0x%x", m_u32MaxConnections, VxGetCurrentThreadId() );
        dumpSocketStats("VxServerMgr");
        // we have reached max connections
        // just close it immediately
        VxCloseSktNow( oAcceptSkt );

        doSktDeleteCleanup();
		// sleep awhile
		VxSleep( 200 );
		return 0; // keep running until number of connections clear up
	}

    struct sockaddr_storage sktAddr;
    socklen_t sktAddrLen = VxSktAddrInit( ipv6, sktAddr );

    std::string rmtIp;
    uint16_t rmtPort{ 0 };
    if( 0 == getpeername( oAcceptSkt, (struct sockaddr*)&sktAddr, &sktAddrLen ) )
    {
        VxSktAddrGetParams( ipv6, sktAddr, rmtIp, rmtPort );
    }

    if( rmtIp.empty() )
    {
        LogMsg( LOG_ERROR, "Failed to get remote ip for accept skt %d", oAcceptSkt );
        VxCloseSktNow( oAcceptSkt );
        return 0;
    }
    else if( isHacker( rmtIp ) )
    {
        LogModule( eLogHackers, LOG_INFO, "Hacker from IP %d attempted connect again", rmtIp.c_str() );
        VxCloseSktNow( oAcceptSkt );
        return 0;
    }

	// add a skt to our list	
	std::shared_ptr<VxSktBase> sktBase = makeNewAcceptSkt( ipv6 );
	m_SktMgrMutex.lock(__FILE__, __LINE__); // dont let other threads mess with array while we add
	m_aoSkts.emplace_back( sktBase );
	// do tell skt to do accept stuff
	sktBase->m_Socket = oAcceptSkt;
	sktBase->setReceiveCallback( m_pfnOurReceive, this );
	sktBase->setTransmitCallback( m_pfnOurTransmit, this );
	m_SktMgrMutex.unlock(__FILE__, __LINE__);

    LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr: doing accept skt %d skt id %d thread 0x%x", sktBase->m_Socket, sktBase->getSktNumber(), VxGetCurrentThreadId() );

    RCODE rcAccept = dynamic_cast<VxSktAccept *>(sktBase.get())->doAccept( this, *(( struct sockaddr * )&acceptAddr) );
	if( rcAccept || poVxThread->isAborted() || INVALID_SOCKET == oListenSkt )
	{
		sktBase->closeSkt(eSktCloseAcceptFailed);
		LogMsg( LOG_ERROR, "VxServerMgr: error %d doing accept skt %d skt id %d thread 0x%x", rc, sktBase->m_Socket, sktBase->getSktNumber(), VxGetCurrentThreadId() );
        moveToEraseList( sktBase );

        rc = -7;
	}
    else
    {
        acceptErrCnt = 0; // reset counter
        m_LastListenActivityMs = GetGmtTimeMs();
        LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr: accept success skt %d skt id %d thread 0x%x", sktBase->m_Socket, sktBase->getSktNumber(), VxGetCurrentThreadId() );
    }

    doSktDeleteCleanup();
	return rc;
}

//============================================================================
void VxServerMgr::listenForConnectionsToAccept( bool ipv6, VxThread* poVxThread )
{
#ifdef DEBUG_SKT_CONNECTIONS
	//LogMsg( LOG_INFO, "111 IN THREAD VxServerMgr::listenForConnectionsToAccept started\n" ); 
	//LogMsg( LOG_INFO, "111 IN THREAD VxServerMgr::listen port %d ip %s skt %d\n", m_LclIp.getPort(), m_LclIp.toString().c_str(), m_aoListenSkts[0] ); 
#endif // DEBUG_SKT_CONNECTIONS

start_over:
    closeListenSocket( ipv6 );

    waitForValidListenSettings( ipv6 );

    if( shouldListenAbort( ipv6 ) )
    {
        return;
    }

    setIsListenParamsChanged( ipv6, false );

    SOCKET listenSock = INVALID_SOCKET;
    if( !createNewListenSocket( ipv6, getListenPort(), listenSock, getLocalIp(ipv6) ) )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket failed create listen socket for ip %s port %d", getLocalIp(ipv6).c_str(), getListenPort() );
        return;
    }

    setListenSkt( ipv6, listenSock );

    if( shouldListenAbort( ipv6 ) )
    {
        return;
    }

    m_LastListenActivityMs = GetGmtTimeMs();

	// for some unknown reason select code that works on mac/windows/linux does not work on android
	// on android when use select the select seems to work but in the accept it gets error 22 (invalid param) .. so do this crap
	while( !shouldListenAbort( ipv6 ) )
	{
        if( getIsListenParamsChanged( ipv6 ) )
        {
            goto start_over;
        }

        RCODE rc = 0;

        int listenResult = listen( listenSock, MAX_LISTEN_BACKLOG );
        if( 0 > listenResult )
		{
            rc = VxGetLastError();
#if defined(TARGET_OS_WINDOWS)
            if( rc == WSAEWOULDBLOCK )
            {
                rc = EAGAIN;
            }
#endif // defined(TARGET_OS_WINDOWS)
		}	

		if( shouldListenAbort( ipv6 ) )
		{
            LogModule( eLogAcceptConn, LOG_DEBUG, "listenForConnectionsToAccept: aborting1" );
			break;
		}

        if( getIsListenParamsChanged( ipv6 ) )
        {
            goto start_over;
        }
		
        if( rc )
        {
            if( rc == EAGAIN )
            {
                static int listenErrCnt = 0;
                listenErrCnt++;
                if( listenErrCnt > 50 )
                {
                    listenErrCnt = 0;
                    LogModule( eLogAcceptConn, LOG_DEBUG, "listenForConnectionsToAccept: try again: listen ip %s port %d skt %d error %d thread 0x%x", m_LclIp.toString().c_str(), m_u16ListenPort, getListenSkt( ipv6 ), rc, VxGetCurrentThreadId());
                }

                VxSleep( 200 );
                if( shouldListenAbort( ipv6 ) )
                {
                    LogModule( eLogAcceptConn, LOG_DEBUG, "listenForConnectionsToAccept: aborting2" );
                    break;
                }

                continue;
            }
            else 
            {
                LogMsg( LOG_DEBUG, "listen: ERROR %s", VxDescribeSktError( rc ) );

                VxSleep( 500 );
                if( shouldListenAbort( ipv6 ) )
                {
                    LogModule( eLogAcceptConn, LOG_DEBUG, "listenForConnectionsToAccept: aborting3" );
                    break;
                }

                // probably we lost internet connection for some reason.. create a new socket and try again
                goto start_over;
            }
        }

		acceptConnection( ipv6, poVxThread, listenSock );
	}	
	
	setIsReadyToAcceptConnections( ipv6, false );

    closeListenSocket( ipv6 );

    LogModule( eLogConnect, LOG_INFO, "Listen Thread is exiting thread 0x%x", VxGetCurrentThreadId() );
}

//============================================================================
bool VxServerMgr::createNewListenSocket( bool ipv6, uint16_t listenPort, SOCKET& retListenSock, std::string lclIp )
{
    // some vpns get confused if the same socket number is reused after closed so make a dummy socket so socket number is incremented
    SOCKET dummySock = socket( ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0 );               // creates IP based TCP socket
    if( dummySock <= 0 )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket failed" );
        return false;
    }

    SOCKET listenSock = socket( ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0 );               // creates IP based TCP socket
    if( listenSock <= 0 )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket failed" );
        VxCloseSkt( dummySock );
        return false;
    }

    if( !lclIp.empty() && !VxIsIpValid(lclIp) )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket invalid local ip %s", lclIp.c_str() );
        lclIp.clear();
    }

    if( !lclIp.empty() )
    {
        LogMsg( LOG_VERBOSE, "VxServerMgr::createListenSocket using local ip %s", lclIp.c_str() );
        lclIp.clear();
    }

#if !defined(TARGET_OS_WINDOWS)
    lclIp.clear();
#endif // !defined(TARGET_OS_WINDOWS)

    bool useAddrInfoBind{true};
#if 0 // seems that VPNs work better without the bind to a specific address
    char hostName[256]; // hostname max length is 255

    if( gethostname( hostName, sizeof(hostName) )  < 0 )
    {
        useAddrInfoBind = false;
        lclIp.clear();
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket gethostname failed %s", strerror(errno) );
    }
    else
    {
        char portStr[6];
        struct addrinfo *ai;
        struct addrinfo hints;
        memset( &hints, 0, sizeof(struct addrinfo) );

        hints.ai_flags = AI_PASSIVE;
        hints.ai_socktype = SOCK_STREAM;

        snprintf(portStr, 6, "%d", listenPort);

        int rc = getaddrinfo(hostName, portStr, &hints, &ai);
        if (rc != 0)
        {
            useAddrInfoBind = false;
            lclIp.clear();

            LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket error Resolving %s: %s", hostName, gai_strerror(rc));
        }
        else
        {
            if (bind(listenSock, ai->ai_addr, ai->ai_addrlen) != 0)
            {
                useAddrInfoBind = false;
                lclIp.clear();

                LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket error Binding to %s:%d: %s",
                              hostName,
                              listenPort,
                              strerror(errno));
            }

            freeaddrinfo (ai);
        }
    }

    #endif // 0;
    lclIp.clear();
    useAddrInfoBind = true;

    if( useAddrInfoBind )
    {
        struct sockaddr_storage sockAddr;
        socklen_t sockAddrLen = VxSktAddrInit( ipv6, sockAddr, lclIp, listenPort );

        // Bind Socket
        int bindStatus = bind( listenSock, (struct sockaddr*)&sockAddr, sockAddrLen );
        int retryCnt = 0;
        while( bindStatus < 0 )
        {
            retryCnt++;
            if( retryCnt >= 2 )
            {
                LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket bind socket %d failed event after %d tries", listenSock, retryCnt );
                break;
            }

            VxSleep( 1000 );
            bindStatus = bind( listenSock,  (struct sockaddr*)&sockAddr, sockAddrLen );
        }

        VxSetSktAllowReuseAddress( listenSock );
    }

    if( m_pfnSktMgrStatus )
    {
        m_pfnSktMgrStatus( "ListenOpen", (void*)listenSock, m_pvSktMgrStatusCallbackUserData );
    }

    retListenSock = listenSock;
    if( VxGetSktStatCallback() )
    {
        VxGetSktStatCallback()->sktConnected4( listenSock, VxGetLclIpAddress( listenSock ), eSktTypeListen, eConnectReasonUnknown );
    }

    VxCloseSkt( dummySock );
    return true;
}

//============================================================================
bool VxServerMgr::shouldListenAbort( bool ipv6 )
{
    if( VxIsAppShuttingDown() )
    {
        return true;
    }

    if( ipv6 && m_ListenThreadIpv6.isAborted() )
    {
        return true;
    }
    else if( !ipv6 && m_ListenThreadIpv4.isAborted() )
    {
        return true;
    }

    return false;
}

//============================================================================
void VxServerMgr::closeListenSocket( bool ipv6 )
{
    SOCKET sktToClose = getListenSkt( ipv6 );
    if( INVALID_SOCKET != sktToClose )
    {
        setListenSkt( ipv6, INVALID_SOCKET );
        LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr:listenForConnectionsToAccept closing listen skt %d", sktToClose );
        
        // set the socket to reuse or even though closed the system may not allow another listen on that port to be done
        // until the system has completely cleaned it up
        char setTrue = 1;
        setsockopt( sktToClose, SOL_SOCKET, SO_REUSEADDR, &setTrue, sizeof( char ) );

        ::VxCloseSkt( sktToClose );
        if( m_pfnSktMgrStatus )
        {
            m_pfnSktMgrStatus( "ListenClose", (void*)sktToClose, m_pvSktMgrStatusCallbackUserData );
        }
    }
}

//============================================================================
void VxServerMgr::setListenPort( uint16_t port )
{
    bool changed{ false };
    lockListenSettings();
    if( m_u16ListenPort != port )
    {
        m_u16ListenPort = port;
        changed = true;
    }

    unlockListenSettings();
    if( changed )
    {
        onListenSettingsChanged( false );
        onListenSettingsChanged( true );
    }
}

//============================================================================
uint16_t VxServerMgr::getListenPort( void )
{
    lockListenSettings();
    uint16_t port = m_u16ListenPort;
    unlockListenSettings();

    return port;
}

//============================================================================
void VxServerMgr::setLocalIp( bool ipv6, std::string& newLocalIp )
{
    bool changed{ false };
    lockListenSettings();
    if( ipv6 )
    {
        if( m_LclAddressIpv6 != newLocalIp )
        {
            m_LclAddressIpv6 = newLocalIp;
            changed = true;
        }
    }
    else
    {
        if( m_LclAddressIpv4 != newLocalIp )
        {
            m_LclAddressIpv4 = newLocalIp;
            changed = true;
        }
    }

    unlockListenSettings();
    if( changed )
    {
        onListenSettingsChanged( ipv6 );
    }
}

//============================================================================
std::string VxServerMgr::getLocalIp( bool ipv6, bool* retIsValid )
{
    lockListenSettings();
    std::string localIp = ipv6 ? m_LclAddressIpv6 : m_LclAddressIpv4;
    unlockListenSettings();

    if( retIsValid )
    {
        *retIsValid = VxIsIpValid( localIp );
    }

    return localIp;
}


//============================================================================
void VxServerMgr::setListenEnable( bool ipv6, bool enable )
{
    bool changed{ false };
    lockListenSettings();
    if( ipv6 )
    {
        if( m_ListenEnabledIpv6 != enable )
        {
            m_ListenEnabledIpv6 = enable;
            changed = true;
        }
    }
    else
    {
        if( m_ListenEnabledIpv4 != enable )
        {
            m_ListenEnabledIpv4 = enable;
            changed = true;
        }
    }

    unlockListenSettings();
    if( changed )
    {
        onListenSettingsChanged( ipv6 );
    }
}

//============================================================================
bool VxServerMgr::getListenEnable( bool ipv6 )
{
    lockListenSettings();
    bool listenEnabled = ipv6 ? m_ListenEnabledIpv6 : m_ListenEnabledIpv4;
    unlockListenSettings();

    return listenEnabled;
}

//============================================================================
void VxServerMgr::setListenSkt( bool ipv6, SOCKET skt )       
{ 
    lockListenSettings();
    ipv6 ? m_ListenSktIpv6 = skt :  m_ListenSktIpv4 = skt; 
    unlockListenSettings();
}

//============================================================================
SOCKET VxServerMgr::getListenSkt( bool ipv6, bool setExistingSktToInvalid )
{
    lockListenSettings();
    SOCKET skt = ipv6 ? m_ListenSktIpv6 : m_ListenSktIpv4;
    if( setExistingSktToInvalid )
    {
        ipv6 ? m_ListenSktIpv6 = INVALID_SOCKET : m_ListenSktIpv4 = INVALID_SOCKET;
    }

    unlockListenSettings();

    return skt;
}

//============================================================================
void VxServerMgr::onListenSettingsChanged( bool ipv6 )
{
    setIsListenParamsChanged( ipv6, true );
}

//============================================================================
void VxServerMgr::setIsListenParamsChanged( bool ipv6, bool isChanged )
{
    lockListenSettings();
    ipv6 ? m_SettingsChangedIpv6 = isChanged : m_SettingsChangedIpv4 = isChanged;
    if( isChanged )
    {
        // force close of listen skt so thread is released to rebuild listen
        SOCKET skt = ipv6 ? m_ListenSktIpv6 : m_ListenSktIpv4;
        ipv6 ? m_ListenSktIpv6 = INVALID_SOCKET : m_ListenSktIpv4 = INVALID_SOCKET;
  
        if( skt != INVALID_SOCKET )
        {
            VxSetSktBlocking( skt, false );
            
            LogModule( eLogAcceptConn, LOG_INFO, "VxServerMgr:listenForConnectionsToAccept closing listen skt %d", skt );
        
            // set the socket to reuse or even though closed the system may not allow another listen on that port to be done
            // until the system has completely cleaned it up
            char setTrue = 1;
            setsockopt( skt, SOL_SOCKET, SO_REUSEADDR, &setTrue, sizeof( char ) );

            ::VxCloseSkt( skt );
            if( m_pfnSktMgrStatus )
            {
                m_pfnSktMgrStatus( "ListenClose", (void*)skt, m_pvSktMgrStatusCallbackUserData );
            }
        }
    }

    unlockListenSettings();
}

//============================================================================
bool VxServerMgr::getIsListenParamsChanged( bool ipv6 )
{
    lockListenSettings();
    bool changed = ipv6 ? m_SettingsChangedIpv6 : m_SettingsChangedIpv4;
    unlockListenSettings();

    return changed;
}

//============================================================================
bool VxServerMgr::isListenParamsValid( bool ipv6 )
{
    lockListenSettings();
    bool valid = VxIsPortValid( m_u16ListenPort );
    if( valid )
    {
        valid &= ipv6 ? VxIsIpValid( m_LclAddressIpv6 ) : VxIsIpValid( m_LclAddressIpv4 );
    }

    unlockListenSettings();

    return valid;
}

//============================================================================
void VxServerMgr::setIsReadyToAcceptConnections( bool ipv6, bool isReady )
{ 
    lockListenSettings();
    ipv6 ? m_IsReadyToAcceptConnectionsIpv6 = isReady : m_IsReadyToAcceptConnectionsIpv4 = isReady;
    unlockListenSettings();
}

//============================================================================
bool VxServerMgr::getIsReadyToAcceptConnections( bool ipv6 )
{ 
    lockListenSettings();
    bool isReadyToAccept = ipv6 ? m_IsReadyToAcceptConnectionsIpv6 : m_IsReadyToAcceptConnectionsIpv4;
    unlockListenSettings();

    return isReadyToAccept;
}

//============================================================================
bool VxServerMgr::waitForValidListenSettings( bool ipv6 )
{
    while( !getListenEnable( ipv6 ) || !isListenParamsValid( ipv6 ) )
    {
        if( shouldListenAbort( ipv6 ) )
        {
            return false;
        }

        VxSleep( 1000 );

        if( shouldListenAbort( ipv6 ) )
        {
            return false;
        }
    }

    if( getUpnpEnable() )
    {
        doPortForward( ipv6, true );
    }

    if( shouldListenAbort( ipv6 ) )
    {
        return false;
    }

    return true;
}

//============================================================================
void VxServerMgr::listenSettingsUpdated( bool ipv6, bool forceListenSktRelease )
{
    // we need to release the listen thread so will update
    // not sure how well this will work
    setIsListenParamsChanged( ipv6, true );
}

//============================================================================
void VxServerMgr::setUpnpEnable( bool enable )
{
    if( enable != VxPortForward::getEnablePortForward() )
    {
        VxPortForward::setEnablePortForward( enable );
        if( enable )
        {
            setIsListenParamsChanged( false, true );
            setIsListenParamsChanged( true, true );
        }
    }
}

//============================================================================
bool VxServerMgr::getUpnpEnable( void )
{
    return VxPortForward::getEnablePortForward();
}

//============================================================================
bool VxServerMgr::doPortForward( bool ipv6, bool addPort )
{
    uint16_t port = getListenPort();
    if( addPort )
    {
        return VxPortForward::addPortForward( ipv6, getLocalIp( ipv6 ), port );
    }
    else
    {
        return VxPortForward::removePortForward( ipv6, port );
    }
}
