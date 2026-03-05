//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppSettings.h"

#include <CoreLib/AppVersion.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxParse.h>

#include <QString>

namespace
{
	#define APP_SETTINGS_DBVERSION		1
	#define SELECTED_THEME_TYPE			"SELECTED_THEME_TYPE"
}

//============================================================================
AppSettings::AppSettings()
: VxSettings( "AppSettingsDb" )
{
}

//============================================================================
const char* AppSettings::getAppShortName( void )
{
	return APP_NAME;
}

//============================================================================
int32_t AppSettings::appSettingStartup( std::string dbSettingsFile )
{
	int32_t rc = dbStartup( APP_SETTINGS_DBVERSION, dbSettingsFile );
	if( 0 == rc )
	{
		m_AppSettingsInitialized = true;
	}

	return rc;
}

//============================================================================
void AppSettings::appSettingShutdown( void )
{
	dbShutdown();
}

//============================================================================
void AppSettings::setIsMessengerFullScreen( bool isFullScreen )
{
	setIniValue( getAppShortName(), "MessangerFullScreen", isFullScreen );
}

//============================================================================
bool AppSettings::getIsMessengerFullScreen( void )
{
	bool fullScreen;
	getIniValue( getAppShortName(), "MessangerFullScreen", fullScreen, false );
	return fullScreen;
}

//============================================================================
void AppSettings::setLastSelectedTheme( EThemeType selectedTheme )
{
	uint32_t themeType = (uint32_t)selectedTheme;
	setIniValue( getAppShortName(), SELECTED_THEME_TYPE, themeType );
}

//============================================================================
EThemeType AppSettings::getLastSelectedTheme( void )
{
	uint32_t themeType = 1;
	getIniValue( getAppShortName(), SELECTED_THEME_TYPE, themeType, (uint32_t)eThemeTypeDark );
	return (EThemeType)themeType;
}

//============================================================================
void AppSettings::setLastBrowseDir( EFileFilterType eFileFilterType, std::string& browseDir )
{
	if( 0 != browseDir.length() )
	{
		QString keyStr = QString("BrowseDir%1").arg( (int)eFileFilterType );
		setIniValue( getAppShortName(), keyStr.toUtf8().constData(), browseDir );
	}
}

//============================================================================
void AppSettings::getLastBrowseDir( EFileFilterType eFileFilterType, std::string& browseDir )
{
	QString keyStr = QString("BrowseDir%1").arg( (int)eFileFilterType );
	getIniValue( getAppShortName(), keyStr.toUtf8().constData(), browseDir, "" );
}

//============================================================================
void AppSettings::setLastBrowseFilter( EFileFilterType fileFilterType )
{
	int32_t filterType = (int32_t)fileFilterType;
	setIniValue( getAppShortName(), "LastBrowseFilter", filterType );
}

//============================================================================
EFileFilterType AppSettings::getLastBrowseFilter( void )
{
	int32_t filterType = 0;
	getIniValue( getAppShortName(), "LastBrowseFilter", filterType, (int)eFileFilterAll );
	return (EFileFilterType)filterType;	
}

//============================================================================
void AppSettings::setLastFileOfferFilter( EFileFilterType fileFilterType )
{
	int32_t filterType = (int32_t)fileFilterType;
	setIniValue( getAppShortName(), "LastFileOfferFilter", filterType );
}

//============================================================================
EFileFilterType AppSettings::getLastFileOfferFilter( void )
{
	int32_t filterType = 0;
	getIniValue( getAppShortName(), "LastFileOfferFilter", filterType, (int)eFileFilterAll );
	return (EFileFilterType)filterType;	
}

//============================================================================
void AppSettings::setLastFileShareViewFilter( EFileFilterType fileFilterType )
{
	int32_t filterType = (int32_t)fileFilterType;
	setIniValue( getAppShortName(), "LastShareViewFilter", filterType );
}

//============================================================================
EFileFilterType AppSettings::getLastFileShareViewFilter( void )
{
	int32_t filterType = 0;
	getIniValue( getAppShortName(), "LastShareViewFilter", filterType, (int)eFileFilterAll );
	return (EFileFilterType)filterType;	
}

//============================================================================
void AppSettings::setLastLibraryFilter( EFileFilterType fileFilterType )
{
	int32_t filterType = (int32_t)fileFilterType;
	setIniValue( getAppShortName(), "LastLibraryFilter", filterType );
}

//============================================================================
EFileFilterType AppSettings::getLastLibraryFilter( void )
{
	int32_t filterType = 0;
	getIniValue( getAppShortName(), "LastLibraryFilter", filterType, (int)eFileFilterAll );
	return (EFileFilterType)filterType;	
}

//============================================================================
void AppSettings::setLastLibraryAudioDir( std::string& browseDir )
{
	setIniValue( getAppShortName(), "LibAudioDir", browseDir );
}

//============================================================================
void AppSettings::getLastLibraryAudioDir( std::string& browseDir )
{
	getIniValue( getAppShortName(), "LibAudioDir", browseDir, "" );
}

//============================================================================
void AppSettings::setLastLibraryImageDir( std::string& browseDir )
{
	setIniValue( getAppShortName(), "LibImageDir", browseDir );
}

//============================================================================
void AppSettings::getLastLibraryImageDir( std::string& browseDir )
{
	getIniValue( getAppShortName(), "LibImageDir", browseDir, "" );
}

//============================================================================
void AppSettings::setLastLibraryVideoDir( std::string& browseDir )
{
	setIniValue( getAppShortName(), "LibVideoDir", browseDir );
}

//============================================================================
void AppSettings::getLastLibraryVideoDir( std::string& browseDir )
{
	getIniValue( getAppShortName(), "LibVideoDir", browseDir, "" );
}

//============================================================================
void AppSettings::setLastGalleryDir( std::string& galleryDir )
{
	setIniValue( getAppShortName(), "GalleryDir", galleryDir );
}

//============================================================================
void AppSettings::getLastGalleryDir( std::string& galleryDir )
{
	getIniValue( getAppShortName(), "GalleryDir", galleryDir, "" );
}

//============================================================================
void AppSettings::setLastVideoFileDir( std::string& fileDir )
{
	setIniValue( getAppShortName(), "VideoFileDir", fileDir );
}

//============================================================================
void AppSettings::getLastVideoFileDir( std::string& fileDir )
{
	getIniValue( getAppShortName(), "VideoFileDir", fileDir, "" );
}

//============================================================================
void AppSettings::setLastAudioFileDir( std::string& fileDir )
{
	setIniValue( getAppShortName(), "AudioFileDir", fileDir );
}

//============================================================================
void AppSettings::getLastAudioFileDir( std::string& fileDir )
{
	getIniValue( getAppShortName(), "AudioFileDir", fileDir, "" );
}

//============================================================================
void AppSettings::setLastAddFileDir( std::string addFileDir )
{
	setIniValue( getAppShortName(), "AddFileDir", addFileDir );
}

//============================================================================
void AppSettings::getLastAddFileDir( std::string& addFileDir )
{
	getIniValue( getAppShortName(), "AddFileDir", addFileDir, "" );
}

//============================================================================
void AppSettings::setLastFolderScanDir( std::string addFileDir )
{
	setIniValue( getAppShortName(), "FolderScanDir", addFileDir );
}

//============================================================================
void AppSettings::getLastFolderScanDir( std::string& addFileDir )
{
	getIniValue( getAppShortName(), "FolderScanDir", addFileDir, "" );
}

//============================================================================
void AppSettings::setLastBrowseShareDir( std::string& browseDir )
{
	setIniValue( getAppShortName(), "ShareBrowseDir", browseDir );
}

//============================================================================
void AppSettings::getLastBrowseShareDir( std::string& browseDir )
{
	getIniValue( getAppShortName(), "ShareBrowseDir", browseDir, "" );
}

//============================================================================
void AppSettings::getLastHostSearchText( ESearchType searchType, std::string& searchText )
{
    getIniValue( getAppShortName(), getAppendedType("LastHostSearchText", searchType).c_str(), searchText, "" );
}

//============================================================================
void AppSettings::setLastHostSearchText( ESearchType searchType, std::string& searchText )
{
    setIniValue( getAppShortName(), getAppendedType("LastHostSearchText", searchType).c_str(), searchText );
}

//============================================================================
void AppSettings::setLastHostSearchAgeType( ESearchType searchType, EAgeType ageType )
{
    uint32_t val = (uint32_t)ageType;
    setIniValue( getAppShortName(), getAppendedType("LastHostSearchAge", searchType).c_str(), val );
}

//============================================================================
void AppSettings::getLastHostSearchAgeType( ESearchType searchType, EAgeType& ageType )
{
    uint32_t val = 0;
    getIniValue( getAppShortName(), getAppendedType("LastHostSearchAge", searchType).c_str(), val, 0 );
    ageType = (EAgeType)val;
}

//============================================================================
void AppSettings::setLastHostSearchGender( ESearchType searchType, EGenderType genderType )
{
    uint32_t val = (uint32_t)genderType;
    setIniValue( getAppShortName(), getAppendedType("LastHostSearchGender", searchType).c_str(), val );
}

//============================================================================
void AppSettings::getLastHostSearchGender( ESearchType searchType, EGenderType& genderType )
{
    uint32_t val = 0;
    getIniValue( getAppShortName(), getAppendedType("LastHostSearchGender", searchType).c_str(), val, 0 );
    genderType = (EGenderType)val;
}

//============================================================================
void AppSettings::setLastHostSearchLanguage( ESearchType searchType, ELanguageType languageType )
{
    uint32_t val = (uint32_t)languageType;
    setIniValue( getAppShortName(), getAppendedType("LastHostSearchLanguage", searchType).c_str(), val );
}

//============================================================================
void AppSettings::getLastHostSearchLanguage( ESearchType searchType, ELanguageType& languageType )
{
    uint32_t val = 0;
    getIniValue( getAppShortName(), getAppendedType("LastHostSearchLanguage", searchType).c_str(), val, 0 );
    languageType = (ELanguageType)val;
}

//============================================================================
void AppSettings::setLastHostSearchContentRating( ESearchType searchType, EContentRating contentRating )
{
    uint32_t val = (uint32_t)contentRating;
    setIniValue( getAppShortName(), getAppendedType("LastHostSearchRating", searchType).c_str(), val );
}

//============================================================================
void AppSettings::getLastHostSearchContentRating( ESearchType searchType, EContentRating& contentRating )
{
    uint32_t val = 0;
    getIniValue( getAppShortName(), getAppendedType("LastHostSearchRating", searchType).c_str(), val, 0 );
    contentRating = (EContentRating)val;
}

//============================================================================
void AppSettings::setCamRotation( std::string camId, uint32_t camRotation )
{
	std::string camKey;
	StdStringFormat( camKey, "CamRotation%s", camId.c_str() );
	setIniValue( getAppShortName(), camKey.c_str(), camRotation );
}

//============================================================================
uint32_t AppSettings::getCamRotation( std::string camId )
{
	uint32_t camRotation = 0;
	std::string camKey;
	StdStringFormat( camKey, "CamRotation%s", camId.c_str() );
	getIniValue( getAppShortName(), camKey.c_str(), camRotation, 0 );
	return camRotation;
}

//============================================================================
void AppSettings::setCamEnable( bool camEnableIn )
{
	uint32_t camEnable = camEnableIn ? 1 : 0;
	setIniValue( getAppShortName(), "CamEnable", camEnable );
}

//============================================================================
bool AppSettings::getCamEnable( void )
{
	uint32_t camEnable = 0;
	getIniValue( getAppShortName(), "CamEnable", camEnable, 1 );
	return camEnable ? true : false;
}

//============================================================================
void AppSettings::setCamSourceId( std::string camId )
{
	setIniValue( getAppShortName(), "CamSourceId", camId );
}

//============================================================================
std::string AppSettings::getCamSourceId( void )
{
	std::string camSourceId;
	getIniValue( getAppShortName(), "CamSourceId", camSourceId, "" );
	return camSourceId;
}

//============================================================================
void AppSettings::setCamShowPreview( bool showPreview )
{
	uint32_t showPreviewVal = showPreview ? 1 : 0;
	setIniValue( getAppShortName(), "CamShowPreview", showPreviewVal );
}

//============================================================================
bool AppSettings::getCamShowPreview( void )
{
	uint32_t showPreview = 0;
	getIniValue( getAppShortName(), "CamShowPreview", showPreview, 0 );
	return showPreview ? true : false;
}

//============================================================================
void AppSettings::setVidFeedRotation( uint32_t feedRotation )
{
	setIniValue( getAppShortName(), "VidFeedRotation", feedRotation );
}

//============================================================================
uint32_t AppSettings::getVidFeedRotation( void )
{
	uint32_t feedRotation = 0;
	getIniValue( getAppShortName(), "VidFeedRotation", feedRotation, 0 );
	return feedRotation;
}

//============================================================================
void AppSettings::setLastAppletLaunched( ELaunchFrame launchPage, EApplet applet )
{
    uint32_t appletVal = (EApplet)applet;
	const char* settingName = launchPage == eLaunchFrameHome ? "LastAppletLaunched1" : "LastAppletLaunched2";
    setIniValue( getAppShortName(), settingName, appletVal );
}

//============================================================================
EApplet AppSettings::getLastAppletLaunched( ELaunchFrame launchPage )
{
    uint32_t appletVal = 0;
	const char* settingName = launchPage == eLaunchFrameHome ? "LastAppletLaunched1" : "LastAppletLaunched2";
    getIniValue( getAppShortName(), settingName, appletVal, 0 );
    return (EApplet)appletVal;
}

//============================================================================
void AppSettings::setVerboseLog( bool verbose )
{
    uint32_t appletVal = verbose;
    setIniValue( getAppShortName(), "VerboseLog", appletVal );
}

//============================================================================
bool AppSettings::getVerboseLog( void )
{
    uint32_t appletVal = 0;
    getIniValue( getAppShortName(), "VerboseLog", appletVal, 0 );
    return (bool)appletVal;
}

//============================================================================
void AppSettings::setLogLevels( uint32_t logLevelFlags )
{
#if defined(DEBUG)
    setIniValue( getAppShortName(), "LogLevelsD", logLevelFlags );
#else
    setIniValue( getAppShortName(), "LogLevels", logLevelFlags );
#endif // defined(DEBUG)
}

//============================================================================
uint32_t AppSettings::getLogLevels( void )
{
    uint32_t logLevelFlags = 0;
#if defined(DEBUG)
    logLevelFlags = LOG_PRIORITY_MASK;	// LOG_VERBOSE
    getIniValue( getAppShortName(), "LogLevelsD", logLevelFlags, LOG_PRIORITY_MASK );
#else
    logLevelFlags = LOG_FATAL | LOG_SEVERE | LOG_ASSERT;
    getIniValue( getAppShortName(), "LogLevels", logLevelFlags, logLevelFlags );
#endif // defined(DEBUG)
    return logLevelFlags;
}

//============================================================================
void AppSettings::setLogModules( uint32_t logLevelFlags )
{
#if defined(DEBUG)
    setIniValue( getAppShortName(), "LogModulesD", logLevelFlags );
#else
    setIniValue( getAppShortName(), "LogModules", logLevelFlags );
#endif // defined(DEBUG)
}

//============================================================================
uint32_t AppSettings::getLogModules( void )
{
    uint32_t logModuleFlags = 0;
#if defined(DEBUG)
	uint32_t logModuleFlagsDefault = 0;
    getIniValue( getAppShortName(), "LogModulesD", logModuleFlags, logModuleFlagsDefault );
#else
    getIniValue( getAppShortName(), "LogModules", logModuleFlags, logModuleFlags );
#endif // defined(DEBUG)
    return logModuleFlags;
}

//============================================================================
bool AppSettings::getMutePhoneRing( void )
{
	bool bMute;
	getIniValue( getAppShortName(), "MutePhoneRing", bMute, false );

	return bMute;
}

//============================================================================
void AppSettings::setMutePhoneRing( bool bMutePhoneRing )
{
	setIniValue( getAppShortName(), "MutePhoneRing", bMutePhoneRing );
}

//============================================================================
bool AppSettings::getMuteNotifySound( void )
{
	bool bMute;
	getIniValue( getAppShortName(), "MuteNotifySound", bMute, false );

	return bMute;
}

//============================================================================
void AppSettings::setMuteNotifySound( bool bNotifySound )
{
	uint32_t u32Value = bNotifySound;
	setIniValue( getAppShortName(), "MuteNotifySound", u32Value );
}

//============================================================================
void AppSettings::setIsConfirmDeleteDisabled( bool confirmIsDisabled )
{
	uint32_t u32Value = confirmIsDisabled;
	setIniValue( getAppShortName(), "ConfirmDeleteDisable", u32Value );
}

//============================================================================
bool AppSettings::getIsConfirmDeleteDisabled( void )
{
	bool confirmIsDisabled;
	getIniValue( getAppShortName(), "ConfirmDeleteDisable", confirmIsDisabled, false );
	return confirmIsDisabled;
}

//============================================================================
std::string AppSettings::getAppendedType( const char* key, ESearchType searchType )
{
    std::string result;
    StdStringFormat( result, "%s%d", key, (int)searchType );
    return result;
}

//============================================================================
void AppSettings::setLastUsedTestUrl( std::string& testUrl )
{
    setIniValue( getAppShortName(), "LastTestUrl", testUrl );
}

//============================================================================
void AppSettings::getLastUsedTestUrl( std::string& testUrl )
{
    getIniValue( getAppShortName(), "LastTestUrl", testUrl, "" );
}

//============================================================================
void AppSettings::setFavoriteHostGroupUrl( std::string& hostUrl )
{
	setIniValue( getAppShortName(), "FavGroupHostUrl", hostUrl );
}

//============================================================================
void AppSettings::getFavoriteHostGroupUrl( std::string& hostUrl )
{
	getIniValue( getAppShortName(), "FavGroupHostUrl", hostUrl, "" );
}

//============================================================================
void AppSettings::setSoundInDeviceIndex( int32_t deviceIndex )
{
	setIniValue( getAppShortName(), "SoundInDeviceIndex", deviceIndex );
}

//============================================================================
int32_t AppSettings::getSoundInDeviceIndex( void )
{
	int32_t deviceIndex = 0;
	getIniValue( getAppShortName(), "SoundInDeviceIndex", deviceIndex );
	return deviceIndex;
}

//============================================================================
void AppSettings::setSoundOutDeviceIndex( int32_t deviceIndex )
{
	setIniValue( getAppShortName(), "SoundOutDeviceIndex", deviceIndex );
}

//============================================================================
int32_t AppSettings::getSoundOutDeviceIndex( void )
{
	int32_t deviceIndex = 0;
	getIniValue( getAppShortName(), "SoundOutDeviceIndex", deviceIndex, 0 );
	return deviceIndex;
}

//============================================================================
void AppSettings::setRunOnStartupCamServer( bool runOnStartup )
{
	setIniValue( getAppShortName(), "RunOnStartupCamServer", runOnStartup );
}

//============================================================================
bool AppSettings::getRunOnStartupCamServer( void )
{
	bool runOnStartup = false;
	getIniValue( getAppShortName(), "RunOnStartupCamServer", runOnStartup, false );
	return runOnStartup;
}

//============================================================================
void AppSettings::setRunOnStartupFileShareServer( bool runOnStartup )
{
	setIniValue( getAppShortName(), "RunOnStartupFileShareServer", runOnStartup );
}

//============================================================================
bool AppSettings::getRunOnStartupFileShareServer( void )
{
	bool runOnStartup = false;
	getIniValue( getAppShortName(), "RunOnStartupFileShareServer", runOnStartup, false );
	return runOnStartup;
}

//============================================================================
void AppSettings::setEchoDelayParam( int delayMs )
{
	uint32_t delayVal = (uint32_t)delayMs;
	setIniValue( getAppShortName(), "EchoDelayParam", delayVal );
}

//============================================================================
int AppSettings::getEchoDelayParam( void )
{
	uint32_t defaultParamVal = 140;
#if defined(TARGET_OS_ANDROID)
	defaultParamVal = 280;
#elif defined(TARGET_OS_WINDOWS) 
	defaultParamVal = 140;
#elif defined(TARGET_OS_LINUX) 
	defaultParamVal = 140;
#endif // defined(TARGET_OS_ANDROID)

	uint32_t paramVal;
	getIniValue( getAppShortName(), "EchoDelayParam", paramVal, defaultParamVal );
	return (int)paramVal;
}

//============================================================================
void AppSettings::setUseMilitaryTime( bool enable )
{
	setIniValue( getAppShortName(), "UseMilitaryTime", enable );
}

//============================================================================
bool AppSettings::getUseMilitaryTime( void )
{
	bool useMilitaryTime = false;
	getIniValue( getAppShortName(), "UseMilitaryTime", useMilitaryTime, false );
	return useMilitaryTime;
}

//============================================================================
void AppSettings::setMaxMessageHistory( int32_t maxHistory )
{
	setIniValue( getAppShortName(), "MaxMessageHistory", maxHistory );
}

//============================================================================
int32_t AppSettings::getMaxMessageHistory( void )
{
	int32_t maxMsgHistory = 0;
	getIniValue( getAppShortName(), "MaxMessageHistory", maxMsgHistory, 200 );
	return maxMsgHistory;
}

//============================================================================
void AppSettings::setWantMicrophone( bool enable )
{
	setIniValue( getAppShortName(), "WantMic", enable );
}

//============================================================================
bool AppSettings::getWantMicrophone( void )
{
	bool wantMick = false;
	getIniValue( getAppShortName(), "WantMic", wantMick, false );
	return wantMick;
}

//============================================================================
void AppSettings::setIsMicrophoneMuted( bool enable )
{
	setIniValue( getAppShortName(), "MuteMic", enable );
}

//============================================================================
bool AppSettings::getIsMicrophoneMuted( void )
{
	bool muteMic = false;
	getIniValue( getAppShortName(), "MuteMic", muteMic, false );
	return muteMic;
}

//============================================================================
void AppSettings::setWantSpeaker( bool enable )
{
	setIniValue( getAppShortName(), "WantSpeaker", enable );
}

//============================================================================
bool AppSettings::getWantSpeaker( void )
{
	bool wantSpeaker = false;
	getIniValue( getAppShortName(), "WantSpeaker", wantSpeaker, false );
	return wantSpeaker;
}

//============================================================================
void AppSettings::setIsSpeakerMuted( bool enable )
{
	setIniValue( getAppShortName(), "MuteSpeaker", enable );
}

//============================================================================
bool AppSettings::getIsSpeakerMuted( void )
{
	bool muteSpeaker = false;
	getIniValue( getAppShortName(), "MuteSpeaker", muteSpeaker, false );
	return muteSpeaker;
}

//============================================================================
void AppSettings::setNoAecLoopback( bool enable )
{
	setIniValue( getAppShortName(), "NoAecLoopback", enable );	
}

//============================================================================
bool AppSettings::getNoAecLoopback( void )
{
	bool enableAudioLoopback = false;
	getIniValue( getAppShortName(), "NoAecLoopback", enableAudioLoopback, false );
	return enableAudioLoopback;
}

//============================================================================
void AppSettings::setWithAecLoopback( bool enable )
{
	setIniValue( getAppShortName(), "WithAecLoopback", enable );	
}

//============================================================================
bool AppSettings::getWithAecLoopback( void )
{
	bool enableAudioLoopback = false;
	getIniValue( getAppShortName(), "WithAecLoopback", enableAudioLoopback, false );
	return enableAudioLoopback;
}

//============================================================================
void AppSettings::setLastUserConnectionsUserViewType( int comboIdx )
{
	uint32_t comboVal = (uint32_t)comboIdx;
	setIniValue( getAppShortName(), "UserConnectionsUserViewType", comboVal );
}

//============================================================================
int AppSettings::getLastUserConnectionsUserViewType( void )
{
	uint32_t comboVal = 0;
	getIniValue( getAppShortName(), "UserConnectionsUserViewType", comboVal, 1 );
	return comboVal;
}

//============================================================================
void AppSettings::setLastHostJoined( std::string lastJoinedHost )
{
	setIniValue( getAppShortName(), "LastHostJoined", lastJoinedHost );
}

//============================================================================
std::string AppSettings::getLastHostJoined( void )
{
	std::string lastHost;
	getIniValue( getAppShortName(), "LastHostJoined", lastHost, "" );
	return lastHost;
}

//============================================================================
void AppSettings::setLastHostJoined( EHostType hostType, std::string lastJoinedHost )
{
	bool valid{ false };
	std::string settingName{ "LastJoined" };
	switch( hostType )
	{
	case eHostTypeChatRoom:
		settingName += "ChatRoom";
		valid = true;
		break;
	case eHostTypeGroup:
		settingName += "Group";
		valid = true;
		break;
	case eHostTypeRandomConnect:
		settingName += "RandomConnect";
		valid = true;
		break;
	default:
		break;
	}

	if( valid )
	{
		setIniValue( getAppShortName(), settingName.c_str(), lastJoinedHost);
        setLastHostJoined( lastJoinedHost );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppSettings::%s invalid host type", __func__ );
	}
}

//============================================================================
std::string AppSettings::getLastHostJoined( EHostType hostType )
{
	std::string lastHost;
	bool valid{ false };
	std::string settingName{ "LastJoined" };
	switch( hostType )
	{
	case eHostTypeChatRoom:
		settingName += "ChatRoom";
		valid = true;
		break;
	case eHostTypeGroup:
		settingName += "Group";
		valid = true;
		break;
	case eHostTypeRandomConnect:
		settingName += "RandomConnect";
		valid = true;
		break;
	default:
		break;
	}

	if( valid )
	{
		getIniValue( getAppShortName(), settingName.c_str(), lastHost, "" );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppSettings::%s invalid host type", __func__ );
	}

	return lastHost;
}

//============================================================================
void AppSettings::setIsAutomatedHost( bool enable )
{
	setIniValue( getAppShortName(), "UnattendedHost", enable );
}

//============================================================================
bool AppSettings::getIsAutomatedHost( void )
{
	bool unattendedHost = false;
	getIniValue( getAppShortName(), "UnattendedHost", unattendedHost, false );
	return unattendedHost;
}

//============================================================================
void AppSettings::setDisableAllSoundEffects( bool disable )
{
	m_DisableSoundEffectsValue = disable;
	m_DisableSoundEffectsCached = true;
	setIniValue( getAppShortName(), "DisableSoundEffect", disable );
}

//============================================================================
bool AppSettings::getDisableAllSoundEffects( void )
{
	if( m_DisableSoundEffectsCached )
	{
		return m_DisableSoundEffectsValue;
	}

	bool disableSoundEffects = false;
	getIniValue( getAppShortName(), "DisableSoundEffect", disableSoundEffects, false );
	m_DisableSoundEffectsValue = disableSoundEffects;
	m_DisableSoundEffectsCached = true;

	return disableSoundEffects;
}

//============================================================================
void AppSettings::setDisableSndTrash( bool disable )
{
	setIniValue( getAppShortName(), "DisableSndTrash", disable );
}

//============================================================================
bool AppSettings::getDisableSndTrash( void )
{
	bool disableSound = false;
	getIniValue( getAppShortName(), "DisableSndTrash", disableSound, false );
	return disableSound;
}

//============================================================================
void AppSettings::setDisableSndKeyClick( bool disable )
{
	setIniValue( getAppShortName(), "DisableSndKeyClick", disable );
}

//============================================================================
bool AppSettings::getDisableSndKeyClick( void )
{
	bool disableSound = false;
	getIniValue( getAppShortName(), "DisableSndKeyClick", disableSound, false );
	return disableSound;
}

//============================================================================
void AppSettings::setDisableSndNotify( bool disable )
{
	setIniValue( getAppShortName(), "DisableSndNotify", disable );
}

//============================================================================
bool AppSettings::getDisableSndNotify( void )
{
	bool disableSound = false;
	getIniValue( getAppShortName(), "DisableSndNotify", disableSound, false );
	return disableSound;
}

//============================================================================
void AppSettings::setDisableSndMsgRx( bool disable )
{
	setIniValue( getAppShortName(), "DisableSndMsgRx", disable );
}

//============================================================================
bool AppSettings::getDisableSndMsgRx( void )
{
	bool disableSound = false;
	getIniValue( getAppShortName(), "DisableSndMsgRx", disableSound, false );
	return disableSound;
}

//============================================================================
void AppSettings::setUseSystemMediaPlayer( bool useSystemPlayer )
{
	setIniValue( getAppShortName(), "UseSystemPlayer", useSystemPlayer );
}

//============================================================================
bool AppSettings::getUseSystemMediaPlayer( void )
{
	bool useSystemPlayer = false;
	getIniValue( getAppShortName(), "UseSystemPlayer", useSystemPlayer, false );

	return useSystemPlayer;
}

//============================================================================
void AppSettings::setLastPlayedMovie( std::string& movieFile )
{
    setIniValue( getAppShortName(), "LastPlayedMovie", movieFile );
}

//============================================================================
void AppSettings::getLastPlayedMovie( std::string& movieFile )
{
    getIniValue( getAppShortName(), "LastPlayedMovie", movieFile, "" );
}

//============================================================================
void AppSettings::setShowInWaveForm( bool show )
{
	setIniValue( getAppShortName(), "ShowInWaveForm", show );
}

//============================================================================
bool AppSettings::getShowInWaveForm( void )
{
	bool showInWaveForm = false;
	getIniValue( getAppShortName(), "ShowInWaveForm", showInWaveForm, true );
	return showInWaveForm;
}

//============================================================================
void AppSettings::setShowOutWaveForm(  bool show )
{
	setIniValue( getAppShortName(), "ShowOutWaveForm", show );
}

//============================================================================
bool AppSettings::getShowOutWaveForm( void )
{
	bool showOutWaveForm = false;
	getIniValue( getAppShortName(), "ShowOutWaveForm", showOutWaveForm, true );
	return showOutWaveForm;
}

//============================================================================
void AppSettings::setShowSoundOutSettings(  bool show )
{
	setIniValue( getAppShortName(), "ShowSoundOutSettings", show );
}

//============================================================================
bool AppSettings::getShowSoundOutSettings( void )
{
	bool showSoundOut = false;
	getIniValue( getAppShortName(), "ShowSoundOutSettings", showSoundOut, false );
	return showSoundOut;
}

//============================================================================
void AppSettings::setShowSoundInSettings(  bool show )
{
	setIniValue( getAppShortName(), "ShowSoundInSettings", show );
}

//============================================================================
bool AppSettings::getShowSoundInSettings( void )
{
	bool showSoundIn = false;
	getIniValue( getAppShortName(), "ShowSoundInSettings", showSoundIn, false );
	return showSoundIn;
}

//============================================================================
void AppSettings::setShowSoundLog(  bool show )
{
	setIniValue( getAppShortName(), "ShowSoundLog", show );
}

//============================================================================
bool AppSettings::getShowSoundLog( void )
{
	bool showSoundLog = false;
	getIniValue( getAppShortName(), "ShowSoundLog", showSoundLog, false );
	return showSoundLog;
}

//============================================================================
void AppSettings::setAgcEnabled( bool enabled )
{
	setIniValue( getAppShortName(), "AgcEnabled", enabled );
}

//============================================================================
bool AppSettings::getAgcEnabled( void )
{
	bool agcEnabled = false;
	getIniValue( getAppShortName(), "AgcEnabled", agcEnabled, false );
	return agcEnabled;
}
