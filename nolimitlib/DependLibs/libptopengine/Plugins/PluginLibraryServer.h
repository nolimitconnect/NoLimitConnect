#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseFilesServer.h"

class PluginLibraryServer : public PluginBaseFilesServer
{
public:
	PluginLibraryServer( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* netIdent, EPluginType pluginType );
	virtual ~PluginLibraryServer() = default;

	virtual void				onNetworkConnectionReady( bool requiresRelay ) override;

	virtual void				updateSharedFilesInfo( void );

	virtual bool				fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool inLibrary );
	virtual bool				fromGuiSetFileIsInLibrary( std::string& fileName, bool inLibrary );

	virtual bool				fromGuiGetFileIsInLibrary( FileInfo& fileInfo );
	virtual void				fromGuiGetFileLibraryList( VxGUID& appInstId, uint8_t fileTypeFilter );
	virtual bool				fromGuiGetIsFileInLibrary( std::string& fileName );

	virtual bool				fromGuiRemoveFromLibrary( std::string& fileName );

	bool						isFileInLibrary( std::string& fileName );
	bool						isFileInLibrary( VxSha1Hash& fileHashId );
	bool						isFileInLibrary( VxGUID& assetId );

	bool						addFileToLibrary( FileInfo& fileInfo );

	void						deleteFile( std::string fileName, bool shredFile ) override;

protected:
	virtual void				onFilesChanged( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes ) override;
};


