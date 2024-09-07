//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VFileMgr.h"

#include <CoreLib/VxDefs.h>
#include <CoreLib/VFile.h>
#include <CoreLib/VxFileUtil.h>

#include <string.h>
#include <memory.h>

#ifdef TARGET_OS_WINDOWS
	#include "shlobj.h" // for VxGetMyDocumentsDir
	#include <direct.h>
#else
	#include <dirent.h> // for searching directories
	#include <unistd.h> 
	#include <sys/vfs.h>    
    #include <sys/statfs.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif


#define VERIFY_CACHE_DATA 0

namespace
{
	
	//============================================================================
	std::wstring Utf8ToWide(const std::string utf8string)
	{
	#ifdef TARGET_OS_WINDOWS
		std::wstring convertedString;
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, 0, 0);
		if(requiredSize > 0)
		{
			std::vector<wchar_t> buffer(requiredSize);
			MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &buffer[0], requiredSize);
			convertedString.assign(buffer.begin(), buffer.end() - 1);
		}

		return convertedString;
	#else
		//TODO Linux version
		size_t asciiSize = utf8string.length();
		wchar_t * buf = new wchar_t[ asciiSize + 2 ];
		buf[0] = 0;
		const char* pTemp = utf8string.c_str();
		for( unsigned int i = 0; i < asciiSize + 1; i++ )
		{
			buf[i] = (wchar_t)pTemp[i];
		}

		std::wstring strResult = buf;
		delete[] buf;
		return strResult;
	#endif // TARGET_OS_WINDOWS
	}

}

//============================================================================
uint64_t VFileMgr::fileExists( const char* fileName )
{
#if defined(TARGET_OS_ANDROID)
    if( fileIsProviderFile( fileName ) )
    {
        return providerFileExists( fileName );
    }
#endif // defined(TARGET_OS_ANDROID)
		int result;
#ifdef TARGET_OS_WINDOWS
	struct __stat64 gStat;
	// Get data associated with the file
	result = _wstat64( Utf8ToWide( fileName ).c_str(), &gStat );
#else
	struct stat64 gStat;
	// Get data associated with the file
	result = stat64( fileName, &gStat );
#endif //TARGET_OS_WINDOWS

	// Check if statistics are valid:
	if( result != 0 )
	{
		//error getting file info
#if defined(DEBUG)
        int errCode = VxGetLastError();
		LogMsg( LOG_DEBUG, "File Exists Error %d %s", errCode, fileName );
#endif // defined(DEBUG)etCurrentWorkingDirectory
		return 0;
	}
	else
	{
		//return file size
		return gStat.st_size;
	}
}

//============================================================================
bool VFileMgr::directoryExists( const char* dirPath )
{
    if( fileIsProviderFile( dirPath ) )
    {
        return providerDirectoryExists( dirPath );
    }

	char acBuf[ VX_MAX_PATH ];
	strcpy( acBuf, dirPath );
	bool bIsDir = true;
	struct stat oFileStat;

	if( strlen( acBuf ) > 3 )
	{
		//if not root of drive remove the trailing backslash
		if( ('/' == acBuf[ strlen( acBuf ) - 1 ]) ||
			('\\' == acBuf[ strlen( acBuf ) - 1 ]) )
		{
			acBuf[ strlen( acBuf ) - 1 ] = 0;
		}
	}

	memset( &oFileStat, 0, sizeof( struct stat ) );
#ifdef TARGET_OS_WINDOWS
	oFileStat.st_mode = _S_IFDIR; //check for dir not file
	if( 0 == stat( acBuf, &oFileStat ) )
	{
		if( false == ( oFileStat.st_mode & _S_IFDIR ))
		{
			//path is not valid directory
			bIsDir = false;
		}
	}
	else
	{
		bIsDir = false;
	}
#else // LINUX or android
	oFileStat.st_mode = S_IFDIR; //check for dir not file
	if( 0 == stat( acBuf, &oFileStat ) )
	{
		if( false == ( oFileStat.st_mode & S_IFDIR ))
		{
			//path is not valid directory
			bIsDir = false;
		}
	}
    else
	{
		bIsDir = false;
	}
#endif // LINUX
	return bIsDir;
}

//============================================================================
bool VFileMgr::fileIsProviderFile( const char* fileName )
{
#if defined(TARGET_OS_ANDROID)
    const int prefixLen = 10;
    const char* contentPrefix = "content://";
    return fileName && strlen( fileName ) > prefixLen && strncmp( fileName, contentPrefix, prefixLen ) == 0;
#else
    return false;
#endif // defined(TARGET_OS_ANDROID)
}
	
//============================================================================
bool VFileMgr::seperatePathAndFile( const char* fileNameAndPath,		// path and file name
										 std::string& strRetPath,	// return path of file
										 std::string& strRetFile )	// return  file name
{
    const std::string speratorStr( "%2F" );

	bool result{ false };
    std::string fullPath( fileNameAndPath );
	if( !fullPath.empty() )
	{
		size_t strPos = fullPath.rfind ( speratorStr );
		if( strPos != std::string::npos )
		{
            strRetFile = fullPath.substr( strPos + speratorStr.length(), fullPath.length() - strPos );
			strRetPath = fullPath.substr( 0, strPos );

		}
		else
		{
			strRetFile = fullPath;
			strRetPath.clear();
		}

		result = true;
	}

	return result;
}

//============================================================================
bool VFileMgr::getFileInfo( const char* fileNameAndPath, VxFileInfoBase& fileInfoBase )
{
    if( fileIsProviderFile( fileNameAndPath ) )
    {
        return providerGetFileInfo( fileNameAndPath, fileInfoBase );
    }

    return VxFileUtil::getFileInfo( fileNameAndPath, fileInfoBase);
}

//============================================================================
VFile* VFileMgr::fileOpen( const char* fileNameIn, const char* fileMode )
{
	vx_assert( fileNameIn );
    vx_assert( fileMode );

    if( fileIsProviderFile( fileNameIn ) )
    {
        return providerFileOpen( fileNameIn, fileMode );
    }

    std::string mode( fileMode );
    vx_assert( mode.size() );

    std::string fileName( fileNameIn );

	bool virtStream{ false };
	if( mode.size() > 0 )
	{
		std::size_t foundPos = mode.find_last_of("v");
		if( foundPos != std::string::npos )
		{
			virtStream = true;
			mode = mode.substr( 0, foundPos );
		}

		if( virtStream )
		{
			// return virtFileOpen( fileName, mode );
			return nullptr;
		}
	}

	VFile* vFile = (VFile*)malloc( sizeof( VFile ) );
	vx_assert( vFile );
	memset( vFile, 0, sizeof( VFile ) );

	vFile->m_FileLen = VxFileUtil::fileExists( fileName.c_str() );
	FILE* fp = fopen( fileName.c_str(), mode.c_str() );
	vFile->m_Error = VxGetLastError();

    LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s filename %s mode %s fp %p", __func__, fileName.c_str(), fileMode, fp );

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
int VFileMgr::fileClose( VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFileClose( fp );
	}

	int result = fclose( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	free( fp );
	return result;
}

//============================================================================
int VFileMgr::fileEof( VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFileEof( fp );
	}

	int result = feof( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::fileError( VFile* fp )
{
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return fp->m_Error;
}

//============================================================================
int VFileMgr::fileFlush( VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFileFlush( fp );
	}

	int result = fflush( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
size_t VFileMgr::fileRead( void* buf, size_t size, size_t count, VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFileRead( buf, size, count, fp );
	}

	int result = fread( buf, size, count, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
size_t VFileMgr::fileWrite( const void* buf, size_t size, size_t count, VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFileWrite( buf, size, count, fp );
	}

	size_t result = fwrite( buf, size, count, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::fileGetC( VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFileGetC( fp );
	}

	int result = fgetc( fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
char*  VFileMgr::fileGetS( char* buf, int size, VFile* fp ) 
{
	if( isProviderFile( fp ) )
	{
		return providerFileGetS( buf, size, fp );
	}

	char* result = fgets( buf, size, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::filePutC( int ch, VFile* fp )
{
	if( isProviderFile( fp ) )
	{
		return providerFilePutC( ch, fp );
	}

	int result = fputc( ch, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::filePutS( const char* s, VFile* fp ) 
{
	if( isProviderFile( fp ) )
	{
		return providerFilePutS( s, fp );
	}

	int result = fputs( s, fp->m_FILE );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::fileGetPos( VFile* fp, fpos_t* pos )
{
	if( isProviderFile( fp ) )
	{
		return providerFileGetPos( fp, pos );
	}

	int result = fgetpos( fp->m_FILE, pos );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::fileSetPos( VFile* fp, const fpos_t* pos )
{
	if( isProviderFile( fp ) )
	{
		return providerFileSetPos( fp, pos );
	}

	int result = fsetpos( fp->m_FILE, pos );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::fileSeek( VFile* fp, size_t offset, int whence )
{
	if( isProviderFile( fp ) )
	{
		return providerFileSeek( fp, offset, whence );
	}

	int result = fseek( fp->m_FILE, offset, whence );
	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
int VFileMgr::fileSeek64( VFile* fp, uint64_t offs )
{
	if( isProviderFile( fp ) )
	{
		return providerFileSeek( fp, offs, SEEK_SET );
	}

#ifdef TARGET_OS_WINDOWS
	int result = _fseeki64( fp->m_FILE, offs, SEEK_SET );
#else
	int result = fseek( fp->m_FILE, offs, SEEK_SET );
#endif// TARGET_OS_WINDOWS

	fp->m_Error = VxGetLastError();
	LogModule( eLogMediaStream, LOG_VERBOSE, "VFileMgr::%s fp %p", __func__, fp );
	return result;
}

//============================================================================
//============================================================================
