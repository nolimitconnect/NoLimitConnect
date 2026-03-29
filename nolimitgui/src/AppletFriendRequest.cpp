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

#include <P2PEngine/P2PEngine.h>
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

    ui.m_AcceptFrame->setVisible( false );
    ui.m_SendFrame->setVisible( false );

    ui.m_AcceptButton->setFixedSize( eButtonSizeMedium );
    ui.m_RejectButton->setFixedSize( eButtonSizeMedium );
    ui.m_SendButton->setFixedSize( eButtonSizeMedium );
    ui.m_CancelButton->setFixedSize( eButtonSizeMedium );

	ui.m_AcceptButton->setIcon( eMyIconAcceptCheckMark );
    ui.m_SendButton->setIcon( eMyIconSendArrowNormal );
	ui.m_AcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_SendButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );

	ui.m_RejectButton->setIcon( eMyIconRejectRedX );
    ui.m_CancelButton->setIcon( eMyIconRejectRedX );
	ui.m_RejectButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );

	connect( ui.m_ClipboardPasteWidget, SIGNAL(signalClipboardPaste(QString)), this, SLOT(slotPaste(QString)) );
	connect( ui.m_ClipboardCopyWidget, SIGNAL(clicked()), this, SLOT(slotCopy()) );

	connect( ui.m_SendButton, SIGNAL(clicked()), this, SLOT(slotSend()) );
	connect( ui.m_CancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()) );
	connect( ui.m_AcceptButton, SIGNAL(clicked()), this, SLOT(slotAccept()) );
	connect( ui.m_RejectButton, SIGNAL(clicked()), this, SLOT(slotReject()) );
}

//============================================================================
void AppletFriendRequest::friendRequestSetup( VxGUID onlineId, bool isAccept )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId, true );
	if( !guiUser )
	{
		okMessageBox( QObject::tr( "User not found" ), QObject::tr( "No user found with id ") + QString( onlineId.toOnlineIdString().c_str() ) );	
		return;
	}

	m_GuiUser = guiUser;
	ui.m_IdentWidget->updateIdentity( guiUser );
	m_IsAccept = isAccept;
	if( m_IsAccept )
	{
		ui.m_AcceptFrame->setVisible( true );
	}
	else
	{
		ui.m_SendFrame->setVisible( true );
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
void AppletFriendRequest::slotPaste( QString pasteText )
{
	ui.m_RequestTextEdit->clear();
	ui.m_RequestTextEdit->appendPlainText( pasteText );
}

//============================================================================
void AppletFriendRequest::slotCopy( void )
{
	ui.m_ClipboardCopyWidget->copyToClipboard( ui.m_RequestTextEdit->toPlainText() );
}

//============================================================================
void AppletFriendRequest::slotSend( void )
{
	std::string requestText = ui.m_RequestTextEdit->toPlainText().toUtf8().constData();
	if( requestText.empty() )
	{
		okMessageBox( QObject::tr( "Friend Request" ), QObject::tr( "Friend Request requires a message with request reason" ) );
		return;
	}

	if( !m_MyApp.getEngine().fromGuiSendFriendRequest( m_GuiUser->getMyOnlineId(), requestText, m_GuiUser->getMyFriendshipToHim() ) )
	{
		okMessageBox( QObject::tr( "Friend Request Send Failed" ), QObject::tr( "Friend Request Send Failed" ) );
	}
	else
	{
		okMessageBox( QObject::tr( "Friend Request Sent" ), QObject::tr( "Friend Request was sent" ) );
	}
}

//============================================================================
void AppletFriendRequest::slotCancel( void )
{
	close();
}

//============================================================================
void AppletFriendRequest::slotAccept( void )
{

}

//============================================================================
void AppletFriendRequest::slotReject( void )
{

}

