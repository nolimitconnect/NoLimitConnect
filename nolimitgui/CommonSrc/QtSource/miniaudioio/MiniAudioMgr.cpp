//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MiniAudioMgr.h"
#include "AudioUtils.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include <ptop_src/ptop_engine_src/MediaProcessor/MediaProcessor.h>

#include <QSurface>
#include <qmath.h>
#include <QTimer>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTimer.h>
#include <CoreLib/VxGlobals.h>

namespace
{
    //============================================================================
    static void* AudioMiniAudioMgrProcessThreadFunc( void* pvContext )
    {
        VxThread* poThread = (VxThread*)pvContext;
        poThread->setIsThreadRunning( true );
        MiniAudioMgr* processor = (MiniAudioMgr*)poThread->getThreadUserParam();
        if( processor )
        {
            processor->processAudioThreaded();
        }

        poThread->threadAboutToExit();
        return nullptr;
    }
}

//============================================================================
MiniAudioMgr::MiniAudioMgr( AppCommon& app, IAudioCallbacks& audioCallbacks, QWidget* parent )
    : QWidget( parent )
    , m_MiniAudioDevices()
    , m_MyApp( app )
    , m_AudioCallbacks( audioCallbacks )
    , m_AudioOutMixer( *this, audioCallbacks, this )
    , m_AudioInIo( *this, this )
    , m_AudioOutIo( *this,this )
    , m_AudioEchoCancel( app, *this, this )
    , m_AudioTestTimer( new QTimer( this ) )
    , m_AudioMasterClock( *this, this )
{
    memset( m_SilenceBuf, 0, sizeof( m_SilenceBuf ) );

    m_AudioTestTimer->setInterval( 1200 );
    connect( m_AudioTestTimer, SIGNAL(timeout()), this, SLOT(slotAudioTestTimer()) );

    memset( m_MyLastAudioOutSample, 0, sizeof( m_MyLastAudioOutSample ) );

    m_AudioOutFormat.setSampleRate( AUDIO_DEVICE_SAMPLE_RATE );
    m_AudioOutFormat.setChannelCount( AUDIO_CHANNELS ); // use 1 channel unless using setBufferSize then use 2 to speed up processing
    m_AudioOutFormat.setSampleFormat( QAudioFormat::Int16 );

    m_AudioInFormat.setSampleRate( AUDIO_DEVICE_SAMPLE_RATE );
    m_AudioInFormat.setChannelCount( AUDIO_CHANNELS ); // use 1 channel unless using setBufferSize then use 2 to speed up processing
    m_AudioInFormat.setSampleFormat( QAudioFormat::Int16 );

    // setEchoCancelEnable( true ); // for now always enabled

    // setDirectLoopbackEnable( true );

    // debug testing flages
    // setAudioTimingEnable( true ); // log audio timing
    // setFrameTimingEnable( true ); // log audio frames and timing
    // setFrameIndexDebugEnable( true ); // log audio frame indexes and when incremented
    // setBitrateDebugEnable( true ); // log audio bit rates that are way out of what should be the rate
    // setSampleCntDebugEnable( true ); // log audio sample counts
}

//============================================================================
// return true if any microphone device is available to be enabled
bool MiniAudioMgr::toGuiIsMicrophoneDeviceAvailable( void )
{
    return m_MaMicDeviceCount > 0;
}

//============================================================================
// enable disable microphone data callback
void MiniAudioMgr::toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput )
{
    static VxGUID nullGuid;
    toGuiWantUserVoiceMicrophone( appModule, nullGuid, wantMicInput );
}

//============================================================================
void MiniAudioMgr::toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput )
{
    bool found{ false };
    m_WantMicMutex.lock();
    for( auto iter = m_WantMicList.begin(); iter != m_WantMicList.end(); iter++ )
    {
        if( iter->first == appModule && iter->second == onlineId )
        {
            found = true;
            if( !wantMicInput )
            {
                m_WantMicList.erase( iter );
                break;
            }
        }
    }

    if( wantMicInput && !found )
    {
        m_WantMicList.push_back( std::make_pair( appModule, onlineId ) );
    }

    bool enableMic = !m_WantMicList.empty();
    m_WantMicMutex.unlock();

    if( ( enableMic != m_WantMicrophone ) && isMicrophoneDeviceAvailable() )
    {
        m_WantMicrophone = enableMic;
        enableMicrophone( enableMic );
    }
}

//============================================================================
 // update microphone output
void MiniAudioMgr::enableMicrophone( bool enable )
{
    if( enable )
    {
        resetMicrophoneBuffers();
    }

    m_AudioInIo.wantMicrophoneInput( enable );

    if( !enable )
    {
        resetMicrophoneBuffers();
    }
}

//============================================================================
// enable disable sound out
void MiniAudioMgr::toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput )
{
    static VxGUID nullGuid;
    toGuiWantUserVoiceSpeaker( appModule, nullGuid, wantSpeakerOutput );
}

//============================================================================
void MiniAudioMgr::toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput )
{
    bool found{ false };
    m_WantSpeakerMutex.lock();
    for( auto iter = m_WantSpeakerList.begin(); iter != m_WantSpeakerList.end(); iter++ )
    {
        if( iter->first == appModule && iter->second == onlineId )
        {
            found = true;
            if( !wantSpeakerOutput )
            {
                m_WantSpeakerList.erase( iter );
                break;
            }
        }
    }

    if( wantSpeakerOutput && !found )
    {
        m_WantSpeakerList.push_back( std::make_pair( appModule, onlineId ) );
    }

    bool enableSpeaker = !m_WantSpeakerList.empty();
    m_WantSpeakerMutex.unlock();

    if( ( enableSpeaker != m_WantSpeakerOutput ) && isSpeakerDeviceAvailable() )
    {
        m_WantSpeakerOutput = enableSpeaker;
        enableSpeakers( m_WantSpeakerOutput );
    }
}

//============================================================================
// enable disable sound out
void MiniAudioMgr::enableSpeakers( bool enable )
{
    if( enable )
    {
        resetSpeakerBuffers();
    }

    m_AudioOutIo.wantSpeakerOutput( enable );

    if( !enable )
    {
        resetSpeakerBuffers();
    }
}

//============================================================================
int MiniAudioMgr::toGuiPlayAudioFrame( EAppModule appModule, int16_t* pu16PcmData, int pcmDataLenInBytes, bool isSilence )
{
    // assumes must be 80 ms of pcm mono
    vx_assert( pcmDataLenInBytes == AUDIO_BUF_SIZE)
    return m_AudioOutMixer.toGuiAudioFrameThreaded( appModule, pu16PcmData, isSilence );
 }

//============================================================================
void MiniAudioMgr::audioIoSystemStartup()
{
    if( !m_AudioIoInitialized )
    {
        //m_EchoCancelEnabled = m_MyApp.getAppSettings().getEchoCancelEnable();
        //m_AudioEchoCancel.enableEchoCancel( m_EchoCancelEnabled );

        startupMiniAudio();

        if( isSpeakerDeviceAvailable() )
        {
            int deviceIndex = 0;
            getSoundOutDeviceIndex( deviceIndex );

            m_AudioOutIo.initAudioOut( m_AudioOutFormat, deviceIndex );
            m_ToneGenerator.setAudioFormat( m_AudioOutFormat );
        }

        if( isMicrophoneDeviceAvailable() )
        {
            int deviceIndex = 0;
            getSoundInDeviceIndex( deviceIndex );

            m_AudioInIo.initAudioIn( m_AudioInFormat, deviceIndex );
        }

        m_AudioIoInitialized = true;
        m_ProcessAudioThread.startThread( (VX_THREAD_FUNCTION_T)AudioMiniAudioMgrProcessThreadFunc, this, "ProcessAudioThread" );
    }
}

//============================================================================
void MiniAudioMgr::audioIoSystemShutdown()
{
    m_ProcessAudioThread.abortThreadRun( true );
    m_ProcessAudioSemaphore.signal();

    m_AudioMasterClock.masterClockShutdown();
    m_AudioIoInitialized = false;
    m_AudioInIo.audioInShutdown();
    m_AudioOutIo.audioOutShutdown();
    shutdownMiniAudio();
}

//============================================================================
// volume is from 0.0 to 1.0
void MiniAudioMgr::setSpeakerVolume( float volume0to1 )
{
    m_AudioOutIo.setSpeakerVolume( (int)( volume0to1 * 100 ) );
}

//============================================================================
uint16_t SwapEndian16( uint16_t src )
{
    return ( ( src & 0xFF00 ) >> 8 ) | ( ( src & 0x00FF ) << 8 );
}

//============================================================================
void MiniAudioMgr::aboutToDestroy()
{
}

//============================================================================
void MiniAudioMgr::setMuteMicrophone( bool mute )      
{ 
    if( m_MicrophoneMuted != mute )
    {
        m_MicrophoneMuted = mute;
    }
}

//============================================================================
void MiniAudioMgr::setMuteSpeaker( bool mute )         
{ 
    if( m_SpeakersMuted != mute )
    {
        m_SpeakersMuted = mute; 
    }
}
    
//============================================================================
int MiniAudioMgr::getAudioInPeakAmplitude( void )
{
    return m_PeakAudioInAmplitude;
}

//============================================================================
int MiniAudioMgr::getAudioOutPeakAmplitude( void )
{
    return m_PeakAudioOutAmplitude;
}

//============================================================================
bool MiniAudioMgr::setSoundInDeviceIndex( int sndInDeviceIndex )
{
    m_MyApp.getAppSettings().setSoundInDeviceIndex( sndInDeviceIndex );
    return true;
}

//============================================================================
bool MiniAudioMgr::getSoundInDeviceIndex( int& retDeviceIndex )
{
    retDeviceIndex = m_MyApp.getAppSettings().getSoundInDeviceIndex();
    return true;
}

//============================================================================
bool MiniAudioMgr::setSoundOutDeviceIndex( int sndOutDeviceIndex )
{
    m_MyApp.getAppSettings().setSoundOutDeviceIndex( sndOutDeviceIndex );
    return true;
}

//============================================================================
bool MiniAudioMgr::getSoundOutDeviceIndex( int& retDeviceIndex )
{
    retDeviceIndex = m_MyApp.getAppSettings().getSoundOutDeviceIndex();
    return true;
}

//============================================================================
bool MiniAudioMgr::soundInDeviceChanged( int deviceIndex )
{
    return m_AudioInIo.soundInDeviceChanged( deviceIndex );
}

//============================================================================
bool MiniAudioMgr::soundOutDeviceChanged( int deviceIndex )
{
    return m_AudioOutIo.soundOutDeviceChanged( deviceIndex );
}

//============================================================================
bool MiniAudioMgr::runAudioDelayTest( void )
{
    if( m_AudioTestState != eAudioTestStateNone )
    {
        LogMsg( LOG_ERROR, "MiniAudioMgr::runAudioTest already running" );
        return false;
    }

    m_EchoDelayCurrentInteration = 0;
    m_EchoDelayResultList.clear();

    m_AudioTestMicEnable = m_AudioInIo.isMicrophoneInputWanted();
    m_AudioTestSpeakerEnable = m_AudioOutIo.isSpeakerOutputWanted();

    if( !m_AudioTestMicEnable )
    {
        m_AudioInIo.wantMicrophoneInput( true );
    }
    
    if( !m_AudioTestSpeakerEnable )
    {
        m_AudioOutIo.wantSpeakerOutput( true );
    }

    setAudioTestState( eAudioTestStateInit );

    m_AudioTestTimer->start();
    return true;
}

//============================================================================
void MiniAudioMgr::slotAudioTestTimer( void )
{
    int audioPeakValue{ 0 };
    int64_t audioDetectTimeMs{ 0 };
    switch( m_AudioTestState )
    {
    case eAudioTestStateInit:
        // waited for sound to be quiet
        LogMsg( LOG_VERBOSE, "Echo Delay Test Init" );
        setAudioTestState( eAudioTestStateRun );
        break;

    case eAudioTestStateRun:
        // send sound test cherp
        LogMsg( LOG_VERBOSE, "Echo Delay Test Run" );
        setAudioTestState( eAudioTestStateResult );
        break;

    case eAudioTestStateResult:
        // get the delay time result
        LogMsg( LOG_VERBOSE, "Echo Delay Test Result" );
        audioDetectTimeMs = getAudioTestDetectTime( audioPeakValue );
        if( !handleAudioTestResult( getAudioTestSentTime(), audioDetectTimeMs, audioPeakValue ) )
        {
            LogMsg( LOG_WARNING, "Echo Delay Test Faled" );
            
            setAudioTestState( eAudioTestStateDone );
            break;
        }

        m_EchoDelayCurrentInteration++;
        if( m_EchoDelayCurrentInteration < m_EchoDelayTestMaxInterations )
        {
            // start test again
            LogMsg( LOG_VERBOSE, "Echo Delay Test Run Again" );
            setAudioTestState( eAudioTestStateInit );
        }
        else
        {
            LogMsg( LOG_VERBOSE, "Echo Delay Test Done" );
            setAudioTestState( eAudioTestStateDone );
        }

        break;

    case eAudioTestStateDone:
        LogMsg( LOG_VERBOSE, "Echo Delay Test Restore Mic/Speaker states and finish" );
        m_AudioTestTimer->stop();
        setAudioTestState( eAudioTestStateNone );

        if( !m_AudioTestMicEnable )
        {
            m_AudioInIo.wantMicrophoneInput( m_AudioTestMicEnable );
        }

        if( !m_AudioTestSpeakerEnable )
        {
            m_AudioOutIo.wantSpeakerOutput( m_AudioTestSpeakerEnable );
        }

        break;

    case eAudioTestStateNone:
    default:
        break;
    }
}

//============================================================================
void MiniAudioMgr::setAudioTestState( EAudioTestState audioTestState )
{
    m_AudioTestState = audioTestState;

    if( eAudioTestStateInit == audioTestState || eAudioTestStateRun == audioTestState )
    {
        m_DelayTestDetectList.clear();
        setAudioTestSentTime( 0 );
        resetMicrophoneBuffers();
        resetSpeakerBuffers();
    }

    emit signalAudioTestState( audioTestState );

}

//============================================================================
bool MiniAudioMgr::handleAudioTestResult( int64_t soundOutTimeMs, int64_t soundDetectTimeMs, int peakVal0to100 )
{
    bool isValid{ false };
    QString resultMsg;
    int64_t timeDif = soundDetectTimeMs - soundOutTimeMs;
    if(!soundOutTimeMs || !soundDetectTimeMs)
    {
        timeDif = 0;
        resultMsg = QObject::tr("Sound Delay Not Detected. Check speaker volume and that microphone is on ");
    }
    else if( timeDif < 50 )
    {
        resultMsg = QObject::tr("Sound Delay too short.. probably noise ");
    }
    else if( timeDif > 900 )
    {
        resultMsg = QObject::tr("Sound Delay too long.. probably mic level low ");
    }
    else
    {
        resultMsg = QObject::tr("Sound Delay is ");
        isValid = true;
    }

    resultMsg += QString::number((int)timeDif);

    LogMsg( LOG_VERBOSE, "MiniAudioMgr::handleAudioTestResult %s", resultMsg.toUtf8().constData() );

    m_EchoDelayResultList.push_back( (int)timeDif );
    // to avoid to much sound thread cpu time used Qt::QueuedConnection when connecting to these signals
    emit signalAudioTestMsg( resultMsg );
    emit signalTestedSoundDelay( (int)timeDif );
    return isValid;
}

//============================================================================
void MiniAudioMgr::setEchoCancelEnable( bool enable ) 
{ 
    m_EchoCancelEnabled = enable; 
    m_AudioEchoCancel.enableEchoCancel( m_EchoCancelEnabled );
    if( m_MyApp.getAppSettings().getIsAppSettingInitialized() )
    {
        m_MyApp.getAppSettings().setEchoCancelEnable( m_EchoCancelEnabled );
    }
}

//============================================================================
void MiniAudioMgr::setEchoCancelerNeedsReset( bool needReset )
{
    m_AudioEchoCancel.setEchoCancelerNeedsReset( needReset );
}

//============================================================================
bool MiniAudioMgr::getFrameTimingEnable( void )
{
    return m_FrameTimingEnabled && m_AudioEchoCancel.getIsInSync();
}

//============================================================================
void MiniAudioMgr::setPeakAmplitudeDebugEnable( bool enable ) 
{ 
    m_PeakAmplitudeDebug = enable; 
    m_AudioEchoCancel.setPeakAmplitudeDebugEnable( enable );
}

//============================================================================
void MiniAudioMgr::echoCancelSyncStateThreaded( bool inSync )
{
    m_IsEchoCancelInSync = inSync;
    m_AudioInIo.echoCancelSyncStateThreaded( inSync );
    m_AudioOutIo.echoCancelSyncStateThreaded( inSync );

        // BRJ may need to implement this
        // IAudioCallbacks& audioCallbacks = getAudioCallbacks();
        // audioCallbacks.fromGuiAudioOutSpaceAvail( AUDIO_BUF_SIZE_8000_1_S16 );

}

//============================================================================
bool MiniAudioMgr::isEchoCancelInSync( void )
{
    return m_IsEchoCancelInSync;
}

//============================================================================
void MiniAudioMgr::microphoneDeviceEnabled( bool isEnabled )
{
    m_AudioMasterClock.microphoneDeviceEnabled( isEnabled );
}

//============================================================================
void MiniAudioMgr::speakerDeviceEnabled( bool isEnabled )
{
    m_AudioMasterClock.speakerDeviceEnabled( isEnabled );
}

//============================================================================
void MiniAudioMgr::fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnt, bool isSilence )
{
    m_MyApp.fromGuiEchoCanceledSamplesThreaded( pcmData, sampleCnt, isSilence );
}

//============================================================================
void MiniAudioMgr::fromGuiAudioOutSpaceAvaiThreaded( int sampleCnt )
{
    m_MyApp.fromGuiAudioOutSpaceAvaiThreaded( sampleCnt );
}

//============================================================================
void MiniAudioMgr::readTestToneSamples( int16_t* pcmData, int sampleCnt )
{
    if( !m_ToneGenerator.isFormatSet() )
    {
        m_ToneGenerator.setAudioFormat( m_AudioOutFormat );
    }

    m_ToneGenerator.readToneSamples( pcmData, sampleCnt );
}

//============================================================================
void MiniAudioMgr::callbackAudioDeviceWrite( int16_t* pcmDataMic, int sampleCntMic )
{
    if( !isAudioInitialized() )
    {
        return;
    }

    int16_t* pcmData = pcmDataMic;
    int sampleCnt = sampleCntMic;
    /*
    int dnSampleDivide = getAudioInIo().getDivideSamplesCount();

    if( dnSampleDivide != 1 )
    {
        int resampledCnt = sampleCntMic / dnSampleDivide;
        static int16_t* sampleOutData = nullptr;
        static int lastMicWriteSamples = 0;
        if( !sampleOutData || sampleCnt != lastMicWriteSamples )
        {
            // first time or device changed or sample count changed
            lastMicWriteSamples = sampleCnt;

            delete[] sampleOutData;
            sampleOutData = new int16_t[ resampledCnt + 1 ]; // plus one for remainder sample if needed

            setEchoCancelerNeedsReset( true ); // tell echo canceler parameters have changed and need to restart
        }

        AudioUtils::dnsamplePcmAudio( pcmDataMic, resampledCnt, dnSampleDivide, sampleOutData );
        pcmData = sampleOutData;
        sampleCnt = resampledCnt;
    }
    */

    if( getEchoCancelEnable() )
    {
        // if using echo cancel use samples as detected by microphone
        getAudioEchoCancel().micWroteSamples( pcmData, sampleCnt );
    }

    if( m_AudioTestState != eAudioTestStateNone )
    {
        audioTestDetectTestSound( pcmData, sampleCnt, GetHighResolutionTimeMs() );
    }

    pcmData = getIsMicrophoneMuted() ? m_SilenceBuf : pcmData;


    if( !getEchoCancelEnable() || m_AudioTestState != eAudioTestStateNone || getDirectLoopbackEnable() )
    {
        // will be processed without echo cancel
        m_AudioWriteMutex.lock();
        m_AudioWriteBuf.writeSamples( pcmData, sampleCnt );
        m_AudioWriteMutex.unlock();
    }

    m_ProcessAudioSemaphore.signal();

    if( getIsMicrophoneMuted() )
    {
        m_PeakAudioInAmplitude = 0;
    }
    else
    {
        m_PeakAudioInAmplitude = AudioUtils::peakPcmAmplitude0to100( pcmData, sampleCnt );
    }
}

//============================================================================
void MiniAudioMgr::callbackAudioDeviceRead( int16_t* pcmData, int sampleRequestCnt )
{
    if( !isAudioInitialized() )
    {
        return;
    }

    addReadSpeakerCount( sampleRequestCnt );

    m_AudioReadMutex.lock();
    int availableCnt = m_AudioReadBuf.getSampleCnt();
    if( availableCnt >= sampleRequestCnt)
    {
        m_AudioReadBuf.readSamples( pcmData, sampleRequestCnt );
        // LogMsg( LOG_ERROR, "MiniAudioMgr::callbackAudioDeviceRead full read %d samples", sampleRequestCnt );
    }
    else
    {
        if(availableCnt)
        {
            LogModule( eLogAudioIo, LOG_ERROR, "MiniAudioMgr::callbackAudioDeviceRead samples available %d requested %d", availableCnt, sampleRequestCnt );
            int samplesToRead = std::min( sampleRequestCnt, availableCnt );
            if( samplesToRead )
            {
                m_AudioReadBuf.readSamples( pcmData, samplesToRead );

                if( samplesToRead < sampleRequestCnt )
                {
                    memset( &pcmData[ samplesToRead ], 0, (sampleRequestCnt - samplesToRead) * AUDIO_BYTES_PER_SAMPLE );
                }
            }
        }
        else
        {
            LogModule( eLogAudioIo, LOG_ERROR, "MiniAudioMgr::callbackAudioDeviceRead no samples available" );
            memset( pcmData, 0, sampleRequestCnt * AUDIO_BYTES_PER_SAMPLE );
        }
    }

    m_AudioReadMutex.unlock();

    if( getEnableSpeakerTestTone() )
    {
        readTestToneSamples( pcmData, sampleRequestCnt );
    }

    if( m_AudioTestState != eAudioTestStateNone )
    {
        memset( pcmData, 0, sampleRequestCnt * AUDIO_BYTES_PER_SAMPLE );
        if( m_AudioTestState == eAudioTestStateRun && !getAudioTestSentTime() )
        {
            int16_t* sampleBuf = (int16_t*)pcmData;
            // create a 480 hz square wave tone for 10 ms as a sound to be detected by microphone for delay timing test
            int maxSamplesToSet = AudioUtils::audioSamplesRequiredForGivenMs( m_AudioOutFormat, 10 );
            maxSamplesToSet = std::min( maxSamplesToSet, sampleRequestCnt );
            int samplesCycle = (m_AudioOutFormat.sampleRate() * m_AudioOutFormat.channelCount()) / (480 * 2);
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

            setAudioTestSentTime( GetHighResolutionTimeMs() );
        }
    }
    else if( getIsSpeakerMuted() )
    {
        memset( pcmData, 0, sampleRequestCnt * AUDIO_BYTES_PER_SAMPLE );
        m_PeakAudioOutAmplitude = 0;
    }
    else
    {
        m_PeakAudioOutAmplitude = AudioUtils::peakPcmAmplitude0to100( pcmData, sampleRequestCnt );
    }

    if( getEchoCancelEnable() )
    {
        // if using echo cancel use final result of samples sent to speaker
        getAudioEchoCancel().speakerReadSamples( pcmData, sampleRequestCnt );
    }

    m_ProcessAudioSemaphore.signal();
}

//============================================================================
void MiniAudioMgr::audioTestDetectTestSound( int16_t* sampleInData, int inSampleCnt, int64_t micWriteTime )
{
    // find peak value and time
    int samplePosVal = 0;
    int64_t sampleTimeMs = 0;

    int16_t sampCompareValue = 32768 / 10;

    for( int i = 0; i < inSampleCnt; i++ )
    {
        if( sampleInData[ i ] > samplePosVal && sampleInData[ i ] > sampCompareValue )
        {
            samplePosVal = sampleInData[ i ];
            sampleTimeMs = micWriteTime + AudioUtils::audioDurationMs( m_AudioInFormat, i * 2 );
        }
    }

    if( samplePosVal && sampleTimeMs )
    {
        m_DelayTestDetectList.push_back( std::make_pair( sampleTimeMs, samplePosVal ) );
    }
}

//============================================================================
int64_t MiniAudioMgr::getAudioTestDetectTime( int& peakValue )
{
    int64_t detectTime = 0;
    peakValue = 0;

    for( auto& pair : m_DelayTestDetectList )
    {
        if( pair.second > peakValue )
        {
            peakValue = pair.second;
            detectTime = pair.first;
        }
    }

    return detectTime;
}

//============================================================================
void MiniAudioMgr::processAudioThreaded( void )
{
    while( false == m_ProcessAudioThread.isAborted() )
    {
        m_ProcessAudioSemaphore.wait();
        if( m_ProcessAudioThread.isAborted() )
        {
            LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded aborting" );
            break;
        }

        if( getDirectLoopbackEnable() || m_AudioTestState != eAudioTestStateNone )
        {
            m_AudioWriteMutex.lock();
            int micSamples = m_AudioWriteBuf.getSampleCnt();
            if( micSamples )
            {
                m_AudioReadMutex.lock();
                m_AudioReadBuf.writeSamples( m_AudioWriteBuf.getSampleBuffer(), micSamples );
                m_AudioReadMutex.unlock();

                m_AudioWriteBuf.samplesWereRead( micSamples );
            }

            m_AudioWriteMutex.unlock();

            if( getEchoCancelEnable() )
            {
                getAudioEchoCancel().setEchoCancelerNeedsReset( true );
            }

            continue;
        }
        else if( getEchoCancelEnable() && isMicrophoneInputWanted() && isSpeakerOutputWanted() )
        {
            getAudioEchoCancel().processEchoCancelThreaded();

            m_EchoCanceledBufMutex.lock();
            if( m_EchoCanceledBuf.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
            {
                fromGuiEchoCanceledSamplesThreaded( m_EchoCanceledBuf.getSampleBuffer(), AUDIO_SAMPLES_PER_FRAME, false );
                m_EchoCanceledBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
            }

            m_EchoCanceledBufMutex.unlock();
        }
        else if( isMicrophoneInputWanted() )
        {
            // send raw audio to engine to be processed and sent out
            m_AudioWriteMutex.lock();
            if( m_AudioWriteBuf.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
            {
                //int16_t tempBuf[ AUDIO_SAMPLES_PER_FRAME ];
                //memcpy( tempBuf, m_AudioWriteBuf.getSampleBuffer(), sizeof( tempBuf ) );
                //int16_t maxVal = -32000;
                //int16_t minVal = 32000;
                //for( int i = 0; i < AUDIO_SAMPLES_PER_FRAME; i++ )
                //{
                //    minVal = std::min( minVal, tempBuf[ i ] );
                //    maxVal = std::max( maxVal, tempBuf[ i ] );
                //}

                //LogMsg( LOG_ERROR, "MiniAudioMgr::processAudioThreaded min %d max %d", minVal, maxVal );

                fromGuiEchoCanceledSamplesThreaded( m_AudioWriteBuf.getSampleBuffer(), AUDIO_SAMPLES_PER_FRAME, false );
                m_AudioWriteBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
            }

            m_AudioWriteMutex.unlock();
        }

        if( getReadSpeakerCount() >= AUDIO_SAMPLES_PER_FRAME )
        {
            // there is enough room to process another mixer frame
            subtractReadSpeakerCount( AUDIO_SAMPLES_PER_FRAME );
            processMixerFrames();
            if( getPlayerNlcActive() )
            {
                m_PlayerNlcMutex.lock();
                if( m_PlayerNlcCache.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
                {
                    toGuiPlayAudioFrame( eAppModulePlayerNlc, m_PlayerNlcCache.getSampleBuffer(), AUDIO_SAMPLES_PER_FRAME * AUDIO_BYTES_PER_SAMPLE,
                        isSilentSamples( m_PlayerNlcCache.getSampleBuffer(), AUDIO_SAMPLES_PER_FRAME ) );
                    m_PlayerNlcCache.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
                }

                m_PlayerNlcMutex.unlock();
            }

            fromGuiAudioOutSpaceAvaiThreaded( AUDIO_SAMPLES_PER_FRAME );
        }
    }

        /*
        if( getAudioTimingDebugEnable() )
        {
            static int64_t lastTime = 0;
            int64_t timeNow = GetHighResolutionTimeMs();
            int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
            lastTime = timeNow;

            static int64_t lastSpaceAvailableTime{ 0 };
            static int funcCallCnt{ 0 };
            funcCallCnt++;
            if( lastSpaceAvailableTime )
            {
                int timeInterval = (int)(timeNow - lastSpaceAvailableTime);
                LogMsg( LOG_VERBOSE, "processAudioOutSpaceAvailableThreaded %d elapsed %d ms app %d ms", funcCallCnt, (int)timeInterval, (int)m_MyApp.elapsedMilliseconds() );
            }

            lastSpaceAvailableTime = timeNow;
        }

        static int16_t prevFrameSample = 0;

        // make current frame ready for read by speakers
        lockMixer();
        AudioLoopbackFrame& audioFrame = getAudioWriteFrame();
        audioFrame.processFrameForSpeakerOutputThreaded( prevFrameSample );
        prevFrameSample = audioFrame.getLastEchoSample();

        // LogMsg( LOG_VERBOSE, " AudioLoopback::processAudioLoopbackThreaded speaker buf m_ProcessedBufMutex.lock()" );
        m_ProcessedBufMutex.lock();

        if( audioFrame.echoSamplesAvailable() != AUDIO_SAMPLES_PER_FRAME )
        {
            LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded incorrect buffer processing should have %d samples but has %d samples elapsed %d ms",
                audioFrame.getFrameIndex(), AUDIO_SAMPLES_PER_FRAME, audioFrame.echoSamplesAvailable(), timeElapsed );
        }

        if( audioFrame.echoSamplesAvailable() * AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER != audioFrame.speakerSamplesAvailable() )
        {
            LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded incorrect upsampling should be %d samples is %d samples elapsed %d ms",
                audioFrame.getFrameIndex(), audioFrame.echoSamplesAvailable() * AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER, audioFrame.speakerSamplesAvailable(), timeElapsed );
        }

        if( m_AudioIoMgr.getBitrateDebugEnable() )
        {
            m_ProcessFrameBitrate.addSamplesAndInterval( audioFrame.echoSamplesAvailable(), timeElapsed );
            m_ProcessSpeakerBitrate.addSamplesAndInterval( audioFrame.speakerSamplesAvailable(), timeElapsed );
        }

        if( m_AudioIoMgr.getSampleCntDebugEnable() )
        {
            m_AudioIoMgr.getAudioMasterClock().audioSpeakerReadSampleCnt( audioFrame.speakerSamplesAvailable() );
        }

        m_SpeakerProcessedBuf.writeSamples( audioFrame.getSpeakerSampleBuf(), audioFrame.speakerSamplesAvailable() );
        // LogMsg( LOG_VERBOSE, " AudioLoopback::processAudioLoopbackThreaded speaker buf m_ProcessedBufMutex.unlock()" );
        m_ProcessedBufMutex.unlock();
        m_EchoProcessedBuf.writeSamples( audioFrame.getEchoSampleBuf(), audioFrame.echoSamplesAvailable() );

        if( m_AudioIoMgr.getSampleCntDebugEnable() )
        {
            LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded processed samples available echo %d speaker %d elapsed %d ms",
                audioFrame.getFrameIndex(), m_EchoProcessedBuf.getSampleCnt(), m_SpeakerProcessedBuf.getSampleCnt(), timeElapsed );
        }

        // move to next frame and clear it so is ready to write to
        incrementMixerWriteIndex();
        AudioLoopbackFrame& nextAudioFrame = getAudioWriteFrame();
        nextAudioFrame.clearFrame( false );
        incrementMixerReadIndex();
        unlockMixer();

        // let the echo canceler unlock the processed speaker samples as soon as possible to avoid
        // stalling the qt audio device read or write call
        m_AudioIoMgr.getAudioEchoCancel().processEchoCancelThreaded( m_EchoProcessedBuf, m_ProcessedBufMutex );

        // do output space available processing
        processOutSpaceAvailable();


        if( m_ProcessAudioLoopbackThread.isAborted() )
        {
            LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded aborting3" );
            break;
        }
    }
    */

    LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded leaving function" );
}

//============================================================================
void MiniAudioMgr::resetMicrophoneBuffers( void )
{
    m_AudioWriteMutex.lock();
    m_AudioWriteBuf.clear();
    m_AudioWriteMutex.unlock();

    getAudioEchoCancel().resetMicrophoneBuffers();
}

//============================================================================
void MiniAudioMgr::resetSpeakerBuffers( void )
{
    m_AudioReadMutex.lock();
    m_AudioReadBuf.clear();
    m_AudioReadMutex.unlock();

    m_AudioMixerMutex.lock();
    m_AudioMixerBuf.clear();
    m_AudioOutMixer.resetMixer();
    m_SpeakerReadSampleCnt = 0;
    m_AudioMixerMutex.unlock();

    getAudioEchoCancel().resetSpeakerBuffers();
}

//============================================================================
void MiniAudioMgr::processMixerFrames( void )
{
    lockMixer();
    AudioMixerFrame& audioFrame = m_AudioOutMixer.getAudioReadFrame();
    if( audioFrame.audioSamplesInUse() >= AUDIO_SAMPLES_PER_FRAME )
    {
        m_AudioReadMutex.lock();
        m_AudioReadBuf.writeSamples( audioFrame.getMixerBuf(), audioFrame.audioSamplesInUse() );
        m_AudioReadMutex.unlock();

        audioFrame.clearFrame( true );
        m_AudioOutMixer.incrementMixerReadIndex();
        m_AudioOutMixer.incrementMixerWriteIndex();
    }
    else
    {
        LogMsg( LOG_ERROR, "P Frame %d MiniAudioMgr::processMixerFrames buffer processing should have %d samples but has %d samples",
            audioFrame.getFrameIndex(), AUDIO_SAMPLES_PER_FRAME, audioFrame.audioSamplesInUse() );
        m_AudioMixerBuf.writeSamples( m_SilenceBuf, AUDIO_SAMPLES_PER_FRAME );
    }
    /*
    AudioLoopbackFrame& audioFrame = getAudioWriteFrame();
    audioFrame.processFrameForSpeakerOutputThreaded( prevFrameSample );
    prevFrameSample = audioFrame.getLastEchoSample();

    // LogMsg( LOG_VERBOSE, " AudioLoopback::processAudioLoopbackThreaded speaker buf m_ProcessedBufMutex.lock()" );
    m_ProcessedBufMutex.lock();

    if( audioFrame.echoSamplesAvailable() != AUDIO_SAMPLES_PER_FRAME )
    {
        LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded incorrect buffer processing should have %d samples but has %d samples elapsed %d ms",
            audioFrame.getFrameIndex(), AUDIO_SAMPLES_PER_FRAME, audioFrame.echoSamplesAvailable(), timeElapsed );
    }

    if( audioFrame.echoSamplesAvailable() * AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER != audioFrame.speakerSamplesAvailable() )
    {
        LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded incorrect upsampling should be %d samples is %d samples elapsed %d ms",
            audioFrame.getFrameIndex(), audioFrame.echoSamplesAvailable() * AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER, audioFrame.speakerSamplesAvailable(), timeElapsed );
    }

    if( m_AudioIoMgr.getBitrateDebugEnable() )
    {
        m_ProcessFrameBitrate.addSamplesAndInterval( audioFrame.echoSamplesAvailable(), timeElapsed );
        m_ProcessSpeakerBitrate.addSamplesAndInterval( audioFrame.speakerSamplesAvailable(), timeElapsed );
    }

    if( m_AudioIoMgr.getSampleCntDebugEnable() )
    {
        m_AudioIoMgr.getAudioMasterClock().audioSpeakerReadSampleCnt( audioFrame.speakerSamplesAvailable() );
    }

    m_SpeakerProcessedBuf.writeSamples( audioFrame.getSpeakerSampleBuf(), audioFrame.speakerSamplesAvailable() );
    // LogMsg( LOG_VERBOSE, " AudioLoopback::processAudioLoopbackThreaded speaker buf m_ProcessedBufMutex.unlock()" );
    m_ProcessedBufMutex.unlock();
    m_EchoProcessedBuf.writeSamples( audioFrame.getEchoSampleBuf(), audioFrame.echoSamplesAvailable() );

    if( m_AudioIoMgr.getSampleCntDebugEnable() )
    {
        LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded processed samples available echo %d speaker %d elapsed %d ms",
            audioFrame.getFrameIndex(), m_EchoProcessedBuf.getSampleCnt(), m_SpeakerProcessedBuf.getSampleCnt(), timeElapsed );
    }

    // move to next frame and clear it so is ready to write to
    incrementMixerWriteIndex();
    AudioLoopbackFrame& nextAudioFrame = getAudioWriteFrame();
    nextAudioFrame.clearFrame( false );
    incrementMixerReadIndex();
    */
    unlockMixer();
}

//============================================================================
void MiniAudioMgr::addEchoCanceledSamples( int16_t* pcmData, int sampleCnt )
{
    m_EchoCanceledBufMutex.lock();
    m_EchoCanceledBuf.writeSamples( pcmData, sampleCnt );
    m_EchoCanceledBufMutex.unlock();
}

//============================================================================
float MiniAudioMgr::calculateMsOfSamples( int sampleCount )
{
    return ((float)sampleCount / (ECHO_SAMPLE_RATE / AUDIO_CHANNELS) / 1000.0f);
}

//============================================================================
bool MiniAudioMgr::isSilentSamples( int16_t* pcmData, int sampleCnt )
{
    for( int i = 0; i < sampleCnt; i++ )
    {
        if( pcmData[ i ] )
        {
            return false;
        }
    }

    return true;
}

//============================================================================
void MiniAudioMgr::setPlayerNlcActive( bool isActive )
{
    if( isActive != m_PlayerNlcActive )
    {
        m_PlayerNlcActive = isActive;
        if( m_PlayerNlcActive )
        {
            m_PlayerNlcMutex.lock();
            m_PlayerNlcCache.clear();
            m_PlayerNlcMutex.unlock();
            clearAllBuffers();
        }
        
        toGuiWantSpeakerOutput( eAppModulePlayerNlc, m_PlayerNlcActive );
    }
}


#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
//============================================================================
int MiniAudioMgr::toGuiPlayAudio( EAppModule appModule, float* audioDataFloat, int audioDataLenInBytes )
{
    // this assumes 20ms of stereo 48000 hz at a time from kodi based player
    // unfortunately there is no good way to down sample stereo to mono so we have to pick a channel 
    // this is because if the channels are 180 degrees out of phase they cancel each other out

    // TODO rework audio out to handle 48000 hz stereo for higher quality playback

    if( VxIsAppShuttingDown() || !audioDataLenInBytes || !audioDataFloat )
    {
        LogModule( eLogAudioIo, LOG_VERBOSE, "MiniAudioMgr::toGuiPlayAudio ignored len %d", audioDataLenInBytes );
        return 0;
    }

    if( eAppModulePlayerNlc == appModule )
    {
        // vx_assert( AUDIO_FRAME_SIZE_KODI == audioDataLenInBytes );

        int kodiSampleCnt = audioDataLenInBytes / sizeof( float );
        int skipCnt = AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
        int pcmSampleCnt = kodiSampleCnt / skipCnt;

        m_PlayerNlcMutex.lock();
        if( pcmSampleCnt > m_PlayerNlcCache.freeSpaceSampleCount() )
        {
            m_PlayerNlcMutex.unlock();
            LogMsg( LOG_ERROR, "MiniAudioMgr::toGuiPlayAudio overrun PlayerNlc" );
            return 0;
        }


        int16_t* pcmBuf = m_PlayerNlcCache.getSampleBuffer();
        pcmBuf += m_PlayerNlcCache.getSampleCnt();
        int totalSamples{ 0 };
        for( int i = 0; i < audioDataLenInBytes / sizeof( float ); i += skipCnt )
        {
            pcmBuf[ totalSamples ] = AudioUtils::floatToPcm( audioDataFloat[ i ] );
            totalSamples++;
        }

        m_PlayerNlcCache.samplesWereWritten( totalSamples );
        m_PlayerNlcMutex.unlock();
        if( IsLogEnabled( eLogAudioIo ) )
        {
            float cachedTime = toGuiGetAudioDelaySeconds( appModule );
            float totalCache = toGuiGetAudioCacheTotalSeconds( appModule );

            LogModule( eLogAudioIo, LOG_VERBOSE, "MiniAudioMgr::toGuiPlayAudio player-nlc samples %d cached sec %3.3f total cache %3.3f sec percent %d", 
                       totalSamples, cachedTime, totalCache, (int)((cachedTime / totalCache )*100) );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "MiniAudioMgr::toGuiPlayAudio unknown module %s ", DescribeAppModule( appModule ) );
    }

    return audioDataLenInBytes;
}

//============================================================================
float MiniAudioMgr::toGuiGetAudioDelaySeconds( EAppModule appModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    m_PlayerNlcMutex.lock();
    int usedPcmSamples = m_PlayerNlcCache.getSampleCnt();
    m_PlayerNlcMutex.unlock();
    return calculateMsOfSamples( usedPcmSamples ) * 1000;
}

//============================================================================
float MiniAudioMgr::toGuiGetAudioCacheFreeSpace( EAppModule appModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    // return total bytes available for kodi to write to
    m_PlayerNlcMutex.lock();
    int availablePcmSampleSpace = m_PlayerNlcCache.freeSpaceSampleCount();
    m_PlayerNlcMutex.unlock();
    return availablePcmSampleSpace * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
}

//============================================================================
float MiniAudioMgr::toGuiGetAudioCacheTotalSeconds( EAppModule appModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    return calculateMsOfSamples( m_PlayerNlcCache.getMaxSamples() ) * 1000;
}

#endif // ENABLE_KODI


//============================================================================
void  MiniAudioMgr::clearAllBuffers()
{
    m_AudioWriteMutex.lock();
    m_AudioWriteBuf.clear();
    m_AudioWriteMutex.unlock();

    m_AudioReadMutex.lock();
    m_AudioReadBuf.clear();
    m_AudioReadMutex.unlock();

    m_AudioMixerMutex.lock();
    m_AudioMixerBuf.clear();
    m_AudioMixerMutex.unlock();

    m_EchoCanceledBufMutex.lock();
    m_EchoCanceledBuf.clear();
    m_EchoCanceledBufMutex.unlock();
}