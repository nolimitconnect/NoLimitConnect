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
#include "MyIcons.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QDesktopServices>
#include <QUrl>

//============================================================================
AppletAboutApp::AppletAboutApp( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_ABOUT_APP, app, parent )
{
    setAppletType( eAppletAboutNoLimitConnect );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    ui.m_AppNameLabel->setText( VxGetApplicationTitle() );
    
	m_MyApp.activityStateChange( this, true );

    connect( ui.gotoWebsiteButton, SIGNAL( clicked() ), this, SLOT( gotoWebsite() ) );
    connect( ui.m_ShowAppInfo, SIGNAL( clicked() ), this, SLOT( slotShowAppInfo() ) );

    setupAboutMe();
}

//============================================================================
void AppletAboutApp::setupAboutMe( void )
{
    QString versionLabel = VxGetApplicationTitle();
    versionLabel += QObject::tr( " Version " );
    versionLabel += VxGetAppVersionString();
    ui.m_AppVersionLabel->setText( versionLabel );

    fillMyNodeUrl( ui.m_NodeUrlLabel );
    ui.m_AvailSpace->setText( GuiHelpers::getAvailableStorageSpaceText() );
    VxNetIdent oMyIdent;
    m_FromGui.fromGuiQueryMyIdent( &oMyIdent );
    std::string strOnlineIp = oMyIdent.getOnlineIpAddress(false).toStdString();
    QString strText = QString( "Online Name: %1" ).arg( oMyIdent.getOnlineName() );
    ui.labelMeLine1->setText( QString( "Online Name: %1" ).arg( oMyIdent.getOnlineName() ) );
    std::string strMyId = oMyIdent.m_DirectConnectId.describeVxGUID();
    ui.labelMeLine2->setText( QString( "Id: %1" ).arg( strMyId.c_str() ) );
    ui.labelMeLine3->setText( QString( "My IPv4 Url:%1" ).arg( oMyIdent.getMyOnlineUrl(false).c_str() ) );
    ui.labelMeLine4->setText( QString( "My IPv6 Url:%1" ).arg( oMyIdent.getMyOnlineUrl(true).c_str() ) );
    ui.labelMeLine6->setText( QString( "Requires Relay?: %1 " ).arg( oMyIdent.requiresRelay() ) );
    ui.labelMeLine7->setText( QString( "Has Profile Pic?: %1" ).arg( oMyIdent.hasProfilePicture() ) );
    ui.labelMeLine9->setText( QString( "Is Big Endian CPU ?: %1" ).arg( IsBigEndianCpu() ? "true" : "false" ) );
}

//============================================================================
void  AppletAboutApp::slotShowAppInfo( void )
{
    m_MyApp.launchApplet( eAppletApplicationInfo, getContentFrameOfOppositePageFrame() );
}

//============================================================================
void  AppletAboutApp::gotoWebsite( void )
{
    QDesktopServices::openUrl( QUrl( "https://nolimitconnect.com/" ) );
}

//============================================================================
AppletAboutApp::~AppletAboutApp()
{
    m_MyApp.activityStateChange( this, false );
}
