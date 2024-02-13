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

#include "P2PEngine/P2PEngine.h"
#include "../CommonSrc/QtSource/AppCommon.h"
#include "../CommonSrc/QtSource/GuiPlayerMgr.h"
#include <Membership/MemberActiveMgr.h>
#include <OfferBase/OfferMgr.h>
#include <PushToTalk/PushToTalkMgr.h>
#include <RandConnect/RandConnectMgr.h>
#include <SendQueue/SendQueueMgr.h>

#include <CoreLib/VxDebug.h>
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

# include "platform/qt/qtandroid/jni/Context.h"

#else 
echo traget os is not defined
#endif 

#include <utils/StringUtils.h>
#include <filesystem/Directory.h>
#include <filesystem/SpecialProtocol.h>
#include <filesystem/File.h>
#include <platform/Environment.h>
#include <utils/log.h>
using namespace XFILE;

#include <MediaPlayerNlc.h>

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
    static MemberActiveMgr memberActiveMgr;
    static OfferMgr offerMgr;
    static PushToTalkMgr pushToTalkMgr;
    static RandConnectMgr randConnectMgr;
    static SendQueueMgr sendQueueMgr;
    static P2PEngine g_P2PEngine( GetVxPeerMgr(), memberActiveMgr, offerMgr, pushToTalkMgr, randConnectMgr, sendQueueMgr );
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
        m_NlcPlayer.fromStartModule( appModule );
        return true;
    }

    return false;
}

//============================================================================
bool INlc::toGuiStopModule( EAppModule appModule )
{
    if( eAppModulePlayerNlc == appModule )
    {
        m_NlcPlayer.fromStopModule( appModule );
        return true;
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
, m_NlcPlayer( GetNlcPlayerInstance() )
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
    }
}

//=== stages of create ===//
//============================================================================
bool INlc::doPreStartup()
{
    LogModule(eLogStartup, LOG_VERBOSE, "INlc::doPreStartup");
#ifdef TARGET_OS_ANDROID
    CJNIContext::createJniContext( GetJavaEnvCache().getJavaVM(),  GetJavaEnvCache().getJavaEnv() );
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
            return false;
        }
    }

    LogMsg( LOG_VERBOSE, "cache storage disk space path %s %s", appCachePath.c_str(), VxFileUtil::describeDiskSpace( appCachePath ).c_str() );
    LogMsg( LOG_VERBOSE, "user storage disk space path %s %s", userWriteablePath.c_str(), VxFileUtil::describeDiskSpace( userWriteablePath ).c_str() );

    bool result = m_OsInterface.doPreStartup();

    return result && m_OsInterface.initUserPaths( appCachePath, userWriteablePath );
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
    getNlcPlayer().fromGuiKeyPressEvent( appModule, key, mod );
}

//============================================================================
void INlc::fromGuiKeyReleaseEvent( EAppModule appModule, int key, int mod )
{
    getNlcPlayer().fromGuiKeyReleaseEvent( appModule, key, mod );
}

//============================================================================
void INlc::fromGuiMousePressEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton )
{
    getNlcPlayer().fromGuiMousePressEvent( appModule, mouseXPos, mouseYPos, mouseButton );
}

//============================================================================
void INlc::fromGuiMouseReleaseEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton )
{
    getNlcPlayer().fromGuiMouseReleaseEvent( appModule, mouseXPos, mouseYPos, mouseButton );
}

//============================================================================
void INlc::fromGuiMouseMoveEvent( EAppModule appModule, int mouseXPos, int mouseYPos )
{
    getNlcPlayer().fromGuiMouseMoveEvent( appModule, mouseXPos, mouseYPos );
}

//============================================================================
void INlc::fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight )
{
	getNlcPlayer().fromGuiResizeBegin( appModule, winWidth, winHeight );
}

//============================================================================
void INlc::fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight )
{
    getNlcPlayer().fromGuiResizeEvent( appModule, winWidth, winHeight );
}

//============================================================================
void INlc::fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight )
{
	getNlcPlayer().fromGuiResizeEnd( appModule, winWidth, winHeight );
}

//============================================================================
void INlc::fromGuiCloseEvent( EAppModule appModule )
{
    getNlcPlayer().fromGuiCloseEvent( appModule );
}

//============================================================================
void INlc::fromGuiVisibleEvent( EAppModule appModule, bool isVisible )
{
    getNlcPlayer().fromGuiVisibleEvent( appModule, isVisible );
}
