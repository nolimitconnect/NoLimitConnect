//============================================================================
// Copyright (C) 2024 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetActionResolveNetworkHostUrl.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
NetActionResolveNetworkHostUrl::NetActionResolveNetworkHostUrl( NetServicesMgr& netServicesMgr )
: NetActionBase( netServicesMgr )
{
}


//============================================================================
void NetActionResolveNetworkHostUrl::doAction( void )
{
	// it may already be resolved
	VxGUID hostOnlineId;
	if( m_Engine.getConnectionMgr().getDefaultHostOnlineId( eHostTypeNetwork, hostOnlineId ) && hostOnlineId.isVxGUIDValid() )
	{
		m_Engine.getNetStatusAccum().setNetHostAvail( true );
		return;
	}

	std::string networkHostUrl;
	m_Engine.getEngineSettings().getNetworkHostUrl( networkHostUrl );
	if( networkHostUrl.empty() )
	{
		m_Engine.getNetStatusAccum().setNetHostAvail( false );
		LogMsg( LOG_ERROR, "NetActionResolveNetworkHostUrl:%s Empty networkHostUrl url", __func__ );
		return;
	}

	m_Engine.getConnectionMgr().applyDefaultHostUrl( eHostTypeNetwork, networkHostUrl );

	bool wasResolved = false;
	int waitCnt = 0;

	while( waitCnt < 20 )
	{
		if( m_Engine.getConnectionMgr().getDefaultHostOnlineId( eHostTypeConnectTest, hostOnlineId ) )
		{
			if( hostOnlineId.isVxGUIDValid() )
			{
				LogMsg( LOG_INFO, "NetActionResolveNetworkHostUrl:%s resolved with id %s", __func__, hostOnlineId.toOnlineIdString().c_str() );
				m_Engine.getNetStatusAccum().setNetHostAvail( true );
				return;
			}
		}

		VxSleep( 1000 );
		waitCnt++;
	}

	LogMsg( LOG_ERROR, "NetActionResolveNetworkHostUrl:%s timed out waiting to resolve", __func__ );
	m_Engine.getNetStatusAccum().setNetHostAvail( false );

}


