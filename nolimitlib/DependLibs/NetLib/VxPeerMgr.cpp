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

#include "VxPeerMgr.h"
#include "VxSktConnect.h"

#include <CoreLib/VxGUID.h>
#include <PktLib/VxPktHdr.h>

#ifdef TARGET_OS_LINUX
    // turn broke pipe exception on disconnected sockets into error instead
    #include <signal.h>
#endif

namespace
{
	void VxPeerMgrRxCallbackHandler( std::shared_ptr<VxSktBase>&  sktBase, void * pvUserCallbackData )
	{
		VxPeerMgr * peerMgr = ( VxPeerMgr * )pvUserCallbackData;
		if( peerMgr )
		{
			peerMgr->handleSktCallback( sktBase );
		}
	}
}

//============================================================================
static void sigpipe_handler(int unused)
{
    LogMsg( LOG_WARN, "SIGPIPE Error.. probably broke socket connection");
}

//============================================================================
VxPeerMgr::VxPeerMgr()
{
#ifdef TARGET_OS_LINUX
    // turn broke pipe exception on disconnected sockets into error instead
    // signal( SIGPIPE, SIG_IGN );
    sigaction(SIGPIPE, &((struct sigaction){sigpipe_handler}), NULL);
#endif

	setReceiveCallback( VxPeerMgrRxCallbackHandler, this );
}

//============================================================================
VxPeerMgr::~VxPeerMgr()
{
	sktMgrShutdown();
}

//============================================================================
void VxPeerMgr::sktMgrShutdown( void )
{
	stopListening();
	m_ClientMgr.sktMgrShutdown();
	VxSktBaseMgr::sktMgrShutdown();
}

//============================================================================
void VxPeerMgr::setReceiveCallback( VX_SKT_CALLBACK pfnReceive, void* pvUserData )
{
	VxServerMgr::setReceiveCallback( pfnReceive, pvUserData );
	m_ClientMgr.setReceiveCallback( pfnReceive, pvUserData );
}

//============================================================================
void VxPeerMgr::setSktMgrStatusCallback( VX_SKT_MGR_STATUS_CALLBACK pfnSktMgrStatus, void* pvUserData )
{
	VxServerMgr::setSktMgrStatusCallback( pfnSktMgrStatus, pvUserData );
	m_ClientMgr.setSktMgrStatusCallback( pfnSktMgrStatus, pvUserData );
}

//============================================================================
void VxPeerMgr::setLocalIp( InetAddress& newLocalIp )
{
	sktMgrSetLocalIp( newLocalIp );
	m_ClientMgr.sktMgrSetLocalIp( newLocalIp );
}

//============================================================================
//! make a new socket... give derived classes a chance to override
std::shared_ptr<VxSktBase> VxPeerMgr::makeNewSkt( void )
{ 
	std::shared_ptr<VxSktBase> sharedSkt( new VxSktConnect() );
	sharedSkt->setThisSkt( sharedSkt ); // so skt can do callbacks without look up in manager
	return sharedSkt;
}

//============================================================================
// find a socket.. assumes list has been locked
std::shared_ptr<VxSktBase>& VxPeerMgr::findSktBase( const VxGUID& connectId, bool acceptSktsOnly )
{
	if( !connectId.isVxGUIDValid() )
	{
		LogMsg( LOG_ERROR, "VxPeerMgr::findSktBase invalid connectId" );
		return  std::make_shared<VxSktBase>();
	}

	std::shared_ptr<VxSktBase>& sktBase = VxSktBaseMgr::findSktBase( connectId, acceptSktsOnly );
	if( !sktBase )
	{
		sktBase = m_ClientMgr.findSktBase( connectId, acceptSktsOnly );
	}

	return sktBase;
}

//============================================================================
//! Connect to ip or url and return socket.. if cannot connect return NULL
std::shared_ptr<VxSktBase> VxPeerMgr::connectTo(	const char*	pIpOrUrl,				// remote ip or url 
										uint16_t		u16Port,				// port to connect to
										int				iTimeoutMilliSeconds )	// milli seconds before connect attempt times out
{
	if( NULL ==  m_pfnUserReceive )
	{
		LogMsg( LOG_INFO, "VxPeerMgr::VxConnectTo: you must call setReceiveCallback first" );
		vx_assert( m_pfnUserReceive );
	}
		
	std::shared_ptr<VxSktBase> sktBase	= makeNewSkt();
	sktBase->m_SktMgr		= this;
	sktBase->setReceiveCallback( m_pfnOurReceive, this );
	sktBase->setTransmitCallback( m_pfnOurTransmit, this );
	RCODE rc = sktBase->connectTo(	m_LclIp,
									pIpOrUrl, 
									u16Port, 
									iTimeoutMilliSeconds );
	if( rc )
	{
		sktBase.reset();
	}
	else
	{
		addSkt( sktBase );
	}

	return sktBase;
}

//============================================================================
std::shared_ptr<VxSktBase> VxPeerMgr::createConnectionUsingSocket( SOCKET skt, const char* rmtIp, uint16_t port )
{
	if( NULL ==  m_pfnUserReceive )
	{
		LogMsg( LOG_ERROR, "VxPeerMgr::createConnectionUsingSocket: you must call setReceiveCallback first" );
		vx_assert( m_pfnUserReceive );
	}

	std::shared_ptr<VxSktBase> sktBase	= makeNewSkt();
	sktBase->m_SktMgr		= this;
	//VxVerifyCodePtr( m_pfnOurReceive );
	sktBase->setReceiveCallback( m_pfnOurReceive, this );
	sktBase->setTransmitCallback( m_pfnOurTransmit, this );
	sktBase->createConnectionUsingSocket( skt, rmtIp, port );
	addSkt( sktBase );
	LogMsg( LOG_INFO, "VxPeerMgr::createConnectionUsingSocket: done skt id %d rmt ip %s port %d", sktBase->getSktNumber(), rmtIp, port  );
	return sktBase;
}

//============================================================================
void VxPeerMgr::handleSktCallback( std::shared_ptr<VxSktBase>& sktBase )
{
}

//============================================================================
bool VxPeerMgr::txPacket(	std::shared_ptr<VxSktBase>&			sktBase,
							const VxGUID&		destOnlineId,
							VxPktHdr*			pktHdr, 				
							bool				bDisconnect )
{
	pktHdr->setDestOnlineId( destOnlineId );
	return txPacketWithDestId( sktBase, pktHdr, bDisconnect );
}

//============================================================================
bool VxPeerMgr::txPacketWithDestId(	std::shared_ptr<VxSktBase>&			sktBase,
									VxPktHdr*			pktHdr, 		
									bool				bDisconnect )
{
    if( !sktBase || false == isSktActive( sktBase ) )
	{

		if( false == m_ClientMgr.isSktActive( sktBase ) )
		{
            LogMsg( LOG_ERROR, "ERROR VxPeerMgr::txPacketWithDestId: skt no longer active" );
			if( sktBase && sktBase->isConnected() )
			{
				sktBase->setIsConnected( false );
				sktBase->closeSkt( eSktCloseTxFailed );
			}
		}

        return false;
	}

    if( sktBase->isConnected() )
    {
        RCODE rc = sktBase->txPacketWithDestId( pktHdr, bDisconnect );
        if( 0 != rc )
        {
            LogMsg( LOG_VERBOSE, "VxPeerMgr::txPacketWithDestId: skt %d returned error %d %s", sktBase->getSktNumber(), rc, sktBase->describeSktError( rc ) );
			if( sktBase->isFatalSocketError( rc ) )
			{
				sktBase->setIsConnected( false );
				sktBase->closeSkt( eSktCloseTxFailed );
			}
        }

        return  ( 0 == rc );
    }
    else
    {
        LogMsg( LOG_ERROR, "ERROR VxPeerMgr::txPacketWithDestId: skt no longer connected" );
    }

    return false;
}

//============================================================================
void VxPeerMgr::dumpSocketStats( const char*reason, bool fullDump )
{
    std::string reasonMsg = reason ? reason : "";
    VxSktBaseMgr::dumpSocketStats( std::string( reasonMsg + " server: " ).c_str(), fullDump );
    m_ClientMgr.dumpSocketStats( std::string( reasonMsg + " client: " ).c_str(), fullDump );
}

//============================================================================
bool VxPeerMgr::closeConnection( VxGUID& connectId, ESktCloseReason closeReason )
{
	bool wasClosed = m_ClientMgr.closeConnection( connectId, closeReason );
	if( !wasClosed )
	{
		wasClosed = VxSktBaseMgr::closeConnection( connectId, closeReason );
	}

	return wasClosed;
}
