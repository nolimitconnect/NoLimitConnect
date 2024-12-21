//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletTestUpnp.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <NetLib/VxPortForward.h>

#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QScrollBar>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QDesktopWidget>
#endif // QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QPlainTextEdit>

#include "ui_AppletTestUpnp.h"

namespace
{
    const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit *            AppletTestUpnp::getLogEdit( void )      { return ui.m_LogPlainTextEdit; }
void                        AppletTestUpnp::clear()                 { getLogEdit()->clear(); }

//============================================================================
AppletTestUpnp::AppletTestUpnp( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_TEST_UPNP, app, parent )
, ui(*(new Ui::AppletTestUpnpUi))
{
    setAppletType( eAppletTestUpnp );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );

    getLogEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
    getLogEdit()->setReadOnly( true );

    connect( this, SIGNAL(signalLogMsg(const QString&)), this, SLOT(slotLogMsg(const QString&)) );

    connect( ui.m_AddPortForwardButton, SIGNAL(clicked()), this, SLOT(slotAddPortForward()) );
    connect( ui.m_RemovePortForwardButton, SIGNAL(clicked()), this, SLOT(slotRemovePortForward()) );
    connect( ui.m_ListPortForwardButton, SIGNAL(clicked()), this, SLOT(slotListPortForward()) );
    connect( ui.m_CopyLogToClipboardButton, SIGNAL(clicked()), this, SLOT(slotCopyLogToClipboard()) );

    ui.m_DebugCheckBox->setChecked( true );
    ui.m_ErrorCheckBox->setChecked( true );

    connect( ui.m_VerboseCheckBox, SIGNAL(clicked()), this, SLOT(slotUpdateLogFlags()) );
    connect( ui.m_DebugCheckBox, SIGNAL(clicked()), this, SLOT(slotUpdateLogFlags()) );
    connect( ui.m_ErrorCheckBox, SIGNAL(clicked()), this, SLOT(slotUpdateLogFlags()) );

    VxAddLogHandler( this );

    m_LogLevelFlags = VxGetLogLevelFlags();
    VxSetLogLevelFlags( m_LogLevelFlags | LOG_ERROR | LOG_DEBUG );

    m_WasUpnpLogEnabled = LogEnabled( eLogPortForward );
    if( !m_WasUpnpLogEnabled )
    {
        m_LogModuleFlags = VxGetLogModuleFlags();
        VxSetLogModuleFlags( m_LogModuleFlags | eLogPortForward );
    }

    m_WasPortForwardEnabled = VxPortForward::getEnablePortForward();
    if( !m_WasPortForwardEnabled )
    {
        VxPortForward::setEnablePortForward( true );
    }

    ui.m_PortEdit->setText( std::to_string( m_Engine.getEngineSettings().getTcpIpPort()).c_str() );
    ui.m_IpEdit->setText( m_Engine.getNetStatusAccum().getLocalIpAddress().c_str() );
}

//============================================================================
AppletTestUpnp::~AppletTestUpnp()
{
    VxRemoveLogHandler( this );

    m_WasPortForwardEnabled = VxPortForward::getEnablePortForward();
    if( !m_WasPortForwardEnabled )
    {
        VxPortForward::setEnablePortForward( false );
    }

    if( !m_WasUpnpLogEnabled )
    {
        VxSetLogModuleFlags( m_LogModuleFlags );
    }

    VxSetLogLevelFlags( m_LogLevelFlags );

    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletTestUpnp::onLogEvent( uint32_t u32LogFlags, const char* logMsg )
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
void AppletTestUpnp::slotLogMsg( const QString& text )
{
    getLogEdit()->appendPlainText( text ); // Adds the message to the widget
    getLogEdit()->verticalScrollBar()->setValue( getLogEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}

//============================================================================
void AppletTestUpnp::slotAddPortForward()
{
    if( validateIpAndPort() )
    {
        logMsg( "VxPortForward::addPortForward params %s IP %s port %d", 
                m_IsIpv6 ? "ipv6" : "ipv4", m_IpAddr.c_str(), m_Port );
        VxPortForward::addPortForward( m_IsIpv6, m_IpAddr.c_str(), m_Port, true );
    }
}

//============================================================================
void AppletTestUpnp::slotRemovePortForward()
{
    if( validateIpAndPort() )
    {
        logMsg( "VxPortForward::removePortForward params %s port %d ", m_IsIpv6 ? "ipv6" : "ipv4", m_Port );
        VxPortForward::removePortForward( m_IsIpv6, m_Port, true );
    }
}

//============================================================================
void AppletTestUpnp::slotListPortForward()
{
    if( validateIpAndPort() )
    {      
        logMsg( "VxPortForward::listPortForward prams %s ip %s", m_IsIpv6 ? "ipv6" : "ipv4", m_IpAddr.c_str() );
        VxPortForward::listPortForward( m_IpAddr.c_str(), m_IsIpv6, true );
    }
}

//============================================================================
void AppletTestUpnp::slotCopyLogToClipboard()
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( getLogEdit()->toPlainText() );
}

//============================================================================
void AppletTestUpnp::slotUpdateLogFlags()
{
    uint32_t logLevelFlags = m_LogLevelFlags;
    if( ui.m_VerboseCheckBox->isChecked() )
    {
        logLevelFlags |= LOG_VERBOSE;
    }
    else
    {
        logLevelFlags &= ~LOG_VERBOSE;
    }

    if( ui.m_DebugCheckBox->isChecked() )
    {
        logLevelFlags |= LOG_DEBUG;
    }
    else
    {
        logLevelFlags &= ~LOG_DEBUG;
    }

    if( ui.m_ErrorCheckBox->isChecked() )
    {
        logLevelFlags |= LOG_ERROR;
    }
    else
    {
        logLevelFlags &= ~LOG_ERROR;
    }

    VxSetLogLevelFlags( logLevelFlags );
}

//============================================================================
void AppletTestUpnp::logMsg( const char* logMsg, ... )
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
bool AppletTestUpnp::validateIpAndPort( void )
{
    m_Port = ui.m_PortEdit->text().toUShort();
    if( m_Port < 80 )
    {
        QMessageBox::information( this, QObject::tr("Invalid Port"), QObject::tr( "Port must be greater than 79" ), QMessageBox::Ok );
        return false;
    }

    m_IpAddr = ui.m_IpEdit->text().toUtf8().constData();
    InetAddress inetAddr( m_IpAddr.c_str() );
    EIpAddrType	addrType = inetAddr.getIpAddrType();
    m_IsIpv6 = eIpAddrTypeIpv6 == addrType;
    if( eIpAddrTypeUnknown == addrType )
    {
        QMessageBox::information( this, QObject::tr("Invalid IP Address"), QObject::tr( "IP Address is invalid" ), QMessageBox::Ok );
        return false;
    }

    return true;
}