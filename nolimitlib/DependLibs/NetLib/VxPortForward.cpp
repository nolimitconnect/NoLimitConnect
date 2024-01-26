//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxPortForward.h"

#include <libminiupnpc/upnpc.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>

#include <array>

bool VxPortForward::m_ForwardEnable = false;

namespace // anonymouse
{
	const int MAX_UPNP_CMD_ARGS = 10;

	class UpnpCmdLine
	{
	public:
		UpnpCmdLine()
		{
		}

		void setupUpnpCmdLine( bool ipv6 )
		{
			std::string strExePathAndFileName;
			if( 0 == VxFileUtil::getExecuteFullPathAndName( strExePathAndFileName ) && !strExePathAndFileName.empty() )
			{
				LogModule( eLogPortForward, LOG_DEBUG, "UpnpCmdLine app exe: %s", strExePathAndFileName.c_str() );
				addArg( strExePathAndFileName.c_str() );
			}
			else
			{
				LogModule( eLogPortForward, LOG_ERROR, "UpnpCmdLine failed to retrieve app exe" );
				addArg( "FakeExe" );
			}

			if( ipv6 )
			{
				addArg( "-6" );
			}
		}

		void addArg( const std::string argStr )
		{
			cmdParams.push_back( argStr );
			argc++;
		}

		bool runCmd( void )
		{
			char** vargs = new char* [argc];
			for( int i = 0; i < argc; i++ )
			{
				vargs[i] =  (char *)cmdParams[i].c_str();
			}

			int rc = runUpnp( argc, vargs );

            delete[] vargs;

			return 0 == rc;
		}


		int argc{ 0 };

		std::vector<std::string> cmdParams;
	};

} 

//============================================================================
void VxPortForward::setEnablePortForward( bool enable )
{
	LogModule( eLogPortForward, LOG_VERBOSE, "VxPortForward::setEnablePortForward %d ", enable );
	m_ForwardEnable = enable;
}

//============================================================================
bool VxPortForward::getEnablePortForward( void )
{
	return m_ForwardEnable;
}

//============================================================================
bool VxPortForward::addPortForward( bool ipv6, std::string ipAddr, uint16_t externPort )
{
	if( !m_ForwardEnable )
	{
		LogModule( eLogPortForward, LOG_DEBUG, "VxPortForward::addPortForward not enabled " );
		return false;
	}

	if( externPort < 80 )
	{
		LogModule( eLogPortForward, LOG_DEBUG, "VxPortForward::addPortForward invalid port %s", externPort );
		return false;
	}

	UpnpCmdLine upnpCmdLine;
	upnpCmdLine.setupUpnpCmdLine( ipv6 );
	
	upnpCmdLine.addArg( "-r" );

	std::string port = std::to_string( externPort );
	upnpCmdLine.addArg( port.c_str() );

	upnpCmdLine.addArg( "TCP" );

	return upnpCmdLine.runCmd();
}

//============================================================================
bool VxPortForward::removePortForward( bool ipv6, uint16_t port )
{
	if( !m_ForwardEnable )
	{
		LogModule( eLogPortForward, LOG_DEBUG, "VxPortForward::removePortForward not enabled " );
		return false;
	}

	UpnpCmdLine upnpCmdLine;
	upnpCmdLine.setupUpnpCmdLine( ipv6 );

	upnpCmdLine.addArg( "-d" );

	std::string portStr = std::to_string( port );
	upnpCmdLine.addArg( portStr.c_str() );

	upnpCmdLine.addArg( "tcp" );

	return upnpCmdLine.runCmd();

}

//============================================================================
bool VxPortForward::listPortForward( bool ipv6 )
{
	if( !m_ForwardEnable )
	{
		LogModule( eLogPortForward, LOG_DEBUG, "VxPortForward::listPortForward not enabled " );
	}

	UpnpCmdLine upnpCmdLine;
	upnpCmdLine.setupUpnpCmdLine( ipv6 );

	upnpCmdLine.addArg( "-l" );
	return upnpCmdLine.runCmd();
}


