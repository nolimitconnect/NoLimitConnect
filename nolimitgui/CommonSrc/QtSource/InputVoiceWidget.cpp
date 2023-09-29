//============================================================================
// Copyright (C) 2015 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputVoiceWidget.h"
#include "ChatEntryWidget.h"
#include "AppCommon.h"
#include "MyIcons.h"
#include "GuiParams.h"

#include <QDebug>

//============================================================================
InputVoiceWidget::InputVoiceWidget( QWidget* parent )
: InputBaseWidget( GetAppInstance(), parent )
, m_IsRecording( false )
{
	qDebug() << "InputVoiceWidget::InputVoiceWidget ";
	m_AssetInfo.setAssetType( eAssetTypeAudio );

	ui.setupUi( this );

    ui.m_RecVoiceButton->setSquareButtonSize( eButtonSizeTiny );
    ui.m_CancelRecordButton->setSquareButtonSize( eButtonSizeTiny );
    ui.m_BackButton->setSquareButtonSize( eButtonSizeTiny );

	ui.m_RecVoiceButton->setIcons( eMyIconMicrophoneOn );
	//ui.m_RecVoiceButton->setIsSlideLeftButton( true );
	ui.m_RecVoiceButton->setPressedSound( eSndDefNone );
	ui.m_CancelRecordButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelRecordButton->setIcons( eMyIconCancelRecord );
	ui.m_CancelRecordButton->setPressedSound( eSndDefCancel );
	ui.m_CancelRecordButton->setVisible( false );

    ui.m_BackButton->setIcons( eMyIconBack );

	connect( ui.m_RecVoiceButton,		SIGNAL(clicked()),				this, SLOT(slotRecordButtonClicked()) );
	//connect( ui.m_RecVoiceButton,		SIGNAL(slideLeftCompleted()),	this, SLOT(slotSlideLeftCompleted()) );
	connect( ui.m_CancelRecordButton,	SIGNAL(clicked()),				this, SLOT(slotRecVoiceCancel()) );
	connect( ui.m_BackButton,		    SIGNAL(clicked()),				this, SLOT(slotExitVoiceWidget()) );
}

//============================================================================
void InputVoiceWidget::startRecording( void ) // so gui can start recording immediately
{
	if( !m_IsRecording )
	{
		slotRecordButtonClicked();
	}
}

//============================================================================
void InputVoiceWidget::updateMicrophoneButton( bool isRecording )
{
	ui.m_CancelRecordButton->setVisible( isRecording );
	ui.m_RecVoiceButton->setIcon( isRecording ? eMyIconMicrophoneOff : eMyIconMicrophoneOn );
	ui.m_RecVoiceButton->setNotifyOnlineEnabled( isRecording );
}

//============================================================================
void InputVoiceWidget::slotRecordButtonClicked( void )
{
	if( m_IsRecording )
	{
		m_IsRecording = false;
		voiceRecord( eAssetActionRecordEnd );
		updateMicrophoneButton( false );
	}
	else
	{
		m_IsRecording = true;
		voiceRecord( eAssetActionRecordBegin );
		updateMicrophoneButton( true );
	}
}

//============================================================================
void InputVoiceWidget::slotRecVoiceCancel( void )
{
    if( m_IsRecording )
    {
        voiceRecord( eAssetActionRecordCancel );
        m_IsRecording = false;
    }

	updateMicrophoneButton( false );
}

//============================================================================
void InputVoiceWidget::slotSlideLeftCompleted()
{
	slotRecVoiceCancel();
    m_MyApp.playSound( eSndDefCancel );
}

//============================================================================
void InputVoiceWidget::slotExitVoiceWidget( void )
{
    if( m_IsRecording )
    {
        voiceRecord( eAssetActionRecordCancel );
        m_IsRecording = false;
        m_MyApp.playSound( eSndDefCancel );
    }

	if( m_ChatEntryWidget )
	{
		this->setVisible( false );
		m_ChatEntryWidget->setEntryMode( eAssetTypeUnknown );
	}
}