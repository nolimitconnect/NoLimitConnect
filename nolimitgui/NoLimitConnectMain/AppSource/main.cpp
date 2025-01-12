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

#include "GuiThreadMainLoader.h"
#include "GuiThreadSettingsLoader.h"
#include "NlcCommonConfig.h"

#include <CommonSrc/QtSource/AppCommon.h>
#include <CommonSrc/QtSource/AppSettings.h>
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

namespace {
    void ProcessQtEvents( int ms = 100 )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
    }
}

//============================================================================
int runApplication( QApplication* myApp, int argc, char** argv )
{
    int timeStart = GetApplicationAliveMs();

    static AppSettings appSettings;
    GuiThreadSettingsLoader threadSettingsLoader(appSettings);
    threadSettingsLoader.start();

    // register types first so connections made in construction have registered signal/slot values
    AppCommon::registerMetaData();
    int timeRegisterMetadata = GetApplicationAliveMs();

    // NOTE OrganizationName and ApplicationName become part of data storage location path
    QCoreApplication::setOrganizationName( "" ); // leave blank or will become part of data storage path
    QCoreApplication::setApplicationName( VxGetApplicationNameNoSpaces() );
    QCoreApplication::setApplicationVersion( VxGetAppVersionString() );
    QGuiApplication::setApplicationDisplayName( VxGetApplicationTitle() );
    QCoreApplication::setOrganizationDomain( VxGetCompanyDomain() );

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

    int timeSetAppDirs = GetApplicationAliveMs();
    if( !threadSettingsLoader.getIsRootStorageSet() )
    {
        LogMsg( LOG_VERBOSE, "%s waiting for root storage setup", __func__ );
        while( !threadSettingsLoader.getIsRootStorageSet() )
        {
            ProcessQtEvents(50);
        }

        int timeWaitRoot = GetApplicationAliveMs();
        LogMsg( LOG_VERBOSE, "%s waited %d ms for root storage setup", __func__, timeWaitRoot - timeSetAppDirs );
    }

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
            ProcessQtEvents(50);
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
            ProcessQtEvents(50);
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

        LogMsg( LOG_VERBOSE, "%s time register %d app dirs %d fonts %d", __func__, timeRegisterMetadata - timeStart,
                timeSetAppDirs - timeRegisterMetadata, timeInitFonts - timeSetAppDirs );
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
