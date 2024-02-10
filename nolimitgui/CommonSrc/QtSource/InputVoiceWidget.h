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

#include "ui_InputVoiceWidget.h"

class InputVoiceWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputVoiceWidget( QWidget* parent=0);

	void						setCanSend( bool canSend ) {};

	void						startRecording( void ); // so gui can start recording immediately

private slots:
    void						slotRecordButtonClicked( void );
	void						slotRecVoiceCancel( void );
	void						slotSlideLeftCompleted();
    void						slotExitVoiceWidget();

protected:
	void						updateMicrophoneButton( bool isRecording );

	Ui::InputVoiceWidget		ui;
	bool						m_IsRecording{ false };
};
