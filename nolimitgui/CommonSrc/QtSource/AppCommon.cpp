//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"

#include "ToGuiActivityInterface.h"

#include "HomeWindow.h"
#include "GuiParams.h"
#include "LogMgr.h"
#include "MyIcons.h"
#include "SoundMgr.h"

#include "ActivityCreateAccount.h"
#include "ActivityShowHelp.h"
#include "ActivitySoundOptions.h"
#include "ActivityYesNoMsgBox.h"

#include "AppletDownloads.h"
#include "AppletUploads.h"

#include "AccountMgr.h"
#include "AppModuleState.h"
#include "AppSettings.h"
#include "AppletMgr.h"

#include "FileListReplySession.h"

#include "GuiAppLoaderThread.h"
#include "GuiMemberActiveMgr.h"
#include "GuiOfferSession.h"
#include "GuiPlayerMgr.h"
#include "GuiPluginMgr.h"
#include "GuiPushToTalkMgr.h"
#include "GuiSendQueueMgr.h"
#include "GuiRandConnectMgr.h"

#include "VxPushButton.h"

#include <BlobXferMgr/BlobInfo.h>
#include <P2PEngine/FileShareSettings.h>
#include <P2PEngine/EngineSettings.h>
#include <P2PEngine/P2PEngine.h>

#include <NetworkMonitor/NetworkMonitor.h>
#include <ThumbMgr/ThumbInfo.h>

#include <PktLib/VxCommon.h>

#include <CoreLib/AppVersion.h>
#include <CoreLib/ConnectId.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxSktUtil.h>

#include <CoreLib/InetAddressParse.h>

#include <NetLib/VxPeerMgr.h>

#include <QApplication>
#include <QMainWindow>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDebug>
#include <QSettings>

#include "RenderGlWidget.h"

#include <signal.h>
#include <array>

namespace
{
	AppCommon * g_AppCommon = 0;

	const int PROCESS_QT_DEFAULT_MS = 50;
	void ProcessQtEvents( int ms = PROCESS_QT_DEFAULT_MS )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}

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
		return APP_NAME;
	}
}

//============================================================================
AppCommon& CreateAppInstance( QApplication* myApp )
{
static AppModuleState appModuleState;
static AppSettings appSettings;
static AccountMgr accountMgr;
static GuiMemberActiveMgr memberActiveMgr;
static GuiPlayerMgr playerMgr;
static GuiPluginMgr pluginMgr;
static GuiPushToTalkMgr pushToTalkMgr;
static GuiRandConnectMgr randConnectMgr;
static GuiSendQueueMgr sendQueueMgr;
static MyIcons myIcons;
    if( !g_AppCommon )
    {
		// need sockets right away so can check for port in use when creating new account
		VxSocketsStartup();

        // constructor of AppCommon will set g_AppCommon
        new AppCommon( *myApp, eAppModeDefault, appModuleState, appSettings, accountMgr, 
					   memberActiveMgr, playerMgr, pluginMgr, pushToTalkMgr, randConnectMgr, 
					   sendQueueMgr, myIcons );
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
AppCommon::AppCommon(	QApplication&	myQApp,
						EDefaultAppMode appDefaultMode,
						AppModuleState& appModuleState,
						AppSettings&	appSettings, 
                        AccountMgr&	    accountMgr,
						GuiMemberActiveMgr& memberActiveMgr,
					    GuiPlayerMgr& playerMgr,
						GuiPluginMgr& pluginMgr,
						GuiPushToTalkMgr& pushToTalkMgr,
						GuiRandConnectMgr& randConnectMgr,
						GuiSendQueueMgr& sendQueueMgr,
						MyIcons& myIcons )
: QWidget()
, m_QApp( myQApp )
, m_AppDefaultMode( appDefaultMode )
, m_AppModuleState(appModuleState)
, m_AppGlobals( *this )
, m_AppSettings( appSettings )
, m_AppShortName( GetAppShortName( appDefaultMode ) )
, m_AppTitle( GetAppTitle( appDefaultMode ) )
, m_AccountMgr( accountMgr )
, m_ConnectIdListMgr( *this )
, m_FileXferMgr( *this )
, m_ThumbMgr( *this )
, m_MemberActiveMgr( memberActiveMgr )
, m_OfferMgr( *this )
, m_PlayerMgr( playerMgr )
, m_PluginMgr( pluginMgr )
, m_PushToTalkMgr( pushToTalkMgr )
, m_RandConnectMgr( randConnectMgr )
, m_SendQueueMgr( sendQueueMgr )
, m_UserMgr( *this )
, m_GroupieListMgr( *this )
, m_HostedListMgr( *this )
, m_HostJoinMgr( *this )
, m_UserJoinMgr( *this )
, m_WebPageMgr( *this )

, m_MyIcons( myIcons )
, m_AppTheme( *this )
, m_AppStyle( *this, m_AppTheme )
, m_AppDisplay( *this )

, m_CamLogic( *this )

, m_SoundMgr( * new SoundMgr( *this ) )

, m_OncePerSecondTimer( new QTimer( this ) )
, m_eLastSelectedWhichContactsToView( eFriendViewEverybody )
, m_bUserCanceledCreateProfile( false )
, m_LastNetworkState( eNetworkStateTypeUnknown )
, m_CamSourceId( 1 )
, m_CamCaptureRotation( 0 )
, m_AppletMgr( *( new AppletMgr( *this, this) ) )
, m_GuiStartupTimer( new QTimer( this ) )
, m_CheckSetupTimer( new QTimer( this ) )
{
    g_AppCommon = this; // need a global instance that can accessed immediately with GetAppInstance() for objects created in ui files

#if !defined(TARGET_OS_WINDOWS)
	// make your application ignore SIGPIPE. that sometimes happens when socket connection is broken
	// If you ignore the SIGPIPE signal, then the functions will return EPIPE erro
	signal( SIGPIPE, SIG_IGN );
#endif // !defined(TARGET_OS_WINDOWS)

	// this just loads the ini file.
	// the AppSettings database is not initialized until loadAccountSpecificSettings
	m_AppSettings.loadProfile();
	// sets root of application data and transfer directories
	VxSetRootUserDataDirectory( m_AppSettings.m_strRootUserDataDir.c_str() );

	// Documents Directory/appshortName/xfer/		app data transfer directory
	VxSetRootXferDirectory( m_AppSettings.m_strRootXferDir.c_str() );

    connect( m_CheckSetupTimer, SIGNAL(timeout()), this, SLOT(slotCheckSetupTimer()) );
	connect( m_GuiStartupTimer, SIGNAL(timeout()), this, SLOT(slotGuiStartupTimer()) );
}

//============================================================================
int64_t AppCommon::elapsedMilliseconds( void )
{
	return GetApplicationAliveMs();
}

//============================================================================
IFromGui& AppCommon::getFromGuiInterface( void )
{
    return getEngine().getFromGuiInterface();
}

//============================================================================
bool AppCommon::loadWithoutThread( void )
{
	GuiAppLoaderThread appLoaderThread( *this );
	appLoaderThread.start();

    // asset database and user specific setting database will be created in sub directory of account login
    // after user has logged into account

	while( !appLoaderThread.getIsSettingsLoaded() )
	{
		ProcessQtEvents( PROCESS_QT_DEFAULT_MS );
	}

	getQApplication().setStyle( &m_AppStyle );

	while( !appLoaderThread.getIsAccountMgrLoaded() )
	{
		ProcessQtEvents( PROCESS_QT_DEFAULT_MS );
	}

	m_ThumbMgr.onAppCommonCreated();
	m_UserMgr.onAppCommonCreated();
	m_OfferMgr.onAppCommonCreated();
	m_HostedListMgr.onAppCommonCreated();
	m_HostJoinMgr.onAppCommonCreated();
	m_UserJoinMgr.onAppCommonCreated();
	m_WebPageMgr.onAppCommonCreated();
	m_ConnectIdListMgr.onAppCommonCreated();
	m_MemberActiveMgr.onAppCommonCreated();
	m_RandConnectMgr.onAppCommonCreated();
	m_SendQueueMgr.onAppCommonCreated();
	m_GroupieListMgr.onAppCommonCreated();

	while( !appLoaderThread.getIsIconsLoaded() )
	{
		ProcessQtEvents( PROCESS_QT_DEFAULT_MS );
	}

	if( !hasExistingAccount() )
    {
        // adult warning
        QString warnAdultTitle = QObject::tr( "You must be an adult to use No Limit Connect application" );
        QString warnAdultBody = QObject::tr( "Although No Limit Connect does not host any offensive media, users of No Limit Connect may host offensive material or act in an offensive manner.\n"
                                            "No Limit Connect does not monitor or log any user actions or content.\n\n"
                                            "Are you an adult and at least 18 years old?" );

        ActivityYesNoMsgBox confirmAdult( *this, nullptr, warnAdultTitle, warnAdultBody );
		getAppTheme().selectTheme( getAppSettings().getLastSelectedTheme(), &confirmAdult );
        if( QDialog::Rejected == confirmAdult.exec() )
        {
            return false;
        }
    }

	m_HomeWindow = new HomeWindow( *this, m_AppTitle );
	getAppTheme().selectTheme( getAppSettings().getLastSelectedTheme(), m_HomeWindow );
    m_HomeWindow->initializeHomePage();

    connect( m_HomeWindow, SIGNAL(signalMainWindowResized()), this, SLOT(slotMainWindowResized()) );
	m_HomeWindow->show();

	connect( this, SIGNAL(signalInternalNetAvailStatus(ENetAvailStatus)), this, SLOT(slotInternalNetAvailStatus(ENetAvailStatus)), Qt::QueuedConnection );

	while( !appLoaderThread.getIsLoadComplete() )
	{
		ProcessQtEvents( PROCESS_QT_DEFAULT_MS );
	}

	appLoaderThread.quit();

	return true;
}

//============================================================================
P2PEngine& AppCommon::getEngine( void ) 
{ 
	return GetPtoPEngine();
}

//============================================================================
void AppCommon::startupAppCommon( QFrame* appletFrame, QFrame* messangerFrame )
{
    if( m_AppCommonInitialized )
    {
        return;
    }

    m_AppCommonInitialized = true;

	m_AppletDownloads = new AppletDownloads( *this, appletFrame );
    m_AppletDownloads->hide();

	m_AppletUploads = new AppletUploads( *this, appletFrame );
	m_AppletUploads->hide();

	m_CreateAccountDlg = new ActivityCreateAccount( *this, appletFrame );

	std::string strAssetDir = m_AppSettings.m_strRootUserDataDir + "assets/";
	VxFileUtil::makeDirectory( strAssetDir );

	connect( m_GuiStartupTimer, SIGNAL(timeout()), this, SLOT(slotGuiStartupTimer()), Qt::QueuedConnection );
	m_GuiStartupTimer->setSingleShot( true );
	m_GuiStartupTimer->setInterval( 1000 );
	m_GuiStartupTimer->start();
}

//============================================================================
void AppCommon::slotGuiStartupTimer( void )
{
	static int guiStartupStep = 0;
	guiStartupStep++;
	if( 1 == guiStartupStep )
	{
		// load sounds to play and sound hardware
		m_SoundMgr.sndMgrStartup();
		m_PlayerMgr.playerMgrStartup();

		connectSignals();
		m_GuiStartupTimer->start();
	}
	else if( 2 == guiStartupStep )
	{
		m_SoundMgr.audioIoSystemStartup();

		m_GuiStartupTimer->start();
	}
	else if( 3 == guiStartupStep )
	{
		std::string strAssetDir = m_AppSettings.m_strRootUserDataDir + "assets/";
		getEngine().fromGuiAppStartup( strAssetDir.c_str(), m_AppSettings.m_strRootUserDataDir.c_str() );
		m_GuiStartupTimer->start();
	}
	else if( 4 == guiStartupStep )
	{
		m_OncePerSecondTimer->setInterval( 1000 ); 
		connect( m_OncePerSecondTimer, SIGNAL(timeout()), this, SLOT(onOncePerSecond()) );
		m_OncePerSecondTimer->start();
	}
}

//============================================================================
void AppCommon::shutdownAppCommon( void )
{
    static bool hasBeenShutdown = false;
    if( false == hasBeenShutdown )
    {
        hasBeenShutdown = true;
        VxSetAppIsShuttingDown( true );
        fromGuiCloseEvent( eAppModuleAll );
		ActivityBase* appPlayer = m_AppletMgr.findAppletDialog( eAppletPlayerNlc );
		if( appPlayer )
		{
			// to force media player stop before exit application
			appPlayer->onActivityFinish();
		}

		VxSleep( 1000 );
		m_SoundMgr.sndMgrShutdown();
        VxSleep( 1000 );

        QApplication::closeAllWindows();
        getEngine().fromGuiAppShutdown();
    }
}

//============================================================================
void AppCommon::setIsMaxScreenSize( bool isMessagerFrame, bool isFullSizeWindow )
{
    m_HomeWindow->setIsMaxScreenSize( isMessagerFrame, isFullSizeWindow );
    emit signalMainWindowResized();
}

//============================================================================
bool AppCommon::getIsMaxScreenSize( bool isMessagerFrame )
{
    return m_HomeWindow->getIsMaxScreenSize( isMessagerFrame );
}

//============================================================================
void AppCommon::switchWindowFocus( QWidget* appIconButton )
{
	m_HomeWindow->switchWindowFocus( appIconButton );
}

//============================================================================
QFrame* AppCommon::getAppletFrame( EApplet applet )
{
	return getHomeWindow().getAppletFrame( applet );
}

//============================================================================
ActivityBase* AppCommon::launchApplet( EApplet applet, QWidget* parent )
{
	return m_AppletMgr.launchApplet( applet, parent );
}

//============================================================================
ActivityBase* AppCommon::launchApplet( EApplet applet, QWidget* parent, QString launchParam, VxGUID& assetId )
{
	return m_AppletMgr.launchApplet( applet, parent, launchParam, assetId );
}

//============================================================================
void AppCommon::activityStateChange( ActivityBase* activity, bool isCreated )
{
	m_AppletMgr.activityStateChange( activity, isCreated );
}

//============================================================================
void AppCommon::setCamCaptureRotation( uint32_t rot )
{ 
	m_CamCaptureRotation = rot; 
}

//============================================================================
void AppCommon::slotMainWindowResized( void )
{
	emit signalMainWindowResized();
}

//============================================================================
void AppCommon::slotMainWindowMoved( void )
{
    emit signalMainWindowMoved();
}

//============================================================================
VxNetIdent* AppCommon::getMyNetIdent( void )
{
	return getAppGlobals().getMyNetIdent();
}

//============================================================================
VxGUID& AppCommon::getMyOnlineId( void )
{
    return getMyNetIdent()->getMyOnlineId();
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
void AppCommon::playSound( ESndDef sndDef )
{
	emit signalPlaySound( sndDef );
}

//============================================================================
void AppCommon::slotPlaySound( ESndDef sndDef )
{
	getSoundMgr().playSnd( sndDef );
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
void AppCommon::toGuiLog( int logFlags, const char* pMsg )
{
	if( VxIsAppShuttingDown() 
		|| ( 0 == pMsg ) )
	{
		return;
	}

    QString strMsg = pMsg;
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
	strMsg.remove(QRegularExpression("[\\n\\r]"));
#else
	strMsg.remove(QRegExp("[\\n\\r]"));
#endif // QT_VERSION > QT_VERSION_CHECK(6,0,0)

#ifdef TARGET_OS_WINDOWS
    qDebug() << strMsg;
#endif // TARGET_OS_WINDOWS
    emit signalLog( logFlags, strMsg );
}

//============================================================================
void AppCommon::toGuiAppErr( EAppErr eAppErr, const char* errMsg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	std::string formatedErr;
	if( errMsg )
	{
		StdStringFormat( formatedErr, "#App Error %d %s", eAppErr, errMsg );
	}
	else
	{
		StdStringFormat( formatedErr, "#App Error %d", eAppErr );
	}

	emit signalLog( LOG_ERROR, formatedErr.c_str() );
	emit signalStatusMsg( formatedErr.c_str() );
}

//============================================================================
void AppCommon::toGuiStatusMessage( const char* statusMsg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	if( statusMsg )
	{
		emit signalLog( LOG_INFO, statusMsg );
		emit signalStatusMsg( statusMsg );
	}
}

//============================================================================
// NOTE: toGuiUserMessage should be called from in gui on gui thread only
void AppCommon::toGuiUserMessage( const char* userMsg, ... )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}
	
	const int MAX_USER_MSG_SIZE = 2048;
	std::array<char, MAX_USER_MSG_SIZE> szBuffer;
	szBuffer[0] = 0;
	va_list arg_ptr;
	va_start(arg_ptr, userMsg);
#ifdef TARGET_OS_WINDOWS
	vsnprintf(szBuffer.data(), MAX_USER_MSG_SIZE, userMsg, (char*)arg_ptr);
#else
    vsnprintf(szBuffer.data(), MAX_USER_MSG_SIZE, userMsg, arg_ptr);
#endif //  TARGET_OS_WINDOWS
	szBuffer.data()[MAX_USER_MSG_SIZE - 1] = 0;
	va_end(arg_ptr);

	if( 0 != szBuffer[0] )
	{
		emit signalStatusMsg( szBuffer.data() );
	}
}

//============================================================================
void AppCommon::toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, std::string& paramMsg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	QString paramValue = paramMsg.empty() ? "" : paramMsg.c_str();
	emit signalInternalPluginMessage( pluginType, onlineId, msgType, paramValue );
}

//============================================================================
void AppCommon::toGuiPluginCommError( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, ECommErr commErr )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalPluginErrorMsg( pluginType, onlineId, msgType, commErr );
}

//============================================================================
void AppCommon::slotInternalPluginMessage( EPluginType pluginType, VxGUID onlineId, EPluginMsgType msgType, QString paramValue )
{
	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiPluginMsg( pluginType, onlineId, msgType, paramValue );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::slotInternalPluginErrorMsg( EPluginType pluginType, VxGUID onlineId, EPluginMsgType msgType, ECommErr commError )
{
	QString commErrDescription = GuiParams::describeCommError( commError );
	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiPluginMsg( pluginType, onlineId, msgType, commErrDescription );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
/// Send Network available status to GUI for display
void AppCommon::toGuiNetAvailableStatus( ENetAvailStatus netAvailStatus )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    emit signalInternalNetAvailStatus( netAvailStatus );
}

//============================================================================
void AppCommon::slotInternalNetAvailStatus( ENetAvailStatus netAvailStatus )
{
	emit signalNetAvailStatus( netAvailStatus );
	if( GuiParams::isNetStatusPtoPReady( netAvailStatus ) )
	{
		if( !m_PtopNetworkReady )
		{
			m_PtopNetworkReady = true;
			checkReadyToLaunchAfterLogonApplets();
		}
	}
}

//============================================================================
void AppCommon::toGuiNetworkState( ENetworkStateType eNetworkState, const char* stateMsg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_LastNetworkState = eNetworkState;
	if( eNetworkStateTypeOnlineDirect == eNetworkState )
	{
		emit signalSetRelayHelpButtonVisibility( false );
	}
	else if( eNetworkStateTypeOnlineThroughRelay == eNetworkState )
	{
		emit signalSetRelayHelpButtonVisibility( true );
	}

	emit signalNetworkStateChanged( eNetworkState );
}

//============================================================================
void AppCommon::slotNetworkStateChanged( ENetworkStateType eNetworkState )
{
	switch( eNetworkState )
	{
	case eNetworkStateTypeUnknown:
	case eNetworkStateTypeLost:
	case eNetworkStateTypeNoInternetConnection:
		//ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateDisconnected ) );
		//ui.searchButton->setEnabled( false );
		//ui.searchButton->setIcon( getMyIcons().getIcon( eMyIconSearchDisabled ) );
		break;

	case eNetworkStateTypeAvail:
	case eNetworkStateTypeTestConnection:
	case eNetworkStateTypeWaitForRelay:
		//ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateDiscover ) );
		break;

	case eNetworkStateTypeOnlineDirect:
		//ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateDirectConnected ) );
		//ui.searchButton->setEnabled( true );
		//ui.searchButton->setIcon( getMyIcons().getIcon( eMyIconSearch ) );
		break;

	case eNetworkStateTypeOnlineThroughRelay:
		//ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateRelayConnected ) );
		//ui.searchButton->setEnabled( true );
		//ui.searchButton->setIcon( getMyIcons().getIcon( eMyIconSearch ) );
		break;

	case eNetworkStateTypeFailedResolveHostNetwork:
		//ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateHostFail ) );
		break;
    case eNetworkStateTypeFailedResolveHostGroup:
        //ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateHostFail ) );
        break;
    case eNetworkStateTypeFailedResolveHostGroupList:
        //ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateHostFail ) );
        break;
    case eMaxNetworkStateType:
        //ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateHostFail ) );
        break;
    case eNetworkStateTypeIpChange:
        //ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNetworkStateHostFail ) );
        break;
	}
}

//============================================================================
void AppCommon::toGuiHostAnnounceStatus( EHostType hostType, VxGUID& sessionId, EHostAnnounceStatus annStatus, const char* msg )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

	if( eHostAnnounceHandshaking == annStatus )
	{
		// ignore
		return;
	}

    const char* hostStatus = DescribeHostAnnounceStatus( annStatus );
	const char* hostTypeStr = DescribeHostType( hostType );
    std::string formatedMsg;
    if( msg )
    {
        StdStringFormat( formatedMsg, "#%s %s %s", hostTypeStr, hostStatus, msg );
    }
    else
    {
        StdStringFormat( formatedMsg, "#%s %s", hostTypeStr, hostStatus );
    }

    emit signalLog( 0, formatedMsg.c_str() );
    emit signalStatusMsg( formatedMsg.c_str() );

    emit signalHostAnnounceStatus( hostType, sessionId, annStatus, formatedMsg.c_str() );
}

//============================================================================
void AppCommon::toGuiHostJoinStatus( EHostType hostType, VxGUID& sessionId, EHostJoinStatus joinStatus, const char* msg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	const char* hostStatus = DescribeHostJoinStatus( joinStatus );
	std::string formatedMsg;
	if( msg )
	{
		StdStringFormat( formatedMsg, "#%s %s", hostStatus, msg );
	}
	else
	{
		StdStringFormat( formatedMsg, "#%s\n", hostStatus );
	}

	emit signalLog( 0, formatedMsg.c_str() );
	emit signalStatusMsg( formatedMsg.c_str() );

	emit signalHostJoinStatus( hostType, sessionId, joinStatus, formatedMsg.c_str() );
}

//============================================================================
void AppCommon::toGuiHostSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr, const char* msg )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    QString hostStatus = GuiParams::describeHostSearchStatus( searchStatus );
    if( eCommErrNone != commErr )
    {
        hostStatus += GuiParams::describeCommError( commErr );
    }

    if( msg )
    {
        hostStatus += msg;
    }

    emit signalLog( 0, hostStatus );
    emit signalStatusMsg( hostStatus );

    emit signalHostSearchStatus( hostType, sessionId, searchStatus, hostStatus );
}

//============================================================================
void AppCommon::toGuiHostSearchResult( EHostType hostType, VxGUID& sessionId, HostedInfo& hostedInfo )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

	getHostedListMgr().toGuiHostSearchResult( hostType, sessionId, hostedInfo );
}

//============================================================================
void AppCommon::toGuiHostSearchComplete( EHostType hostType, VxGUID& sessionId )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	getHostedListMgr().toGuiHostSearchComplete( hostType, sessionId );
}

//============================================================================
void AppCommon::toGuiGroupieSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr, const char* msg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	QString hostStatus = GuiParams::describeHostSearchStatus( searchStatus );
	if( eCommErrNone != commErr )
	{
		hostStatus += GuiParams::describeCommError( commErr );
	}

	if( msg )
	{
		hostStatus += msg;
	}

	emit signalLog( 0, hostStatus );
	emit signalStatusMsg( hostStatus );

	emit signalGroupieSearchStatus( hostType, sessionId, searchStatus, hostStatus );
}

//============================================================================
void AppCommon::toGuiGroupieSearchResult( EHostType hostType, VxGUID& sessionId, GroupieInfo& groupieInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	getGroupieListMgr().toGuiGroupieSearchResult( hostType, sessionId, groupieInfo );
}

//============================================================================
void AppCommon::toGuiGroupieSearchComplete( EHostType hostType, VxGUID& sessionId )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	getGroupieListMgr().toGuiGroupieSearchComplete( hostType, sessionId );
}

//============================================================================
void AppCommon::toGuiIsPortOpenStatus( EIsPortOpenStatus eIsPortOpenStatus, const char* msg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	const char* portOpenStatus = DescribePortOpenStatus( eIsPortOpenStatus );
	std::string formatedMsg;
	if( msg )
	{
		StdStringFormat( formatedMsg, "#%s %s", portOpenStatus, msg );
	}
	else
	{
		StdStringFormat( formatedMsg, "#%s", portOpenStatus );
	}

	emit signalLog( 0, formatedMsg.c_str() );
	emit signalStatusMsg( formatedMsg.c_str() );

	emit signalIsPortOpenStatus( eIsPortOpenStatus, formatedMsg.c_str() );
}

//============================================================================
void AppCommon::toGuiRunTestStatus( const char*testName, ERunTestStatus eRunTestStatus, const char* msg )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    const char* runTestStatus = DescribeRunTestStatus( eRunTestStatus );
    std::string formatedMsg;
    if( msg )
    {
        StdStringFormat( formatedMsg, "#%s %s", runTestStatus, msg );
    }
    else
    {
        StdStringFormat( formatedMsg, "#%s", runTestStatus );
    }

    emit signalLog( 0, formatedMsg.c_str() );
    emit signalStatusMsg( formatedMsg.c_str() );

    emit signalRunTestStatus( testName,  eRunTestStatus, formatedMsg.c_str() );
}

//============================================================================
void AppCommon::toGuiRandomConnectStatus( ERandomConnectStatus eRandomConnectStatus, const char* msg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	const char* phoneShakeStatus = DescribeRandomConnectStatus( eRandomConnectStatus );
	std::string formatedMsg;
	if( msg )
	{
		StdStringFormat( formatedMsg, "#%s", msg );
	}
	else
	{
		StdStringFormat( formatedMsg, "#%s", phoneShakeStatus );
	}

	emit signalLog( 0, formatedMsg.c_str() );
	emit signalStatusMsg( formatedMsg.c_str() );

	emit signalRandomConnectStatus( eRandomConnectStatus, formatedMsg.c_str() );
}

//============================================================================
	// prompt user to confirm wants to shutdown app.. caller must call appCommonShutdown if answer is yes
bool AppCommon::confirmAppShutdown( QWidget* parentWindow )
{
	if( m_bUserCanceledCreateProfile )
	{
		// we are shutting down because user canceled creating profile
		return true;
	}
		    
    // Setting parent will keep message box in the center of QMainWindow

    static bool confirmedExit = false;
    if( confirmedExit )
    {
        return true;
    }

	// to get here we must be on the home page
	QString title = QObject::tr( "Close Application" );
	QString body = QObject::tr( "Are you sure you want to exit?" );

	ActivityYesNoMsgBox confirmExit( *this, parentWindow, title, body );
	if( QDialog::Accepted == confirmExit.exec() )
	{
        confirmedExit = true;
        return true;
	}

	return false;
}

//============================================================================
bool AppCommon::userCanceled( void )
{
	return m_bUserCanceledCreateProfile;
}

//============================================================================
void AppCommon::onOncePerSecond( void )
{
	static int64_t startTime = 0;
	if( getGuiCpuTimeEnable() )
	{
		startTime = GetHighResolutionTimeMs();
	}

    // the wait is probably no longer needed since not running seperate threads for loading
    // but it does give other things a chance to run a bit
    static int waitCnt = 0;
    if( waitCnt > 1 )
    {
        if( !m_LoginBegin )
        {
            m_LoginBegin = true;
            doLogin();
        }

        if( getLoginCompleted() )
        {
            getEngine().fromGuiOncePerSecond();
        }
    }
    else
    {
        waitCnt++;
        LogMsg( LOG_VERBOSE, "Wait to login seconds %d alive ms %" PRId64 "", waitCnt, GetApplicationAliveMs() );
    }

	static int64_t endTime = 0;
	if( getGuiCpuTimeEnable() )
	{
		endTime = GetHighResolutionTimeMs();
		int elapsedTime = (int)(endTime - startTime);
		if( elapsedTime > 2 )
		{
			LogMsg( LOG_VERBOSE, "AppCommon::onOncePerSecond %d ms in function", elapsedTime );
		}
	}
}

//============================================================================
//! time to flash the notify icon
void AppCommon::slotOnNotifyIconFlashTimeout( bool bWhite )
{
	if( bWhite )
	{
		////ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNotifyStarWhite ) );	
	}
	else
	{
		////ui.notifyButton->setIcon( getMyIcons().getIcon( eMyIconNotifyStarGreen ) );	
	}
}

//============================================================================
void AppCommon::slotStatusMsg( QString strMsg )
{
	LogMsg( LOG_INFO, strMsg.toStdString().c_str() );
	//ui.StatusMsgLabel->setText( strMsg );
	if(strMsg.isEmpty())
	{
		//ui.StatusMsgLabel->setVisible( false );
	}
	else
	{
		//ui.StatusMsgLabel->setVisible( true );
	}
}

//============================================================================
void AppCommon::slotAppErr( EAppErr eAppErr, QString errMsg )
{
	//errMsg.remove(QRegExp("[\\n\\r]"));
	QMessageBox::warning(this, QObject::tr("Application Error"), errMsg );
}

//============================================================================
void AppCommon::toGuiSetGameValueVar( EPluginType pluginType, VxGUID& onlineId, int32_t s32VarId, int32_t s32VarValue )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiSetGameValueVar( pluginType, onlineId, s32VarId, s32VarValue );
}

//============================================================================
void AppCommon::slotInternalToGuiSetGameValueVar( EPluginType pluginType, VxGUID onlineId, int32_t s32VarId, int32_t s32VarValue )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiSetGameValueVar( pluginType, onlineId, s32VarId, s32VarValue );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiSetGameActionVar(	EPluginType	pluginType, 
										VxGUID&		onlineId, 
										int32_t		s32VarId, 
										int32_t		s32VarValue )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiSetGameActionVar( pluginType, onlineId, s32VarId, s32VarValue );
}

//============================================================================
void AppCommon::slotInternalToGuiSetGameActionVar( EPluginType pluginType, VxGUID onlineId, int32_t s32VarId, int32_t s32VarValue )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiSetGameActionVar( pluginType, onlineId, s32VarId, s32VarValue );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::slotRelayHelpButtonClicked( void )
{
	ActivityShowHelp * helpDlg = new ActivityShowHelp( *this, this );
	helpDlg->show();
}

//============================================================================
void AppCommon::slotSetRelayHelpButtonVisibility( bool isVisible )
{
	//ui.m_RelayHelpButton->setVisible( isVisible );
}

//============================================================================
void AppCommon::toGuiAssetAdded( AssetBaseInfo* assetInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiAssetAdded( *assetInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiAssetAdded( AssetBaseInfo assetInfo )
{
	// when assets are added they might call wantToGuiActivityCallbacks and change the clientList
	// make sure que is empty before attempting to use the activeity interfaces
	ProcessQtEvents( 20 );

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiAssetAdded( assetInfo );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiAssetUpdated( AssetBaseInfo* assetInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiAssetUpdated( *assetInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiAssetUpdated( AssetBaseInfo assetInfo )
{
	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiAssetUpdated( assetInfo );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiAssetRemoved( AssetBaseInfo* assetInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiAssetRemoved( *assetInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiAssetRemoved( AssetBaseInfo assetInfo )
{
	// when assets are removed they might call wantToGuiActivityCallbacks and change the clientList
// check if client is in list each time to avoid out of range vector interation crash

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiAssetUpdated( assetInfo );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiAssetXferState( VxGUID& assetUniqueId, EAssetSendState assetSendState, int param )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiAssetXferState( assetUniqueId, assetSendState, param );
}

//============================================================================
void AppCommon::slotInternalToGuiAssetXferState( VxGUID assetUniqueId, EAssetSendState assetSendState, int param )
{
	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiAssetXferState( assetUniqueId, assetSendState, param );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiAssetSessionHistory( AssetBaseInfo* assetInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	// use pointers because there may be a lot of history objects and would otherwise fill the queued signals/slots
	AssetBaseInfo* newAssetInfo = new AssetBaseInfo( *assetInfo );
	emit signalInternalToGuiAssetSessionHistory( newAssetInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiAssetSessionHistory( AssetBaseInfo* assetInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiAssetSessionHistory( *assetInfo );
	}

	m_ToGuiActivityInterfaceBusy = false;
	delete assetInfo;
}

//============================================================================
void AppCommon::toGuiAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiAssetAction( assetAction, assetId, pos0to100000 );
}

//============================================================================
void AppCommon::slotInternalToGuiAssetAction( EAssetAction assetAction, VxGUID assetId, int pos0to100000 )
{
    if( ( eAssetActionRxNotifyNewMsg == assetAction )
        || ( eAssetActionRxViewingMsg == assetAction ) )
    {
        emit signalAssetViewMsgAction( assetAction, assetId, pos0to100000 );
        return;
    }

	m_ToGuiActivityInterfaceBusy = true;
    for( auto client : m_ToGuiActivityInterfaceList )
    {
        client->toGuiClientAssetAction( assetAction, assetId, pos0to100000 );
    }

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID onlineId, int pos0to100000 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalMultiSessionAction( onlineId, mSessionAction, pos0to100000 );
}

//============================================================================
void AppCommon::slotInternalMultiSessionAction( VxGUID onlineId, EMSessionAction mSessionAction, int pos0to100000 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
    for( auto client : m_ToGuiActivityInterfaceList )
    {
        client->toGuiMultiSessionAction(  mSessionAction, onlineId, pos0to100000 );
    }

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiBlobAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalBlobAction( assetAction, assetId, pos0to100000 );
}

//============================================================================
void AppCommon::slotInternalBlobAction( EAssetAction assetAction, VxGUID assetId, int pos0to100000 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	if( ( eAssetActionRxNotifyNewMsg == assetAction )
		|| ( eAssetActionRxViewingMsg == assetAction ) )
	{
		emit signalBlobViewMsgAction( assetAction, assetId, pos0to100000 );
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiClientBlobAction( assetAction, assetId, pos0to100000 );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiBlobAdded( BlobInfo* blobInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalBlobAdded( *blobInfo );
}

//============================================================================
void AppCommon::slotInternalBlobAdded( BlobInfo blobInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
    for( auto client : m_ToGuiActivityInterfaceList )
    {
        client->toGuiBlobAdded( blobInfo );
    }

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiBlobSessionHistory( BlobInfo* blobInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalBlobSessionHistory( *blobInfo );
}


//============================================================================
void AppCommon::slotInternalBlobSessionHistory( BlobInfo blobInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_ToGuiActivityInterfaceBusy = true;
    for( auto client : m_ToGuiActivityInterfaceList )
    {
        client->toGuiBlobSessionHistory( blobInfo );
    }

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiNetworkIsTested( bool requiresRelay, std::string& ipAddr, uint16_t ipPort )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalNetworkIsTested( requiresRelay, ipAddr.c_str(), ipPort );
}

//============================================================================
void AppCommon::slotInternalNetworkIsTested( bool requiresRelay, QString ipAddr, uint16_t ipPort )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	static bool firstTime = true;
	if( firstTime )
	{
		firstTime = false;
		if( m_AppSettings.getRunOnStartupCamServer() && eFriendStateIgnore != getMyNetIdent()->getPluginPermission( ePluginTypeCamServer ) )
		{
			getEngine().fromGuiStartPluginSession( ePluginTypeCamServer, getMyNetIdent()->getMyOnlineId(), 0, getMyNetIdent()->getMyOnlineId() );
		}
	}
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
void AppCommon::wantToGuiActivityCallbacks(	ToGuiActivityInterface *	callback, 
											bool						wantCallback )
{
static bool actCallbackShutdownComplete = false;
	if( VxIsAppShuttingDown() )
	{
		if( actCallbackShutdownComplete )
		{
			return;
		}

		actCallbackShutdownComplete = true;
		clearToGuiActivityInterfaceList();
		return;
	}

	if( m_ToGuiActivityInterfaceBusy )
	{
		LogMsg( LOG_ERROR, "AppCommon::wantToGuiActivityCallbacks do NOT call while busy" );
		vx_assert( false );
	}

	if( wantCallback )
	{
		for( auto client : m_ToGuiActivityInterfaceList )
		{
			if( client == callback )
			{
				LogMsg( LOG_INFO, "WARNING. Ignoring New ToGuiActivityInterface.h because already in list" );
				return;
			}
		}

		m_ToGuiActivityInterfaceList.push_back( callback );
		return;
	}

	for( auto iter = m_ToGuiActivityInterfaceList.begin(); iter != m_ToGuiActivityInterfaceList.end(); ++iter )
	{
		ToGuiActivityInterface* client = *iter;
		if( client == callback )
		{
			m_ToGuiActivityInterfaceList.erase( iter );
			return;
		}
	}

	LogMsg( LOG_INFO, "WARNING. ToGuiActivityInterface.h remove not found in list" );
	return;
}

//============================================================================
void AppCommon::clearToGuiActivityInterfaceList( void )
{
	if( m_ToGuiActivityInterfaceList.size() )
	{
		m_ToGuiActivityInterfaceList.clear();
	}
}

//============================================================================
void AppCommon::wantToGuiHardwareCtrlCallbacks(	ToGuiHardwareControlInterface *	callback, 
												bool							wantCallback )
{
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
}

//============================================================================
void AppCommon::clearHardwareCtrlList( void )
{
	if( m_ToGuiHardwareCtrlList.size() )
	{
		m_ToGuiHardwareCtrlList.clear();
	}
}

//============================================================================
void AppCommon::wantToGuiUserUpdateCallbacks( ToGuiUserUpdateInterface * callback, bool wantCallback )
{
	if( !callback || (ToGuiUserUpdateInterface *)0xFFFFFFFFFFFFFFFF == callback )
	{
		LogMsg( LOG_ERROR, "AppCommon::wantToGuiUserUpdateCallbacks invalid param" );
		vx_assert( false );
		return;
	}

    static bool userCallbackShutdownComplete = false;
    if( VxIsAppShuttingDown() )
    {
        if( userCallbackShutdownComplete )
        {
            return;
        }

        userCallbackShutdownComplete = true;
        clearUserUpdateClientList();
        return;
    }

	if( m_ToGuiUserUpdateClientBusy )
	{
		LogMsg( LOG_INFO, "AppCommon::wantToGuiUserUpdateCallbacks do NOT call while busy" );
	}

    if( wantCallback )
    {
        for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
        {
            ToGuiUserUpdateInterface* client = *iter;
            if( client == callback )
            {
                LogMsg( LOG_INFO, "WARNING. Ignoring New wantToGuiUserUpdateCallbacks because already in list" );
                return;
            }
        }

        m_ToGuiUserUpdateClientList.push_back( callback );
        return;
    }

    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        if( client == callback )
        {
            m_ToGuiUserUpdateClientList.erase( iter );
            return;
        }
    }

    LogMsg( LOG_INFO, "WARNING. ToGuiUserUpdateInterface remove not found in list" );
    return;
}

//============================================================================
void AppCommon::clearUserUpdateClientList( void )
{
    if( m_ToGuiUserUpdateClientList.size() )
    {
        m_ToGuiUserUpdateClientList.clear();
    }
}

//============================================================================
void AppCommon::refreshFriend( VxGUID& onlineId )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	VxGUID friendId( onlineId.getVxGUIDHiPart(), onlineId.getVxGUIDLoPart() );
	emit signalRefreshFriend( friendId );
}

//============================================================================
void AppCommon::onMessengerReady( bool isReady )
{
	if( isReady != m_IsMessengerReady )
	{
		m_IsMessengerReady = isReady;
		m_ThumbMgr.onMessengerReady( isReady );
		m_UserMgr.onMessengerReady( isReady );
		m_OfferMgr.onMessengerReady( isReady );
		m_HostJoinMgr.onMessengerReady( isReady );
		m_UserJoinMgr.onMessengerReady( isReady );
		m_WebPageMgr.onMessengerReady( isReady );
		m_ConnectIdListMgr.onMessengerReady( isReady );
		checkSystemReady();
	}
}

//============================================================================
void AppCommon::onUserLoggedOn( void )
{
	if( !m_IsLoggedOn )
	{
		m_IsLoggedOn = true;
		checkSystemReady();
	}
}

//============================================================================
bool AppCommon::checkSystemReady( void )
{
	if( !m_IsGuiSystemReady && m_IsMessengerReady && m_IsLoggedOn )
	{
		// one time only each application and user ready at startup
		m_IsGuiSystemReady = true;
		m_PushToTalkMgr.onSystemReady();

		m_CamLogic.camLogicStartup();
		
		m_ThumbMgr.onSystemReady( m_IsGuiSystemReady );
		m_UserMgr.onSystemReady( m_IsGuiSystemReady );
		m_OfferMgr.onSystemReady( m_IsGuiSystemReady );
		m_HostJoinMgr.onSystemReady( m_IsGuiSystemReady );
		m_UserJoinMgr.onSystemReady( m_IsGuiSystemReady );
		m_WebPageMgr.onSystemReady( m_IsGuiSystemReady );
		m_ConnectIdListMgr.onSystemReady( m_IsGuiSystemReady );

		emit signalSystemReady( m_IsGuiSystemReady );

		checkReadyToLaunchAfterLogonApplets();
	}

	return m_IsGuiSystemReady;
}

//============================================================================
std::string AppCommon::describeHostedId( HostedId& hostedId )
{
	std::string adminDesc = DescribeHostType( hostedId.getHostType() );
	adminDesc += " admin ";
	adminDesc += describeUser( hostedId.getHostOnlineId() );
	return adminDesc;
}

//============================================================================
std::string AppCommon::describeGroupieId( GroupieId& groupieId )
{
	std::string desc = DescribeHostType( groupieId.getHostType() );
	desc += describeUser( groupieId.getHostOnlineId() );
	desc += " user ";
	desc += describeUser( groupieId.getUserOnlineId() );
	return desc;
}

//============================================================================
std::string AppCommon::describeUser( VxGUID& onlineId )
{
	std::string userDesc = m_UserMgr.getUserOnlineName( onlineId );
	userDesc += " id ";
	userDesc += onlineId.toOnlineIdString();
	return userDesc;
}

//============================================================================
std::string AppCommon::describeUser( GuiUser* guiUser )
{
	if( guiUser )
	{
		std::string userDesc = guiUser->getOnlineName();
		userDesc += " id ";
		userDesc += guiUser->getMyOnlineId().toOnlineIdString();
		return userDesc;
	}
	else
	{
		return "AppCommon::describeUser null user";
	}
}


//============================================================================
bool AppCommon::hasExistingAccount( void )
{
	if( !getAccountMgr().getLastLogin().empty() )
	{
		return true;
	}

	return false;
}

//============================================================================
void AppCommon::checkIsGuiThread( void )
{
	if( !VxIsGuiThreadId() )
	{
		vx_assert( false );
		LogMsg( LOG_FATAL, "AppCommon::checkIsGuiThread failed" );
		exit(99);
	}
}
