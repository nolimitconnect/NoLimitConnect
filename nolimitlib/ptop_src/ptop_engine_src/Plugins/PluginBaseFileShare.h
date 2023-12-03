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

#include "PluginBaseFiles.h"
#include "PluginSessionMgr.h"
#include "FileInfoSharedFilesMgr.h"

#include <ptop_src/ptop_engine_src/P2PEngine/FileShareSettings.h>

#include <PktLib/VxCommon.h>

class PktFileListReply;
class FileShareSettings;
class FileInfo;
class P2PEngine;
class IToGui;
class VxNetIdent;
class FileTxSession;
class FileRxSession;
class VxFileShredder;

class PluginBaseFileShare : public PluginBaseFiles
{
public:
	PluginBaseFileShare( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType,  std::string fileShareDbName );
	virtual ~PluginBaseFileShare() = default;


	virtual void				onSharedFilesUpdated( uint16_t u16FileTypes );

protected:
	FileInfoSharedFilesMgr		m_FileInfoSharedFilesMgr;
};


