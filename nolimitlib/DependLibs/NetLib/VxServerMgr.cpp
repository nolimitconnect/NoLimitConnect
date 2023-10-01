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

#include "VxSktUtil.h"
#include "VxSktAccept.h"
#include "VxServerMgr.h"
#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxParse.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/AppErr.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxThread.h>

#include <stdio.h>
#include <memory.h>
#include <time.h>

//#define DEBUG_VXSERVER_MGR
#define DISABLE_WATCHDOG 1

int VxServerMgr::m_iAcceptMgrCnt = 0;				// number of accept managers that have been created

//============================================================================
namespace
{
    const int INACTIVE_LISTEN_RESTART_MS = 15 * 60 * 1000; // if no listen activity for 15 minutes reopen listen socke

    void * VxServerMgrVxThreadFunc(  void * pvContext )
	{
        if( pvContext )
        {
            VxThread* poVxThread = (VxThread*)pvContext;
            poVxThread->setIsThreadRunning( true );
            VxServerMgr * poMgr = (VxServerMgr *)poVxThread->getThreadUserParam();
            if( poMgr )
            {
                LogModule( eLogListen, LOG_INFO, "#### VxServerMgr: Mgr id %d Listen port %d thread started thread 0x%x", poMgr->m_iMgrId, poMgr->getListenPort(), VxGetCurrentThreadId() );
                poMgr->listenForConnectionsToAccept( poVxThread );
                // quitting
                LogModule( eLogListen, LOG_INFO, "#### VxServerMgr: Mgr id %d Listen port %d thread 0x%x tid %d quiting", poMgr->m_iMgrId, poMgr->getListenPort(), VxGetCurrentThreadId(), poVxThread->getThreadTid() );
            }

            //! VxThread calls this just before exit
            poVxThread->threadAboutToExit();
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
	memset( m_aoListenSkts, 0, sizeof( m_aoListenSkts ) );
}

//============================================================================
VxServerMgr::~VxServerMgr()
{
	stopListening();
}

//============================================================================
// overrides VxSktBaseMgr
void VxServerMgr::sktMgrShutdown( void )
{
	stopListening();
	VxSktBaseMgr::sktMgrShutdown();
}

//============================================================================
void VxServerMgr::fromGuiKickWatchdog( void )
{
	//LogMsg( LOG_SKT, "VxServerMgr: fromGuiKickWatchdog\n" );
	m_LastWatchdogKickMs = GetTimeStampMs();
}

//============================================================================
bool VxServerMgr::checkWatchdog( void )
{
	// there is an issue with android.. in some cases it can leave the listen socket open even though the
	// app is stopped or crashed. this watchdog is used to force the listen socket to close
	// and terminate the app if it is unresponsive.
#ifdef _DEBUG
	return true; // if we are debugging don't timeout because we are on a breakpoint
#endif 
#ifdef DISABLE_WATCHDOG
	return true; // if we are debugging don't timeout because we are on a breakpoint
#endif 

	if( ( GetTimeStampMs() - m_LastWatchdogKickMs ) < 4000 )
	{
		return true;
	}

	LogMsg( LOG_INFO, "Listen Watchdog Timeout" );
	return false;
}

//============================================================================
std::shared_ptr<VxSktBase> VxServerMgr::makeNewAcceptSkt( void )				
{ 
    std::shared_ptr<VxSktBase> sharedSkt( new VxSktAccept() );
	sharedSkt->setThisSkt( sharedSkt ); // so skt can do callbacks without look up in manager
	return sharedSkt;
}

//============================================================================
bool VxServerMgr::isListening( void )							
{ 
    return m_u16ListenPort && m_ListenVxThread.isThreadRunning() && m_aoListenSkts[ 0 ] != INVALID_SOCKET;
}

//============================================================================
bool VxServerMgr::restartListening( void )
{
    bool result{ false };
    if( isListening() )
    {
        LogModule( eLogListen, LOG_VERBOSE, "restartListen thread will restart port 0x%x", m_u16ListenPort );
        m_RestartListen = true;
        result = true;
    }
    else if( m_u16ListenPort != 0 )
    {
        LogModule( eLogListen, LOG_VERBOSE, "restartListen start listen on port 0x%x", m_u16ListenPort );
        result = startListening( m_u16ListenPort, nullptr );
    }
    else
    {
        LogModule( eLogListen, LOG_ERROR, "restartListen listen not running and invalid port %d", m_u16ListenPort );
    }

    return result;
}

//============================================================================
bool VxServerMgr::startListening( uint16_t u16ListenPort, const char* ip )
{
    stopListening();

    if( VxIsAppShuttingDown() )
    {
        return false;
    }

    if( 0 == u16ListenPort )
    {
        AppErr( eAppErrBadParameter, "VxServerMgr::startListening Bad param port %d", u16ListenPort );
        return false;
    }

    m_LastWatchdogKickMs = GetTimeStampMs();
    m_u16ListenPort = u16ListenPort;
    m_IsReadyToAcceptConnections = false;

    if( 0 != internalStartListen() )
    {
        LogModule( eLogListen, LOG_ERROR, "ipv4 listen() internalStartListen failed" );
        return false;
    }
    
    // wait for thread to open the socket
    bool isReady = false;
    int waitCnt = 0;
    while( !isReady && waitCnt < 40 )
    {
        isReady = m_IsReadyToAcceptConnections;
        if( !isReady )
        {
            VxSleep( 200 );
            waitCnt++;
        }
    }

    if( !isReady )
    {
        LogMsg( LOG_SEVERE, "ipv4 listen() open listen port failed" );
        return false;
    }

    return true;
}

//============================================================================
RCODE VxServerMgr::internalStartListen( void )
{
	// make a useful thread name
	std::string strVxThreadName;
	StdStringFormat( strVxThreadName, "VxServerMgr%d", m_iMgrId );
    return m_ListenVxThread.startThread( (VX_THREAD_FUNCTION_T)VxServerMgrVxThreadFunc, this, strVxThreadName.c_str() );
}

//============================================================================
void VxServerMgr::closeAndAbortListenSocket( void )
{
    if( m_iActiveListenSktCnt )
    {
        m_ListenVxThread.abortThreadRun( true );

        m_ListenSktIsBoundToIp = false;
        m_IsReadyToAcceptConnections = false;
        LogModule( eLogListen, LOG_DEBUG, "### VxServerMgr: Mgr %d stop listening %d skt cnt %d thread 0x%x", m_iMgrId, m_u16ListenPort, m_iActiveListenSktCnt, VxGetCurrentThreadId() );
        m_u16ListenPort = 0;

        // kill previous thread if running
        m_ListenVxThread.abortThreadRun( true );
        m_ListenMutex.lock();
        for( int i = 0; i < m_iActiveListenSktCnt; i++ )
        {
            if( INVALID_SOCKET != m_aoListenSkts[ i ] )
            {
                LogModule( eLogListen, LOG_INFO, "VxServerMgr: Mgr %d closing listen skt %d", m_iMgrId, i );

                // closing the thread should release it so it can exit
                SOCKET sktToClose = m_aoListenSkts[ i ];
                m_aoListenSkts[ i ] = INVALID_SOCKET;
                ::VxCloseSktNow( sktToClose );
                if( m_pfnSktMgrStatus )
                {
                    m_pfnSktMgrStatus( "ListenClose", (void*)sktToClose, m_pvSktMgrStatusCallbackUserData );
                }
            }
            else
            {
                LogModule( eLogListen, LOG_ERROR, "VxServerMgr:stopListening skt idx %d had invalid socket", i );
            }
        }

        m_iActiveListenSktCnt = 0;
        m_ListenMutex.unlock();
        if( m_ListenVxThread.isThreadRunning() )
        {
            m_ListenVxThread.killThread();
        }
    }
    else
    {
        LogModule( eLogListen, LOG_DEBUG, "VxServerMgr:stopListening called with no listen sockets" );
    }
}

//============================================================================
RCODE VxServerMgr::stopListening( void )
{
    if( !isListening() )
    {
        return 0; // not listening
    }

    closeAndAbortListenSocket();

    m_ListenMutex.lock();
    m_u16ListenPort = 0;
    m_LclIp.setIpAndPort( "", m_u16ListenPort );
    m_ListenMutex.unlock();

	return 0;
}

//============================================================================
RCODE VxServerMgr::acceptConnection( VxThread* poVxThread, SOCKET oListenSkt )
{
	RCODE rc = 0;
	if( INVALID_SOCKET == oListenSkt )
	{
        LogModule( eLogListen, LOG_ERROR, "VxServerMgr::acceptConnection INVALID LISTEN SOCKET thread 0x%x", VxGetCurrentThreadId() );
		return -2;
	}

	if( VxIsAppShuttingDown() )
	{
        LogModule( eLogListen, LOG_ERROR, "VxServerMgr::acceptConnection App Shutting down thread 0x%x", VxGetCurrentThreadId() );
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
    struct  sockaddr acceptAddr;
    socklen_t acceptAddrLen = sizeof( struct  sockaddr );
    memset( &acceptAddr, 0, sizeof( struct sockaddr ) );

#if defined( TARGET_OS_WINDOWS ) || defined( TARGET_OS_ANDROID )
    // NOTE: in android the return to blocking on listen doesn't work so we just set it once before start listening so accept does not get hung
    SOCKET oAcceptSkt = accept( oListenSkt, &acceptAddr, &acceptAddrLen );
#else //LINUX
	// NOTE: accept can hang waiting for connection in linux or android if
	// connection is dropped before the accept happens so set to non blocking.. the reason it hangs is it will wait until next connection occures	
	VxSetSktBlocking( oListenSkt, false );
    SOCKET oAcceptSkt = accept( oListenSkt, &acceptAddr, &acceptAddrLen );
	VxSetSktBlocking( oListenSkt, true );
#endif // LINUX

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
                    LogModule( eLogListen, LOG_DEBUG, "VxServerMgr::acceptConnection: listen port %d skt %d error %d thread 0x%x", m_u16ListenPort, oListenSkt, rc, VxGetCurrentThreadId() );
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
            LogModule( eLogListen, LOG_INFO, "VxServerMgr: no rc acceptConnection skt %d rc %d thread 0x%x", oListenSkt, VxGetLastError(), VxGetCurrentThreadId() );
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
            LogModule( eLogListen, LOG_INFO, "VxServerMgr: other error acceptConnection skt %d rc %d thread 0x%x", oListenSkt, VxGetLastError(), VxGetCurrentThreadId() );
			VxSleep( 200 );
			return rc;
		}
    }
    else
    {
        acceptErrCnt = 0;
    }

    LogModule( eLogListen, LOG_DEBUG, "VxServerMgr::acceptConnection: listen skt %d accepted skt %d thread 0x%x", oListenSkt, oAcceptSkt, VxGetCurrentThreadId() );
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

    struct sockaddr_in sktAddr;
    socklen_t sktAddrLen = sizeof( sktAddr );
    memset( &sktAddr, 0, sizeof( sktAddr ) );

    std::string rmtIp;
    if( 0 == getpeername( oAcceptSkt, (struct sockaddr*)&sktAddr, &sktAddrLen ) )
    {
        rmtIp = inet_ntoa( sktAddr.sin_addr );
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
	std::shared_ptr<VxSktBase> sktBase = makeNewAcceptSkt();
	m_SktMgrMutex.lock(__FILE__, __LINE__); // dont let other threads mess with array while we add
	m_aoSkts.emplace_back( sktBase );
	// do tell skt to do accept stuff
	sktBase->m_Socket = oAcceptSkt;
	sktBase->setReceiveCallback( m_pfnOurReceive, this );
	sktBase->setTransmitCallback( m_pfnOurTransmit, this );
	m_SktMgrMutex.unlock(__FILE__, __LINE__);

    LogModule( eLogListen, LOG_INFO, "VxServerMgr: doing accept skt %d skt id %d thread 0x%x", sktBase->m_Socket, sktBase->getSktNumber(), VxGetCurrentThreadId() );

    RCODE rcAccept = dynamic_cast<VxSktAccept *>(sktBase.get())->doAccept( this, *(( struct sockaddr * )&acceptAddr) );
	if( rcAccept || poVxThread->isAborted() || INVALID_SOCKET == oListenSkt )
	{
		sktBase->closeSkt(eSktCloseAcceptFailed);
		LogMsg( LOG_ERROR, "VxServerMgr: error %d doing accept skt %d skt id %d thread 0x%x", rc, sktBase->m_Socket, sktBase->getSktNumber(), VxGetCurrentThreadId() );
        moveToEraseList( sktBase );

        rc = -5;
	}
    else
    {
        acceptErrCnt = 0; // reset counter
        m_LastListenActivityMs = GetGmtTimeMs();
        LogModule( eLogListen, LOG_INFO, "VxServerMgr: accept success skt %d skt id %d thread 0x%x", sktBase->m_Socket, sktBase->getSktNumber(), VxGetCurrentThreadId() );
    }

    doSktDeleteCleanup();
	return rc;
}

//============================================================================
void VxServerMgr::listenForConnectionsToAccept( VxThread* poVxThread )
{
#ifdef DEBUG_SKT_CONNECTIONS
	//LogMsg( LOG_INFO, "111 IN THREAD VxServerMgr::listenForConnectionsToAccept started\n" ); 
	//LogMsg( LOG_INFO, "111 IN THREAD VxServerMgr::listen port %d ip %s skt %d\n", m_LclIp.getPort(), m_LclIp.toStdString().c_str(), m_aoListenSkts[0] ); 
#endif // DEBUG_SKT_CONNECTIONS

    uint16_t listenPort = m_u16ListenPort;
    if( listenPort < 80 )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket invalid listen port %d", listenPort );
        return;
    }

    closeListenSocket();

    SOCKET listenSock = INVALID_SOCKET;
    if( !createNewListenSocket( listenPort, listenSock ) )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket failed create listen socket for port %d", listenPort );
        return;
    }

    m_aoListenSkts[0] = listenSock;
    m_iActiveListenSktCnt = 1;

    m_IsReadyToAcceptConnections = true;

    m_LastListenActivityMs = GetGmtTimeMs();

	// for some unknown reason code that works on mac/windows/linux to use select does not work on android
	// on android when use select the select seems to work but in the accept it gets error 22 (invalid param) .. so do this crap
	while( !shouldListenAbort() && ( checkWatchdog() ) )
	{
        RCODE rc = 0;
        if( m_RestartListen || GetGmtTimeMs() - m_LastListenActivityMs > INACTIVE_LISTEN_RESTART_MS )
        {
            refreshListenSocket( listenPort );
        }

        int listenResult = listen( m_aoListenSkts[0], 8 );
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
		

		if( shouldListenAbort() )
		{
            LogModule( eLogListen, LOG_DEBUG, "listenForConnectionsToAccept: aborting1" );
			break;
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
                    LogModule( eLogListen, LOG_DEBUG, "listenForConnectionsToAccept: try again: listen ip %s port %d skt %d error %d thread 0x%x", m_LclIp.toStdString().c_str(), m_u16ListenPort, m_aoListenSkts[0], rc, VxGetCurrentThreadId() );
                }

                VxSleep( 200 );
                if( shouldListenAbort() )
                {
                    LogModule( eLogListen, LOG_DEBUG, "listenForConnectionsToAccept: aborting2" );
                    break;
                }

                continue;
            }
            else 
            {
                LogMsg( LOG_DEBUG, "listen: ERROR %s", VxDescribeSktError( rc ) );

                VxSleep( 500 );
                if( shouldListenAbort() )
                {
                    LogModule( eLogListen, LOG_DEBUG, "listenForConnectionsToAccept: aborting3" );
                    break;
                }

                // probably we lost internet connection for some reason.. create a new socket and try again
                refreshListenSocket( listenPort );
                continue;
            }
        }

		acceptConnection( poVxThread, m_aoListenSkts[0] );
	}	
	
	m_IsReadyToAcceptConnections = false;

    closeListenSocket();

	if( (false == VxIsAppShuttingDown() )
		&& ( false == checkWatchdog() ) )
	{
        LogModule( eLogListen, LOG_ERROR, "Listen Failed Watchdog" );
		std::terminate();
	}

    LogModule( eLogConnect, LOG_INFO, "Listen Thread is exiting thread 0x%x", VxGetCurrentThreadId() );
}

//============================================================================
bool VxServerMgr::createNewListenSocket( uint16_t listenPort, SOCKET& retListenSock )
{
    SOCKET listenSock = socket( AF_INET, SOCK_STREAM, 0 );               // creates IP based TCP socket
    if( listenSock < 0 )
    {
        LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket failed" );
        return false;
    }

    // don't know why reuse port doesn't work
    VxSetSktAllowReusePort( listenSock );

    struct sockaddr_in serverAddr;
    memset( &serverAddr, 0, sizeof( struct sockaddr_in ) );

    serverAddr.sin_family = AF_INET;                    // sets listen socket protocol type
    serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );   // sets our local IP address
    serverAddr.sin_port = htons( listenPort );          // sets the listen port number 

    // Bind Socket
    int bindStatus = bind( listenSock, ( struct sockaddr* )&serverAddr, sizeof( struct sockaddr ) );
    int retryCnt = 0;
    while( bindStatus < 0 )
    {
        retryCnt++;
        if( retryCnt >= 3 )
        {
            LogMsg( LOG_ERROR, "VxServerMgr::createListenSocket bind socket %d failed event after %d tries", listenSock, retryCnt );
            break;
        }

        VxSleep( 1000 );
        bindStatus = bind( listenSock, ( struct sockaddr* )&serverAddr, sizeof( struct sockaddr ) );
    }

    // don't know why reuse port doesn't work
    VxSetSktAllowReusePort( listenSock );

    // android set listen skt back to blocking doesn't work so just set to non blocking always ( part of accept hang fix ) 
    VxSetSktBlocking( listenSock, false );

    if( m_pfnSktMgrStatus )
    {
        m_pfnSktMgrStatus( "ListenOpen", (void*)listenSock, m_pvSktMgrStatusCallbackUserData );
    }

    retListenSock = listenSock;
    if( VxGetSktStatCallback() )
    {
        VxGetSktStatCallback()->sktConnected3( listenSock, VxGetLclIpAddress( listenSock ), eSktTypeListen );
    }

    return true;
}

//============================================================================
bool VxServerMgr::shouldListenAbort( void )
{
    if( m_ListenVxThread.isAborted()
        || VxIsAppShuttingDown()
        || (0 >= m_iActiveListenSktCnt) )
    {
        return true;
    }

    return false;
}

//============================================================================
void VxServerMgr::closeListenSocket( void )
{
    if( INVALID_SOCKET != m_aoListenSkts[ 0 ] )
    {
        SOCKET sktToClose = m_aoListenSkts[ 0 ];
        LogModule( eLogListen, LOG_INFO, "VxServerMgr:listenForConnectionsToAccept closing listen skt %d", sktToClose );
        m_aoListenSkts[ 0 ] = INVALID_SOCKET;
        ::VxCloseSktNow( sktToClose );
        if( m_pfnSktMgrStatus )
        {
            m_pfnSktMgrStatus( "ListenClose", (void*)sktToClose, m_pvSktMgrStatusCallbackUserData );
        }
    }
}

//============================================================================
bool VxServerMgr::refreshListenSocket( uint16_t listenPort )
{
    closeListenSocket();

    // better to create a first new socket before or we get the same socket handle back which may be confusing to VPN
    SOCKET junkSock = socket( AF_INET, SOCK_STREAM, 0 );

    SOCKET listenSock = INVALID_SOCKET;
    if( !createNewListenSocket( listenPort, listenSock ) )
    {
        LogMsg( LOG_ERROR, "Thread createListenSocket failed create listen socket for port %d ", listenPort );
        return false;
    }

    LogModule( eLogListen, LOG_DEBUG, "listenForConnectionsToAccept: created new socket to listen on" );
    m_aoListenSkts[ 0 ] = listenSock;
    m_iActiveListenSktCnt = 1;

    m_LastListenActivityMs = GetGmtTimeMs();

    VxCloseSkt( junkSock );
    return true;
}