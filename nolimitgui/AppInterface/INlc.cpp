//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // for linux error 53 about subscript
#include <AppInterface/INlc.h>
#include <CoreLib/VxDebug.h>

#include "ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h"
#include "../CommonSrc/QtSource/AppCommon.h"

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxStringUtils.h>

#include <NetLib/VxPeerMgr.h>

#ifdef TARGET_OS_WINDOWS
# include "AppInterface/OsWin32/IWin32.h"
#elif TARGET_OS_LINUX
# include "AppInterface/OsLinux/ILinux.h"
#elif TARGET_OS_ANDROID
# include "AppInterface/OsAndroid/IAndroid.h"
# include "CoreLib/VxJava.h"
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
# include "platform/qt/qtandroid/jni/Context.h"
#endif // ENABLE_KODI
#else 
echo traget os is not defined
#endif 

#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
#include <utils/StringUtils.h>
#include <filesystem/Directory.h>
#include <filesystem/SpecialProtocol.h>
#include <filesystem/File.h>
#include <platform/Environment.h>
#include <utils/log.h>
using namespace XFILE;

#if defined(ENABLE_KODI)
#include <Application.h>
#elif defined(ENABLE_NLC_PLAYER)
#include <MediaPlayerNlc.h>
#endif // defined(ENABLE_NLC_PLAYER)

#endif // defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)

#include <QStandardPaths>

//============================================================================
VxPeerMgr& GetVxPeerMgr( void )
{
    static VxPeerMgr g_VxPeerMgr;
    return  g_VxPeerMgr;
}

//============================================================================
P2PEngine& GetPtoPEngine()
{
    static P2PEngine g_P2PEngine( GetVxPeerMgr() );
    return g_P2PEngine;
}

//============================================================================
P2PEngine& INlc::getPtoP()
{
    return GetPtoPEngine();
}

//============================================================================
INlc& INlc::getINlc()
{
    static INlc g_INlc;
    return g_INlc;
}

//============================================================================
IToGui& IToGui::getToGui()
{
    return INlc::getINlc();
}

//============================================================================
IAudioRequests& IToGui::getAudioRequests()
{
    return INlc::getINlc();
}

//============================================================================
AppCommon& INlc::getAppCommon()
{
    return GetAppInstance();
}

//============================================================================
void INlc::toGuiSetIsAppModuleRunning( EAppModule appModule, bool isRunning )
{ 
    m_IsRunning[ appModule ] = isRunning; 
    if( eAppModulePlayerNlc == appModule )
    {
        GetAppInstance().getPlayerMgr().toGuiPlayerNlcReady( isRunning );
    }
}

//============================================================================
bool INlc::toGuiRunModule( EAppModule appModule )
{
    if( eAppModulePlayerNlc == appModule )
    {
#if defined(ENABLE_NLC_PLAYER)
        m_NlcPlayer.fromStartModule( appModule );
        return true;
#endif // ENABLE_NLC_PLAYER
    }

    return false;
}

//============================================================================
void INlc::toGuiCreateUserDirs( void )
{
    createUserDirs();
}

//============================================================================
INlc::INlc()
: m_ILog()
#ifdef TARGET_OS_WINDOWS
, m_OsInterface( *new IWin32() )
#elif TARGET_OS_LINUX
, m_OsInterface( *new ILinux() )
#elif TARGET_OS_ANDROID
, m_OsInterface( *new IAndroid() )
#endif 

#if defined(ENABLE_KODI)
, m_Kodi( GetKodiInstance() )
#endif // ENABLE_KODI
#if defined(ENABLE_NLC_PLAYER)
, m_NlcPlayer( GetNlcPlayerInstance() )
#endif // ENABLE_NLC_PLAYER
{
    memset( m_IsRunning, 0, sizeof( m_IsRunning  ) );
}

//============================================================================
INlc::~INlc()
{
    delete &m_OsInterface;
}

//=== utilities ===//
//============================================================================
bool INlc::initDirectories()
{
    return m_OsInterface.initDirectories();
}

//============================================================================
void INlc::createUserDirs() const
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::createUserDirs");
    CDirectory::Create( "special://home/" );
    CDirectory::Create( "special://home/addons" );
    CDirectory::Create( "special://home/addons/packages" );
    CDirectory::Create( "special://home/addons/temp" );
    CDirectory::Create( "special://home/media" );
    CDirectory::Create( "special://home/system" );
    CDirectory::Create( "special://masterprofile/" );
    CDirectory::Create( "special://temp/" );
    CDirectory::Create( "special://logpath" );
    CDirectory::Create( "special://temp/temp" ); // temp directory for python and dllGetTempPathA

    // clear our archive cache before starting up anything more
    auto archiveCachePath = CSpecialProtocol::TranslatePath( "special://temp/archive_cache/" );
    if( CDirectory::Exists( archiveCachePath ) )
        if( !CDirectory::RemoveRecursive( archiveCachePath ) )
            CLog::Log( LOGWARNING, "Failed to remove the archive cache at %s", archiveCachePath.c_str() );
    CDirectory::Create( archiveCachePath );
#endif // ENABLE_KODI
}

//=== open ssl ===//
//============================================================================
void INlc::setSslCertFile( std::string certFile )
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::setSslCertFile (%s)", certFile.c_str());
    m_SslCertFile = certFile;
    if( !m_SslCertFile.empty() )
    {
        if( VxFileUtil::fileExists( certFile.c_str() ) )
        {
            setenv( "SSL_CERT_FILE", certFile.c_str(), 1 );
        }

        // TODO: anything special for linux?
    }
}

//=== stages of create ===//
//============================================================================
bool INlc::doPreStartup()
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::doPreStartup");
#ifdef TARGET_OS_ANDROID
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    CJNIContext::createJniContext( GetJavaEnvCache().getJavaVM(),  GetJavaEnvCache().getJavaEnv() );
# endif // ENABLE_KODI
#endif // TARGET_OS_ANDROID

    CLog::SetLogLevel( LOG_LEVEL_DEBUG );

    const int FFMPEG_LOG_LEVEL_NORMAL = 0; // shows notice, error, severe and fatal
    getILog().setFfmpegLogLevel( FFMPEG_LOG_LEVEL_NORMAL );
    // const int FFMPEG_LOG_LEVEL_DEBUG = 1; // shows all
    // getILog().setFfmpegLogLevel( FFMPEG_LOG_LEVEL_DEBUG ); // unusable

    QString appCachePathQ = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/";
    std::string appCachePath = appCachePathQ.toUtf8().constData();
    QString userWriteablePathQ = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
    std::string userWriteablePath = userWriteablePathQ.toUtf8().constData();

    bool result = m_OsInterface.doPreStartup();

    result &= m_OsInterface.initUserPaths( appCachePath, userWriteablePath );
	return result;
}

//============================================================================
bool INlc::doStartup()
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::doStartup");
    return m_OsInterface.doStartup();
}

//=== stages of run ===//
//============================================================================
bool INlc::initRun( const CAppParamParser& params )
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::initRun");
    return m_OsInterface.initRun( params );
}

//============================================================================
bool INlc::doRun( EAppModule appModule )
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::doRun");
    return m_OsInterface.doRun( appModule );
}

//=== stages of destroy ===//
//============================================================================
void INlc::doPreShutdown()
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::doPreShutdown");
    m_OsInterface.doPreShutdown();
}

//============================================================================
void INlc::doShutdown()
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::doShutdown");
    m_OsInterface.doShutdown();
}

//============================================================================
//=== to player nlc events ===//
//============================================================================
//============================================================================
void INlc::fromGuiKeyPressEvent( EAppModule appModule, int key, int mod )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiKeyPressEvent( appModule, key, mod );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiKeyReleaseEvent( EAppModule appModule, int key, int mod )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiKeyReleaseEvent( appModule, key, mod );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiMousePressEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiMousePressEvent( appModule, mouseXPos, mouseYPos, mouseButton );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiMouseReleaseEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiMouseReleaseEvent( appModule, mouseXPos, mouseYPos, mouseButton );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiMouseMoveEvent( EAppModule appModule, int mouseXPos, int mouseYPos )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiMouseMoveEvent( appModule, mouseXPos, mouseYPos );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
	getNlcPlayer().fromGuiResizeBegin( appModule, winWidth, winHeight );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiResizeEvent( appModule, winWidth, winHeight );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
	getNlcPlayer().fromGuiResizeEnd( appModule, winWidth, winHeight );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiCloseEvent( EAppModule appModule )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiCloseEvent( appModule );
#endif // ENABLE_KODI
}

//============================================================================
void INlc::fromGuiVisibleEvent( EAppModule appModule, bool isVisible )
{
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    getNlcPlayer().fromGuiVisibleEvent( appModule, isVisible );
#endif // ENABLE_KODI
}
