#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseFilesServer.h"

#include <Plugins/FileInfoLibraryMgr.h>

class PluginFileShareServer : public PluginBaseFilesServer
{
public:
	PluginFileShareServer( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* netIdent, EPluginType pluginType );
	virtual ~PluginFileShareServer() = default;

	virtual void				onNetworkConnectionReady( bool requiresRelay ) override;

	virtual void				updateSharedFilesInfo( void );

	void						deleteFile( std::string fileName, bool shredFile ) override;

	void						onPktStreamCtrlReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

protected:
	virtual void				onFilesChanged( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes ) override;
};


