//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiMainLoaderThread.h"

#include "GuiHelpers.h"

#include <GuiInterface/IMediaPlayerRequests.h>
#include <GuiInterface/OsInterface/OsInterface.h>

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxJava.h>
#include <CoreLib/VxTime.h>

#ifdef TARGET_OS_ANDROID
# include "platform/qt/qtandroid/jni/Context.h"
#endif // TARGET_OS_ANDROID

#include <QMessageBox>
#include <QStandardPaths>

//============================================================================
GuiMainLoaderThread::GuiMainLoaderThread( QObject* parent )
    : QThread(parent)
{
    m_ElapsedTimer.start();
}

//============================================================================
GuiMainLoaderThread::~GuiMainLoaderThread()
{

}

//============================================================================
void GuiMainLoaderThread::run()
{
    int64_t timeStart = GetApplicationAliveMs();
    LogMsg( LOG_VERBOSE, "GuiMainLoaderThread::run start in %d ms", timeStart );

    #ifdef TARGET_OS_ANDROID
    CJNIContext::createJniContext( GetJavaEnvCache().getJavaVM(),  GetJavaEnvCache().getJavaEnv() );
    #endif // TARGET_OS_ANDROID

    QString appCachePathQ = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/";
    std::string appCachePath = appCachePathQ.toUtf8().constData();

    QString userWriteablePathQ = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
    std::string userWriteablePath = userWriteablePathQ.toUtf8().constData();
    if( !VxFileUtil::testIsWritablePath( userWriteablePath ) )
    {
       // fallback to root storage
        userWriteablePath = VxGetRootDataStorageDirectory();
        userWriteablePath += "userdata/";

        if( !VxFileUtil::testIsWritablePath( userWriteablePath ) )
        {
            QString warnWritableTitle = QObject::tr( "No Writable Location for user data" );
            QString warnWritableBody = QObject::tr( "No location found to store user data.\n Application will exit" );

            QMessageBox warnStorage( QMessageBox::Icon::Information, warnWritableTitle, warnWritableBody, QMessageBox::Ok );
            warnStorage.exec();
            return;
        }
    }

    LogMsg( LOG_VERBOSE, "cache storage path %s disk space %s", appCachePath.c_str(), VxFileUtil::describeDiskSpace( appCachePath ).c_str() );
    LogMsg( LOG_VERBOSE, "user storage path %s disk space %s", userWriteablePath.c_str(), VxFileUtil::describeDiskSpace( userWriteablePath ).c_str() );

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

    bool result = IMediaPlayerRequests::getOsInterface().doPreStartup();

    result &= IMediaPlayerRequests::getOsInterface().initUserPaths( appCachePath, userWriteablePath );

    LogMsg( LOG_VERBOSE, "GuiMainLoaderThread::run os interface startup complete %d ms", GetApplicationAliveMs() );


    GetPtoPEngine(); // engine first.. there is some interdependencies
    LogMsg( LOG_VERBOSE, "GuiMainLoaderThread::run GetPtoPEngine complete in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    setIsLoadComplete( true );
}

