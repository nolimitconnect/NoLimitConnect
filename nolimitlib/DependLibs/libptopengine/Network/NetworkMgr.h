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

#include "NetworkDefs.h"
#include "NearbyMgr.h"

#include <CoreLib/VxDefs.h>
#include <PktLib/VxFriendMatch.h>

#include <GuiInterface/IDefs.h>

class P2PEngine;
class VxPeerMgr;
class BigListMgr;
class P2PConnectList;
class VxNetIdentBase;
class VxSktBase;
class VxGUID;
class InetAddress;
class PktAnnounce;

class NetworkMgr
{
public:
	NetworkMgr( P2PEngine&		engine, 
				VxPeerMgr&		peerMgr,
				BigListMgr&		bigListMgr,
				P2PConnectList&	connectionList );
	virtual ~NetworkMgr() = default;

	P2PEngine&					getEngine( void )											{ return m_Engine; }
	VxPeerMgr&					getPeerMgr( void )											{ return m_PeerMgr; }

	void						setLocalIpAddress( std::string lclIp )						{ m_strLocalIpAddr = lclIp; }
	std::string					getLocalIpAddress( void )									{ return m_strLocalIpAddr; }
	void						setNetworkKey( std::string networkName );
	const char*					getNetworkKey( void )										{ return m_NetworkName.c_str(); }

	void						networkMgrStartup( void );
	void						networkMgrShutdown( void );

	virtual void				fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork = false );
	virtual void				fromGuiNetworkLost( void );
    virtual ENetLayerState	    fromGuiGetNetLayerState( ENetLayerType netLayer = eNetLayerTypeInternet );

    virtual bool				isInternetAvailable( void )									{ return eNetLayerStateAvailable == fromGuiGetNetLayerState( eNetLayerTypeInternet ); }

	virtual bool				isNetworkAvailable( void )									{ return m_bNetworkAvailable; }
	virtual bool				isCellularNetwork( void )									{ return m_bIsCellularNetwork; }

	virtual void				onPktAnnUpdated( void );
	virtual void				onOncePerSecond( void );

	virtual	void				handleTcpSktCallback( std::shared_ptr<VxSktBase>& sktBase );
	virtual	void				handleSktMgrStatusCallback( const char* statParam, void* statValue );

protected:
	P2PEngine&					m_Engine;
    PktAnnounce&				m_PktAnn;
    VxPeerMgr&					m_PeerMgr;
	BigListMgr&					m_BigListMgr;
	P2PConnectList&				m_ConnectList;

	std::string					m_NetworkName;

    bool						m_bNetworkAvailable{ false };
	bool						m_bIsCellularNetwork{ false };
	std::string					m_strLocalIpAddr;
	InetAddress					m_LocalIp;
};

