//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SessionWidget.h"

#include "AppCommon.h"
#include "ActivityBase.h"
#include "GuiHelpers.h"

#include "ui_SessionWidget.h"

#include <QDebug>

QLabel *                    SessionWidget::getSessionStatusLabel( void )		{ return ui.m_StatusLabel; }
HistoryListWidget *         SessionWidget::getSessionHistoryList( void )		{ return ui.m_HistoryList; }
ChatEntryWidget *           SessionWidget::getSessionChatEntry( void )			{ return ui.m_ChatEntry; }
void						SessionWidget::initializeHistory( void )			{ getSessionHistoryList()->initializeHistory(); }
void						SessionWidget::setStatusMsgVisible( bool visible )	{ ui.m_StatusLabel->setVisible( visible ); }
bool						SessionWidget::getStatusMsgVisible( void )			{ return ui.m_StatusLabel->isVisible(); }
void						SessionWidget::showInviteFrame( bool show )			{ ui.m_CreateInviteFrame->setVisible( show ); }

//============================================================================
SessionWidget::SessionWidget( QWidget* parent, EAssetType inputMode )
: QWidget( parent )
, ui(*(new Ui::SessionWidgetUi))
, m_MyApp(GetAppInstance())
, m_InputMode( inputMode )
{
	qDebug() << "SessionWidget input mode " << inputMode;
	ui.setupUi(this);
	showInviteFrame( false );

	ui.m_CreateInviteButton->setFixedSize( eButtonSizeSmall );
	ui.m_CreateInviteButton->setIcon( eMyIconInviteCreate );

	connect( ui.m_ChatEntry, SIGNAL(signalUserInputButtonClicked()), this, SIGNAL(signalUserInputButtonClicked()) );
	connect( ui.m_ChatEntry, SIGNAL(signalFaceInputVisibilityChanged(bool)), ui.m_HistoryList, SLOT(setHidden(bool)) );
	connect( ui.m_CreateInviteButton, SIGNAL(clicked()), this, SLOT(slotCreateInviteButtonClicked()) );
    connect( &m_MyApp, SIGNAL(signalStatusMsg(QString)), this, SLOT(slotStatusMsg(QString)) );

	setEntryMode( m_InputMode );
}

//============================================================================
void SessionWidget::slotStatusMsg( QString msg )
{
    if( !msg.isEmpty() )
    {
        ui.m_StatusLabel->setText( msg );
    }
}

//============================================================================
void SessionWidget::setGroupieId( GroupieId& groupieId )
{
	m_GroupieId = groupieId;
	ui.m_ChatEntry->setGroupieId( groupieId );
	ui.m_HistoryList->setGroupieId( groupieId );

	if( eHostTypeChatRoom == groupieId.getHostType() )
	{
		hideVideoCaptureInput(); // dont allow users of chat room to send video through host 
	}

	m_IsInitialized = true;
}

//============================================================================
void SessionWidget::setLimitToTextAndPhotos( bool justTextAndPhotos )
{
	m_JustTextAndPhotos = justTextAndPhotos;
	ui.m_ChatEntry->setLimitToTextAndPhotos( justTextAndPhotos );
}

//============================================================================
void SessionWidget::setEntryMode( EAssetType inputMode )
{
	ui.m_ChatEntry->setEntryMode( inputMode );
}

//============================================================================
void SessionWidget::setIsPersonalRecorder( bool isPersonal )
{
	ui.m_ChatEntry->setIsPersonalRecorder( isPersonal );
}

//============================================================================
void SessionWidget::setCanSend( bool canSend )
{
	ui.m_ChatEntry->setCanSend( canSend );
}

//============================================================================
void SessionWidget::setInputClientCallback( InputClientBaseCallback* clientCallback )
{
	m_ClientCallback = clientCallback;
	ui.m_ChatEntry->setInputClientCallback( clientCallback );
}

//============================================================================
void SessionWidget::setMediaModule( EMediaModule mediaModule )
{
	ui.m_ChatEntry->setMediaModule( mediaModule );
}

//============================================================================
void SessionWidget::setPluginType( EPluginType pluginType )
{
	m_PluginType = pluginType;
	ui.m_ChatEntry->setPluginType( pluginType );
	ui.m_HistoryList->setPluginType( pluginType );
}

//============================================================================
void SessionWidget::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
	if( m_IsInitialized )
	{
		ui.m_ChatEntry->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
	}
}

//============================================================================
void SessionWidget::onActivityStop( void )
{
	if( m_IsInitialized )
	{
		ui.m_HistoryList->onActivityStop();
	}
}

//============================================================================
void SessionWidget::hideVideoCaptureInput( void )
{
	ui.m_ChatEntry->hideVideoCaptureInput();
}

//============================================================================
void SessionWidget::slotCreateInviteButtonClicked( void )
{
	GuiHelpers::showCreateInvite( m_GroupieId.getHostType(), dynamic_cast<QWidget*>( GuiHelpers::findParentActivity( parentWidget() ) ) );
}

