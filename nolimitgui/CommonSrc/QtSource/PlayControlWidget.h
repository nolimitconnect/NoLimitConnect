#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_PlayControlWidget.h"

#include <QWidget>

#include <vector>

class QTimer;

class PlayControlWidget : public QWidget
{
	Q_OBJECT

public:
	PlayControlWidget( QWidget* parent = nullptr );

	QSlider*					getPlayPosSlider( void )		{ return ui.m_PlayPosSlider; }
	VxPushButton*				getPlayPauseButton( void )		{ return ui.m_PlayPauseButton; }

	void						setSliderMax( int maxPosition );
	void						setSliderPosition( int sliderPosition );

	void						setPlayPause( bool isPaused );

	void						setCanSeek( bool canSeek );

signals:
	void						signalRestart( void );
	void						signalPlayPauseButtonClicked( void );
	void						signalStopButtonClicked( void );
	void						signalSliderChanged( int sliderPos );

	void						signalUpdateControlsTimeout( void );

protected slots:
	void						slotRestartButtonClicked( void );
	void						slotPlayButtonClicked( void );
	void						slotStopButtonClicked( void );
	void						slotSliderPressed( void );
	void						slotSliderReleased( void );

	void						slotUpdatePlayerControls( void );

protected:

	//=== vars ===//
	bool						m_IsPaused{ false };
	bool						m_SliderIsPressed{ false };

	QTimer*						m_UpdateTimer{ nullptr };

	Ui::PlayControlUi			ui;

};
