//============================================================================
// Copyright (C) 2019 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppSetup.h"
#include "AppGlobals.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QThread>

//============================================================================
bool AppSetup::areUserAssetsInitilized()
{
    return VxFileUtil::fileExists( getAssetsInitilizedFileName().c_str() );
}

//============================================================================
void AppSetup::initializeUserAssets()
{
    QThread* thread = new QThread();
    AppSetupWorker* worker = new AppSetupWorker();
    worker->moveToThread( thread );
    connect( worker, SIGNAL( error( QString ) ), this, SLOT( slotError( QString ) ) );
    connect( worker, SIGNAL( signalSetupResult( int, qint64 ) ), this, SLOT( slotSetupResult( int, qint64 ) ) );
    connect( thread, SIGNAL( started() ), worker, SLOT( process() ) );
    connect( worker, SIGNAL( finished() ), thread, SLOT( quit() ) );
    connect( worker, SIGNAL( finished() ), worker, SLOT( deleteLater() ) );
    connect( thread, SIGNAL( finished() ), thread, SLOT( deleteLater() ) );
    thread->start();
}

//============================================================================
std::string AppSetup::getAssetsInitilizedFileName()
{
    std::string rootDataPath = VxGetAppDirectory( eAppDirRootUserData );
    rootDataPath += SETUP_COMPLETE_FILE;
    return rootDataPath;
}

//============================================================================
void AppSetup::slotSetupResult( int rc, qint64 bytesCopied )
{
    if( rc )
    {
        LogMsg( LOG_ERROR, "AppSetup ERROR %d ", rc );
    }
    else
    {
        std::string completedFile = getAssetsInitilizedFileName();
        const char* completedText = "SUCCESS\n";
        VxFileUtil::writeWholeFile( completedFile .c_str(), (void *)completedText, (uint32_t)strlen( completedText ) );
        LogMsg( LOG_ERROR, "AppSetup SUCESS bytes copied %" PRId64 " ", bytesCopied );
    }

    emit signalSetupResult( this, rc, bytesCopied );
}

//============================================================================
void AppSetup::slotError( QString err )
{
    LogMsg( LOG_ERROR, "AppSetup thread error %s", err.toUtf8().constData() );
}

//============================================================================
void AppSetupWorker::process()
{
    std::string srcPath = VxGetAppDirectory( eAppDirAppExe );
    srcPath += "assets/";
    std::string destPath = VxGetAppDirectory( eAppDirRootDataStorage );
    int64_t bytesCopied = 0;
    RCODE rc = VxFileUtil::recursiveCopyDirectory( srcPath.c_str(), destPath.c_str(), bytesCopied );
    emit signalSetupResult( rc, bytesCopied );
}
