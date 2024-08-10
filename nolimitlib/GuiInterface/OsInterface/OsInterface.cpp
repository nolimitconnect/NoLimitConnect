//============================================================================
// Copyright (C) 2023 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "OsInterface.h"
#include <GuiInterface/IToGui.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTimer.h>

#include "CompileInfo.h"
#include "threads/Thread.h"
#include "platform/xbmc.h"
#include "settings/AdvancedSettings.h"
#include "utils/CPUInfo.h"
#include "platform/Environment.h"
#include "utils/CharsetConverter.h" // Required to initialize converters before usage

#if defined(TARGET_OS_WINDOWS)
#include "threads/platform/win/Win32Exception.h"
#include "platform/win32/CharsetConverter.h"

#include <dbghelp.h>
#include <mmsystem.h>
#include <Objbase.h>
#include <shellapi.h>
#include <WinSock2.h>
#endif //defined(TARGET_OS_WINDOWS)

#if defined(TARGET_OS_ANDROID)
# include "platform/qt/qtandroid/jni/Context.h"
# include "platform/qt/qtandroid/jni/System.h"
# include "platform/qt/qtandroid/jni/ApplicationInfo.h"
# include "platform/qt/qtandroid/jni/JNIFile.h"
# include <android/asset_manager.h>
# include <android/asset_manager_jni.h>
# include "AssetDirectoryList.hh"
#endif // TARGET_OS_ANDROID

#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/log.h"
#include "utils/URIUtils.h"
#include "NlcCoreUtil.h"
#include "platform/win32/WIN32Util.h"

#include "ServiceBroker.h"
#include "settings/SettingsComponent.h"

#include "FileItem.h"

#include <CoreLib/VxGlobals.h>

using namespace XFILE;

extern "C" int XBMC_Run( bool renderGUI );


namespace
{

//============================================================================
bool CopyIfRequiredAssetFile( std::string assetFileName, std::string destFile, bool replaceIfDifferent = true )
{
    bool result = false;
    if( !assetFileName.empty() && !destFile.empty() )
    {
        VxFileUtil::makeForwardSlashPath( assetFileName );
        VxFileUtil::makeForwardSlashPath( destFile );

        int64_t assetLength = VxFileUtil::fileExists( assetFileName.c_str() );
        int64_t destFileLen = VxFileUtil::fileExists( destFile.c_str() );
        if(  assetLength )
        {
            if( !destFileLen || ( replaceIfDifferent && ( assetLength != destFileLen ) ) )
            {
                std::string destDir = VxFileUtil::getJustPath( destFile );
                if( !VxFileUtil::directoryExists( destDir.c_str() ) )
                {
                    VxFileUtil::makeDirectory( destDir.c_str() );
                }

                char* buffer = (char*)malloc( assetLength + 1 );
                VxFileUtil::readWholeFile( assetFileName.c_str(), buffer, ( uint32_t )assetLength );
                buffer[ assetLength ] = 0;
                result = ( 0 == VxFileUtil::writeWholeFile( destFile.c_str(), buffer, ( uint32_t )assetLength ) );
                free( buffer );
            }
        }
        else
        {
            LogMsg( LOG_ERROR, "%s asset file %s does not exist", __func__, destFile.c_str() );
        }

        if( ( 0 != assetLength ) && !VxFileUtil::fileExists( destFile.c_str() ) )
        {
            LogMsg( LOG_ERROR, "Could not create file %s len %d", destFile.c_str(), assetLength );
        }
        else
        {
            result = true;
        }
    }

    return result;
}

//============================================================================
bool CopyIfRequiredAssetDirectory( std::string assetFileDir, std::string destDir, bool replaceIfDifferent = true )
{
    bool result = true;
    if( !assetFileDir.empty() && !destDir.empty() )
    {
        VxFileUtil::assureTrailingDirectorySlash( assetFileDir );
        VxFileUtil::assureTrailingDirectorySlash( destDir );
        VxFileUtil::makeDirectory( destDir );
        std::vector<std::string> fileList;

        if( 0 == VxFileUtil::listFilesInDirectory( assetFileDir.c_str(), fileList ) )
        {
            LogModule( eLogStartup, LOG_DEBUG, "%s copy %d app assets to read/write location %s -> %s", __func__, fileList.size(), assetFileDir.c_str(), destDir.c_str() );

            for( auto& fileNameAndPath : fileList )
            {
                //! remove the path and return just the file name
                std::string justFileName;
                VxFileUtil::getJustFileName( fileNameAndPath.c_str(), justFileName );

                result &= CopyIfRequiredAssetFile( assetFileDir + justFileName, destDir + justFileName, replaceIfDifferent );
            }
        }
        else
        {
            result = false;
            LogMsg( LOG_DEBUG, "Could list asset dir %s", assetFileDir.c_str() );
        }
    }
    else
    {
        result = false;
        LogMsg( LOG_DEBUG, "Empty asset directory name" );
    }

    return result;
}

} // anonymouse namespace

#if defined( TARGET_OS_ANDROID )

static AAssetManager* android_asset_manager = NULL;
void android_fopen_set_asset_manager(AAssetManager* manager) {
    android_asset_manager = manager;
}

//============================================================================
bool CopyIfRequiredApkFile( std::string apkFileName, std::string destFile, bool replaceIfDifferent = true )
{
    bool result = false;
    AAssetManager* mgr = android_asset_manager;
    vx_assert(NULL != mgr);
    if( mgr )
    {
        AAsset* apkAssetFile = AAssetManager_open(mgr, apkFileName.c_str(), AASSET_MODE_UNKNOWN);
        vx_assert( apkAssetFile );

        if ( apkAssetFile )
        {
            size_t assetLength = AAsset_getLength( apkAssetFile );
            u_int64_t destFileLen = VxFileUtil::fileExists( destFile.c_str(), false );
            if( !destFileLen || ( replaceIfDifferent && ( assetLength != destFileLen ) ) )
            {
                std::string destDir = VxFileUtil::getJustPath( destFile );
                if( !VxFileUtil::directoryExists( destDir.c_str() ) )
                {
                    VxFileUtil::makeDirectory( destDir.c_str() );
                }

                char* buffer = (char*) malloc(assetLength + 1);
                AAsset_read( apkAssetFile, buffer, assetLength);
                buffer[ assetLength ] = 0;
                result = ( 0 == VxFileUtil::writeWholeFile( destFile.c_str(), buffer, assetLength ) );
                free(buffer);
            }

            if( ( 0 != assetLength ) && !VxFileUtil::fileExists( destFile.c_str() ) )
            {
                LogMsg( LOG_ERROR, "Could not create file %s len %d", destFile.c_str(), assetLength );
            }
            else
            {
                result = true;
            }

            AAsset_close( apkAssetFile );

        }
        else
        {
            LogMsg( LOG_ERROR,  "Asset Manager Cannot open file %s", destFile.c_str() );
        }
    }
    else
    {
        LogMsg( LOG_ERROR,  "Asset Manager Cannot open apk file %s", apkFileName.c_str() );
    }

    return result;
}

//============================================================================
bool CopyIfRequiredApkDirectory( std::string apkFileDir, std::string destDir, bool replaceIfDifferent = true )
{
    bool result = true;
    VxFileUtil::assureTrailingDirectorySlash( destDir );
    if( !VxFileUtil::directoryExists( destDir.c_str() ) )
    {
        VxFileUtil::makeDirectory( destDir.c_str() );
        if( !VxFileUtil::directoryExists( destDir.c_str() ) )
        {
            LogMsg( LOG_ERROR,  "%s could not created directory %s", __func__, destDir.c_str() );
        }
    }

    AAssetManager* assetMgr = android_asset_manager;
    AAssetDir* assetDir = AAssetManager_openDir( assetMgr, apkFileDir.c_str() );
    if( assetDir )
    {
        // LogMsg( LOG_DEBUG, "%s copy apk directory to read/write location  %s", __func__, destDir.c_str() );
        const char* fileName;
        while ((fileName = AAssetDir_getNextFileName(assetDir)) != NULL)
        {
            std::string srcFile = apkFileDir + "/" + fileName;
            std::string destFile = destDir + fileName;
            // LogMsg( LOG_DEBUG, "%s copy apk file %s to read/write location  %s", __func__, srcFile.c_str(), destFile.c_str() );

            //__android_log_print(ANDROID_LOG_DEBUG, "Debug", filename);
            result &= CopyIfRequiredApkFile( srcFile, destFile, replaceIfDifferent );
        }
    }
    else
    {
        result = false;
        LogMsg( LOG_DEBUG, "%s Could not open apk dir %s", __func__, apkFileDir.c_str() );
    }

    return result;
}

#endif // defined( TARGET_OS_ANDROID )


//============================================================================
bool OsInterface::doRun( EAppModule appModule )
{
    LogModule( eLogStartup, LOG_VERBOSE, "OsInterface::doRun");

    if( !IToGui::getIToGui().toGuiGetIsAppModuleRunning( appModule ) )
    {
        if( eAppModulePlayerNlc == appModule )
        { 
            #if defined(TARGET_OS_ANDROID)
                int attachedThreadState = CJNIContext::getJniContext().attachThread();
            #endif //  TARGET_OS_ANDROID

            //CAppEnvironment::SetUp(m_CmdLineParams->GetAppParams());

			int runExitCode = XBMC_Run( true );
			setRunResultCode( runExitCode );
            #if defined(TARGET_OS_ANDROID)
                CJNIContext::getJniContext().detachThread( attachedThreadState );
            #endif //  TARGET_OS_ANDROID

            IToGui::getIToGui().toGuiSetIsAppModuleRunning( appModule, false );

            //CAppEnvironment::TearDown();
        }
    }

    return true;
}

//=== utilities ===//
//============================================================================

bool OsInterface::initUserPaths( std::string& appCachePath, std::string& userWriteablePath )
{
    LogModule( eLogStartup, LOG_VERBOSE, "OsInterface::initUserPaths");
        // special://xbmc 	Kodi's installation root directory. This path is read-only contains the Kodi binary, support libraries and default configuration files, skins, scripts and plugins. Users should not modify files or install addons in this directory. special://xbmc 	Kodi's installation root directory. This path is read-only contains the Kodi binary, support libraries and default configuration files, skins, scripts and plugins. Users should not modify files or install addons in this directory.
        // located in installdirectory/assets/kodi for windows and linux
        // located in cached apk folder for android at appwritablestorage/apk/assets

        // special://home 	Kodi's user specific (OS user) configuration directory. This path contains a writable version of the special://xbmc directories. Any addons should be installed here.
        // located in appwritablestorage/nlc/kodi

        // special://masterprofile 	Kodi's main configuration directory. Normally located at special://home/userdata, this directory contains global settings and sources, as well as any Kodi profile directories. Normally special://home/userdata
        // special://userdata 	Alias from special://masterprofile.
        // located in appwritablestorage/nlc/kodi/userdata

        // special:://system
        // located in appwritablestorage/nlc/kodi/system

        // special:://home
        // located in appwritablestorage/nlc/kodi/

#ifdef DEBUG
    VxTimer loadTimer;
#endif // DEBUG
    std::string strExePathAndFileName;
    std::string exePath;
    if( 0 == VxFileUtil::getExecuteFullPathAndName( strExePathAndFileName ) )
    {
        // strip the exe file name
        VxFileUtil::makeForwardSlashPath( strExePathAndFileName );
        size_t lastSlash = strExePathAndFileName.rfind( '/' );
        if( lastSlash != std::string::npos )
        {
            exePath = strExePathAndFileName.substr(0, lastSlash + 1 );
            VxFileUtil::makeForwardSlashPath( exePath );
            VxFileUtil::assureTrailingDirectorySlash( exePath );
            VxSetAppExeDirectory( exePath.c_str() );
        }
    }

#if defined(TARGET_OS_ANDROID)
    CJNIContext& jniContext = CJNIContext::getJniContext();
    AAssetManager* assetMgr = jniContext.getAssetManager();
    android_fopen_set_asset_manager( assetMgr );

    std::string systemLibsDir = CJNISystem::getProperty("java.library.path");
    std::string nativeLibsDir = jniContext.getApplicationInfo().nativeLibraryDir;

    // std::string apkResourceDir = jniContext.getPackageResourcePath();

    std::string apkCachePath = appCachePath;
    std::string kodiCachedApkRootPath = apkCachePath + "apkcache/";   
    if( !VxFileUtil::directoryExists( kodiCachedApkRootPath.c_str() ) )
    {
        VxFileUtil::makeDirectory( kodiCachedApkRootPath );
        if( !VxFileUtil::directoryExists( kodiCachedApkRootPath.c_str() ) )
        {
            LogMsg( LOG_ERROR, "%s Could not create apk cache directory %s", __func__, kodiCachedApkRootPath.c_str() );
        }
    }

    std::string kodiCachedApkAssetsPath = apkCachePath + "apkcache/assets/";
    if( !VxFileUtil::directoryExists( kodiCachedApkAssetsPath.c_str() ) )
    {
        VxFileUtil::makeDirectory( kodiCachedApkAssetsPath );
        if( !VxFileUtil::directoryExists( kodiCachedApkAssetsPath.c_str() ) )
        {
            LogMsg( LOG_ERROR, "%s Could not create assets cache directory %s", __func__, kodiCachedApkAssetsPath.c_str() );
        }
    }

    exePath = kodiCachedApkRootPath;

    VxSetAppExeDirectory( exePath.c_str() ); // we use the cached apk as the exe dir so kodi can find what is normally in the install directory

    std::string appRwStorageDir = userWriteablePath ;

    appRwStorageDir += "NoLimitConnect/";

    if( !VxFileUtil::directoryExists( appRwStorageDir.c_str() ) )
    {
        VxFileUtil::makeDirectory(appRwStorageDir);
        if( !VxFileUtil::directoryExists( appRwStorageDir.c_str() ) )
        {
            LogMsg( LOG_ERROR, "%s Could not created directory %s", __func__, appRwStorageDir.c_str() );
        }
    }

    VxSetAppDirectory( eAppDirRootDataStorage, appRwStorageDir.c_str() );

    LogModule( eLogStartup,  LOG_DEBUG, " Copy Android assets start " );

    LogModule( eLogStartup,  LOG_DEBUG, " storage (%s)\n resource (%s)\n sys (%s)\n native (%s) ", appRwStorageDir.c_str(), kodiCachedApkAssetsPath.c_str(), systemLibsDir.c_str(), nativeLibsDir.c_str() );

    // copy all apk files for nlc to use as the xbmc directory

    // unfortunately asset manager does not have something like sub directory detection so have to copy each directory individualy
    std::string apkVersionFile = kodiCachedApkAssetsPath + "/ApkFilesVersion.txt";
    std::string appVersionCode = CCompileInfo::GetVersionCode();
    if( appVersionCode != VxFileUtil::readVersionFile( apkVersionFile ) )
    {
        bool result = true;
        std::vector<std::string> dirList;
        getAssetDirList( dirList );
        for( auto iter = dirList.begin(); iter != dirList.end(); ++iter )
        {
            result &= CopyIfRequiredApkDirectory( *iter, kodiCachedApkAssetsPath + *iter );
        }

        if( result )
        {
            VxFileUtil::writeWholeFile( apkVersionFile.c_str(), (void *)appVersionCode.c_str(), (uint32_t)appVersionCode.length() );
        }
        else
        {
            #ifdef DEBUG
                LogMsg( LOG_DEBUG, "Copy kodi apk assets to cache failed " );
            #endif // DEBUG
        }
    }

    LogModule( eLogStartup,  LOG_DEBUG, " Copy Android assets complete " );

    // asset manager will not copy native libs
    //setenv( "KODI_ANDROID_LIBS", cachedLibsPath.c_str(), 0 );
    //CopyIfRequiredApkDirectory( nativeLibsDir, cachedLibsPath );

    // TODO: find out why nativeLibsDir looks like
    // /data/app/com.nolimitconnect.nolimitconnect-oO3ZXHSRDp2lPf3GpOMXrA==/lib/arm
    setenv( "KODI_ANDROID_LIBS", nativeLibsDir.c_str(), 0 );
    setenv( "KODI_ANDROID_SYSTEM_LIBS", systemLibsDir.c_str(), 0);
    setenv( "KODI_ANDROID_APK", kodiCachedApkRootPath.c_str(), 0);

#endif // #if defined(TARGET_OS_ANDROID)

    // root of where we can write files
    std::string appStorageDir = VxGetRootDataStorageDirectory();

    // determine where we can copy assets from
    // this is a special hack for running in linux from qcreator directly when built into nolimitgui/bin-OsName directory
    // the executable is built into a subdirectory of nolimitgui/bin-OsName directory
#if defined(FLATPAKBUILD)
    // flatpak cannot accese exe directory and is installed instead so no absolute path available
    std::string kodiExeAssetsPath = "assets/kodi";
#else
    std::string kodiExeAssetsPath = exePath + "assets/kodi";
#endif // defined(FLATPAKBUILD)

    std::string assetTest1 = kodiExeAssetsPath + "/userdata/guisettings.xml";
    if( !VxFileUtil::fileExists( assetTest1.c_str() ) )
    {
        std::string kodiTestAssetPath = VxFileUtil::moveUpADirectory(exePath);
        kodiTestAssetPath += "assets/kodi";
        std::string assetTest2 = kodiTestAssetPath + "/userdata/guisettings.xml";
        if( VxFileUtil::fileExists( assetTest2.c_str() ) )
        {
            kodiExeAssetsPath = kodiTestAssetPath;
        }
    }

    std::string kodiBinLibStoragePath = kodiExeAssetsPath + "/libs";
    VxFileUtil::makeDirectory( kodiBinLibStoragePath );

    std::string kodiUserStoragePath = appStorageDir + "nlc/kodi";
    VxFileUtil::makeDirectory( kodiUserStoragePath );

    std::string kodiDownloadAddonPath = kodiUserStoragePath + "/adddon";
    VxFileUtil::makeDirectory( kodiUserStoragePath );

    std::string nolimitStorageDir = appStorageDir + "nolimit";
    VxFileUtil::makeDirectory( nolimitStorageDir );

    std::string tempDir = appStorageDir + "temp";
    VxFileUtil::makeDirectory( tempDir );

    std::string logsDir = appStorageDir + "logs";
    VxFileUtil::makeDirectory( logsDir );

#ifdef DEBUG
    LogMsg( LOG_VERBOSE, "storage (%s) kodi (%s) nolimit (%s) log (%s) ", appStorageDir.c_str(), kodiUserStoragePath.c_str(), nolimitStorageDir.c_str(), logsDir.c_str() );
#endif // DEBUG

    // home of install distrubuted assets ( for android this is cached assets in storage/apkcache/assets/kodi )
    setenv( "KODI_HOME", kodiUserStoragePath.c_str(), 0 );

    // the execuatable binary assets path
    setenv( "KODI_BIN_HOME", kodiExeAssetsPath.c_str(), 0 );

    // this is where downloaded binary extentions to kodi executable is put
    setenv( "KODI_BINADDON_PATH", kodiBinLibStoragePath.c_str(), 0 );

    // writable storage root directory where downloaded addons and stuff goes
    setenv( "HOME", kodiUserStoragePath.c_str(), 0 );

    // temporary files
    setenv( "KODI_TEMP", tempDir.c_str(), 0 );

    // map our special drives

    VxFileUtil::makeForwardSlashPath( kodiExeAssetsPath );
    // special://xbmcbin/
    CSpecialProtocol::SetXBMCBinPath( kodiExeAssetsPath );
    // special://xbmc/   
    CSpecialProtocol::SetXBMCPath( kodiExeAssetsPath );

    VxFileUtil::makeForwardSlashPath( kodiUserStoragePath );

    std::string binAddonPath = kodiUserStoragePath + "/addons";
    VxFileUtil::makeDirectory( binAddonPath );
    CSpecialProtocol::SetXBMCBinAddonPath( binAddonPath );

    std::string binAltAddonPath = kodiExeAssetsPath + "/addons";
    VxFileUtil::makeDirectory( binAltAddonPath );
    CSpecialProtocol::SetXBMCAltBinAddonPath( binAltAddonPath );

    CSpecialProtocol::SetHomePath( kodiUserStoragePath );

    std::string masterProfilePath = kodiUserStoragePath + "/userdata";
    VxFileUtil::makeDirectory( masterProfilePath );
    CSpecialProtocol::SetMasterProfilePath( masterProfilePath );

    // there is not individual users for media player so just use master profile
    // special://masterprofile/   
    CSpecialProtocol::SetProfilePath( masterProfilePath );

    CSpecialProtocol::SetTempPath( tempDir );
    CSpecialProtocol::SetLogPath( logsDir );

    // setup paths so accessable outside of kodi
    VxFileUtil::assureTrailingDirectorySlash( exePath );

    VxFileUtil::makeForwardSlashPath( kodiExeAssetsPath );
    VxFileUtil::assureTrailingDirectorySlash( kodiExeAssetsPath );

    VxFileUtil::assureTrailingDirectorySlash( kodiBinLibStoragePath );

    VxFileUtil::assureTrailingDirectorySlash( kodiUserStoragePath );

    VxFileUtil::makeForwardSlashPath( kodiDownloadAddonPath );
    VxFileUtil::assureTrailingDirectorySlash( kodiDownloadAddonPath );

    VxFileUtil::makeForwardSlashPath( nolimitStorageDir );
    VxFileUtil::assureTrailingDirectorySlash( nolimitStorageDir );

    VxFileUtil::makeForwardSlashPath( tempDir );
    VxFileUtil::assureTrailingDirectorySlash( tempDir );

    VxFileUtil::makeForwardSlashPath( logsDir );
    VxFileUtil::assureTrailingDirectorySlash( logsDir );

    VxFileUtil::makeForwardSlashPath( binAddonPath );
    VxFileUtil::assureTrailingDirectorySlash( binAddonPath );

    VxFileUtil::makeForwardSlashPath( binAltAddonPath );
    VxFileUtil::assureTrailingDirectorySlash( binAltAddonPath );

    VxFileUtil::makeForwardSlashPath( masterProfilePath );
    VxFileUtil::assureTrailingDirectorySlash( masterProfilePath );


    VxSetKodiExeDirectory( exePath.c_str() );
    VxSetAppDirectory( eAppDirExeKodiAssets, kodiExeAssetsPath.c_str() );
    VxSetAppDirectory( eAppDirAppKodiData, kodiUserStoragePath.c_str() );
    VxSetAppDirectory( eAppDirExeNoLimitAssets, nolimitStorageDir.c_str() );

    // move some requried files into place for initial startup

    // advanced settings requires settings.xml
    std::string settingsXmlBinFile = CSpecialProtocol::TranslatePath( "special://xbmc/system/settings/settings.xml");

    std::string settingXmlPath = CSpecialProtocol::TranslatePath( "special://home/system/settings/");
    std::string settingsXmlKodiFile = settingXmlPath + "settings.xml";
    if( !VxFileUtil::fileExists( settingsXmlKodiFile.c_str() ) )
    {
        VxFileUtil::makeDirectory( settingXmlPath );
        CopyIfRequiredAssetFile( settingsXmlBinFile, settingsXmlKodiFile );
        if( !VxFileUtil::fileExists( settingsXmlKodiFile.c_str() ) )
        {
            LogMsg( LOG_ERROR, "ERROR could not copy settings file %s", settingsXmlKodiFile.c_str() );
        }
    }

#if defined(TARGET_OS_ANDROID)
    std::string androidBinXmlFile = CSpecialProtocol::TranslatePath( "special://xbmc/system/settings/android.xml" );
    std::string androidXmlFile = CSpecialProtocol::TranslatePath( "special://home/system/settings/android.xml" );
    if( !VxFileUtil::fileExists( androidXmlFile.c_str() ) )
    {
        CopyIfRequiredAssetFile( androidBinXmlFile, androidXmlFile );

        if( !VxFileUtil::fileExists( androidXmlFile.c_str() ) )
        {
            LogMsg( LOG_ERROR, "ERROR could not open settings file %s", androidXmlFile.c_str() );
        }
    }

#endif // #if defined(TARGET_OS_ANDROID)

    // initialize all the directories we can without the user specific directories that can only be set after log in
    // BRJ std::string nolimitDir = VxFileUtil::makeKodiPath( VxGetAppDirectory( eAppDirExeKodiAssets ).c_str() );
    // BRJ CSpecialProtocol::SetAppAssetsPath( URIUtils::AddFileToFolder( nolimitDir, "nolimitassets" ) );	// /exe/assets/nolimit/

    // BRJ nolimitDir = VxFileUtil::makeKodiPath( VxGetAppDirectory( eAppDirAppKodiData ).c_str() );
    // BRJ CSpecialProtocol::SetAppDataPath( URIUtils::AddFileToFolder( nolimitDir, "nolimitdata" ) );		// /storage/NoLimitConnect/nolimit/


    // Kodi master profile path ( In Roaming pm windows )
    // BRJ nolimitDir = CSpecialProtocol::TranslatePath( "special://masterprofile/" );
    // CEnvironment::setenv( CCompileInfo::GetUserProfileEnvName(), nolimitDir.c_str() );
    // LogMsg( LOG_VERBOSE, "master profile path %s", nolimitDir.c_str() );

#ifdef DEBUG
    LogMsg( LOG_VERBOSE, "Initalize directories took %3.3f sec", loadTimer.elapsedSec() );
#endif // DEBUG
	return true;
}

//============================================================================
bool OsInterface::initDirectories()
{
    LogModule(eLogStartup, LOG_VERBOSE, "OsInterface::initDirectories");

	//=== relative to executable paths ===//
    // do not call this until user logs on so that eAppDirUserSpecific is set

    std::string nolimitDir = VxFileUtil::makeKodiPath( VxGetAppDirectory( eAppDirUserSpecific ).c_str() );
    CSpecialProtocol::SetAccountsPath( URIUtils::AddFileToFolder( nolimitDir, "nolimitaccount" ) );	// /storage/nlc/accounts/userId/

	nolimitDir = VxFileUtil::makeKodiPath( VxGetAppDirectory( eAppDirSettings ).c_str() );
    CSpecialProtocol::SetUserGroupPath( URIUtils::AddFileToFolder( nolimitDir, "nolimitsettings" ) ); // /storage/nlc/accounts/userId/settings
   
    nolimitDir = VxFileUtil::makeKodiPath( VxGetAppDirectory( eAppDirUserXfer ).c_str() ); // Documents Directory/nlc/userId/   where transfer directories are
	CSpecialProtocol::SetUserXferPath( URIUtils::AddFileToFolder( nolimitDir, "nolimitxfer" ) ); 

    return true;
}
