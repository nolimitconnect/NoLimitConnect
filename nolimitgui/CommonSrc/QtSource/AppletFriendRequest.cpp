//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletFriendRequest.h"

#include "AppCommon.h"
#include "GuiOfferSession.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletFriendRequest.h"

//============================================================================
AppletFriendRequest::AppletFriendRequest(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_APPLET_FRIEND_REQUEST, app, parent )
, ui(*(new Ui::AppletFriendRequestUi))
{
    setPluginType( ePluginTypeFriendRequest );
    setAppletType( eAppletFriendRequest );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	//ui.m_PermissionButton->setFixedSize( eButtonSizeMedium );
 //   ui.m_HangUpButton->setFixedSize( eButtonSizeMedium );
 //   ui.m_HangUpButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	//ui.m_HangUpButton->setIcon( eMyIconRedX );
	//connect( ui.m_HangUpButton, SIGNAL(clicked()), this, SLOT(slotEndSession()) );
}

//============================================================================
//! called by base class with in session state
void AppletFriendRequest::onInSession( bool isInSession )
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
void AppletFriendRequest::toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg )
{
	if( ( pluginType == m_ePluginType )
		&& m_HisIdent 
		&& ( m_HisIdent->getMyOnlineId() == friendIdent->getMyOnlineId() ) )
	{
		//ui.m_InstMsgWidget->toGuiInstMsg( instMsg );
	}
}

//============================================================================
void AppletFriendRequest::callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg )
{
	toGuiInstMsg( guiUser, pluginType, msg.c_str() );
}

//============================================================================
void AppletFriendRequest::onOfferWasSet( void )
{
	//OfferBaseInfo& offerInfo = getOfferInfo();
	//GuiUser* guiUser = m_MyApp.getUserMgr().getUser( offerInfo.getFromOnlineId() );
	//if( guiUser )
	//{
	//	setupBaseWidgets( guiUser, ui.m_FriendIdentWidget, ui.m_PermissionButton, ui.m_PermissionLabel );
	//}
	//else
	//{
	//	LogMsg( LOG_ERROR, "AppletFriendRequest::%s user not found %s", __func__ );
	//}
}

//============================================================================
void AppletFriendRequest::onStateTextChanged( QString& stateText )
{
	//ui.m_StateText->setText( stateText );
}