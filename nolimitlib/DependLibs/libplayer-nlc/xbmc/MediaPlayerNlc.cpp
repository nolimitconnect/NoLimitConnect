
#include "MediaPlayerNlc.h"

#include "ApplicationPlayer.h"

#include "NlcUrl.h"
#include "ServiceBroker.h"
#include "ServiceManager.h"

#include "windowing/WinSystem.h"
#include "windowing/GraphicContext.h"

#include "settings/DisplaySettings.h"
#include "settings/SettingsComponent.h"
#include "settings/Settings.h"
#include "settings/lib/SettingsManager.h"

#include <GuiInterface/IToGui.h>
#include <GuiInterface/OsInterface/OsInterface.h>
#include <GuiInterface/IMediaPlayerCallback.h>
#include <GuiInterface/IMediaPlayerRequests.h>

#include <CoreLib/VxDebug.h>

//CAppParamParser MediaPlayerNlc::m_AppParamParser;

//============================================================================
MediaPlayerNlc& GetNlcPlayerInstance()
{
    static MediaPlayerNlc g_NlcPlayer;
    return g_NlcPlayer;
}


//============================================================================
MediaPlayerNlc::MediaPlayerNlc()
	: CApplication()
{
}

//============================================================================
IMediaPlayerRequests& MediaPlayerNlc::getIMediaPlayerRequests( void )
{
	return *this;
}

//============================================================================
void MediaPlayerNlc::wantMediaPlayerCallback( IMediaPlayerCallback* client, bool wantCallback )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "MediaPlayerNlc::wantConnectIdListCallback null client" );
		vx_assert( false );
        return;
    }

    lockClientList();
    bool foundClient{ false };
    for( auto iter = m_MediaPlayerCallbackClients.begin(); iter != m_MediaPlayerCallbackClients.end(); ++iter )
    {
        if( *iter == client )
        {
            foundClient = true;
            if( !wantCallback )
            {
                m_MediaPlayerCallbackClients.erase( iter );
            }

            break;
        }
    }

    if( !foundClient && wantCallback )
    {
        m_MediaPlayerCallbackClients.push_back( client );
    }

    unlockClientList();
}

//============================================================================
enum EAppModule MediaPlayerNlc::getAppModule( void )
{ 
	return EAppModule::eAppModulePlayerNlc; 
}

//============================================================================
bool MediaPlayerNlc::testQuitFlag()
{
    // BRJ TODO implement
    return false;
}

//============================================================================
void MediaPlayerNlc::fromGuiInitCommandLine( int argc, char** argv )
{
	//m_AppParamParser.Parse( argv, argc );
}

//============================================================================
/// NOTE: called from RenderPlayerNlcThread::run()
bool MediaPlayerNlc::fromThreadStartModule( EAppModule appModule )
{
	if( eAppModulePlayerNlc == appModule && !m_ModuleIsRunning )
	{
		if( !m_ModuleIsInitialized )
		{
			m_ModuleIsInitialized = true;
			getOsInterface().doStartup();
		}

		m_ModuleIsRunning = true;
		getOsInterface().doRun( appModule ); // will stay in this function until kodi is shutdown
		m_ModuleIsRunning = false;
	}

	return true;
}

//============================================================================
bool MediaPlayerNlc::fromGuiStopModule( EAppModule appModule )
{
	if( eAppModulePlayerNlc == appModule && m_ModuleIsRunning )
	{
		//m_ModuleIsRunning = !INlcRender::getINlcRender().toGuiStopModule( eAppModulePlayerNlc );
	}

	return m_ModuleIsRunning;
}

//============================================================================
bool MediaPlayerNlc::fromGuiIsModuleRunning( EAppModule appModule )
{
	return m_ServiceManager && m_ServiceManager->GetInitLevel() >= 3;
}

//============================================================================
bool MediaPlayerNlc::assureInitialized( void )
{
	if( !m_ServiceManager.get() )
	{
		// has been shutdown.. restart
		LogModule( eLogPlayerNlc, LOG_ERROR, "media player could not be initialized" );
		return false;
	}

    if( !m_ServiceManager->HasPlayerFactory() )
    {
        LogModule( eLogPlayerNlc, LOG_ERROR, "media player has no player factory init level %d",
                  m_ServiceManager->GetInitLevel() );
        return false;
    }

    return true;
}

//============================================================================
bool MediaPlayerNlc::fromGuiPlayMedia( AssetBaseInfo& assetInfo, int pos0to100000 )
{
	fromGuiMediaPlayerAction( eMediaPlayerActionPlayStop );
    bool result{ false };
    if( !assureInitialized() )
    {
        LogModule( eLogPlayerNlc, LOG_ERROR, "media player not initialized" );
        return false;
    }

	// to do stop previous media
	if( assetInfo.getIsStream() || assetInfo.isValidFile() )
	{
		m_AssetInfo = assetInfo;
		if( !assetInfo.getIsStream() || !m_FeedId.isVxGUIDValid() )
		{
			m_FeedId = m_AssetInfo.getAssetUniqueId();
		}

		NlcUrl fileUrl;
		fileUrl.SetFileName( m_AssetInfo.getAssetName() );
		m_FileItem = CFileItem( fileUrl, false );
		m_FileItem.SetIsVirtualStream( assetInfo.getIsStream() );
		if( pos0to100000 )
		{
			std::string kodiPercent = std::to_string( 100000.0f / (float)pos0to100000 );
			m_FileItem.SetProperty( "StartPercent", kodiPercent.c_str() );
		}

		EnableLogTimer( true );

		if( m_AssetInfo.isVideoAsset() )
		{
			result = playVideoFile( pos0to100000 );
		}
		else if( m_AssetInfo.isAudioAsset() )
		{
			result = playAudioFile( pos0to100000 );
		}
	}

	return result;
}

//============================================================================
bool MediaPlayerNlc::fromGuiPlayStream( AssetBaseInfo& assetInfo, VxGUID lclSessionId, int pos0to100000 )
{
	m_FeedId = lclSessionId;
	return fromGuiPlayMedia( assetInfo, pos0to100000 );
}

//============================================================================
bool MediaPlayerNlc::fromGuiMediaPlayerAction( EMediaPlayerAction playerAction )
{
	bool result{ false };
	if( eMediaPlayerActionPlayStop == playerAction )
	{
		StopPlaying();
		return true;
	}

	assureInitialized();


	return result;
}

//============================================================================
bool MediaPlayerNlc::playAudioFile( int position0to100000 )
{
    assureInitialized();
	const std::string audioPlayerName( "audiodefaultplayer" );
	bool result = PlayFile( m_FileItem, audioPlayerName, false );

	return result;
}

//============================================================================
bool MediaPlayerNlc::playVideoFile( int position0to100000 )
{
    assureInitialized();
	const std::string videoPlayerName( "videodefaultplayer" );
	bool result = PlayFile( m_FileItem, videoPlayerName, false );

	return result;
}

//============================================================================
void MediaPlayerNlc::onInitLevel( int level, bool success )
{
	if( level == 3 && success )
	{
		initVideoSettings();
	}

	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerInitLevel( level, success );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlayerRunning( bool isRunning )
{
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerIsRunning( isRunning );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlayFile( void )
{
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerPlayFile( m_FeedId );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlayStarted( void )
{
	setIsPlayingMedia( true );
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerPlayStarted( m_FeedId );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onStopPlaying( void )
{
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerStopPlaying( m_FeedId );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlaybackStopped( void )
{
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerPlaybackStopped( m_FeedId );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlaybackEnded( void )
{
	setIsPlayingMedia( false );
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerPlaybackEnded( m_FeedId );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlayPause( bool isPaused )
{
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerPlayPause( m_FeedId, isPaused );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onCanSeek( bool canSeek, bool canPause )
{
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerCanSeek( m_FeedId, canSeek, canPause );
    }

    unlockClientList();
}

//============================================================================
void MediaPlayerNlc::fromGuiPlayPauseButtonClicked( void )
{
	PlayPauseButtonClicked();
}

//============================================================================
void MediaPlayerNlc::fromGuiStopButtonClicked( void )
{
	fromGuiMediaPlayerAction( eMediaPlayerActionPlayStop );
}

//============================================================================
void MediaPlayerNlc::fromGuiGetCanSeek( void )
{
	const auto appPlayer = GetComponent<CApplicationPlayer>();
	if( appPlayer )
	{
		bool canSeek = appPlayer->CanSeek();
		bool canPause = appPlayer->CanPause();
		lockClientList();
		for( auto client : m_MediaPlayerCallbackClients )
		{
			client->fromMediaPlayerCanSeek( m_FeedId, canSeek, canPause );
		}

		unlockClientList();
	}
}

//============================================================================
bool MediaPlayerNlc::fromGuiMediaPlayerSeek( int position0to100000 )
{
	bool result{ false };
	assureInitialized();

	float playPos = (float)(position0to100000) / 1000.0f;
	const auto appPlayer = GetComponent<CApplicationPlayer>();
	if( appPlayer )
	{
		if( appPlayer->CanSeek() )
		{
			appPlayer->SeekPercentage(playPos);
			result = true;
		}
	}

	return result;
}

//============================================================================
void MediaPlayerNlc::fromGuiUpdatePlayPosition( void )
{
	int playPos{ 0 };
	const auto appPlayer = GetComponent<CApplicationPlayer>();
	if( appPlayer && appPlayer->IsPlaying() )
	{
		playPos = (int)(appPlayer->GetPercentage() * 1000);
	}

	lockClientList();
	for( auto client : m_MediaPlayerCallbackClients )
	{
		client->fromMediaPlayerUpdatePlayPosition( m_FeedId, playPos );
	}

	unlockClientList();
}

//============================================================================
void MediaPlayerNlc::onPlaybackPaused( void ) 
{
	onPlayPause( true );
}

//============================================================================
void MediaPlayerNlc::onPlaybackResumed( void )  
{
	onPlayPause( false );
}

//============================================================================
void MediaPlayerNlc::onPlaybackError( void ) 
{
	onPlaybackStopped();
}

//============================================================================
void MediaPlayerNlc::fromGuiUpdateGlWidgetSize( int width, int height ) 
{
	//if( CServiceBroker::GetWinSystem() )
	//{
	//	CServiceBroker::GetWinSystem()->GetGfxContext().ApplyWindowResize( width, height );
	//}		
}

//============================================================================
void MediaPlayerNlc::initVideoSettings( void )
{
	// setup video settings because we do not read from files
	//CDisplaySettings::GetInstance()

	auto settingsComponent = CServiceBroker::GetSettingsComponent();
	if (!settingsComponent)
		return;

	auto settings = settingsComponent->GetSettings();
	if (!settings)
		return;

	SettingPtr renderWidth = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_WINDOW_WIDTH, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( renderWidth );
    settings->SetInt( CSettings::SETTING_WINDOW_WIDTH, 320 );

	SettingPtr renderHeight = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_WINDOW_HEIGHT, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( renderHeight );
    settings->SetInt( CSettings::SETTING_WINDOW_HEIGHT, 240 );

    SettingPtr useDisplayAsClock = settings->GetSettingsManager()->CreateSetting( "boolean", CSettings::SETTING_VIDEOPLAYER_USEDISPLAYASCLOCK, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( useDisplayAsClock );
    settings->SetBool( CSettings::SETTING_VIDEOPLAYER_USEDISPLAYASCLOCK, false );

	SettingPtr subsCustomPath = settings->GetSettingsManager()->CreateSetting( "string", CSettings::SETTING_SUBTITLES_CUSTOMPATH, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( subsCustomPath );
    settings->SetString( CSettings::SETTING_SUBTITLES_CUSTOMPATH, "" );

	SettingPtr audioPassthrough = settings->GetSettingsManager()->CreateSetting( "boolean", CSettings::SETTING_AUDIOOUTPUT_PASSTHROUGH, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( audioPassthrough );
    settings->SetBool( CSettings::SETTING_AUDIOOUTPUT_PASSTHROUGH, false );

	SettingPtr localeSubLanguage = settings->GetSettingsManager()->CreateSetting( "string", CSettings::SETTING_LOCALE_SUBTITLELANGUAGE, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( localeSubLanguage );
    settings->SetString( CSettings::SETTING_LOCALE_SUBTITLELANGUAGE, "original" );

	SettingPtr stereoscopicPlayback = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_VIDEOPLAYER_STEREOSCOPICPLAYBACKMODE, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( stereoscopicPlayback );
    settings->SetInt( CSettings::SETTING_VIDEOPLAYER_STEREOSCOPICPLAYBACKMODE, 0 ); // should probably be 100 (ignore) instead of ask
	
	SettingPtr parseCaptions = settings->GetSettingsManager()->CreateSetting( "boolean", CSettings::SETTING_SUBTITLES_PARSECAPTIONS, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( parseCaptions );
    settings->SetBool( CSettings::SETTING_SUBTITLES_PARSECAPTIONS, false );

	SettingPtr playerStretch = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_VIDEOPLAYER_STRETCH43, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( playerStretch );
    settings->SetInt( CSettings::SETTING_VIDEOPLAYER_STRETCH43, 0 );

	SettingPtr renderMethod = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_VIDEOPLAYER_RENDERMETHOD, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( renderMethod );
    settings->SetInt( CSettings::SETTING_VIDEOPLAYER_RENDERMETHOD, 0 );
	
	SettingPtr noofBuffers = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_VIDEOSCREEN_NOOFBUFFERS, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( noofBuffers );
    settings->SetInt( CSettings::SETTING_VIDEOSCREEN_NOOFBUFFERS, 3 );

	SettingPtr allowAspectErr = settings->GetSettingsManager()->CreateSetting( "integer", CSettings::SETTING_VIDEOPLAYER_ERRORINASPECT, settings->GetSettingsManager() );
    settings->GetSettingsManager()->AddSettingInternal( allowAspectErr );
    settings->SetInt( CSettings::SETTING_VIDEOPLAYER_ERRORINASPECT, 0 );

}