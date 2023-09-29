//============================================================================
// Copyright (C) 2009 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://www.nolimitconnect.com
//============================================================================

#include "LogMgr.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiParams.h"
#include "SoundMgr.h"
#include "MyIcons.h"

#include "VxPushButton.h"

#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGUID.h>

#include <QApplication>
#include <QMainWindow>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDebug>
#include <QSettings>

#include "RenderGlWidget.h"

namespace
{
	AppCommon * g_AppCommon = 0;

	QString GetAppTitle( EDefaultAppMode appMode )
	{
		switch( appMode )
		{
		case eAppModeDefault:
			return QObject::tr( "No Limit Connect" );
		case eAppModeNlcViewer:
			return QObject::tr( "NoLimitConnect Player" );
		case eAppModeNlcProvider:
			return QObject::tr( "NoLimitConnect Provider" );
		case eAppModeNlcStation:
			return QObject::tr( "NoLimitConnect Station" );
		case eAppModeNlcNetworkHost:
			return QObject::tr( "NoLimitConnect Network Host" );
		case eAppModeUnknown:
		default:
			return QObject::tr( "NoLimitConnect Unknown App" );	
		}
	}

	QString GetAppShortName( EDefaultAppMode appMode )
	{
		//NOTE: do not translate or will cause new settings each time user changes languages
		QString appShortName = VxGetApplicationNameNoSpaces(); 
		switch( appMode )
		{
		case eAppModeDefault:
			return "TeatApp";
		case eAppModeNlcViewer:
			return "NoLimitPlayer";
		case eAppModeNlcProvider:
			return "NoLimitProvider";
		case eAppModeNlcStation:
			return "NoLimitStation";
		case eAppModeNlcNetworkHost:
			return "NoLimitNetHost";
		case eAppModeUnknown:
		default:
			return "NoLimitUnknownApp";
		}
	}
}

//============================================================================
AppCommon& CreateAppInstance( QApplication* myApp )
{
static AppSettings appSettings;
    if( !g_AppCommon )
    {
        // constructor of AppCommon will set g_AppCommon
        new AppCommon( *myApp, eAppModeDefault, appSettings );
    }

    return *g_AppCommon;
}

//============================================================================
AppCommon& GetAppInstance( void )
{
	return *g_AppCommon;
}

//============================================================================
void DestroyAppInstance()
{
    delete g_AppCommon;
    g_AppCommon = nullptr;
}

//============================================================================
AppCommon::AppCommon( QApplication&	myQApp, EDefaultAppMode appDefaultMode, AppSettings& appSettings )
: QWidget()
, m_QApp( myQApp )
, m_AppDefaultMode( appDefaultMode )
, m_AppSettings( appSettings )
, m_AppShortName( GetAppShortName( appDefaultMode ) )
, m_AppTitle( GetAppTitle( appDefaultMode ) )
, m_MyIcons( *this )
, m_AppTheme( *this )
, m_AppStyle( *this, m_AppTheme )
, m_AppDisplay( *this )

, m_SoundMgr( * new SoundMgr( *this ) )

, m_OncePerSecondTimer( new QTimer( this ) )

, m_CamSourceId( 1 )
, m_CamCaptureRotation( 0 )

, m_CheckSetupTimer( new QTimer( this ) )
{
    g_AppCommon = this; // need a global instance that can accessed immediately with GetAppInstance() for objects created in ui files

		// set application short name used for directory paths
	VxSetApplicationNameNoSpaces( m_AppShortName.toUtf8().constData() );
	m_AppSettings.setAppShortName( m_AppShortName );

	// this just loads the ini file.
	// the AppSettings database is not initialized until loadAccountSpecificSettings
	m_AppSettings.loadProfile();
	// sets root of application data and transfer directories
	VxSetRootUserDataDirectory( m_AppSettings.m_strRootUserDataDir.c_str() );

	// Documents Directory/appshortName/xfer/		app data transfer directory
	VxSetRootXferDirectory( m_AppSettings.m_strRootXferDir.c_str() );

    connect( m_CheckSetupTimer, SIGNAL( timeout() ), this, SLOT( slotCheckSetupTimer() ) );
}

//============================================================================
int64_t AppCommon::elapsedMilliseconds( void )
{
	return GetApplicationAliveMs();
}

//============================================================================
void AppCommon::registerMetaData( void )
{
	qRegisterMetaType<uint8_t>( "uint8_t" );
	qRegisterMetaType<uint8_t>( "uint16_t" );
	qRegisterMetaType<int32_t>( "int32_t" );
	qRegisterMetaType<uint32_t>( "uint32_t" );
	qRegisterMetaType<uint64_t>( "uint64_t" );
	qRegisterMetaType<int64_t>( "int64_t" );

	qRegisterMetaType<EAudioTestState>( "EAudioTestState" );

}

//============================================================================
void AppCommon::loadWithoutThread( void )
{
    uint64_t startMs = elapsedMilliseconds();

    registerMetaData();

    // create settings database appshortname_settings.db3 in /appshortName/data/
    QString strSettingsDbFileName = VxGetAppNoLimitDataDirectory().c_str() + m_AppShortName + "_test_settings.db3";
    m_AppSettings.appSettingStartup( strSettingsDbFileName.toUtf8().constData(), m_AppDefaultMode );

    // Now that settings are set up we can enable logging with log settings
    GetLogMgrInstance().startupLogMgr();

    // asset database and user specific setting database will be created in sub directory of account login
    // after user has logged into account

    uint64_t loadingMs = GetApplicationAliveMs();
    LogMsg( LOG_DEBUG, "LoadSettings %" PRId64 " ms alive ms %" PRId64 "", loadingMs - startMs, loadingMs );

    // load icons from resources
    m_MyIcons.myIconsStartup();

    uint64_t iconsMs = GetApplicationAliveMs();
    LogMsg( LOG_DEBUG, "Load Icons %" PRId64 " ms alive ms %" PRId64 "", iconsMs - loadingMs, iconsMs );

	if( getAppSettings().getFeatureEnable( eAppFeatureTheme ) )
	{
		getQApplication().setStyle( &m_AppStyle );
	}

    // load sounds to play and sound hardware
    m_SoundMgr.sndMgrStartup();

    uint64_t styleMs = GetApplicationAliveMs();
    LogMsg( LOG_DEBUG, "Setup Style %" PRId64 " ms alive ms %" PRId64 "", styleMs - iconsMs, styleMs );

    setLoopbackMyselfTestAllowed( getAppSettings().getAllowLoopBackOfMyself() );

	SetUseMilitaryTime( getAppSettings().getUseMilitaryTime() );

    uint64_t homePageMs = GetApplicationAliveMs();
    LogMsg( LOG_DEBUG, "Initialize Home Page %" PRId64 " ms alive ms %" PRId64 "", homePageMs - styleMs, homePageMs );
}

//============================================================================
void AppCommon::startupAppCommon( void )
{
    if( m_AppCommonInitialized )
    {
        return;
    }

    m_AppCommonInitialized = true;
    // need to setup theme before the first window or dialog is created
    getAppTheme().selectTheme( getAppSettings().getLastSelectedTheme(), &getHomePage() );

	m_OncePerSecondTimer->setInterval( 1000 ); 
	connect( m_OncePerSecondTimer, SIGNAL(timeout()), this, SLOT( onOncePerSecond() ) );
	m_OncePerSecondTimer->start();

	std::string strAssetDir = m_AppSettings.m_strRootUserDataDir + "assets/";
	VxFileUtil::makeDirectory( strAssetDir );

    m_SoundMgr.audioIoSystemStartup();
}

//============================================================================
void AppCommon::shutdownAppCommon( void )
{
    static bool hasBeenShutdown = false;
    if( false == hasBeenShutdown )
    {
        hasBeenShutdown = true;
        VxSetAppIsShuttingDown( true );


		VxSleep( 1000 );
		m_SoundMgr.sndMgrShutdown();
        VxSleep( 1000 );

        QApplication::closeAllWindows();
    }
}

//============================================================================
void AppCommon::setIsMaxScreenSize( bool isMessagerFrame, bool isFullSizeWindow )
{

}

//============================================================================
bool AppCommon::getIsMaxScreenSize( bool isMessagerFrame )
{
    return false;
}

//============================================================================
void AppCommon::switchWindowFocus( QWidget* appIconButton )
{
}

//============================================================================
void AppCommon::setCamCaptureRotation( uint32_t rot )
{ 
	m_CamCaptureRotation = rot; 
}

//============================================================================
void AppCommon::applySoundSettings( bool useDefaultsInsteadOfSettings )
{
	if( useDefaultsInsteadOfSettings )
	{
		m_SoundMgr.muteNotifySound( false );
		m_SoundMgr.mutePhoneRing( false );
	}
	else
	{
		m_SoundMgr.muteNotifySound( m_AppSettings.getMuteNotifySound() );
		m_SoundMgr.mutePhoneRing( m_AppSettings.getMutePhoneRing() );
	}
}

//============================================================================
void AppCommon::insertKeystroke( int keyCode )
{
	if( 0 == keyCode )
	{
		LogMsg( LOG_ERROR, "AppCommon::insertKeystroke 0 keyCode" );
		return;
	}

	QWidget*receiver = QApplication::focusWidget();
	if( 0 == receiver )
	{
		receiver = getQApplication().activeWindow();
	}

	if( 0 == receiver )
	{
		LogMsg( LOG_DEBUG,  "AppCommon::insertKeystroke no reciever" );
		return;
	}

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    Qt::KeyboardModifiers keyFlags = 0;
#else
    Qt::KeyboardModifiers keyFlags;
#endif // QT_VERSION < QT_VERSION_CHECK(6,0,0)

	QString keyText = "";

	QKeyEvent * qtKeyEvent = new QKeyEvent( QEvent::KeyPress, keyCode, keyFlags, keyText );
	QCoreApplication::postEvent( receiver, qtKeyEvent );
	qtKeyEvent = new QKeyEvent( QEvent::KeyRelease, keyCode, keyFlags, keyText );
	QCoreApplication::postEvent( receiver, qtKeyEvent );
}

//============================================================================
void AppCommon::okMessageBox( QString title, QString msg )
{
	QMessageBox::information( this, title, msg, QMessageBox::Ok );
}

//============================================================================
void AppCommon::okMessageBox2( QString title, const char* msg, ... )
{
	char szBuffer[2048];
	va_list argList;
	va_start(argList, msg);
	vsnprintf( szBuffer, sizeof( szBuffer ), msg, argList );
	va_end(argList);

	okMessageBox( title, szBuffer );
}

//============================================================================
bool AppCommon::yesNoMessageBox( QString title, QString msg )
{
	if( QMessageBox::Yes == QMessageBox::question( this, title, msg, QMessageBox::Yes | QMessageBox::No ) )
	{
		return true;
	}

	return false;
}

//============================================================================
bool AppCommon::yesNoMessageBox2( QString title, const char* msg, ... )
{
	char szBuffer[2048];
	va_list argList;
	va_start(argList, msg);
	vsnprintf( szBuffer, sizeof( szBuffer ), msg, argList );
	va_end(argList);

	return yesNoMessageBox( title, szBuffer );
}

//============================================================================
void AppCommon::errMessageBox( QString title, QString msg )
{
	QMessageBox::warning( this, title, msg, QMessageBox::Ok );
}

//============================================================================
void AppCommon::errMessageBox2( QString title, const char* msg, ... )
{
	char szBuffer[2048];
	va_list argList;
	va_start(argList, msg);
	vsnprintf( szBuffer, sizeof( szBuffer ), msg, argList );
	va_end(argList);

	errMessageBox( title, szBuffer );
}

//============================================================================
void AppCommon::wantToGuiHardwareCtrlCallbacks( ToGuiHardwareControlInterface* callback, bool wantCallback )
{
	/*
	static bool actCallbackShutdownComplete = false;
	if( VxIsAppShuttingDown() )
	{
		if( actCallbackShutdownComplete )
		{
			return;
		}

		actCallbackShutdownComplete = true;
		clearHardwareCtrlList();
		return;
	}

	if( m_ToGuiHardwareCtrlBusy )
	{
		LogMsg( LOG_ERROR, "AppCommon::wantToGuiHardwareCtrlCallbacks do NOT call while busy" );
	}

	if( wantCallback )
	{
		for( auto iter = m_ToGuiHardwareCtrlList.begin(); iter != m_ToGuiHardwareCtrlList.end(); ++iter )
		{
			ToGuiHardwareControlInterface* client = *iter;
			if( client == callback )
			{
				LogMsg( LOG_INFO, "WARNING. Ignoring New wantToGuiHardwareCtrlCallbacks because already in list" );
				return;
			}
		}

		m_ToGuiHardwareCtrlList.push_back( callback );
		return;
	}

	for( auto iter = m_ToGuiHardwareCtrlList.begin(); iter != m_ToGuiHardwareCtrlList.end(); ++iter )
	{
		ToGuiHardwareControlInterface* client = *iter;
		if( client == callback )
		{
			m_ToGuiHardwareCtrlList.erase( iter );
			return;
		}
	}

	LogMsg( LOG_INFO, "WARNING. ToGuiHardwareControlInterface remove not found in list" );
	return;
	*/
}

//============================================================================
void AppCommon::clearHardwareCtrlList( void )
{
	/*
	if( m_ToGuiHardwareCtrlList.size() )
	{
		m_ToGuiHardwareCtrlList.clear();
	}
	*/
}

//============================================================================
/// Mute/Unmute microphone
void AppCommon::fromGuiMuteMicrophone( bool muteMic )
{
	m_SoundMgr.setMuteMicrophone( muteMic );
	// getEngine().fromGuiMuteMicrophone( muteMic );

	m_ToGuiHardwareCtrlBusy = true;
	for( auto hardwareIter = m_ToGuiHardwareCtrlList.begin(); hardwareIter != m_ToGuiHardwareCtrlList.end(); ++hardwareIter )
	{
		ToGuiHardwareControlInterface* toGuiClient = *hardwareIter;
		toGuiClient->callbackToGuiMicrophoneMuted( muteMic );
	}

	m_ToGuiHardwareCtrlBusy = false;
}

//============================================================================
/// Returns true if microphone is muted
bool AppCommon::fromGuiIsMicrophoneMuted( void )
{
	// return getEngine().fromGuiIsMicrophoneMuted();
	return false;
}

//============================================================================
/// Mute/Unmute speaker
void AppCommon::fromGuiMuteSpeaker( bool muteSpeaker )
{
	m_SoundMgr.setMuteSpeaker( muteSpeaker );
	// getEngine().fromGuiMuteSpeaker( muteSpeaker );

	m_ToGuiHardwareCtrlBusy = true;
	for( auto hardwareIter = m_ToGuiHardwareCtrlList.begin(); hardwareIter != m_ToGuiHardwareCtrlList.end(); ++hardwareIter )
	{
		ToGuiHardwareControlInterface* toGuiClient = *hardwareIter;
		toGuiClient->callbackToGuiSpeakerMuted( muteSpeaker );
	}

	m_ToGuiHardwareCtrlBusy = false;
}

//============================================================================
/// Returns true if speaker is muted
bool AppCommon::fromGuiIsSpeakerMuted( void )
{
	// return getEngine().fromGuiIsSpeakerMuted();
	return false;
}

//============================================================================
void AppCommon::fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLen )
{
}

//============================================================================
void AppCommon::fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnt, bool isSilence )
{
}
