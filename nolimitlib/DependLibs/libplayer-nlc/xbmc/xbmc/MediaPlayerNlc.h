
#pragma once

#include "Application.h"
#include "AppParamParser.h"
#include "FileItem.h"

#include <GuiInterface/IMediaPlayerInterface.h>
#include <GuiInterface/IDefs.h>

#include <AssetMgr/AssetInfo.h>

#include <CoreLib/VxMutex.h>

class MediaPlayerNlc : public CApplication, public IMediaPlayerRequests
{
public:
	MediaPlayerNlc();

	void						wantMediaPlayerCallback( IMediaPlayerCallback* client, bool wantCallback );

	enum EAppModule				getAppModule( void ) override;
    bool						testQuitFlag() override;

	void						fromGuiInitCommandLine( int argc, char** argv ) override;

	bool						fromStartModule( EAppModule appModule ) override;
	bool						fromStopModule( EAppModule appModule ) override;
	bool						fromGuiIsModuleRunning( EAppModule appModule ) override;

	bool						fromGuiPlayMedia( AssetBaseInfo& assetInfo, int pos0to100000 ) override;
	bool						fromGuiMediaPlayerAction( EMediaPlayerAction playerAction ) override;
	bool						fromGuiMediaPlayerSeek( int position0to100000 ) override;

	bool						fromGuiPlayStream( std::string url, int pos0to100000 ) override;

protected:
    bool						assureInitialized( void );

	bool						playAudioFile( int position0to100000 );
	bool						playVideoFile( int position0to100000 );

	void						onInitLevel( int initLevel, bool success ) override;

	void						onPlayFile( void ) override;
    void						onPlayStarted( void ) override;
    void						onStopPlaying( void ) override;
    void						onPlaybackStopped( void )  override;
    void						onPlaybackEnded( void ) override;

	void						lockClientList( void ) { m_MediaPlayerCallbackMutex.lock(); }
	void						unlockClientList( void ) { m_MediaPlayerCallbackMutex.unlock(); }

	//=== vars ===//
	static CAppParamParser		m_AppParamParser;
	AssetBaseInfo				m_AssetInfo;
	CFileItem					m_FileItem;

	bool						m_ModuleIsInitialized{ false };
	bool						m_ModuleIsRunning{ false };

	std::vector<IMediaPlayerCallback*>    m_MediaPlayerCallbackClients;
    VxMutex						m_MediaPlayerCallbackMutex;

	VxGUID						m_FeedId;
	VxGUID						m_SessionId;
};
