//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletJoinBase.h"

#include "AppCommon.h"
#include "AppletClientBase.h"
#include "AppletMgr.h"
#include "AppletMultiMessenger.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiHostedListSession.h"
#include "GuiHostedListItem.h"

#include <ptop_src/ptop_engine_src/UserJoinMgr/UserJoinMgr.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxPtopUrl.h>

//============================================================================
AppletJoinBase::AppletJoinBase( const char*name, AppCommon& app, QWidget* parent )
: AppletBase( name, app, parent )
{
    m_NetworkHostUrl = m_MyApp.getFromGuiInterface().fromGuiQueryDefaultUrl( eHostTypeNetwork );
    VxPtopUrl netHostUrl( m_NetworkHostUrl );
    m_NetHostPtopUrl = netHostUrl;
    if( m_NetHostPtopUrl.isValid() )
    {
        m_NetworkHostOnlineId = m_NetHostPtopUrl.getOnlineId();
    }

	m_JoinedHostSession.initializeWithNewVxGUID();

	ui.setupUi( getContentItemsFrame() );

	ui.m_GuiHostedListWidget->setIsHostView( false );
	ui.m_GuiGroupieListWidget->setIsHostView( false );

	// so is actually destroyed
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );
	connect( ui.m_ChooseHostButton, SIGNAL( clicked() ), this, SLOT( slotChooseHostModeButtonClick() ) );
	connect( ui.m_IgnoredHostsButton, SIGNAL( clicked() ), this, SLOT( slotShowIgnoredHostsListButtonClicked() ) );

	connect( ui.m_GuiHostedListWidget, SIGNAL( signalIconButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ), this, SLOT( slotIconButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalMenuButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ), this, SLOT( slotMenuButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalJoinButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ), this, SLOT( slotJoinButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalConnectButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ), this, SLOT( slotConnectButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalKickButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ), this, SLOT( slotKickButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalIgnoreButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ), this, SLOT( slotIgnoreButtonClicked( GuiHostedListSession*, GuiHostedListItem* ) ) );

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
	m_MyApp.getHostedListMgr().wantHostedListCallbacks( this, true );
	m_MyApp.getGroupieListMgr().wantGroupieListCallbacks( this, true );
	m_MyApp.getHostJoinMgr().wantHostJoinCallbacks( this, true );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, true );

	m_UserListMode = true;
	changeGuiMode( false );
}

//============================================================================
AppletJoinBase::~AppletJoinBase()
{
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, false );
	m_MyApp.getGroupieListMgr().wantGroupieListCallbacks( this, false );
	m_MyApp.getHostedListMgr().wantHostedListCallbacks( this, false );
	m_MyApp.getHostJoinMgr().wantHostJoinCallbacks( this, false );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, false );
	m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletJoinBase::setHostType( EHostType hostType ) 
{ 
	m_HostType = hostType; 
	if( m_UserListMode )
	{
		setListLabel( QObject::tr( "User List" ) );
		ui.m_ChooseHostButton->setVisible( true );
		ui.m_HostAdminLabel->setVisible( true );
		ui.m_HostAdminIdentWidget->setVisible( true );

		ui.m_GuiHostedListWidget->setVisible( false );
		ui.m_GuiGroupieListWidget->setVisible( true );
	}
	else
	{
		setListLabel( GuiParams::describeHostType( hostType ) + QObject::tr( "List" ) );
		ui.m_ChooseHostButton->setVisible( false );
		ui.m_HostAdminLabel->setVisible( false );
		ui.m_HostAdminIdentWidget->setVisible( false );

		ui.m_GuiHostedListWidget->setVisible( true );
		ui.m_GuiGroupieListWidget->setVisible( false );
	}

	ui.m_GuiHostedListWidget->setHostType( m_HostType );
	ui.m_GuiGroupieListWidget->setHostType( m_HostType );

	queryHostedList();
}

//============================================================================
void AppletJoinBase::queryHostedList( void )
{
	if( isNetworkHostUrlValid() )
	{
		VxGUID nullGuid;
		m_MyApp.getFromGuiInterface().fromGuiQueryHostListFromNetworkHost( m_NetHostPtopUrl, getHostType(), nullGuid );
	}
	else
	{
		okMessageBox( QObject::tr( "Network Host Url Is Invalid" ), QObject::tr( "You need to set a valid Network Host URL or IP:Port in network settings" ) );
	}
}

//============================================================================
void AppletJoinBase::slotChooseHostModeButtonClick( void )
{
	changeGuiMode( false );
}

//============================================================================
void AppletJoinBase::changeGuiMode( bool userListMode )
{
	// if( userListMode != m_UserListMode )
	{
		m_UserListMode = userListMode;
		if( userListMode )
		{
			setListLabel( QObject::tr( "User List" ) );
			ui.m_ChooseHostButton->setVisible( true );
			ui.m_HostAdminLabel->setVisible( true );
			ui.m_HostAdminIdentWidget->setVisible( true );

			ui.m_GuiHostedListWidget->setVisible( false );
			ui.m_GuiGroupieListWidget->setVisible( true );
		}
		else
		{
			setListLabel( QObject::tr( "Host List" ) );
			ui.m_ChooseHostButton->setVisible( false );
			ui.m_HostAdminLabel->setVisible( false );
			ui.m_HostAdminIdentWidget->setVisible( false );

			ui.m_GuiHostedListWidget->setVisible( true );
			ui.m_GuiGroupieListWidget->setVisible( false );
		}
	}
}

//============================================================================
void AppletJoinBase::setStatusMsg( QString statusMsg )
{
	ui.m_StatusLabel->setText( statusMsg );
}

//============================================================================
void AppletJoinBase::setListLabel( QString labelText )
{
	ui.m_HostOrListViewLabel->setText( labelText );
}

//============================================================================
void AppletJoinBase::callbackUserAdded( GuiUser* guiUser )
{
	updateUser( guiUser );
}

//============================================================================
void AppletJoinBase::callbackUserUpdated( GuiUser* guiUser )
{
	updateUser( guiUser );
}

//============================================================================
void AppletJoinBase::callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId )
{
	if( hostedId.getHostType() == m_HostType && guiHosted )
	{
		ui.m_GuiHostedListWidget->updateHostedList( hostedId, guiHosted, sessionId );
		GroupieId groupieId( m_MyApp.getMyOnlineId(), hostedId );
		if( !m_UserListMode && m_MyApp.getUserJoinMgr().getUserJoinState( groupieId ) == eJoinStateJoinIsGranted )
		{
			onJoinedHost( groupieId, guiHosted );
		}
	}
}

//============================================================================
void AppletJoinBase::callbackGuiHostedListSearchComplete( EHostType hostType, VxGUID& sessionId )
{
	setStatusMsg( QObject::tr( "Host list from network host completed" ) );
}

//============================================================================
void AppletJoinBase::callbackGuiGroupieListSearchResult( GroupieId& groupieId, GuiGroupie* guiGroupie, VxGUID& sessionId )
{
	if( groupieId.getHostType() == m_HostType && guiGroupie )
	{
		ui.m_GuiGroupieListWidget->updateGroupieList( groupieId, guiGroupie, sessionId );
	}
}

//============================================================================
void AppletJoinBase::callbackGuiGroupieListSearchComplete( EHostType hostType, VxGUID& sessionId )
{
	setStatusMsg( QObject::tr( "User list from host completed" ) );
}

//============================================================================
void AppletJoinBase::slotIconButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::slotIconButtonClicked" );

	GuiHosted* guiHosted = hostSession->getGuiHosted();

	if( guiHosted && guiHosted->readyForClientLaunch() )
	{
		launchClientApplet( guiHosted );
	}
}

//============================================================================
bool AppletJoinBase::launchClientApplet( GuiHosted* guiHosted )
{
	if( guiHosted && guiHosted->readyForClientLaunch() )
	{
		if( eHostTypeGroup == guiHosted->getHostType() )
		{
			AppletMultiMessenger* messenger = m_MyApp.getAppletMultiMessenger();
			if( messenger )
			{
				messenger->userJoinedHost( guiHosted );
				return true;
			}
		}
		else
		{
			AppletClientBase* clientApplet{ nullptr };
            switch( guiHosted->getHostType() )
            {
            case eHostTypeChatRoom:
                if( !m_MyApp.getAppletMgr().isAppletLaunched( eAppletChatRoomClient ) )
                {
                    clientApplet = dynamic_cast<AppletClientBase*>(m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomClient, getParentPageFrame() ));
                }

                break;
            case eHostTypeRandomConnect:
                if( !m_MyApp.getAppletMgr().isAppletLaunched( eAppletRandomConnectClient ) )
                {
                    clientApplet = dynamic_cast<AppletClientBase*>(m_MyApp.getAppletMgr().launchApplet( eAppletRandomConnectClient, getParentPageFrame() ));
                }

                break;
            default:
                break;
            }

            if( clientApplet )
            {
                clientApplet->userJoinedHost( guiHosted );
                return true;
            }
		}
	}
	else
	{
		GuiHelpers::errorMsgBox( eErrMsgNotConnectedToHost, this );
	}

	return false;
}

//============================================================================
void AppletJoinBase::slotMenuButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::slotMenuButtonClicked" );
}

//============================================================================
void AppletJoinBase::slotJoinButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	std::string ptopUrlIpv4 = hostSession->getHostUrl(false);
	std::string ptopUrlIpv6 = hostSession->getHostUrl(true);
	std::string joinUrl = ptopUrlIpv4.empty() ? ptopUrlIpv6 : ptopUrlIpv4;
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::slotJoinButtonClicked url %s", joinUrl.c_str() );
	if( !joinUrl.empty() )
	{
		m_MyApp.getUserJoinMgr().setLastJoinAttempted( joinUrl );
	}

	m_MyApp.getFromGuiInterface().fromGuiJoinHost( hostSession->getHostType(), hostSession->getSessionId(), ptopUrlIpv4, ptopUrlIpv6 );
}

//============================================================================
void AppletJoinBase::slotConnectButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::slotConnectButtonClicked" );
	std::string ptopUrlIpv4 = hostSession->getHostUrl(false);
	std::string ptopUrlIpv6 = hostSession->getHostUrl(true);
	GroupieId groupieId( m_MyApp.getMyOnlineId(), hostSession->getHostedId() );
	GuiUserJoin* userJoin = m_MyApp.getUserJoinMgr().getUserJoin( groupieId );
	EJoinState joinState{ eJoinStateNone };
	if( userJoin )
	{
		joinState = userJoin->getJoinState();
	}

	if( joinState == eJoinStateJoinIsGranted )
	{
		m_MyApp.getFromGuiInterface().fromGuiLeaveHost( hostSession->getHostType(), hostSession->getSessionId(), ptopUrlIpv4, ptopUrlIpv6 );
	}
	else
	{
		m_MyApp.getFromGuiInterface().fromGuiJoinHost( hostSession->getHostType(), hostSession->getSessionId(), ptopUrlIpv4, ptopUrlIpv6 );
	}
}

//============================================================================
void AppletJoinBase::slotKickButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::slotKickButtonClicked" );
	if( yesNoMessageBox( QObject::tr( "Revoke Membership" ), QObject::tr( "Are You Sure You Want To Revoke Membership?" ) ) )
	{
		std::string ptopUrlIpv4 = hostSession->getHostUrl(false);
		std::string ptopUrlIpv6 = hostSession->getHostUrl(true);
		m_MyApp.getFromGuiInterface().fromGuiUnJoinHost( hostSession->getHostType(), hostSession->getSessionId(), ptopUrlIpv4, ptopUrlIpv6 );
	}
}

//============================================================================
void AppletJoinBase::slotIgnoreButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::slotIgnoreButtonClicked" );
	GuiHosted* guiHosted = hostSession->getGuiHosted();
	if( !guiHosted )
	{
		return;
	}

	if( m_MyApp.getEngine().getIgnoreListMgr().addHostIgnore( guiHosted->getHostOnlineId(), guiHosted->getHostInviteUrl(false), guiHosted->getHostInviteUrl(true),
		guiHosted->getHostTitle(), guiHosted->getThumbId(), guiHosted->getHostDescription() ) )
	{
		ui.m_GuiHostedListWidget->removeItemWidget( hostItem );
	}
}

//============================================================================
void AppletJoinBase::slotShowIgnoredHostsListButtonClicked( void )
{
	if( !m_MyApp.getEngine().getIgnoreListMgr().hasIgnoredHosts() )
	{
		okMessageBox( QObject::tr( "No Ignored Hosts" ), QObject::tr( "The Ignored Hosts List Is Empty" ) );
		return;
	}

	m_MyApp.launchApplet( eAppletIgnoredHosts, getParentPageFrame() );
}

//============================================================================
void AppletJoinBase::callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostJoinRequested %s %s", guiHostJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostJoinRequested( groupieId, guiHostJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostJoinGrante %s %s", guiHostJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostJoinWasGranted( groupieId, guiHostJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostJoinIsGranted %s %s", guiHostJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostJoinIsGranted( groupieId, guiHostJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostUnJoinGranted %s %s", guiHostJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostUnJoinGranted( groupieId, guiHostJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostJoinDenied %s %s", guiHostJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostJoinDenied( groupieId, guiHostJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiHostJoinLeaveHost( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostJoinLeaveHost %s", m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostJoinLeaveHost( groupieId );
}

//============================================================================
void AppletJoinBase::callbackGuiHostUnJoin( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostUnJoin %s", m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostUnJoin( groupieId );
}

//============================================================================
void AppletJoinBase::callbackGuiHostJoinRemoved( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiHostJoinRemoved %s", m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiHostJoinRemoved( groupieId );
}

//============================================================================
//============================================================================

//============================================================================
void AppletJoinBase::callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserJoinRequested %s %s", guiUserJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserJoinRequested( groupieId, guiUserJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserJoinWasGranted %s %s", guiUserJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserJoinWasGranted( groupieId, guiUserJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserJoinGranted %s %s", guiUserJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserJoinIsGranted( groupieId, guiUserJoin );
	if( guiUserJoin && guiUserJoin->getUser() )
	{
		m_AdminGroupieId = groupieId;
		if( guiUserJoin->getMyOnlineId() == m_AdminGroupieId.getHostOnlineId() )
		{
			ui.m_HostAdminIdentWidget->updateIdentity( guiUserJoin->getUser(), true );
		}

		ui.m_GuiGroupieListWidget->addOrUpdateUser( groupieId, guiUserJoin );
	}

	GuiHosted* guiHosted = ui.m_GuiHostedListWidget->findGuiHostedByHostId( groupieId.getHostedId() );
	onJoinedHost( groupieId, guiHosted );
}

//============================================================================
void AppletJoinBase::callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserUnJoinGranted %s %s", guiUserJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserUnJoinGranted( groupieId, guiUserJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserJoinDenied %s %s", guiUserJoin->getOnlineName().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserJoinDenied( groupieId, guiUserJoin );
}

//============================================================================
void AppletJoinBase::callbackGuiUserJoinLeaveHost( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserJoinLeaveHost %s", m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserJoinLeaveHost( groupieId );
}

//============================================================================
void AppletJoinBase::callbackGuiUserJoinRemoved( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::callbackGuiUserJoinRemoved %s", m_MyApp.describeGroupieId( groupieId ).c_str() );
	ui.m_GuiHostedListWidget->callbackGuiUserJoinLeaveHost( groupieId );
}

//============================================================================
void AppletJoinBase::updateUser( GuiUser* guiUser )
{
	LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::updateUser %s id %s",
		guiUser->getOnlineName().c_str(), guiUser->getMyOnlineId().describeVxGUID().c_str() );

	ui.m_GuiHostedListWidget->updateUser( guiUser );
	ui.m_GuiGroupieListWidget->updateUser( guiUser );

	if( guiUser->getMyOnlineId() == m_AdminGroupieId.getHostOnlineId() )
	{
		ui.m_HostAdminIdentWidget->updateIdentity( guiUser, true );
	}
}

//============================================================================
void AppletJoinBase::onJoinedHost( GroupieId& groupieId, GuiHosted* guiHosted )
{
	if( !m_UserListMode )
	{
		m_AdminGroupieId = groupieId;
		// find and fill in the host admin
		LogModule( eLogUserGuiEvent, LOG_VERBOSE, "AppletJoinBase::onJoinedHost Admin title %s groupie %s",
			guiHosted->getHostTitle().c_str(), m_MyApp.describeGroupieId( groupieId ).c_str() );

		if( launchClientApplet( guiHosted ) )
		{
			// closeApplet();
		}
	}
}
