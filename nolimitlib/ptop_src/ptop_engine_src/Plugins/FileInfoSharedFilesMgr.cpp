//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileInfoSharedFilesMgr.h"
#include <ptop_src/ptop_engine_src/Plugins/FileInfo.h>

#include "PluginBase.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/Plugins/PluginBase.h>
#include <GuiInterface/IToGui.h>

#include <PktLib/PktAnnounce.h>
#include <PktLib/PktsFileList.h>
#include <PktLib/PktsFileInfo.h>

#include <CoreLib/Sha1GeneratorMgr.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxSha1Hash.h>
#include <CoreLib/VxFileShredder.h>

//============================================================================
FileInfoSharedFilesMgr::FileInfoSharedFilesMgr( P2PEngine& engine, PluginBase& plugin, std::string sharedFilesDbName )
	: FileInfoDb( sharedFilesDbName )
	, FileInfoBaseMgr( engine, plugin, *this )
{
	LogMsg( LOG_VERBOSE, "FileInfoSharedFilesMgr::FileInfoSharedFilesMgr %s %p", DescribePluginType( plugin.getPluginType() ), this );
}

//============================================================================
FileInfoSharedFilesMgr::~FileInfoSharedFilesMgr()
{
	fileInfoMgrShutdown();
}
