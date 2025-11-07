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

#include "GuiHelpers.h"
#include "GuiThreadMainLoader.h"
#include "GuiThreadSettingsLoader.h"
#include "NlcCommonConfig.h"

#include <src/AppCommon.h>
#include <src/AppSettings.h>
#include <src/HomeWindow.h>
#include <src/GuiParams.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSettings>
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

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>

#include "AccountMgr.h"
#include "GuiHelpers.h"

#include <NetLib/VxPeerMgr.h>

namespace {

    //============================================================================
    void setupRootStorageDirectory()
    {
        std::string strRootAppDataDir;

        //=== determine root path to store all application data and settings etc ===//
        QString dataPath = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );

        strRootAppDataDir = dataPath.toUtf8().constData();

#ifdef DEBUG
        // remove the D from the end so release and debug builds use the same storage directory
        if( !strRootAppDataDir.empty() && ( strRootAppDataDir.c_str()[ strRootAppDataDir.length() - 1 ] == 'D' ) )
        {
            strRootAppDataDir = strRootAppDataDir.substr( 0, strRootAppDataDir.length() - 1 );
        }
#endif // DEBUG

        VxFileUtil::makeForwardSlashPath( strRootAppDataDir );
        strRootAppDataDir += "/";
        VxSetAppDirectory( eAppData, strRootAppDataDir );

        // No need to put application in path because when call QCoreApplication::setApplicationName("AppName")
        // it made it a sub directory of DataLocation
        VxSetRootDataStorageDirectory( strRootAppDataDir.c_str() );
        VxSetRootUserDataDirectory( strRootAppDataDir.c_str() );

    //=== determine root path for data xfer like incomplete/downloads/uploads etc ===//
#if defined(TARGET_OS_WINDOWS) || defined(TARGET_OS_ANDROID)
        QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#else
        // linux hides document under .local so use home directory if possible
        QString docsPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if( docsPath.isEmpty() )
        {
            docsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        }
#endif // TARGET_OS_WINDOWS

        std::string rootXferDir = docsPath.toUtf8().constData();

        VxFileUtil::makeForwardSlashPath( rootXferDir );
        VxFileUtil::assurePathEndWithSlash( rootXferDir );

        rootXferDir += VxGetApplicationNameNoSpaces();
        VxFileUtil::assurePathEndWithSlash( rootXferDir );
        VxFileUtil::makeDirectory( rootXferDir.c_str() );

        // sets root of data transfer directories

        VxSetRootXferDirectory( rootXferDir.c_str() );

        if( !VxFileUtil::directoryExists( rootXferDir.c_str() ) )
        {
            LogMsg( LOG_ERROR, "%s Could not create xfer dir %s", __func__, rootXferDir.c_str());
        }
    }
}

//============================================================================
int runApplication( QApplication* myApp, int argc, char** argv )
{
    // NOTE OrganizationName and ApplicationName become part of data storage location path
    QCoreApplication::setOrganizationName( "" ); // leave blank or will become part of data storage path
    QCoreApplication::setApplicationName( VxGetApplicationNameNoSpaces() );
    QCoreApplication::setApplicationVersion( VxGetAppVersionString() );
    QGuiApplication::setApplicationDisplayName( VxGetApplicationTitle() );
    QCoreApplication::setOrganizationDomain( VxGetCompanyDomain() );

    QSettings settings (VxGetCompanyDomain(), VxGetApplicationNameNoSpaces() );

    // TODO fix and apply theme to age confirm dialog.. Android Qt 6.8.3 does not send button click in ActivityMsgBoxYesNo when done before startup
    if (!settings.contains("isAdult")) {
        QString warnAdultTitle = QObject::tr( "You must be an adult to use No Limit Connect application" );
        QString warnAdultBody = QObject::tr( "Although No Limit Connect does not host any offensive media, users of No Limit Connect may host offensive material or act in an offensive manner.\n"
                                            "No Limit Connect does not monitor or log any user actions or content.\n\n"
                                            "Are you an adult and at least 18 years old?" );

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, warnAdultTitle,
                                      warnAdultBody,
                                      QMessageBox::Yes | QMessageBox::No);

        bool isAdult = (reply == QMessageBox::Yes);

        if (!isAdult) {
            QString deniedTitle = QObject::tr("Access Denied");
            QString deniedBody = QObject::tr("You must be 18 or older to use this application.");
            QMessageBox::information(nullptr, deniedTitle, deniedBody);
            return 0; // Exit application
        }

        settings.setValue("isAdult", isAdult);
    }

    int timeStart = GetApplicationAliveMs();

    static AppSettings appSettings;
    GuiThreadSettingsLoader threadSettingsLoader(appSettings);

    // register types first so connections made in construction have registered signal/slot values
    AppCommon::registerMetaData();
    int timeRegisterMetadata = GetApplicationAliveMs();



    // must be ran after application name is set or paths with app name may be lower case instead of upper case
    setupRootStorageDirectory();

    threadSettingsLoader.start();

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

    // initialize display scaling etc
    // the best method I have found to scale the gui is to use the default font height as the scaling factor
    QFontMetrics fontMetrics( myApp->font() );
    GuiParams::initGuiParams(fontMetrics.height());

    GuiThreadMainLoader mainLoaderThread;
    mainLoaderThread.start();

    int timeInitFonts = GetApplicationAliveMs();

    LogMsg( LOG_VERBOSE, "root storage disk space path %s %s", VxGetRootDataStorageDirectory().c_str(), VxFileUtil::describeDiskSpace( VxGetRootDataStorageDirectory() ).c_str() );

    bool haveWaitTime{ false };
    if( !mainLoaderThread.getIsLoadComplete() )
    {
        haveWaitTime = true;
        LogMsg( LOG_VERBOSE, "%s waiting for main loader thread", __func__ );
        while( !mainLoaderThread.getIsLoadComplete() )
        {
            GuiHelpers::processQtEvents();
        }

        int waitMainLoaderThread = GetApplicationAliveMs();
        LogMsg( LOG_VERBOSE, "%s waited for main loader thread %d ms", __func__, waitMainLoaderThread - timeInitFonts );
    }

    if( !threadSettingsLoader.getIsSettingsLoaded() )
    {
        haveWaitTime = true;
        int waitStart = GetApplicationAliveMs();
        while( !threadSettingsLoader.getIsSettingsLoaded() )
        {
            GuiHelpers::processQtEvents();
        }

        // now that settings are loaded we can start using LogModule
        if( LogEnabled( eLogStartup ) )
        {
            int waitEnd = GetApplicationAliveMs();
            LogMsg( LOG_VERBOSE, "%s waited for settings loader thread %d ms", __func__, waitEnd - waitStart );
        }
    }

    int timePreStartApp = GetApplicationAliveMs();
    if( LogEnabled( eLogStartup ) )
    {     
        if( haveWaitTime )
        {
            LogMsg( LOG_VERBOSE, "%s time waiting for loaders %d ms", __func__, timePreStartApp - timeInitFonts );
        }

        LogMsg( LOG_VERBOSE, "%s time register %d app fonts %d", __func__, timeRegisterMetadata - timeStart,
                timeInitFonts - timeRegisterMetadata );
    }

    AppCommon& appCommon = CreateAppInstance( myApp, appSettings );

    int createAppCommon = GetApplicationAliveMs();

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

    int copyFonts = GetApplicationAliveMs();

    if( !appCommon.loadWithThread() )
    {
        LogMsg( LOG_ERROR, "%s user is not of legal age ", __func__ );
        return false;
    }

    if( LogEnabled( eLogStartup ) )
    {
        int timeNow = GetApplicationAliveMs();
        LogMsg( LOG_VERBOSE, "%s setup %d md create AppCommon %d font copy %d load %d total %d ms", __func__,
           timePreStartApp - timeStart, createAppCommon - timePreStartApp, copyFonts - createAppCommon, timeNow - copyFonts, timeNow - timeStart );
    }

    int result = myApp->exec();

	return result;
}

//============================================================================
int main( int argc, char** argv )
{
#if defined(TARGET_OS_WINDOWS)
    // unfortunatly this does not fix the issue but since it only happens in debug builds the crash on shutdown can be ignored
    // QTBUG-118330 
    qputenv( "QT_FFMPEG_HWACCEL", "none" ); // to stop crash by Qt6Multimediad.dll not releasing d3d11 textures
#endif // defined(TARGET_OS_WINDOWS)

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
