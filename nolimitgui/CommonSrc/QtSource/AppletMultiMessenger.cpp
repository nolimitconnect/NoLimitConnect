//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletMultiMessenger.h"

#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppSettings.h"
#include "ActivityMessageBox.h"

#include "GuiHelpers.h"
#include "GuiMemberActiveMgr.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"
#include "GuiPluginMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <P2PEngine/EngineSettings.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

//============================================================================
AppletMultiMessenger::AppletMultiMessenger(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_APPLET_MULTI_MESSENGER, app, parent )
, m_TodGameLogic( app, app.getEngine(), ePluginTypeMessenger, this )
{
    setPluginType( ePluginTypeMessenger );
    setAppletType( eAppletMultiMessenger );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	ui.m_SessionWidget->setAppModule( eAppModuleMessenger );
	ui.m_SessionWidget->setPluginType( getPluginType() );
	ui.m_SessionWidget->setInputClientCallback( this );

	ui.m_UserListWidget->setUserViewType( eUserViewTypeFriendsOnline );

    m_ResponseFrame			= ui.m_ResponseFrame;
    m_HangupSessionFrame	= ui.m_HangupSessionFrame;
    m_VidChatWidget			= ui.m_VidWidget;

    m_ResponseFrame->setVisible( false );
    m_HangupSessionFrame->setVisible( false );
    m_VidChatWidget->setVisible( false );
    m_VidChatWidget->setRecordFilePath( VxGetDownloadsDirectory().c_str() );
    ui.m_SessionWidget->setIsPersonalRecorder( false );

    ui.m_SessionWidget->setEntryMode( eAssetTypeUnknown );
	ui.m_SessionWidget->setAppModule( eAppModuleMessenger );
	ui.m_SessionWidget->setPluginType( ePluginTypeMessenger );
	ui.m_SessionWidget->setInputClientCallback( this );

    m_TodGameLogic.setVisible( false );

    connect( ui.m_SessionWidget,	    SIGNAL(signalUserInputButtonClicked()),		this,	SLOT(slotUserInputButtonClicked()) );

	connect( ui.m_UserListWidget,		SIGNAL(signalUserSelected(GuiUser*)),		this,	SLOT(slotUserSelected(GuiUser*)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalSetSessionVisible(bool)),		this,	SLOT(slotSetSessionVisible(bool)));
	connect( ui.m_UserListWidget,		SIGNAL(signalViewChanged(EUserViewType)),  this,	SLOT(slotViewChanged(EUserViewType)));
	
    m_MyApp.activityStateChange( this, true ); 
	m_MyApp.getOfferMgr().wantGuiOfferCallbacks( this, true );
	
	manageUsers( ui.m_UserListWidget );

    m_MyApp.onMessengerReady( true );
}

//============================================================================
AppletMultiMessenger::~AppletMultiMessenger()
{
	m_MyApp.onMessengerReady( false );
	m_MyApp.getOfferMgr().wantGuiOfferCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletMultiMessenger::setupMultiSessionActivity( GuiUser* hisIdent )
{
	m_HisIdent = hisIdent;

	m_VidChatWidget->setVideoFeedId( m_HisIdent->getMyOnlineId(), eAppModuleMessenger );
	m_TodGameWidget->getVidWidget()->setVideoFeedId( m_HisIdent->getMyOnlineId(), eAppModuleMessenger );

	m_VidChatWidget->setRecordFriendName( m_HisIdent->getOnlineName().c_str() );

    GroupieId groupieId( hisIdent->getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypePeerUser );

    ui.m_SessionWidget->setGroupieId( groupieId );

	/*
	m_TodGameLogic.setGuiWidgets( m_HisIdent, m_TodGameWidget );


	MultiSessionState * sessionState = new MultiSessionState( m_MyApp, *this, eMSessionTypePhone );
	sessionState->setGuiWidgets(	hisIdent,
									m_OffersFrame,
									m_ResponseFrame,
									m_HangupSessionFrame,
									ui.m_VoipButton,
									ui.m_AcceptOfferButton,
									ui.m_AcceptLabel,
									ui.m_RejectOfferButton,
									ui.m_RejectLabel,
									ui.m_HangupSessionButton,
									m_VidChatWidget,
									&m_TodGameLogic );
	m_MSessionsList.push_back( sessionState );

	sessionState = new MultiSessionState( m_MyApp, *this, eMSessionTypeVidChat );
	sessionState->setGuiWidgets(	hisIdent,
									m_OffersFrame,
									m_ResponseFrame,
									m_HangupSessionFrame,
									ui.m_VideoChatButton,
									ui.m_AcceptOfferButton,
									ui.m_AcceptLabel,
									ui.m_RejectOfferButton,
									ui.m_RejectLabel,
									ui.m_HangupSessionButton,
									m_VidChatWidget,
									&m_TodGameLogic );
	m_MSessionsList.push_back( sessionState );

	sessionState = new MultiSessionState( m_MyApp, *this, eMSessionTypeTruthOrDare );
	sessionState->setGuiWidgets(	hisIdent,
									m_OffersFrame,
									m_ResponseFrame,
									m_HangupSessionFrame,
									ui.m_TrueOrDareButton,
									ui.m_AcceptOfferButton,
									ui.m_AcceptLabel,
									ui.m_RejectOfferButton,
									ui.m_RejectLabel,
									ui.m_HangupSessionButton,
									m_VidChatWidget,
									&m_TodGameLogic );
	m_MSessionsList.push_back( sessionState );
	*/

	ui.m_SessionWidget->setCanSend( false );

	if( false == checkForSendAccess( false ) )
	{
		showReasonAccessNotAllowed();
	}

	m_MyApp.toGuiAssetAction( eAssetActionRxViewingMsg, m_HisIdent->getMyOnlineId(), 1 );
}

//============================================================================
void AppletMultiMessenger::onActivityFinish( void )
{
	ui.m_SessionWidget->onActivityStop();
	m_OfferSessionLogic.onStop();
}

//============================================================================
void AppletMultiMessenger::callbackToGuiRxedPluginOffer( GuiOfferSession* offer )
{
	m_OfferSessionLogic.callbackToGuiRxedPluginOffer( offer );
}

//============================================================================
void AppletMultiMessenger::callbackToGuiRxedOfferReply( GuiOfferSession* offerSession )
{
	m_OfferSessionLogic.callbackToGuiRxedOfferReply( offerSession );
}

//============================================================================
void AppletMultiMessenger::showReasonAccessNotAllowed( void )
{
	EPluginAccess accessState = m_HisIdent->getMyAccessPermissionFromHim( m_ePluginType );
	QString accessDesc = DescribePluginType( m_ePluginType );
	accessDesc += QObject::tr( " with " );
	accessDesc += m_HisIdent->getOnlineName().c_str();
	accessDesc +=  " ";

	if( ePluginAccessOk != accessState )
	{
		accessDesc +=  DescribePluginAccess( accessState );
		setStatusMsg( accessDesc );
		return;
	}

	if( false == m_HisIdent->isOnline() )
	{
		accessDesc +=   QObject::tr( " requires user be online " );
		setStatusMsg( accessDesc );
		return;
	}
}

//============================================================================
void AppletMultiMessenger::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
    if( !guiUser || !guiUser->isValid() )
    {
        LogMsg( LOG_ERROR, "AppletMultiMessenger::%s invalid param", __func__ );
        return;
    }

    if( m_UserMgr.isUserInSession( guiUser->getMyOnlineId() ) || guiUser->isFriend() )
	{
		QString statMsg = guiUser->getOnlineName().c_str();
		statMsg +=  m_MyApp.getUserMgr().isUserOnline( guiUser->getMyOnlineId() ) ? QObject::tr( " is online" ) : QObject::tr( " went offline" );
		setStatusMsg( statMsg );

		checkForSendAccess( false );
	}
}

//============================================================================
void AppletMultiMessenger::slotUserInputButtonClicked( void )
{
	checkForSendAccess( true );
}

//============================================================================
bool AppletMultiMessenger::checkForSendAccess( bool sendOfferIfPossible )
{
	bool canSend = false;
    if( !m_HisIdent )
    {
        LogMsg( LOG_DEBUG, "AppletMultiMessenger::checkForSendAccess null m_HisIdent" );
        return false;
    }

	if(  m_HisIdent->isOnline()
		&& m_HisIdent->isMyAccessAllowedFromHim( m_ePluginType ) )
	{
		canSend = true;
	}

	if( canSend != m_CanSend )
	{
		m_CanSend = canSend;
		ui.m_SessionWidget->setCanSend( m_CanSend );
	}

	if( m_CanSend && sendOfferIfPossible && ( false == m_OfferOrResponseIsSent ) )
	{
		QString offMsg = QObject::tr( "Requested Text Chat Session with " );
		offMsg += m_HisIdent->getOnlineName().c_str();
		m_MyApp.toGuiStatusMessage( offMsg.toUtf8().constData() );
		//m_Engine.fromGuiMultiSessionAction( eMSessionActionChatSessionReq, m_HisIdent->getMyOnlineId(), 0, m_OfferSessionLogic.getOfferSessionId() );
		if( m_OfferSessionLogic.sendOfferOrResponse() )
		{
			m_OfferOrResponseIsSent = true;
		}
	}

	QVector<MultiSessionState*>::iterator iter;
	for( iter = m_MSessionsList.begin();  iter != m_MSessionsList.end(); ++iter )
	{
		(*iter)->checkForSendAccess();
	}

	return canSend;
}


//============================================================================
void AppletMultiMessenger::showEvent( QShowEvent* ev )
{
	AppletPeerBase::showEvent( ev );
	m_MyApp.wantToGuiActivityCallbacks( this, true );
    if( m_HisIdent )
    {
        m_MyApp.toGuiAssetAction( eAssetActionRxViewingMsg, m_HisIdent->getMyOnlineId(), 1 );
    }
}

//============================================================================
void AppletMultiMessenger::hideEvent( QHideEvent* ev )
{
    if( m_HisIdent )
    {
        m_MyApp.toGuiAssetAction( eAssetActionRxViewingMsg, m_HisIdent->getMyOnlineId(), 0 );
    }

	m_MyApp.wantToGuiActivityCallbacks( this, false );
    if( ePluginTypeInvalid != m_ePluginType )
    {
        m_MyApp.getPluginMgr().setPluginVisible( m_ePluginType, false );
    }

    AppletPeerBase::hideEvent( ev );
}

//============================================================================
MultiSessionState*	AppletMultiMessenger::getMSessionState( EMSessionType sessionType )
{
	if( (sessionType >= 0 ) 
		&& ( sessionType < m_MSessionsList.size() ) )
	{
		return m_MSessionsList[ sessionType ];
	}

	return 0;
}

//============================================================================
void AppletMultiMessenger::onSessionStateChange( ESessionState eSessionState )
{

}

//============================================================================
// called from session logic
void AppletMultiMessenger::onInSession( bool isInSession )
{
	QVector<MultiSessionState*>::iterator iter;
	for( iter = m_MSessionsList.begin();  iter != m_MSessionsList.end(); ++iter )
	{
		(*iter)->onInSession( isInSession );
	}
}

//============================================================================
void AppletMultiMessenger::toGuiMultiSessionAction(  EMSessionAction mSessionAction, VxGUID onlineId, int pos0to100000 )
{
	if( m_HisIdent && m_HisIdent->getMyOnlineId() == onlineId )
	{
		if( onlineId == m_HisIdent->getMyOnlineId() )
		{
			if( eMSessionActionChatSessionAccept == mSessionAction )
			{
				onInSession( true );
			}
			else if( eMSessionActionChatSessionReject == mSessionAction )
			{
				onInSession( false );
			}

			MultiSessionState* sessionState = getMSessionState( ( EMSessionType )pos0to100000 );
			if( sessionState )
			{
				sessionState->handleMultiSessionAction( mSessionAction );
			}
		}
	}
}

//============================================================================
void AppletMultiMessenger::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
	if( m_IsInitialized )
	{
		ui.m_SessionWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
		m_TodGameWidget->ui.TruthOrDareWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
		if( m_VidChatWidget && m_VidChatWidget->isVisible() )
		{
			m_VidChatWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
		}
	}
}

//============================================================================
void AppletMultiMessenger::callbackToGuiPluginSessionEnded( GuiOfferSession* offer )
{
	//handleSessionEnded( offerResponse, this );
}; 

//============================================================================
void AppletMultiMessenger::setStatusMsg( QString strStatus )
{
	m_MyApp.toGuiStatusMessage( strStatus.toUtf8().constData() );
}

//============================================================================
void AppletMultiMessenger::toGuiSetGameValueVar(	EPluginType pluginType, 
													VxGUID&		onlineId, 
													int32_t		s32VarId, 
													int32_t		s32VarValue )
{
	if( ( pluginType == m_ePluginType )
		&& ( onlineId == m_HisIdent->getMyOnlineId() ) )
	{
		// emit signalToGuiSetGameValueVar( s32VarId, s32VarValue );
	}
}

//============================================================================
void AppletMultiMessenger::toGuiSetGameActionVar(	EPluginType     pluginType, 
													VxGUID&		    onlineId, 
													int32_t			s32VarId, 
													int32_t			s32VarValue )
{
	if( ( pluginType == m_ePluginType )
		&& ( onlineId == m_HisIdent->getMyOnlineId() ) )
	{
		// emit signalToGuiSetGameActionVar( s32VarId, s32VarValue );
	}
}

//============================================================================
void AppletMultiMessenger::slotSetSessionVisible( bool visible )
{
    if( ui.m_SessionFrame->isVisible() )
    {
        ui.m_SessionFrame->setVisible( false );
        //ui.m_EyeHosts->setIcon( eMyIconEyeHide );
    }
    else
    {
        ui.m_SessionFrame->setVisible( true );
        //ui.m_EyeHosts->setIcon( eMyIconEyeShow );
    }
}

//============================================================================
void AppletMultiMessenger::slotUserSelected( GuiUser* guiUser )
{
	setSelectedUser( guiUser );
}

//============================================================================
void AppletMultiMessenger::setSelectedUser( GuiUser* guiUser )
{
	if( m_SelectedUser )
	{
		// deselect previous user
		bool isRandConnectUser = m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeRandomConnect, m_SelectedUser->getMyOnlineId() );
		if( isRandConnectUser )
		{
			sendRandConnectSelected( guiUser->getMyOnlineId(), false );
		}
	}

	m_SelectedUser = guiUser;
	if( m_SelectedUser )
	{
		onSelectedUserChanged( m_SelectedUser );
	}
}

//============================================================================
void AppletMultiMessenger::onSelectedUserChanged( GuiUser* guiUser )
{
	if( guiUser )
	{
		bool isRandConnectUser = m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeRandomConnect, guiUser->getMyOnlineId() );
		if( isRandConnectUser )
		{
			sendRandConnectSelected( guiUser->getMyOnlineId(), true );
		}

        GroupieId groupieId( guiUser->getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypePeerUser );
        ui.m_SessionWidget->setGroupieId( groupieId );
	}
}

//============================================================================
void AppletMultiMessenger::userJoinedHost( GuiHosted* guiHosted )
{

}

//============================================================================
void AppletMultiMessenger::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}

//============================================================================
bool AppletMultiMessenger::checkIfCanSend( void )
{
	if( !m_SelectedUser )
	{
		GuiHelpers::errorMsgBox( eErrMsgNoUserSelectedToSendTo, this );
		return false;
	}
	else if( m_SelectedUser->isOnline() )
	{
		return true;
	}
	else if( !m_SelectedUser->isOnline() )
	{
		ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, "%s is offline. Message will be queued until the next time you are both online", 
									  m_SelectedUser->getOnlineName().c_str() );
		errMsgBox.showCancelButton( true );
        errMsgBox.exec();

		return errMsgBox.wasOkButtonClicked() ? true : false;
	}
	else
	{
		return true;
	}
}

//============================================================================
bool AppletMultiMessenger::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	if( !m_SelectedUser )
	{
		GuiHelpers::errorMsgBox( eErrMsgNoUserSelectedToSendTo, this );
		return false;
	}
	else if( !m_SelectedUser->isOnline() )
	{
		assetInfo.setDestUserId( m_SelectedUser->getMyOnlineId() );
		return getMyApp().getEngine().fromGuiQueueAssetAction( assetAction, assetInfo );
	}
	else
	{
		assetInfo.setDestUserId( m_SelectedUser->getMyOnlineId() );
		return getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
	}
}

//============================================================================
bool AppletMultiMessenger::sendRandConnectSelected( VxGUID& onlineId, bool isSelected )
{
	LogMsg( LOG_VERBOSE, "AppletMultiMessenger::%s selected %d %s", __func__,
			isSelected, m_MyApp.describeUser( onlineId ).c_str() );

	if( !m_MyApp.getMemberActiveMgr().isMemberOfHostType( eHostTypeRandomConnect, m_MyApp.getMyOnlineId() ) )
	{
		return false;
	}

	return getMyApp().getEngine().fromGuiSendRandConnectSelected( onlineId, isSelected );
}