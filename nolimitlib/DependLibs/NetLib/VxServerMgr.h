#pragma once
//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxHackerMgr.h"
#include "VxSktStatMgr.h"
#include "VxSktThrottle.h"
#include "VxSktBaseMgr.h"

#include <CoreLib/VxThread.h>
#include <vector>

// from header in case not defined
#ifndef FD_SETSIZE
#define FD_SETSIZE      64
#endif /* FD_SETSIZE */

// implements a manager to manage accept sockets
class VxServerMgr : public VxSktBaseMgr, public VxHackerMgr, public VxSktStatMgr 
{
public:
    const int MAX_LISTEN_BACKLOG = 4;

	VxServerMgr();
	virtual ~VxServerMgr() = default;


    void				        sktMgrStartup( bool ipv6 ) override;
	void				        sktMgrShutdown( void ) override;

	// overrides SktMgrBase
	//! make a new socket... give derived classes a chance to override
	virtual std::shared_ptr<VxSktBase> makeNewAcceptSkt( bool ipv6 );

    virtual void				setIsReadyToAcceptConnections( bool ipv6, bool isReady );
    virtual bool				getIsReadyToAcceptConnections( bool ipv6 );

    virtual bool				isListening( bool ipv6 );

    void						setUpnpEnable( bool enable );
    bool						getUpnpEnable( void );

    void						setListenPort( uint16_t port );
	uint16_t					getListenPort( void );

    void						setLocalIp( bool ipv6, std::string& newLocalIp );
    std::string				    getLocalIp( bool ipv6, bool* retIsValid = nullptr );

    void                        setListenEnable( bool ipv6, bool enable ); // if not enabled then listen thread will wait until enabled
    bool                        getListenEnable( bool ipv6 ); // if not enabled then listen thread will wait until enabled

    void                        setIsListenParamsChanged( bool ipv6, bool isChanged );
    bool                        getIsListenParamsChanged( bool ipv6 );

    void                        listenSettingsUpdated( bool ipv6, bool forceListenSktRelease = true );

    void                        setListenSkt( bool ipv6, SOCKET skt );
    SOCKET                      getListenSkt( bool ipv6, bool setExistingSktToInvalid = false );
    
	virtual void				lockListenSettings( void ) 					{ m_ListenSettingsMutex.lock(); }
	virtual void				unlockListenSettings( void )				{ m_ListenSettingsMutex.unlock(); }

	void						listenForConnectionsToAccept( bool ipv6, VxThread* poVxThread );

    VxThread					m_ListenThreadIpv6;		// thread to listen for incoming ipv6 connections
    VxThread					m_ListenThreadIpv4;		// thread to listen for incoming ipv4 connections

    int							m_iMgrId{ 0 };			// unique id for this manager

protected:
    virtual RCODE				startListeningThreads( bool ipv6 );
	void						stopListeningThreads( void );

    void                        onListenSettingsChanged( bool ipv6 );
    bool                        waitForValidListenSettings( bool ipv6 );

    bool						isListenParamsValid( bool ipv6 );

    bool                        createNewListenSocket( bool ipv6, uint16_t listenPort, SOCKET& retListenSock, std::string lclIP );

    RCODE 						acceptConnection( bool ipv6, VxThread* poVxThread, SOCKET oListenSkt );
    void                        closeListenSocket( bool ipv6 );

    bool                        shouldListenAbort( bool ipv6 );

    bool                        doPortForward( bool ipv6, bool addPort );

    static constexpr size_t     m_u32MaxConnections = 30000;
    static int					m_iAcceptMgrCnt;				    // number of managers created
    RCODE						m_rcLastError = 0;					// last error that occurred

    VxMutex                     m_ListenSettingsMutex;
    uint16_t					m_u16ListenPort = 0;				// what port to listen on

    std::string                 m_LclAddressIpv6;
    std::string                 m_LclAddressIpv4;

    bool                        m_ListenEnabledIpv6{ false };
    bool                        m_ListenEnabledIpv4{ true };

    bool                        m_SettingsChangedIpv6{ false };
    bool                        m_SettingsChangedIpv4{ false };

    bool						m_IsReadyToAcceptConnectionsIpv6{ false };
    bool						m_IsReadyToAcceptConnectionsIpv4{ false };

    SOCKET						m_ListenSktIpv6{ INVALID_SOCKET };
    SOCKET						m_ListenSktIpv4{ INVALID_SOCKET };	   

    int64_t						m_LastWatchdogKickMs{ 0 };
    VxMutex                     m_ListenMutex;
    bool                        m_ListenSktIsBoundToIp{ false };
    bool                        m_IsAndroidOs{ false };

    int64_t						m_LastListenActivityMs{ 0 };
};

