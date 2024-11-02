//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLog.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QScrollBar>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QDesktopWidget>
#endif // QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QPlainTextEdit>

#include "ui_AppletLog.h"

namespace
{
    const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit *            AppletLog::getLogEdit( void )      { return ui.m_LogPlainTextEdit; }
void                        AppletLog::clear()                 { getLogEdit()->clear(); }

//============================================================================
AppletLog::AppletLog( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_LOG, app, parent )
, ui(*(new Ui::AppletLogUi))
{
    setAppletType( eAppletLog );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );

    setupApplet();
    VxAddLogHandler( this );
}

//============================================================================
AppletLog::~AppletLog()
{
    VxRemoveLogHandler( this );

    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletLog::setupApplet( void )
{
    getLogEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
    getLogEdit()->setReadOnly( true );

    connect( ui.gotoWebsiteButton, SIGNAL(clicked()), this, SLOT(gotoWebsite()) );
    connect( ui.m_CopyToClipboardButton, SIGNAL(clicked()), this, SLOT(slotCopyToClipboardClicked()) );
    connect( ui.m_LogSettingsButton, SIGNAL(clicked()), this, SLOT(slotLogSettingButtonClick()) );

    connect( this, SIGNAL( signalLogMsg(const QString&)), this, SLOT(slotLogMsg(const QString&)) );
    connect( this, SIGNAL( signalInfoMsg(const QString&)), this, SLOT(slotInfoMsg(const QString&)) );

    fillBasicInfo();
}

//============================================================================
void AppletLog::onLogEvent( uint32_t u32LogFlags, const char* logMsg )
{
    m_LogMutex.lock();
    if( m_VerboseLog
        || ( u32LogFlags & ~LOG_VERBOSE ) )
    {
        QString logStr( logMsg );
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
        logStr.remove(QRegularExpression("[\\n\\r]"));
#else
        logStr.remove(QRegExp("[\\n\\r]"));
#endif // QT_VERSION > QT_VERSION_CHECK(6,0,0)
        emit signalLogMsg( logStr );
    }

    m_LogMutex.unlock();
}

//============================================================================
void AppletLog::slotLogMsg( const QString& text )
{
    getLogEdit()->appendPlainText( text ); // Adds the message to the widget
    getLogEdit()->verticalScrollBar()->setValue( getLogEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}

//============================================================================
void  AppletLog::gotoWebsite( void )
{
    QDesktopServices::openUrl( QUrl( VxGetCompanyWebsite() ) );
}

//============================================================================
void AppletLog::slotLogSettingButtonClick( void )
{
    m_MyApp.launchApplet( eAppletLogSettings, getContentFrameOfMessengerFrame() );
}

//============================================================================
void AppletLog::slotCopyToClipboardClicked( void )
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( getLogEdit()->toPlainText() );
}

//============================================================================
void AppletLog::logMsg( const char* logMsg, ... )
{
    std::array<char, MAX_INFO_MSG_SIZE> szBuffer;
	va_list arg_ptr;
	va_start(arg_ptr, logMsg);
    vsnprintf(szBuffer.data(), MAX_INFO_MSG_SIZE, logMsg, arg_ptr);
	szBuffer.data()[MAX_INFO_MSG_SIZE - 1] = 0;
	va_end(arg_ptr);

    onLogEvent( LOG_INFO, szBuffer.data() );
}

//============================================================================
void AppletLog::fillBasicInfo( void )
{
    logMsg( "app: %s version %s", VxGetApplicationTitle(), VxGetAppVersionString() );
    std::string strExePathAndFileName;
    if( 0 == VxFileUtil::getExecuteFullPathAndName( strExePathAndFileName ) )
    {
        logMsg( "app exe: %s", strExePathAndFileName.c_str() );
    }

    logMsg( "directories:" );
    logMsg( "app data: %s", VxGetAppDirectory( eAppData ).c_str() );

    logMsg( "storage: %s", VxGetAppDirectory( eAppDirRootDataStorage ).c_str() );
    logMsg( "user: %s", VxGetAppDirectory( eAppDirUserSpecific ).c_str() );

    logMsg( "download: %s", VxGetAppDirectory( eAppDownload ).c_str() );
    logMsg( "music: %s", VxGetAppDirectory( eAppMusic ).c_str() );
    logMsg( "video: %s", VxGetAppDirectory( eAppVideo ).c_str() );
    logMsg( "picture: %s", VxGetAppDirectory( eAppPictures ).c_str() );
    logMsg( "document: %s", VxGetAppDirectory( eAppDocuments ).c_str() );
}
