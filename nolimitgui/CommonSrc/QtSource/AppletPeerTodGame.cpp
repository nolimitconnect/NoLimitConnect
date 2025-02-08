//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerTodGame.h"

#include "AppCommon.h"

#include "GuiOfferSession.h"
#include "AppGlobals.h"
#include "VidWidget.h"

#include <P2PEngine/P2PEngine.h>
#include <P2PEngine/EngineSettings.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>

#include "ui_AppletPeerTodGame.h"

namespace
{
	#define GAME_SETTINGS_KEY "TODGAME"
}

//============================================================================
AppletPeerTodGame::AppletPeerTodGame( AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_TO_FRIEND_TOD_GAME, app, parent )
, ui(*(new Ui::AppletPeerTodGameUi))
, m_TodGameLogic( app, app.getEngine(), ePluginTypeTruthOrDare, this )
{
    setPluginType( ePluginTypeTruthOrDare );
    setAppletType( eAppletPeerTruthOrDare );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	ui.m_PermissionButton->setFixedSize( eButtonSizeMedium );
    ui.m_HangUpButton->setFixedSize( eButtonSizeMedium );
    ui.m_HangUpButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_HangUpButton->setIcon( eMyIconRedX );
	connect( ui.m_HangUpButton, SIGNAL(clicked()), this, SLOT(slotEndSession()) );

	ui.m_TodGameWidget->getVidWidget()->setRecordFilePath( VxGetDownloadsDirectory().c_str() );
	setVidCamWidget( ui.m_TodGameWidget->getVidWidget() );
	connectBarWidgets();

	//m_OfferSessionLogic.sendOfferOrResponse();
	//bool bSentMsg = m_FromGui.fromGuiMakePluginOffer(	EPluginType pluginType, VxGUID& onlineId, OfferBaseInfo& offerInfo, VxGUID& lclSessionId );
	//if( false == bSentMsg )
	//{
	//	handleUserWentOffline();
	//}
	//else
	//{
	//	setStatusText( tr( "Waiting For Offer Reply" ) );
	//}
	// m_TodGameLogic.setGameStatus( eTxedOffer );
}

//============================================================================
//! called by base class with in session state
void AppletPeerTodGame::onInSession( bool isInSession )
{
	if( isInSession )
	{
		//setStatusText( tr( "In Truth Or Dare Session" ) );
		m_TodGameLogic.beginGame( ! m_OfferSessionLogic.isHost() );
		//m_Engine.fromGuiStartPluginSession( VxGUID& onlineId, void * pvUserData )
	}
	//else
	//{
	//	setStatusText( tr( "Truth Or Dare Session Ended" ) );
	//}
}

//============================================================================
void AppletPeerTodGame::toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg )
{
	if( ( pluginType == m_ePluginType )
		&& m_HisIdent 
		&& ( m_HisIdent->getMyOnlineId() == friendIdent->getMyOnlineId() ) )
	{
		ui.m_InstMsgWidget->toGuiInstMsg( instMsg );
	}
}

//============================================================================
void AppletPeerTodGame::callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg )
{
	toGuiInstMsg( guiUser, pluginType, msg.c_str() );
}

//============================================================================
bool AppletPeerTodGame::setOfferSession( std::shared_ptr<GuiOfferSession> offerSession )
{
	bool setupSessionResult{ false };
    m_HisIdent = offerSession->getUser();
	if( m_HisIdent )
	{
		m_TodGameLogic.setGuiWidgets( m_HisIdent, ui.m_TodGameWidget );
		ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );

		ui.m_TodGameWidget->getVidWidget()->setVideoFeedId( m_HisIdent->getMyOnlineId(), eAppModuleTruthOrDare );
		ui.m_TodGameWidget->getVidWidget()->setRecordFriendName( m_HisIdent->getOnlineName().c_str() );	

        setupSessionResult = AppletPeerBase::setOfferSession( offerSession );
	}

	return setupSessionResult;
}

//============================================================================
void AppletPeerTodGame::onOfferWasSet( void )
{
	OfferBaseInfo& offerInfo = getOfferInfo();
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( offerInfo.getFromOnlineId() );
	if( guiUser )
	{
		setupBaseWidgets( guiUser, ui.m_FriendIdentWidget, ui.m_PermissionButton, ui.m_PermissionLabel );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletPeerTodGame::%s user not found %s", __func__ );
	}
}
