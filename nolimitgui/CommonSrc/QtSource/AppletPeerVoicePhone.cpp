//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerVoicePhone.h"

#include "GuiOfferSession.h"
#include "AppGlobals.h"

#include <P2PEngine/P2PEngine.h>

#include "GuiOfferSession.h"

#include <CoreLib/ObjectCommonDefs.h>

#include "ui_AppletPeerVoicePhone.h"

//============================================================================
AppletPeerVoicePhone::AppletPeerVoicePhone(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_TO_FRIEND_VOICE_PHONE, app, parent )
, ui(*(new Ui::AppletPeerVoicePhoneUi))
{
    setPluginType( ePluginTypeVoicePhone );
    setAppletType( eAppletPeerVoicePhone );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	setupActivityVoicePhone();
	// m_OfferSessionLogic.sendOfferOrResponse();
}

//============================================================================
void AppletPeerVoicePhone::setupActivityVoicePhone( void )
{
    /*
	setupBaseWidgets( ui.m_TitleBarWidget, ui.m_FriendIdentWidget, ui.m_PermissionButton, ui.m_PermissionLabel );
    QString titleText = QObject::tr("Voice Phone (VOIP) - ");
    titleText += m_HisIdent->getOnlineName();
    ui.m_TitleBarWidget->setTitleBarText( titleText );
	ui.m_TitleBarWidget->enableAudioControls( true );
    connectBarWidgets();
	ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );
	ui.m_HangUpButton->setIcon( eMyIconVoicePhoneCancel );
	connect( ui.m_HangUpButton, SIGNAL(clicked()), this, SLOT(reject()) );
    */
}

//============================================================================
void AppletPeerVoicePhone::callbackToGuiRxedPluginOffer( GuiOfferSession* offer )
{
	m_OfferSessionLogic.callbackToGuiRxedPluginOffer( offer );
}

//============================================================================
void AppletPeerVoicePhone::callbackToGuiRxedOfferReply( GuiOfferSession* offerSession )
{
	m_OfferSessionLogic.callbackToGuiRxedOfferReply( offerSession );
}

//============================================================================
//! called by base class with in session state
void AppletPeerVoicePhone::onInSession( bool isInSession )
{
	if( isInSession )
	{
		setStatusText( QObject::tr( "In Voice Phone Session" ) );
	}
	else
	{
		setStatusText( QObject::tr( "Voice Phone Session Ended" ) );
	}
}

//============================================================================
void AppletPeerVoicePhone::toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg )
{
	if( ( pluginType == m_ePluginType )
		&& m_HisIdent 
		&& ( m_HisIdent->getMyOnlineId() == friendIdent->getMyOnlineId() ) )
	{
		ui.m_InstMsgWidget->toGuiInstMsg( instMsg );
	}
}; 

