//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_ANDROID)

#include "VirtStorageProvider.h"

#include <P2PEngine/P2PEngine.h>
#include <Plugins/FileInfo.h>

#include <QDir>
#include <QUrl>

#if defined (Q_OS_ANDROID)
#include <QtQml/QQmlFile>

# if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#  include <QtAndroid>
# else
#  include <QtCore/private/qandroidextras_p.h>
# endif
#endif //defined (Q_OS_ANDROID)


//============================================================================
VirtStorageProvider& GetVirtStorageProvider( void )
{
    static VirtStorageProvider g_VirtStorageProvider;
    return g_VirtStorageProvider;
}

//============================================================================
void VirtStorageProvider::fromGuiBrowseFiles( std::string& folderName, uint8_t fileFilterMask )
{
#if !defined(TARGET_OS_ANDROID)
    GetPtoPEngine().getFromGuiInterface().fromGuiBrowseFiles( folderName, fileFilterMask );
    return;
#endif // !defined(TARGET_OS_ANDROID)

    std::string testFile;

	std::vector<FileInfo> fileList;
	if( 0 == fileFilterMask )
	{
		fileFilterMask = VXFILE_TYPE_ALLNOTEXE | VXFILE_TYPE_DIRECTORY;
	}

    VxGUID onlineId = GetPtoPEngine().getMyOnlineId();
    QDir browseDir( folderName.c_str() );

    QFileInfoList fileInfoList = browseDir.entryInfoList();
    LogMsg( LOG_VERBOSE, "%d files in dir %s", fileList.size(), folderName.c_str() );
    for( auto fileListInfo : fileInfoList )
    {
        std::string fileName = fileListInfo.filePath().toUtf8().constData();

		if( fileListInfo.isDir() )
		{
			LogMsg( LOG_VERBOSE, "Directory %s", fileName.c_str() );

			if( fileFilterMask & VXFILE_TYPE_DIRECTORY )
			{
				VxFileUtil::assureTrailingDirectorySlash( fileName );
				FileInfo dirInfo( onlineId, fileName, 0, VXFILE_TYPE_DIRECTORY );
                GetPtoPEngine().getToGui().toGuiFileList( dirInfo );
			}
		}
		else if( fileListInfo.isExecutable() )
		{
			LogMsg( LOG_VERBOSE, "Executable ignored File %s", fileName.c_str() );
		}
		else if( fileListInfo.isReadable() )
		{
            int64_t fileLen{ 0 };
            std::string resolvedName;
#if defined(TARGET_OS_ANDROID)
            QString contentPath( fileName.c_str() );
            QUrl contentUrl(contentPath);
            QString fileNameResolved = QQmlFile::urlToLocalFileOrQrc(contentUrl);
            resolvedName = fileNameResolved.toStdString();
#endif // defined(TARGET_OS_ANDROID)

            //if( StorageFile::resolveFileNameAndLength( fileName, resolvedName, fileLen ) )
            if( !resolvedName.empty() )
            {
                LogMsg( LOG_VERBOSE, "Readable File len1 %" PRId64 " resolved to %s from %s",
                       fileLen, resolvedName.c_str(), fileName.c_str());

			    uint8_t fileType = VxFileNameToFileType( fileName );
                if( fileLen && fileType & fileFilterMask )
			    {
                    LogMsg( LOG_VERBOSE, "Readable File %s len %" PRId64 " type 0x%x ", resolvedName.c_str(), fileLen, fileType );
                    std::size_t found = resolvedName.find( "Test.x264-ION10.mp4" );
                    if( found != std::string::npos )
                    {
                        testFile = resolvedName;
                    }

				    FileInfo fileInfo( onlineId, resolvedName, fileLen, fileType );
				    fileInfo.setIsInLibrary( GetPtoPEngine().fromGuiGetIsFileInLibrary( resolvedName ) );
				    fileInfo.setIsSharedFile( GetPtoPEngine().fromGuiGetIsFileShared( fileInfo ) );

				    // GetAppInstance().getFileXferMgr().toGuiFileList( fileInfo );
			    }
            }
            else
            {
                LogMsg( LOG_VERBOSE, "Could Not Resolve length %" PRId64 " File %s", fileLen, fileName.c_str() );
            }
		}
		else
		{
			LogMsg( LOG_VERBOSE, "NOT Readable File %s", fileName.c_str() );
		}
    }

    GetPtoPEngine().getToGui().toGuiFileListCompleted();
}

//============================================================================
void VirtStorageProvider::loadUrl( const QUrl &url )
{
/*
    m_sequence.clear();
    qDebug() << "hello" << url << QQmlFile::urlToLocalFileOrQrc( url );
    QFile file(QQmlFile::urlToLocalFileOrQrc(url));
    if (!file.open(QFile::ReadOnly))
    {
        emit error(tr("Could not open file. '%1'").arg(file.errorString()));
        return;
    }
*/
}

//============================================================================
bool VirtStorageProvider::requestAndroidStoragePermissions( void )
{
    return checkUserPermission({ "android.permission.READ_EXTERNAL_STORAGE"});
}

//============================================================================
bool VirtStorageProvider::checkUserPermission( QString permissionName ) // returns false if user denies permission to use android hardware
{

#if defined (Q_OS_ANDROID)

    if( QtAndroidPrivate::Authorized != QtAndroidPrivate::checkPermission(permissionName).result() )
    {
        if( QtAndroidPrivate::Authorized != QtAndroidPrivate::requestPermission(permissionName).result() )
        {
            return false;
        }
    }


    return true;
#else
    return true;
#endif // defined (Q_OS_ANDROID)
}

#endif // defined(TARGET_OS_ANDROID)