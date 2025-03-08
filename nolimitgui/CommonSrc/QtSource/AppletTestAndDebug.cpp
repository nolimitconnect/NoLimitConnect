//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletTestAndDebug.h"

#include "AppCommon.h"
#include "AppletBrowseFiles.h"
#include "AppletChooseUser.h"
#include "AppletPopupMenu.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiParams.h"
#include "WaitingSpinnerWidget.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxUrl.h>

#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QScrollBar>
#include <QTimer>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QDesktopWidget>
#endif // QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include "ui_AppletTestAndDebug.h"

namespace
{
    const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit *            AppletTestAndDebug::getInfoEdit( void )     { return ui.m_InfoPlainTextEdit; }

//============================================================================
AppletTestAndDebug::AppletTestAndDebug( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_TEST_AND_DEBUG, app, parent )
, ui(*(new Ui::AppletTestAndDebugUi))
{
    setAppletType( eAppletTestAndDebug );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    getInfoEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
    getInfoEdit()->setReadOnly( true );

    VxNetIdent oMyIdent;
    m_FromGui.fromGuiQueryMyIdent( &oMyIdent );

    ui.m_MyUrlEdit->setText( QString( oMyIdent.getMyOnlineUrl().c_str() ) );

    fillCpuInfo();
    std::string strValue;
    m_MyApp.getAppSettings().getLastUsedTestUrl( strValue );
    if( !strValue.empty() )
    {
        ui.m_TestUrlEdit->setText( strValue.c_str() );
        ui.m_TestUrlsComboBox->addItem( strValue.c_str() );
    }

    m_Engine.getEngineSettings().getNetworkHostUrl( strValue );
    if( !strValue.empty() )
    {
        ui.m_TestUrlsComboBox->addItem( strValue.c_str() );
    }

    m_Engine.getEngineSettings().getConnectTestUrl( strValue );
    if( !strValue.empty() )
    {
        ui.m_TestUrlsComboBox->addItem( strValue.c_str() );
    }

    m_Engine.getEngineSettings().getRandomConnectUrl( strValue );
    if( !strValue.empty() )
    {
        ui.m_TestUrlsComboBox->addItem( strValue.c_str() );
    }

    m_Engine.getEngineSettings().getGroupHostUrl( strValue );
    if( !strValue.empty() )
    {
        ui.m_TestUrlsComboBox->addItem( strValue.c_str() );
    }

    m_Engine.getEngineSettings().getChatRoomHostUrl( strValue );
    if( !strValue.empty() )
    {
        ui.m_TestUrlsComboBox->addItem( strValue.c_str() );
    }

    if( !ui.m_TestUrlsComboBox->currentText().isEmpty() )
    {
        if( ui.m_TestUrlEdit->text().isEmpty() )
        {
            ui.m_TestUrlEdit->setText( ui.m_TestUrlsComboBox->currentText() );
        }
    }

    connect( ui.m_ShowLogButton, SIGNAL(clicked()), this, SLOT(slotShowLogButtonClick()) );
    connect( ui.m_ShowAppInfoButton, SIGNAL(clicked()), this, SLOT(slotShowAppInfoButtonClick()) );

    connect( ui.m_CopyMyUrlButton, SIGNAL(clicked()), this, SLOT(slotCopyMyUrlToClipboardClicked()) );

    connect( ui.m_CopyTestUrlButton, SIGNAL(clicked()), this, SLOT(slotCopyTestUrlToClipboardClicked()) );
    connect( ui.m_CopyResultToClipboardButton, SIGNAL(clicked()), this, SLOT(slotCopyResultToClipboardClicked()) );
    connect( ui.m_ClearResultsButton, SIGNAL(clicked()), this, SLOT(slotClearResultsButtonClicked()) );

    connect( ui.m_BrowseFilesButton, SIGNAL(clicked()), this, SLOT(slotBrowseFilesButtonClicked()) );

    connect( ui.m_GenerateGuidButton, SIGNAL(clicked()), this, SLOT(slotGenerateGuidButtonClicked()) ); 
    connect( ui.m_GenKeyButton, SIGNAL(clicked()), this, SLOT(slotGenKeyButtonClicked()) );

    connect( ui.m_PurgeCacheButton, SIGNAL(clicked()), this, SLOT(slotPurgeCacheButtonClicked()) );
    connect( ui.m_PurgeEverythingButton, SIGNAL(clicked()), this, SLOT(slotPurgeEverythingButtonClicked()) );

    connect( ui.m_BrowseDownloadsButton, SIGNAL(clicked()), this, SLOT(slotBrowseDownloadsButtonClicked()) );
    connect( ui.m_BrowseAppDataButton, SIGNAL(clicked()), this, SLOT(slotBrowseBrowseAppDataButtonClicked()) );

    connect( ui.m_DeleteDbButton, SIGNAL(clicked()), this, SLOT(slotDeleteDbButtonClicked()) );

    connect( this, SIGNAL(signalLogMsg(const QString&)), this, SLOT(slotInfoMsg(const QString&)) );
    connect( this, SIGNAL(signalInfoMsg(const QString&)), this, SLOT(slotInfoMsg(const QString&)) );

    connect( ui.m_TestUrlsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotNewUrlSelected(int)) );

    updateDlgFromSettings();

    // Log is seperate now VxAddLogHandler( this );
    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletTestAndDebug::~AppletTestAndDebug()
{
    // Log is seperate now VxRemoveLogHandler( this );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletTestAndDebug::updateDlgFromSettings()
{
    std::string lastTestUrl;
    m_MyApp.getAppSettings().getLastUsedTestUrl( lastTestUrl );
    if( !lastTestUrl.empty() )
    {
        ui.m_TestUrlEdit->setText( lastTestUrl.c_str() );
    }
}

//============================================================================
void AppletTestAndDebug::updateSettingsFromDlg()
{
    if( !ui.m_TestUrlEdit->text().isEmpty() )
    {
        std::string testUrl( ui.m_TestUrlEdit->text().toUtf8().constData() );
        m_MyApp.getAppSettings().setLastUsedTestUrl( testUrl );
    }
}

//============================================================================
void AppletTestAndDebug::slotNewUrlSelected( int comboBoxIdx )
{
    QString urlText = ui.m_TestUrlsComboBox->currentText();
    if( !urlText.isEmpty() )
    {
        ui.m_TestUrlEdit->setText( urlText );
    }
}

//============================================================================
void AppletTestAndDebug::slotPingTestButtonClicked( void )
{
    startUrlTest( eNetCmdHostPing );
}

//============================================================================
void AppletTestAndDebug::slotHackerViewButtonClicked( void )
{
    getInfoEdit()->clear();
    m_MyApp.launchApplet( eAppletHackerList, getParentPageFrame() );
}

//============================================================================
void AppletTestAndDebug::slotSktStatViewButtonClicked( void )
{
    getInfoEdit()->clear();
    m_MyApp.launchApplet( eAppletSocketList, getParentPageFrame() );
}

//============================================================================
void AppletTestAndDebug::startUrlTest( ENetCmdType netCmdType )
{
    getInfoEdit()->clear();
    VxUrl myUrl( ui.m_MyUrlEdit->text().toUtf8().constData() );
    VxUrl testUrl( ui.m_TestUrlEdit->text().toUtf8().constData() );

    if( myUrl.validateUrl( true ) && testUrl.validateUrl( false ) )
    {
        infoMsg( "Testing %s", DescribeNetCmdType( netCmdType ) );
        VxGUID::generateNewVxGUID(m_SessionId);
        m_MyApp.getEngine().fromGuiRunUrlAction( m_SessionId, myUrl.getUrl().c_str(), testUrl.getUrl().c_str(), netCmdType );
    }
    else
    {
        if( !myUrl.validateUrl( true ) )
        {
            infoMsg( "Invalid My URL" );
        }

        if( !testUrl.validateUrl( false ) )
        {
            infoMsg( "Invalid Test URL" );
        }
    }
}

//============================================================================
void AppletTestAndDebug::slotBrowseFilesButtonClicked( void )
{
    m_MyApp.launchApplet( eAppletBrowseFiles, getContentFrameOfOppositePageFrame() );
}

//============================================================================
void AppletTestAndDebug::slotBrowseDownloadsButtonClicked( void )
{
    AppletBrowseFiles* applet = (AppletBrowseFiles*)m_MyApp.launchApplet( eAppletBrowseFiles, getContentFrameOfOppositePageFrame() );
    if( applet )
    {
        std::string downDir = VxGetAppDirectory( eAppDirDownloads );
        if( !downDir.empty() )
        {
            applet->setCurrentDirectory( downDir.c_str() );
        }
    }
}

//============================================================================
void AppletTestAndDebug::slotBrowseBrowseAppDataButtonClicked( void )
{
    AppletBrowseFiles* applet = (AppletBrowseFiles *)m_MyApp.launchApplet( eAppletBrowseFiles, getContentFrameOfOppositePageFrame() );
    if( applet )
    {
        std::string downDir = VxGetAppDirectory( eAppDirAppNoLimitData );
        if( !downDir.empty() )
        {
            applet->setCurrentDirectory( downDir.c_str() );
        }
    }
}

//============================================================================
void AppletTestAndDebug::onLogEvent( uint32_t u32LogFlags, const char* logMsg )
{
    // Log is seperate now 

    //m_LogMutex.lock();
    //if( m_VerboseLog
    //    || ( u32LogFlags & ~LOG_VERBOSE ) )
    //{
    //    QString logStr( logMsg );
    //    logStr.remove( QRegExp( "[\\n\\r]" ) );
    //    emit signalLogMsg( logStr );
    //}

    //m_LogMutex.unlock();
}

//============================================================================
void AppletTestAndDebug::toGuiInfoMsg( char * infoMsg )
{
    m_LogMutex.lock();

    QString infoStr( infoMsg );
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
    infoStr.remove(QRegularExpression("[\\n\\r]"));
#else
    infoStr.remove(QRegExp("[\\n\\r]"));
#endif // QT_VERSION > QT_VERSION_CHECK(6,0,0)
    emit signalInfoMsg( infoStr );

    m_LogMutex.unlock();
}

//============================================================================
void AppletTestAndDebug::slotLogMsg( const QString& text )
{
    //getLogEdit()->appendPlainText( text ); // Adds the message to the widget
    //getLogEdit()->verticalScrollBar()->setValue( getLogEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
    //m_LogFile.write( text ); // Logs to file
}

//============================================================================
void AppletTestAndDebug::slotInfoMsg( const QString& text )
{
    getInfoEdit()->appendPlainText( text ); // Adds the message to the widget
    getInfoEdit()->verticalScrollBar()->setValue( getInfoEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
    //m_LogFile.write( text ); // Logs to file
}

//============================================================================
void AppletTestAndDebug::slotShowLogButtonClick( void )
{
    fillBasicInfo();
    fillExtraInfo();
    fillCpuInfo();

    m_MyApp.launchApplet( eAppletLog, getContentFrameOfOppositePageFrame() );
}

//============================================================================
void AppletTestAndDebug::slotShowAppInfoButtonClick( void )
{
    fillBasicInfo();
    fillExtraInfo();
    fillCpuInfo();

    m_MyApp.launchApplet( eAppletApplicationInfo, getContentFrameOfOppositePageFrame() );
}

//============================================================================
void AppletTestAndDebug::slotCopyMyUrlToClipboardClicked( void )
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( ui.m_MyUrlEdit->text() );
}

//============================================================================
void AppletTestAndDebug::slotCopyTestUrlToClipboardClicked( void )
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( ui.m_TestUrlEdit->text() );
}

//============================================================================
void AppletTestAndDebug::slotCopyResultToClipboardClicked( void )
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( getInfoEdit()->toPlainText() );
}

//============================================================================
void AppletTestAndDebug::slotClearResultsButtonClicked( void )
{
    getInfoEdit()->setPlainText( "" );
}

//============================================================================
void AppletTestAndDebug::infoMsg( const char* errMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, errMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), errMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );

    toGuiInfoMsg( as8Buf );
}

//============================================================================
void AppletTestAndDebug::logMsg( const char* logMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, logMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), logMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );

    onLogEvent( LOG_INFO, as8Buf );
}

//============================================================================
void AppletTestAndDebug::fillBasicInfo( void )
{
    infoMsg( "website: %s", VxGetCompanyWebsite() );
    infoMsg( "app: %s version %s", VxGetApplicationTitle(), VxGetAppVersionString() );
    infoMsg( "disk space available: %s", GuiParams::describeFileLength( m_MyApp.getFromGuiInterface().fromGuiGetDiskFreeSpace() ).toUtf8().constData() );

    infoMsg( "directories:" );
    std::string strExePathAndFileName;
    if( 0 == VxFileUtil::getExecuteFullPathAndName( strExePathAndFileName ) )
    {
        infoMsg( "app exe: %s", strExePathAndFileName.c_str() );
    }

    infoMsg( "app data: %s", VxGetAppDirectory( eAppData ).c_str() );

    infoMsg( "storage: %s", VxGetAppDirectory( eAppDirRootDataStorage ).c_str() );
    infoMsg( "personal records: %s", VxGetAppDirectory( eAppDirPersonalRecords ).c_str() );
    infoMsg( "downloads: %s", VxGetAppDirectory( eAppDirDownloads ).c_str() );
    infoMsg( "user account specific: %s", VxGetAppDirectory( eAppDirUserSpecific ).c_str() );

    infoMsg( "thumbs: %s", VxGetAppDirectory( eAppDirThumbs ).c_str() );

    //infoMsg( "download: %s", VxGetAppDirectory( eAppDownload ).c_str() );
    //infoMsg( "music: %s", VxGetAppDirectory( eAppMusic ).c_str() );
    //infoMsg( "video: %s", VxGetAppDirectory( eAppVideo ).c_str() );
    //infoMsg( "picture: %s", VxGetAppDirectory( eAppPictures ).c_str() );
    //infoMsg( "document: %s", VxGetAppDirectory( eAppDocuments ).c_str() );
}

//============================================================================
void AppletTestAndDebug::fillCpuInfo( void )
{
    infoMsg( "C++ value sizes: int %d long %d long long %d int64_t %d PRId64 %s", sizeof( int ), sizeof( long ), sizeof( long long ), sizeof( int64_t ), PRId64 );
}

//============================================================================
void AppletTestAndDebug::fillExtraInfo( void )
{
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    QRect rec = QApplication::primaryScreen()->availableGeometry();
    int screenHeight = rec.height();
    int screenWidth = rec.width();
    int xDpi = QApplication::primaryScreen()->physicalDotsPerInchX();
    int yDpi = QApplication::primaryScreen()->physicalDotsPerInchY();
    int ratioDpi = QApplication::primaryScreen()->devicePixelRatio();
#else
    QRect rec = QApplication::desktop()->screenGeometry();
    int screenHeight = rec.height();
    int screenWidth = rec.width();
    int xDpi = QApplication::desktop()->physicalDpiX();
    int yDpi = QApplication::desktop()->physicalDpiY();
    int ratioDpi = QApplication::desktop()->devicePixelRatio();
#endif // QT_VERSION >= QT_VERSION_CHECK(6,0,0)

    infoMsg( "screen size: width %d heigth %d", screenWidth, screenHeight );
    infoMsg( "screen dpi: x %d y %d ratio %d", xDpi, yDpi, ratioDpi );
    infoMsg( "dpi scale: %3.1f", GuiParams().getGuiScale() );
    infoMsg( "thumbnail size: %d", GuiParams().getThumbnailSize().width() );
}

//============================================================================
void AppletTestAndDebug::slotGenerateGuidButtonClicked( void )
{
    VxGUID guid;
    guid.initializeWithNewVxGUID();
    infoMsg( "{ %lluU, %lluU }, ", guid.getVxGUIDHiPart(), guid.getVxGUIDLoPart() );
    infoMsg( "%s", guid.toOnlineIdString().c_str() );
}

//============================================================================
void AppletTestAndDebug::slotPurgeCacheButtonClicked( void )
{
    if( yesNoMessageBox( QObject::tr( "Are you sure?" ), QObject::tr( "This will delete all thumbnail images not currently in use by your identity or hosts" ) ) )
    {
        int64_t bytesPurged = m_MyApp.getEngine().fromGuiClearCache( eCacheTypeThumbnail );
        infoMsg( QString( GuiParams::describeFileLength( bytesPurged ) + QObject::tr( " of disk space freed by deleting cached thumbnails" ) ).toUtf8().constData() );
    }
}

//============================================================================
void AppletTestAndDebug::slotGenKeyButtonClicked( void )
{
    std::string keyPwd1( "4512448476bf2836979cf61b7c88f8fc70099NoLimitNet172.94.58.10345124" );
    VxKey key1;
    key1.setKeyFromPassword( keyPwd1.c_str(), keyPwd1.length() );
    infoMsg( "Gen Key 1 pwd %s", keyPwd1.c_str() );
    infoMsg( "Gen Key 1 result %s", key1.describeKey().c_str() );

    std::string keyPwd2 = ui.m_GenKeyPwdEdit->text().toUtf8().constData();
    key1.setKeyFromPassword( keyPwd2.c_str(), keyPwd2.length() );
    infoMsg( "Gen Key 1 pwd %s", keyPwd2.c_str() );
    infoMsg( "Gen Key 1 result %s", key1.describeKey().c_str() );
}

//============================================================================
void AppletTestAndDebug::slotPurgeEverythingButtonClicked( void )
{
    if( QMessageBox::Yes == GuiHelpers::errorMsgBox( eErrMsgPurgeEverythingWarning, this ) )
    {
        m_MyApp.getEngine().fromGuiClearCache( eCacheTypeThumbnail );
#if defined(TARGET_OS_WINDOWS)
        std::string userDataFolder = "rmdir /s ";
        userDataFolder = VxGetAppDirectory(eAppDirRootUserData);
        system( userDataFolder.c_str() );
        std::string appDataFolder = "rmdir /s ";
        appDataFolder = VxGetAppDirectory( eAppDirRootDataStorage );
        system( userDataFolder.c_str() );
#else

#endif //defined(TARGET_OS_WINDOWS)   
        infoMsg( "Purge Complete: You must restart application" );
    }
    else
    {
        infoMsg( "Purge was canceled" );
    }
}

//============================================================================
void AppletTestAndDebug::slotTestChooseUserButtonClicked( void )
{
    GuiUser* testUser = m_MyApp.getUserMgr().getUserForTest();
    GuiUser* myselfUser = m_MyApp.getUserMgr().getUser( m_MyApp.getMyOnlineId() );
    if( testUser && myselfUser )
    {
        AppletChooseUser* appletChooseUser = dynamic_cast<AppletChooseUser*>(m_MyApp.launchApplet( eAppletChooseUser, getContentFrameOfOppositePageFrame() ));
        if( appletChooseUser )
        {
            appletChooseUser->setChooseUserReason( eChooseUserReasonTest );
            appletChooseUser->updateUser( myselfUser );
			appletChooseUser->updateUser( testUser );
			appletChooseUser->updateUser( myselfUser );
            appletChooseUser->updateUser( testUser );
        }
        else
        {
            infoMsg( "slotTestChooseUserButtonClicked null appletChooseUser" );
        }
    }
    else
    {
        infoMsg( "slotTestChooseUserButtonClicked null testUser" );
    }
}

//============================================================================
void AppletTestAndDebug::slotDeleteDbButtonClicked( void )
{
	AppletPopupMenu* popupMenu = dynamic_cast<AppletPopupMenu*>(m_MyApp.launchApplet( eAppletPopupMenu, GuiHelpers::getParentPageFrame( this ) ));
	if( popupMenu )
	{
		popupMenu->showDeleteDbMenu();
	}
}
