//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateMachine.h"
#include "NetworkStateLost.h"
#include "NetworkStateAvail.h"
#include "NetworkStateIpChange.h"
#include "NetworkStateTestConnection.h"
#include "NetworkStateOnlineDirect.h"
#include "NetworkStateOnlineThroughRelay.h"
#include "NetworkStateWaitForRelay.h"

#include "NetworkEventBase.h"
#include "NetworkEventAvail.h"
#include "NetworkEventLost.h"

#include <Network/NetworkMgr.h>
#include <Network/NetConnector.h>
#include <NetServices/NetServicesMgr.h>

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxSktUtil.h>
#include <CoreLib/VxResolveHost.h>
#include <NetLib/VxPeerMgr.h>
#include <NetLib/VxPortForward.h>

#include <time.h>
#include <atomic>


#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif

namespace
{
	//============================================================================
    void * NetworkStateMachineThreadFunc( void * pvContext )
	{
        static std::atomic_int threadCnt(0);
#ifdef DEBUG_PTOP_NETWORK_STATE
        LogMsg( LOG_INFO, "NetworkStateMachineThreadFunc thread starting %d", threadCnt.load() );
#endif // DEBUG_PTOP_NETWORK_STATE
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );
		NetworkStateMachine * stateMachine = (NetworkStateMachine *)poThread->getThreadUserParam();
        if( stateMachine && false == poThread->isAborted() )
		{
			stateMachine->runStateMachineThread();
		}

		poThread->threadAboutToExit();
        threadCnt--;
#ifdef DEBUG_PTOP_NETWORK_STATE
        LogMsg( LOG_INFO, "NetworkStateMachineThreadFunc Thread exiting %d", threadCnt.load() );
#endif // DEBUG_PTOP_NETWORK_STATE
        return nullptr;
	}
}

//============================================================================
NetworkStateMachine::NetworkStateMachine(	P2PEngine& engine,
											NetworkMgr&	networkMgr )
: m_Engine( engine )
, m_EngineSettings( engine.getEngineSettings() )
, m_PktAnn( engine.getMyPktAnnounce() )
, m_NetworkMgr( networkMgr )
, m_NetServicesMgr( engine.getNetServicesMgr() )
, m_NetConnector( engine.getNetConnector() )
//, m_NetPortForward( *this )
, m_DirectConnectTester( *this )
{
    for( int i = 0; i < eMaxNetLayerType; i++ )
    {
        m_NetLayerStates[ i ] = eNetLayerStateUndefined;
    }
}

//============================================================================
NetworkStateMachine::~NetworkStateMachine()
{
	stateMachineShutdown();
}

//============================================================================
void NetworkStateMachine::stateMachineStartup( void )
{	
	if( m_StateMachineInitialized )
	{
		stateMachineShutdown();
	}

	startupNetworkModules();

	m_NetworkStateLost = new NetworkStateLost(  *this );
	m_CurNetworkState = m_NetworkStateLost;
	m_NetworkStateList.emplace_back( m_NetworkStateLost );
	m_NetworkStateList.emplace_back( new NetworkStateAvail( *this ) );
	m_NetworkStateList.emplace_back( new NetworkStateTestConnection( *this ) );
	m_NetworkStateList.emplace_back( new NetworkStateOnlineDirect( *this ) );
	m_NetworkStateList.emplace_back( new NetworkStateWaitForRelay( *this ) );
	m_NetworkStateList.emplace_back( new NetworkStateOnlineThroughRelay( *this ) );
	m_StateMachineInitialized = true;

    m_NetworkStateThread.startThread( (VX_THREAD_FUNCTION_T)NetworkStateMachineThreadFunc, this, "NetworkStateMachineThread" );
}

//============================================================================
void NetworkStateMachine::startupNetworkModules( void )
{
	LogModule( eLogNetworkState, LOG_DEBUG, "NetworkStateMachine::%s", __func__ );
	m_NetworkMgr.networkMgrStartup();

	m_NetServicesMgr.netServicesStartup();
	m_NetConnector.netConnectorStartup();
	m_NetConnector.stayConnectedStartup();
}

//============================================================================
void NetworkStateMachine::stateMachineShutdown( void )
{
	if( m_StateMachineInitialized )
	{
		m_StateMachineInitialized = false;
		m_NetworkStateThread.abortThreadRun( true );
		shutdownNetworkModules();
		m_CurNetworkState->exitNetworkState();
		m_NetworkStateThread.killThread();
		destroyNetworkStates();
	}
}

//============================================================================
void NetworkStateMachine::shutdownNetworkModules( void )
{
	//m_NetPortForward.netPortForwardShutdown();
	m_NetConnector.stayConnectedShutdown();
	m_NetConnector.netConnectorShutdown();
	m_NetServicesMgr.netServicesShutdown();
	m_NetworkMgr.networkMgrShutdown();
}

//============================================================================
bool NetworkStateMachine::isP2POnline( void )
{
	return ( eNetworkStateTypeOnlineDirect == m_eCurRunningStateType ) 
			|| ( eNetworkStateTypeOnlineThroughRelay == m_eCurRunningStateType );
}

//============================================================================
bool NetworkStateMachine::shouldAbort( void )
{
	if( ( false == m_StateMachineInitialized )
		|| m_NetworkStateThread.isAborted() 
		|| VxIsAppShuttingDown() )
	{
        if( IsLogEnabled( eLogNetworkState ) )
        {
            if( false == m_StateMachineInitialized )
            {
                LogMsg( LOG_INFO, "NetworkStateMachine::shouldAbort: m_StateMachineInitialized false" );
            }
            else if( m_NetworkStateThread.isAborted() )
            {
                LogMsg( LOG_INFO, "NetworkStateMachine::shouldAbort: m_NetworkStateThread.isAborted()" );
            }
            else
            {
                LogMsg( LOG_INFO, "NetworkStateMachine::shouldAbort: VxIsAppShuttingDown()" );
            }
        }

		return true;
	}

	return false;
}

//============================================================================
bool NetworkStateMachine::isNetworkStateChangePending( void )
{
	if( shouldAbort() )
	{
		return true;
	}

	if( m_eCurRunningStateType != m_CurNetworkState->getNetworkStateType() )
	{
		return true;
	}

	return false;
}

//============================================================================
void NetworkStateMachine::destroyNetworkStates( void )
{
#ifdef DEBUG_PTOP_NETWORK_STATE
    LogMsg( LOG_INFO, "NetworkStateMachine::destroyNetworkStates" );
#endif // DEBUG_PTOP_NETWORK_STATE
	std::vector<NetworkStateBase *>::iterator iter;
	for( iter = m_NetworkStateList.begin(); iter != m_NetworkStateList.end(); ++iter )
	{
		delete *iter;
	}

	m_NetworkStateList.clear();
}

//============================================================================
void NetworkStateMachine::changeNetworkState( ENetworkStateType eNetworkStateType )
{
	m_NetworkStateMutex.lock();
	ENetworkStateType curNetworkStateType = getCurNetworkStateType();
	m_NetworkStateMutex.unlock();
	if( shouldAbort() )
	{
		if( eNetworkStateTypeLost != curNetworkStateType )
		{
			m_NetworkStateMutex.lock();
			m_CurNetworkState = findNetworkState( eNetworkStateTypeLost );
			m_NetworkStateMutex.unlock();
		}	
		
		return;
	}

	if( curNetworkStateType != eNetworkStateType )
	{
		LogModule( eLogNetworkState, LOG_DEBUG, "changeNetworkState %s to %s", DescribeNetworkState( curNetworkStateType ), DescribeNetworkState( eNetworkStateType ) );
		m_NetworkStateMutex.lock();
		m_CurNetworkState = findNetworkState( eNetworkStateType );
		m_NetworkStateMutex.unlock();
		if( !m_CurNetworkState )
		{
			LogMsg( LOG_FATAL, "NULL NetworkState %s to %s", DescribeNetworkState( curNetworkStateType ), DescribeNetworkState( eNetworkStateType ) );
		}
	}
}

//============================================================================
NetworkStateBase * NetworkStateMachine::findNetworkState( ENetworkStateType eNetworkStateType )
{
	std::vector< NetworkStateBase * >::iterator iter;
	for( iter = m_NetworkStateList.begin(); iter != m_NetworkStateList.end(); ++iter )
	{
		if( eNetworkStateType == (*iter)->getNetworkStateType() )
		{
			return (*iter);
		}
	}

	vx_assert( false );
	return NULL;
}

//============================================================================
ENetworkStateType NetworkStateMachine::getCurNetworkStateType( void )
{
	return m_CurNetworkState->getNetworkStateType();
}

//============================================================================
void NetworkStateMachine::runStateMachineThread( void )
{
	while( false == shouldAbort() )
	{
		NetworkStateBase * thisStateToRun = m_CurNetworkState;
		m_eCurRunningStateType = thisStateToRun->getNetworkStateType();

		thisStateToRun->enterNetworkState();
		if( false == shouldAbort() )
		{
			thisStateToRun->runNetworkState();
		}

		if( false == shouldAbort() )
		{
			thisStateToRun->exitNetworkState();
		}
	}
}

//============================================================================
bool NetworkStateMachine::checkAndHandleNetworkEvents( void )
{
    if( shouldAbort() )
    {
        return true;
    }

	if( m_NetworkEventList.size())
	{
		m_NetworkStateMutex.lock();
		NetworkEventBase * eventBase = m_NetworkEventList[0];
		m_NetworkEventList.erase( m_NetworkEventList.begin() );
		m_NetworkStateMutex.unlock();

		if( eNetworkEventTypeLost == eventBase->getNetworkEventType() )
		{
            LogModule( eLogNetworkState, LOG_DEBUG, "checkAndHandleNetworkEvents network event LOST" );
			changeNetworkState( eNetworkStateTypeLost );
		}
		else if( eNetworkEventTypeAvail == eventBase->getNetworkEventType() )
		{
            LogModule( eLogNetworkState, LOG_DEBUG, "checkAndHandleNetworkEvents network event avail" );

			NetworkEventAvail * eventAvail = (NetworkEventAvail *)eventBase;
			eventAvail->runNetworkEvent();
			m_NetworkMgr.fromGuiNetworkAvailable( eventAvail->getLclIp(), eventAvail->getIsCellNetwork() );

			changeNetworkState( eNetworkStateTypeAvail );
		}
	}

	return isNetworkStateChangePending();
}

//============================================================================
void NetworkStateMachine::fromGuiUserLoggedOn( void )
{
    if( IsLogEnabled( eLogNetworkState ) )
    {
        LogMsg( LOG_INFO, "NetworkStateMachine::fromGuiUserLoggedOn\n" );
    }

	m_bUserLoggedOn = true;
}

//============================================================================
void NetworkStateMachine::updateFromEngineSettings( EngineSettings& engineSettings )
{
    // TODO: should probably use a mutex here
	uint16_t u16TcpPort = engineSettings.getTcpIpPort();
	m_PktAnn.setOnlinePort( u16TcpPort );

    m_Engine.getNetStatusAccum().setIpPort( u16TcpPort );
	m_Engine.getPeerMgr().setUpnpEnable( engineSettings.getUseUpnpPortForward() );

	bool ipv6 = m_Engine.getEngineSettings().getUseIpv6();
	EFirewallTestType firewallTestType = engineSettings.getFirewallTestSetting();
	m_Engine.getNetStatusAccum().setFirewallTestType( firewallTestType );
	if( eFirewallTestAssumeNoFirewall == firewallTestType )
	{
		std::string externIp;
		engineSettings.getUserSpecifiedExternIpAddr( externIp, ipv6 );
		if( !externIp.empty() )
		{
			m_Engine.getNetStatusAccum().setDirectConnectTested( true, false, externIp );
		}
	}

	std::string networkKey;
	engineSettings.getNetworkKey( networkKey );
    if( !networkKey.empty() && ( networkKey != m_NetworkMgr.getNetworkKey() ) )
    {
        m_NetworkMgr.setNetworkKey( networkKey.c_str() );
        // will restore only if network key has changed
        m_Engine.getBigListMgr().dbRestoreAll();
    }	
}

//============================================================================
void NetworkStateMachine::restartNetwork( void )
{
	bool isCell		= m_bIsCellNetwork;
	std::string ip	= m_LocalNetworkIp;
    LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::restartNetwork" );

    static bool lastIsCell = false;
    static std::string lastIpFound;
    if( ( lastIpFound != m_LocalNetworkIp )  || ( lastIsCell != isCell ) )
    {
        fromGuiNetworkLost();
        fromGuiNetworkAvailable( ip.c_str(), isCell );
    }
}

//============================================================================
void NetworkStateMachine::fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork )
{
	if( !lclIp )
	{
		LogMsg( LOG_ERROR, "%s invalid param NULL ip", __func__ );
		return;
	}

    bool hasChanged = ( m_LocalNetworkIp != lclIp );
    uint16_t u16TcpPort = m_Engine.getEngineSettings().getTcpIpPort();
    hasChanged |= ( m_PktAnn.getOnlinePort() != u16TcpPort );
    hasChanged |= ( m_bIsCellNetwork != isCellularNetwork );

	if( hasChanged )
	{
		EIpAddrType addrType = VxGetIpAddrType( lclIp );
		if( addrType == eIpAddrTypeUnknown )
		{
			LogMsg( LOG_ERROR, "%s invalid ip %s", __func__, lclIp );
			return;
		}

		m_Engine.getPeerMgr().setUpnpEnable( m_Engine.getEngineSettings().getUseUpnpPortForward() );

		m_LocalNetworkIp = lclIp;
		m_bIsCellNetwork = isCellularNetwork;
		VxSetLclIpAddress( lclIp );

		m_Engine.lockAnnouncePktAccess();
        m_PktAnn.setOnlinePort( u16TcpPort );
		m_Engine.unlockAnnouncePktAccess();

        m_Engine.getToGui().toGuiUpdateMyIdent( &m_PktAnn );
        m_Engine.getNetStatusAccum().setIpPort( u16TcpPort );
        LogModule( eLogNetworkState, LOG_INFO, " fromGuiNetworkAvailable hasChanged %s", m_LocalNetworkIp.c_str() );
    }

    LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::fromGuiNetworkAvailable creating network available event %s", lclIp );

	m_NetworkStateMutex.lock();
	m_NetworkEventList.emplace_back( new NetworkEventAvail( *this, lclIp, isCellularNetwork ) );
	m_NetworkStateMutex.unlock();

}

//============================================================================
void NetworkStateMachine::fromGuiNetworkLost( void )
{
    LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::fromGuiNetworkLost" );
	m_Engine.getPeerMgr().stopListening( m_Engine.getEngineSettings().getUseIpv6() );
	m_LocalNetworkIp = "";
	m_NetworkStateMutex.lock();
	m_NetworkEventList.emplace_back( new NetworkEventLost( *this ) );
	m_NetworkStateMutex.unlock();
}

//============================================================================
ENetLayerState NetworkStateMachine::fromGuiGetNetLayerState( ENetLayerType netLayer )
{
    ENetLayerState netState = eNetLayerStateUndefined;

    LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::fromGuiGetNetLayerState" );
    if( netLayer == eNetLayerTypeInternet )
    {
        netState = getNetworkMgr().fromGuiGetNetLayerState( netLayer );
    }
    else
    {
        // TODO add network layer states to NetworkStateMachine
        netState = eNetLayerStateAvailable;
    }

    return netState;
}

//============================================================================
void NetworkStateMachine::setNetLayerState( ENetLayerType layerType, ENetLayerState layerState )
{
    if( ( layerType >= eNetLayerTypeUndefined ) && ( layerType >= eNetLayerTypeUndefined ) )
    {
        m_NetLayerStates[ layerType ] = layerState;
    }
    else
    {
        LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::setNetLayerState invalid param" );
    }
}

//============================================================================
ENetLayerState NetworkStateMachine::getNetLayerState( ENetLayerType netLayer )
{
    if( ( netLayer >= eNetLayerTypeUndefined ) && ( netLayer >= eNetLayerTypeUndefined ) )
    {
        if( netLayer == eNetLayerTypeInternet )
        {
            setNetLayerState( netLayer, getNetworkMgr().fromGuiGetNetLayerState( netLayer ) );
        }

        // TODO update other layer states

        return m_NetLayerStates[ netLayer ];
    }
    else
    {
        LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::setNetLayerState invalid param" );
        return eNetLayerStateWrongType;
    }
}

//============================================================================
void NetworkStateMachine::fromGuiNetworkSettingsChanged( void )
{
	EFirewallTestType firewallType = m_Engine.getEngineSettings().getFirewallTestSetting();
    m_Engine.getNetStatusAccum().setFirewallTestType( firewallType );

	bool ipv6 = m_Engine.getEngineSettings().getUseIpv6();
	std::string externIp;
	if( eFirewallTestAssumeNoFirewall == firewallType )
	{
		m_Engine.getEngineSettings().getUserSpecifiedExternIpAddr( externIp, ipv6 );
	}

	if( !externIp.empty() )
	{
		EIpAddrType addrType{ eIpAddrTypeUnknown };
		std::string myIp;
		m_Engine.getMyPktAnnounce().getOnlineIpAddress( myIp, addrType );
		if( externIp != myIp )
		{
			m_Engine.getMyPktAnnounce().setOnlineIpAddress( externIp );
		}

		fromGuiNetworkLost();
		m_Engine.getNetStatusAccum().fromGuiNetworkSettingsChanged();
		fromGuiNetworkAvailable( externIp.c_str(), false );
	}
	else if( m_LocalNetworkIp.length() )
	{
		bool isCell = isCellularNetwork();
		std::string ip = m_LocalNetworkIp;
        LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::fromGuiNetworkSettingsChanged" );
		fromGuiNetworkLost();

		m_Engine.getNetStatusAccum().fromGuiNetworkSettingsChanged();
		fromGuiNetworkAvailable( ip.c_str(), isCell );
	}
}

//============================================================================
void NetworkStateMachine::onPktRelayServiceReply( std::shared_ptr<VxSktBase>& sktBase, PktRelayServiceReply * poPkt )
{
	m_CurNetworkState->onPktRelayServiceReply( sktBase, poPkt );
}

//============================================================================
bool NetworkStateMachine::checkForAbortOrShutdown( void )
{
	if( VxIsAppShuttingDown() || m_NetworkStateThread.isAborted() )
	{
		return true;
	}

	//if( 36000 < m_PortForwardTimer.elapsedSec() )
	if( 30 < m_PortForwardTimer.elapsedSec() )
	{
		// time to run upnp again
		startUpnpOpenPort();
	}

	return false;
}

//============================================================================
void NetworkStateMachine::startUpnpOpenPort( void )
{
	if( m_bUserLoggedOn && 
		( eNetworkStateTypeLost != getCurNetworkStateType() ) )
	{
		uint16_t u16Port;
		//LogMsg( LOG_INFO, "NetworkStateMachine::startUpnpOpenPort\n" );

        u16Port = m_Engine.getEngineSettings().getTcpIpPort();
		if( u16Port != m_PktAnn.getOnlinePort() )
		{
            LogModule( eLogNetworkState, LOG_INFO, "startUpnpOpenPort engine port %d different than pkt ann port %d", u16Port, m_PktAnn.getOnlinePort() );
			m_PktAnn.setOnlinePort( u16Port );
		}

		if( m_Engine.getEngineSettings().getUseUpnpPortForward() )
		{
			std::string			lclIp			= m_NetworkMgr.getLocalIpAddress();
			int64_t				timeNow			= GetTimeStampMs();

			if( ( ( timeNow - m_LastUpnpForwardTime ) < 180000 )
				&& ( m_LastUpnpForwardPort == u16Port )
				&& ( m_LastUpnpForwardIp == lclIp ) )
			{
#ifdef DEBUG_PTOP_NETWORK_STATE
                LogMsg( LOG_INFO, "Skipping UNPNP due to not enough elapsed time" );
#endif // DEBUG_PTOP_NETWORK_STATE
			}
			else
			{
				m_LastUpnpForwardTime	= timeNow;
				m_LastUpnpForwardPort	= u16Port;
				m_LastUpnpForwardIp		= lclIp;
				m_PortForwardTimer.startTimer();
				m_PortForwardCompleted = false;
				m_PortForwardSucces = false;

				// m_NetPortForward.beginPortForward( u16Port, lclIp.c_str() );
				m_PortForwardSucces = VxPortForward::addPortForward( false, lclIp, u16Port );

				m_PortForwardCompleted = true;
			}
		}
	}
}

//============================================================================
bool NetworkStateMachine::hasUpnpOpenPortFinished( void )
{
	if( false == m_Engine.getEngineSettings().getUseUpnpPortForward() )
	{
		return true;
	}

	// timeout after 8 seconds
	if( 8 < m_PortForwardTimer.elapsedSec() )
	{
		return true;
	}

	return m_PortForwardCompleted;
	// return m_NetPortForward.hasPortForwardCompleted();
}

//============================================================================
bool NetworkStateMachine::didUpnpOpenPortSucceed( void )
{
	//return m_NetPortForward.didUpnpPortOpen();
	return m_PortForwardSucces;
}

//============================================================================
void NetworkStateMachine::setPktAnnounceWithCanDirectConnect( std::string& myIpAddr, bool requiresRelay )
{
	m_PktAnn.setRequiresRelay( requiresRelay );
	m_PktAnn.m_DirectConnectId.setIpAddress( myIpAddr.c_str() );
	if( false == requiresRelay )
	{
		m_PktAnn.setHasRelay( false );
		if( false == m_PktAnn.m_DirectConnectId.getIpAddress().isValid() )
		{
			vx_assert( m_PktAnn.m_DirectConnectId.getIpAddress().isValid() );
			m_PktAnn.m_DirectConnectId.getIpAddress().setIp( m_LastKnownExternalIpAddr.c_str() );
		}
		else
		{
			m_LastKnownExternalIpAddr = myIpAddr;
		}
	}
}

//============================================================================
bool NetworkStateMachine::resolveWebsiteUrls( void )
{
	std::string networkHostUrl;
	m_EngineSettings.getNetworkHostUrl( networkHostUrl );
	std::string connectTestHostUrl;
	m_EngineSettings.getConnectTestUrl( connectTestHostUrl );
	if( m_bWebsiteUrlsResolved
		&& ( networkHostUrl == m_LastResolvedHostWebsite )
		&& ( connectTestHostUrl == m_LastResolvedConnectTest ) )
	{
		//LogMsg( LOG_INFO, "NetworkStateMachine::resolveWebsiteUrls already resolved\n" );
		return true;
	}
	else
	{
		m_bWebsiteUrlsResolved = false;
	}

	//LogMsg( LOG_INFO, "NetworkStateMachine::resolveWebsiteUrls\n" );
	bool resolveNetHostResult		= resolveUrl( networkHostUrl, m_HostIp, m_u16HostPort );
	bool resolveConnectTestResult	= resolveUrl( connectTestHostUrl, m_NetServiceIp, m_u16NetServicePort );
	EngineParams& engineParams		= m_Engine.getEngineParams();
	if( resolveNetHostResult )
	{
		m_LastResolvedHostWebsite = networkHostUrl;
		engineParams.setLastHostWebsiteUrl( m_LastResolvedHostWebsite );
		engineParams.setLastHostWebsiteResolvedIp( m_HostIp );
	}
	else
	{
		// use last known resolved ip
		std::string lastHostUrl;
		engineParams.getLastHostWebsiteUrl( lastHostUrl );
		if( lastHostUrl == networkHostUrl )
		{
			std::string lastHostIp = "";
			engineParams.setLastHostWebsiteResolvedIp( lastHostIp );
			if( 0 != lastHostIp.length() )
			{
				m_HostIp = lastHostIp;
                resolveNetHostResult = true;
			}
		}
	}

	if( resolveConnectTestResult )
	{
		m_LastResolvedConnectTest = connectTestHostUrl;
		engineParams.setLastConnectTestUrl( m_LastResolvedConnectTest );
		engineParams.setLastConnectTestResolvedIp( m_NetServiceIp );
	}
	else
	{
		// use last known resolved ip
		std::string lastNetServiceUrl;
		engineParams.getLastConnectTestUrl( lastNetServiceUrl );
		if( lastNetServiceUrl == connectTestHostUrl )
		{
			std::string lastNetServiceIp;
			engineParams.getLastConnectTestResolvedIp( lastNetServiceIp );
			if( 0 != lastNetServiceIp.length() )
			{
				m_NetServiceIp = lastNetServiceIp;
				resolveConnectTestResult = true;
			}
		}
	}

	if( resolveNetHostResult && resolveConnectTestResult )
	{
		m_bWebsiteUrlsResolved = true;
		std::string myLclIp = VxGetLclIpAddress();

		m_bHostIpMatch = false;
		if( myLclIp == m_HostIp )
		{
			m_bHostIpMatch = true;
		}

		m_bNetServiceIpMatch = false;
		if( myLclIp == m_NetServiceIp )
		{
			m_bNetServiceIpMatch = true;
		}
	}
	else
	{
        LogModule( eLogNetworkState, LOG_ERROR, "Failed to resolve network websites %s %s", networkHostUrl.c_str(), connectTestHostUrl.c_str() );
	}


	return ( resolveNetHostResult && resolveConnectTestResult );
}

//============================================================================
bool NetworkStateMachine::resolveUrl( std::string& websiteUrl, std::string& retIp, uint16_t& u16RetPort )
{
	return VxResolveHostToIp( websiteUrl.c_str(), retIp, u16RetPort );
}

//============================================================================
void NetworkStateMachine::onOncePerHour( void )
{
    LogModule( eLogNetworkState, LOG_INFO, "NetworkStateMachine::onOncePerHour" );
	//if( isP2POnline() && m_EngineSettings.getUseUpnpPortForward() )
	//{
	//	if( m_NetPortForward.elapsedSecondsLastAttempt() > 60 )
	//	{
	//		m_NetPortForward.beginPortForward( m_PktAnn.getOnlinePort(), m_NetworkMgr.getLocalIpAddress().c_str() );
	//	}
	//}

	//if( isP2POnline() && ( false == m_Engine.getIsMyHostServiceEnabled( eHostServiceNetworkHost ) ) )
	//{
	//	m_NetServicesMgr.announceToHost( getHostIp(), getHostPort() );
	//}
}

//============================================================================
void NetworkStateMachine::externalIpAddressHasChanged( std::string& oldIpAddress, std::string& newIpAddress )
{
	m_Engine.getNetStatusAccum().setExternalIpAddress( newIpAddress );
	m_LocalNetworkIp = "";
	m_NetworkStateMutex.lock();
	m_NetworkStateList.emplace_back( new NetworkStateIpChange( *this, oldIpAddress, newIpAddress ) );
	m_NetworkStateMutex.unlock();
}
