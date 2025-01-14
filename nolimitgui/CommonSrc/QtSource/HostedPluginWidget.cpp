//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "HostedPluginWidget.h"
#include "GuiHostSession.h"
#include "GuiParams.h"
#include "AppCommon.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

#include "ui_GuiHostedListItem.h"

VxPushButton*              HostedPluginWidget:: getIdentAvatarButton( void )        { return ui.m_IconButton; }
VxPushButton*               HostedPluginWidget::getIdentFriendshipButton( void ) { return ui.m_FriendshipButton; }
VxPushButton*               HostedPluginWidget::getIdentMenuButton( void ) { return ui.m_MenuButton; }

QLabel*             HostedPluginWidget::getIdentLine1( void ) { return ui.m_TitlePart1; }
QLabel*             HostedPluginWidget::getIdentLine2( void ) { return ui.m_DescPart1; }

//============================================================================
HostedPluginWidget::HostedPluginWidget( QWidget* parent )
: IdentLogicInterface( parent )
, ui(*(new Ui::GuiHostedListItemUi))
, m_MyApp( GetAppInstance() )
{
	ui.setupUi( this );
    setupIdentLogic();
    connect( ui.m_JoinButton,		SIGNAL(pressed()),	this, SLOT(slotJoinButtonPressed()) );
    connect( ui.m_ConnectButton,    SIGNAL( pressed() ), this, SLOT( slotConnectButtonPressed() ) );
   
    ui.m_MenuButton->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_MenuButton->setIcon( eMyIconMenu );
    ui.m_JoinButton->setIcon( eMyIconPersonAdd );
    ui.m_MenuButton->setFixedSize( GuiParams::getButtonSize( eButtonSizeSmall ) );
    ui.m_ConnectButton->setIcon( eMyIconConnect );
    ui.m_ConnectButton->setFixedSize( GuiParams::getButtonSize( eButtonSizeSmall ) );
    showConnectButton( false );
}

//============================================================================
HostedPluginWidget::~HostedPluginWidget()
{
}

//============================================================================
MyIcons& HostedPluginWidget::getMyIcons( void )
{
    return m_MyApp.getMyIcons();
}

//============================================================================
void HostedPluginWidget::resizeEvent(QResizeEvent* resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    updateWidgetFromInfo();
}

//============================================================================
void HostedPluginWidget::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
}

//============================================================================
void HostedPluginWidget::setHostSession( GuiHostSession* hostSession )
{
    m_HostSession = hostSession;
}

//============================================================================
GuiHostSession * HostedPluginWidget::getHostSession( void )
{
    return m_HostSession;
}

//============================================================================
void HostedPluginWidget::onIdentAvatarButtonClicked()
{
    LogMsg( LOG_DEBUG, "HostedPluginWidget::slotIconButtonClicked" );
}

//============================================================================
void HostedPluginWidget::onIdentMenuButtonClicked( void )
{
}

//============================================================================
void HostedPluginWidget::updateWidgetFromInfo( void )
{
    GuiHostSession* hostSession = getHostSession();
    if( nullptr == hostSession )
    {
        return;
    }

    GuiUser* hostIdent = hostSession->getGuiUser();
    if( hostIdent )
    {
        updateIdentity( hostIdent );
    }

    if( m_MyApp.getEngine().getMyOnlineId() == hostIdent->getMyOnlineId() )
    {
        ui.m_TitlePart2->setText( QObject::tr( " Hosted By Me") );
        ui.m_FriendshipButton->setIcon( eMyIconAdministrator );
    }

    if( !ui.m_IconButton->hasImage() )
    {
        VxGUID thumbId = hostSession->getHostThumbId();
        if( !thumbId.isVxGUIDValid() )
        {
            thumbId = hostIdent->getHostThumbId( hostSession->getHostType(), true );
        }
       
        if( thumbId.isVxGUIDValid() )
        {
            QImage thumbImage;
            if( m_MyApp.getThumbMgr().getThumbImage( thumbId, thumbImage ) )
            {
                ui.m_IconButton->setIconOverrideImage( thumbImage );
            }
        }
    }

    // set text of line 2
    std::string strDesc = hostSession->getHostDescription();
    if( strDesc.empty() )
    {
        strDesc = hostIdent->getOnlineDescription();
    }

    if( !strDesc.empty() )
    {
        ui.m_DescPart2->setText( strDesc.c_str() );
    }
}

//============================================================================
void HostedPluginWidget::setJoinedState( EJoinState joinState )
{
    // todo update join 
    switch( joinState )
    {
    case eJoinStateJoinWasGranted:
        ui.m_ConnectButton->setIcon( eMyIconConnect );
        showConnectButton( true );
        break;
    case eJoinStateJoinIsGranted:
        ui.m_ConnectButton->setIcon( eMyIconDisconnect );
        showConnectButton( true );
        break;
    case eJoinStateSending:
    case eJoinStateSendFail:
    case eJoinStateSendAcked:
    case eJoinStateJoinRequested:
    case eJoinStateJoinDenied:
    case eJoinStateNone:
    default:
        showConnectButton( false );
        break;
    }
}

//============================================================================
void HostedPluginWidget::showConnectButton( bool isAccepted )
{
    ui.m_JoinButton->setVisible( !isAccepted );
    ui.m_JoinLabel->setVisible( !isAccepted );
    ui.m_ConnectButton->setVisible( isAccepted );
    ui.m_ConnectLabel->setVisible( isAccepted );
}

//============================================================================
void HostedPluginWidget::slotJoinButtonPressed( void )
{
    LogMsg( LOG_DEBUG, "HostedPluginWidget::slotJoinButtonPressed" );
}

//============================================================================
void HostedPluginWidget::slotConnectButtonPressed( void )
{
    LogMsg( LOG_DEBUG, "HostedPluginWidget::slotConnectButtonPressed" );
}
