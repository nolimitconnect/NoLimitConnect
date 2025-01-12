//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiThreadSettingsLoader.h"

#include "AppSettings.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTime.h>

#include <QStandardPaths>

//============================================================================
void GuiThreadSettingsLoader::run()
{
    int timeStart = GetApplicationAliveMs();

    setupRootStorageDirectory();
    setIsRootStorageSet( true );
    int timeRootDirSetup = GetApplicationAliveMs();

    // create settings database appshortname_settings.db3 in /appshortName/data/
    std::string strSettingsDbFileName = VxGetAppNoLimitDataDirectory() + m_AppSettings.getAppShortName() + "_settings.db3";
    m_AppSettings.appSettingStartup( strSettingsDbFileName.c_str() );
   
    VxSetLogLevelFlags( m_AppSettings.getLogLevels() );
    VxSetModuleLogFlags( m_AppSettings.getLogModules() );

    setIsSettingsLoaded( true );
    if( LogEnabled( eLogStartup ) )
    {
        int timeAppSettingsSetup = GetApplicationAliveMs();
        LogMsg( LOG_VERBOSE, "Setup Time root storage %d ms app settings %d ms",
                timeRootDirSetup - timeStart, timeAppSettingsSetup - timeRootDirSetup );
    }   
}

//============================================================================
void GuiThreadSettingsLoader::setupRootStorageDirectory()
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

    rootXferDir += VxGetApplicationNameNoSpacesLowerCase();
    VxFileUtil::assurePathEndWithSlash( rootXferDir );
    VxFileUtil::makeDirectory( rootXferDir.c_str() );

    // sets root of data transfer directories
    
    VxSetRootXferDirectory( rootXferDir.c_str() );

    if( !VxFileUtil::directoryExists( rootXferDir.c_str() ) )
    {
        LogMsg( LOG_ERROR, "%s Could not create xfer dir %s", __func__, rootXferDir.c_str());
    }
}
