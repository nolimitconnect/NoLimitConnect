//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetServicesMgr.h"

#include "NetServiceHdr.h"

#include "NetActionIsMyPortOpen.h"
#include "NetActionResolveConnectTestUrl.h"
#include "NetActionResolveDefaultUserHosts.h"
#include "NetActionResolveNetworkHostUrl.h"
#include "NetActionWaitForInternet.h"

#include <Network/NetworkStateMachine.h>
#include <Network/NetworkMgr.h>
#include <Network/NetConnector.h>

#include <P2PEngine/EngineSettingsDefaultValues.h>
#include <P2PEngine/P2PEngine.h>
#include <P2PEngine/EngineSettings.h>

#include <CoreLib/OsDetect.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxSktUtil.h>
#include <CoreLib/VxTime.h>

#include <NetLib/VxSktBase.h>

#include <PktLib/PktsTestConnection.h>
#include <PktLib/PktsQueryHostUrl.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <array>

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif //_MSC_VER

namespace
{
	//============================================================================
    void * NetServicesMgrThreadFunc( void * pvContext )
	{
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );

		NetServicesMgr * poMgr = (NetServicesMgr *)poThread->getThreadUserParam();
        if( poMgr && false == poThread->isAborted() )
        {
            poMgr->runNetActions();
        }

		poThread->threadAboutToExit();
        return nullptr;
	}
}

//============================================================================
NetServicesMgr::NetServicesMgr( P2PEngine& engine )
: m_Engine( engine )
, m_PktAnn( engine.getMyPktAnnounce() )
, m_EngineSettings( engine.getEngineSettings() )
, m_NetworkMgr( engine.getNetworkMgr() )
, m_NetServiceUtils( engine )
, m_NetActionIdle( *this )
, m_CurNetAction( &m_NetActionIdle )
{
}

//============================================================================
NetServicesMgr::~NetServicesMgr()
{
	netServicesShutdown();
}

//============================================================================
bool NetServicesMgr::shouldAbort( void )
{
	return VxIsAppShuttingDown() || m_NetActionThread.isAborted();
}

//============================================================================
std::string NetServicesMgr::getNetworkKey( void )
{
    return m_NetworkMgr.getNetworkKey();
}

//============================================================================
uint16_t NetServicesMgr::getRxNetServicePort( void )
{
    return m_Engine.getMyPktAnnounce().getOnlinePort();
}

//============================================================================
std::string NetServicesMgr::getRxNetIpAddress( bool isIpv6Connection )
{
    return m_Engine.getNetStatusAccum().getMyNetServiceIpAddress( isIpv6Connection );
}

//============================================================================
VxGUID& NetServicesMgr::getMyOnlineId( void )
{
    return m_Engine.getMyPktAnnounce().getMyOnlineId();
}

//============================================================================
void NetServicesMgr::netServicesStartup( void )
{
	m_NetActionThread.startThread( (VX_THREAD_FUNCTION_T)NetServicesMgrThreadFunc, this, "NetServMgrThrd" );
}

//============================================================================
void NetServicesMgr::netServicesShutdown( void )
{
	m_NetActionThread.abortThreadRun( true );
	m_NetActionMutex.lock();
	if( m_CurNetAction )
	{
		LogMsg( LOG_INFO, "NetServicesMgr::%s: action type %d thread 0x%x", __func__, m_CurNetAction->getNetActionType(), VxGetCurrentThreadId() );
	}

	m_NetActionMutex.unlock();
	m_NetActionSemaphore.signal();
	m_NetActionThread.killThread();
}

//============================================================================
void NetServicesMgr::addNetActionToQueue( enum ENetActionType netAction )
{
	if( shouldAbort() )
	{
		return;
	}

	if( isActionQued( netAction ) )
	{
		LogMsg( LOG_INFO, "NetServicesMgr::%s: action %s already in queue", __func__, DescribeNetAction( netAction ) );
		return;
	}

	switch( netAction )
	{
	case eNetActionIsPortOpen:
		addNetActionCommand( new NetActionIsMyPortOpen( *this ) );
		break;

	case eNetActionResolveConnectTestUrl:
		addNetActionCommand( new NetActionResolveConnectTestUrl( *this ) );
		break;

	case eNetActionResolveDefaultUserHosts:
		addNetActionCommand( new NetActionResolveDefaultUserHosts( *this ) );
		break;

	case eNetActionResolveNetworkHostUrl:
		addNetActionCommand( new NetActionResolveNetworkHostUrl( *this ) );
		break;
	
	case eNetActionWaitForInternet:
		addNetActionCommand( new NetActionWaitForInternet( *this ) );
		break;

	default:
		LogMsg( LOG_ERROR, "NetServicesMgr::%s: unknown action %d", __func__, netAction );
		return;
	}
}

//============================================================================
bool NetServicesMgr::isActionQued( ENetActionType eNetActionType )
{
	if( m_NetActionQueue.size() )
	{
		m_NetActionMutex.lock();
		for( auto netAction : m_NetActionQueue )
		{
			if( eNetActionType == netAction->getNetActionType() )
			{
				m_NetActionMutex.unlock();
				return true;
			}
		}

		m_NetActionMutex.unlock();
	}

	return false;
}

//============================================================================
void NetServicesMgr::addNetActionCommand( NetActionBase * netActionBase )
{
	m_NetActionMutex.lock();
	m_NetActionQueue.emplace_back( netActionBase );
	m_NetActionMutex.unlock();
	m_NetActionSemaphore.signal();
}

//============================================================================
void NetServicesMgr::runNetActions( void )
{
	int netActionNum = 0;
	while( ( false == m_NetActionThread.isAborted() )
			&& ( false == VxIsAppShuttingDown() ) )
	{
		m_NetActionSemaphore.wait();
		if( m_NetActionThread.isAborted()
			|| VxIsAppShuttingDown() )
		{
			LogMsg( LOG_DEBUG, "NetServicesMgr::runNetActions %d abort 1", netActionNum );
			return;
		}

		while( m_NetActionQueue.size() )
		{
			m_NetActionMutex.lock();
			m_CurNetAction = m_NetActionQueue.at(0);
			m_NetActionQueue.erase( m_NetActionQueue.begin() );
			m_NetActionMutex.unlock();

			netActionNum++;
			if( m_NetActionThread.isAborted()
				|| VxIsAppShuttingDown() )
			{
				LogMsg( LOG_DEBUG, "NetServicesMgr::runNetActions %d abort 2", netActionNum );
				return;
			}

			LogMsg( LOG_VERBOSE, "NetServicesMgr::runNetActions %d starting action %s", netActionNum, DescribeNetAction( m_CurNetAction->getNetActionType() ) );
			m_CurNetAction->enterAction();
			m_CurNetAction->doAction();
			m_CurNetAction->exitAction();
			LogMsg( LOG_VERBOSE, "NetServicesMgr::runNetActions %d done action %s", netActionNum, DescribeNetAction( m_CurNetAction->getNetActionType() ) );

			m_NetActionMutex.lock();
			delete m_CurNetAction;
			if( m_NetActionThread.isAborted()
				|| VxIsAppShuttingDown() )
			{
				m_CurNetAction = NULL;
				m_NetActionMutex.unlock();
				return;
			}

			m_CurNetAction = &m_NetActionIdle;
			m_NetActionMutex.unlock();

			if( m_NetActionThread.isAborted()
				|| VxIsAppShuttingDown() )
			{
				return;
			}

			// we keep the connection open until command que is empty
			if( 0 == m_NetActionQueue.size() )
			{
				if( m_SktToNetServices.isConnected() )
				{
					m_SktToNetServices.closeSkt(43);
				}
			}
		}
	}
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdPing( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr )
{
	return  sendPong( sktBase, netServiceHdr, netServiceHdr.m_NetCmdType == eNetCmdClientPing );
}

//============================================================================
RCODE NetServicesMgr::sendPong( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr, bool isClientPing )
{
    LogModule( eLogIsPortOpenTest, LOG_INFO, "Got ping from %s ", sktBase->getRemoteIp().c_str(), VxGetCurrentThreadId() );
	std::string content;
    StdStringFormat( content, "PONG-%s", sktBase->getRemoteIp().c_str() );
	return m_NetServiceUtils.buildAndSendCmd( sktBase, isClientPing ? eNetCmdClientPong : eNetCmdHostPong, content );
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdPong( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr )
{
    LogModule( eLogIsPortOpenTest, LOG_INFO, "Got PONG from %s thread 0x%x", sktBase->getRemoteIp().c_str(), VxGetCurrentThreadId() );
	return 0;
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdIsMyPortOpenReq( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr )
{
	char * pSktBuf = (char *)sktBase->getSktReadBuf();
	if( false == ( '/' == pSktBuf[ netServiceHdr.m_TotalDataLen - 1 ] ) )
	{
		sktBase->sktBufAmountRead( 0 );
        LogModule( eLogIsPortOpenTest, LOG_ERROR, "NetServicesMgr::handleNetCmdIsMyPortOpenReq: invalid content");
		std::string toClientContent = "0-"; // assume fail
		return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdIsMyPortOpenReply, toClientContent, eNetCmdErrorInvalidContent );
	}

	sktBase->sktBufAmountRead( 0 );

	pSktBuf[ netServiceHdr.m_TotalDataLen - 1 ] = 0;

	std::string fromClientContent = &pSktBuf[ netServiceHdr.m_SktDataUsed ];

	return handleNetCmdIsMyPortOpenReqContent( sktBase, netServiceHdr, fromClientContent );
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdIsMyPortOpenReqContent( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr, std::string& fromClientNetCmdContent )
{

	InetAddrAndPort rmtAddr;
    VxGetRmtAddress( sktBase->m_Socket, rmtAddr );
	std::string strRmtAddr = rmtAddr.toString().c_str();

	std::string toClientContent = "0-"; // assume fail
	toClientContent += strRmtAddr;

	if( strRmtAddr.empty() )
	{
        LogModule( eLogIsPortOpenTest, LOG_ERROR, "NetServicesMgr::handleNetCmdIsMyPortOpenReq: could not determine remote address");
		return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdIsMyPortOpenReply, toClientContent, eNetCmdErrorFailedResolveIpAddr );
	}
	
	if( fromClientNetCmdContent.empty() )
	{
        LogModule( eLogIsPortOpenTest, LOG_ERROR, "NetServicesMgr::handleNetCmdIsMyPortOpenReq: could not parse from client content");
		return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdIsMyPortOpenReply, toClientContent, eNetCmdErrorInvalidContent );
	}

	uint16_t u16Port = (uint16_t)atoi( fromClientNetCmdContent.c_str() );
	if( 0 == u16Port )
	{
        LogModule( eLogIsPortOpenTest, LOG_ERROR, "NetServicesMgr::handleNetCmdIsMyPortOpenReq: could not parse client port" );
		return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdIsMyPortOpenReply, toClientContent, eNetCmdErrorInvalidContent );
	}

    LogModule( eLogIsPortOpenTest, LOG_ERROR, "handleNetCmdIsMyPortOpenReq: Attempting ping ip %s port %d", strRmtAddr.c_str(), u16Port );
	VxTimer cmdTimer;
	std::string retPong;
	bool pingSuccess = doNetCmdPing( strRmtAddr.c_str(), u16Port, retPong, true );
	if( false == pingSuccess )
	{
        LogModule( eLogIsPortOpenTest, LOG_ERROR, "handleNetCmdIsMyPortOpenReq: FAILED PING ip %s port %d %3.3f sec", strRmtAddr.c_str(), u16Port, cmdTimer.elapsedMs() );
		return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdIsMyPortOpenReply, toClientContent, eNetCmdErrorPortIsClosed );
	}

    LogModule( eLogIsPortOpenTest, LOG_ERROR, "handleNetCmdIsMyPortOpenReq: SUCCESS PING ip %s port %d %3.3f sec", strRmtAddr.c_str(), u16Port, cmdTimer.elapsedMs() );
	toClientContent = "1-";
	toClientContent += strRmtAddr;
	return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdIsMyPortOpenReply, toClientContent, eNetCmdErrorNone );
}

//============================================================================
bool NetServicesMgr::doNetCmdPing( const char* ipAddress, uint16_t u16Port, std::string& retPong, bool isClientPing )
{
    int64_t pingStart = GetGmtTimeSec();
	EIpAddrType addrType = VxGetIpAddrType( ipAddress );

	LogModule( eLogIsPortOpenTest, LOG_INFO, "Test Client Connection send PING %s:%d", ipAddress, u16Port );
	VxSktConnectSimple toClientConn;
	if( INVALID_SOCKET != toClientConn.connectTo( ipAddress, u16Port, addrType, PORT_TEST_CONNECT_TO_CLIENT1_TIMEOUT ) )
	{        
        double connectTime = GetGmtTimeSec() - pingStart;
        VxTimer	pingTimer;
		LogModule( eLogIsPortOpenTest, LOG_INFO, "##P NetServicesMgr::doNetCmdPing connected to %s:%d in %3.3f sec thread 0x%x", ipAddress, u16Port, connectTime, VxGetCurrentThreadId() );
		bool sndPingResult = sendAndRecievePing( pingTimer, toClientConn, retPong, isClientPing, PONG_RX_TIMEOUT );
        double totalTime = GetGmtTimeSec() - pingStart;
		if( sndPingResult )
		{
			LogModule( eLogIsPortOpenTest, LOG_INFO, "##P NetServicesMgr::doNetCmdPing SUCCESS rx PONG from %s:%d in %3.3f sec thread 0x%x", ipAddress, u16Port, totalTime, VxGetCurrentThreadId() );
		}
		else
		{
			LogModule( eLogIsPortOpenTest, LOG_INFO, "##P NetServicesMgr::doNetCmdPing FAILED rx PONG from %s:%d in %3.3f sec thread 0x%x", ipAddress, u16Port, totalTime, VxGetCurrentThreadId() );

		}

		return sndPingResult;
	}
	
    double failTime = GetGmtTimeSec() - pingStart;
    LogMsg( LOG_ERROR, "##P NetServicesMgr::doNetCmdPing:  timeout %d could not connect to %s:%d %3.3f sec FAIL", PORT_TEST_CONNECT_TO_CLIENT1_TIMEOUT, ipAddress, u16Port, failTime );
	return false;
}

//============================================================================
bool NetServicesMgr::sendAndRecievePing( VxTimer& pingTimer, VxSktConnectSimple& toClientConn, std::string& retPong, bool isClientPing, int receiveTimeout )
{	
	std::string strNetCmd;
	std::string strPing = "PING";
	ENetCmdType netCmdPingType = isClientPing ? eNetCmdClientPing : eNetCmdHostPing;
	ENetCmdType netCmdPongType = isClientPing ? eNetCmdClientPong : eNetCmdHostPong;
	
	std::string netServChallengeHash;
	uint16_t cryptoKeyPort = toClientConn.getCryptoKeyPort();
    m_NetServiceUtils.generateNetServiceChallengeHash(netServChallengeHash, cryptoKeyPort, getNetworkKey() );

	m_NetServiceUtils.buildNetCmd( strNetCmd, netCmdPingType, netServChallengeHash, strPing );

    LogModule( eLogIsPortOpenTest, LOG_ERROR, "##P NetServicesMgr::sendAndRecievePing: skt %d cypto port %d strNetCmd(%s) thread 0x%x", toClientConn.getSktHandle(),
		cryptoKeyPort, strNetCmd.c_str(), VxGetCurrentThreadId() );

    // startSendTime is also the time it took to connect
	double startSendTime = pingTimer.elapsedSec();
	bool wasSent = m_NetServiceUtils.sendNetServiceRequest( netCmdPingType, &toClientConn, strNetCmd, 8000 );
	if( !wasSent )
	{
		double failSendTime = pingTimer.elapsedSec();
        LogModule( eLogIsPortOpenTest, LOG_ERROR, "##P NetServicesMgr::sendAndRecievePing: skt %d sendData failed %s in %3.3f sec thread 0x%x", toClientConn.getSktHandle(),
                  VxDescribeSktError( toClientConn.getLastError() ), failSendTime - startSendTime, VxGetCurrentThreadId() );
		toClientConn.closeSkt();
		return false;
	}

	double endSendTime = pingTimer.elapsedSec();
	char rxBuf[ 513 ];
	rxBuf[0] = 0;
	NetServiceHdr netServiceHdr;
	if( false == m_NetServiceUtils.rxNetServiceCmd( netCmdPongType, &toClientConn, rxBuf, sizeof( rxBuf ) - 1, netServiceHdr, receiveTimeout, receiveTimeout )  )
	{
		double failResponseTime = pingTimer.elapsedSec();
		LogModule( eLogIsPortOpenTest, LOG_ERROR, "##P NetServicesMgr::sendAndRecievePing: skt %d no response with timeout spec %d and times connect %3.3f sec send %3.3f sec fail respond %3.3f sec thread 0x%x",
			toClientConn.getSktHandle(), receiveTimeout, startSendTime, startSendTime - endSendTime,  failResponseTime - startSendTime, VxGetCurrentThreadId() );
		toClientConn.closeSkt();
		return false;
	}

    rxBuf[ sizeof( rxBuf ) - 1 ] = 0;
	double successResponseTime = pingTimer.elapsedSec();
	LogModule( eLogIsPortOpenTest, LOG_DEBUG, "## NetServicesMgr::sendAndRecievePing: skt %d client rxNetServiceCmd response %3.3f sec thread 0x%x closing socket",
		toClientConn.getSktHandle(), successResponseTime, VxGetCurrentThreadId() );
	SOCKET historySkt = toClientConn.getSktHandle();
	toClientConn.closeSkt();

    if( netServiceHdr.m_ContentDataLen <= 0 )
    {
		LogModule( eLogIsPortOpenTest, LOG_ERROR, "## NetServicesMgr::sendAndRecievePing: skt %d  No Content connect %3.3f sec send %3.3f sec fail respond %3.3f sec thread 0x%x",
			historySkt, startSendTime, endSendTime - startSendTime, successResponseTime - startSendTime, VxGetCurrentThreadId() );
        return false;
    }

	if( ( 0 == netServiceHdr.m_TotalDataLen )
		|| ( 511 <= netServiceHdr.m_TotalDataLen )
		|| ( '/' != rxBuf[ netServiceHdr.m_ContentDataLen - 1 ] ) )
	{
		LogModule( eLogIsPortOpenTest, LOG_ERROR, "##P NetServicesMgr::sendAndRecievePing: skt %d invalid response  connect %3.3f sec send %3.3f sec fail respond %3.3f sec thread 0x%x",
			historySkt, startSendTime, endSendTime - startSendTime,  successResponseTime - startSendTime, VxGetCurrentThreadId() );
		return false;
	}

	rxBuf[ netServiceHdr.m_ContentDataLen - 1 ] = 0; 
	LogModule( eLogIsPortOpenTest, LOG_DEBUG, "##P NetServicesMgr::sendAndRecievePing: skt %d SUCCESS connect %3.3f sec send %3.3f sec response %3.3f sec thread 0x%x content %s",
		historySkt, startSendTime, endSendTime - startSendTime, successResponseTime - startSendTime, VxGetCurrentThreadId(), rxBuf );
	retPong = rxBuf;
	return true;
}

//============================================================================
void NetServicesMgr::setMyPortOpenResultCallback( MY_PORT_OPEN_CALLBACK_FUNCTION pfuncPortOpenCallbackHandler, void * userData )
{
	m_pfuncPortOpenCallbackHandler = pfuncPortOpenCallbackHandler;
	m_PortOpenCallbackUserData = userData;
}

//============================================================================
void NetServicesMgr::setQueryHostOnlineIdResultCallback( QUERY_HOST_ID_CALLBACK_FUNCTION pfuncQueryHostIdCallbackHandler, void * userData )
{
    m_pfuncQueryHostIdCallbackHandler = pfuncQueryHostIdCallbackHandler;
    m_QueryHostIdCallbackUserData = userData;
}

//============================================================================
void NetServicesMgr::netActionResultIsMyPortOpen( ENetCmdError eAppErr, std::string& myExternalIp )
{
    if( eAppErr == eNetCmdErrorNone )
    {
        LogModule( eLogNetworkState, LOG_INFO, "NetServicesMgr::%s CAN DIRECT CONNECT extern ip %s", __func__, myExternalIp.c_str() );
        // tested and can direct connect
        m_Engine.getNetStatusAccum().setDirectConnectTested( true, false, myExternalIp );
    }
    else if( eAppErr == eNetCmdErrorPortIsClosed )
    {
        // tested but cannot direct connect
        LogModule( eLogNetworkState, LOG_INFO, "NetServicesMgr::%s REQUIRES RELAY extern ip %s", __func__, myExternalIp.c_str() );
        m_Engine.getNetStatusAccum().setDirectConnectTested( true, true, myExternalIp );
    }
    else
    {
        // port open test failed with other error
        LogModule( eLogNetworkState, LOG_INFO, "NetServicesMgr::%s err %d extern ip %s", __func__, eAppErr, myExternalIp.c_str() );
        m_Engine.getNetStatusAccum().setDirectConnectTested( false, false, myExternalIp );
    }
}

//============================================================================
VxSktConnectSimple * NetServicesMgr::actionReqConnectToNetService( void )
{
	if( m_SktToNetServices.isConnected() )
	{
		// already connected
		return &m_SktToNetServices;
	}

	bool sktConnected = actionReqConnectToNetService( m_SktToNetServices );
	return sktConnected ? &m_SktToNetServices : 0;
}

//============================================================================
bool NetServicesMgr::actionReqConnectToNetService( VxSktConnectSimple& sktSimple )
{
	sktSimple.closeSkt();
	std::string netSrvUrl;
	if( m_Engine.getNetworkStateMachine().isNetworkWebsitesResolved() )
	{
		VxMakePtopUrl( m_Engine.getNetworkStateMachine().getNetServiceIp(), m_Engine.getNetworkStateMachine().getNetServicePort(), netSrvUrl );
	}
	else
	{
		m_EngineSettings.getConnectTestUrl( netSrvUrl );
	}

	std::string strHost;
	std::string strFile;
	uint16_t u16Port;

	if( false == sktSimple.connectToWebsite( netSrvUrl.c_str(), strHost, strFile, u16Port, eIpAddrTypeUnknown, NETSERVICE_CONNECT_TIMEOUT ) )
	{
#ifdef LOG_PORT_TEST
		LogMsg( LOG_ERROR, "NetServicesMgr::%s: FAILED to Connect to %s", __func__, netSrvUrl.c_str() );
#endif // LOG_PORT_TEST

		return false;
	}

	//LogMsg( LOG_VERBOSE, "actionReqConnectToNetService: SUCCESS\n" );
	return true;
}

//============================================================================
VxSktConnectSimple * NetServicesMgr::actionReqConnectToHost( void )
{
	if( m_SktToHost.isConnected() )
	{
		// already connected
		return &m_SktToHost;
	}

	if( false == actionReqConnectToHost( m_SktToHost ) )
	{
		return 0;
	}

	return &m_SktToHost;
}

//============================================================================
bool NetServicesMgr::actionReqConnectToHost( VxSktConnectSimple& sktSimple )
{
	std::string anchorUrl;
	if( m_Engine.getNetworkStateMachine().isNetworkWebsitesResolved() )
	{
		VxMakePtopUrl( m_Engine.getNetworkStateMachine().getHostIp(), m_Engine.getNetworkStateMachine().getHostPort(), anchorUrl );
	}
	else
	{
		m_EngineSettings.getNetworkHostUrl( anchorUrl );
	}

	std::string strHost;
	std::string strFile;
	uint16_t u16Port;

	if( false == sktSimple.connectToWebsite( anchorUrl.c_str(), strHost, strFile, u16Port, eIpAddrTypeUnknown, NETSERVICE_CONNECT_TIMEOUT ) )
	{
        if( IsLogEnabled( eLogNetworkState ) )
        {
            LogMsg( LOG_ERROR, "### ERROR NetServicesMgr::%s: FAILED to Connect to %s timeout %d", __func__, anchorUrl.c_str(), NETSERVICE_CONNECT_TIMEOUT );
        }

		return false;
	}

	//LogMsg( LOG_INFO, "actionReqConnectToHost: SUCCESS\n" );
	return true;
}

//============================================================================
ENetCmdError NetServicesMgr::doIsMyPortOpen( std::string& retMyExternalIp, bool userTest )
{
    retMyExternalIp.clear();

	uint16_t tcpListenPort = m_Engine.getMyPktAnnounce().getOnlinePort();
	bool ipv6 = m_EngineSettings.getUseIpv6();
	// for logging.. do not bind local ip
	std::string lclIp = m_Engine.getNetStatusAccum().getLocalIpAddress();
	
	VxTimer portTestTimer;
	m_IsPortOpenMutex.lock();

    EFirewallTestType testType = getEngineSettings().getFirewallTestSetting();
    if( eFirewallTestAssumeNoFirewall == testType )
    {
        m_EngineSettings.getUserSpecifiedExternIpAddr( retMyExternalIp, ipv6 );
        if( !retMyExternalIp.empty() )
        {
			m_Engine.getNetStatusAccum().setDirectConnectTested( true, false, retMyExternalIp );

			if( m_pfuncPortOpenCallbackHandler )
			{
				m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, eNetCmdErrorNone, retMyExternalIp );
			}

            LogModule( eLogIsPortOpenTest, LOG_INFO, "NetServicesMgr::%s: Your TCP Port %d IS ASSUMED OPEN :) IP is %s->%s in %3.3f sec thread 0x%x", __func__, tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
            m_Engine.sendToGuiStatusMessage( "Your TCP Port %d IS ASSUMED OPEN ON IP %s)", tcpListenPort, retMyExternalIp.c_str() );
			m_IsPortOpenMutex.unlock();
			if( userTest )
			{
				sendUserLog( "Your TCP Port %d IS ASSUMED OPEN ON IP %s)", tcpListenPort, retMyExternalIp.c_str() );
			}
            return eNetCmdErrorNone;
        }
        else
        {
			if( m_pfuncPortOpenCallbackHandler )
			{
				m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, eNetCmdErrorBadParameter, retMyExternalIp );
			}

            LogModule( eLogIsPortOpenTest, LOG_ERROR, "NetServicesMgr::%s: Test Port %d Is Open ERROR no external IP specified for Assume No Firewall thread 0x%x", 
					__func__, tcpListenPort, VxGetCurrentThreadId() );
            m_Engine.sendToGuiStatusMessage( "Test Port %d Is Open ERROR no external IP specified for Assume No Firewall thread 0x%x", tcpListenPort, VxGetCurrentThreadId() );
			m_IsPortOpenMutex.unlock();
			if( userTest )
			{
				sendUserLog( "Test Port %d Is Open ERROR no external IP specified for Assume No Firewall", tcpListenPort );
			}
            return eNetCmdErrorBadParameter;
        }
    }

    // use connection test url to test if listen port is open
    VxSktConnectSimple portOpenConn1;

    std::string netSrvUrl;
    if( m_Engine.getNetStatusAccum().isHostResolved( eHostTypeConnectTest ) )
    {
		m_Engine.getNetStatusAccum().getResolvedHost( eHostTypeConnectTest, netSrvUrl );
    }
    else
    {
        m_EngineSettings.getConnectTestUrl( netSrvUrl );
    }

    LogModule( eLogIsPortOpenTest, LOG_DEBUG, "%s Starting IsPortOpen port %d test url %s app sec %d thread 0x%x", __func__, lclIp.c_str(), tcpListenPort, netSrvUrl.c_str(), GetApplicationAliveSec(), VxGetCurrentThreadId() );
    m_Engine.sendToGuiStatusMessage( "Attempting Connect lcl ip %s to connect test service %s", lclIp.c_str(), netSrvUrl.c_str() );
	if( userTest )
	{
		sendUserLog( "Attempting Connect lcl ip %s to connect test service %s", lclIp.c_str(), netSrvUrl.c_str() );
	}

    std::string strHost;
    std::string strFile;
    uint16_t u16Port = 0;
	ENetCmdError portOpenTestError = eNetCmdErrorNone;
    if( VxSplitHostAndFile( netSrvUrl.c_str(), strHost, strFile, u16Port ) )
    {
		RCODE rc = 0;
        portOpenConn1.connectTo( strHost.c_str(), u16Port, eIpAddrTypeUnknown, NETSERVICE_CONNECT_TIMEOUT, &rc );
        if( !portOpenConn1.isConnected() )
        {
			if( m_pfuncPortOpenCallbackHandler )
			{
				m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, eNetCmdErrorConnectFailed, retMyExternalIp );
			}

            LogModule( eLogIsPortOpenTest, LOG_ERROR, "NetServicesMgr::%s: FAILED to Connect lcl ip %s to connect test service %s error %s", 
					   __func__, lclIp.c_str(), netSrvUrl.c_str(), VxDescribeSktError( rc ) );
            m_Engine.sendToGuiStatusMessage( "FAILED Connect lcl ip %s to connect test service %s error %s", lclIp.c_str(), netSrvUrl.c_str(), VxDescribeSktError( rc ) );
			m_IsPortOpenMutex.unlock();
			if( userTest )
			{
				sendUserLog( "FAILED Connect lcl ip %s to connect test service %s error %s", lclIp.c_str(), netSrvUrl.c_str(), VxDescribeSktError( rc ) );
			}
            return eNetCmdErrorConnectFailed;
        }
        else
        {
			if( lclIp.empty() )
			{
				lclIp = portOpenConn1.getLocalIpAddress();
				if( !lclIp.empty() && m_NetworkMgr.getLocalIpAddress().empty() )
				{
					m_NetworkMgr.setLocalIpAddress( lclIp );
				}
			}

            m_Engine.sendToGuiStatusMessage( "Success Connect lcl ip %s to connect test service %s.. starting test", lclIp.c_str(), netSrvUrl.c_str() );
			if( userTest )
			{
				sendUserLog( "Success Connect lcl ip %s to connect test service %s.. starting is port open test", lclIp.c_str(), netSrvUrl.c_str() );
			}

            portOpenTestError = sendAndRecieveIsMyPortOpen( portTestTimer,
                                                            &portOpenConn1,
                                                            tcpListenPort,
                                                            retMyExternalIp,
                                                            userTest,
															NETSERVICE_IS_PORT_OPEN_TXRX_TIMEOUT);

            portOpenConn1.closeSkt();
        }
    }
    else
    {
        LogModule( eLogIsPortOpenTest, LOG_INFO, "NetServicesMgr::%s: FAILED to Split Service URL %s thread 0x%x", __func__, netSrvUrl.c_str(), VxGetCurrentThreadId() );
		if( userTest )
		{
			sendUserLog( "NetServicesMgr::%s: FAILED to Split Service URL %s", __func__, netSrvUrl.c_str());
		}

        portOpenTestError = eNetCmdErrorBadParameter;
		if( m_pfuncPortOpenCallbackHandler )
		{
			m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, eNetCmdErrorBadParameter, retMyExternalIp );
		}

		m_IsPortOpenMutex.unlock();
		return portOpenTestError;
    }

	// release the thread waiting for port open test result as soon as possible
	if( m_pfuncPortOpenCallbackHandler )
	{
		m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, portOpenTestError, retMyExternalIp );
	}

	if( eNetCmdErrorResponseTimedOut == portOpenTestError )
	{
		m_Engine.sendToGuiStatusMessage( "TCP Port %d Test site %s Timed out so will have to be tested manually", tcpListenPort, netSrvUrl.c_str() );
		if( userTest )
		{
			sendUserLog( "TCP Port %d Test site %s Timed out so will have to be tested manually", tcpListenPort, netSrvUrl.c_str() );
		}
		m_IsPortOpenMutex.unlock();
		return portOpenTestError;
	}

	if( eNetCmdErrorNone == portOpenTestError )
	{
		m_Engine.getNetStatusAccum().setDirectConnectTested( true, false, retMyExternalIp );
		if( m_pfuncPortOpenCallbackHandler )
        {
            m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, portOpenTestError, retMyExternalIp );
        }

        LogModule( eLogIsPortOpenTest, LOG_INFO, "NetServicesMgr::%s: Your TCP Port %d IS OPEN :) IP %s->%s->connect test %s sec %3.3f thread 0x%x", 
            __func__, tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), netSrvUrl.c_str(), portTestTimer.elapsedSec(), VxGetCurrentThreadId() );

		m_Engine.sendToGuiStatusMessage( "Your TCP Port %d IS OPEN :) IP %s->%s->connect test %s", tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), netSrvUrl.c_str() );
		if( userTest )
		{
			sendUserLog( "Your TCP Port %d IS OPEN :) IP %s->%s->connect test %s", tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), netSrvUrl.c_str() );
		}
	}
	else
	{
		portOpenTestError = eNetCmdErrorPortIsClosed;
		m_Engine.getNetStatusAccum().setDirectConnectTested( true, true, retMyExternalIp );

        if( m_pfuncPortOpenCallbackHandler )
        {
            m_pfuncPortOpenCallbackHandler( m_PortOpenCallbackUserData, portOpenTestError, retMyExternalIp );
        }

		LogModule( eLogIsPortOpenTest, LOG_INFO, "Your TCP Port %d IS CLOSED :( IP %s->%s->%s sec %3.3f", tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), netSrvUrl.c_str(), portTestTimer.elapsedSec() );

        m_Engine.sendToGuiStatusMessage( "Your TCP Port %d IS CLOSED :( IP %s->%s->connect test %s", tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), netSrvUrl.c_str() );
		if( userTest )
		{
			sendUserLog( "Your TCP Port %d IS CLOSED :( IP %s->%s->connect test %s", tcpListenPort, lclIp.c_str(), retMyExternalIp.c_str(), netSrvUrl.c_str() );
		}
	}

	m_IsPortOpenMutex.unlock();
	return portOpenTestError; 
}

//============================================================================
bool NetServicesMgr::testLoobackPing( std::string localIP, uint16_t tcpListenPort )
{
	VxSktConnectSimple toClientConn;
	VxTimer	pingTimer;
	std::string ipAddress = localIP;

	if( INVALID_SOCKET == toClientConn.connectTo( ipAddress.c_str(), tcpListenPort, VxGetIpAddrType( ipAddress.c_str() ), 2000 ) )
	{
        if( IsLogEnabled( eLogNetworkState ) )
        {
		    LogMsg( LOG_ERROR, "##P NetServicesMgr::testLoobackPing: could not connect to %s:%d %3.3f sec", ipAddress.c_str(), tcpListenPort, pingTimer.elapsedSec() );
        }

		return false;
	}

	toClientConn.closeSkt();
	return true;
}

//============================================================================
ENetCmdError NetServicesMgr::sendAndRecieveIsMyPortOpen( VxTimer&				portTestTimer,
														VxSktConnectSimple *	netServConn, 
														int						tcpListenPort,
														std::string&			retMyExternalIp,
														bool					sendMsgToUser,
														int						sendRecieveTimeout )
{
    if( nullptr == netServConn || !netServConn->isConnected() )
    {
		LogMsg( LOG_INFO, "Is Your TCP Port %d Open Test Connect FAILED (%3.3f sec) thread 0x%x", tcpListenPort, portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
		if( sendMsgToUser )
		{
			sendUserLog( "Is TCP Port %d Open Test Connect FAILED (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec()  );
		}

		return eNetCmdErrorConnectFailed;
	}

	std::string cmdContent =  std::to_string( tcpListenPort );

	LogMsg( LOG_INFO, "Is Port %d Open Connected in %3.3f sec now Sending data thread 0x%x", tcpListenPort, portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
	if( sendMsgToUser )
	{
		sendUserLog( "Is Port %d Open Connected in %3.3f sec now Sending data", tcpListenPort, portTestTimer.elapsedSec() );
	}

	portTestTimer.startTimer();
	setIsTestConnectionActive( true );
	bool wasSent = 0 == getNetUtils().buildAndSendCmd( netServConn, eNetCmdIsMyPortOpenReq, cmdContent );
	if( !wasSent )
	{
		LogMsg( LOG_ERROR, "Is TCP Port %d Open Send Command Error (%3.3f sec) thread 0x%x", tcpListenPort, portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
		if( sendMsgToUser )
		{
			sendUserLog( "Is TCP Port %d Open Send Command Error (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec()  );
		}

		setIsTestConnectionActive( false );
		return eNetCmdErrorTxFailed;
	}

	VxSleep( 1000 );
	char rxBuf[ 1024 ];
	rxBuf[ 0 ] = 0;
	NetServiceHdr netServiceHdr;
	if( false == m_NetServiceUtils.rxNetServiceCmd( eNetCmdIsMyPortOpenReply,
													netServConn, 
													rxBuf, 
													sizeof( rxBuf ) - 1, 
													netServiceHdr, 
													sendRecieveTimeout,
													sendRecieveTimeout ) )
	{
		setIsTestConnectionActive( false );
		if( sendMsgToUser )
		{
			sendUserLog( "Is TCP Port %d Open Connect Test Server Response Time Out (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec() );
		}

		m_Engine.sendToGuiStatusMessage( "Is TCP Port %d Open Connect Test Server Response Time Out (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec()  );
		return eNetCmdErrorResponseTimedOut;
	}

	setIsTestConnectionActive( false );
    rxBuf[ sizeof( rxBuf ) - 1 ] = 0;
	std::string content = rxBuf;
	if( 0 == content.length() )
	{
		LogMsg( LOG_ERROR, "Is TCP Port %d Open No Server Response Content (%3.3f sec) thread 0x%x", tcpListenPort, portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
		if( sendMsgToUser )
		{
			sendUserLog( "Is TCP Port %d Open No Server Response Content (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec()  );
		}

		return eNetCmdErrorInvalidContent;
	}

	const char* contentBuf = content.c_str();
	if( '/' != contentBuf[content.length() -1] )
	{
		LogMsg( LOG_ERROR, "Is TCP Port %d Open Test Invalid Response Content (%3.3f sec) thread 0x%x", tcpListenPort, portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
		if( sendMsgToUser )
		{
			sendUserLog( "Is TCP Port %d Open Test Invalid Response Content (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec()  );
		}

		return eNetCmdErrorInvalidContent;
	}

	((char *)contentBuf)[content.length() -1] = 0;

	std::vector<std::string> contentParts;
	StdStringSplit( content, '-', contentParts );
	if( 2 != contentParts.size() )
	{
		LogMsg( LOG_ERROR, "Is TCP Port %d Open Test Invalid Content Parts (%3.3f sec) thread 0x%x", tcpListenPort, portTestTimer.elapsedSec(), VxGetCurrentThreadId() );
		if( sendMsgToUser )
		{
			sendUserLog( "Is TCP Port %d Open Test Invalid Content Parts (%3.3f sec)", tcpListenPort, portTestTimer.elapsedSec()  );
		}

		return eNetCmdErrorInvalidContent;
	}

	retMyExternalIp = contentParts[1];

	std::string strPayload = contentParts[0];
    int iIsOpen = atoi( contentParts[ 0 ].c_str() );

    LogModule( eLogConnect, LOG_INFO, "NetActionIsMyPortOpen::%s: test can direct connect %s my ip %s:%d thread 0x%x", 
			   __func__, strPayload.c_str(), retMyExternalIp.c_str(), tcpListenPort, VxGetCurrentThreadId() );
	if( sendMsgToUser )
	{
		sendUserLog( "NetActionIsMyPortOpen::%s: test can direct connect %s my ip %s:%d",
					 __func__, strPayload.c_str(), retMyExternalIp.c_str(), tcpListenPort );
	}

	return iIsOpen ? eNetCmdErrorNone : eNetCmdErrorPortIsClosed;
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdIsMyPortOpenReply( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr )
{
    RCODE rc = -1;
    // not called here.. handled in NetAction
    return rc;
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdQueryHostIdReq( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr )
{
    std::string toClientContent = "0"; // assume fail

    char * pSktBuf = ( char * )sktBase->getSktReadBuf();
    if( false == ( '/' == pSktBuf[ netServiceHdr.m_TotalDataLen - 1 ] ) )
    {
        sktBase->sktBufAmountRead( 0 );
        LogModule( eLogNetService, LOG_ERROR, "NetServicesMgr::%s: invalid content", __func__ );
        return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdQueryHostOnlineIdReply, toClientContent, eNetCmdErrorInvalidContent );
    }

    if( !m_Engine.getHasAnyAnnonymousHostService() )
    {
        // only allowed to query host Id if we have an annonymous Host Service
        LogModule( eLogNetService, LOG_ERROR, "NetServicesMgr::%s: Access Denied.. No Annonymous host service", __func__ );
        return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdQueryHostOnlineIdReply, toClientContent, eNetCmdErrorPermissionLevel );
    }

    VxGUID myId = m_Engine.getMyOnlineId();
    std::string hexMyId;
    if( !myId.toHexString( hexMyId ) )
    {
        LogModule( eLogNetService, LOG_ERROR, "NetServicesMgr::%s: Access Denied.. Invalid My Online Id", __func__ );
        return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdQueryHostOnlineIdReply, toClientContent, eNetCmdErrorInvalidContent );
    }

    LogModule( eLogNetService, LOG_INFO, "%s: SUCCESS %s", __func__, hexMyId.c_str() );
    return m_NetServiceUtils.buildAndSendCmd( sktBase, eNetCmdQueryHostOnlineIdReply, hexMyId, eNetCmdErrorNone );
}

//============================================================================
ENetCmdError NetServicesMgr::sendAndRecieveQueryHostId( VxTimer&				testTimer,
														VxSktConnectSimple *	netServConn,
														VxGUID&					retHostId,
														bool					sendMsgToUser )
{
    retHostId.clearVxGUID();
    if( nullptr == netServConn || !netServConn->isConnected() ) 
    {
        LogModule( eLogNetService, LOG_INFO, "Query Host Online Id Connect FAILED (%3.3f sec) thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
        if( sendMsgToUser )
        {
            m_Engine.sendToGuiStatusMessage( "Query Host Online Id Connect FAILED (%3.3f sec)", testTimer.elapsedSec() );
        }

        return eNetCmdErrorConnectFailed;
    }

    std::string strContent("0");

    LogMsg( LOG_INFO, "Query Host Online Id Connected in  %3.3f sec now Sending data thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
    testTimer.startTimer();
	setIsQueryUrlActive( true );
    RCODE rc = m_NetServiceUtils.buildAndSendCmd( netServConn, eNetCmdQueryHostOnlineIdReq, strContent, eNetCmdErrorNone );
    if( rc )
    {
        LogMsg( LOG_ERROR, "Query Host Online Id Send Command Error (%3.3f sec) thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
        if( sendMsgToUser )
        {
            m_Engine.sendToGuiStatusMessage( "Query Host Online Id Send Command Error (%3.3f sec)", testTimer.elapsedSec() );
        }

        return eNetCmdErrorTxFailed;
    }

    VxSleep( 100 );
    char rxBuf[ 513 ];
    rxBuf[ 0 ] = 0;
    NetServiceHdr netServiceHdr;
    if( false == m_NetServiceUtils.rxNetServiceCmd( eNetCmdQueryHostOnlineIdReply,
													netServConn,
                                                    rxBuf,
                                                    sizeof( rxBuf ) - 1,
                                                    netServiceHdr,
                                                    IS_PORT_OPEN_RX_HDR_TIMEOUT,
                                                    IS_PORT_OPEN_RX_DATA_TIMEOUT ) )
    {
        if( sendMsgToUser )
        {
            sendUserLog( "Query Host Online Id Server Response Time Out (%3.3f sec) thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
        }

        m_Engine.sendToGuiStatusMessage( "Query Host Online Id Server Response Time Out (%3.3f sec)", testTimer.elapsedSec() );
		setIsQueryUrlActive( false );
        return eNetCmdErrorResponseTimedOut;
    }

    if( netServiceHdr.getError() )
    {
        LogMsg( LOG_ERROR, "Query Host Online Id Server Response Has Error %d (%3.3f sec) thread 0x%x", netServiceHdr.getError(), testTimer.elapsedSec(), VxGetCurrentThreadId() );
        if( sendMsgToUser )
        {
            sendUserLog( "Query Host Online Id Server ResponseHas Error Code %d (%3.3f sec)", netServiceHdr.getError(), testTimer.elapsedSec() );
        }

		setIsQueryUrlActive( false );
        return eNetCmdErrorNone;
    }

	setIsQueryUrlActive( false );
    rxBuf[ sizeof( rxBuf ) - 1 ] = 0;
    std::string content = rxBuf;
    if( 0 == content.length() )
    {
        LogMsg( LOG_ERROR, "Query Host Online Id Server Response Content (%3.3f sec) thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
        if( sendMsgToUser )
        {
            sendUserLog( "Query Host Online Id Server Response Content (%3.3f sec)", testTimer.elapsedSec() );
        }

        return eNetCmdErrorRxFailed;
    }

    const char* contentBuf = content.c_str();
    if( '/' != contentBuf[ content.length() - 1 ] )
    {
        LogMsg( LOG_ERROR, "Query Host Online Id Invalid Response Content (%3.3f sec) thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
        if( sendMsgToUser )
        {
            sendUserLog( "Query Host Online Id Invalid Response Content (%3.3f sec)", testTimer.elapsedSec() );
        }

        return eNetCmdErrorInvalidContent;
    }

    ( ( char * )contentBuf )[ content.length() - 1 ] = 0;

    VxGUID hostId;
    hostId.fromVxGUIDHexString( content.c_str() );
    if( !hostId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "Query Host Online Id Invalid Content (%3.3f sec) thread 0x%x", testTimer.elapsedSec(), VxGetCurrentThreadId() );
        if( sendMsgToUser )
        {
            sendUserLog( "Query Host Online Id Invalid Content (%3.3f sec)", testTimer.elapsedSec() );
        }

        return eNetCmdErrorInvalidContent;
    }

    retHostId = hostId;
    return eNetCmdErrorNone;
}

//============================================================================
RCODE NetServicesMgr::handleNetCmdQueryHostIdReply( std::shared_ptr<VxSktBase>& sktBase, NetServiceHdr& netServiceHdr )
{
    RCODE rc = -1;
    // not called here.. handled in NetAction
    return rc;
}

//============================================================================
void NetServicesMgr::netActionResultQueryHostId( ENetCmdError eAppErr, VxGUID& hostId )
{
    if( m_pfuncQueryHostIdCallbackHandler )
    {
        m_pfuncQueryHostIdCallbackHandler( m_QueryHostIdCallbackUserData, eAppErr, hostId );
    }
}

//============================================================================
bool NetServicesMgr::fetchExternalIpAddress( VxSktConnectSimple* sktSimple, std::string& retExternIpAddr, int receiveTimeout )
{
	if( !sktSimple || !sktSimple->isConnected() )
	{
		LogMsg( LOG_ERROR, "fetchExternalIpAddress null or not connected socket" );
		return false;
	}

	std::string strNetCmd;
	std::string strPing = "PING";

	std::string netServChallengeHash;
	uint16_t cryptoKeyPort = sktSimple->getCryptoKeyPort();
	m_NetServiceUtils.generateNetServiceChallengeHash( netServChallengeHash, cryptoKeyPort, getNetworkKey() );
	m_NetServiceUtils.buildNetCmd( strNetCmd, eNetCmdHostPing, netServChallengeHash, strPing );

	bool wasSent = m_NetServiceUtils.sendNetServiceRequest( eNetCmdHostPing, sktSimple, strNetCmd, 8000 );
	if( !wasSent )
	{
        LogMsg( LOG_VERBOSE, "fetchExternalIpAddress sendData error" );
		return false;
	}

	char rxBuf[513];
	rxBuf[0] = 0;
	NetServiceHdr netServiceHdr;
	if( false == m_NetServiceUtils.rxNetServiceCmd( eNetCmdHostPong, sktSimple, rxBuf, sizeof( rxBuf ) - 1, netServiceHdr, receiveTimeout, receiveTimeout ) )
	{
		LogMsg( LOG_VERBOSE, "fetchExternalIpAddress failed recieve response" );
		return false;
	}

	rxBuf[sizeof( rxBuf ) - 1] = 0;

	if( netServiceHdr.m_ContentDataLen <= 0 )
	{
		LogMsg( LOG_VERBOSE, "fetchExternalIpAddress no content recieved" );
		return false;
	}

	if( ( 0 == netServiceHdr.m_TotalDataLen )
		|| ( 511 <= netServiceHdr.m_TotalDataLen )
		|| ( '/' != rxBuf[netServiceHdr.m_ContentDataLen - 1] ) )
	{
		LogMsg( LOG_VERBOSE, "fetchExternalIpAddress invalid content recieved" );
		return false;
	}

	rxBuf[netServiceHdr.m_ContentDataLen - 1] = 0;
	std::string content = rxBuf;
	// strip PONG-
	size_t pongPos = content.find( "PONG-" );
	if( pongPos == std::string::npos )
	{
		LogMsg( LOG_VERBOSE, "fetchExternalIpAddress PONG not found" );
		return false;
	}

	std::string ipAddr = content.substr( pongPos + 5 );
	if( ipAddr.empty() )
	{
		LogMsg( LOG_VERBOSE, "fetchExternalIpAddress Empty PONG content" );
		return false;
	}

	if( !VxIsIPv4Address( ipAddr.c_str() ) && !VxIsIPv6Address( ipAddr.c_str() )  )
	{
		LogMsg( LOG_VERBOSE, "fetchExternalIpAddress Invalid ip addr %s", ipAddr.c_str() );
		return false;
	}

	retExternIpAddr = ipAddr;

	return true;
}

//============================================================================
bool NetServicesMgr::shouldHandleNetServicePacket( void )
{
	return getIsTestConnectionActive() ||
		m_Engine.getMyPktAnnounce().isPluginEnabled( ePluginTypeHostNetwork ) ||
		m_Engine.getMyPktAnnounce().isPluginEnabled( ePluginTypeHostConnectTest );
}

//============================================================================
bool NetServicesMgr::getNetPktRxCryptoPassword( std::string& retPwd, std::shared_ptr<VxSktBase>& sktBase )
{
	if( sktBase->isAcceptSocket() )
	{
		return NetServiceUtils::generateNetPktCryptoPassword( retPwd, getNetworkKey(), getRxNetServicePort(), "0.0.0.0" );
	}
	else
	{
        return NetServiceUtils::generateNetPktCryptoPassword( retPwd, getNetworkKey(), getRxNetServicePort(), getRxNetIpAddress(sktBase->getIsIpv6Connection()) );
	}
}

//============================================================================
bool NetServicesMgr::handlePktNetService( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, bool& permissionError )
{
	permissionError = false;
	bool status{ false };
	switch( pktHdr->getPktType() )
	{
	case PKT_TYPE_TEST_CONN_TEST_REQ:
		status = handlePktConnTestReq( sktBase, (PktTestConnTestReq*)pktHdr, permissionError );
		break;
	case PKT_TYPE_TEST_CONN_TEST_REPLY:
		status = handlePktConnTestReply( sktBase, (PktTestConnTestReply*)pktHdr, permissionError );
		break;
	case PKT_TYPE_TEST_CONN_PING_REQ:
		status = handlePktConnPingReq( sktBase, (PktTestConnPingReq*)pktHdr, permissionError );
		break;
	case PKT_TYPE_TEST_CONN_PING_REPLY:
		status = handlePktConnPingReply( sktBase, (PktTestConnPingReply*)pktHdr, permissionError );
		break;
	case PKT_TYPE_QUERY_HOST_URL_REQ:
		status = handlePktQueryHostUrlReq( sktBase, (PktQueryHostUrlReq*)pktHdr, permissionError );
		break;
	case PKT_TYPE_QUERY_HOST_URL_REPLY:
		status = handlePktQueryHostUrlReply( sktBase, (PktQueryHostUrlReply*)pktHdr, permissionError );
		break;
	default:
		permissionError = true;
	}

	return status && !permissionError;
}

//============================================================================
bool NetServicesMgr::handlePktConnTestReq( std::shared_ptr<VxSktBase>& sktBase, PktTestConnTestReq* pkt, bool& permissionError )
{
	if( !m_Engine.getMyPktAnnounce().isPluginEnabled( ePluginTypeHostConnectTest ) )
	{
		permissionError = true;
		return false;
	}

	bool status{ false };
	std::string netCmd;
	if( pkt->getNetCmd( netCmd ) )
	{
		NetServiceHdr netServiceHdr;
		std::string netCmdContent;
		EPluginType pluginType = NetServiceUtils::parseHttpNetService( (char *)netCmd.c_str(), (int)netCmd.length(), netServiceHdr, netCmdContent );
        if( ePluginTypeHostConnectTest == pluginType && eNetCmdIsMyPortOpenReq == netServiceHdr.m_NetCmdType )
		{
			status = ( 0 == handleNetCmdIsMyPortOpenReqContent( sktBase, netServiceHdr, netCmdContent ) );
		}
		else
		{
			LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnPingReq invalid net service command" );
		}
	}

	return status;
}

//============================================================================
bool NetServicesMgr::handlePktConnTestReply( std::shared_ptr<VxSktBase>& sktBase, PktTestConnTestReply* pkt, bool& permissionError )
{
	// should never happen.. only sent and recieved from simple socket
	permissionError = true;
	LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnTestReply rxed pkt" );
	return false;
}

//============================================================================
bool NetServicesMgr::handlePktConnPingReq( std::shared_ptr<VxSktBase>& sktBase, PktTestConnPingReq* pkt, bool& permissionError )
{
	if( !getIsTestConnectionActive() && !m_Engine.getPluginPermission( ePluginTypeHostConnectTest ) && !m_Engine.getMyPktAnnounce().isPluginEnabled( ePluginTypeHostNetwork ) )
	{
		permissionError = true;
		LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnPingReq when not actively testing connection or hosting connection test" );
		return false;
	}

	if( !sktBase->isConnected() )
	{
		LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnPingReq skt disconnected" );
		return false;
	}

	bool status{ false };
	std::string netCmd;
	if( pkt->getNetCmd( netCmd ) )
	{
		NetServiceHdr netServiceHdr;
		EPluginType pluginType = NetServiceUtils::parseHttpNetServiceHdr( (char *)netCmd.c_str(), (int)netCmd.length(), netServiceHdr );
        if( eNetCmdHostPing == netServiceHdr.m_NetCmdType || eNetCmdClientPing == netServiceHdr.m_NetCmdType)
		{
			status = buildAndSendPktTestConnPingReply( sktBase,  eNetCmdClientPing == netServiceHdr.m_NetCmdType );
		}
		else
		{
			LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnPingReq invalid net service command" );
		}
	}

	return status;
}

//============================================================================
bool NetServicesMgr::handlePktConnPingReply( std::shared_ptr<VxSktBase>& sktBase, PktTestConnPingReply* pkt, bool& permissionError )
{
	// should never happen.. only sent and recieved from simple socket
	permissionError = true;
	LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnPingReply rxed pkt" );
	return false;
}

//============================================================================
bool NetServicesMgr::handlePktQueryHostUrlReq( std::shared_ptr<VxSktBase>& sktBase, PktQueryHostUrlReq* pkt, bool& permissionError )
{
	if( !m_Engine.getMyPktAnnounce().isPluginEnabled( ePluginTypeHostConnectTest ) && !m_Engine.getMyPktAnnounce().isPluginEnabled( ePluginTypeHostNetwork ) )
	{
		// only network host and connection test should respond to url query
		permissionError = true;
		return false;
	}

	bool status{ false };
	std::string netCmd;
	if( pkt->getNetCmd( netCmd ) )
	{
		NetServiceHdr netServiceHdr;
		EPluginType pluginType = NetServiceUtils::parseHttpNetServiceHdr( (char *)netCmd.c_str(), (int)netCmd.length(), netServiceHdr );
        if( eNetCmdQueryHostOnlineIdReq == netServiceHdr.m_NetCmdType )
		{
			status = buildAndSendPktQueryUrlReply( sktBase );
		}
		else
		{
			LogMsg( LOG_ERROR, "NetServicesMgr::handlePktConnPingReq invalid net service command" );
		}
	}

	return status;
}

//============================================================================
bool NetServicesMgr::handlePktQueryHostUrlReply( std::shared_ptr<VxSktBase>& sktBase, PktQueryHostUrlReply* pkt, bool& permissionError )
{
	// should never happen.. only sent and recieved from simple socket
	permissionError = true;
	LogMsg( LOG_ERROR, "NetServicesMgr::handlePktQueryHostUrlReply rxed pkt" );
	return false;
}

//============================================================================
bool NetServicesMgr::buildAndSendPktTestConnPingReply( std::shared_ptr<VxSktBase>& sktBase, bool isClientPing )
{
	bool status{ false };

	LogModule( eLogIsPortOpenTest, LOG_INFO, "Got ping from %s ", sktBase->getRemoteIp().c_str(), VxGetCurrentThreadId() );
	std::string content;
    StdStringFormat( content, "PONG-%s", sktBase->getRemoteIp().c_str() );

	ENetCmdType cmdType = isClientPing ? eNetCmdClientPong : eNetCmdHostPong;

	std::string newCmd;
	if( getNetUtils().buildCmd( newCmd, sktBase, cmdType, content ) )
	{
		status = sendNetServicePacket( cmdType, sktBase, newCmd, 5000 );
	}

	return status;
}

//============================================================================
bool NetServicesMgr::buildAndSendPktQueryUrlReply( std::shared_ptr<VxSktBase>& sktBase )
{
	bool status{ false };

	VxGUID myId = m_Engine.getMyOnlineId();
	std::string hexMyId;
    if( !myId.toHexString( hexMyId ) )
    {
        LogModule( eLogNetService, LOG_ERROR, "NetServicesMgr::handlePktQueryHostUrlReq: Access Denied.. Invalid My Online Id" );
		return false;
    }
	
	std::string newCmd;
	if( getNetUtils().buildCmd( newCmd, sktBase, eNetCmdQueryHostOnlineIdReply, hexMyId ) )
	{
		status = sendNetServicePacket( eNetCmdQueryHostOnlineIdReply, sktBase, newCmd, 5000 );
	}

	return status;
}

//============================================================================
bool NetServicesMgr::sendNetServicePacket(	ENetCmdType         netCmdRequestType, ///< which type of net service to request
											std::shared_ptr<VxSktBase>& sktBase,
											std::string&		 netCmd,
											int                 txDataTimeout )
{
	if( netCmd.length() < MIN_NET_CMD_LEN || netCmd.length() > MAX_NET_CMD_LEN )
	{
		LogMsg( LOG_ERROR, "NetActionAnnounce::sendNetServiceRequest: invalid net cmd len %d type %s cmd %s", netCmd.length(), DescribeNetCmdType( netCmdRequestType ), netCmd.c_str() );
		return false;
	}

	std::unique_ptr<VxPktHdr> pktPtr;
	std::string cryptoPwd;
	uint16_t keyPort = sktBase->isAcceptSocket() ? getRxNetServicePort() : sktBase->getRemotePort();
    std::string keyIpAddr = sktBase->isAcceptSocket() ? getRxNetIpAddress(sktBase->getIsIpv6Connection())  : sktBase->getRemoteIpAddress();
	if( eNetCmdClientPong == netCmdRequestType )
	{
		//keyPort = m_Engine.getNetworkStateMachine().getNetServicePort();
		keyIpAddr = "0.0.0.0";
	}

	NetServiceUtils::generateNetPktCryptoPassword( cryptoPwd, getNetworkKey(), keyPort, keyIpAddr );

	if( eNetCmdHostPing == netCmdRequestType ||  eNetCmdClientPing == netCmdRequestType )
	{
		PktTestConnPingReq* pkt = new PktTestConnPingReq();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdHostPong == netCmdRequestType || eNetCmdClientPong == netCmdRequestType )
	{
		PktTestConnPingReply* pkt = new PktTestConnPingReply();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdClientPing == netCmdRequestType )
	{
		PktTestConnPingReq* pkt = new PktTestConnPingReq();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdClientPong == netCmdRequestType )
	{
		PktTestConnPingReply* pkt = new PktTestConnPingReply();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdIsMyPortOpenReq == netCmdRequestType )
	{
		PktTestConnTestReq* pkt = new PktTestConnTestReq();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdIsMyPortOpenReply == netCmdRequestType )
	{
		PktTestConnTestReply* pkt = new PktTestConnTestReply();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdQueryHostOnlineIdReq == netCmdRequestType )
	{
		PktQueryHostUrlReq* pkt = new PktQueryHostUrlReq();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else if( eNetCmdQueryHostOnlineIdReply == netCmdRequestType )
	{
		PktQueryHostUrlReply* pkt = new PktQueryHostUrlReply();
		pkt->setNetCmd( netCmd );
		pktPtr.reset( pkt );
	}
	else
	{
		LogMsg( LOG_ERROR, "NetServicesMgr::sendNetServicePacket: unknown net cmd request %d", netCmdRequestType );
		return false;
	}

	bool wasSent{ false };
	int pktLen = pktPtr->getPktLength();
	if( pktLen && !cryptoPwd.empty() )
	{
		uint8_t* pktData = (uint8_t*)pktPtr.get();

		std::unique_ptr<VxCrypto> txCrypto = std::make_unique<VxCrypto>();
		txCrypto->setPassword( cryptoPwd.c_str(), cryptoPwd.length() );
		if( 0 == txCrypto->encrypt( pktData, pktLen ) )
		{
			RCODE rc = sktBase->sendData( (char *)pktData, pktLen, txDataTimeout );
			wasSent = 0 == rc;
			if( !wasSent )
			{
				LogMsg( LOG_ERROR, "NetServicesMgr::sendNetServicePacket: sendData failed timout %d error %d %s", txDataTimeout, rc, VxDescribeSktError( rc ) );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "NetServicesMgr::sendNetServicePacket: could not encrypt len %d", pktLen );
			return false;
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "NetServicesMgr::sendNetServicePacket: invalid pkt len %d or cryptoPwdEmpty", pktLen );
		return false;
	}

	LogModule( eLogNetService, LOG_VERBOSE, "NetServicesMgr::sendNetServicePacket: wasSent %d for port %d ip %s cmd %s", wasSent, keyPort, keyIpAddr.c_str(), netCmd.c_str());
	return wasSent;
}

//============================================================================
void NetServicesMgr::sendUserLog( const char* msg, ... )
{
	std::array<char, 1024> as8Buf;
	va_list argList;
	va_start( argList, msg );
	vsnprintf( as8Buf.data(), as8Buf.size(), msg, argList);
	as8Buf[ as8Buf.size() - 1 ] = 0;
	va_end( argList );
	IToGui::getIToGui().toGuiIsPortOpenStatus( eIsPortOpenStatusLogMsg, as8Buf.data() );
}
