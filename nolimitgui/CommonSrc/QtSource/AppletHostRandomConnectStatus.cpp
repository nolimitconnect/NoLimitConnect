//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "AppletHostRandomConnectStatus.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "GuiHelpers.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

//============================================================================
AppletHostRandomConnectStatus::AppletHostRandomConnectStatus( AppCommon& app, QWidget* parent )
    : AppletBase( OBJNAME_APPLET_HOST_RANDOM_CONNECT_STATUS, app, parent )
    , m_UpdateStatusTimer( new QTimer( this ) )
{
    ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletHostRandomConnectStatus );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_HostingRequirementsButton->setVisible( false );
    getRandomConnectHostPermissionWidget()->setPluginType( ePluginTypeHostRandomConnect );
    getConnectionTestWidget()->setPluginType( ePluginTypeHostConnectTest );
    getRandomConnectPermissionWidget()->setPluginType( ePluginTypeHostRandomConnect );
    getGroupHostPermissionWidget()->setPluginType( ePluginTypeHostGroup );

    ui.m_OpenPortCheckBox->setEnabled( false );
    ui.m_HostPermissionCheckBox->setEnabled( false );
    ui.m_ConnectionTestPermissionCheckBox->setEnabled( false );

    ui.m_OptionalLabel->setVisible( false );
    ui.m_RandomConnectPermissionWidget->setVisible( false );

    ui.m_AdditionalLabel1->setVisible( false );
    ui.m_AdditionalLabel2->setVisible( false );
    ui.m_AdditionalPermissionWidget->setVisible( false );
    m_MyApp.activityStateChange( this, true );

    connect( ui.m_HostingRequirementsButton, SIGNAL( clicked() ), this, SLOT( slotHostRequirementsButtonClicked() ) );
    connect( m_UpdateStatusTimer, SIGNAL( timeout() ), this, SLOT( slotUpdateStatusTimeout() ) );
    m_UpdateStatusTimer->start( 3000 );
    slotUpdateStatusTimeout();
}

//============================================================================
AppletHostRandomConnectStatus::~AppletHostRandomConnectStatus()
{
    m_UpdateStatusTimer->stop();
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostRandomConnectStatus::slotHostRequirementsButtonClicked()
{
}

//============================================================================
void AppletHostRandomConnectStatus::slotUpdateStatusTimeout()
{
    bool haveOpenPort = m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen();
    bool networkHostEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostRandomConnect ) != eFriendStateIgnore;
    bool connectTestEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostConnectTest ) != eFriendStateIgnore;
    ui.m_OpenPortCheckBox->setChecked( haveOpenPort );
    ui.m_HostPermissionCheckBox->setChecked( networkHostEnabled );
    ui.m_ConnectionTestPermissionCheckBox->setChecked( connectTestEnabled );
    if( !haveOpenPort )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Open Port Required. Check RandomConnect Settings" ) );
    }
    else if( !networkHostEnabled )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "RandomConnect Hosting Permission is disabled" ) );
    }
    else if( !connectTestEnabled )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Connection Test Permission is disabled" ) );
    }
    else
    {
        ui.m_HostingStatusText->setText( QObject::tr( "RandomConnect Hosting Conditions Are Met" ) );
    }

    //int availGroupsCnt = m_MyApp.getFromGuiInterface().fromGuiGetJoinedListCount( ePluginTypeRandomConnectSearchList );
    //ui.m_GroupListCountLabel->setText( QString::number( availGroupsCnt ) );
    std::string url;
    m_MyApp.getFromGuiInterface().fromGuiGetNodeUrl( false, url );
    ui.m_UrlText->setText( url.c_str() );
}
