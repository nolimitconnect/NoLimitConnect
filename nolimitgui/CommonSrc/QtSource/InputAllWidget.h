#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputBaseWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class InputAllWidget;
}
QT_END_NAMESPACE

class InputAllWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputAllWidget( QWidget* parent = nullptr );

	void						setCanSend( bool canSend );

	void						hideVideoCaptureInput( bool hideCapture = true );
	void						hideAudioCaptureInput( bool hideCapture = true );

signals:
	void						signalUserInputButtonClicked( void );

	void						signalAllTextButtonClicked( void );
	void						signalAllEmoteButtonClicked( void );
	void						signalAllVideoButtonClicked( void );
	void						signalAllCameraButtonClicked( void );
	void						signalAllGalleryButtonClicked( void );
	void						signalAllMicButtonPressed( void );
	void						signalAllMicButtonReleased( void );

private slots:
	void						slotTextButtonClicked( void );
	void						slotEmoteButtonClicked( void );
	void						slotVideoButtonClicked( void );
	void						slotCameraButtonClicked( void );
	void						slotGalleryButtonClicked( void );
	void						slotMicButtonPressed( void );
	void						slotMicButtonReleased( void );

protected:
	Ui::InputAllWidget&			ui;
};
