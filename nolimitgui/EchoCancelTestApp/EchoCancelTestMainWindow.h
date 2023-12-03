#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "soundio/AudioTestGenerator.h"

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IAudioDefs.h>

#include <QString>
#include <QMainWindow>
#include <QMediaDevices>
#include <QIODevice>
#include <QTimer>

#include "ui_EchoCancelTestMainWindow.h"

class AppCommon;

class EchoCancelTestMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	EchoCancelTestMainWindow( AppCommon& app, QWidget* parent = nullptr );
	virtual ~EchoCancelTestMainWindow() override;

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

    void                        statusMsg( const char* errMsg, ... );
    void						setStatusLabel( QString strMsg );

signals:
    void                        signalInfoMsg( QString& infoStr );

protected slots:
    void                        inDeviceChanged( int index );
    void                        updateInAudioDevices( void );

    void                        outDeviceChanged( int index );
    void                        updateOutAudioDevices( void );

    void                        slotApplyInDeviceChange( void );
    void                        slotApplyOutDeviceChange( void );
    void                        slotPeakTimerTimeout( void );

    void                        slotStartTestSoundDelay( void );
    void                        slotEchoDelaySaveButtonClicked( void );
    void                        slotTestedSoundDelayResult( int echoDelayMs );
    // void                        slotEchoCancelEnableChange( int checkState );
    void                        slotAudioTestState( EAudioTestState audioTestState );
    void                        slotAudioTestMsg( QString audioTestMsg );

    void                        slotWantMicrophoneCheckBox( int checkState );
    void                        slotMuteMicrophoneCheckBox( int checkState );
    void                        slotSendMicToSpeakerCheckBox( int checkState );

    void                        slotWantSpeakerCheckBox( int checkState );
    void                        slotMuteSpeakerCheckBox( int checkState );
    void                        slotEchoCancelEnableCheckBox( int checkState );

    void                        slotGenerateToneCheckBox( int checkState );

protected:
    void                        showEvent( QShowEvent* ev ) override;
    void                        hideEvent( QHideEvent* ev ) override;

    void                        showEchoDelayTestResults( void );

    void                        loadAppSettings( void );
    void                        saveAppSettings( void );

    void                        setEchoDelay( int echoDelay );

    //=== vars ===//
    Ui::EchoCancelTestMainWindowUi   ui;
    AppCommon&                  m_MyApp;

    QTimer*                     m_PeakTimer{ nullptr };

    QMediaDevices*              m_devices = nullptr;
    QScopedPointer<AudioTestGenerator>   m_Test8000HzMono200HzToneGenerator;
    EAudioTestState             m_AudioTestState{ eAudioTestStateNone };
    std::vector<int>            m_EchoDelayResultList;
};
