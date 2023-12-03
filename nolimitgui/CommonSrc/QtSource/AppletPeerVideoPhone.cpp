//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "GuiOfferSession.h"
#include "AppletPeerVideoPhone.h"
#include "AppGlobals.h"
#include "MyIcons.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
AppletPeerVideoPhone::AppletPeerVideoPhone(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_TO_FRIEND_VIDEO_PHONE, app, parent )
{
    setPluginType( ePluginTypeVideoPhone );
    setAppletType( eAppletPeerVideoPhone );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	setupActivityVideoPhone();
	// m_OfferSessionLogic.sendOfferOrResponse();
}

//============================================================================
void AppletPeerVideoPhone::setupActivityVideoPhone()
{
    /*
	setupBaseWidgets( ui.m_TitleBarWidget, ui.m_FriendIdentWidget, ui.m_PermissionButton, ui.m_PermissionLabel );
	ui.m_TitleBarWidget->setTitleBarText( tr("Video Chat") );
	ui.m_TitleBarWidget->enableAudioControls( true );
    connectBarWidgets();
	ui.m_InstMsgWidget->setInstMsgWidgets( m_ePluginType, m_HisIdent );

	ui.m_CamVidWidget->setVideoFeedId( m_HisIdent->getMyOnlineId() );
	ui.m_CamVidWidget->setRecordFilePath( VxGetDownloadsDirectory().c_str() );
	ui.m_CamVidWidget->setRecordFriendName( m_HisIdent->getOnlineName() );
	setVidCamWidget( ui.m_CamVidWidget );
    */
}

//============================================================================
void AppletPeerVideoPhone::callbackToGuiRxedPluginOffer( GuiOfferSession* offer )
{
	m_OfferSessionLogic.callbackToGuiRxedPluginOffer( offer );
}

//============================================================================
void AppletPeerVideoPhone::callbackToGuiRxedOfferReply( GuiOfferSession* offerSession )
{
	m_OfferSessionLogic.callbackToGuiRxedOfferReply( offerSession );
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
}; 

