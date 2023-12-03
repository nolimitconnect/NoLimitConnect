//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "MiniAudioOut.h"
#include "MiniAudioMgr.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTimer.h>

#include <QDebug>
#include <QtEndian>
#include <QTimer>
#include <QMessageBox>

#include <math.h>

//============================================================================
MiniAudioOut::MiniAudioOut( MiniAudioMgr& mgr, QObject *parent )
: MiniAudioOutDevice( mgr )
, m_MyApp( mgr.getMyApp() )
{
}

//============================================================================
bool MiniAudioOut::initAudioOut( QAudioFormat& audioFormat, int deviceIndex )
{
    m_AudioFormat = audioFormat;
    soundOutDeviceChanged( deviceIndex );
    audioFormat = m_AudioFormat;
    return m_initialized;
}

//============================================================================
bool MiniAudioOut::soundOutDeviceChanged( int deviceIndex )
{
    int deviceCount = m_AudioIoMgr.getAudioOutDeviceCount();
    if( !deviceCount )
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound Out Device" ), QObject::tr( "No Sound Output Devices Avalable" ), QMessageBox::Ok );
        return false;
    }

    if( deviceIndex >= deviceCount )
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound Out Device" ), QObject::tr( "Sound Output Device Index Out Of Range. Will Use Default Device" ), QMessageBox::Ok );
        deviceIndex = 0;
    }

    stopAudioOut();
    if( !m_AudioFormat.sampleRate() )
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound Out Device" ), QObject::tr( "Sound Output Device Invalid Format" ), QMessageBox::Ok );
        deviceIndex = 0;
        m_AudioFormat.setSampleRate( AUDIO_DEVICE_SAMPLE_RATE );
        m_AudioFormat.setChannelCount( AUDIO_CHANNELS );
    }

    int preferredDeviceIndex = deviceIndex;
    int actualRate = 0;
    m_initialized = initializeAudioOutDevice( preferredDeviceIndex, m_AudioFormat.sampleRate(), actualRate );
    if( m_initialized )
    {
        m_AudioDeviceIndex = preferredDeviceIndex;
        if( actualRate != m_AudioFormat.sampleRate() )
        {
            m_AudioFormat.setSampleRate( actualRate );
            m_AudioIoMgr.setOutSampleRate( actualRate );
        }

        if( preferredDeviceIndex != deviceIndex )
        {
            m_AudioIoMgr.setSoundInDeviceIndex( deviceIndex );
        }

        setUpsampleMultiplier( m_AudioFormat.sampleRate() / AUDIO_DEVICE_SAMPLE_RATE );

        if( m_AudioIoMgr.isSpeakerOutputWanted() )
        {
            startAudioOut();
        }
    }
    else
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound Out Device" ), QObject::tr( "Could not initialize sound out device " ) + m_AudioIoMgr.getAudioOutDeviceDesc( deviceIndex ).c_str(), QMessageBox::Ok );
    }

    return m_initialized;
}

//============================================================================
void MiniAudioOut::wantSpeakerOutput( bool enableOutput )
{
    m_SpeakerOutputEnabled = enableOutput;

    if( enableOutput )
    {
        m_AudioOutDeviceIsStarted = startAudioOutDevice();
    }
    else
    {
        stopAudioOutDevice();
        m_AudioOutDeviceIsStarted = false;
    }
}

//============================================================================
void MiniAudioOut::startAudioOut( void )
{
    if( !m_AudioOutDeviceIsStarted )
    {
        m_AudioOutDeviceIsStarted = startAudioOutDevice();
        if( m_AudioOutDeviceIsStarted )
        {
            m_AudioIoMgr.speakerDeviceEnabled( true );
        }
        else
        {
            LogMsg( LOG_DEBUG, "MiniAudioIn::startAudioOut failed " );
        }
    } 
}

//============================================================================
void MiniAudioOut::stopAudioOut( void )
{
    if( m_AudioOutDeviceIsStarted )
    {
        m_AudioOutDeviceIsStarted = false;
        stopAudioOutDevice();
        m_AudioIoMgr.speakerDeviceEnabled( false );
    }
}

//============================================================================
void MiniAudioOut::setSpeakerVolume( int volume0to100 )
{
    //qreal linearVolume = QAudio::convertVolume( volume0to100 / qreal( 100 ),
    //    QAudio::LogarithmicVolumeScale,
    //    QAudio::LinearVolumeScale );

    //m_AudioOutputDevice->setVolume( linearVolume );
}

//============================================================================
void MiniAudioOut::echoCancelSyncStateThreaded( bool inSync )
{

}

//============================================================================
int MiniAudioOut::callbackAudioRead( int16_t* pcmData, int sampleCnt )
{
    if( VxIsAppShuttingDown() )
    {
        // do not attempt anything while being destroyed
        return 0;
    }

    if( !pcmData )
    {
        LogMsg( LOG_ERROR, "MiniAudioOut::callbackAudioRead has null pcmData." );
        return sampleCnt;
    }

    if( !sampleCnt )
    {
        LogMsg( LOG_ERROR, "MiniAudioOut::callbackAudioRead has 0 sampleCnt.");
        return sampleCnt;
    }

    m_AudioIoMgr.callbackToSpeakerRead( pcmData, sampleCnt );
    return sampleCnt;

    /*
    if( !m_AudioIoMgr.isAudioInitialized() || m_AudioIoMgr.getIsSpeakerMuted() )
    {
        memset( pcmData, 0, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
        return sampleCnt;
    }

    if( m_AudioIoMgr.getEnableSpeakerTestTone() )
    {
        m_AudioIoMgr.readTestToneSamples( pcmData, sampleCnt );

        return sampleCnt;
    }

    if( m_AudioIoMgr.getDirectLoopbackEnable() )
    {
        m_AudioIoMgr.directLoopbackRead( pcmData, sampleCnt );
        m_PeakAudioOutAmplitude = AudioUtils::peakPcmAmplitude0to100( pcmData, sampleCnt );
        bool hasAmp{ false };
        for( int i = 0; i < sampleCnt; i++ )
        {
            if( pcmData[ i ] )
            {
                hasAmp = true;
                break;
            }
        }

        if( !hasAmp )
        {
            LogMsg( LOG_ERROR, "MiniAudioOut::callbackAudioRead has 0 m_PeakAudioOutAmplitude." );
        }

        return sampleCnt;
    }

    static int64_t lastTime = 0;
    int64_t timeNow = GetHighResolutionTimeMs();
    int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
    lastTime = timeNow;
    if( m_AudioIoMgr.getAudioTimingDebugEnable() )
    {
        LogMsg( LOG_VERBOSE, "MiniAudioOut::callbackAudioRead samples %d ms %d", sampleCnt, timeElapsed );
    }

    static int lastSpeakerReadSamples = 0;
    static int audioReadDurationUs = 0;
    if( sampleCnt != lastSpeakerReadSamples )
    {
        // first time or device changed or read buffer len changed
        audioReadDurationUs = (int)AudioUtils::audioDurationUs( m_AudioFormat.sampleRate(), sampleCnt );
        m_SpeakerReadTimeEstimator.setIntervalUs( audioReadDurationUs );
        lastSpeakerReadSamples = sampleCnt;
    }

    int64_t speakerReadTimeMs = m_SpeakerReadTimeEstimator.estimateTime( timeNow );
    int speakerReqSampleCnt = sampleCnt;

    if( m_AudioTestState != eAudioTestStateNone )
    {
        memset( pcmData, 0, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
        if( m_AudioTestState == eAudioTestStateRun && !m_AudioTestSentTimeMs )
        {
            int16_t* sampleBuf = (int16_t*)pcmData;
            // create a 480 hz square wave tone for 10 ms as a sound to be detected by microphone for delay timing test
            int maxSamplesToSet = AudioUtils::audioSamplesRequiredForGivenMs( m_AudioFormat, 10 );
            maxSamplesToSet = std::min( maxSamplesToSet, sampleCnt );
            int samplesCycle = (m_AudioFormat.sampleRate() * m_AudioFormat.channelCount()) / (480 * 2);
            bool sampleIsMax{ true };
            for( int i = 0; i < maxSamplesToSet; i += samplesCycle )
            {
                int16_t sampVal = sampleIsMax ? 32767 : -32768;
                for( int j = 0; j < samplesCycle && ((j + i) < maxSamplesToSet); j++ )
                {
                    sampleBuf[ i + j ] = sampVal;
                }

                sampleIsMax = !sampleIsMax;
            }

            m_AudioTestSentTimeMs = speakerReadTimeMs;
        }

        m_AudioIoMgr.getAudioMasterClock().audioSpeakerReadDurationTime( audioReadDurationUs / 1000, speakerReadTimeMs );
        if( m_AudioIoMgr.getSampleCntDebugEnable() )
        {
            m_AudioIoMgr.getAudioMasterClock().audioMicWriteSampleCnt( speakerReqSampleCnt );
        }

        return sampleCnt;
    }

    int readAmount;
    if( m_AudioIoMgr.getAudioLoopbackEnable() )
    {
        readAmount = m_AudioIoMgr.getAudioLoopback().readRequestFromSpeaker( pcmData, sampleCnt );
    }
    else
    {
        readAmount = m_AudioIoMgr.getAudioOutMixer().readRequestFromSpeaker( pcmData, sampleCnt );
    }

    if( readAmount != sampleCnt )
    {
        LogMsg( LOG_DEBUG, "MiniAudioOut::readData mismatch with maxlen %d and read %d", sampleCnt, readAmount );
    }

    if( m_AudioIoMgr.getPeakAmplitudeDebugEnable() )
    {
        static int64_t lastMixerPcmTime{ 0 };
        static int funcCallCnt{ 0 };
        funcCallCnt++;
        if( lastMixerPcmTime )
        {
            LogMsg( LOG_VERBOSE, "MiniAudioOut::readData %d peak amplitude %d", funcCallCnt, m_PeakAudioOutAmplitude );
        }

        lastMixerPcmTime = timeNow;
    }

    if( m_AudioIoMgr.getIsEchoCancelEnabled() )
    {
        m_AudioIoMgr.getAudioEchoCancel().speakerReadSamples( m_EchoFarBuffer.getSampleBuffer(), m_EchoFarBuffer.getSampleCnt(),
            speakerReadTimeMs + (audioReadDurationUs / 1000), m_SpeakerReadTimeEstimator.getHasMaxTimestamps() );
    }

    m_EchoFarBuffer.clear();

    // master clock is based on speaker read event/length
    // using audioReadDurationUs if using fixed buffer size  has rounding errors.. have to use elapsed time us
    //m_AudioIoMgr.getAudioMasterClock().audioSpeakerReadUs( thisCallTimeUs - lastCallTimeUs, false );
    //lastCallTimeUs = thisCallTimeUs;

    m_AudioIoMgr.getAudioMasterClock().audioSpeakerReadDurationTime( audioReadDurationUs / 1000, speakerReadTimeMs );

    if( m_AudioIoMgr.getIsSpeakerMuted() )
    {
        memset( pcmData, 0, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
        m_PeakAudioOutAmplitude = 0;
    }
    else
    {
        m_PeakAudioOutAmplitude = AudioUtils::peakPcmAmplitude0to100( pcmData, sampleCnt );
    }
    */

    return sampleCnt;
}
