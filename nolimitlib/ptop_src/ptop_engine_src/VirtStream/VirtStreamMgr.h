//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include "VirtStreamFile.h"

#include <CoreLib/VirtFileMgr.h>

class P2PEngine;

class VirtStreamMgr : public VirtFileMgr
{
public:
	VirtStreamMgr() = delete;
	VirtStreamMgr( P2PEngine& engine );

	VFile*						fileOpen( const char* fileName, const char* fileMode )  override;
	int							fileClose( VFile* vFile )  override;
	int							fileEof( VFile* fp )  override;
	int							fileError( VFile* fp )  override;
	int							fileFlush( VFile* fp )  override;
	size_t						fileRead( void* buf, size_t size, size_t count, VFile* fp ) override;
	size_t						fileWrite( const void* buf, size_t size, size_t count, VFile* fp ) override;
	int							fileGetC( VFile* fp ) override;
	char*						fileGetS( char* buf, int size, VFile* fp ) override;
	int							filePutC( int ch, VFile* fp ) override;
	int							filePutS( const char* s, VFile* fp ) override;
	int							fileGetPos( VFile* fp, fpos_t* pos ) override;
	int							fileSetPos( VFile* fp, const fpos_t* pos ) override;
	int							fileSeek( VFile* fp, size_t offset, int whence ) override;
	fpos_t						fileTell( VFile* fp ) override;

protected:
	int							isVirtualFile( struct VFile* vFile ) { return vFile->m_VirtFileType > 0; }
	
	VFile*						virtFileOpen( const char* fileName, const char* fileMode );
	int							virtFileClose(VFile* fp);
	int							virtFileEof(VFile* fp);
	int							virtFileError(VFile* fp);
	int							virtFileFlush(VFile* fp);

	size_t						virtFileRead(void* buf, size_t size, size_t count, VFile* fp);
	size_t						virtFileWrite(const void* buf, size_t size, size_t count, VFile* fp);

	int							virtFileGetC(VFile* fp);
	char*						virtFileGetS(char* buf, int size, VFile* fp);
	int							virtFilePutC(int ch, VFile* fp);
	int							virtFilePutS(const char* s, VFile* fp);

	int							virtFileGetPos( VFile* fp, fpos_t* pos );
	int							virtFileSetPos( VFile* fp, const fpos_t* pos );
	int							virtFileSeek( VFile* fp, size_t offset, int whence);
	fpos_t						virtFileTell( VFile* fp );

	P2PEngine&					m_Engine;
};
