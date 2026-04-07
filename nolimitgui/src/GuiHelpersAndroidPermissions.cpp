//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

// manage the ever changing android permissions
// NOTE: even if you have broad permissions like  "android.permission.MANAGE_EXTERNAL_STORAGE"
// does not mean you have narrow permissions like "android.permission.READ_MEDIA_VIDEO"

#include "GuiHelpers.h"
#include "GuiParams.h"

#include <CoreLib/AssetDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFileSystemModel>
#include <QStandardPaths>
#include <QUrl>

#if defined (Q_OS_ANDROID)
# if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#  include <QtAndroid>
# else
#  include <QtCore/private/qandroidextras_p.h>
#  include <QJniObject>
# endif
#endif //defined (Q_OS_ANDROID)

//============================================================================
int GuiHelpers::getAndroidSDKVersion( void )
{
#if defined (Q_OS_ANDROID)
    return QJniObject::getStaticField<jint>(
        "android/os/Build$VERSION", "SDK_INT");
#endif // defined (Q_OS_ANDROID)

    return 0;
}

//============================================================================
bool GuiHelpers::havePermission( QString permissionName ) // returns false if user denies permission to use android hardware
{
#if defined (Q_OS_ANDROID)
    QtAndroidPrivate::PermissionResult result = QtAndroidPrivate::checkPermission( permissionName ).result();
    if( QtAndroidPrivate::Authorized != result )
    {
        if( QtAndroidPrivate::Denied == result )
        {
            return false;
        }
    }
#endif // defined (Q_OS_ANDROID)

    return true;
}

//============================================================================
bool GuiHelpers::requestFilePermission( enum EMediaFileType permissionType, bool showUserMsgIfDenied )
{
#ifdef Q_OS_ANDROID
    if( havePermission(QLatin1String("android.permission.MANAGE_EXTERNAL_STORAGE")) )
    {
        return true;
    }

    if( havePermission(QLatin1String("android.permission.WRITE_EXTERNAL_STORAGE")) )
    {
        return true;
    }

    if( havePermission(QLatin1String("android.permission.READ_EXTERNAL_STORAGE")) )
    {
        return true;
    }

    bool result{true};

    QStringList permissionList;
    if( getAndroidSDKVersion() >= 33 )
    {
        switch(permissionType)
        {
        case eMediaFileAny:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_VIDEO"));
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_AUDIO"));
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_IMAGES"));
            break;

        case eMediaFileImage:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_IMAGES"));
            break;

        case eMediaFileAudio:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_AUDIO"));
            break;

        case eMediaFileVideo:
            permissionList.emplace_back(QLatin1String("android.permission.READ_MEDIA_VIDEO"));
            break;

        default:
            break;
        }
    }
    else
    {
        permissionList.emplace_back(QLatin1String("android.permission.READ_EXTERNAL_STORAGE"));
    }

    for( auto permission : permissionList )
    {
        if( !GuiParams::requestPermission( permission ) )
        {
            LogMsg( LOG_ERROR, "%s permission denied %s", __func__, permission.toUtf8().constData() );
            result = false;
            break;
        }
    }

    if(!result && showUserMsgIfDenied)
    {
        showFilePermissionError();
    }

    return result;
#else
    return true;
#endif // Q_OS_ANDROID
}

//============================================================================
static QString getRealPathFromUri(const QUrl &url)
{
    QString path = "";

    QFileInfo info = QFileInfo(url.toString());
    if(info.isFile())
    {
        QString abs = QFileInfo(url.toString()).absoluteFilePath();
        if(!abs.isEmpty() && abs != url.toString() && QFileInfo(abs).isFile())
        {
            return abs;
        }
    }
    else if(info.isDir())
    {
        QString abs = QFileInfo(url.toString()).absolutePath();
        if(!abs.isEmpty() && abs != url.toString() && QFileInfo(abs).isDir())
        {
            return abs;
        }
    }
    QString localfile = url.toLocalFile();
    if((QFileInfo(localfile).isFile() || QFileInfo(localfile).isDir()) && localfile != url.toString())
    {
        return localfile;
    }
#ifdef Q_OS_ANDROID
    QJniObject jUrl = QJniObject::fromString(url.toString());
    QJniObject jContext = QtAndroidPrivate::context();
    QJniObject jContentResolver = jContext.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    QJniObject jUri = QJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jUrl.object<jstring>());
    QJniObject jCursor = jContentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", jUri.object<jobject>(), nullptr, nullptr, nullptr, nullptr);
    QJniObject jScheme = jUri.callObjectMethod("getScheme", "()Ljava/lang/String;");
    QJniObject authority;
    if(jScheme.isValid())
    {
        authority = jUri.callObjectMethod("getAuthority", "()Ljava/lang/String;");
    }
    if(authority.isValid() && authority.toString() == "com.android.externalstorage.documents")
    {
        QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
        path = jPath.toString();
    }
    else if(jCursor.isValid() && jCursor.callMethod<jboolean>("moveToFirst"))
    {
        QJniObject jColumnIndex = QJniObject::fromString("_data");
        jint columnIndex = jCursor.callMethod<jint>("getColumnIndexOrThrow", "(Ljava/lang/String;)I", jColumnIndex.object<jstring>());
        QJniObject jRealPath = jCursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
        path = jRealPath.toString();
        if(authority.isValid() && authority.toString().startsWith("com.android.providers") && !url.toString().startsWith("content://media/external/"))
        {
            QStringList list = path.split(":");
            if(list.count() == 2)
            {
                QString type = list.at(0);
                QString id = list.at(1);
                if(type == "image")
                    type = type + "s";
                if(type == "document" || type == "documents")
                    type = "file";
                if(type == "msf")
                    type = "downloads";
                if(QList<QString>({"images","video","audio"}).contains(type))
                    type = type + "/media";
                path = "content://media/external/"+type;
                path = path + "/" + id;
                return getRealPathFromUri(path);
            }
        }
    }
    else
    {
        QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
        path = jPath.toString();
    }

    if(path.startsWith("primary:"))
    {
        path = path.remove(0,QString("primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/document/primary:"))
    {
        path = path.remove(0,QString("/document/primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/tree/primary:"))
    {
        path = path.remove(0,QString("/tree/primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/storage/emulated/0/"))
    {
        path = path.remove(0,QString("/storage/emulated/0/").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/tree//"))
    {
        path = path.remove(0,QString("/tree//").length());
        path = "/" + path;
    }
    if(!QFileInfo(path).isFile() && !QFileInfo(path).isDir() && !path.startsWith("/data"))
        return url.toString();
    return path;
#else
    return url.toString();
#endif

}

//============================================================================
void GuiHelpers::contentUrlToFileSystemPath( QString& contentUrl )
{
    QString realPath = getRealPathFromUri(contentUrl);
    QFileInfo fileInfo(contentUrl);

    LogMsg(LOG_DEBUG, "contentUrlToFileSystemPath %s result %s fileInfo %s ",
           contentUrl.toUtf8().constData(),
           realPath.toUtf8().constData(), fileInfo.fileName().toUtf8().constData() );


    /*
    QUrl url_path(contentUrl);

    qDebug() << "[Original String]:" << contentUrl;
    qDebug() << "--------------------------------------------------------------------";
    qDebug() << "[getRealPathFromUri]:" << getRealPathFromUri(url_path);


    qDebug() << "(QUrl::toEncoded)          :" << url_path.toEncoded(QUrl::FullyEncoded);
    qDebug() << "(QUrl::url)                :" << url_path.url();
    qDebug() << "(QUrl::toString)           :" << url_path.toString();
    qDebug() << "(QUrl::toDisplayString)    :" << url_path.toDisplayString(QUrl::FullyEncoded);
    qDebug() << "(QUrl::fromPercentEncoding):" << url_path.fromPercentEncoding(contentUrl.toUtf8());
    qDebug() << "(QQmlFile::urlToLocalFileOrQrc):" << QQmlFile::urlToLocalFileOrQrc(contentUrl).toStdString();



    qDebug() << "(QFileInfo::canonicalPath)      :" << fileInfo.canonicalPath();
    qDebug() << "(QFileInfo::absolutePath)       :" << fileInfo.absolutePath();
    qDebug() << "(QFileInfo::path)               :" << fileInfo.path();
    qDebug() << "(QFileInfo::baseName)           :" << fileInfo.baseName();
    qDebug() << "(QFileInfo::filePath)           :" << fileInfo.filePath();
    qDebug() << "(QFileInfo::absoluteFilePath)   :" << fileInfo.absoluteFilePath();
    qDebug() << "(QFileInfo::canonicalFilePath)  :" << fileInfo.canonicalFilePath();

    QDir dir(contentUrl);
    qDebug() << "(QDir::canonicalPath)      :" << dir.canonicalPath();
    qDebug() << "(QDir::absolutePath)       :" << dir.absolutePath();
    qDebug() << "(QDir::path)               :" << dir.path();

    qDebug() << "(QDir::clean)           :" << dir.cleanPath(contentUrl);
    qDebug() << "(QDir::absoluteFilePath)   :" << dir.absoluteFilePath(contentUrl);
*/
    static QVector<QStringList> locations;
    if(locations.isEmpty())
    {
        locations.push_back( QStandardPaths::standardLocations(QStandardPaths::AppDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::DesktopLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::MusicLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::MoviesLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::PicturesLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::TempLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::HomeLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::AppLocalDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::CacheLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::GenericDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::RuntimeLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::ConfigLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::DownloadLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::GenericCacheLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::GenericConfigLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::AppConfigLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::PublicShareLocation ) );
        locations.push_back( QStandardPaths::standardLocations( QStandardPaths::TemplatesLocation ) );
    }

    int row = 0;
    for( auto stringList : locations )
    {
        row++;
        int col = 0;
        for( auto string : stringList )
        {
            col++;
            QDir dir(string);


            LogMsg(LOG_DEBUG, "%d-%d %s \n con %s \n abs %s \n path %s",
                   row, col, string.toUtf8().constData(),
                   dir.canonicalPath().toUtf8().constData(),
                   dir.absolutePath().toUtf8().constData(),
                   dir.path().toUtf8().constData()
                   );
        }
    }


    LogMsg(LOG_DEBUG, "END");
}

std::string GuiHelpers::getRealFileName( QString selectedFileIn )
{
    return selectedFileIn.toUtf8().constData();
#if defined(TARGET_OS_ANDROID)
    std::string selectedFile = selectedFileIn.toUtf8().constData();
    std::string justFile;
    std::string justPath;
    /*
    VxFileUtil::seperatePathAndFile( selectedFile, justPath, justFile );
    if( justPath.empty() || justFile.empty() )
    {
        return selectedFile;
    }
*/

    QDir browseDir( "content://com.android.externalstorage.documents/document/primary:NoLimitConnectData/MediaTestFiles" );

    QFileInfoList fileInfoList = browseDir.entryInfoList();
    LogMsg( LOG_VERBOSE, "%zu files in dir %s", fileInfoList.size(), justPath.c_str() );
    for( auto fileListInfo : fileInfoList )
    {
        std::string fileName = fileListInfo.filePath().toUtf8().constData();

        if( fileName.empty() )
        {
            continue;
        }

        if( fileListInfo.isDir() )
        {
            LogMsg( LOG_VERBOSE, "Directory %s", fileName.c_str() );


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
                uint8_t fileType = VxFileNameToFileType( fileName );

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

    return selectedFile;
#else
    return selectedFileIn.toUtf8().constData();
#endif // defined(TARGET_OS_ANDROID)
}
