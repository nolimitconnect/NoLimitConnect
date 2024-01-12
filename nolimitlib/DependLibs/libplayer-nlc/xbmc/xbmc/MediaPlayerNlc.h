
#pragma once

#include "Application.h"
#include "AppParamParser.h"
#include "FileItem.h"

#include <GuiInterface/IMediaPlayerInterface.h>
#include <GuiInterface/IDefs.h>

#include <AssetMgr/AssetInfo.h>

class MediaPlayerNlc : public CApplication, public IMediaPlayerRequests
{
public:
	MediaPlayerNlc();

	enum EAppModule				getAppModule( void ) override;
    bool						testQuitFlag() override;

	void						fromGuiInitCommandLine( int argc, char** argv ) override;

	bool						fromStartModule( EAppModule appModule ) override;
	bool						fromStopModule( EAppModule appModule ) override;
	bool						fromGuiIsModuleRunning( EAppModule appModule ) override;

	bool						fromGuiPlayMedia( AssetBaseInfo& assetInfo, int pos0to100000 ) override;
	bool						fromGuiMediaPlayerAction( EMediaPlayerAction playerAction ) override;
	bool						fromGuiMediaPlayerSeek( int position0to100000 ) override;

protected:
    bool						assureInitialized( void );

	bool						playAudioFile( int position0to100000 );
	bool						playVideoFile( int position0to100000 );

	//=== vars ===//
	static CAppParamParser		m_AppParamParser;
	AssetBaseInfo				m_AssetInfo;
	CFileItem					m_FileItem;

	bool						m_ModuleIsInitialized{ false };
	bool						m_ModuleIsRunning{ false };
};
