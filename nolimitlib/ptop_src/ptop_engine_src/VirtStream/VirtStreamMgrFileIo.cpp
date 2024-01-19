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
#include <Plugins/PluginMgr.h>
#include <Plugins/PluginFileShareClient.h>

#include <CoreLib/VFile.h>
#include <CoreLib/VxFileUtil.h>

#include <NetLib/VxSktBase.h>

//============================================================================
VFile* VirtStreamMgr::fileOpen( const char* fileName, const char* fileMode )
{
	std::string mode( fileMode );
	vx_assert( mode.size() );

	bool virtStream{ false };
	if( mode.size() > 0 )
	{
		std::size_t foundPos = mode.find_last_of("v");
		if( foundPos > 0 )
		{
			virtStream = true;
			mode = mode.substr( 0, foundPos );
		}

		if( virtStream )
		{
			return virtFileOpen( fileName, mode );
		}
	}

	VFile* vFile = (VFile*)malloc( sizeof( VFile ) );
	vx_assert( vFile );
	memset( vFile, 0, sizeof( VFile ) );

	vFile->m_FileLen = VxFileUtil::fileExists( fileName );
	FILE* fp = fopen( fileName, mode.c_str() );
	vFile->m_Error = VxGetLastError();

	LogModule( eLogMediaStream, LOG_VERBOSE, "%s filname %s mode %s fp %p", __func__, fileName, fileMode, fp );

	if( fp )
	{
		vFile->m_FILE = fp;	
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
	free( fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VirtStreamMgr::fileError( VFile* fp )
{
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
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
	LogModule( eLogMediaStream, LOG_VERBOSE, "%s fp %p", __func__, fp );
	return result;
}

//============================================================================
//============================================================================
	
//============================================================================
VFile* VirtStreamMgr::virtFileOpen( std::string fileName, std::string fileMode )
{
	VxFileUtil::makeForwardSlashPath( fileName );

	std::string streamFileName;
	VxFileUtil::makeShortFileName( fileName.c_str(), streamFileName );

	std::string assetFileName;
	VxFileUtil::makeShortFileName( m_LiveStream.m_StreamAssetInfo.getAssetName().c_str(), assetFileName );

	if( streamFileName != assetFileName )
	{
		LogMsg( LOG_ERROR, "%s fileName %s does not match asset name %s",
				__func__, fileName.c_str(), m_LiveStream.m_StreamAssetInfo.getAssetName().c_str() );
		return nullptr;
	}

	if( !m_LiveStream.isConnected() )
	{
		LogMsg( LOG_WARN, "%s user %s is no longer connected", __func__,
				m_Engine.describeUser( m_LiveStream.m_StreamAssetInfo.getDestUserId() ).c_str() );
		return nullptr;
	}

	VFile* vFile = new VFile();
	memset( vFile, 0, sizeof( VFile ) );
	vFile->m_FileLen = m_LiveStream.m_StreamAssetInfo.getAssetLength();
	vFile->m_VirtFileType = (int16_t)m_LiveStream.m_StreamAssetInfo.getAssetType();

	lockSteamMgr();
	if( m_LiveStream.m_VFile )
	{
		delete m_LiveStream.m_VFile;
		m_LiveStream.m_VFile = nullptr;
	}

	m_LiveStream.m_VFile = vFile;
	unlockSteamMgr();
	return vFile;
}

//============================================================================
int VirtStreamMgr::virtFileClose( VFile* fp )
{
	int retVal = -1;
	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return retVal;
	}
		
	delete m_LiveStream.m_VFile;
	m_LiveStream.m_VFile = nullptr;
	m_LiveStream.m_StreamCache.clearCache( false );

	unlockSteamMgr();
	return retVal;
}

//============================================================================
int VirtStreamMgr::virtFileEof( VFile* fp )
{
	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return 0;
	}

	bool eof = m_LiveStream.m_VFile->m_FileOffs == m_LiveStream.m_VFile->m_FileLen;
	unlockSteamMgr();
	return eof;
}

//============================================================================
int VirtStreamMgr::virtFileError( VFile* fp )
{
	int retVal = -1;
	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return retVal;
	}
	
	m_LiveStream.isConnected();
	retVal = m_LiveStream.getError();
	unlockSteamMgr();
	return retVal;
}

//============================================================================
int VirtStreamMgr::virtFileFlush( VFile* fp )
{
	return 0;
}

//============================================================================
size_t VirtStreamMgr::virtFileRead( void* buf, size_t size, size_t count, VFile* fp )
{
	int retVal = -1;

	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return retVal;
	}

	m_LiveStream.isConnected();
	if( m_LiveStream.getError() )
	{
		retVal = m_LiveStream.getError();
		unlockSteamMgr();
		return retVal;
	}

	int64_t readAttemptLen = size * count;
	if( !waitForStream( m_LiveStream.m_VFile->m_FileOffs, readAttemptLen ) )
	{
		unlockSteamMgr();
		LogModule( eLogMediaStream, LOG_ERROR, "%s timeout waiting for stream file %s at offs%" PRId64 " len%s" PRId64,
				   m_LiveStream.m_StreamAssetInfo.getAssetName().c_str(), m_LiveStream.m_VFile->m_FileOffs, readAttemptLen );
		return retVal;
	}
		

	int64_t readLen = m_LiveStream.m_StreamCache.readData( m_LiveStream.m_VFile->m_FileOffs, (char*)buf, readAttemptLen );
	if( readLen == readAttemptLen )
	{
		m_LiveStream.m_VFile->m_FileOffs += readLen;
		retVal = 0;
	}

	unlockSteamMgr();
	return retVal;
}

//============================================================================
size_t VirtStreamMgr::virtFileWrite(const void* buf, size_t size, size_t count, VFile* fp)
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileGetC( VFile* fp )
{
	int retVal = -1;

	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return retVal;
	}

	m_LiveStream.isConnected();
	if( m_LiveStream.getError() )
	{
		retVal = m_LiveStream.getError();
		unlockSteamMgr();
		return retVal;
	}

	int64_t readAttemptLen = 1;
	char retChar[1];
	int64_t readLen = m_LiveStream.m_StreamCache.readData( m_LiveStream.m_VFile->m_FileOffs, (char*)retChar, readAttemptLen );
	if( readLen == readAttemptLen )
	{
		m_LiveStream.m_VFile->m_FileOffs += readLen;
		unlockSteamMgr();
		return retChar[0];
	}

	return -1;
}

//============================================================================
char* VirtStreamMgr::virtFileGetS( char* buf, int size, VFile* fp )
{
	int retVal = -1;

	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return nullptr;
	}

	m_LiveStream.isConnected();
	if( m_LiveStream.getError() )
	{
		unlockSteamMgr();
		return nullptr;
	}
	
	std::string readStr;
	int result = -1;
	int64_t readIdx = m_LiveStream.m_VFile->m_FileOffs;
	bool foundEnd{ false };
	while( !foundEnd && readIdx < m_LiveStream.m_VFile->m_FileLen && readIdx < size )
	{
		char retChar[1];
		int64_t readLen = m_LiveStream.m_StreamCache.readData( readIdx + readStr.size(), (char*)retChar, 1);
		if( readLen )
		{
			readStr.push_back( retChar[0] );
			if( retChar[0] == '\n' )
			{
				readStr.push_back( 0 );
				result = 0;
				foundEnd = true;
				break;
			}
		}
		else
		{
			break;
		}
	}
	
	unlockSteamMgr();
	if( result == 0 )
	{
		memcpy( buf, readStr.c_str(), readStr.length() );
		return buf;
	}

	return nullptr;
}

//============================================================================
int VirtStreamMgr::virtFilePutC(int ch, VFile* fp)
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::virtFilePutS(const char* s, VFile* fp)
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileGetPos( VFile* fp, fpos_t* pos )
{
	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return -1;
	}

	*pos = m_LiveStream.m_VFile->m_FileOffs;
	unlockSteamMgr();
	return 0;
}

//============================================================================
int VirtStreamMgr::virtFileSetPos( VFile* fp, const fpos_t* pos )
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::virtFileSeek( VFile* fp, size_t offset, int whence )
{
	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return -1;
	}

	int64_t origPos = m_LiveStream.m_VFile->m_FileOffs;
	switch( whence )
	{
	case SEEK_SET:
		// Beginning of file
		m_LiveStream.m_VFile->m_FileOffs = offset;
		break;

	case SEEK_CUR:
		// Current position of the file pointer
		m_LiveStream.m_VFile->m_FileOffs += offset;
		break;

	case SEEK_END:
		m_LiveStream.m_VFile->m_FileOffs = m_LiveStream.m_VFile->m_FileLen + offset;
		break;
	}

	int64_t newPos = m_LiveStream.m_VFile->m_FileOffs;
	if( newPos < 0 )
	{
		m_LiveStream.m_VFile->m_FileOffs = 0;
		LogMsg( LOG_ERROR, "%s invalid pos" PRId64, __func__, newPos );
	}

	unlockSteamMgr();

	if( newPos >= 0 && newPos != origPos )
	{
		sendStreamSeek( newPos );
		return 0;
	}

	return -1;
}

//============================================================================
fpos_t VirtStreamMgr::virtFileTell( VFile* fp )
{
	lockSteamMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "%s wrong VFile", __func__ );
		unlockSteamMgr();
		vx_assert( false );
		return -1;
	}

	int64_t fPos = m_LiveStream.m_VFile->m_FileOffs;
	unlockSteamMgr();
	return fPos;
}
