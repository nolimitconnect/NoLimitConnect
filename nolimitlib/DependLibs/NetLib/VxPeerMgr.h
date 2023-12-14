#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxServerMgr.h"
#include "VxClientMgr.h"

class VxSktConnect;
class VxGUID;
class VxPktHdr;

class VxPeerMgr : public VxServerMgr
{
public:
	VxPeerMgr();
	virtual ~VxPeerMgr();

	void						sktMgrStartup( void ) override;
    void						sktMgrShutdown( void ) override;

	VxClientMgr&				getClientMgr( void )			                            { return m_ClientMgr; }

	virtual void				lockSktList( void ) override								{ m_SktListMutex.lock(); m_ClientMgr.lockSktList(); }
	virtual void				unlockSktList( void ) override								{ m_SktListMutex.unlock(); m_ClientMgr.unlockSktList(); }


	// find a socket.. assumes list has been locked
    virtual std::shared_ptr<VxSktBase>	findSktBase( const VxGUID& connectId, bool acceptSktsOnly = false ) override;

    virtual void				setReceiveCallback( VX_SKT_CALLBACK pfnReceive, void* pvUserData ) override;
	virtual void				setSktMgrStatusCallback( VX_SKT_MGR_STATUS_CALLBACK pfnSktMgrStatus, void* pvUserData ) override;


    virtual std::shared_ptr<VxSktBase>	makeNewSkt( void ) override;

	virtual	void				handleSktCallback( std::shared_ptr<VxSktBase>& sktBase );

	//! Connect to ip or URL and return socket.. if cannot connect return NULL
	virtual std::shared_ptr<VxSktBase>	connectTo(	const char*		pIpOrUrl,						// remote ip or url 
													uint16_t		u16Port,						// port to connect to
													int				iTimeoutMilliSeconds = 1000 );	// seconds before connect attempt times out

	virtual std::shared_ptr<VxSktBase>	createConnectionUsingSocket( SOCKET skt, const char* rmtIp, uint16_t port );

	virtual bool				txPacket(	std::shared_ptr<VxSktBase>&	sktBase,
											const VxGUID&				destOnlineId,			    // online id of destination user
											VxPktHdr*					pktHdr, 				    // packet to send
											bool						bDisconnect = false );	    // if true disconnect after send

	virtual bool				txPacketWithDestId(	std::shared_ptr<VxSktBase>&	sktBase,
													VxPktHdr*					pktHdr, 				// packet to send
													bool						bDisconnect = false );	// if true disconnect after send

    virtual void                dumpSocketStats( const char* reason = nullptr, bool fullDump = false ) override;

    virtual void                setSktLoopback( std::shared_ptr<VxSktBase>& sktLoopback ) override          { m_SktLoopback = sktLoopback; m_ClientMgr.setSktLoopback( sktLoopback ); }

	virtual bool				closeConnection( VxGUID& socketId, ESktCloseReason closeReason ) override;

	virtual void				startListening( bool ipv6, uint16_t port );
	virtual void				stopListening( bool ipv6 );

	virtual bool				isReadyToAcceptConnections( bool ipv6 );

    virtual void				onOncePer30Seconds( VxGUID& myOnlineId ) override;

protected:

	VxClientMgr					m_ClientMgr;
};
