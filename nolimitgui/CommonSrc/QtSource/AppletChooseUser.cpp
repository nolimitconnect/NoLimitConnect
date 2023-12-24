//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChooseUser.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppSettings.h"

#include "GuiParams.h"
#include "GuiUserListWidget.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletChooseUser::AppletChooseUser( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_CHOOSE_USER, app, parent )
{
	setAppletType( eAppletChooseUser );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	// only update users using this applet
	ui.m_UserListWidget->disconnectUserUpdates();
	ui.m_UserListWidget->setUserViewType( eUserViewTypeEverybody );

	connect( this,						SIGNAL(signalBackButtonClicked()),												this, SLOT(closeApplet()) );

	connect( ui.m_UserListWidget,		SIGNAL(signalUserListItemClicked(GuiUserSessionBase*,GuiUserListItem*)),		this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalAvatarButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),		this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalFriendshipButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),	this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalOfferViewButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),		this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalOfferAcceptButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),	this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );

	connect( ui.m_UserListWidget,		SIGNAL(signalOfferRejectButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),	this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalPushToTalkButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),	this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalMenuButtonClicked(GuiUserSessionBase*,GuiUserListItem*)),			this, SLOT(slotUserSelected(GuiUserSessionBase*,GuiUserListItem*)) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletChooseUser::~AppletChooseUser()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletChooseUser::setChooseUserReason( EChooseUserReason chooseUserReason )   
{ 
	m_ChooseUserReason = chooseUserReason; 
	if( chooseUserReason == eChooseUserReasonChatRoomHost )
	{
		setTitleBarText( QObject::tr("Choose Chat Room Host") );
	}
}

//============================================================================
void AppletChooseUser::setChooseInstructionsText( QString instructionText )
{
	ui.m_ChooseLabel->setText( instructionText );
}

//============================================================================
void AppletChooseUser::addUser( VxGUID& onlineId )
{
	GuiUser* user = m_MyApp.getUserMgr().getUser( onlineId );
	if( user )
	{
		ui.m_UserListWidget->updateUser( user );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChooseUser::addUser null user %s", onlineId.toOnlineIdString().c_str() );
	}
}

//============================================================================
void AppletChooseUser::showEvent( QShowEvent* ev )
{
	AppletBase::showEvent( ev );
}

//============================================================================
void AppletChooseUser::slotUserSelected( GuiUserSessionBase* userSession, GuiUserListItem* userItem )
{
	if( userSession )
	{
		GuiUser* user = userSession->getUserIdent();
		if( user )
		{
			EHostType hostType{ eHostTypeUnknown };
			EApplet appletHostAdmin{ eAppletUnknown };
			EApplet appleHostJoin{ eAppletUnknown };

			switch( hostType )
			{
			case eChooseUserReasonGroupHost:
				hostType = eHostTypeGroup;
				appletHostAdmin = eAppletGroupHostAdmin;
				appleHostJoin = eAppletGroupJoin;
				break;

			case eChooseUserReasonChatRoomHost:
				hostType = eHostTypeChatRoom;
				appletHostAdmin = eAppletChatRoomHostAdmin;
				appleHostJoin = eAppletChatRoomJoin;
				break;

			case eChooseUserReasonRandomConnectHost:
				hostType = eHostTypeRandomConnect;
				appletHostAdmin = eAppletRandomConnectHostAdmin;
				appleHostJoin = eAppletRandomConnectJoin;
				break;

			default:
				return;
			}

			if( hostType != eHostTypeUnknown )
			{
				if( user->getMyOnlineId() == m_MyApp.getMyOnlineId() )
				{
					m_MyApp.getAppletMgr().launchApplet( appletHostAdmin, getParentPageFrame() );
				}
				else
				{
					if( m_MyApp.getUserJoinMgr().isUserJoinedToHost( hostType ) )
					{
						m_MyApp.getHostedListMgr().launchClientAppletOfAlreadyConnectedHost( hostType,
																							 m_MyApp.getUserJoinMgr().getUserJoinedHostOnlineId( hostType ),
																							 getParentPageFrame() );
					}
					else
					{
						// let user select a host to connect to
						m_MyApp.getAppletMgr().launchApplet( appleHostJoin, getParentPageFrame() );
					}
				}

				close();
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletChooseUser::slotUserSelected null user" );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChooseUser::slotUserSelected null userSession" );
	}
}