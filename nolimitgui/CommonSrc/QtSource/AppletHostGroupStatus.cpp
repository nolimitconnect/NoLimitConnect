//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostGroupStatus.h"

#include "AppletInformation.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "PermissionWidget.h"

#include <QDesktopServices>
#include <QTimer>

#include "ui_AppletHostNetworkStatus.h"

PermissionWidget*           AppletHostGroupStatus::getGroupHostPermissionWidget()    { return ui.m_HostPermissionWidget; }
PermissionWidget*           AppletHostGroupStatus::getConnectionTestWidget()         { return ui.m_ConnectTestPermissionWidget; }

//============================================================================
AppletHostGroupStatus::AppletHostGroupStatus( AppCommon& app, QWidget* parent )
    : AppletBase( OBJNAME_APPLET_GROUP_HOST_STATUS, app, parent )
    , ui(*(new Ui::AppletHostNetworkStatusUi))
    , m_UpdateStatusTimer( new QTimer( this ) )
{
    ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletHostGroupStatus );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_HostingRequirementsButton->setVisible( false );
    ui.m_OptionalServiceLabel1->setVisible( false );
    ui.m_OptionalServiceLabel2->setVisible( false );

    getGroupHostPermissionWidget()->setPluginType( ePluginTypeHostGroup );
    getGroupHostPermissionWidget()->setEnabled( true );

    getConnectionTestWidget()->setPluginType( ePluginTypeHostConnectTest );
    getConnectionTestWidget()->setVisible( false );

    ui.m_OpenPortCheckBox->setEnabled( false );
    ui.m_HostPermissionCheckBox->setEnabled( false );
    ui.m_ConnectionTestPermissionCheckBox->setEnabled( false );
    ui.m_ConnectionTestPermissionCheckBox->setVisible( false );

    m_MyApp.activityStateChange( this, true );

    connect( ui.m_HostingRequirementsButton, SIGNAL(clicked()), this, SLOT(slotHostRequirementsButtonClicked() ) );
    connect( m_UpdateStatusTimer, SIGNAL( timeout() ), this, SLOT(slotUpdateStatusTimeout() ) );

    ui.m_WebsiteWidget->setUrlType( eWebsiteUrlVpn );

    m_UpdateStatusTimer->start( 3000 );
    slotUpdateStatusTimeout();
}

//============================================================================
AppletHostGroupStatus::~AppletHostGroupStatus()
{
    m_UpdateStatusTimer->stop();
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostGroupStatus::slotUpdateStatusTimeout()
{
    bool haveOpenPort = m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen();
    if( haveOpenPort )
    {
        ui.m_VpnEvalLlabel->setVisible( false );
        ui.m_WebsiteWidget->setVisible( false );
    }

    bool groupHostEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostGroup ) != eFriendStateIgnore;
    bool connectTestEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostConnectTest ) != eFriendStateIgnore;
    ui.m_OpenPortCheckBox->setChecked( haveOpenPort );
    ui.m_HostPermissionCheckBox->setChecked( groupHostEnabled );
    ui.m_ConnectionTestPermissionCheckBox->setChecked( connectTestEnabled );
    if( !haveOpenPort )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Open Port Required. Check Network Settings" ) );
    }
    else if( !groupHostEnabled )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Group Hosting Permission is disabled" ) );
    }
    else
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Group Hosting Conditions Are Met" ) );
    }

    int availGroupsCnt = m_MyApp.getFromGuiInterface().fromGuiGetJoinedListCount( ePluginTypeHostGroup );
    ui.m_GroupListCountLabel->setText( QString::number( availGroupsCnt ) );
    std::string url;
    m_MyApp.getFromGuiInterface().fromGuiGetNodeUrl( url );
    ui.m_UrlText->setText( url.c_str() );
    updateOnlineMembers();
}

//============================================================================
void AppletHostGroupStatus::slotHostRequirementsButtonClicked()
{
    AppletInformation* activityInfo = new AppletInformation( m_MyApp, this, eInfoTypeIgnoredList );
    if( activityInfo )
    {
        activityInfo->show();
    }
}

//============================================================================
void AppletHostGroupStatus::updateOnlineMembers( void )
{
    int onlinMemberCnt = ui.m_FriendListWidget->updateHostServerMembers( eHostTypeGroup );
     ui.m_GroupListCountLabel->setText( QString::number( onlinMemberCnt ) );
}
