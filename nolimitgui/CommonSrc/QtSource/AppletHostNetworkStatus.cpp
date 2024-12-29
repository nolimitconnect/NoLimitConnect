//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostNetworkStatus.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QDesktopServices>
#include <QTimer>

#include "ui_AppletHostNetworkStatus.h"

PermissionWidget*           AppletHostNetworkStatus::getNetworkHostPermissionWidget()    { return ui.m_HostPermissionWidget; }
PermissionWidget*           AppletHostNetworkStatus::getConnectionTestWidget()         { return ui.m_ConnectTestPermissionWidget; }

//============================================================================
AppletHostNetworkStatus::AppletHostNetworkStatus( AppCommon& app, QWidget* parent )
    : AppletBase( OBJNAME_APPLET_HOST_NETWORK_STATUS, app, parent )
    , ui(*(new Ui::AppletHostNetworkStatusUi))
    , m_UpdateStatusTimer( new QTimer( this ) )
{
    ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletHostNetworkStatus );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_HostingRequirementsButton->setVisible( false );
    getNetworkHostPermissionWidget()->setPluginType( ePluginTypeHostNetwork );
    getConnectionTestWidget()->setPluginType( ePluginTypeHostConnectTest );

    ui.m_OpenPortCheckBox->setEnabled( false );
    ui.m_HostPermissionCheckBox->setEnabled( false );
    ui.m_ConnectionTestPermissionCheckBox->setEnabled( false );

    m_MyApp.activityStateChange( this, true );

    connect( ui.m_HostingRequirementsButton, SIGNAL(clicked()), this, SLOT( slotHostRequirementsButtonClicked() ) );
    connect( m_UpdateStatusTimer, SIGNAL( timeout() ), this, SLOT( slotUpdateStatusTimeout() ) );
    connect( ui.m_VistEvalVpnsButton, SIGNAL(clicked()), this, SLOT( gotoWebsite() ) );

    m_UpdateStatusTimer->start( 3000 );
    slotUpdateStatusTimeout();
}

//============================================================================
AppletHostNetworkStatus::~AppletHostNetworkStatus()
{
    m_UpdateStatusTimer->stop();
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostNetworkStatus::slotHostRequirementsButtonClicked()
{
}

//============================================================================
void AppletHostNetworkStatus::slotUpdateStatusTimeout()
{
    bool haveOpenPort = m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen();
    bool networkHostEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostNetwork ) != eFriendStateIgnore;
    bool connectTestEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostConnectTest ) != eFriendStateIgnore;
    ui.m_OpenPortCheckBox->setChecked( haveOpenPort );
    ui.m_HostPermissionCheckBox->setChecked( networkHostEnabled );
    ui.m_ConnectionTestPermissionCheckBox->setChecked( connectTestEnabled );
    if( !haveOpenPort )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Open Port Required. Check Network Settings" ) );
    }
    else if( !networkHostEnabled )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Network Hosting Permission is disabled" ) );
    }
    else if( !connectTestEnabled )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Connection Test Permission is disabled" ) );
    }
    else
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Network Hosting Conditions Are Met" ) );
    }

    int availGroupsCnt = m_MyApp.getFromGuiInterface().fromGuiGetJoinedListCount( ePluginTypeNetworkSearchList );
    ui.m_GroupListCountLabel->setText( QString::number( availGroupsCnt ) );
    std::string url;
    m_MyApp.getFromGuiInterface().fromGuiGetNodeUrl( url );
    ui.m_UrlText->setText( url.c_str() );
}

//============================================================================
void AppletHostNetworkStatus::gotoWebsite( void )
{
    QDesktopServices::openUrl( QUrl( "https://nolimitconnect.com/nlc/vpns/" ) );
}
