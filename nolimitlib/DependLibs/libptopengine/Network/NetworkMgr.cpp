//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkMgr.h"
#include "NetworkDefs.h"

#include <P2PEngine/P2PEngine.h>
#include <P2PEngine/P2PConnectList.h>
#include <BigListLib/BigListMgr.h>
#include <BigListLib/BigListInfo.h>
#include <NetworkMonitor/NetworkMonitor.h>

#include <NetLib/VxSktBase.h>
#include <NetLib/VxPeerMgr.h>
#include <NetLib/VxSktConnect.h>
#include <NetLib/VxSktCrypto.h>

#include <PktLib/VxCommon.h>
#include <PktLib/PktAnnounce.h>
#include <PktLib/PktsRelay.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#ifdef _MSC_VER
# pragma warning(disable: 4355) //'this' : used in base member initializer list
#endif

namespace
{
	void NetworkPeerSktCallbackHandler( std::shared_ptr<VxSktBase>&  sktBase, void * pvUserCallbackData )
	{
        if( pvUserCallbackData )
		{
            NetworkMgr * netMgr = (NetworkMgr *)pvUserCallbackData;
            netMgr->handleTcpSktCallback( sktBase );
		}
	}

	void NetworkSktMgrStatusCallbackHandler( const char* statParam, void* statValue, void* pvUserCallbackData )
	{
		if( pvUserCallbackData )
		{
			NetworkMgr* netMgr = (NetworkMgr*)pvUserCallbackData;
			netMgr->handleSktMgrStatusCallback( statParam, statValue );
		}
	}
}

//============================================================================
NetworkMgr::NetworkMgr( P2PEngine&		engine, 
						VxPeerMgr&		peerMgr,
						BigListMgr&		bigListMgr,
						P2PConnectList&	connectionList
						)
: m_Engine( engine )
, m_PktAnn( engine.getMyPktAnnounce() )
, m_PeerMgr( peerMgr )
, m_BigListMgr( bigListMgr )
, m_ConnectList( connectionList )
#if ENABLE_COMPONENT_NEARBY
, m_NearbyMgr( engine, *this )
#endif // ENABLE_COMPONENT_NEARBY
{
	VxSetNetworkLoopbackAllowed( false );

	m_PeerMgr.setReceiveCallback( NetworkPeerSktCallbackHandler, this );
	m_PeerMgr.setSktMgrStatusCallback( NetworkSktMgrStatusCallbackHandler, this );
}

//============================================================================
void NetworkMgr::networkMgrStartup( void )
{
	// set the network key immediately so services like NetService has it available durring startup
	NetHostSetting netHostSettings;
	m_Engine.getEngineSettings().getNetHostSettings( netHostSettings );
	setNetworkKey( netHostSettings.getNetworkKey() );
	m_PeerMgr.sktMgrStartup();
#if ENABLE_COMPONENT_NEARBY
	m_NearbyMgr.networkMgrStartup();
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
void NetworkMgr::networkMgrShutdown( void )
{
	m_PeerMgr.sktMgrShutdown();
#if ENABLE_COMPONENT_NEARBY
	m_NearbyMgr.networkMgrShutdown();
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
void NetworkMgr::setNetworkKey( std::string networkName )					
{ 
	m_NetworkName = networkName;
}

//============================================================================
void NetworkMgr::fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork )
{
    if( !lclIp )
    {
        LogMsg( LOG_SEVERE, "fromGuiNetworkAvailable invalid param lclIp is null" );
        return;
    }

    std::string strIp = lclIp;
    if( strIp.empty() )
    {
        LogMsg( LOG_ERROR, "fromGuiNetworkAvailable param lclIp is empty" );
        return;
    }

    if( m_bNetworkAvailable 
        && ( m_strLocalIpAddr == strIp )
        && ( m_bIsCellularNetwork == isCellularNetwork ) )
    {
        LogModule( eLogNetworkMgr, LOG_DEBUG, "fromGuiNetworkAvailable but network already set to %s\n", m_strLocalIpAddr.c_str() );
        return;
    }

	m_bIsCellularNetwork = isCellularNetwork;
	m_strLocalIpAddr = lclIp;
	m_LocalIp.setIp( lclIp );
    m_bNetworkAvailable = true;

	if( m_LocalIp.isIPv4() && m_LocalIp.isValid() )
	{
#if ENABLE_COMPONENT_NEARBY
		m_Engine.getMyPktAnnounce().getLanIPv4().setIp( lclIp );
#endif // ENABLE_COMPONENT_NEARBY
		m_Engine.getNetStatusAccum().setLanIpAddress( false, m_LocalIp.toStdString() );
	}
	else
	{
		m_Engine.getMyPktAnnounce().getLanIPv4().setToInvalid();
	}

#if ENABLE_COMPONENT_NEARBY
	m_NearbyMgr.fromGuiNetworkAvailable( lclIp, isCellularNetwork );
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
void NetworkMgr::fromGuiNetworkLost( void )
{
	m_bNetworkAvailable =  false ;
#if ENABLE_COMPONENT_NEARBY
	m_NearbyMgr.fromGuiNetworkLost();
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
ENetLayerState NetworkMgr::fromGuiGetNetLayerState( ENetLayerType netLayer )
{
    ENetLayerState netState = eNetLayerStateWrongType;
    if( netLayer == eNetLayerTypeInternet )
    {
        NetworkMonitor&	netMonitor = m_Engine.getNetworkMonitor();

        if( !netMonitor.getIsInitialized() )
        {
            netState = eNetLayerStateUndefined;
        }
        else
        {
            netState = netMonitor.getIsInternetAvailable() ? eNetLayerStateAvailable : eNetLayerStateFailed;
        }
    }

    return netState;
}

//============================================================================
void NetworkMgr::onPktAnnUpdated( void )
{
#if ENABLE_COMPONENT_NEARBY
	m_NearbyMgr.onPktAnnUpdated();
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
void NetworkMgr::onOncePerSecond( void )
{
	if ( VxIsAppShuttingDown() )
	{
		return;
	}
	
#if ENABLE_COMPONENT_NEARBY
	m_NearbyMgr.onOncePerSecond();
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
void NetworkMgr::handleTcpSktCallback( std::shared_ptr<VxSktBase>& sktBase )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	sktBase->setLastActiveTimeMs( GetGmtTimeMs() );

	switch( sktBase->getCallbackReason() )
	{
	case eSktCallbackReasonConnectError:
        LogModule( eLogNetworkMgr, LOG_ERROR, "NetworkMgr:TCP skt %d skt id %d connect error %s thread 0x%x",
                    sktBase->getSktHandle(), sktBase->getSktNumber(), sktBase->describeSktError( sktBase->getLastSktError() ), VxGetCurrentThreadId() );
		break;

	case eSktCallbackReasonConnected:
        LogModule( eLogNetworkMgr, LOG_INFO, "NetworkMgr:TCP skt %d skt id %d %s port %d to local port %d thread 0x%x",
                    sktBase->getSktHandle(), sktBase->getSktNumber(), sktBase->describeSktType().c_str(), sktBase->m_RmtIp.getPort(), sktBase->m_LclIp.getPort(), VxGetCurrentThreadId() );
		break;

	case eSktCallbackReasonData:
		m_Engine.handleTcpData( sktBase );
		break;

	case eSktCallbackReasonClosed:
        LogModule( eLogNetworkMgr, LOG_INFO, "NetworkMgr:TCP skt handle %d num %d id %s closed %s thread 0x%x",
                    sktBase->getSktHandle(), sktBase->getSktNumber(), sktBase->getSocketIdText().c_str(), sktBase->describeSktError( sktBase->getLastSktError() ), VxGetCurrentThreadId() );
		m_Engine.onConnectionLost( sktBase );
		break;

	case eSktCallbackReasonError:
 		LogModule( eLogNetworkMgr, LOG_ERROR, "NetworkMgr:TCP skt %d skt id %d error %s thread 0x%x",
                    sktBase->getSktHandle(), sktBase->getSktNumber(), sktBase->describeSktError( sktBase->getLastSktError() ), VxGetCurrentThreadId() );
		break;

	case eSktCallbackReasonClosing:
        LogModule( eLogNetworkMgr, LOG_INFO, "NetworkMgr:TCP eSktCallbackReasonClosing skt handle %d num %d id %s thread 0x%x", 
				   sktBase->getSktHandle(), sktBase->getSktNumber(), sktBase->getSocketIdText().c_str(), VxGetCurrentThreadId() );
		m_Engine.onConnectionClosing( sktBase ); 
		break;

	case eSktCallbackReasonConnecting:
        LogModule( eLogNetworkMgr, LOG_INFO, "NetworkMgr:TCP eSktCallbackReasonConnecting skt %d skt id %d thread 0x%x", sktBase->getSktHandle(), sktBase->getSktNumber(), VxGetCurrentThreadId() );
		break;

	default:
		LogMsg( LOG_ERROR, "NetworkMgrTCP: UNKNOWN CallbackReason %d skt %d skt id %d error %s thread 0x%x", 
                sktBase->getCallbackReason(), sktBase->getSktHandle(), sktBase->getSktNumber(), sktBase->describeSktError( sktBase->getLastSktError() ), VxGetCurrentThreadId() );
		break;
	}
}

//============================================================================
void NetworkMgr::handleSktMgrStatusCallback( const char* statParam, void* statValue )
{
	if( statParam )
	{
		std::string sktMgrParam( statParam );
		if( !sktMgrParam.empty() )
		{
			m_Engine.sktMgrStatusCallback( sktMgrParam, (int64_t)statValue );
		}
	}
}
