#pragma once
//============================================================================
// Copyright (C) 2012 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxProfile.h>

#include <string>

class AppProfile : public VxProfile
{
public:
	AppProfile();

	bool						isValid( void );
	RCODE						saveDebugSettings( void );
	void						loadProfile( void );

	const char*				    getLocalHostIp(); // if not "" then use ip to force listen on specific network adapter

	//=== vars ===//
	bool						m_bIsValid;
	uint16_t					m_u16TcpPort;
	uint16_t					m_u16UdpPort;
	std::string					m_strUserName;
	std::string					m_strUserGuid;
	std::string					m_strNetworkName;

	std::string					m_strNetworkIp;


	uint32_t					m_u32EnableDebug;
	uint32_t					m_u32DisableUdp;
	uint32_t					m_u32LogFlags;
	uint32_t					m_u32LogToFile;
	uint32_t					m_u32PaidOnly;
	std::string					m_strDebugFileName;

	std::string					m_strExeDir;

    std::string                 m_NlcPathPrefix; // prefix hash from exe directory so can run multiple instances on same device
	std::string					m_strRootUserDataDir;
	std::string					m_strRootXferDir;
	uint32_t					m_u32NetServiceOnly;			// this app is not user but set for net services only

protected:
	std::string&				getOsSpecificAppDataDir( void );
	std::string&				getOsSpecificHomeDir( void );
	std::string&				getOsSpecificDocumentsDir( void );

	std::string					m_strOsSpecificAppDataDir;
	std::string					m_strOsSpecificHomeDir;
	std::string					m_strOsSpecificDocumentsDir;

	std::string					m_strLocalHostIp;
};

