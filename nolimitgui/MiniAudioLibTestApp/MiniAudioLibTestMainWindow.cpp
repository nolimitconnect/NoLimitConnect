//============================================================================
// Copyright (C) 2013 Brett R. Jones
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
// https://www.nolimitconnect.com
//============================================================================

#include "MiniAudioLibTestMainWindow.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include "GuiParams.h"
#include "SoundMgr.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QAudioSink>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QLabel>
#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QScopedPointer>
#include <QMediaDevices>

namespace
{
    const int MAX_INFO_MSG_SIZE = 2048;
}

//============================================================================
MiniAudioLibTestMainWindow::MiniAudioLibTestMainWindow( AppCommon& app, QWidget* parent )
: QMainWindow( parent )
, m_MyApp( app )
, m_PeakTimer( new QTimer( this ) )
{
    ui.setupUi( this );

    ui.m_AudioInPeakProgressBar->setValue( 0 );
    ui.m_AudioOutPeakProgressBar->setValue( 0 );
    m_PeakTimer->setInterval( 200 );

    int echoDelayMs = m_MyApp.getAppSettings().getEchoDelayParam();
    ui.m_EchoDelayLineEdit->setText( QString::number( echoDelayMs ) );
    ui.m_TestDelayResultLineEdit->setText( QString::number( 0 ) );

    bool echoCancelEnable = m_MyApp.getAppSettings().getEchoCancelEnable();
    ui.m_EchoCancelEnableCheckBox->setChecked( echoCancelEnable );
    ui.m_EchoCancelEnableCheckBox->setVisible( false );

    // connect( ui.m_EchoCancelEnableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotMiniAudioLibEnableChange(int)) );
    // connect( ui.m_SendMicToSpeakerCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotSendMicToSpeakesChange( int ) ) );

    connect( ui.m_TestSoundDelayButton, SIGNAL(clicked()), this, SLOT( slotStartTestSoundDelay() ) );
    connect( ui.m_EchoDelaySaveButton, SIGNAL(clicked()), this, SLOT( slotEchoDelaySaveButtonClicked() ) );
    connect( &m_MyApp.getSoundMgr(), SIGNAL(signalTestedSoundDelay(int)), this, SLOT(slotTestedSoundDelayResult(int)), Qt::QueuedConnection );
    connect( &m_MyApp.getSoundMgr(), SIGNAL(signalAudioTestState(EAudioTestState)), this, SLOT(slotAudioTestState(EAudioTestState)), Qt::QueuedConnection );
    connect( &m_MyApp.getSoundMgr(), SIGNAL(signalAudioTestMsg(QString)), this, SLOT(slotAudioTestMsg(QString)), Qt::QueuedConnection );

    QAudioFormat mixerFormat;
    mixerFormat.setSampleRate( ECHO_SAMPLE_RATE );
    mixerFormat.setChannelCount( 1 );
    mixerFormat.setSampleFormat( QAudioFormat::Int16 );

    int toneSampleRateHz = 200;
    double fps = 1.0 / (double)toneSampleRateHz;
    int durationMicroSeconds = fps * 1000000;
    m_Test8000HzMono200HzToneGenerator.reset( new AudioTestGenerator( mixerFormat, durationMicroSeconds, toneSampleRateHz ) );
    updateOutAudioDevices();

    int soundOutDeviceIndex = 0;
    m_MyApp.getSoundMgr().getSoundOutDeviceIndex( soundOutDeviceIndex );
    ui.m_OutDeviceComboBox->setCurrentIndex( soundOutDeviceIndex );

    updateInAudioDevices();

    int soundInDeviceIndex = 0;
    m_MyApp.getSoundMgr().getSoundInDeviceIndex( soundInDeviceIndex );
    ui.m_InDeviceComboBox->setCurrentIndex( soundInDeviceIndex );

    connect( ui.m_InDeviceComboBox, QOverload<int>::of( &QComboBox::activated ), this, &MiniAudioLibTestMainWindow::inDeviceChanged );
    connect( ui.m_ApplyDefaultInDeviceButton, SIGNAL( clicked() ), this, SLOT( slotApplyInDeviceChange() ) );

    connect( ui.m_OutDeviceComboBox, QOverload<int>::of( &QComboBox::activated ), this, &MiniAudioLibTestMainWindow::outDeviceChanged );
    connect( ui.m_ApplyDefaultOutDeviceButton, SIGNAL(clicked()), this, SLOT(slotApplyOutDeviceChange()) );

    connect( ui.m_WantMicrophoneCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotWantMicrophoneCheckBox( int ) ) );
    connect( ui.m_MuteMicrophoneCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotMuteMicrophoneCheckBox( int ) ) );

    connect( ui.m_SendMicToSpeakerCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotSendMicToSpeakerCheckBox( int ) ) );
    connect( ui.m_DirectLoopbackCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotDirectMicToSpeakerCheckBox( int ) ) );

    connect( ui.m_WantSpeakerCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotWantSpeakerCheckBox( int ) ) );
    connect( ui.m_MuteSpeakersCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotMuteSpeakerCheckBox( int ) ) );

    connect( ui.m_EchoCancelEnableCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotMiniAudioLibEnableCheckBox( int ) ) );

    connect( ui.m_GenerateToneCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotGenerateToneCheckBox( int ) ) );

    connect( m_PeakTimer, SIGNAL( timeout() ), this, SLOT( slotPeakTimerTimeout() ) );
    m_PeakTimer->start();
}

//============================================================================
MiniAudioLibTestMainWindow::~MiniAudioLibTestMainWindow()
{
    m_PeakTimer->stop();
    m_MyApp.getSoundMgr().setDirectLoopbackEnable( false );
    m_MyApp.getSoundMgr().setEchoCancelLoopbackEnable( false );
}

//============================================================================
void MiniAudioLibTestMainWindow::setStatusLabel( QString strMsg )
{
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void MiniAudioLibTestMainWindow::showEvent( QShowEvent* ev )
{
    QMainWindow::showEvent( ev );
    static bool firstShow = true;
    if( firstShow )
    {
        firstShow = false;
        m_MyApp.setMainWindow( ui.m_CentralWidget );
        m_MyApp.startupAppCommon();
        loadAppSettings();
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::hideEvent( QHideEvent* ev )
{
    QMainWindow::hideEvent( ev );
}

//============================================================================
void MiniAudioLibTestMainWindow::toGuiInfoMsg( char * infoMsg )
{
    QString infoStr( infoMsg );

    static QRegularExpression removeExpress( "[\\n\\r]" );
    infoStr.remove( removeExpress );

    emit signalInfoMsg( infoStr );
}

//============================================================================
void MiniAudioLibTestMainWindow::infoMsg( const char* errMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, errMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), errMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );

    toGuiInfoMsg( as8Buf );
}

//============================================================================
void MiniAudioLibTestMainWindow::statusMsg( const char* errMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, errMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), errMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );

    setStatusLabel( as8Buf );
}

//============================================================================
void MiniAudioLibTestMainWindow::inDeviceChanged( int index )
{
    m_MyApp.getSoundMgr().soundInDeviceChanged( index );
}

//============================================================================
void MiniAudioLibTestMainWindow::updateInAudioDevices( void )
{
    ui.m_InDeviceComboBox->clear();
    std::vector<std::string>& inDeviceList = m_MyApp.getSoundMgr().getAudioInDevices();

    int devIndex = 0;
    for( auto& deviceDesc : inDeviceList )
    {
        ui.m_InDeviceComboBox->addItem( deviceDesc.c_str(), QVariant::fromValue( devIndex ) );
        devIndex++;
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::outDeviceChanged( int index )
{
    m_MyApp.getSoundMgr().soundOutDeviceChanged( index );
}

//============================================================================
void MiniAudioLibTestMainWindow::updateOutAudioDevices( void )
{
    ui.m_OutDeviceComboBox->clear();
    std::vector<std::string>& inDeviceList = m_MyApp.getSoundMgr().getAudioOutDevices();

    int devIndex = 0;
    for( auto& deviceDesc : inDeviceList )
    {
        ui.m_OutDeviceComboBox->addItem( deviceDesc.c_str(), QVariant::fromValue( devIndex ) );
        devIndex++;
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::slotApplyInDeviceChange( void )
{
    QString sndInDevDescription = ui.m_InDeviceComboBox->currentText();
    if( !sndInDevDescription.isEmpty() )
    {
        if( m_MyApp.getSoundMgr().soundInDeviceChanged( ui.m_InDeviceComboBox->currentIndex() ) )
        {
            m_MyApp.getSoundMgr().setSoundInDeviceIndex( ui.m_InDeviceComboBox->currentIndex() );
            QMessageBox::information( this, QObject::tr( "Sound In Device" ), sndInDevDescription + QObject::tr( "Preferred Sound In Device is now " ) + sndInDevDescription, QMessageBox::Ok );
        }
        else
        {
            QMessageBox::information( this, QObject::tr( "Sound In Device" ), sndInDevDescription + QObject::tr( " failed to initialize" ), QMessageBox::Ok );
        }
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "Sound In Device" ), QObject::tr( "No Sound In Device Is Available" ), QMessageBox::Ok );
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::slotApplyOutDeviceChange( void )
{
    QString sndOutDevDescription = ui.m_OutDeviceComboBox->currentText();
    if( !sndOutDevDescription.isEmpty() )
    {
        if( m_MyApp.getSoundMgr().soundOutDeviceChanged( ui.m_OutDeviceComboBox->currentIndex() ) )
        {
            m_MyApp.getAppSettings().setSoundOutDeviceIndex( ui.m_OutDeviceComboBox->currentIndex() );
            QMessageBox::information( this, QObject::tr( "Sound Out Device" ), sndOutDevDescription + QObject::tr( "Preferred Sound Out Device is now " ) + sndOutDevDescription, QMessageBox::Ok );
        }
        else
        {
            QMessageBox::information( this, QObject::tr( "Sound Out Device" ), sndOutDevDescription + QObject::tr( " failed to initialize" ), QMessageBox::Ok );
        }
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "Sound Out Device" ), QObject::tr( "No Sound Out Device Is Available" ), QMessageBox::Ok );
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::slotPeakTimerTimeout( void )
{
    ui.m_AudioInPeakProgressBar->setValue( m_MyApp.getSoundMgr().getAudioInPeakAmplitude() );
    ui.m_AudioOutPeakProgressBar->setValue( m_MyApp.getSoundMgr().getAudioOutPeakAmplitude() );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotStartTestSoundDelay( void )
{
    if( eAudioTestStateNone == m_AudioTestState )
    {
        m_EchoDelayResultList.clear();
        m_MyApp.getSoundMgr().runAudioDelayTest();
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "Echo delay test is running" ), QObject::tr( "Echo delay test can not be run until the previous test finishes" ), QMessageBox::Ok );
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::slotEchoDelaySaveButtonClicked( void )
{
    int delayMs = ui.m_EchoDelayLineEdit->text().toInt();
    if( delayMs < 40 || delayMs > 500 )
    {
        QMessageBox::information( this, QObject::tr( "Echo Delay Value Invalid" ), QObject::tr( "Echo Delay value must be between 40 and 500 milliseconds" ), QMessageBox::Ok );
    }
    else
    {
        m_MyApp.getAppSettings().setEchoDelayParam( delayMs );
        m_MyApp.getSoundMgr().setEchoDelayMsParam( delayMs );
        QMessageBox::information( this, QObject::tr( "Echo Delay Value Save" ), QObject::tr( "Echo Delay value has been saved for use by Echo Cancelation" ), QMessageBox::Ok );
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::slotAudioTestState( EAudioTestState audioTestState )
{
    switch( audioTestState )
    {
    case eAudioTestStateInit:
        break;

    case eAudioTestStateRun:
        break;

    case eAudioTestStateDone:
        showEchoDelayTestResults();
        break;

    case eAudioTestStateNone:
    default:
        break;
    }
}

//============================================================================
void MiniAudioLibTestMainWindow::slotTestedSoundDelayResult( int echoDelayMs )
{
    m_EchoDelayResultList.push_back( echoDelayMs );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotAudioTestMsg( QString audioTestMsg )
{
    setStatusLabel( audioTestMsg );
}

//============================================================================
void MiniAudioLibTestMainWindow::showEchoDelayTestResults( void )
{
    if( !m_EchoDelayResultList.empty() )
    {
        bool resultsValid{ true };

        QString resultMsg( QObject::tr( "Echo Delays " ) );
        bool firstResult{ true };
        int averageDelay = 0;
        for( auto delayMs : m_EchoDelayResultList )
        {
            if( delayMs < 40 || delayMs > 500 )
            {
                resultsValid = false;
            }

            averageDelay += delayMs;
            if( !firstResult )
            {
                resultMsg += QObject::tr( ", ");
            }
            else
            {
                firstResult = false;
            }

            resultMsg += QString::number( delayMs );
        }

        averageDelay = averageDelay / m_EchoDelayResultList.size();
        setStatusLabel( resultMsg );

        ui.m_TestDelayResultLineEdit->setText( QString::number( averageDelay - 5 ) );
        resultMsg += resultsValid ? QObject::tr( "\nDelay Test Is Valid\n" ) : QObject::tr( "\nDelay Test Is Invalid\n" );

        if( resultsValid )
        {
            QString msg( QObject::tr( "If you are having echo issues you may want to enter value " ) );
            msg += QString::number( averageDelay - 5 );
            msg += QObject::tr( " into  Echo delay ms field and click Save Echo Delay To Echo Canceller button\n" );
            msg += resultMsg;
            QMessageBox::information( this, QObject::tr( "Echo Delay Test Is Valid" ), msg, QMessageBox::Ok );
        }
        else
        {   
            QMessageBox::information( this, QObject::tr( "Echo Delay Test Is Invalid. Check microphone and speaker. Try turning up the volume or placing microphone closer to speaker" ), resultMsg, QMessageBox::Ok );
        }
    }  
}

//============================================================================
void MiniAudioLibTestMainWindow::loadAppSettings( void )
{
    int	echoDelay = m_MyApp.getAppSettings().getEchoDelayParam();
    bool enableMiniAudioLib = m_MyApp.getAppSettings().getEchoCancelEnable();

    int32_t	micDeviceIdx = m_MyApp.getAppSettings().getSoundInDeviceIndex();
    bool wantMicInput = m_MyApp.getAppSettings().getWantMicrophone();
    bool muteMicrophone = m_MyApp.getAppSettings().getMuteMicrophone();
    bool sendMicInToSpeaker = m_MyApp.getAppSettings().getSendMicInToSpeaker();

    int32_t	speakerDeviceIdx = m_MyApp.getAppSettings().getSoundOutDeviceIndex();
    bool wantSpeakerOutput = m_MyApp.getAppSettings().getWantSpeaker();
    bool muteSpeaker = m_MyApp.getAppSettings().getMuteSpeaker();

    setEchoDelay( echoDelay );

    // setup echo cancel enable
    ui.m_EchoCancelEnableCheckBox->setChecked( enableMiniAudioLib );
    m_MyApp.getSoundMgr().setEchoCancelEnable( enableMiniAudioLib );

    // setup speaker first
    if( speakerDeviceIdx < ui.m_OutDeviceComboBox->count() )
    {
        ui.m_OutDeviceComboBox->setCurrentIndex( speakerDeviceIdx );
    }
    else
    {
        speakerDeviceIdx = 0;
        ui.m_OutDeviceComboBox->setCurrentIndex( speakerDeviceIdx );
        m_MyApp.getAppSettings().setSoundOutDeviceIndex( speakerDeviceIdx );
    }

    ui.m_MuteSpeakersCheckBox->setChecked( muteSpeaker );
    m_MyApp.getSoundMgr().setMuteSpeaker( muteSpeaker );
    ui.m_WantSpeakerCheckBox->setChecked( wantSpeakerOutput );

    // setup microphone
    if( micDeviceIdx < ui.m_InDeviceComboBox->count() )
    {
        ui.m_InDeviceComboBox->setCurrentIndex( micDeviceIdx );
    }
    else
    {
        micDeviceIdx = 0;
        ui.m_InDeviceComboBox->setCurrentIndex( micDeviceIdx );
        m_MyApp.getAppSettings().setSoundInDeviceIndex( micDeviceIdx );
    }

    ui.m_MuteMicrophoneCheckBox->setChecked( muteMicrophone );
    m_MyApp.getSoundMgr().setMuteMicrophone( muteMicrophone );
    ui.m_WantMicrophoneCheckBox->setChecked( wantMicInput );

    // loopback sound 
    ui.m_SendMicToSpeakerCheckBox->setChecked( sendMicInToSpeaker );
    m_MyApp.getSoundMgr().setEchoCancelLoopbackEnable( sendMicInToSpeaker );
}

//============================================================================7
void MiniAudioLibTestMainWindow::saveAppSettings( void )
{
    int	echoDelay = ui.m_EchoDelayLineEdit->text().toInt();
    m_MyApp.getAppSettings().setEchoDelayParam( echoDelay );

    bool enableMiniAudioLib = ui.m_EchoCancelEnableCheckBox->isChecked();
    m_MyApp.getAppSettings().setEchoCancelEnable( enableMiniAudioLib );

    int32_t	micDeviceIdx = ui.m_InDeviceComboBox->currentIndex();
    m_MyApp.getAppSettings().setSoundInDeviceIndex( micDeviceIdx );

    bool wantMicInput = ui.m_WantMicrophoneCheckBox->isChecked();
    m_MyApp.getAppSettings().setWantMicrophone( wantMicInput );

    bool muteMicrophone = ui.m_MuteMicrophoneCheckBox->isChecked();
    m_MyApp.getAppSettings().setMuteMicrophone( muteMicrophone );

    bool sendMicInToSpeaker = ui.m_SendMicToSpeakerCheckBox->isChecked();
    m_MyApp.getAppSettings().setSendMicInToSpeaker( sendMicInToSpeaker );

    int32_t	speakerDeviceIdx = ui.m_OutDeviceComboBox->currentIndex();
    m_MyApp.getAppSettings().setSoundOutDeviceIndex( speakerDeviceIdx );

    bool wantSpeakerOutput = ui.m_WantSpeakerCheckBox->isChecked();
    m_MyApp.getAppSettings().setWantSpeaker( wantSpeakerOutput );

    bool muteSpeaker = ui.m_MuteSpeakersCheckBox->isChecked();
    m_MyApp.getAppSettings().setMuteSpeaker( muteSpeaker );
}

//============================================================================
void MiniAudioLibTestMainWindow::setEchoDelay( int echoDelayMs )
{
    m_MyApp.getSoundMgr().setEchoDelayMsParam( echoDelayMs );
    ui.m_EchoDelayLineEdit->setText( QString::number( echoDelayMs ) );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotWantMicrophoneCheckBox( int checkState )
{
    bool wantMicInput = ui.m_WantMicrophoneCheckBox->isChecked();
    m_MyApp.getAppSettings().setWantMicrophone( wantMicInput );
    m_MyApp.getSoundMgr().toGuiWantMicrophoneRecording( eAppModuleMicrophone, wantMicInput );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotMuteMicrophoneCheckBox( int checkState )
{
    bool muteMicInput = ui.m_MuteMicrophoneCheckBox->isChecked();
    m_MyApp.getAppSettings().setWantMicrophone( muteMicInput );
    m_MyApp.getSoundMgr().setMuteMicrophone( muteMicInput );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotSendMicToSpeakerCheckBox( int checkState )
{
    bool sendMicToSpeaker = ui.m_SendMicToSpeakerCheckBox->isChecked();
    m_MyApp.getAppSettings().setSendMicInToSpeaker( sendMicToSpeaker );
    m_MyApp.getSoundMgr().setEchoCancelLoopbackEnable( sendMicToSpeaker );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotDirectMicToSpeakerCheckBox( int checkState )
{
    bool directMicToSpeaker = ui.m_DirectLoopbackCheckBox->isChecked();
    m_MyApp.getSoundMgr().setDirectLoopbackEnable( directMicToSpeaker );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotWantSpeakerCheckBox( int checkState )
{
    bool wantSpeakerOutput = ui.m_WantSpeakerCheckBox->isChecked();
    m_MyApp.getAppSettings().setWantSpeaker( wantSpeakerOutput );
    m_MyApp.getSoundMgr().toGuiWantSpeakerOutput( eAppModuleMicrophone, wantSpeakerOutput );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotMuteSpeakerCheckBox( int checkState )
{
    bool muteSpeaker = ui.m_MuteSpeakersCheckBox->isChecked();
    m_MyApp.getAppSettings().setMuteSpeaker( muteSpeaker );
    m_MyApp.getSoundMgr().setMuteSpeaker( muteSpeaker );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotMiniAudioLibEnableCheckBox( int checkState )
{
    bool enableMiniAudioLib = ui.m_EchoCancelEnableCheckBox->isChecked();
    m_MyApp.getAppSettings().setEchoCancelEnable( enableMiniAudioLib );
    m_MyApp.getSoundMgr().setEchoCancelEnable( enableMiniAudioLib );
}

//============================================================================
void MiniAudioLibTestMainWindow::slotGenerateToneCheckBox( int checkState )
{
    bool enableTone = ui.m_GenerateToneCheckBox->isChecked();
    m_MyApp.getSoundMgr().setEnableSpeakerTestTone( enableTone );
}
