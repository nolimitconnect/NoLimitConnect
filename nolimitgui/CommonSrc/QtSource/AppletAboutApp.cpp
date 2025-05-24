//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletAboutApp.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/OsDetect.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QDesktopServices>
#include <QUrl>

#include "ui_AppletAboutApp.h"

//============================================================================
AppletAboutApp::AppletAboutApp( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_ABOUT_APP, app, parent )
, ui(*(new Ui::AppletAboutAppUi))
{
    setAppletType( eAppletAboutNoLimitConnect );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    ui.m_AppNameLabel->setText( VxGetApplicationTitle() );
    
	m_MyApp.activityStateChange( this, true );

    connect( ui.m_ShowAppInfo, SIGNAL(clicked()), this, SLOT(slotShowAppInfo()) );
    connect( ui.m_CopyDeviceInfoToClipboard, SIGNAL(clicked()), this, SLOT(slotCopyDeviceInfoToClipboard()) );
    connect( ui.m_CopyConnectionInfoToClipboard, SIGNAL(clicked()), this, SLOT(slotCopyConnectionInfoToClipboard()) );

    setupDeviceInfo();
    setupConnectionInfo();
}

//============================================================================
void AppletAboutApp::setupDeviceInfo( void )
{
    ui.m_CopyDeviceInfoToClipboard->setActionText( QObject::tr( "Copy device info to clipboard (For Bug Report)" ) );
    QString versionLabel = VxGetApplicationTitle();
    versionLabel += QObject::tr( " Version " );
    versionLabel += VxGetAppVersionString();
    ui.m_AppVersionLabel->setText( versionLabel );
    ui.m_DeviceInfoTextEdit->appendPlainText( versionLabel );
    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "OS: %1" ).arg( OsDetect::getOsName().c_str() ) );
    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "CPU: %1" ).arg( OsDetect::getCpuName().c_str() ) );
    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "Is Big Endian CPU: %1" ).arg( IsBigEndianCpu() ? "true" : "false" ) );
    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "Available Storage Space: %1" ).arg( GuiHelpers::getAvailableStorageSpaceText() ) );

    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "Qt Version %1" ).arg( qVersion() ) );

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

    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "Screen size: width %1 heigth %2" ).arg( screenWidth ).arg( screenHeight ) );
    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "Screen DPI: x %1 y %2 ratio %3" ).arg( xDpi ).arg( yDpi ).arg( ratioDpi ) );
    ui.m_DeviceInfoTextEdit->appendPlainText( QString( "DPI scale: %2").arg( GuiParams::getGuiScale() ) );
}

//============================================================================
void AppletAboutApp::setupConnectionInfo( void )
{
    ui.m_CopyConnectionInfoToClipboard->setActionText( QObject::tr( "Copy connection info to clipboard (NOT RECOMMENDED)" ) );
    VxNetIdent oMyIdent;
    m_FromGui.fromGuiQueryMyIdent( &oMyIdent );

    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "Online Name: %1" ).arg( oMyIdent.getOnlineName() ) );
    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "My Url: %1" ).arg( oMyIdent.getMyOnlineUrl().c_str() ) );
    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "Local IPv4 Address: %1" ).arg( m_MyApp.getEngine().getNetStatusAccum().getLocalIpv4().c_str()));
    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "Local IPv6 Address: %1" ).arg( m_MyApp.getEngine().getNetStatusAccum().getLocalIpv6().c_str()));
    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "Requires Relay?: %1 " ).arg( oMyIdent.requiresRelay() ) );
    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "Has About Me Content?: %1" ).arg( oMyIdent.hasAboutMeContent() ) );
    ui.m_ConnectionInfoTextEdit->appendPlainText( QString( "Has Storyboard Content?: %1" ).arg( oMyIdent.hasStoryboardContent() ) );
}

//============================================================================
void AppletAboutApp::slotShowAppInfo( void )
{
    m_MyApp.launchApplet( eAppletApplicationInfo, getContentFrameOfOppositePageFrame() );
}

//============================================================================
AppletAboutApp::~AppletAboutApp()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletAboutApp::slotCopyDeviceInfoToClipboard( void )
{
    ui.m_CopyDeviceInfoToClipboard->copyToClipboard( ui.m_DeviceInfoTextEdit->toPlainText() );
}

//============================================================================
void AppletAboutApp::slotCopyConnectionInfoToClipboard( void )
{
    ui.m_CopyConnectionInfoToClipboard->copyToClipboard( ui.m_ConnectionInfoTextEdit->toPlainText() );
}