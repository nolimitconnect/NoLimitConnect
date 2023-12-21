//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppSettings.h"
#include "GuiUserMultiListWidget.h"
#include "ActivityMessageBox.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"
#include "AppGlobals.h"
#include "MyIcons.h"
#include "AppCommon.h"

#include <CoreLib/VxGlobals.h>

//============================================================================
GuiUserMultiListWidget::GuiUserMultiListWidget(	QWidget* parent )
: QWidget( parent )
, m_MyApp( GetAppInstance() )
{
    ui.setupUi( this );

	ui.m_IdentWidget->setupIdentLogic( eButtonSizeSmall );

    ui.m_HostViewFrame->setFixedHeight( GuiParams::getButtonSize( eButtonSizeSmall ).height() + 4 );
	ui.m_FriendsView->setFixedSize( eButtonSizeSmall );
    ui.m_FriendsView->setIcon( eMyIconEyeFriends );
	ui.m_GroupHost->setFixedSize( eButtonSizeSmall );
    ui.m_GroupHost->setIcon( eMyIconEyeGroup );
	ui.m_ChatRoomHost->setFixedSize(eButtonSizeSmall);
    ui.m_ChatRoomHost->setIcon( eMyIconEyeChatRoom );
	ui.m_RandomConnectHost->setFixedSize( eButtonSizeSmall );
    ui.m_RandomConnectHost->setIcon( eMyIconEyeRandomConnect );
	ui.m_EverybodyView->setFixedSize( eButtonSizeSmall );
    ui.m_EverybodyView->setIcon( eMyIconEyeAll );

	ui.m_EyeUsers->setFixedSize( eButtonSizeSmall );
    ui.m_EyeUsers->setIcon( eMyIconEyeShow );
	ui.m_EyeSession->setFixedSize( eButtonSizeSmall );
    ui.m_EyeSession->setIcon( eMyIconEyeShow );

    ui.m_EyeSearch->setFixedSize( eButtonSizeSmall );
    ui.m_EyeSearch->setIcon( eMyIconEyeSearchDisabled );
    ui.m_EyeSearch->setVisible( false );
    ui.m_SearchBarWidget->setVisible( false );

    m_OffersFrame			= ui.m_OffersFrame;

    connect( ui.m_EyeUsers,		        SIGNAL(clicked()),						this,	SLOT(slotEyeHostButtonClicked()) );
    connect( ui.m_EyeSession,           SIGNAL(clicked()),						this,	SLOT(slotEyeSessionButtonClicked()) );
    connect( ui.m_EyeSearch,            SIGNAL(clicked()),                      this,   SLOT(slotEyeSearchButtonClicked()) );

    connect( ui.m_EverybodyView,		SIGNAL(clicked()),						this,	SLOT(slotEverybodyButtonClicked()) );
    connect( ui.m_FriendsView,		    SIGNAL(clicked()),						this,	SLOT(slotFriendsButtonClicked()) );
    connect( ui.m_GroupHost,	        SIGNAL(clicked()),						this,	SLOT(slotGroupHostButtonClicked()) );
    connect( ui.m_ChatRoomHost ,        SIGNAL(clicked()),						this,	SLOT(slotChatRoomHostButtonClicked()) );
    connect( ui.m_RandomConnectHost,	SIGNAL(clicked()),	                    this,	SLOT(slotRandomConnectHostButtonClicked()) );

	connect( ui.m_UserListWidget,		SIGNAL(signalUserAvatarClicked(GuiUser*)), this, SLOT(slotUserSelected(GuiUser*)) );
    connect( ui.m_SearchBarWidget,      SIGNAL(signalSearchTextChanged(QString)), this, SLOT(slotSearchTextChanged(QString)) );

    ui.m_GroupHost->setVisible( false );
    ui.m_GroupLabel->setVisible( false );
    ui.m_ChatRoomHost->setVisible( false );
    ui.m_ChatRoomLabel->setVisible( false );
    ui.m_RandomConnectHost->setVisible( false );
    ui.m_RandomConnectLabel->setVisible( false );
}

//============================================================================
void GuiUserMultiListWidget::setHostAdminId( GroupieId adminId )
{
    m_HostAdminId = adminId;
}

//============================================================================
void GuiUserMultiListWidget::setUserViewType( EUserViewType viewType )
{
    m_UserViewType = viewType;

    ui.m_IdentWidget->clearIdentity();
    ui.m_EverybodyView->setNotifyType( eNotifyOffline );
    ui.m_FriendsView->setNotifyType( eNotifyOffline );
    ui.m_GroupHost->setNotifyType( eNotifyOffline );
    ui.m_ChatRoomHost->setNotifyType( eNotifyOffline );
    ui.m_RandomConnectHost->setNotifyType( eNotifyOffline );

    switch( viewType )
    {
    case eUserViewTypeEverybody:
        ui.m_EverybodyLabel->setVisible( true );
        ui.m_EverybodyView->setVisible( true );
        ui.m_EverybodyView->setNotifyType( eNotifyOnline );
        break;

    case eUserViewTypeFriends:
        ui.m_FriendsLabel->setVisible( true );
        ui.m_FriendsView->setVisible( true );
        ui.m_FriendsView->setNotifyType( eNotifyOnline );
        break;

    case eUserViewTypeGroup:
        ui.m_GroupLabel->setVisible( true );
        ui.m_GroupHost->setVisible( true );
        ui.m_GroupHost->setNotifyType( eNotifyOnline );
        break;

    case eUserViewTypeChatRoom:
        ui.m_ChatRoomLabel->setVisible( true );
        ui.m_ChatRoomHost->setVisible( true );
        ui.m_ChatRoomHost->setNotifyType( eNotifyOnline );
        break;

    case eUserViewTypeRandomConnect:
        ui.m_RandomConnectLabel->setVisible( true );
        ui.m_RandomConnectHost->setVisible( true );
        ui.m_RandomConnectHost->setNotifyType( eNotifyOnline );
        break;

    default:
        break;
    }

    ui.m_SearchBarWidget->setSearchText( "" );
    ui.m_UserListWidget->setUserViewType( viewType );

    updateSelectionByViewType( viewType );
    updateUsersByViewType( viewType );

    emit signalViewChanged( viewType );
}

//============================================================================
void GuiUserMultiListWidget::setStatusMsg( QString strStatus )
{
	m_MyApp.toGuiStatusMessage( strStatus.toUtf8().constData() );
}

//============================================================================
void GuiUserMultiListWidget::slotEyeHostButtonClicked( void )
{
    if( ui.m_UserListWidget->isVisible() )
    {
        ui.m_UserListWidget->setVisible( false );
        ui.m_EyeUsers->setIcon( eMyIconEyeHide );
        ui.StyledDlgItemsBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
        this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    }
    else
    {
        ui.m_UserListWidget->setVisible( true );
        ui.m_EyeUsers->setIcon( eMyIconEyeShow );
        ui.StyledDlgItemsBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    }
}

//============================================================================
void GuiUserMultiListWidget::hideEyeSession( void )
{
    ui.m_EyeSession->setVisible( false );
}

//============================================================================
void GuiUserMultiListWidget::slotEyeSessionButtonClicked( void )
{
    if( m_SessionVisible )
    {
        m_SessionVisible = false;
        ui.m_EyeSession->setIcon( eMyIconEyeHide );
        emit signalSetSessionVisible( m_SessionVisible );
    }
    else
    {
        m_SessionVisible = true;
        ui.m_EyeSession->setIcon( eMyIconEyeShow );
        emit signalSetSessionVisible( m_SessionVisible );
    }
}

//============================================================================
void GuiUserMultiListWidget::slotEyeSearchButtonClicked( void )
{
    if( ui.m_SearchBarWidget->isVisible() )
    {
        ui.m_SearchBarWidget->setVisible( false );
        ui.m_EyeSearch->setIcon( eMyIconEyeSearchDisabled );
    }
    else
    {
        ui.m_SearchBarWidget->setVisible( true );
        ui.m_EyeSearch->setIcon( eMyIconEyeSearchEnabled );
    }
}

//============================================================================
void GuiUserMultiListWidget::slotEverybodyButtonClicked( void )
{
    setUserViewType( eUserViewTypeOnline );
}

//============================================================================
void GuiUserMultiListWidget::slotFriendsButtonClicked( void )
{
    setUserViewType( eUserViewTypeFriends );
}

//============================================================================
void GuiUserMultiListWidget::slotGroupHostButtonClicked( void )
{
    setUserViewType( eUserViewTypeGroup );
}

//============================================================================
void GuiUserMultiListWidget::slotChatRoomHostButtonClicked( void )
{
    setUserViewType( eUserViewTypeChatRoom );
}

//============================================================================
void GuiUserMultiListWidget::slotRandomConnectHostButtonClicked( void )
{
    setUserViewType( eUserViewTypeRandomConnect );
}

//============================================================================
void GuiUserMultiListWidget::slotUserSelected( GuiUser* guiUser )
{
	setSelectedUser( guiUser );
}

//============================================================================
void GuiUserMultiListWidget::slotSearchTextChanged( QString searchText )
{
    ui.m_UserListWidget->searchTextChanged( searchText );
}

//============================================================================
void GuiUserMultiListWidget::setSelectedUser( GuiUser* guiUser )
{
	m_SelectedUser = guiUser;

	onSelectedUserChanged( m_SelectedUser );
}

//============================================================================
void GuiUserMultiListWidget::onSelectedUserChanged( GuiUser* guiUser )
{
    ui.m_SentToWhoLabel->setVisible( false );
    ui.m_HostIconButton->setVisible( false );

    ui.m_IdentWidget->setVisible( true );	
    ui.m_IdentWidget->updateIdentity( guiUser );

    if( IsHostedMembersViewType( m_UserViewType ) )
    {
        switch( m_UserViewType )
        {
        case eUserViewTypeGroup:
            ui.m_HostIconButton->setNotifyType( eNotifyRelayed );
            break;
        case eUserViewTypeChatRoom:
            ui.m_HostIconButton->setNotifyType( eNotifyRelayed );
            break;
        case eUserViewTypeRandomConnect:
            ui.m_HostIconButton->setNotifyType( eNotifyRelayed );
            break;
        default:
            break;
        }
    }

    emit signalUserSelected( guiUser );
}

//============================================================================
void GuiUserMultiListWidget::userJoinedHost( GuiHosted* guiHosted )
{

}

//============================================================================
GuiUserListWidget* GuiUserMultiListWidget::getUserListWidget( void )
{
    return ui.m_UserListWidget;
}

//============================================================================
void GuiUserMultiListWidget::updateSelectionByViewType( EUserViewType viewType )
{
    ui.m_IdentWidget->clearIdentity();
    emit signalUserSelected( nullptr );
    if( IsHostedMembersViewType( viewType ) )
    {
        ui.m_IdentWidget->setVisible( false );
        ui.m_SentToWhoLabel->setVisible( true );
        ui.m_HostIconButton->setVisible( true );
        switch( viewType )
        {
        case eUserViewTypeGroup:
            ui.m_HostIconButton->setIcon( eMyIconGroupClient );
            break;
        case eUserViewTypeChatRoom:
            ui.m_HostIconButton->setIcon( eMyIconChatRoomClient );
            break;
        case eUserViewTypeRandomConnect:
            ui.m_HostIconButton->setIcon( eMyIconRandomConnectClient );
            break;
        default:
            break;
        }
    }
    else
    {
        ui.m_IdentWidget->setVisible( true );
        ui.m_SentToWhoLabel->setVisible( false );
        ui.m_HostIconButton->setVisible( false );
    }
}

//============================================================================
void GuiUserMultiListWidget::updateUsersByViewType( EUserViewType viewType )
{
    ui.m_UserListWidget->setUserViewType( viewType );
}