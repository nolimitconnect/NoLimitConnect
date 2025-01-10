#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "AppDefs.h"
#include "AppProfile.h"

#include <GuiInterface/IDefs.h>

#include <CoreLib/VxFileTypeMasks.h>
#include <CoreLib/VxSettings.h>

class AppSettings : public VxSettings
{
public:
	AppSettings();
	virtual ~AppSettings() = default;

	const char*					getAppShortName( void );

	RCODE						appSettingStartup( std::string dbSettingsFile );
	void						appSettingShutdown( void );

	bool						getIsAppSettingInitialized( void )			{ return m_AppSettingsInitialized; }

	void						setIsMessengerFullScreen( bool isFullScreen );
	bool						getIsMessengerFullScreen( void );

	void						setLastSelectedTheme( EThemeType selectedTheme );
	EThemeType					getLastSelectedTheme( void );

	void						setMutePhoneRing( bool bMutePhoneRing );
	bool						getMutePhoneRing( void );

	void						setMuteNotifySound( bool bMuteNotifySound );
	bool						getMuteNotifySound( void );

	void						setIsConfirmDeleteDisabled( bool confirmIsDisabled );
	bool						getIsConfirmDeleteDisabled( void );

	void						setLastAddFileDir( std::string addFileDir );
	void						getLastAddFileDir( std::string& addFileDir );

	void						setLastBrowseShareDir( std::string& browseDir );
	void						getLastBrowseShareDir( std::string& browseDir );

	void						setLastBrowseDir( EFileFilterType fileFilterType, std::string& browseDir );
	void						getLastBrowseDir( EFileFilterType fileFilterType, std::string& browseDir );

	void						setLastBrowseFilter( EFileFilterType fileFilterType );
	EFileFilterType				getLastBrowseFilter( void );

	void						setLastFileOfferFilter( EFileFilterType fileFilterType );
	EFileFilterType				getLastFileOfferFilter( void );

	void						setLastFileShareViewFilter( EFileFilterType fileFilterType );
	EFileFilterType				getLastFileShareViewFilter( void );

	void						setLastLibraryFilter( EFileFilterType fileFilterType );
	EFileFilterType				getLastLibraryFilter( void );
	void						setLastLibraryAudioDir( std::string& browseDir );
	void						getLastLibraryAudioDir( std::string& browseDir );
	void						setLastLibraryImageDir( std::string& browseDir );
	void						getLastLibraryImageDir( std::string& browseDir );
	void						setLastLibraryVideoDir( std::string& browseDir );
	void						getLastLibraryVideoDir( std::string& browseDir );

	void						setLastGalleryDir( std::string& galleryDir );
	void						getLastGalleryDir( std::string& galleryDir );
	void						setLastVideoFileDir( std::string& fileDir );
	void						getLastVideoFileDir( std::string& fileDir );
	void						setLastAudioFileDir( std::string& fileDir );
	void						getLastAudioFileDir( std::string& fileDir );

    void						setLastHostSearchText( ESearchType searchType, std::string& searchText );
    void						getLastHostSearchText( ESearchType searchType, std::string& searchText );
    void						setLastHostSearchAgeType( ESearchType searchType, EAgeType ageType );
    void						getLastHostSearchAgeType( ESearchType searchType, EAgeType& ageType );
    void						setLastHostSearchGender( ESearchType searchType, EGenderType genderType );
    void						getLastHostSearchGender( ESearchType searchType, EGenderType& genderType );
    void						setLastHostSearchLanguage( ESearchType searchType, ELanguageType languageType );
    void						getLastHostSearchLanguage( ESearchType searchType, ELanguageType& languageType );
    void						setLastHostSearchContentRating( ESearchType searchType, EContentRating contentRating );
    void						getLastHostSearchContentRating( ESearchType searchType, EContentRating& contentRating );

	void						setMicMuted( bool isMuted );
	bool						getMicMuted( void );

	void						setSpeakerMuted( bool isMuted );
	bool						getSpeakerMuted( void );

	void						setCamEnable( bool camEnable );
	bool						getCamEnable( void );

	void						setCamSourceId( std::string camId );
	std::string					getCamSourceId( void );

	void						setCamShowPreview( bool showPreview );
	bool						getCamShowPreview( void );

	void						setCamRotation( std::string camId, uint32_t camRotation );
	uint32_t					getCamRotation( std::string camId );

	void						setVidFeedRotation( uint32_t feedRotation );
	uint32_t					getVidFeedRotation( void );

    void						setLastAppletLaunched( ELaunchFrame launchPage, EApplet applet );
    EApplet						getLastAppletLaunched( ELaunchFrame launchPage );

    void						setLastUsedTestUrl( std::string& testUrl );
    void						getLastUsedTestUrl( std::string& testUrl );

    void                        setVerboseLog( bool verbose );
    bool                        getVerboseLog( void );

    void                        setLogLevels( uint32_t logLevelFlags );
    uint32_t                    getLogLevels( void );

    void                        setLogModules( uint32_t logModuleFlags );
    uint32_t                    getLogModules( void );

	void						setFavoriteHostGroupUrl( std::string& hostUrl );
	void						getFavoriteHostGroupUrl( std::string& hostUrl );

	void						setSoundInDeviceIndex( int32_t deviceIndex );
	int32_t						getSoundInDeviceIndex( void );

	void						setSoundOutDeviceIndex( int32_t deviceIndex );
	int32_t						getSoundOutDeviceIndex( void );

	void						setRunOnStartupCamServer( bool runOnStartup );
	bool						getRunOnStartupCamServer( void );

	void						setRunOnStartupFileShareServer( bool runOnStartup );
	bool						getRunOnStartupFileShareServer( void );

	void						setEchoDelayParam( int delayMs );
	int							getEchoDelayParam( void );

	void						setEchoCancelEnable( bool enable );
	bool						getEchoCancelEnable( void );

	void						setUseMilitaryTime( bool enable );
	bool						getUseMilitaryTime( void );

	void						setMaxMessageHistory( int32_t maxHistory );
	int32_t						getMaxMessageHistory( void );

	void						setWantMicrophone( bool enable );
	bool						getWantMicrophone( void );

	void						setMuteMicrophone( bool enable );
	bool						getMuteMicrophone( void );

	void						setWantSpeaker( bool enable );
	bool						getWantSpeaker( void );

	void						setMuteSpeaker( bool enable );
	bool						getMuteSpeaker( void );

	void						setSendMicInToSpeaker( bool enable );
	bool						getSendMicInToSpeaker( void );

	void						setLastUserConnectionsUserViewType( int comboIdx );
	int							getLastUserConnectionsUserViewType( void );

	void						setLastHostJoined( std::string lastJoinedHost );
	std::string					getLastHostJoined( void );

	void						setIsAutomatedHost( bool enable );
	bool						getIsAutomatedHost( void );

	void						setDisableSoundEffects( bool disable );
	bool						getDisableSoundEffects( void );

	void						setUseSystemMediaPlayer( bool useSystemPlayer );
	bool						getUseSystemMediaPlayer( void );

	void						setLastPlayedMovie( std::string& movieFile );
	void						getLastPlayedMovie( std::string& movieFile );

protected:
    std::string                 getAppendedType( const char* key, ESearchType searchType );

	bool						m_AppSettingsInitialized{ false };

	bool						m_DisableSoundEffectsCached{ false };
	bool						m_DisableSoundEffectsValue{ false };
};

