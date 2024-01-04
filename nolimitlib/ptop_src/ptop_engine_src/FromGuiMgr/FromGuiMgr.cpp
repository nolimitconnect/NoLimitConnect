//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FromGuiMgr.h"

#include "FromGuiAction.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

namespace
{
	//============================================================================
	static void* FromGuiMgrThreadFunc( void* pvContext )
	{
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );
		FromGuiMgr* poMgr = (FromGuiMgr*)poThread->getThreadUserParam();
		if( poMgr )
		{
			poMgr->fromGuiThreadWork( poThread );
		}

		poThread->threadAboutToExit();
		return nullptr;
	}
};

//============================================================================
FromGuiMgr::FromGuiMgr( P2PEngine& engine )
: m_Engine( engine )
, m_WorkerThreadName( "workerThreadFromGui" )
{
	m_WorkerThread.startThread( (VX_THREAD_FUNCTION_T)FromGuiMgrThreadFunc, this, m_WorkerThreadName.c_str() );			
}

//============================================================================
void FromGuiMgr::fromGuMgrShutdown( void )
{
	m_WorkerThread.abortThreadRun( true );
	m_FromGuiSemaphore.signal();
}

//============================================================================
void FromGuiMgr::fromGuiAppStartup( std::string assetDir, std::string rootDataDir )
{
	queFromGuiAction( new FromGuiStartupDirectoryAction( m_Engine, eFromGuiTypeAppStartup, assetDir, rootDataDir ) );
}

//============================================================================
void FromGuiMgr::fromGuiSetUserSpecificDir( std::string userSpecificDir )
{
	queFromGuiAction( new FromGuiStartupDirectoryAction( m_Engine, eFromGuiTypeSetUserSpecificDir, userSpecificDir ) );
}

//============================================================================
void FromGuiMgr::fromGuiSetUserXferDir( std::string userXferDir )
{
	queFromGuiAction( new FromGuiStartupDirectoryAction( m_Engine, eFromGuiTypeSetUserXferDir, userXferDir ) );
}

//============================================================================
void FromGuiMgr::fromGuiUserLoggedOn( VxNetIdent* netIdent )
{
	queFromGuiAction( new FromGuiUserLogon( m_Engine, eFromGuiTypeiUserLoggedOn, netIdent ) );
}

//============================================================================
void FromGuiMgr::fromGuiAnnounceHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 )
{
	queFromGuiAction( new FromGuiHostAction( m_Engine, eFromGuiAnnounceHost, adminId, sessionId, hostUrlIpv4, hostUrlIpv6 ) );
}

//============================================================================
void FromGuiMgr::fromGuiJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 )
{
	queFromGuiAction( new FromGuiHostAction( m_Engine, eFromGuiJoinHost, adminId, sessionId, hostUrlIpv4, hostUrlIpv6 ) );
}

//============================================================================
void FromGuiMgr::fromGuiLeaveHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 )
{
	queFromGuiAction( new FromGuiHostAction( m_Engine, eFromGuiLeaveHost, adminId, sessionId, hostUrlIpv4, hostUrlIpv6 ) );
}

//============================================================================
void FromGuiMgr::fromGuiUnJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 )
{
	queFromGuiAction( new FromGuiHostAction( m_Engine, eFromGuiUnJoinHost, adminId, sessionId, hostUrlIpv4, hostUrlIpv6 ) );
}

//============================================================================
void FromGuiMgr::fromGuiJoinLastJoinedHost( HostedId& adminId, VxGUID& sessionId )
{
	queFromGuiAction( new FromGuiJoinLastHostAction( m_Engine, eFromGuiJoinLastJoinedHost, adminId, sessionId ) );
}

//============================================================================
void FromGuiMgr::fromGuiQueryHostListFromNetworkHost( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll, VxGUID& searchSessionId )
{
	queFromGuiAction( new FromGuiQueryHostListFromNetworkHostAction( m_Engine, eFromGuiQueryHostListFromNetworkHost, netHostUrl, hostType, hostIdIfNullThenAll, searchSessionId ) );
}

//============================================================================
void FromGuiMgr::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable )
{
	queFromGuiAction( new FromGuiSearchHostAction( m_Engine, eFromGuiSearchHost, hostType, searchParams, enable ) );
}

//============================================================================
void FromGuiMgr::queFromGuiAction( FromGuiActionBase* fromGuiAction )
{
	lockFromGuiQue();
	m_FromGuiActionQue.emplace_back( fromGuiAction );
	unlockFromGuiQue();

	m_FromGuiSemaphore.signal();
}

//============================================================================
void FromGuiMgr::fromGuiThreadWork( VxThread* workThread )
{
	while( true )
	{
		if( workThread->isAborted() || VxIsAppShuttingDown() )
		{
			return;
		}

		m_FromGuiSemaphore.wait();
		
		if( workThread->isAborted() || VxIsAppShuttingDown() )
		{
			return;
		}

		while( !m_FromGuiActionQue.empty() )
		{
			if( workThread->isAborted() || VxIsAppShuttingDown() )
			{
				return;
			}

			FromGuiActionBase* fromGuiAction{ nullptr };
			lockFromGuiQue();
			if( !m_FromGuiActionQue.empty() )
			{
				fromGuiAction = m_FromGuiActionQue.front();
				m_FromGuiActionQue.pop_front();
			}

			unlockFromGuiQue();
			if( fromGuiAction )
			{
                LogMsg( LOG_VERBOSE, "FromGuiMgr executing %s", fromGuiAction->describeGuiAction().c_str() );
				fromGuiAction->executeAction();
			}
		}
	}
}
