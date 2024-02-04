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

#include "ActivityInformation.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletHostGroupStatus::AppletHostGroupStatus( AppCommon& app, QWidget* parent )
    : AppletBase( OBJNAME_APPLET_GROUP_HOST_STATUS, app, parent )
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

    connect( ui.m_HostingRequirementsButton, SIGNAL(clicked()), this, SLOT( slotHostRequirementsButtonClicked() ) );
    connect( m_UpdateStatusTimer, SIGNAL( timeout() ), this, SLOT( slotUpdateStatusTimeout() ) );
    connect( ui.m_VistEvalVpnsButton, SIGNAL(clicked()), this, SLOT( gotoWebsite() ) );

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

    int availGroupsCnt = m_MyApp.getFromGuiInterface().fromGuiGetJoinedListCount( ePluginTypeNetworkSearchList );
    ui.m_GroupListCountLabel->setText( QString::number( availGroupsCnt ) );
    std::string url;
    m_MyApp.getFromGuiInterface().fromGuiGetNodeUrl( false, url );
    ui.m_UrlText->setText( url.c_str() );
    updateOnlineMembers();
}

//============================================================================
void AppletHostGroupStatus::slotHostRequirementsButtonClicked()
{
    ActivityInformation* activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeIgnoredList );
    if( activityInfo )
    {
        activityInfo->show();
    }
}

//============================================================================
void AppletHostGroupStatus::gotoWebsite( void )
{
    QDesktopServices::openUrl( QUrl( "https://nolimitconnect.com/nlc/vpns/" ) );
}

//============================================================================
void AppletHostGroupStatus::updateOnlineMembers( void )
{
    int onlinMemberCnt = ui.m_FriendListWidget->updateHostServerMembers( eHostTypeGroup );
     ui.m_GroupListCountLabel->setText( QString::number( onlinMemberCnt ) );
}
