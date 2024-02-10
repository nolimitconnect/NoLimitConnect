//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "SessionWidget.h"
#include "ui_SessionWidget.h"

#include <QDebug>

//============================================================================
SessionWidget::SessionWidget( QWidget* parent, EAssetType inputMode )
: QWidget( parent )
, m_MyApp(GetAppInstance())
, m_InputMode( inputMode )
{
	qDebug() << "SessionWidget input mode " << inputMode;
	ui.setupUi(this);
	ui.m_IdentWidget->setVisible( false );

	connect( ui.m_ChatEntry, SIGNAL(signalUserInputButtonClicked()), this, SIGNAL(signalUserInputButtonClicked()) );
    connect( &m_MyApp, SIGNAL( signalStatusMsg(QString) ), this, SLOT( slotStatusMsg(QString) ) );

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
	ui.m_ChatEntry->setGroupieId( groupieId );
	ui.m_HistoryList->setGroupieId( groupieId );
	ui.m_IdentWidget->updateIdentity( groupieId.getUserOnlineId() );
	m_IsInitialized = true;
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
void SessionWidget::setAppModule( EAppModule appModule )
{
	ui.m_ChatEntry->setAppModule( appModule );
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