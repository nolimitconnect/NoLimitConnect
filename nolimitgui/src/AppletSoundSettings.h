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

#include "AppletClientBase.h"

#include "GuiAudioLevelCallback.h"
#include "GuiUserUpdateCallback.h"

#include "miniaudio/AudioTestGenerator.h"

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IAudioDefs.h>

#include <QString>
#include <QDialog>
#include <QMediaDevices>
#include <QIODevice>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletSoundSettingsUi;
}
QT_END_NAMESPACE

class VxNetIdent;
class GuiHostSession;

class AppletSoundSettings : public AppletClientBase, public GuiAudioLevelCallback
{
	Q_OBJECT
public:
	AppletSoundSettings( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletSoundSettings() override;

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

    void                        slotStartTestSoundDelay( void );

    void                        slotTestedSoundDelayResult( int echoDelayMs );

    void                        slotAudioTestState( EAudioTestState audioTestState );
    void                        slotAudioTestMsg( QString audioTestMsg );

    void                        slotGenerateToneCheckBox( int checkedState );

    void                        slotSendMicEchoCanceledToSpeakerCheckBox( int checkedState );

protected:
    void                        showEvent( QShowEvent* ev ) override;
    void                        hideEvent( QHideEvent* ev ) override;

    void                        showEchoDelayTestResults( void );

    void						callbackGuiMicrophoneLevel( int micLevel ) override;

    //=== vars ===//
    Ui::AppletSoundSettingsUi&  ui;

    QMediaDevices*              m_devices = nullptr;
    QScopedPointer<AudioTestGenerator>   m_Test8000HzMono200HzToneGenerator;
    EAudioTestState             m_AudioTestState{ eAudioTestStateNone };
    std::vector<int>            m_EchoDelayResultList;
};
