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

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiOfferSession.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

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

	ui.m_PermissionButton->setFixedSize( eButtonSizeMedium );
    ui.m_HangUpButton->setFixedSize( eButtonSizeMedium );
    ui.m_HangUpButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_HangUpButton->setIcon( eMyIconRejectRedX );
	connect( ui.m_HangUpButton, SIGNAL(clicked()), this, SLOT(slotEndSession()) );
    connect( ui.m_ShowInWaveFormCheckBox, SIGNAL(clicked()), this, SLOT(slotShowInWaveFormCheckBoxClicked()) );
    connect( ui.m_ShowOutWaveFormCheckBox, SIGNAL(clicked()), this, SLOT(slotShowOutWaveFormCheckBoxClicked()) );

	bool showInWaveForm = m_MyApp.getAppSettings().getShowVoicePhoneInWaveForm();
	ui.m_AudioInWaveFormFrame->setVisible( showInWaveForm );
	ui.m_ShowInWaveFormCheckBox->setChecked( showInWaveForm );
	bool showOutWaveForm = m_MyApp.getAppSettings().getShowVoicePhoneOutWaveForm();
	ui.m_AudioOutWaveFormFrame->setVisible( showOutWaveForm );
	ui.m_ShowOutWaveFormCheckBox->setChecked( showOutWaveForm );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletPeerVoicePhone::~AppletPeerVoicePhone()
{
    m_MyApp.getAudioMgr().toGuiWantMicrophoneRecording( eMediaModuleSoundSettings, false );
    m_MyApp.getAudioMgr().toGuiWantSpeakerOutput( eMediaModuleSoundSettings, false );

    m_MyApp.activityStateChange( this, false );
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
}

//============================================================================
void AppletPeerVoicePhone::callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg )
{
	toGuiInstMsg( guiUser, pluginType, msg.c_str() );
}

//============================================================================
bool AppletPeerVoicePhone::setOfferSession( std::shared_ptr<GuiOfferSession>& offerSession )
{
	bool setupSessionResult{ false };
	m_HisIdent = offerSession->getUser();
	if( m_HisIdent )
	{
		setupSessionResult = AppletPeerBase::setOfferSession( offerSession );

		ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );
	}

	return setupSessionResult;
}

//============================================================================
void AppletPeerVoicePhone::onOfferWasSet( void )
{
	OfferBaseInfo& offerInfo = getOfferInfo();
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( offerInfo.getFromOnlineId() );
	if( guiUser )
	{
		setupBaseWidgets( guiUser, ui.m_FriendIdentWidget, ui.m_PermissionButton, ui.m_PermissionLabel );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletPeerVoicePhone::%s user not found %s", __func__, offerInfo.getFromOnlineId().toHexString().c_str() );
	}
}

//============================================================================
void AppletPeerVoicePhone::onStateTextChanged( QString& stateText )
{
	ui.m_StateText->setText( stateText );
}

//============================================================================
void AppletPeerVoicePhone::callbackToGuiPluginSessionStarted( std::shared_ptr<GuiOfferSession>& offer )
{
	if( isOfferMatch( offer ) )
	{
		LogMsg( LOG_VERBOSE, "AppletPeerVideoPhone::%s match", __func__ );
		ui.m_StateText->setText( QObject::tr( "Session Started" ) );
	}
	else
	{
		LogMsg( LOG_VERBOSE, "AppletPeerVideoPhone::%s NOT match", __func__ );
	}
}

//============================================================================
void AppletPeerVoicePhone::callbackToGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offer )
{
	if( isOfferMatch( offer ) )
	{
		LogMsg( LOG_VERBOSE, "AppletPeerVideoPhone::%s match", __func__ );
		ui.m_StateText->setText( QObject::tr( "Session Ended" ) );
	}
	else
	{
		LogMsg( LOG_VERBOSE, "AppletPeerVideoPhone::%s NOT match", __func__ );
	}
}

//============================================================================
void AppletPeerVoicePhone::slotShowInWaveFormCheckBoxClicked( void )
{   
    bool showWaveForm = ui.m_ShowInWaveFormCheckBox->isChecked();
    ui.m_AudioInWaveFormFrame->setVisible( showWaveForm );
    m_MyApp.getAppSettings().setShowVoicePhoneInWaveForm( showWaveForm );
}

//============================================================================
void AppletPeerVoicePhone::slotShowOutWaveFormCheckBoxClicked( void )
{   
    bool showWaveForm = ui.m_ShowOutWaveFormCheckBox->isChecked();
    ui.m_AudioOutWaveFormFrame->setVisible( showWaveForm );
    m_MyApp.getAppSettings().setShowVoicePhoneOutWaveForm( showWaveForm );
}
