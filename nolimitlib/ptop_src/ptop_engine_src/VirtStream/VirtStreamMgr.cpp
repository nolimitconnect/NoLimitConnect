//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VirtStreamMgr.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VFile.h>

VirtFileMgr& GetVirtFileMgr()
{
	static VirtStreamMgr virtStreamMgr(GetPtoPEngine());
	return virtStreamMgr;
}

//============================================================================
VirtStreamMgr::VirtStreamMgr( P2PEngine& engine )
	: m_Engine( engine )
{

}

//============================================================================
VFile* VirtStreamMgr::fileOpen( const char* fileName, const char* fileMode )
{
	vx_assert( strlen( fileMode  ) );
	if( strlen( fileMode ) > 0 )
	{
		const char* virtPos = strrchr( fileMode, 'v' );
		if( virtPos )
		{
			return virtFileOpen( fileName, fileMode );
		}
	}

	VFile* vFile = (VFile*)malloc( sizeof( VFile ) );
	FILE* fp = fopen( fileName, fileMode );
	if( fp )
	{
		vFile->m_FILE = fp;
		vFile->m_Error = VxGetLastError();
		return vFile;
	}
	else
	{
		free( vFile );
	}

	return 0;
}

//============================================================================
int VirtStreamMgr::fileClose( VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileClose( fp );
	}

	int result = fclose( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	free( fp );
	return result;
	return result;
}

//============================================================================
int VirtStreamMgr::fileEof( VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileEof( fp );
	}

	int result = feof( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::fileError( VFile* fp )
{
	return fp->m_Error;
}

//============================================================================
int VirtStreamMgr::fileFlush( VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileFlush( fp );
	}

	int result = fflush( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
size_t VirtStreamMgr::fileRead( void* buf, size_t size, size_t count, VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileRead( buf, size, count, fp );
	}

	int result = fread( buf, size, count, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
size_t VirtStreamMgr::fileWrite( const void* buf, size_t size, size_t count, VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileWrite( buf, size, count, fp );
	}

	int result = fwrite( buf, size, count, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::fileGetC( VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileGetC( fp );
	}

	int result = fgetc( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
char*  VirtStreamMgr::fileGetS( char* buf, int size, VFile* fp ) 
{
	if( isVirtualFile( fp ) )
	{
		return virtFileGetS( buf, size, fp );
	}

	char* result = fgets( buf, size, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::filePutC( int ch, VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFilePutC( ch, fp );
	}

	int result = fputc( ch, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::filePutS( const char* s, VFile* fp ) 
{
	if( isVirtualFile( fp ) )
	{
		return virtFilePutS( s, fp );
	}

	int result = fputs( s, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::fileGetPos( VFile* fp, fpos_t* pos )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileGetPos( fp, pos );
	}

	int result = fgetpos( fp->m_FILE, pos );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::fileSetPos( VFile* fp, const fpos_t* pos )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileSetPos( fp, pos );
	}

	int result = fsetpos( fp->m_FILE, pos );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
int VirtStreamMgr::fileSeek( VFile* fp, size_t offset, int whence )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileSeek( fp, offset, whence );
	}

	int result = fseek( fp->m_FILE, offset, whence );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
fpos_t VirtStreamMgr::fileTell( VFile* fp )
{
	if( isVirtualFile( fp ) )
	{
		return virtFileTell( fp );
	}

	fpos_t result = ftell( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	return result;
}

//============================================================================
//============================================================================
	
//============================================================================
VFile* VirtStreamMgr::virtFileOpen( const char* fileName, const char* fileMode )
{

	return nullptr;
}

//============================================================================
int VirtStreamMgr::virtFileClose( VFile* fp )
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileEof(VFile* fp)
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileError(VFile* fp)
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileFlush( VFile* fp )
{
	return 0;
}

//============================================================================
size_t VirtStreamMgr::virtFileRead( void* buf, size_t size, size_t count, VFile* fp )
{

	return 0;
}

//============================================================================
size_t VirtStreamMgr::virtFileWrite(const void* buf, size_t size, size_t count, VFile* fp)
{

	return 0;
}

//============================================================================
int VirtStreamMgr::virtFileGetC( VFile* fp )
{
	return -1;
}

//============================================================================
char* VirtStreamMgr::virtFileGetS(char* buf, int size, VFile* fp)
{

	return nullptr;
}

//============================================================================
int VirtStreamMgr::virtFilePutC(int ch, VFile* fp)
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFilePutS(const char* s, VFile* fp)
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileGetPos( VFile* fp, fpos_t* pos )
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileSetPos( VFile* fp, const fpos_t* pos )
{

	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileSeek( VFile* fp, size_t offset, int whence)
{

	return -1;
}

//============================================================================
fpos_t VirtStreamMgr::virtFileTell( VFile* fp )
{
	return 0;
}