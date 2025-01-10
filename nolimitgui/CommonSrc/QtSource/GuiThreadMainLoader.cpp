//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiThreadMainLoader.h"

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
GuiThreadMainLoader::GuiThreadMainLoader( QObject* parent )
    : QThread(parent)
{
}

//============================================================================
void GuiThreadMainLoader::run()
{
    int timeStart = GetApplicationAliveMs();

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

    bool result = IMediaPlayerRequests::getOsInterface().doPreStartup();

    result &= IMediaPlayerRequests::getOsInterface().initUserPaths( appCachePath, userWriteablePath );

    int timePreStartupEnd = GetApplicationAliveMs();
    LogMsg( LOG_VERBOSE, "GuiThreadMainLoader::run os interface startup took %d ms at %d ms",
           timePreStartupEnd - timeStart, timePreStartupEnd );


    GetPtoPEngine(); // engine first.. there is some interdependencies
    int mainLoadThreadEnd = GetApplicationAliveMs();
    LogMsg( LOG_VERBOSE, "GuiThreadMainLoader::run GetPtoPEngine complete in %d ms at %d",
           mainLoadThreadEnd - timePreStartupEnd, mainLoadThreadEnd );

    setIsLoadComplete( true );
}

