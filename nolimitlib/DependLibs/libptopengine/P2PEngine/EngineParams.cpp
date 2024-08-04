//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "EngineParams.h"
#include "EngineSettingsDefaultValues.h"
#include <NetLib/VxGetRandomPort.h>

#define ENGINE_PARAMS_DBVERSION 1
#define MY_SETTINGS_KEY "RCKEY"

//============================================================================
EngineParams::EngineParams()
: VxSettings( "EngineParamsDb" )
, m_Initialized( false )
{
}

//============================================================================
EngineParams::~EngineParams()
{
	engineParamsShutdown();
}

//============================================================================
RCODE EngineParams::engineParamsStartup( std::string& strDbFileName )
{
	if( m_Initialized )
	{
		engineParamsShutdown();
	}

	RCODE rc = dbStartup(ENGINE_PARAMS_DBVERSION, strDbFileName.c_str());
	if( 0 == rc )
	{
		m_Initialized = true;
	}

	return rc;
}

//============================================================================
void EngineParams::engineParamsShutdown( void )
{
	if( m_Initialized )
	{
		dbShutdown();
		m_Initialized = false;
	}
}


//============================================================================
void EngineParams::setLastHostWebsiteUrl( std::string& strWebsiteUrl )
{
	setIniValue( MY_SETTINGS_KEY, "HostUrl", strWebsiteUrl );
}

//============================================================================
void EngineParams::getLastHostWebsiteUrl( std::string& strWebsiteUrl )
{
	getIniValue( MY_SETTINGS_KEY, "HostUrl", strWebsiteUrl, "" );
}

//============================================================================
void EngineParams::setLastHostWebsiteResolvedIp( std::string& strWebsiteIp )
{
	setIniValue( MY_SETTINGS_KEY, "HostIp", strWebsiteIp );
}

//============================================================================
void EngineParams::getLastHostWebsiteResolvedIp( std::string& strWebsiteIp )
{
	getIniValue( MY_SETTINGS_KEY, "HostIp", strWebsiteIp, "" );
}

//============================================================================
void EngineParams::setLastConnectTestUrl( std::string& strWebsiteUrl )
{
	setIniValue( MY_SETTINGS_KEY, "NetServiceUrl", strWebsiteUrl );
}

//============================================================================
void EngineParams::getLastConnectTestUrl( std::string& strWebsiteUrl )
{
	getIniValue( MY_SETTINGS_KEY, "NetServiceUrl", strWebsiteUrl, "" );
}

//============================================================================
void EngineParams::setLastConnectTestResolvedIp( std::string& strWebsiteIp )
{
	setIniValue( MY_SETTINGS_KEY, "NetServiceIp", strWebsiteIp );
}

//============================================================================
void EngineParams::getLastConnectTestResolvedIp( std::string& strWebsiteIp )
{
	getIniValue( MY_SETTINGS_KEY, "NetServiceIp", strWebsiteIp, "" );
}

//============================================================================
void EngineParams::setLastListenSocket( int64_t listenSocket )
{
	setIniValue( MY_SETTINGS_KEY, "LastListenSkt", listenSocket );
}

//============================================================================
int64_t EngineParams::getLastListenSocket( void )
{
	int64_t listenSkt{ 0 };
	getIniValue( MY_SETTINGS_KEY, "LastListenSkt", listenSkt, 0 );
	return listenSkt;
}
