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

enum ENetworkEventType
{
	eNetworkEventTypeUnknown				= 0,
	eNetworkEventTypeLost					= 1,
	eNetworkEventTypeAvail					= 2,

	eMaxNetworkEventType
};

class P2PEngine;
class NetworkStateMachine;
class NetServicesMgr;
class VxNetIdent;
class PktAnnounce;
class VxConnectInfo;
class PktRelayServiceReply;
class VxSktBase;

class NetworkEventBase
{
public:
	NetworkEventBase( NetworkStateMachine& stateMachine );

	virtual ENetworkEventType	getNetworkEventType( void )									{ return m_eNetworkEventType; }
	virtual void				setNetworkEventType( ENetworkEventType eStateType )			{ m_eNetworkEventType = eStateType; }

	bool						checkForAbortOrShutdown( void );

	//=== vars ===//
	P2PEngine&					m_Engine;
	PktAnnounce&				m_PktAnn;
	NetServicesMgr&				m_NetServicesMgr;
	NetworkStateMachine&		m_NetworkStateMachine;
	ENetworkEventType			m_eNetworkEventType;
};


