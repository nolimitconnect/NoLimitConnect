
#include "MediaPlayerNlc.h"

#include "ServiceManager.h"
#include "NlcUrl.h"

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
bool MediaPlayerNlc::fromStartModule( EAppModule appModule )
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
bool MediaPlayerNlc::fromStopModule( EAppModule appModule )
{
	if( eAppModulePlayerNlc == appModule && m_ModuleIsRunning )
	{
		m_ModuleIsRunning = !INlc::getINlc().toGuiStopModule( eAppModulePlayerNlc );
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
	if( assetInfo.isValidFile() )
	{
		m_AssetInfo = assetInfo;
		NlcUrl fileUrl;
		fileUrl.SetFileName( m_AssetInfo.getAssetName() );
		m_FileItem = CFileItem( fileUrl, false );
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
bool MediaPlayerNlc::fromGuiMediaPlayerSeek( int position0to100000 )
{
	bool result{ false };
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
