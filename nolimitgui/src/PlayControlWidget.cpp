//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PlayControlWidget.h"
#include "GuiParams.h"

#include <QTimer>

#include "ui_PlayControlWidget.h"


QSlider*					PlayControlWidget::getPlayPosSlider( void )		{ return ui.m_PlayPosSlider; }
VxPushButton*				PlayControlWidget::getPlayPauseButton( void )		{ return ui.m_PlayPauseButton; }

//============================================================================
PlayControlWidget::PlayControlWidget( QWidget* parent )
: QWidget(parent)
, ui(*(new Ui::PlayControlUi))
, m_UpdateTimer( new QTimer( this  ) )
{
	ui.setupUi( this );
	QSize buttonSize( GuiParams::getButtonSize( eButtonSizeTiny ) );
	ui.m_PlayPauseButton->setFixedSizeAbsolute( buttonSize );
	ui.m_PlayPauseButton->setPressedSound( eSndDefNone );

	ui.m_StopButton->setFixedSizeAbsolute( buttonSize );
	ui.m_StopButton->setPressedSound( eSndDefNone );
	ui.m_RestartButton->setFixedSizeAbsolute( buttonSize );
	ui.m_RestartButton->setPressedSound( eSndDefNone );
	ui.m_RestartButton->setVisible( false );

	ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
	ui.m_StopButton->setIcons( eMyIconMediaStop );
	ui.m_RestartButton->setIcons( eMyIconArrowLeft );
	
	ui.m_PlayPosSlider->setRange( 0, 100000 );
	ui.m_PlayPosSlider->setMinimum( 0 );
	ui.m_PlayPosSlider->setMaximum( 100000 );
	ui.m_PlayPosSlider->setEnabled( false );

	connect( ui.m_PlayPauseButton, SIGNAL(clicked()), this, SLOT(slotPlayButtonClicked()) );
	connect( ui.m_StopButton, SIGNAL(clicked()), this, SLOT(slotStopButtonClicked()) );
	connect( ui.m_RestartButton, SIGNAL(clicked()), this, SLOT(slotRestartButtonClicked()) );

	connect( ui.m_PlayPosSlider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()) );
	connect( ui.m_PlayPosSlider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()) );

	m_UpdateTimer->setInterval( 200 );
	connect( m_UpdateTimer,		SIGNAL(timeout()),				this, SLOT(slotUpdatePlayerControls()) );
	m_UpdateTimer->start();

    //ui.m_PlayPosSlider->setVisible( true );
	//QSize sizeHint( 200, GuiParams::getButtonSize( eButtonSizeLarge ).height() + GuiParams::getButtonSize( eButtonSizeTiny ).height() );
	//setSizeHint( sizeHint );
	//setFixedHeight( sizeHint.height() );
}

//============================================================================
void PlayControlWidget::slotUpdatePlayerControls( void )
{
	emit signalUpdateControlsTimeout();
}

//============================================================================
void PlayControlWidget::slotSliderPressed( void )
{
	m_SliderIsPressed = true;
}

//============================================================================
void PlayControlWidget::slotSliderReleased( void )
{
	int posVal = ui.m_PlayPosSlider->value();
	emit signalSliderChanged( posVal );
	m_SliderIsPressed = false;
}

//============================================================================
void PlayControlWidget::slotPlayButtonClicked( void )
{
	emit signalPlayPauseButtonClicked();
}

//============================================================================
void PlayControlWidget::slotStopButtonClicked( void )
{
	emit signalStopButtonClicked();
}

//============================================================================
void PlayControlWidget::slotRestartButtonClicked( void )
{
	emit signalRestart();
}

//============================================================================
void PlayControlWidget::setSliderMax( int maxPosition )
{
	ui.m_PlayPosSlider->setRange( 0, maxPosition );
	ui.m_PlayPosSlider->setMinimum( 0 );
	ui.m_PlayPosSlider->setMaximum( maxPosition );
}

//============================================================================
void PlayControlWidget::setSliderPosition( int sliderPosition )
{
	ui.m_PlayPosSlider->setValue( sliderPosition );
}

//============================================================================
void PlayControlWidget::setPlayPause( bool isPaused )
{
	m_IsPaused = isPaused;
	if( m_IsPaused )
	{
		ui.m_PlayPauseButton->setIcons( eMyIconPauseNormal );
	}
	else
	{
		ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
	}
}

//============================================================================
void PlayControlWidget::setCanSeek( bool canSeek )
{
	ui.m_PlayPosSlider->setEnabled( canSeek );
}

