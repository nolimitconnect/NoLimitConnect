
#pragma once

#include "Application.h"
#include "AppParamParser.h"
#include "FileItem.h"

#include <GuiInterface/IMediaPlayerInterface.h>
#include <GuiInterface/IDefs.h>

#include <ptop_src/ptop_engine_src/AssetMgr/AssetInfo.h>

class MediaPlayerNlc : public CApplication, public IMediaPlayerRequests
{
public:
	MediaPlayerNlc();

	virtual enum EAppModule     getAppModule( void ) override;
    virtual bool                testQuitFlag() override;

	virtual void				fromGuiInitCommandLine( int argc, char** argv ) override;
	virtual void				fromStartModule( EAppModule appModule ) override;
	virtual void				fromStopModule( EAppModule appModule ) override;

	virtual bool				fromGuiPlayMedia( AssetBaseInfo& assetInfo, int pos0to100000 ) override;
	virtual bool				fromGuiMediaPlayerAction( EMediaPlayerAction playerAction ) override;
	virtual bool				fromGuiMediaPlayerSeek( int position0to100000 ) override;

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
