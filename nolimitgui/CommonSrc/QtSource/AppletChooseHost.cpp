//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChooseHost.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppSettings.h"

#include "GuiParams.h"
#include "GuiUserListWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletChooseHost.h"

//============================================================================
AppletChooseHost::AppletChooseHost( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_CHOOSE_HOST, app, parent )
, ui(*(new Ui::AppletChooseHostUi))
{
	setAppletType( eAppletChooseHost );
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
AppletChooseHost::~AppletChooseHost()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletChooseHost::setChooseUserReason( EChooseUserReason chooseUserReason )   
{ 
	m_ChooseUserReason = chooseUserReason; 
	if( chooseUserReason == eChooseUserReasonChatRoomHost )
	{
		setTitleBarText( QObject::tr("Choose Chat Room Host") );
	}
}

//============================================================================
void AppletChooseHost::setChooseInstructionsText( QString instructionText )
{
	ui.m_ChooseLabel->setText( instructionText );
}

//============================================================================
void AppletChooseHost::addUser( VxGUID& onlineId )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( guiUser )
	{
		updateUser( guiUser );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChooseHost::addUser null user %s", onlineId.toOnlineIdString().c_str() );
	}
}

//============================================================================
void AppletChooseHost::updateUser( GuiUser* guiUser )
{
	if( guiUser )
	{
		ui.m_UserListWidget->updateUser( guiUser );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChooseHost::addUser null user " );
	}
}

//============================================================================
void AppletChooseHost::showEvent( QShowEvent* ev )
{
	AppletBase::showEvent( ev );
}

//============================================================================
void AppletChooseHost::slotUserSelected( GuiUserSessionBase* userSession, GuiUserListItem* userItem )
{
	if( userSession )
	{
		GuiUser* guiUser = userSession->getUserIdent();
		if( guiUser )
		{
			EHostType hostType{ eHostTypeUnknown };
			EApplet appletHostAdmin{ eAppletUnknown };
			EApplet appleHostJoin{ eAppletUnknown };

            switch( getChooseUserReason() )
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

			case eChooseUserReasonTest:
				LogMsg( LOG_VERBOSE, "AppletChooseHost::slotUserSelected %s", guiUser->describeUser().toUtf8().constData() );
				emit signalUserChoosen( guiUser->getMyOnlineId() );
				closeApplet();
				return;

			default:
				return;
			}

			if( hostType != eHostTypeUnknown )
			{
				if( guiUser->getMyOnlineId() == m_MyApp.getMyOnlineId() )
				{
					m_MyApp.getAppletMgr().launchApplet( appletHostAdmin, getParentPageFrame() );
				}
				else
				{
					HostedId adminId( guiUser->getMyOnlineId(), hostType );
					if( guiUser->isOnline() && m_MyApp.getUserJoinMgr().isUserJoinedToHost( adminId ) )
					{
						m_MyApp.getHostedListMgr().launchClientAppletOfAlreadyConnectedHost( adminId,
																							 getParentPageFrame() );
					}
					else
					{
						// let user select a host to connect to
						m_MyApp.getAppletMgr().launchApplet( appleHostJoin, getParentPageFrame() );
					}
				}

				closeApplet();
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletChooseHost::slotUserSelected null user" );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChooseHost::slotUserSelected null userSession" );
	}
}
