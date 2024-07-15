//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputAllWidget.h"

#include "AppCommon.h"
#include "GuiParams.h"

#include "ui_InputAllWidget.h"

//============================================================================
InputAllWidget::InputAllWidget( QWidget* parent )
	: InputBaseWidget( GetAppInstance(), parent )
	, ui(*(new Ui::InputAllWidget))
{
	ui.setupUi( this );

	ui.m_SelectFaceAllButton->setIcons( eMyIconFaceSelectNormal );
	ui.m_CameraAllButton->setIcons( eMyIconCameraNormal );
	ui.m_GalleryAllButton->setIcons( eMyIconGalleryNormal );
	ui.m_VideoAllButton->setIcons( eMyIconCamcorderNormal );
	ui.m_MicAllButton->setIcons( eMyIconMicrophoneOn );

	ui.m_SelectFaceAllButton->setSquareButtonSize( eButtonSizeTiny );
	ui.m_CameraAllButton->setSquareButtonSize( eButtonSizeTiny );
	ui.m_GalleryAllButton->setSquareButtonSize( eButtonSizeTiny );
	ui.m_VideoAllButton->setSquareButtonSize( eButtonSizeTiny );
	ui.m_MicAllButton->setSquareButtonSize( eButtonSizeTiny );

	connect( ui.m_TextAllButton, SIGNAL(clicked()), this, SLOT( slotTextButtonClicked() ) );
	connect( ui.m_SelectFaceAllButton, SIGNAL(clicked()), this, SLOT( slotEmoteButtonClicked() ) );
	connect( ui.m_CameraAllButton, SIGNAL(clicked()), this, SLOT( slotCameraButtonClicked() ) );
	connect( ui.m_GalleryAllButton, SIGNAL(clicked()), this, SLOT( slotGalleryButtonClicked() ) );
	connect( ui.m_VideoAllButton, SIGNAL(clicked()), this, SLOT( slotVideoButtonClicked() ) );
	connect( ui.m_MicAllButton, SIGNAL( pressed() ), this, SLOT( slotMicButtonPressed() ) );
	connect( ui.m_MicAllButton, SIGNAL( released() ), this, SLOT( slotMicButtonReleased() ) );
}

//============================================================================
void InputAllWidget::setCanSend( bool canSend )
{
	ui.m_TextAllButton->setEnabled( canSend );
	ui.m_SelectFaceAllButton->setEnabled( canSend );
	ui.m_CameraAllButton->setEnabled( canSend );
	ui.m_GalleryAllButton->setEnabled( canSend );
	ui.m_VideoAllButton->setEnabled( canSend );
	ui.m_MicAllButton->setEnabled( canSend );
}

//============================================================================
void InputAllWidget::slotTextButtonClicked( void )
{
	if( !checkIfCanSend() )
	{
		return;
	}

	emit signalUserInputButtonClicked();
	emit signalAllTextButtonClicked();
}

//============================================================================
void InputAllWidget::slotEmoteButtonClicked( void )
{
	if( !checkIfCanSend() )
	{
		return;
	}

	emit signalUserInputButtonClicked();
	emit signalAllEmoteButtonClicked();
}

//============================================================================
void InputAllWidget::slotVideoButtonClicked( void )
{
	if( !checkIfCanSend() )
	{
		return;
	}

	emit signalUserInputButtonClicked();
	emit signalAllVideoButtonClicked();
}

//============================================================================
void InputAllWidget::slotCameraButtonClicked( void )
{
	if( !checkIfCanSend() )
	{
		return;
	}

	emit signalUserInputButtonClicked();
	emit signalAllCameraButtonClicked();
}

//============================================================================
void InputAllWidget::slotGalleryButtonClicked( void )
{
	if( !checkIfCanSend() )
	{
		return;
	}

	emit signalUserInputButtonClicked();
	emit signalAllGalleryButtonClicked();
}

//============================================================================
void InputAllWidget::slotMicButtonPressed( void )
{
	if( !checkIfCanSend() )
	{
		return;
	}

	emit signalUserInputButtonClicked();
	emit signalAllMicButtonPressed();
}

//============================================================================
void InputAllWidget::slotMicButtonReleased( void )
{
	emit signalAllMicButtonReleased();
}

//============================================================================
void InputAllWidget::hideVideoCaptureInput( void )
{
	ui.m_VideoAllButton->setVisible( false );
}
