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

#include "VxTime.h" // time stamp and other time functions

class VxGUID;

enum EAppDir
{
	eAppDirUnknown = 0,

	// standard paths set from qt paths
	eAppData,
	eAppDownload,
	eAppMusic,
	eAppVideo,
	eAppPictures,
	eAppDocuments,

	// assets
	eAppDirAppExe,
    eAppDirKodiExe,
	eAppDirExeKodiAssets,
	eAppDirExeNoLimitAssets,

	eAppDirRootDataStorage,
	eAppDirAppTempData,
	eAppDirAppLogs,
	eAppDirAppKodiData,
	eAppDirAppNoLimitData,

	eAppDirRootUserData,

	eAppDirUserSpecific,
	eAppDirSettings,
	eAppDirAboutMePageServer,
	eAppDirAboutMePageClient,
	eAppDirStoryboardPageServer,
	eAppDirStoryBoardPageClient,
	eAppDirRootXfer,
	eAppDirUserXfer,
	eAppDirDownloads,
	eAppDirUploads,
	eAppDirIncomplete,
	eAppDirPersonalRecords,

    eAppDirThumbs,
    eAppDirCamRecord,

	eMaxAppDir

};

//============================================================================
void							VxSetAppIsShuttingDown( bool bIsShuttingDown );
bool							VxIsAppShuttingDown( void );

const char*						VxGetCompanyDomain( void );
const char*						VxGetOrginizationName( void );
const char*						VxGetCompanyWebsite( void );

void							VxSetNetworkHostName( const char* netHostName );
const char*						VxGetNetworkHostName( void );
void							VxSetNetworkHostPort( uint16_t netHostPort );
uint16_t						VxGetNetworkHostPort( void );
void							VxSetNetworkHostUrl( const char* netHostUrl );
const char*						VxGetNetworkHostUrl( void );

const char*						VxGetApplicationTitle( void );
const char*						VxGetApplicationNameNoSpaces( void );
const char*						VxGetApplicationNameNoSpacesLowerCase( void );

uint16_t						VxGetAppVersionShort( void );
uint32_t						VxGetAppVersionFull( void );
const char*						VxGetAppVersionString( void );

//============================================================================
void                            VxSetAppDirectory( enum EAppDir appDir, std::string setDir );
std::string& 					VxGetAppDirectory( enum EAppDir appDir );

// exe and app resource path	s
void							VxSetAppExeDirectory( const char* exeDir );
std::string&					VxGetAppExeDirectory( void );

void							VxSetKodiExeDirectory( const char* exeDir );
std::string&					VxGetKodiExeDirectory( void );

// user writable directories	
void							VxSetRootDataStorageDirectory( const char* rootDataDir );
std::string&					VxGetRootDataStorageDirectory( void );

std::string&					VxGetAppTempDirectory( void );
std::string&					VxGetAppLogsDirectory( void );
std::string&					VxGetAppKodiDataDirectory( void );
std::string&					VxGetAppNoLimitDataDirectory( void );
std::string&					VxGetAppThumbnailDirectory( void );

void							VxSetRootUserDataDirectory( const char* rootUserDataDir );
std::string&					VxGetRootUserDataDirectory( void );

void							VxSetUserSpecificDataDirectory( const char* userDataDir  );
std::string&					VxGetUserSpecificDataDirectory( void  );
std::string&					VxGetSettingsDirectory( void );

std::string&					VxGetAboutMePageServerDirectory( void ); 
std::string                     VxGetAboutMePageClientDirectory( VxGUID& onlineId );
std::string&					VxGetStoryBoardPageServerDirectory( void );
std::string                     VxGetStoryBoardPageClientDirectory( VxGUID& onlineId );

void							VxSetRootXferDirectory( const char* rootXferDir  );
std::string&					VxGetRootXferDirectory( void ) ;

void							VxSetUserXferDirectory( std::string userXferDir  );
std::string&					VxGetUserXferDirectory( void  );
std::string&					VxGetDownloadsDirectory( void );
std::string&					VxGetUploadsDirectory( void );
std::string&					VxGetIncompleteDirectory( void );
std::string&					VxGetPersonalRecordDirectory( void );

//============================================================================
//=== miscellaneous ===//
//============================================================================
void							VxSetNetworkLoopbackAllowed( bool bIsLoopbackAllowed );
bool							VxIsNetworkLoopbackAllowed( void );

int 							VxGlobalAccessLock( void );
int 							VxGlobalAccessUnlock( void );

void							VxSetLclIpAddress( const char* lclIpAddress );
std::string						VxGetLclIpAddress( void );

// set time format to military 24hr or AM/PM
void							SetUseMilitaryTime( bool useMilitaryTime );
bool							GetUseMilitaryTime( void );

void							VxSetMaxMessageHistory( int16_t maxHistory );
int32_t							VxGetMaxMessageHistory( void );
