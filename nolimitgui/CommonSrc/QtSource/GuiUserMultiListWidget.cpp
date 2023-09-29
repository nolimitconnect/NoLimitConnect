//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
	ui.m_GroupHosts->setFixedSize( eButtonSizeSmall );
    ui.m_GroupHosts->setIcon( eMyIconEyeGroup );
	ui.m_ChatRoomHosts->setFixedSize(eButtonSizeSmall);
    ui.m_ChatRoomHosts->setIcon( eMyIconEyeChatRoom );
	ui.m_RandomConnectHosts->setFixedSize( eButtonSizeSmall );
    ui.m_RandomConnectHosts->setIcon( eMyIconEyeRandomConnect );
	ui.m_EverybodyView->setFixedSize( eButtonSizeSmall );
    ui.m_EverybodyView->setIcon( eMyIconEyeAll );
    ui.m_EverybodyView->setVisible( false );

	ui.m_EyeHosts->setFixedSize( eButtonSizeSmall );
    ui.m_EyeHosts->setIcon( eMyIconEyeShow );
	ui.m_EyeSession->setFixedSize( eButtonSizeSmall );
    ui.m_EyeSession->setIcon( eMyIconEyeShow );

    ui.m_EyeSearch->setFixedSize( eButtonSizeSmall );
    ui.m_EyeSearch->setIcon( eMyIconEyeSearchDisabled );
    ui.m_SearchBarWidget->setVisible( false );

    m_OffersFrame			= ui.m_OffersFrame;

    connect( ui.m_EyeHosts,		        SIGNAL(clicked()),						this,	SLOT(slotEyeHostButtonClicked()) );
    connect( ui.m_EyeSession,           SIGNAL(clicked()),						this,	SLOT(slotEyeSessionButtonClicked()) );
    connect( ui.m_EyeSearch,            SIGNAL(clicked()),                      this,   SLOT(slotEyeSearchButtonClicked()) );

    connect( ui.m_FriendsView,		    SIGNAL(clicked()),						this,	SLOT(slotFriendsButtonClicked()) );
    connect( ui.m_GroupHosts,	        SIGNAL(clicked()),						this,	SLOT(slotGroupHostButtonClicked()) );
    connect( ui.m_ChatRoomHosts,        SIGNAL(clicked()),						this,	SLOT(slotChatRoomHostButtonClicked()) );
    connect( ui.m_RandomConnectHosts,	SIGNAL(clicked()),	                    this,	SLOT(slotRandomConnectHostButtonClicked()) );

	connect( ui.m_UserListWidget,		SIGNAL(signalUserAvatarClicked(GuiUser*)), this, SLOT(slotUserSelected(GuiUser*)) );
    connect( ui.m_SearchBarWidget,      SIGNAL(signalSearchTextChanged(QString)), this, SLOT(slotSearchTextChanged(QString)) );

    ui.m_GroupHosts->setVisible( false );
    ui.m_ChatRoomHosts->setVisible( false );
    ui.m_RandomConnectHosts->setVisible( false );
}

//============================================================================
void GuiUserMultiListWidget::setUserViewType( EUserViewType viewType )
{
    ui.m_FriendsView->setNotifyOnlineEnabled( false );
    ui.m_GroupHosts->setNotifyOnlineEnabled( false );
    ui.m_ChatRoomHosts->setNotifyOnlineEnabled( false );
    ui.m_RandomConnectHosts->setNotifyOnlineEnabled( false );

    switch( viewType )
    {
    case eUserViewTypeFriends:
        ui.m_FriendsView->setVisible( true );
        ui.m_FriendsView->setNotifyOnlineEnabled( true );
        break;

    case eUserViewTypeGroup:
        ui.m_GroupHosts->setVisible( true );
        ui.m_GroupHosts->setNotifyOnlineEnabled( true );
        break;

    case eUserViewTypeChatRoom:
        ui.m_ChatRoomHosts->setVisible( true );
        ui.m_ChatRoomHosts->setNotifyOnlineEnabled( true );
        break;

    case eUserViewTypeRandomConnect:
        ui.m_RandomConnectHosts->setVisible( true );
        ui.m_RandomConnectHosts->setNotifyOnlineEnabled( true );
        break;

    default:
        break;
    }

    ui.m_SearchBarWidget->setSearchText( "" );
    ui.m_UserListWidget->setUserViewType( viewType );
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
        ui.m_EyeHosts->setIcon( eMyIconEyeHide );
        ui.StyledDlgItemsBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
        this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    }
    else
    {
        ui.m_UserListWidget->setVisible( true );
        ui.m_EyeHosts->setIcon( eMyIconEyeShow );
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
	ui.m_IdentWidget->updateIdentity( guiUser );
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