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

#include <CoreLib/VxDefs.h>

#include <GuiInterface/IToGui.h>
#include <GuiInterface/IFromGui.h>

class P2PEngine;
class NetworkStateMachine;
class NetServicesMgr;
class VxNetIdent;
class PktAnnounce;
class VxConnectInfo;
class PktRelayServiceReply;
class VxSktBase;

class NetworkStateBase
{
public:
	NetworkStateBase( NetworkStateMachine& stateMachine );
    virtual ~NetworkStateBase(){};

	virtual ENetworkStateType	getNetworkStateType( void )									{ return m_ENetworkStateType; }
	virtual void				setNetworkStateType( ENetworkStateType eStateType )			{ m_ENetworkStateType = eStateType; }

	virtual void				enterNetworkState( void )									{};
	virtual void				runNetworkState( void )										{};
	virtual void				exitNetworkState( void )									{};

	virtual void				fromGuiUserLoggedOn( VxNetIdent* netIdent )				{};
	virtual void				fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork = false ){};
	virtual void				fromGuiNetworkLost( void )									{};
    virtual ENetLayerState	    fromGuiGetNetLayerState( ENetLayerType netLayer = eNetLayerTypeInternet ) { return eNetLayerStateUndefined; }

	virtual void				onPktRelayServiceReply( std::shared_ptr<VxSktBase>& sktBase, PktRelayServiceReply * pkt ) {};

	virtual void				checkAndHandleNetworkEvents( void );

protected:
	bool						checkForAbortOrShutdown( void );
	bool						networkStateChangePending( void );

	//=== vars ===//
	P2PEngine&					m_Engine;
	PktAnnounce&				m_PktAnn;
	NetServicesMgr&				m_NetServicesMgr;
	NetworkStateMachine&		m_NetworkStateMachine;
	ENetworkStateType			m_ENetworkStateType;
};


