//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InstMsgWidget.h"

#include "AppCommon.h"
#include "AppGlobals.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxTimeUtil.h>

#include <time.h>

#include "ui_InstMsgWidget.h"

//============================================================================
InstMsgWidget::InstMsgWidget( QWidget* parent )
: QWidget( parent )
, ui(*(new Ui::InstMsgWidgetClass))
, m_MyApp( GetAppInstance() )
, m_Engine( m_MyApp.getEngine() )
, m_ePluginType( ePluginTypeInvalid )
, m_HisIdent( 0 )
{
	ui.setupUi( this );
	//ui.m_MsgListWidget->setReadOnly(true);
	ui.m_ClearHistoryButton->setIcons( eMyIconShredderNormal );
	ui.m_CancelTextButton->setIcons( eMyIconSendArrowCancel );
	ui.m_SendTextButton->setIcons( eMyIconSendArrowNormal );

	connect( ui.m_CancelTextButton,		SIGNAL(clicked()),						this, SLOT(slotCancelButtonClicked()) );
	connect( ui.m_SendTextButton,		SIGNAL(clicked()),						this, SLOT(slotSendButtonClicked()) );
	connect( ui.m_ClearHistoryButton,	SIGNAL(clicked()),						this, SLOT(slotClearHistoryButtonClicked()) );
	connect( this,						SIGNAL(signalToGuiInstMsg(QString)),	this, SLOT(slotToGuiInstMsg(QString)) );

	ui.m_ChatTextEdit->setFocus();
}

//============================================================================
void InstMsgWidget::setInstMsgWidgets( EPluginType pluginType, GuiUser* hisIdent )
{
	m_ePluginType	= pluginType;
	m_HisIdent		= hisIdent;
}

//============================================================================
void InstMsgWidget::slotClearHistoryButtonClicked( void )
{
	ui.m_MsgListWidget->clear();
}

//============================================================================
void InstMsgWidget::slotCancelButtonClicked( void )
{
	ui.m_ChatTextEdit->clear();
}

//============================================================================
void InstMsgWidget::slotSendButtonClicked( void )
{
	// What did they want to say (minus white space around the string):
	QString message = ui.m_ChatTextEdit->toPlainText().trimmed();

	// Only send the text if it's not empty
	if(!message.isEmpty())
	{
		sendChatMsg( message );
	}

	ui.m_ChatTextEdit->clear();

	// Put the focus back into the input box so they can type again:
	ui.m_ChatTextEdit->setFocus();
	//emit signalInputCompleted();
}

//============================================================================
void InstMsgWidget::sendChatMsg( QString strChatMsg )
{
	if( 0 == m_HisIdent )
	{
		return;
	}

	// echo back with what was sent to our own screen
	QString strName = m_MyApp.getAppGlobals().getMyNetIdent()->getOnlineName();
	strName += ": ";
	strName += strChatMsg;
	appendHistoryMsg( strName );

	bool sentMsg = m_Engine.fromGuiInstMsg(	m_ePluginType,
											m_HisIdent->getMyOnlineId(), 
											strChatMsg.toUtf8().constData() );
	if( false == sentMsg )
	{
		QString strOfflineMsg = m_MyApp.getAppGlobals().getMyNetIdent()->getOnlineName();
		strOfflineMsg += ": is offline";
		appendHistoryMsg( strOfflineMsg );
	}
}

//============================================================================
void InstMsgWidget::appendHistoryMsg( QString strChatMsg )
{
	ui.m_MsgListWidget->addItem( strChatMsg );
}

//============================================================================
void InstMsgWidget::toGuiInstMsg( QString instMsg )
{
	emit signalToGuiInstMsg( instMsg );
}

//============================================================================
void InstMsgWidget::slotToGuiInstMsg( QString instMsg )
{
	if( 0 == m_HisIdent )
	{
		return;
	}

	// echo back with what was sent to our own screen
	QString strMsg = m_HisIdent->getOnlineName().c_str();
	strMsg += VxTimeUtil::getChatHourMinTimeStamp().c_str();
	strMsg += ": ";
	strMsg += instMsg;
	appendHistoryMsg( strMsg );
}
