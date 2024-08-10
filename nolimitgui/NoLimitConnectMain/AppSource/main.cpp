//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiMainLoaderThread.h"
#include "NlcCommonConfig.h"

#include <CommonSrc/QtSource/AppCommon.h>
#include <CommonSrc/QtSource/HomeWindow.h>
#include <CommonSrc/QtSource/GuiParams.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QStandardPaths>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
# include <QGLFormat>
# if defined (Q_OS_ANDROID)
#  include <QtAndroid>
# endif
#else
# if defined (Q_OS_ANDROID)
#include <QCoreApplication>
#include <QtCore/QLoggingCategory>
#include <QtCore/QJniEnvironment>
#include <QtCore/private/qandroidextras_p.h>
# endif
#endif

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>

#include "AccountMgr.h"
#include "GuiHelpers.h"

#include <NetLib/VxPeerMgr.h>

#include "MediaPlayerNlc.h"

#include <libplayer-nlc/xbmc/filesystem/Directory.h>
#include <libplayer-nlc/xbmc/filesystem/SpecialProtocol.h>
#include <libplayer-nlc/xbmc/filesystem/File.h>
#include <libplayer-nlc/xbmc/platform/Environment.h>
#include <libplayer-nlc/xbmc/utils/log.h>

using namespace XFILE;


namespace{
    void ProcessQtEvents( int ms = 100 )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}

    void qtLogMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        const char *file = context.file ? context.file : "";
        const char *function = context.function ? context.function : "";
        switch (type) {
        case QtDebugMsg:
            LogMsg( LOG_DEBUG, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtInfoMsg:
            //LogMsg( LOG_INFO, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtWarningMsg:
            LogMsg( LOG_WARN, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtCriticalMsg:
            LogMsg( LOG_SEVERE, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtFatalMsg:
            LogMsg( LOG_FATAL, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        }
    }

    //============================================================================
    bool setupRootStorageDirectory()
    {
        std::string strRootUserDataDir;

        //=== determine root path to store all application data and settings etc ===//
        QString dataPath = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );

        strRootUserDataDir = dataPath.toUtf8().constData();

#ifdef DEBUG
        // remove the D from the end so release and debug builds use the same storage directory
        if( !strRootUserDataDir.empty() && ( strRootUserDataDir.c_str()[ strRootUserDataDir.length() - 1 ] == 'D' ) )
        {
            strRootUserDataDir = strRootUserDataDir.substr( 0, strRootUserDataDir.length() - 1 );
        }
#endif // DEBUG

        VxFileUtil::makeForwardSlashPath( strRootUserDataDir );
        strRootUserDataDir += "/";
        VxSetAppDirectory( eAppData, strRootUserDataDir );

        // No need to put application in path because when call QCoreApplication::setApplicationName("AppName")
        // it made it a sub directory of DataLocation
        VxSetRootDataStorageDirectory( strRootUserDataDir.c_str() );

        return VxFileUtil::testIsWritablePath( strRootUserDataDir );
    }
}

#if defined (Q_OS_ANDROID) && QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QtAndroid>
const QVector<QString> permissions({"android.permission.READ_EXTERNAL_STORAGE",
                                    "android.permission.WRITE_EXTERNAL_STORAGE",
                                    "android.permission.INTERNET",
                                    "android.permission.ACCESS_WIFI_STATE",
                                    "android.permission.CHANGE_WIFI_STATE",
                                    "android.permission.ACCESS_NETWORK_STATE",
                                    "android.permission.CHANGE_NETWORK_STATE",
                                    "android.permission.RECORD_AUDIO",
                                    "android.permission.CAMERA",
                                    "android.permission.VIBRATE",
                                    "android.permission.READ_PHONE_STATE",
                                    "android.permission.KILL_BACKGROUND_PROCESSES"});

#endif //  defined (Q_OS_ANDROID) && QT_VERSION < QT_VERSION_CHECK(6,0,0)

//============================================================================
int runApplication( QApplication* myApp, int argc, char** argv )
{
    GuiMainLoaderThread mainLoaderThread;
    // register types first so connections made in construction have rergistered signal/slot values
    AppCommon::registerMetaData();

#if defined (Q_OS_ANDROID)
    const QString externStoragePemission(QLatin1String ("android.permission.WRITE_EXTERNAL_STORAGE"));
    auto storagePermissionResult = QtAndroidPrivate::checkPermission(externStoragePemission).result();
    if( storagePermissionResult != QtAndroidPrivate::Authorized )
    {
        if( QtAndroidPrivate::Authorized != QtAndroidPrivate::requestPermission(externStoragePemission).result() )
        {
            LogMsg(LOG_WARN, "NO external storage permission");
        }
    }
#endif // defined (Q_OS_ANDROID)

    // NOTE OrganizationName and ApplicationName become part of data storage location path
    QCoreApplication::setOrganizationName( "" ); // leave blank or will become part of data storage path
    QCoreApplication::setApplicationName( VxGetApplicationNameNoSpaces() );
    QCoreApplication::setApplicationVersion( VxGetAppVersionString() );
    QGuiApplication::setApplicationDisplayName( VxGetApplicationTitle() );
    QCoreApplication::setOrganizationDomain( VxGetCompanyDomain() );

    // TODO allow user to change where the data is stored
    if( !setupRootStorageDirectory() )
    {
        QString warnWritableTitle = QObject::tr( "No Writable Location for application data" );
        QString warnWritableBody = QObject::tr( "No location found to store application data.\n Application will exit" );

        QMessageBox warnStorage( QMessageBox::Icon::Information, warnWritableTitle, warnWritableBody, QMessageBox::Ok );
        warnStorage.exec();
        return -1;
    }

    QStringList downloadPath =  QStandardPaths::standardLocations(QStandardPaths::DownloadLocation );
    std::string download = downloadPath[0].toUtf8().constData();
    VxFileUtil::makeForwardSlashPath( download );
    VxSetAppDirectory( eAppDownload, download + "/" );

    QStringList musicPath =  QStandardPaths::standardLocations(QStandardPaths::MusicLocation );
    std::string music = musicPath[0].toUtf8().constData();
    VxFileUtil::makeForwardSlashPath( music );
    VxSetAppDirectory( eAppMusic, music + "/" );

    QStringList videoPath =  QStandardPaths::standardLocations(QStandardPaths::MoviesLocation );
    std::string video = videoPath[0].toUtf8().constData();
    VxFileUtil::makeForwardSlashPath( video );
    VxSetAppDirectory( eAppVideo, video + "/" );

    QStringList picturePath =  QStandardPaths::standardLocations(QStandardPaths::PicturesLocation );
    std::string picture = picturePath[0].toUtf8().constData();
    VxFileUtil::makeForwardSlashPath( picture );
    VxSetAppDirectory( eAppPictures, picture + "/" );

    QStringList documentPath =  QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation );
    std::string document = documentPath[0].toUtf8().constData();
    VxFileUtil::makeForwardSlashPath( document );
    VxSetAppDirectory( eAppDocuments, document + "/" );

    mainLoaderThread.start();

    // initialize display scaling etc
    // the best method I have found to scale the gui is to use the default font height as the scaling factor
    QFontMetrics fontMetrics( myApp->font() );
    GuiParams::initGuiParams(fontMetrics.height());

    LogMsg( LOG_VERBOSE, "root storage disk space path %s %s", VxGetRootDataStorageDirectory().c_str(), VxFileUtil::describeDiskSpace( VxGetRootDataStorageDirectory() ).c_str() );

    while( !mainLoaderThread.getIsLoadComplete() )
    {
        ProcessQtEvents(50);
    }

    AppCommon& appCommon = CreateAppInstance( myApp );

    LogMsg( LOG_VERBOSE, "runApplication appCommon create complete" );

    std::string fontDir = VxGetFontDirectory();
    std::string defaultFont = fontDir + "arial.ttf";
    if( false == VxFileUtil::fileExists( defaultFont.c_str() ) )
    {
        GuiHelpers::copyResourceToOnDiskFile( ":/AppRes/Resources/arial.ttf", defaultFont.c_str() );
    }

    std::string teletextFont = fontDir + "teletext.ttf";
    if( false == VxFileUtil::fileExists( teletextFont.c_str() ) )
    {
        GuiHelpers::copyResourceToOnDiskFile( ":/AppRes/Resources/teletext.ttf", teletextFont.c_str() );
    }

    if( !appCommon.loadWithoutThread() )
    {
        LogMsg( LOG_VERBOSE, "runApplication user is not of legal age " );
        return false;
    }

    // send command line parameters to Kodi
    IMediaPlayerRequests::getNlcPlayer().fromGuiInitCommandLine( argc, argv );

    LogMsg( LOG_VERBOSE, "myApp->exec" );

    int result = myApp->exec();

	return result;
}

//============================================================================
int main( int argc, char** argv )
{
    // filter out gralloc4: messages

    // THIS SHOULD WORK BUT DOES NOT
    // turn off android log messages except critical
    // QLoggingCategory::setFilterRules("*.debug=false\n"
    //                                  "*.info=false\n"
    //                                  "*.warning=false\n"
    //                                  "*.critical=true");

    // THIS ALSO DOES NOT WORK.. must be a logcat thing
    //qInstallMessageHandler(qtLogMessageOutput);

    int retVal{ 0 };

    VxSetGuiThreadId();
    LogMsg( LOG_DEBUG, "Creating QApplication" );

    QCoreApplication::addLibraryPath( "." );
    QApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
    QApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

    // for some reason QApplication must be newed or does not initialize
    QApplication* myApp = new QApplication( argc, argv );

    try
    {
        retVal = runApplication( myApp, argc, argv );
    }
    catch( ... )
    {
        // clean up here, e.g. save the session
        // and close all config files.
        LogMsg( LOG_ERROR, "ERROR Application threw and exception" );

        return EXIT_FAILURE; // exit the application
    }

    return retVal;
}
