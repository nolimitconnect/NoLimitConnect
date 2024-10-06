#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkStateBase.h"
#include "DirectConnectTester.h"

#include <CoreLib/VxThread.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxTimer.h>

#include <vector>
#include <string>

class P2PEngine;
class EngineSettings;
class PktAnnounce;
class NetworkMgr;
class NetConnector;
class NetworkStateLost;
class VxNetIdent;
class PktRelayServiceReply;
class NetworkEventBase;

class NetworkStateMachine
{
public:
	NetworkStateMachine( P2PEngine& engine,
						 NetworkMgr& networkMgr );
	virtual ~NetworkStateMachine();

	NetworkStateMachine() = delete; // don't allow default constructor
	NetworkStateMachine(const NetworkStateMachine&) = delete; // don't allow copy constructor

	void						stateMachineStartup( void );
	void						stateMachineShutdown( void );

	void						startupNetworkModules( void );
	void						shutdownNetworkModules( void );

	P2PEngine& getEngine( void ) { return m_Engine; }
	PktAnnounce& getMyPktAnnounce( void ) { return m_PktAnn; }
	NetworkMgr& getNetworkMgr( void ) { return m_NetworkMgr; }
	DirectConnectTester& getDirectConnectTester( void ) { return m_DirectConnectTester; }

	bool						isThisNodeHostWebsite( void ) { return m_bHostIpMatch; }
	bool						isThisNodeConnectTest( void ) { return m_bNetServiceIpMatch; }
	bool						isNetworkWebsitesResolved( void ) { return m_bWebsiteUrlsResolved; }
	std::string& getHostIp( void ) { return m_HostIp; }
	uint16_t					getHostPort( void ) { return m_u16HostPort; }
	std::string& getNetServiceIp( void ) { return m_NetServiceIp; }
	uint16_t					getNetServicePort( void ) { return m_u16NetServicePort; }

	bool						isP2POnline( void );

	bool						isUserLoggedOn( void ) { return m_bUserLoggedOn; }

	bool						isCellularNetwork( void ) { return m_bIsCellNetwork; }
	void						setIsRelayServiceConnected( bool connected ) { m_bRelayServiceConnected = connected; }
	bool						isRelayServiceConnected( void ) { return m_bRelayServiceConnected; }

	void						restartNetwork( void );

	void						fromGuiUserLoggedOn( void );
	void						fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork = false );
	void						fromGuiNetworkLost( void );
	virtual ENetLayerState	    fromGuiGetNetLayerState( ENetLayerType netLayer = eNetLayerTypeInternet );

	void						fromGuiNetworkSettingsChanged( void );

	void						onOncePerHour( void );
	void						onPktRelayServiceReply( std::shared_ptr<VxSktBase>& sktBase, PktRelayServiceReply* pkt );

	bool						checkForAbortOrShutdown( void );
	void						changeNetworkState( ENetworkStateType eNetworkStateType );
	ENetworkStateType			getCurNetworkStateType( void );
	void						lockResources( void ) { m_NetworkStateMutex.lock(); }
	NetworkStateBase* getCurNetworkState( void ) { return m_CurNetworkState; }
	void						unlockResources( void ) { m_NetworkStateMutex.unlock(); }

	void						runStateMachineThread( void );

	void						startUpnpOpenPort( void );
	bool						hasUpnpOpenPortFinished( void );
	bool						didUpnpOpenPortSucceed( void );

	void						setPktAnnounceWithCanDirectConnect( std::string& myIpAddr, bool requiresRelay = false );

	bool						shouldAbort( void );
	bool						checkAndHandleNetworkEvents( void );
	bool						isNetworkStateChangePending( void );

	bool						resolveWebsiteUrls( void );
	void						updateFromEngineSettings( EngineSettings& engineSettings );

	void                        setNetLayerState( ENetLayerType layerType, ENetLayerState layerState );
	ENetLayerState              getNetLayerState( ENetLayerType layerType );

	void						externalIpAddressHasChanged( std::string& oldIpAddress, std::string& newIpAddress );

protected:
	void						destroyNetworkStates( void );
	NetworkStateBase* findNetworkState( ENetworkStateType eNetworkStateType );
	bool						resolveUrl( std::string& websiteUrl, std::string& retIp, uint16_t& u16RetPort );

	//=== vars ===//
	P2PEngine&					m_Engine;
	EngineSettings&				m_EngineSettings;
	PktAnnounce&				m_PktAnn;
	NetworkMgr&					m_NetworkMgr;
	NetServicesMgr&				m_NetServicesMgr;
	NetConnector&				m_NetConnector;

	bool						m_StateMachineInitialized{ false };

	//NetPortForward				m_NetPortForward;
	VxTimer						m_PortForwardTimer;

	DirectConnectTester			m_DirectConnectTester;

	bool						m_bUserLoggedOn{ false };
	std::string					m_LocalNetworkIp;
	bool						m_bIsCellNetwork{ false };
	bool						m_bRelayServiceConnected{ false };

	std::vector<NetworkStateBase*>	m_NetworkStateList;
	NetworkStateLost*				m_NetworkStateLost{ nullptr };
	NetworkStateBase*				m_CurNetworkState{ nullptr };
	ENetworkStateType				m_eCurRunningStateType{ eNetworkStateTypeLost };

	VxThread					m_NetworkStateThread;
	VxMutex						m_NetworkStateMutex;

	std::vector<NetworkEventBase*>	m_NetworkEventList;

	std::string					m_HostIp;
	uint16_t					m_u16HostPort{ 0 };
	std::string					m_LastResolvedHostWebsite;

	std::string					m_NetServiceIp;
	uint16_t					m_u16NetServicePort{ 0 };
	std::string					m_LastResolvedConnectTest;

	bool						m_bWebsiteUrlsResolved{ false };
	bool						m_bHostIpMatch{ false };
	bool						m_bNetServiceIpMatch{ false };
	std::string					m_LastKnownExternalIpAddr{ "0.0.0.0" };

	time_t						m_LastUpnpForwardTime{ 0 };
	uint16_t					m_LastUpnpForwardPort{ 0 };
	std::string					m_LastUpnpForwardIp;

	ENetLayerState              m_NetLayerStates[eMaxNetLayerType];

	bool						m_PortForwardCompleted{ false };
	bool						m_PortForwardSucces{ false };
};


