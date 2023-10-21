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

#include "VxClientMgr.h"
#include "VxSktConnect.h"

//============================================================================
VxClientMgr::VxClientMgr()
{
	m_eSktMgrType = eSktMgrTypeTcpConnect;
}

//============================================================================
void VxClientMgr::sktMgrStartup( void )
{
	// empty
}

//============================================================================
//! make a new socket... give derived classes a chance to override
std::shared_ptr<VxSktBase> VxClientMgr::makeNewSkt( void )
{ 
	std::shared_ptr<VxSktBase> sharedSkt( new VxSktConnect() );
	sharedSkt->setThisSkt( sharedSkt ); // so skt can do callbacks without look up in manager
	return sharedSkt;
}

//============================================================================
//! Connect to ip or url and return socket.. if cannot connect return NULL
std::shared_ptr<VxSktBase> VxClientMgr::connectTo(	const char*		pIpOrUrl,				// remote ip or url 
													uint16_t		u16Port,				// port to connect to
													int				iTimeoutMilliSeconds )	// milli seconds before connect attempt times out
{
	if( NULL ==  m_pfnUserReceive )
	{
		LogMsg( LOG_INFO, "VxClientMgr::VxConnectTo: you must call setReceiveCallback first" );
		vx_assert( m_pfnUserReceive );
	}

	std::shared_ptr<VxSktBase> sktBase = makeNewSkt();
	sktBase->m_SktMgr		= this;
	RCODE rc = sktBase->connectTo(	m_LclIp,
									pIpOrUrl, 
									u16Port, 
									iTimeoutMilliSeconds );
	if( rc )
	{
		LogModule( eLogSkt, LOG_INFO, "VxClientMgr::VxConnectTo: error %d\n", rc );

		sktBase.reset();
	}
	else
	{
		addSkt( sktBase );
	}

	return sktBase;
}
