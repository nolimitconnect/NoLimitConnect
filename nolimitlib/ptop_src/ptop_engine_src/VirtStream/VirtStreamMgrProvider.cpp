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

#include "VirtProviderFile.h"

#include <CoreLib/VFile.h>
#include <CoreLib/VxFileUtil.h>


#if defined (TARGET_OS_ANDROID)
#include <QtQml/QQmlFile>
#include <QUrl>
#include <QFile>

# if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#  include <QtAndroid>
# else
#  include <QtCore/private/qandroidextras_p.h>
# endif
#endif //defined (TARGET_OS_ANDROID)

#if defined (Q_OS_ANDROID)
namespace
{
/*
    public static FileDetail getFileDetailFromUri(final Context context, final Uri uri) {
        FileDetail fileDetail = null;
        if (uri != null) {
            fileDetail = new FileDetail();
            // File Scheme.
            if (ContentResolver.SCHEME_FILE.equals(uri.getScheme())) {
                File file = new File(uri.getPath());
                fileDetail.fileName = file.getName();
                fileDetail.fileSize = file.length();
            }
            // Content Scheme.
            else if (ContentResolver.SCHEME_CONTENT.equals(uri.getScheme())) {
                Cursor returnCursor =
                        context.getContentResolver().query(uri, null, null, null, null);
                if (returnCursor != null && returnCursor.moveToFirst()) {
                    int nameIndex = returnCursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                    int sizeIndex = returnCursor.getColumnIndex(OpenableColumns.SIZE);
                    fileDetail.fileName = returnCursor.getString(nameIndex);
                    fileDetail.fileSize = returnCursor.getLong(sizeIndex);
                    returnCursor.close();
                }
            }
        }
        return fileDetail;
    }
*/

} // namespace

#endif // defined (Q_OS_ANDROID)


//============================================================================
VFile* VirtStreamMgr::providerFileOpen( std::string fileNameIn, std::string fileMode )
{
#if defined( TARGET_OS_ANDROID )
    QString contentPath( fileNameIn.c_str() );
    QUrl contentUrl(contentPath);

    QString fileName = QQmlFile::urlToLocalFileOrQrc(contentUrl);

    VirtProviderFile* providerFile = new VirtProviderFile(fileName);
    if( providerFile->open( QIODevice::ReadOnly ) )
    {
		VFile* vFile = new VFile();
		memset( vFile, 0, sizeof( VFile ) );
        vFile->m_ProviderFileType = 1;
        vFile->m_FileLen = providerFile->size();

        providerFile->m_FileMode = fileMode;
        providerFile->m_FileName = fileNameIn;
        providerFile->m_VFile = vFile;

        lockProviderMgr();
        m_ProviderFiles.emplace_back(providerFile);
        unlockProviderMgr();

        return vFile;
    }

    delete providerFile;

#endif // defined( TARGET_OS_ANDROID )

	return nullptr;
}

//============================================================================
int VirtStreamMgr::providerFileClose( VFile* fp )
{
	int retVal = -1;
#if defined(TARGET_OS_ANDROID)
	lockProviderMgr();
    auto iter = std::find_if(m_ProviderFiles.begin(), m_ProviderFiles.end(), 
                              [&](VirtProviderFile* file) { return file->m_VFile == fp; });
    if( iter != m_ProviderFiles.end() )
    {
        VirtProviderFile* providerFile = *iter;
        providerFile->closeFile();
        providerFile->deleteLater();
		m_ProviderFiles.erase( iter );
    }

	unlockProviderMgr();
#endif // defined(TARGET_OS_ANDROID)

	return retVal;
}

//============================================================================
int VirtStreamMgr::providerFileEof( VFile* fp )
{
	lockProviderMgr();
	
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return 0;
	}

	bool eof = m_LiveStream.m_VFile->m_FileOffs == m_LiveStream.m_VFile->m_FileLen;
	unlockProviderMgr();
	return eof;
}

//============================================================================
int VirtStreamMgr::providerFileError( VFile* fp )
{
	int retVal = -1;
	lockProviderMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return retVal;
	}
	
	m_LiveStream.isConnected();
	retVal = m_LiveStream.getError();
	unlockProviderMgr();
	return retVal;
}

//============================================================================
int VirtStreamMgr::providerFileFlush( VFile* fp )
{
	return 0;
}

//============================================================================
size_t VirtStreamMgr::providerFileRead( void* buf, size_t size, size_t count, VFile* fp )
{
	int retVal = -1;

	lockProviderMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return retVal;
	}

	m_LiveStream.isConnected();
	if( m_LiveStream.getError() )
	{
		retVal = m_LiveStream.getError();
		unlockProviderMgr();
		return retVal;
	}

	int64_t wantReadLen = size * count;
	int64_t readAttemptLen = std::min( wantReadLen, m_LiveStream.m_VFile->m_FileLen - m_LiveStream.m_VFile->m_FileOffs );
	if( !waitForStream( m_LiveStream.m_VFile->m_FileOffs, readAttemptLen ) )
	{
		unlockProviderMgr();
		LogModule( eLogMediaStream, LOG_ERROR, "VirtStreamMgr::%s timeout waiting for stream file %s at offs%" PRId64 " len%s" PRId64,
				   m_LiveStream.m_StreamAssetInfo.getAssetName().c_str(), m_LiveStream.m_VFile->m_FileOffs, readAttemptLen );
		return retVal;
	}
		
	int64_t readLen = 0;
	if( m_LiveStream.m_FileTail.hasData( m_LiveStream.m_VFile->m_FileOffs, readAttemptLen ) )
	{
		readLen = m_LiveStream.m_FileTail.readData( m_LiveStream.m_VFile->m_FileOffs, (char*)buf, readAttemptLen );
	}
	else
	{
		readLen = m_LiveStream.m_StreamCache.readData( m_LiveStream.m_VFile->m_FileOffs, (char*)buf, readAttemptLen );
	}


	if( readLen == readAttemptLen )
	{
#if VERIFY_CACHE_DATA
		verifyCacheData( m_LiveStream.m_VFile->m_FileOffs, (uint8_t *)buf, readAttemptLen );
#endif // VERIFY_CACHE_DATA

		m_LiveStream.m_VFile->m_FileOffs += readLen;
		retVal = 0;
	}
	else
	{

	}

	unlockProviderMgr();
	return retVal ? retVal : readLen;
}

//============================================================================
size_t VirtStreamMgr::providerFileWrite(const void* buf, size_t size, size_t count, VFile* fp)
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::providerFileGetC( VFile* fp )
{
	int retVal = -1;
	lockProviderMgr();
	if( !waitForStream( m_LiveStream.m_VFile->m_FileOffs, 1 ) )
	{
		unlockProviderMgr();
		LogModule( eLogMediaStream, LOG_ERROR, "VirtStreamMgr::%s timeout waiting for stream file %s at offs%" PRId64 " len%s" PRId64,
				   m_LiveStream.m_StreamAssetInfo.getAssetName().c_str(), m_LiveStream.m_VFile->m_FileOffs, 1 );
		return retVal;
	}

	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return retVal;
	}

	m_LiveStream.isConnected();
	if( m_LiveStream.getError() )
	{
		retVal = m_LiveStream.getError();
		unlockProviderMgr();
		return retVal;
	}

	int64_t readAttemptLen = 1;
	char retChar[1];
	int64_t readLen = m_LiveStream.m_StreamCache.readData( m_LiveStream.m_VFile->m_FileOffs, (char*)retChar, readAttemptLen );
	if( readLen == readAttemptLen )
	{
		m_LiveStream.m_VFile->m_FileOffs += readLen;
		unlockProviderMgr();
		return retChar[0];
	}

	return -1;
}

//============================================================================
char* VirtStreamMgr::providerFileGetS( char* buf, int size, VFile* fp )
{
	int retVal = -1;
	lockProviderMgr();
	if( !waitForStream( m_LiveStream.m_VFile->m_FileOffs, 1 ) )
	{
		unlockProviderMgr();
		LogModule( eLogMediaStream, LOG_ERROR, "VirtStreamMgr::%s timeout waiting for stream file %s at offs%" PRId64 " len%s" PRId64,
				   m_LiveStream.m_StreamAssetInfo.getAssetName().c_str(), m_LiveStream.m_VFile->m_FileOffs, 1 );
		return nullptr;
	}
	
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return nullptr;
	}

	m_LiveStream.isConnected();
	if( m_LiveStream.getError() )
	{
		unlockProviderMgr();
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
	
	unlockProviderMgr();
	if( result == 0 )
	{
		memcpy( buf, readStr.c_str(), readStr.length() );
		return buf;
	}

	return nullptr;
}

//============================================================================
int VirtStreamMgr::providerFileGetPos( VFile* fp, fpos_t* pos )
{
    lockProviderMgr();
    if( fp != m_LiveStream.m_VFile )
    {
        LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
        unlockProviderMgr();
        vx_assert( false );
        return -1;
    }

#if defined(TARGET_OS_LINUX)
    fpos_t posConvert;
    posConvert.__pos = m_LiveStream.m_VFile->m_FileOffs;
    *pos = posConvert;
#else
    *pos = m_LiveStream.m_VFile->m_FileOffs;
#endif
    unlockProviderMgr();
    return 0;
}

//============================================================================
int VirtStreamMgr::providerFilePutC(int ch, VFile* fp)
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::providerFilePutS(const char* s, VFile* fp)
{
	// not implemented
	return -1;
}
//============================================================================
int VirtStreamMgr::providerFileSetPos( VFile* fp, const fpos_t* pos )
{
	// not implemented
	return -1;
}

//============================================================================
int VirtStreamMgr::providerFileSeek( VFile* fp, size_t offset, int whence )
{
	lockProviderMgr();
	if( fp != m_LiveStream.m_VFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return -1;
	}

	int64_t origPos = m_LiveStream.m_VFile->m_FileOffs;
	switch( whence )
	{
	case SEEK_SET:
		// Beginning of file
		if( offset >= m_LiveStream.m_VFile->m_FileLen )
		{
			unlockProviderMgr();
			return -1;
		}

		m_LiveStream.m_VFile->m_FileOffs = offset;
		break;

	case SEEK_CUR:
		// Current position of the file pointer
		if( m_LiveStream.m_VFile->m_FileOffs + offset < 0 ||
			m_LiveStream.m_VFile->m_FileOffs + offset >= m_LiveStream.m_VFile->m_FileLen )
		{
			unlockProviderMgr();
			return -1;
		}

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

	unlockProviderMgr();

	if( newPos >= 0 && newPos != origPos && 
		!m_LiveStream.m_StreamCache.hasData( newPos, 1 ) &&
		!m_LiveStream.m_FileTail.hasData( newPos, 1 ) )
	{
		return sendStreamSeek( newPos ) ? 0 : -1;
	}

	return 0;
}

//============================================================================
int VirtStreamMgr::listProviderFilesAndFolders( const char* srcDir, std::vector<VxFileInfo>& fileList, uint8_t fileFilterMask )
{


	return 0;
}
