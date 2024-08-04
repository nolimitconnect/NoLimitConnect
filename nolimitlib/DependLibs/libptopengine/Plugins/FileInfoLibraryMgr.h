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

#include "FileInfoDb.h"
#include "FileInfoBaseMgr.h"

#include <PktLib/VxCommon.h>

#include <CoreLib/VxThread.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/Sha1GeneratorCallback.h>

class FileInfo;
class IToGui;
class PluginBase;
class P2PEngine;
class PktFileListReply;
class PktFileInfoSearchReply;
class PktFileInfoMoreReply;
class SearchParams;
class SharedFilesMgr;
class VxSha1Hash;
class VxFileShredder;

class FileInfoLibraryMgr : public FileInfoDb, public FileInfoBaseMgr
{
public:
	FileInfoLibraryMgr() = delete;
	FileInfoLibraryMgr( const FileInfoLibraryMgr& rhs ) = delete;
	FileInfoLibraryMgr( P2PEngine& engine, PluginBase& plugin, std::string fileDbName );
	virtual ~FileInfoLibraryMgr();

	FileInfoLibraryMgr& operator=( const FileInfoLibraryMgr& rhs ) = delete;

};

