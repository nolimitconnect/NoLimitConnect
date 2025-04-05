//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostChatRoomStatus.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QTimer>

#include "ui_AppletHostNetworkStatus.h"

PermissionWidget*           AppletHostChatRoomStatus::getChatRoomHostPermissionWidget()   { return ui.m_HostPermissionWidget; }
PermissionWidget*           AppletHostChatRoomStatus::getConnectionTestWidget()           { return ui.m_ConnectTestPermissionWidget; }

//============================================================================
AppletHostChatRoomStatus::AppletHostChatRoomStatus( AppCommon& app, QWidget* parent )
    : AppletBase( OBJNAME_APPLET_HOST_CHAT_ROOM_STATUS, app, parent )
    , ui(*(new Ui::AppletHostNetworkStatusUi))
    , m_UpdateStatusTimer( new QTimer( this ) )
{
    ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletHostChatRoomStatus );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_HostingRequirementsButton->setVisible( false );
    ui.m_OptionalServiceLabel1->setVisible( false );
    ui.m_OptionalServiceLabel2->setVisible( false );

    getChatRoomHostPermissionWidget()->setPluginType( ePluginTypeHostChatRoom );
    getChatRoomHostPermissionWidget()->setEnabled( true );

    getConnectionTestWidget()->setPluginType( ePluginTypeHostConnectTest );
    getConnectionTestWidget()->setVisible( false );

    ui.m_OpenPortCheckBox->setEnabled( false );
    ui.m_HostPermissionWidget->setEnabled( false );
    ui.m_ConnectionTestPermissionCheckBox->setEnabled( false );
    ui.m_ConnectionTestPermissionCheckBox->setVisible( false );

    ui.m_WebsiteWidget->setUrlType( eWebsiteUrlVpn );

    m_MyApp.activityStateChange( this, true );

    connect( ui.m_HostingRequirementsButton, SIGNAL(clicked()), this, SLOT( slotHostRequirementsButtonClicked() ) );
    connect( m_UpdateStatusTimer, SIGNAL( timeout() ), this, SLOT( slotUpdateStatusTimeout() ) );
    

    m_UpdateStatusTimer->start( 3000 );
    slotUpdateStatusTimeout();
}

//============================================================================
AppletHostChatRoomStatus::~AppletHostChatRoomStatus()
{
    m_UpdateStatusTimer->stop();
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostChatRoomStatus::slotHostRequirementsButtonClicked()
{
}

//============================================================================
void AppletHostChatRoomStatus::slotUpdateStatusTimeout()
{
    bool haveOpenPort = m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen();
    bool networkHostEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostChatRoom ) != eFriendStateIgnore;
    bool connectTestEnabled = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( ePluginTypeHostConnectTest ) != eFriendStateIgnore;
    ui.m_OpenPortCheckBox->setChecked( haveOpenPort );
    ui.m_HostPermissionCheckBox->setChecked( networkHostEnabled );
    ui.m_ConnectionTestPermissionCheckBox->setChecked( connectTestEnabled );
    if( !haveOpenPort )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Open Port Required. Check ChatRoom Settings" ) );
    }
    else if( !networkHostEnabled )
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Chat Room Hosting Permission is disabled" ) );
    }
    else
    {
        ui.m_HostingStatusText->setText( QObject::tr( "Chat Room Hosting Conditions Are Met" ) );
    }

    //int availGroupsCnt = m_MyApp.getFromGuiInterface().fromGuiGetJoinedListCount( ePluginTypeChatRoomSearchList );
    //ui.m_GroupListCountLabel->setText( QString::number( availGroupsCnt ) );
    std::string url;
    m_MyApp.getFromGuiInterface().fromGuiGetNodeUrl( url );
    ui.m_UrlText->setText( url.c_str() );
    // BRJ TODO investigate why this is needed. there should be nothing to set it to disabled
    getChatRoomHostPermissionWidget()->setEnabled( true );
    updateOnlineMembers();
}

//============================================================================
void AppletHostChatRoomStatus::updateOnlineMembers( void )
{
    int onlinMemberCnt = ui.m_FriendListWidget->updateHostServerMembers( eHostTypeChatRoom );
    ui.m_GroupListCountLabel->setText( QString::number( onlinMemberCnt ) );
}
