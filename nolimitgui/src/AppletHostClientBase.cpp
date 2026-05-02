//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostClientBase.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "AssetSendMgr.h"
#include "GuiHelpers.h"
#include "GuiMemberActiveMgr.h"
#include "VxProgressBar.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>
#include <QLabel>
#include <QTimer>

#include "ui_AppletHostClient.h"

namespace
{
ELogModule HostTypeToLogModule( EHostType hostType )
{
	switch( hostType )
	{
	case eHostTypeGroup:
		return eLogGroup;

	case eHostTypeChatRoom:
		return eLogChatRoom;

	case eHostTypeRandomConnect:
		return eLogRandomConnect;

	default:
		return eLogNone;
	}
}

} // namespace

//============================================================================
AppletHostClientBase::AppletHostClientBase( const char* objName, AppCommon& app, EApplet applet, EHostType hostType, EPluginType pluginType, QWidget* parent )
: AppletClientBase( objName, app, parent )
, ui( *( new Ui::AppletHostClientUi ) )
{
	setAppletType( applet );
	setHostType( hostType );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( pluginType );

	ui.m_SessionWidget->setPluginType( getPluginType() );
	ui.m_SessionWidget->setInputClientCallback( this );

	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );
	connect( ui.m_UserListWidget, SIGNAL(signalSetMembersVisible(bool)), this, SLOT(slotSetMembersVisible(bool)) );
	connect( ui.m_UserListWidget, SIGNAL(signalSetSessionVisible(bool)), this, SLOT(slotSetSessionVisible(bool)) );
	connect( ui.m_UserListWidget, SIGNAL(signalViewChanged(EUserViewType)), this, SLOT(slotViewChanged(EUserViewType)) );
	connect( ui.m_UserListWidget, SIGNAL(signalLeftHost()), this, SLOT(closeApplet()) );

    connect( ui.m_UserListWidget, SIGNAL(signalUserSelected(GuiUser*)), this, SLOT(slotUserSelected(GuiUser*)) );

    // Connect to AssetSendMgr for multi-send progress updates
    connect( &m_MyApp.getAssetSendMgr(), &AssetSendMgr::signalSendingToMember, this, &AppletHostClientBase::slotSendingToMember );
    connect( &m_MyApp.getAssetSendMgr(), &AssetSendMgr::signalMultiSendComplete, this, &AppletHostClientBase::slotMultiSendComplete );

	// Restore eye button states for this applet type
	bool eyeUsersVisible = m_MyApp.getAppSettings().getAppletEyeUsersVisible( applet );
	ui.m_UserListWidget->setMembersVisible( eyeUsersVisible );
	bool eyeSessionVisible = m_MyApp.getAppSettings().getAppletEyeSessionVisible( applet );
	ui.m_UserListWidget->setSessionsVisible( eyeSessionVisible );
	ui.m_SessionWidget->setVisible( eyeSessionVisible );
    ui.m_SessionWidget->setCanSendInterface( this );

	m_MyApp.activityStateChange( this, true );

	// Recover host context when this applet is reopened while already joined.
	GroupieId joinedAdminGroupieId = m_MyApp.getUserJoinMgr().getJoinedAdminGroupieId( hostType );
	if( joinedAdminGroupieId.isValid() )
	{
		setAdminGroupieId( joinedAdminGroupieId );
	}
}

//============================================================================
AppletHostClientBase::~AppletHostClientBase()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostClientBase::userJoinedHost( GuiHosted* guiHosted )
{
	if( guiHosted && guiHosted->getHostType() == m_HostType )
	{
		GuiUser* adminUser = guiHosted->getUser();
        // if we are the host then should view from admin applet instead of client applet
		if( adminUser && adminUser->getMyOnlineId() != m_MyApp.getMyOnlineId() )
		{
			HostedId adminId( adminUser->getMyOnlineId(), guiHosted->getHostType() );
			GroupieId adminGroupieId( adminUser->getMyOnlineId(), adminId );
			if( adminId.isValid() )
			{
                setAdminGroupieId( adminGroupieId );
				AppletClientBase::userJoinedHost( guiHosted );
			}
		}
	}
}

//============================================================================
void AppletHostClientBase::setAdminGroupieId( GroupieId& adminGroupieId )
{
    if( !adminGroupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "AppletHostClientBase::%s invalid adminGroupieId", __func__ );
        return;
    }

    if(adminGroupieId.getHostType() != m_HostType )
    {
        LogMsg( LOG_ERROR, "AppletHostClientBase::%s invalid adminGroupieId host type: %d does not match applet host type: %d", __func__, adminGroupieId.getHostType(), m_HostType );
        return;
    }

    if(adminGroupieId.getHostOnlineId() == m_MyApp.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "AppletHostClientBase::%s adminGroupieId host online id cannot be myself", __func__ );
        return;
    }

    // originally we set ourself as the user id of the admin. now we set it to the actual admin id and then the user id is used as selected user to send to.
	m_AdminGroupieId = adminGroupieId;
    m_AdminGroupieId.setUserOnlineId( adminGroupieId.getHostOnlineId() );

	LogMsg( LOG_VERBOSE, "AppletChatRoomClient::%s %s", __func__, m_MyApp.describeGroupieId( m_AdminGroupieId ).c_str() );
	ui.m_UserListWidget->setHostAdminId( m_AdminGroupieId );
	ui.m_SessionWidget->setHostAdminId( m_AdminGroupieId );

	ELogModule hostLogModule = HostTypeToLogModule( m_AdminGroupieId.getHostType() );
	if( hostLogModule != eLogNone && LogEnabled( hostLogModule ) )
	{
		m_MyApp.getConnectIdListMgr().dumpOnlineUsers();
		m_MyApp.getConnectIdListMgr().dumpHostedUsers( m_AdminGroupieId.getHostedId() );
	}

	GuiUser* adminUser = m_MyApp.getUserMgr().getUser( m_AdminGroupieId.getHostOnlineId() );
	if( adminUser )
	{
		if( !adminUser->isOnline() )
		{
			LogMsg( LOG_ERROR, "AppletChatRoomClient::%s admin user is offline", __func__ );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChatRoomClient::%s failed to find admin", __func__ );
	}
}

//============================================================================
void AppletHostClientBase::showEvent( QShowEvent* ev )
{
	ActivityBase::showEvent( ev );
}

//============================================================================
GroupieId AppletHostClientBase::getActiveAdminGroupieId( void )
{
	GroupieId adminGroupieId = ui.m_UserListWidget->getHostAdminId();
	if( adminGroupieId.isValid() )
	{
		return adminGroupieId;
	}

	if( m_AdminGroupieId.isValid() )
	{
		ui.m_UserListWidget->setHostAdminId( m_AdminGroupieId );
		ui.m_SessionWidget->setHostAdminId( m_AdminGroupieId );
		return m_AdminGroupieId;
	}

	GroupieId joinedAdminGroupieId = m_MyApp.getUserJoinMgr().getJoinedAdminGroupieId( m_HostType );
	if( joinedAdminGroupieId.isValid() )
	{
		setAdminGroupieId( joinedAdminGroupieId );
		return joinedAdminGroupieId;
	}

	return adminGroupieId;
}

//============================================================================
void AppletHostClientBase::slotSetSessionVisible( bool visible )
{
	m_MyApp.getAppSettings().setAppletEyeSessionVisible( m_EAppletType, visible );
	ui.m_SessionWidget->setVisible( visible );
}

//============================================================================
void AppletHostClientBase::slotSetMembersVisible( bool visible )
{
	m_MyApp.getAppSettings().setAppletEyeUsersVisible( m_EAppletType, visible );

	if( visible )
	{
		ui.m_UserListWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		ui.verticalLayout->setStretch( 0, 1 );
	}
	else
	{
		ui.m_UserListWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
		ui.verticalLayout->setStretch( 0, 0 );
	}

	ui.verticalLayout->setStretch( 1, 1 );
	ui.m_UserListWidget->updateGeometry();
	ui.m_SessionWidget->updateGeometry();
	ui.verticalLayout->invalidate();
}

//============================================================================
void AppletHostClientBase::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}

//============================================================================
bool AppletHostClientBase::checkIfCanSend( void )
{
	GroupieId adminGroupieId = getActiveAdminGroupieId();
	return AppletBase::checkIfCanSend( adminGroupieId.getHostedId() );
}

//============================================================================
bool AppletHostClientBase::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	GroupieId groupieId = getActiveAdminGroupieId();
    if( m_SelectedUser )
    {
        LogModule( eLogAssets, LOG_VERBOSE, "AppletHostClientBase::%s assetAction: %d for selected user: %s", __func__, 
            assetAction, m_SelectedUser->getOnlineName().c_str());
        groupieId.setUserOnlineId( m_SelectedUser->getMyOnlineId() );
    }

	if(LogEnabled(eLogAssets))LogModule( eLogAssets, LOG_VERBOSE,
		"AppletHostClientBase::%s action %s host %s admin %s selected %s creator %s assetId %s assetType %s assetName %s",
		__func__,
		DescribeAssetAction( assetAction ),
		DescribeHostType( groupieId.getHostType() ),
		m_MyApp.getUserName( groupieId.getHostOnlineId() ).c_str(),
		m_MyApp.getUserName( groupieId.getUserOnlineId() ).c_str(),
		m_MyApp.getUserName( assetInfo.getCreatorId() ).c_str(),
		assetInfo.getAssetUniqueId().toHexString().c_str(),
		DescribeAssetType( assetInfo.getAssetType() ),
		assetInfo.getAssetName().c_str() );

    m_SendToGroupieAdminId = groupieId;

	return m_MyApp.getAssetSendMgr().handleGroupieAssetAction( this, m_SendToGroupieAdminId, assetAction, assetInfo );
}

//============================================================================
void AppletHostClientBase::slotUserSelected( GuiUser* guiUser )
{
    m_SelectedUser = guiUser;
}

//============================================================================
void AppletHostClientBase::slotSendingToMember( VxGUID assetId, VxGUID memberId, QString memberName )
{
    ui.m_SessionWidget->sendingToMember( assetId, memberId, memberName );
	const QString progressText = QObject::tr( "Sending to: %1" ).arg( memberName );

    QLabel* statusLabel = ui.m_SessionWidget->getSessionStatusLabel();
    if( statusLabel )
    {
		statusLabel->setText( progressText );
        statusLabel->setVisible( true );
    }
}

//============================================================================
void AppletHostClientBase::slotMultiSendComplete( VxGUID assetId, bool allSucceeded, int successCount, int failCount )
{
    ui.m_SessionWidget->multiSendComplete( assetId, allSucceeded, successCount, failCount );
    QLabel* statusLabel = ui.m_SessionWidget->getSessionStatusLabel();
    if( statusLabel )
    {
        if( allSucceeded )
        {
            statusLabel->setText( QObject::tr( "Sent to %1 member(s)" ).arg( successCount ) );
        }
        else
        {
            statusLabel->setText( QObject::tr( "Sent: %1 success, %2 failed" ).arg( successCount ).arg( failCount ) );
        }
        // Clear status after a delay
        QTimer::singleShot( 3000, statusLabel, [statusLabel]() {
            statusLabel->setVisible( false );
        } );
    }
}

//============================================================================
ECanSendState AppletHostClientBase::getCanSendState( void )
{
	GroupieId adminGroupieId = getSendToAdminGroupieId();

    std::set<VxGUID> sendToSet;
    ECanSendState canSendState = m_MyApp.getAssetSendMgr().getSendToSet( adminGroupieId, sendToSet );
    // show user popup if we cannot send to anyone
    if( canSendState != ECanSendState::eCanSend )
    {
        GuiHelpers::showCannotSendReason( canSendState );
    }

    return canSendState;        
}

//============================================================================
GroupieId AppletHostClientBase::getSendToAdminGroupieId()
{
    if( !m_SendToGroupieAdminId.isValid() )
    {
        m_SendToGroupieAdminId = getActiveAdminGroupieId();
    }
    
    return m_SendToGroupieAdminId;
}