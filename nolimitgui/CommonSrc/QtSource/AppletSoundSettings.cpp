//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSoundSettings.h"

#include "ActivityInformation.h"
#include "ActivityMessageBox.h"

#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppSettings.h"

#include "GuiHostSession.h"
#include "GuiParams.h"
#include "GuiHelpers.h"
#include "SoundMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/ObjectCommonDefs.h>
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
AppletSoundSettings::AppletSoundSettings( AppCommon& app, QWidget*	parent )
: AppletClientBase( OBJNAME_APPLET_SOUND_SETTINGS, app, parent )
, m_PeakTimer( new QTimer( this ) )
, m_devices( new QMediaDevices( this ) )
{
    setAppletType( eAppletSoundSettings );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_AudioInPeakProgressBar->setValue( 0 );
    ui.m_AudioOutPeakProgressBar->setValue( 0 );

    m_PeakTimer->setInterval( 200 );

    int echoDelayMs = m_MyApp.getAppSettings().getEchoDelayParam();
    ui.m_EchoDelayLineEdit->setText( QString::number( echoDelayMs ) );
    ui.m_TestDelayResultLineEdit->setText( QString::number( 0 ) );

    bool echoCancelEnable = m_MyApp.getAppSettings().getEchoCancelEnable();
    ui.m_EchoCancelEnableCheckBox->setChecked( echoCancelEnable );
    ui.m_EchoCancelEnableCheckBox->setVisible( false );

    connectBarWidgets();

    connect( ui.m_TestSoundDelayButton, SIGNAL(clicked()), this, SLOT( slotStartTestSoundDelay() ) );
    connect( ui.m_EchoDelaySaveButton, SIGNAL(clicked()), this, SLOT( slotEchoDelaySaveButtonClicked() ) );
    connect( &m_MyApp.getSoundMgr(), SIGNAL(signalTestedSoundDelay(int)), this, SLOT(slotTestedSoundDelayResult(int)), Qt::QueuedConnection );
    connect( &m_MyApp.getSoundMgr(), SIGNAL(signalAudioTestState(EAudioTestState)), this, SLOT(slotAudioTestState(EAudioTestState)), Qt::QueuedConnection );
    connect( &m_MyApp.getSoundMgr(), SIGNAL(signalAudioTestMsg(QString)), this, SLOT(slotAudioTestMsg(QString)), Qt::QueuedConnection );

    //connect( ui.m_EchoCancelEnableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotEchoCancelEnableChange(int)) );

    connect( ui.m_MuteMicrophoneCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotMuteMicrophoneCheckBox( int ) ) );
    connect( ui.m_MuteSpeakersCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotMuteSpeakerCheckBox( int ) ) );

    connect( ui.m_GenerateToneCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( slotGenerateToneCheckBox(int) ) );
    connect( ui.m_SendMicDirectlyToSpeakersCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( slotSendMicDirectlyToSpeakersCheckBox(int) ) );
    connect( ui.m_SendMicEchoCanceledToSpeakerCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( slotSendMicEchoCanceledToSpeakerCheckBox(int) ) );

    updateInAudioDevices();

    int soundInDeviceIndex = 0;
    m_MyApp.getSoundMgr().getSoundInDeviceIndex( soundInDeviceIndex );
    ui.m_InDeviceComboBox->setCurrentIndex( soundInDeviceIndex );

    connect( ui.m_InDeviceComboBox, QOverload<int>::of( &QComboBox::activated ), this, &AppletSoundSettings::inDeviceChanged );
    connect( m_devices, &QMediaDevices::audioInputsChanged, this, &AppletSoundSettings::updateInAudioDevices );
    connect( ui.m_ApplyDefaultInDeviceButton, SIGNAL(clicked()), this, SLOT( slotApplyInDeviceChange() ) );
    
    updateOutAudioDevices();

    int soundOutDeviceIndex = 0;
    m_MyApp.getSoundMgr().getSoundOutDeviceIndex( soundOutDeviceIndex );
    ui.m_OutDeviceComboBox->setCurrentIndex( soundOutDeviceIndex );

    connect( ui.m_OutDeviceComboBox, QOverload<int>::of( &QComboBox::activated ), this, &AppletSoundSettings::outDeviceChanged );
    connect( m_devices, &QMediaDevices::audioOutputsChanged, this, &AppletSoundSettings::updateOutAudioDevices );
    connect( ui.m_ApplyDefaultOutDeviceButton, SIGNAL(clicked()), this, SLOT(slotApplyOutDeviceChange()) );

    m_MyApp.activityStateChange( this, true );

    connect( m_PeakTimer, SIGNAL( timeout() ), this, SLOT( slotPeakTimerTimeout() ) );
    m_PeakTimer->start();

    if( !m_MyApp.getSoundMgr().isSpeakerDeviceAvailable() )
    {
        QMessageBox::information( this, QObject::tr( "Speaker Device Unavailable" ), QObject::tr( "No speaker device is available to enable" ), QMessageBox::Ok );
    }
    else if( !m_MyApp.getSoundMgr().isMicrophoneDeviceAvailable() )
    {
        m_MyApp.getSoundMgr().playSnd( eSndDefBusy );
        QMessageBox::information( this, QObject::tr( "Microphone Device Unavailable" ), QObject::tr( "No microphone device is available to enable" ), QMessageBox::Ok );
    }
    else
    {
        //m_MyApp.getSoundMgr().setDirectLoopbackEnable( true );
        ui.m_SendMicEchoCanceledToSpeakerCheckBox->setChecked( true );
    }
}

//============================================================================
AppletSoundSettings::~AppletSoundSettings()
{
    m_MyApp.getFromGuiInterface().fromGuiPushToTalk( m_MyApp.getMyOnlineId(), false );

    m_PeakTimer->stop();
    m_MyApp.getSoundMgr().setDirectLoopbackEnable( false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletSoundSettings::setStatusLabel( QString strMsg )
{
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletSoundSettings::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    m_MyApp.wantToGuiActivityCallbacks( this, true );
}

//============================================================================
void AppletSoundSettings::hideEvent( QHideEvent* ev )
{
    m_MyApp.wantToGuiActivityCallbacks( this, false );
    ActivityBase::hideEvent( ev );
}

//============================================================================
void AppletSoundSettings::toGuiInfoMsg( char * infoMsg )
{
    QString infoStr( infoMsg );

    static QRegularExpression removeExpress( "[\\n\\r]" );
    infoStr.remove( removeExpress );

    emit signalInfoMsg( infoStr );
}

//============================================================================
void AppletSoundSettings::infoMsg( const char* errMsg, ... )
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
void AppletSoundSettings::statusMsg( const char* errMsg, ... )
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
void AppletSoundSettings::inDeviceChanged( int index )
{
    m_MyApp.getSoundMgr().soundInDeviceChanged( index );
}

//============================================================================
void AppletSoundSettings::updateInAudioDevices( void )
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
void AppletSoundSettings::outDeviceChanged( int index )
{
    m_MyApp.getSoundMgr().soundOutDeviceChanged( index );
}

//============================================================================
void AppletSoundSettings::updateOutAudioDevices( void )
{
    ui.m_OutDeviceComboBox->clear();

    std::vector<std::string>& outDeviceList = m_MyApp.getSoundMgr().getAudioOutDevices();

    int devIndex = 0;
    for( auto& deviceDesc : outDeviceList )
    {
        ui.m_OutDeviceComboBox->addItem( deviceDesc.c_str(), QVariant::fromValue( devIndex ) );
        devIndex++;
    }
}

//============================================================================
void AppletSoundSettings::slotApplyInDeviceChange( void )
{
    QString sndInDevDescription = ui.m_InDeviceComboBox->currentText();
    if( !sndInDevDescription.isEmpty() )
    {
        if( m_MyApp.getSoundMgr().setSoundInDeviceIndex( ui.m_InDeviceComboBox->currentIndex() ) )
        {
            
            QMessageBox::information( this, QObject::tr( "Sound In Device" ), sndInDevDescription + QObject::tr( " device is saved as preferred Sound In Device" ), QMessageBox::Ok );
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
void AppletSoundSettings::slotApplyOutDeviceChange( void )
{
    QString sndOutDevDescription = ui.m_OutDeviceComboBox->currentText();
    if( !sndOutDevDescription.isEmpty() )
    {
        if( m_MyApp.getSoundMgr().setSoundOutDeviceIndex( ui.m_OutDeviceComboBox->currentIndex() ) )
        {
            m_MyApp.getAppSettings().setSoundOutDeviceIndex( ui.m_OutDeviceComboBox->currentIndex() );
            QMessageBox::information( this, QObject::tr( "Sound Out Device" ), sndOutDevDescription + QObject::tr( " device is saved as preferred Sound Out Device" ), QMessageBox::Ok );
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
void AppletSoundSettings::slotPeakTimerTimeout( void )
{
    ui.m_AudioInPeakProgressBar->setValue( m_MyApp.getSoundMgr().getAudioInPeakAmplitude() );
    ui.m_AudioOutPeakProgressBar->setValue( m_MyApp.getSoundMgr().getAudioOutPeakAmplitude() );
}

//============================================================================
void AppletSoundSettings::slotStartTestSoundDelay( void )
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
void AppletSoundSettings::slotEchoDelaySaveButtonClicked( void )
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
void AppletSoundSettings::slotAudioTestState( EAudioTestState audioTestState )
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
void AppletSoundSettings::slotTestedSoundDelayResult( int echoDelayMs )
{
    m_EchoDelayResultList.push_back( echoDelayMs );
}

//============================================================================
void AppletSoundSettings::slotAudioTestMsg( QString audioTestMsg )
{
    setStatusLabel( audioTestMsg );
}

//============================================================================
void AppletSoundSettings::showEchoDelayTestResults( void )
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
void AppletSoundSettings::slotMuteMicrophoneCheckBox( int checkedState )
{
    bool muteMic = ui.m_MuteMicrophoneCheckBox->isChecked();
    m_MyApp.getSoundMgr().setMuteMicrophone( muteMic );
    ui.m_AudioInPeakProgressBar->setValue( 0 );
    ui.m_AudioOutPeakProgressBar->setValue( 0 );
}

//============================================================================
void AppletSoundSettings::slotMuteSpeakerCheckBox( int checkedState )
{
     bool muteSpeaker = ui.m_MuteSpeakersCheckBox->isChecked();
     m_MyApp.getSoundMgr().setMuteSpeaker( muteSpeaker );
     ui.m_AudioInPeakProgressBar->setValue( 0 );
     ui.m_AudioOutPeakProgressBar->setValue( 0 );
}

//============================================================================
void AppletSoundSettings::slotGenerateToneCheckBox( int checkedState )
{
    bool genTone = ui.m_GenerateToneCheckBox->isChecked();
    m_MyApp.getSoundMgr().setEnableSpeakerTestTone( genTone );
    ui.m_AudioInPeakProgressBar->setValue( 0 );
    ui.m_AudioOutPeakProgressBar->setValue( 0 );
}

//============================================================================
void AppletSoundSettings::slotSendMicDirectlyToSpeakersCheckBox( int checkedState )
{
    if( !m_MyApp.getSoundMgr().isMicrophoneDeviceAvailable() )
    {
        m_MyApp.getSoundMgr().playSnd( eSndDefBusy );
        QMessageBox::information( this, QObject::tr( "Microphone Device Unavailable" ), QObject::tr( "No microphone device is available to enable" ), QMessageBox::Ok );
        return;
    }

    bool directMicToSpeaker = ui.m_SendMicDirectlyToSpeakersCheckBox->isChecked();
    m_MyApp.getSoundMgr().setDirectLoopbackEnable( directMicToSpeaker );
    ui.m_AudioInPeakProgressBar->setValue( 0 );
    ui.m_AudioOutPeakProgressBar->setValue( 0 );
}

//============================================================================
void AppletSoundSettings::slotSendMicEchoCanceledToSpeakerCheckBox( int checkedState )
{
    if( !m_MyApp.getSoundMgr().isMicrophoneDeviceAvailable() )
    {
        m_MyApp.getSoundMgr().playSnd( eSndDefBusy );
        QMessageBox::information( this, QObject::tr( "Microphone Device Unavailable" ), QObject::tr( "No microphone device is available to enable" ), QMessageBox::Ok );
        return;
    }

    bool enableLoopback = ui.m_SendMicEchoCanceledToSpeakerCheckBox->isChecked();
    if( !m_MyApp.getFromGuiInterface().fromGuiPushToTalk( m_MyApp.getMyOnlineId(), enableLoopback ) )
    {
        if( enableLoopback )
        {
            m_MyApp.getSoundMgr().playSnd( eSndDefBusy );
            QMessageBox::information( this, QObject::tr( "Push To Talk" ), QObject::tr( "Sound settings Push To Talk failed to enable" ), QMessageBox::Ok );
        }
    }

    ui.m_AudioInPeakProgressBar->setValue( 0 );
    ui.m_AudioOutPeakProgressBar->setValue( 0 );
}