//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "MiniAudioIn.h"
#include "MiniAudioMgr.h"

#include "AudioUtils.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QDebug>
#include <QtEndian>
#include <QMessageBox>

#include <math.h>

//============================================================================
MiniAudioIn::MiniAudioIn( MiniAudioMgr& mgr, QObject *parent )
: MiniAudioInDevice( mgr )
, m_MyApp( mgr.getMyApp() )
{
    memset( m_MicSilence, 0, sizeof( m_MicSilence ) );

    m_MicInBitrate.setLogMessagePrefix( "Mic in " );
    m_MicOutBitrate.setLogMessagePrefix( "Mic out " );
}

//============================================================================
bool MiniAudioIn::initAudioIn( QAudioFormat& audioFormat, int deviceIndex )
{
    m_AudioFormat = audioFormat;
    soundInDeviceChanged( deviceIndex );
    audioFormat = m_AudioFormat;
    return m_initialized;
}

//============================================================================
bool MiniAudioIn::soundInDeviceChanged( int deviceIndex )
{
    int deviceCount = m_AudioIoMgr.getAudioInDeviceCount();
    if( !deviceCount )
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound In Device" ), QObject::tr( "No Sound Input Devices Avalable" ), QMessageBox::Ok );
        return false;
    }

    if( deviceIndex >= deviceCount )
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound In Device" ), QObject::tr( "Sound Input Device Index Out Of Range. Will Use Default Device" ), QMessageBox::Ok );
        deviceIndex = 0;
    }

    stopAudioIn();
    m_AudioFormat.setSampleRate( AUDIO_DEVICE_SAMPLE_RATE );
    m_AudioFormat.setChannelCount( AUDIO_CHANNELS );

    int preferredDeviceIndex = deviceIndex;
    int actualRate = 0;
    m_initialized = initializeAudioInDevice( preferredDeviceIndex, m_AudioFormat.sampleRate(), actualRate );
    if( m_initialized )
    {
        if( actualRate != m_AudioFormat.sampleRate() )
        {
            m_AudioFormat.setSampleRate( actualRate );
        }

        if( preferredDeviceIndex != deviceIndex )
        {
            m_AudioIoMgr.setSoundInDeviceIndex( deviceIndex );
        }

        setDivideSamplesCount( m_AudioFormat.sampleRate() / AUDIO_DEVICE_SAMPLE_RATE );

        if( m_AudioIoMgr.isMicrophoneInputWanted() )
        {
            startAudioIn();
        }
    }
    else
    {
        QMessageBox::information( nullptr, QObject::tr( "Sound In Device" ), QObject::tr( "Could not initialize sound in device " ) + m_AudioIoMgr.getAudioInDeviceDesc( deviceIndex ).c_str(), QMessageBox::Ok );
    }

    return m_initialized;
}

//============================================================================
void MiniAudioIn::setMicrophoneVolume( int volume0to100 )
{
    //qreal linearVolume = QAudio::convertVolume( volume0to100 / qreal( 100 ),
    //    QAudio::LogarithmicVolumeScale,
    //    QAudio::LinearVolumeScale );

    // m_AudioInputDevice->setVolume( linearVolume );
}

////============================================================================
//static void apply_s16le_volume( float volume, uchar *data, int datalen )
//{
//    int samples = datalen / 2;
//    float mult = pow( 10.0, 0.05 * volume );

//    for( int i = 0; i < samples; i++ ) {
//        qint16 val = qFromLittleEndian<qint16>( data + i * 2 )*mult;
//        qToLittleEndian<qint16>( val, data + i * 2 );
//    }
//}

//============================================================================
void MiniAudioIn::startAudioIn( void )
{
    if( !m_AudioInDeviceIsStarted )
    {
        m_AudioInDeviceIsStarted = startAudioInDevice();
        if( m_AudioInDeviceIsStarted )
        {
            m_AudioIoMgr.microphoneDeviceEnabled( true );
        }
        else
        {
            LogMsg( LOG_DEBUG, "MiniAudioIn::startAudioIn failed " );
        }
    }
}

//============================================================================
void MiniAudioIn::stopAudioIn( void )
{
    if( m_AudioInDeviceIsStarted )
    {
        m_AudioInDeviceIsStarted = false;
        stopAudioInDevice();
        m_AudioIoMgr.microphoneDeviceEnabled( false );
    }
}

//============================================================================
void MiniAudioIn::echoCancelSyncStateThreaded( bool inSync )
{
    m_EchoCancelInSync = inSync;
    // mic write 48000 is eratic timing.. commented out to not clutter the log
    // m_MicInBitrate.setIsBitrateLogEnabled( inSync && m_AudioIoMgr.getBitrateDebugEnable() );
    m_MicOutBitrate.setIsBitrateLogEnabled( inSync && m_AudioIoMgr.getBitrateDebugEnable() );
}

//============================================================================
void MiniAudioIn::wantMicrophoneInput( bool enableInput )
{
    m_MicInputEnabled = enableInput;

    if( enableInput )
    {
        m_AudioInDeviceIsStarted = startAudioInDevice();
    }
    else
    {
        stopAudioInDevice();
        m_AudioInDeviceIsStarted = false;
    }
}

//============================================================================
int MiniAudioIn::callbackAudioWrite( int16_t* pcmData, int sampleCnt )
{
    // LogMsg( LOG_VERBOSE, "MiniAudioIn::callbackAudioWrite %d ", sampleCnt );

    if( VxIsAppShuttingDown() )
    {
        // do not attempt anything while being destroyed
        return 0;
    }

    if( !pcmData )
    {
        LogMsg( LOG_ERROR, "MiniAudioIn::callbackAudioWrite has null pcmData." );
        return sampleCnt;
    }

    if( !sampleCnt )
    {
        LogMsg( LOG_ERROR, "MiniAudioIn::writeData has 0 sampleCnt." );
        return sampleCnt;
    }

    m_AudioIoMgr.callbackAudioDeviceWrite( pcmData, sampleCnt );
    return sampleCnt;

/*

    if( m_AudioIoMgr.getDirectLoopbackEnable() )
    {
        m_AudioIoMgr.directLoopbackWrite( pcmData, sampleCnt );
        m_PeakAmplitude = AudioUtils::peakPcmAmplitude0to100( pcmData, sampleCnt );
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
            LogMsg( LOG_ERROR, "MiniAudioIn::callbackAudioWrite has 0 m_PeakAmplitude." );
        }

        return sampleCnt;
    }

    // Qt no longer supports 8000 hz on all devices so everthing in is 48000 hz pcm data
    // ptop engine uses 16000 hz only

    // NOTE: so far Qt has used microphone write lengths evenly divisible down to 8000Hz. If this changes will need to add remainder handling

    static int64_t lastTime = 0;
    int64_t timeNow = GetHighResolutionTimeMs();
    int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
    lastTime = timeNow;
    if( m_AudioIoMgr.getAudioTimingDebugEnable() )
    {
        LogMsg( LOG_VERBOSE, "MiniAudioIn::writeData samples %d ms %d", sampleCnt, timeElapsed );
    }

    int inSampleCnt = sampleCnt;
    int16_t* sampleInData = pcmData;

    if( timeElapsed )
    {
        m_MicInBitrate.addSamplesAndInterval( inSampleCnt, timeElapsed );
    }

    int outSampleCnt = inSampleCnt / m_DivideCnt;
    if( inSampleCnt % m_DivideCnt )
    {
        LogMsg( LOG_VERBOSE, "MiniAudioIn::writeData samples have a downsample remainder %d of sample cnt %d ", inSampleCnt % m_DivideCnt, inSampleCnt );
    }

    static int16_t* sampleOutData = nullptr;
    static int lastMicWriteSamples = 0;
    if( sampleCnt != lastMicWriteSamples )
    {
        // first time or device changed or read buffer len changed
        // unfortunately Qt sometimes changes the microphone write len so also need to handle that
        m_MicWriteDurationUs = (int)AudioUtils::audioDurationUs( m_AudioFormat, sampleCnt * AUDIO_BYTES_PER_SAMPLE  );
        m_MicWriteTimeEstimator.setIntervalUs( m_MicWriteDurationUs );
        if( 0 != lastMicWriteSamples )
        {
            LogMsg( LOG_VERBOSE, "MiniAudioIn::writeData len changed from %d to %d ", lastMicWriteSamples, sampleCnt );
        }

        lastMicWriteSamples = sampleCnt;

        delete[] sampleOutData;
        sampleOutData = new int16_t[ (inSampleCnt / getDivideSamplesCount()) + 1 ]; // plus one for remainder sample if needed

        m_AudioIoMgr.setEchoCancelerNeedsReset( true ); // tell echo canceler parameters have changed and need to restart
    }

    int64_t micWriteTime = m_MicWriteTimeEstimator.estimateTime( timeNow );

    if( m_AudioTestState != eAudioTestStateNone )
    {
        m_AudioIoMgr.getAudioMasterClock().audioMicWriteDurationTime( m_MicWriteDurationUs / 1000, micWriteTime );
        if( m_AudioIoMgr.getSampleCntDebugEnable() )
        {
            m_AudioIoMgr.getAudioMasterClock().audioMicWriteSampleCnt( inSampleCnt );
        }

        if( m_AudioTestState == eAudioTestStateRun )
        {
            audioTestDetectTestSound( sampleInData, inSampleCnt, micWriteTime );
        }

        return sampleCnt;
    }

    if( m_DivideCnt != 1 )
    {
        AudioUtils::dnsamplePcmAudio( sampleInData, outSampleCnt, m_DivideCnt, sampleOutData );
    }
    else
    {
        sampleOutData = sampleInData;
        outSampleCnt = inSampleCnt;
    }

    if( timeElapsed )
    {
        m_MicOutBitrate.addSamplesAndInterval( outSampleCnt, timeElapsed );
    }

    int64_t micTailTimeMs = micWriteTime + (m_MicWriteDurationUs / 1000);
    if( m_AudioIoMgr.getIsEchoCancelEnabled() )
    {
        m_AudioIoMgr.getAudioEchoCancel().micWroteSamples( sampleOutData, outSampleCnt, micTailTimeMs, m_MicWriteTimeEstimator.getHasMaxTimestamps() );
    }
    else
    {
        LogMsg( LOG_ERROR, " MiniAudioIn::writeData ERROR must use echo canceler.. direct write of samples not implemented" );
    }

    bool micIsMuted = m_AudioIoMgr.getIsMicrophoneMuted();
    if( micIsMuted )
    {
        m_PeakAmplitude = 0;
    }
    else
    {
        m_PeakAmplitude = AudioUtils::peakPcmAmplitude0to100( sampleOutData, outSampleCnt );
    }

    m_AudioIoMgr.getAudioMasterClock().audioMicWriteDurationTime( m_MicWriteDurationUs / 1000, micWriteTime );
    if( m_AudioIoMgr.getSampleCntDebugEnable() )
    {
        m_AudioIoMgr.getAudioMasterClock().audioMicWriteSampleCnt( inSampleCnt );
    }
    */

    return sampleCnt;
}
