
#include "MediaPlayerNlc.h"

#include "ApplicationPlayer.h"
#include "NlcUrl.h"
#include "ServiceManager.h"

#include "../../../../../nolimitgui/AppInterface/INlc.h"
#include "../../../../GuiInterface/OsInterface/OsInterface.h"

#include <CoreLib/VxDebug.h>

CAppParamParser MediaPlayerNlc::m_AppParamParser;

//============================================================================
MediaPlayerNlc::MediaPlayerNlc()
	: CApplication()
{
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
	m_AppParamParser.Parse( argv, argc );
}

//============================================================================
/// NOTE: 
bool MediaPlayerNlc::fromGuiStartModule( EAppModule appModule )
{
	if( eAppModulePlayerNlc == appModule && !m_ModuleIsRunning )
	{
		// this thread is from RenderPlayerNlcThread::run()
		// INlc::getINlc().getOsInterface().doPreStartup(); initalized by NLC
		if( !m_ModuleIsInitialized )
		{
			m_ModuleIsInitialized = true;
			INlc::getINlc().getOsInterface().doStartup();
			INlc::getINlc().getOsInterface().initRun( m_AppParamParser );
		}

		m_ModuleIsRunning = true;
		INlc::getINlc().getOsInterface().doRun( appModule ); // will stay in this function until kodi is shutdown
		m_ModuleIsRunning = false;
	}

	return true;
}

//============================================================================
bool MediaPlayerNlc::fromGuiStopModule( EAppModule appModule )
{
	if( eAppModulePlayerNlc == appModule && m_ModuleIsRunning )
	{
		//m_ModuleIsRunning = !INlc::getINlc().toGuiStopModule( eAppModulePlayerNlc );
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
	bool isInitialize{ false };
	if( !m_ServiceManager.get() )
	{
		// has been shutdown.. restart
		isInitialize = INlc::getINlc().toGuiRunModule( eAppModulePlayerNlc );
	}

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
	lockClientList();
    for( auto client : m_MediaPlayerCallbackClients )
    {
		client->fromMediaPlayerInitLevel( level, success );
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
