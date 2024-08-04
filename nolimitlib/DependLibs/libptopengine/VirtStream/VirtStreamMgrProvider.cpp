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

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VFile.h>
#include <CoreLib/VxFileUtil.h>


#if defined (TARGET_OS_ANDROID)
#include <QtQml/QQmlFile>
#include <QDir>
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
VirtProviderFile* VirtStreamMgr::findProviderFile( VFile* fp )
{
#if defined (TARGET_OS_ANDROID)
    auto iter = std::find_if(m_ProviderFiles.begin(), m_ProviderFiles.end(),
                             [&](VirtProviderFile* file) { return file->m_VFile == fp; });
    if( iter != m_ProviderFiles.end() )
    {
        return *iter;
    }
#endif // defined (TARGET_OS_ANDROID)

    return nullptr;
}

//============================================================================
bool VirtStreamMgr::providerDirectoryExists( std::string dirPath )
{
    bool dirExists{ false };

#if defined (TARGET_OS_ANDROID)
    QDir qDir( dirPath.c_str() );
    dirExists = qDir.exists();
#endif // defined (TARGET_OS_ANDROID)

    return dirExists;
}

//============================================================================
uint64_t VirtStreamMgr::providerFileExists( std::string fileName )
{
    uint64_t fileLen{0};
#if defined (TARGET_OS_ANDROID)
    VirtProviderFile* providerFile = new VirtProviderFile(fileName.c_str());
    if( providerFile->open( QIODevice::ReadOnly ) )
    {
        fileLen = providerFile->size();
        providerFile->closeFile();
    }

    delete providerFile;
#endif // defined (TARGET_OS_ANDROID)

    return fileLen;
}

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
        retVal = 0;
    }

	unlockProviderMgr();
#endif // defined(TARGET_OS_ANDROID)

	return retVal;
}

//============================================================================
int VirtStreamMgr::providerFileEof( VFile* fp )
{
    bool eof{ false };
#if defined(TARGET_OS_ANDROID)
	lockProviderMgr();
    VirtProviderFile* providerFile = findProviderFile( fp );
    if( !providerFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return 0;
	}

    eof = fp->m_FileOffs == fp->m_FileLen;
	unlockProviderMgr();
#endif // defined(TARGET_OS_ANDROID)

	return eof;
}

//============================================================================
int VirtStreamMgr::providerFileError( VFile* fp )
{
	int retVal = -1;
	lockProviderMgr();
    VirtProviderFile* providerFile = findProviderFile( fp );
    if( !providerFile )
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
#if defined(TARGET_OS_ANDROID)
	lockProviderMgr();
    VirtProviderFile* providerFile = findProviderFile( fp );
    if( !providerFile )
	{
		LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
		unlockProviderMgr();
		vx_assert( false );
		return retVal;
	}

    if( !providerFile->isOpen() )
    {
        LogMsg( LOG_ERROR, "VirtStreamMgr::%s file not open", __func__ );
        unlockProviderMgr();
        vx_assert( false );
        return retVal;
    }

	int64_t wantReadLen = size * count;
    int64_t readAttemptLen = std::min( wantReadLen, fp->m_FileLen - fp->m_FileOffs );

    int64_t readLen = providerFile->read( (char*)buf, readAttemptLen );
    if( readLen > 0 )
    {
        fp->m_FileOffs += readLen;
        retVal = 0;
    }

	unlockProviderMgr();
	return retVal ? retVal : readLen;
#else
    return 0;
#endif // defined(TARGET_OS_ANDROID)
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
    if( fp->m_FileOffs == fp->m_FileLen )
    {
        return EOF;
    }

    char retChar[1];
    retChar[0] = 0;
    int readLen = providerFileRead( retChar, 1, 1, fp );

    return readLen == 1 ? retChar[0] : -1;
}

//============================================================================
char* VirtStreamMgr::providerFileGetS( char* buf, int size, VFile* fp )
{
#if defined(TARGET_OS_ANDROID)
	lockProviderMgr();
    VirtProviderFile* providerFile = findProviderFile( fp );
    if( !providerFile )
    {
        LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
        unlockProviderMgr();
        vx_assert( false );
        return nullptr;
    }
	
	std::string readStr;
	int result = -1;
    int64_t readIdx = fp->m_FileOffs;
	bool foundEnd{ false };
    while( !foundEnd && readIdx < fp->m_FileLen && readIdx < size )
	{
		char retChar[1];
        int64_t readLen = providerFile->read( (char*)retChar, 1 );
        if( readLen == 1 )
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
#endif // defined(TARGET_OS_ANDROID)

	return nullptr;
}

//============================================================================
int VirtStreamMgr::providerFileGetPos( VFile* fp, fpos_t* pos )
{
    lockProviderMgr();
    VirtProviderFile* providerFile = findProviderFile( fp );
    if( !providerFile )
    {
        LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
        unlockProviderMgr();
        vx_assert( false );
        return -1;
    }

#if defined(TARGET_OS_LINUX)
    fpos_t posConvert;
    posConvert.__pos = fp->m_FileOffs;
    *pos = posConvert;
#else
    *pos = fp->m_FileOffs;
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
    int result = -1;
#if defined(TARGET_OS_ANDROID)
	lockProviderMgr();
    VirtProviderFile* providerFile = findProviderFile( fp );
    if( !providerFile )
    {
        LogMsg( LOG_ERROR, "VirtStreamMgr::%s wrong VFile", __func__ );
        unlockProviderMgr();
        vx_assert( false );
        return -1;
    }

    int64_t origPos = fp->m_FileOffs;
	switch( whence )
	{
	case SEEK_SET:
		// Beginning of file
        if( offset >= fp->m_FileLen )
		{
			unlockProviderMgr();
			return -1;
		}

        fp->m_FileOffs = offset;
		break;

	case SEEK_CUR:
		// Current position of the file pointer
        if( fp->m_FileOffs + offset < 0 ||
            fp->m_FileOffs + offset >= fp->m_FileLen )
		{
			unlockProviderMgr();
			return -1;
		}

        fp->m_FileOffs += offset;
		break;

	case SEEK_END:
        fp->m_FileOffs = fp->m_FileLen + offset;
		break;
	}

    int64_t newPos = fp->m_FileOffs;
	if( newPos < 0 )
	{
        fp->m_FileOffs = 0;
		LogMsg( LOG_ERROR, "%s invalid pos" PRId64, __func__, newPos );
	}

    if( providerFile->seek( newPos ) )
    {
        result = 0;
    }

	unlockProviderMgr();
#endif // defined(TARGET_OS_ANDROID)

    return result;
}

//============================================================================
int VirtStreamMgr::listProviderFilesAndFolders( const char* srcDir, std::vector<VxFileInfo>& fileList, uint8_t fileFilterMask )
{
    fileList.clear();
#if defined(TARGET_OS_ANDROID)

    std::string folderName( srcDir );
    //VxFileUtil::removeTrailingDirectorySlash(folderName);
    //VxFileUtil::encodePercentEncodingOfSlash(folderName);

    if( 0 == fileFilterMask )
    {
        fileFilterMask = VXFILE_TYPE_ALLNOTEXE | VXFILE_TYPE_DIRECTORY;
    }

    VxGUID onlineId = m_Engine.getMyOnlineId();
    QDir browseDir( srcDir );

    QFileInfoList fileInfoList = browseDir.entryInfoList();
    LogMsg( LOG_VERBOSE, "VirtStreamMgr::%s %d files in dir %s", __func__, fileList.size(), folderName.c_str() );
    for( auto fileListInfo : fileInfoList )
    {
        std::string fileName = fileListInfo.filePath().toUtf8().constData();
        VxFileUtil::decodePercentEncodingOfSlash( fileName );

        VxFileInfo vxFileInfo;

        if( fileListInfo.isDir() )
        {
            LogMsg( LOG_VERBOSE, "Directory %s", fileName.c_str() );

            if( fileFilterMask & VXFILE_TYPE_DIRECTORY )
            {
                VxFileUtil::assureTrailingDirectorySlash( fileName );
                vxFileInfo.setFileName( fileName.c_str() );
                vxFileInfo.setFileType( VXFILE_TYPE_DIRECTORY );
                fileList.push_back( vxFileInfo );
            }
        }
        else if( fileListInfo.isExecutable() )
        {
            LogMsg( LOG_VERBOSE, "Executable ignored File %s", fileName.c_str() );
        }
        else if( fileListInfo.isReadable() )
        {
            int64_t fileLen = fileListInfo.size();

            if( fileLen )
            {
                vxFileInfo.setFileName( fileName.c_str() );
                vxFileInfo.setFileType( VxFileNameToFileType( fileName ) );
                vxFileInfo.setFileLength( fileLen );
                fileList.push_back( vxFileInfo );
            }
            else
            {
                LogMsg( LOG_VERBOSE, "Could Not Resolve file length of file %s", fileName.c_str() );
            }
        }
        else
        {
            LogMsg( LOG_VERBOSE, "NOT Readable File %s", fileName.c_str() );
        }
    }

	return 0;
#else
    return -1;
#endif // defined(TARGET_OS_ANDROID)
}
