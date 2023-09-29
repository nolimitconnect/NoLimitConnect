//============================================================================
// Copyright (C) 2013 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "AppletPeerTodGame.h"
#include "AppCommon.h"

#include "GuiOfferSession.h"
#include "AppGlobals.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/P2PEngine/EngineSettings.h>

#include <CoreLib/VxGlobals.h>

namespace
{
	#define GAME_SETTINGS_KEY "TODGAME"
}

//============================================================================
AppletPeerTodGame::AppletPeerTodGame( AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_TO_FRIEND_TOD_GAME, app, parent )
, m_TodGameLogic( app, app.getEngine(), ePluginTypeTruthOrDare, this )
{
    setPluginType( ePluginTypeTruthOrDare );
    setAppletType( eAppletPeerTruthOrDare );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

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
void AppletPeerTodGame::callbackToGuiRxedPluginOffer( GuiOfferSession* offer )
{
	m_OfferSessionLogic.callbackToGuiRxedPluginOffer( offer );
}

//============================================================================
void AppletPeerTodGame::callbackToGuiRxedOfferReply( GuiOfferSession* offerSession )
{
	m_OfferSessionLogic.callbackToGuiRxedOfferReply( offerSession );

}

//============================================================================
//! called by base class with in session state
void AppletPeerTodGame::onInSession( bool isInSession )
{
	if( isInSession )
	{
		//setStatusText( tr( "In Truth Or Dare Session" ) );
		m_TodGameLogic.beginGame( ! m_OfferSessionLogic.isRmtInitiated() );
		//m_Engine.fromGuiStartPluginSession( VxNetIdent* netIdent, void * pvUserData )
	}
	//else
	//{
	//	setStatusText( tr( "Truth Or Dare Session Ended" ) );
	//}
}

//============================================================================
//! called after session end or dialog exit
void AppletPeerTodGame::onEndSession( void )
{

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
}; 

//============================================================================
bool AppletPeerTodGame::offerSession( GuiUser* guiUser, GuiOfferSession* offerSession )
{
	bool setupSessionResult{ false };
	m_HisIdent = guiUser;
	if( m_HisIdent )
	{
		m_TodGameLogic.setGuiWidgets( m_HisIdent, ui.m_TodGameWidget );
		ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );

		ui.m_TodGameWidget->getVidWidget()->setVideoFeedId( m_HisIdent->getMyOnlineId(), eAppModuleTruthOrDare );
		ui.m_TodGameWidget->getVidWidget()->setRecordFriendName( m_HisIdent->getOnlineName().c_str() );	

		setupSessionResult = AppletPeerBase::offerSession( guiUser, offerSession );
	}

	return setupSessionResult;
}