//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "NlcCommonConfig.h"
#ifdef BUILD_NLC_APP

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
#include <NetLib/VxPeerMgr.h>
#include <AppInterface/INlc.h>

#if defined(ENABLE_KODI)
#include "Application.h"
#elif defined(ENABLE_NLC_PLAYER)
#include "MediaPlayerNlc.h"
#endif // ENABLE_KODI

#include <libplayer-nlc/xbmc/xbmc/filesystem/Directory.h>
#include <libplayer-nlc/xbmc/xbmc/filesystem/SpecialProtocol.h>
#include <libplayer-nlc/xbmc/xbmc/filesystem/File.h>
#include <libplayer-nlc/xbmc/xbmc/platform/Environment.h>
#include <libplayer-nlc/xbmc/xbmc/utils/log.h>

using namespace XFILE;


namespace{
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

#endif


int runApplication( QApplication* myApp, int argc, char** argv )
{

#if defined (Q_OS_ANDROID) && QT_VERSION < QT_VERSION_CHECK(6,0,0)
    //Request requiered permissions at runtime.. does not seem to work with Qt 6.2.0
    for(const QString &permission : permissions)
    {
        LogMsg( LOG_DEBUG, "requesting permission %s", permission.toUtf8().constData() );
        auto result = QtAndroid::checkPermission(permission);
        if(result == QtAndroid::PermissionResult::Denied)
        {
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
            {
                LogMsg( LOG_DEBUG, "DENIED permission %s", permission.toUtf8().constData() );
                delete myApp;
                return 0;
            }

            LogMsg( LOG_DEBUG, "ACCEPTED permission %s", permission.toUtf8().constData() );
        }
    }

    LogMsg( LOG_DEBUG, "permission done" );
#endif
#if defined (Q_OS_ANDROID)

    const QString externStoragePemission(QLatin1String ("android.permission.WRITE_EXTERNAL_STORAGE"));
    auto storagePermissionResult = QtAndroidPrivate::checkPermission(externStoragePemission).result();
    if( storagePermissionResult != QtAndroidPrivate::Authorized )
    {
        if( QtAndroidPrivate::Authorized !=  QtAndroidPrivate::requestPermission(externStoragePemission).result() )
        {
            LogMsg(LOG_INFO, "Cannot Proceed without external storage permission");
            return 0;
        }
    }
#endif // defined (Q_OS_ANDROID)

    // initialize display scaling etc
    // the best method I have found to scale the gui is to use the default font height as the scaling factor
    QFontMetrics fontMetrics( myApp->font() );
    GuiParams::initGuiParams(fontMetrics.height());

    // chicken and egg kind of thing.. we need the storage path here
    QGuiApplication::setApplicationDisplayName( VxGetApplicationTitle() );
    QCoreApplication::setOrganizationDomain( VxGetCompanyDomain() );
    // NOTE OrganizationName and ApplicationName become part of data storage location path
    QCoreApplication::setOrganizationName( "" ); // leave blank or will become part of data storage path
    QCoreApplication::setApplicationName( VxGetApplicationNameNoSpaces() );
    QCoreApplication::setApplicationVersion( VxGetAppVersionString() );

    // TODO allow user to change where the data is stored
    if( !setupRootStorageDirectory() )
    {
        QString warnWritableTitle = QObject::tr( "No Writable Location for application data" );
        QString warnWritableBody = QObject::tr( "No location found to store application data.\n Application will exit" );

        QMessageBox warnStorage( QMessageBox::Icon::Information, warnWritableTitle, warnWritableBody, QMessageBox::Ok );
        warnStorage.exec();
        return -1;
    }

    LogMsg( LOG_VERBOSE, "root storage disk space path %s %s", VxGetRootDataStorageDirectory().c_str(), VxFileUtil::describeDiskSpace( VxGetRootDataStorageDirectory() ).c_str() );

    INlc& nolimit = INlc::getINlc();
    nolimit.doPreStartup();
    LogMsg( LOG_VERBOSE, "runApplication doPreStartup complete" );

    GetPtoPEngine(); // engine first.. there is some interdependencies
    AppCommon& appCommon = CreateAppInstance( nolimit, myApp );

    LogMsg( LOG_VERBOSE, "runApplication appCommon complete" );

    if( !appCommon.hasExistingAccount() )
    {
        // adult warning
        QString warnAdultTitle = QObject::tr( "You must be an adult to use No Limit Connect Application" );
        QString warnAdultBody = QObject::tr( "Although No Limit Connect does not host any offensive media, users of No Limit Connect may host offensive material or act in an offensive manner.\n"
            "No Limit Connect does not monitor or log any user actions or content.\n\n"
            "Are you an adult and at least 18 years old?" );

        QMessageBox warnAdult( QMessageBox::Icon::Question, warnAdultTitle, warnAdultBody, QMessageBox::Yes | QMessageBox::No );
        if( QMessageBox::No == warnAdult.exec() )
        {
            return 0;
        }
    }

    appCommon.loadWithoutThread();

#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    // send command line parameters to Kodi
    INlc::getINlc().getNlcPlayer().fromGuiInitCommandLine( argc, argv );
#endif // ENABLE_KODI

    LogMsg( LOG_VERBOSE, "myApp->exec" );

    int result = myApp->exec();

	return result;
}

#endif // BUILD_NLC_APP

#if !defined(TARGET_OS_WINDOWS)
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/syscall.h>

/*
void signal_handler(int signum, siginfo_t *info, void *extra)
{
    LogMsg( LOG_ERROR, "Signal Handler signum %d thread ID: %d ", signum, syscall(SYS_gettid));
}

void set_signal_handler( void )
{
   static struct sigaction action;

    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = signal_handler;
    sigaction( SIGINT, &action, NULL );
}
*/

#endif // !defined(TARGET_OS_WINDOWS)

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

//#if !defined(TARGET_OS_WINDOWS)
//    set_signal_handler();
//#endif // !defined(TARGET_OS_WINDOWS)

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
