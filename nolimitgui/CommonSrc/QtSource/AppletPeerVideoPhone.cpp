//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerVideoPhone.h"

#include "AppCommon.h"
#include "GuiOfferSession.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletPeerVideoPhone.h"

//============================================================================
AppletPeerVideoPhone::AppletPeerVideoPhone(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_TO_FRIEND_VIDEO_PHONE, app, parent )
, ui(*(new Ui::AppletPeerVideoPhoneUi))
{
    setPluginType( ePluginTypeVideoPhone );
    setAppletType( eAppletPeerVideoPhone );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	ui.m_PermissionButton->setFixedSize( eButtonSizeMedium );
    ui.m_HangUpButton->setFixedSize( eButtonSizeMedium );
    ui.m_HangUpButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_HangUpButton->setIcon( eMyIconRedX );
	connect( ui.m_HangUpButton, SIGNAL(clicked()), this, SLOT(slotEndSession()) );
}

//============================================================================
//! called by base class with in session state
void AppletPeerVideoPhone::onInSession( bool isInSession )
{
	//if( isInSession )
	//{
	//	setStatusText( tr( "In Video Chat Session" ) );
	//}
	//else
	//{
	//	setStatusText( tr( "Video Phone Chat Ended" ) );
	//}
}

//============================================================================
void AppletPeerVideoPhone::toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg )
{
	if( ( pluginType == m_ePluginType )
		&& m_HisIdent 
		&& ( m_HisIdent->getMyOnlineId() == friendIdent->getMyOnlineId() ) )
	{
		ui.m_InstMsgWidget->toGuiInstMsg( instMsg );
	}
}

//============================================================================
void AppletPeerVideoPhone::callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg )
{
	toGuiInstMsg( guiUser, pluginType, msg.c_str() );
}

//============================================================================
bool AppletPeerVideoPhone::setOfferSession( std::shared_ptr<GuiOfferSession> offerSession )
{
	bool setupSessionResult{ false };
    m_HisIdent = offerSession->getUser();
	if( m_HisIdent )
	{
		setupSessionResult = AppletPeerBase::setOfferSession( offerSession );

		ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );

		ui.m_CamVidWidget->setVideoFeedId( m_HisIdent->getMyOnlineId(), eAppModuleVideoPhone );
		ui.m_CamVidWidget->setRecordFriendName( m_HisIdent->getOnlineName().c_str() );	
	}

	return setupSessionResult;
}

//============================================================================
void AppletPeerVideoPhone::onOfferWasSet( void )
{
	OfferBaseInfo& offerInfo = getOfferInfo();
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( offerInfo.getFromOnlineId() );
	if( guiUser )
	{
		setupBaseWidgets( guiUser, ui.m_FriendIdentWidget, ui.m_PermissionButton, ui.m_PermissionLabel );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletPeerVideoPhone::%s user not found", __func__ );
	}
}

//============================================================================
void AppletPeerVideoPhone::onStateTextChanged( QString& stateText )
{
	ui.m_StateText->setText( stateText );
}