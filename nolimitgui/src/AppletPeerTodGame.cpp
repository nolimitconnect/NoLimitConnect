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
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include "ui_AppletPeerTodGame.h"

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
	connect( &m_TodGameLogic, SIGNAL(signalGameStatus(QString)), this, SLOT(slotGameStatus(QString)) );
}

//============================================================================
void AppletPeerTodGame::onResizeEvent( QSize& newSize )
{
	ui.m_InstMsgWidget->setMaximumHeight( newSize.height() / 4 );
}

//============================================================================
//! called by base class with in session state
void AppletPeerTodGame::onInSession( bool isInSession )
{
	if( isInSession )
	{
        m_TodGameLogic.beginGame( m_OfferSessionLogic.isHost() );
	}
	else
	{
		m_TodGameLogic.endGame();
		setStatusText( tr( "Truth Or Dare Session Ended" ) );
	}
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
bool AppletPeerTodGame::setOfferSession( std::shared_ptr<GuiOfferSession>& offerSession )
{
	bool setupSessionResult{ false };
    m_HisIdent = offerSession->getUser();
	if( m_HisIdent )
	{
		m_TodGameLogic.setGuiWidgets( m_HisIdent, ui.m_TodGameWidget, ui.m_TodGameWidget->getMyStatsWidget(), ui.m_TodGameWidget->getHisStatsWidget() );
		ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );

		ui.m_TodGameWidget->getVidWidget()->setVideoFeedId( m_HisIdent->getMyOnlineId(), eMediaModuleTruthOrDare );
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

//============================================================================
void AppletPeerTodGame::onStateTextChanged( QString& stateText )
{
	ui.m_StateText->setText( stateText );
}

//============================================================================
void AppletPeerTodGame::slotGameStatus( QString statusText )
{
	ui.m_StateText->setText( statusText );
}
