//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUserMultiListWidget.h"

#include "ActivityMessageBox.h"
#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppSettings.h"
#include "AppletMultiMessenger.h"
#include "AppletMgr.h"

#include "GuiHelpers.h"
#include "GuiMemberActiveMgr.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"

#include "MyIconsDefs.h"

#include <CoreLib/VxGlobals.h>

#include "ui_GuiUserMultiListWidget.h"

EUserViewType               GuiUserMultiListWidget::getUserViewType( void )						{ return ui.m_UserListWidget->getUserViewType(); };

//============================================================================
GuiUserMultiListWidget::GuiUserMultiListWidget(	QWidget* parent )
: QWidget( parent )
, ui(*(new Ui::GuiUserMultiListWidgetUi))
, m_MyApp( GetAppInstance() )
, m_MemberActiveMgr( m_MyApp.getMemberActiveMgr() )
{
    ui.setupUi( this );

	ui.m_IdentWidget->setupIdentLogic( eButtonSizeSmall );
    ui.m_AdminIdentWidget->setupIdentLogic( eButtonSizeSmall );
    ui.m_HostIconButton->setFixedSize( eButtonSizeSmall );

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
    connect( ui.m_EverybodyLabel,		SIGNAL(clicked()),						this,	SLOT(slotEverybodyButtonClicked()) );
    connect( ui.m_FriendsView,		    SIGNAL(clicked()),						this,	SLOT(slotFriendsButtonClicked()) );
    connect( ui.m_FriendsLabel,		    SIGNAL(clicked()),						this,	SLOT(slotFriendsButtonClicked()) );
    connect( ui.m_GroupHost,	        SIGNAL(clicked()),						this,	SLOT(slotGroupHostButtonClicked()) );
    connect( ui.m_GroupLabel,	        SIGNAL(clicked()),						this,	SLOT(slotGroupHostButtonClicked()) );
    connect( ui.m_ChatRoomHost ,        SIGNAL(clicked()),						this,	SLOT(slotChatRoomHostButtonClicked()) );
    connect( ui.m_ChatRoomLabel ,       SIGNAL(clicked()),						this,	SLOT(slotChatRoomHostButtonClicked()) );
    connect( ui.m_RandomConnectHost,	SIGNAL(clicked()),	                    this,	SLOT(slotRandomConnectHostButtonClicked()) );
    connect( ui.m_RandomConnectLabel,	SIGNAL(clicked()),	                    this,	SLOT(slotRandomConnectHostButtonClicked()) );

	connect( ui.m_UserListWidget,		SIGNAL(signalUserAvatarClicked(GuiUser*)), this, SLOT(slotUserSelected(GuiUser*)) );
    connect( ui.m_SearchBarWidget,      SIGNAL(signalSearchTextChanged(QString)), this, SLOT(slotSearchTextChanged(QString)) );

    bool isMemberOfGroup = m_MemberActiveMgr.isMemberOfHostType( eHostTypeGroup, m_MyApp.getMyOnlineId() );
    ui.m_GroupHost->setVisible( isMemberOfGroup );
    ui.m_GroupLabel->setVisible( isMemberOfGroup );
    bool isMemberOfChatRoom = m_MemberActiveMgr.isMemberOfHostType( eHostTypeChatRoom, m_MyApp.getMyOnlineId() );
    ui.m_ChatRoomHost->setVisible( isMemberOfChatRoom );
    ui.m_ChatRoomLabel->setVisible( isMemberOfChatRoom );
    bool isMemberOfRandConnect = m_MemberActiveMgr.isMemberOfHostType( eHostTypeRandomConnect, m_MyApp.getMyOnlineId() );
    ui.m_RandomConnectHost->setVisible( isMemberOfRandConnect );
    ui.m_RandomConnectLabel->setVisible( isMemberOfRandConnect );
    ui.m_AdminFrame->setVisible( false );
    m_MemberActiveMgr.wantMemberActiveCallback( this, true );
}

//============================================================================
GuiUserMultiListWidget::~GuiUserMultiListWidget( void )
{
    m_MemberActiveMgr.wantMemberActiveCallback( this, false );
}

//============================================================================
void GuiUserMultiListWidget::setHostAdminId( GroupieId& adminId )
{
    m_HostAdminId = adminId;
    GuiUser* guiUser = m_MyApp.getUserMgr().getUser( m_HostAdminId.getHostOnlineId() );
    if( guiUser )
    {
        ui.m_UserListWidget->setHostAdminId( adminId );
        ui.m_AdminIdentWidget->updateIdentity( guiUser );
        ui.m_AdminFrame->setVisible( true );
        setHostViewType( adminId.getHostType() );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiUserMultiListWidget::setHostAdminId null admin %s", m_MyApp.describeGroupieId( adminId ).c_str() );
        ui.m_AdminFrame->setVisible( false );
    }
}

//============================================================================
void GuiUserMultiListWidget::setAllowMyselfInList( bool allowMyself )
{
    ui.m_UserListWidget->setAllowMyselfInList( allowMyself );
}

//============================================================================
bool GuiUserMultiListWidget::getAllowMyselfInList( void )
{
    return ui.m_UserListWidget->getAllowMyselfInList();
}

//============================================================================
void GuiUserMultiListWidget::setHostViewType( EHostType hostType )
{
    switch( hostType )
    {
    case eHostTypeGroup:
        setUserViewType( eUserViewTypeGroup );
        break;
    case eHostTypeChatRoom:
        setUserViewType( eUserViewTypeChatRoom );
        break;
    case eHostTypeRandomConnect:
        setUserViewType( eUserViewTypeRandomConnect );
        break;
    default:
        break;
    }
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
    case eUserViewTypeOnline:
    case eUserViewTypeEverybody:
        ui.m_EverybodyLabel->setVisible( true );
        ui.m_EverybodyView->setVisible( true );
        ui.m_EverybodyView->setNotifyType( eNotifyOnline );
        break;

    case eUserViewTypeFriendsOnline:
        ui.m_FriendsLabel->setText( GuiParams::describeUserViewType( eUserViewTypeFriendsOnline ) );
        ui.m_FriendsLabel->setVisible( true );
        ui.m_FriendsView->setVisible( true );
        ui.m_FriendsView->setNotifyType( eNotifyOnline );
        break;

    case eUserViewTypeFriendsOffline:
        ui.m_FriendsLabel->setText( GuiParams::describeUserViewType( eUserViewTypeFriendsOffline ) );
        ui.m_FriendsLabel->setVisible( true );
        ui.m_FriendsView->setVisible( true );
        ui.m_FriendsView->setNotifyType( eNotifyAttention );
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
    if( eUserViewTypeFriendsOnline == getUserViewType() )
    {
        setUserViewType( eUserViewTypeFriendsOffline );
    }
    else
    {
        setUserViewType( eUserViewTypeFriendsOnline );
    }
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
    if( guiUser->isOnline() || 
        ( !guiUser->isOnline() && ( getUserViewType() == eUserViewTypeOffline || getUserViewType() == eUserViewTypeFriendsOffline ) ) )
    {
	    m_SelectedUser = guiUser;

	    onSelectedUserChanged( m_SelectedUser );
    }
}

//============================================================================
void GuiUserMultiListWidget::onSelectedUserChanged( GuiUser* guiUser )
{

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

    if( getHostAdminId().isValid() )
    {
        // launch messenger in opposite page and set the selected user
        AppletMultiMessenger* messenger = dynamic_cast<AppletMultiMessenger*>( m_MyApp.getAppletMgr().launchApplet( eAppletMultiMessenger, GuiHelpers::getOppositePageFrame( this ) ) );
        if( messenger )
        {
            messenger->setSelectedUser( guiUser );
        }
    }
    else
    {
        ui.m_SentToWhoLabel->setVisible( false );
        ui.m_HostIconButton->setVisible( false );

        ui.m_IdentWidget->setVisible( true );	
        ui.m_IdentWidget->updateIdentity( guiUser );
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
    bool hasAdmin = getHostAdminId().isValid();
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
        if( !hasAdmin )
        {
            ui.m_IdentWidget->setVisible( true );
            ui.m_SentToWhoLabel->setVisible( false );
            ui.m_HostIconButton->setVisible( false );
        }
    }
}

//============================================================================
void GuiUserMultiListWidget::updateUsersByViewType( EUserViewType viewType )
{
    ui.m_UserListWidget->setUserViewType( viewType );
}

//============================================================================
void GuiUserMultiListWidget::callbackGuiMemberIsJoinedToHost( VxGUID& onlineId, EHostType hostType, bool isJoined )
{
    if( onlineId != m_MyApp.getMyOnlineId() )
    {
        return;
    }

    switch( hostType )
    {
    case eHostTypeGroup:
        ui.m_GroupHost->setVisible( isJoined );
        ui.m_GroupLabel->setVisible( isJoined );
        break;

    case eHostTypeChatRoom:
        ui.m_ChatRoomHost->setVisible( isJoined );
        ui.m_ChatRoomLabel->setVisible( isJoined );
        break;

    case eHostTypeRandomConnect:
        ui.m_RandomConnectHost->setVisible( isJoined );
        ui.m_RandomConnectLabel->setVisible( isJoined );
        break;

    default:
        break;
    }
}