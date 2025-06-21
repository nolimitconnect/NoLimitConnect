//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MiniAudioMgr.h"

#include "AudioUtils.h"
#include "AppCommon.h"
#include "AppSettings.h"

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
MiniAudioMgr::MiniAudioMgr( AppCommon& app, IAudioCallbacks& audioCallbacks, QObject* parent )
    : QObject( parent )
    , m_MiniAudioDevices()
    , m_MyApp( app )
    , m_AudioCallbacks( audioCallbacks )
    //, m_AudioOutMixer( *this, audioCallbacks, this )
    , m_AudioInIo( *this, this )
    , m_AudioOutIo( *this,this )
    , m_AudioEchoCancel( app, *this, this )
    , m_AudioTestTimer( new QTimer( this ) )
    , m_AudioMasterClock( *this, this )
{
    memset( m_SilenceBuf, 0, sizeof( m_SilenceBuf ) );

    // double because some slow systems may not process frames in time for speaker read
    m_SpeakerReadBuf.setMaxSamples( AUDIO_SAMPLES_PER_FRAME * 2 );
    m_PlayerCacheBuf.setMaxSamples( AUDIO_SAMPLES_PER_FRAME * PLAYER_CACHE_FRAMES_CNT );

    m_AudioTestTimer->setInterval( 1200 );
    connect( m_AudioTestTimer, SIGNAL(timeout()), this, SLOT(slotAudioTestTimer()) );

    memset( m_MyLastAudioOutSample, 0, sizeof( m_MyLastAudioOutSample ) );

    m_AudioOutFormat.setSampleRate( AUDIO_DEVICE_SAMPLE_RATE );
    m_AudioOutFormat.setChannelCount( AUDIO_CHANNELS );
    m_AudioOutFormat.setSampleFormat( VxAudioFormat::Int16 );

    m_AudioInFormat.setSampleRate( AUDIO_DEVICE_SAMPLE_RATE );
    m_AudioInFormat.setChannelCount( AUDIO_CHANNELS );
    m_AudioInFormat.setSampleFormat( VxAudioFormat::Int16 );

    MiniAudioMgr::setEchoCancelEnable( m_MyApp.getAppSettings().getEchoCancelEnable() ); // for now always enabled

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
        m_WantMicList.emplace_back( std::make_pair( appModule, onlineId ) );
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
        m_WantSpeakerList.emplace_back( std::make_pair( appModule, onlineId ) );
    }

    bool enableSpeaker = !m_WantSpeakerList.empty();
    m_WantSpeakerMutex.unlock();

    if( ( enableSpeaker != m_WantSpeakerOutput ) && isSpeakerDeviceAvailable() )
    {
        m_WantSpeakerOutput = enableSpeaker;
        enableSpeakers( appModule, m_WantSpeakerOutput );
    }
}

//============================================================================
// enable disable sound out
void MiniAudioMgr::enableSpeakers( EAppModule appModule, bool enable )
{
    if( enable )
    {
        resetSpeakerBuffers(appModule);
    }

    m_AudioOutIo.wantSpeakerOutput( enable );
}

//============================================================================
void MiniAudioMgr::audioIoSystemStartup()
{
    if( !m_AudioIoInitialized )
    {
        int startTime = GetApplicationAliveMs();
        LogMsg( LOG_DEBUG, "%s begin at %d", __func__, startTime );
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
        int endTime = GetApplicationAliveMs();
        LogMsg( LOG_DEBUG, "%s took %d ms at %d", __func__, endTime - startTime, endTime );
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
        resetSpeakerBuffers( eAppModuleAll );
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

    m_EchoDelayResultList.emplace_back( (int)timeDif );
    // to avoid to much sound thread cpu time used Qt::QueuedConnection when connecting to these signals
    emit signalAudioTestMsg( resultMsg );
    emit signalTestedSoundDelay( (int)timeDif );
    return isValid;
}

//============================================================================
void MiniAudioMgr::setEchoCancelEnable( bool enable ) 
{ 
    if( m_EchoCancelEnabled != enable )
    {
        resetMicrophoneBuffers();
        resetSpeakerBuffers( eAppModuleAll );
        m_EchoCancelEnabled = enable; 
        m_AudioEchoCancel.enableEchoCancel( m_EchoCancelEnabled );
        if( m_MyApp.getAppSettings().getIsAppSettingInitialized() )
        {
            m_MyApp.getAppSettings().setEchoCancelEnable( m_EchoCancelEnabled );
        }
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
    
    int dnSampleDivide = getAudioInIo().getDivideSamplesCount();
    if( !dnSampleDivide )
    {
        LogMsg( LOG_FATAL, "MiniAudioMgr::callbackAudioDeviceWrite dnSampleDivide cannot be zero" );
        return;
    }

    static std::vector<int16_t> downSampled;
    static int lastMicWriteSamples = 0;

    if( 1 == dnSampleDivide )
    {
        if( sampleCnt != lastMicWriteSamples )
        {
            // first time or device changed or sample count changed
            lastMicWriteSamples = sampleCnt;

            calculateMicWriteBufferSize( sampleCnt );
        }
    }
    else
    {
        int resampledCnt = sampleCntMic / dnSampleDivide;

        if( sampleCnt != lastMicWriteSamples )
        {
            // first time or device changed or sample count changed
            lastMicWriteSamples = sampleCnt;

            downSampled.resize( resampledCnt + 1 );

            setEchoCancelerNeedsReset( true ); // tell echo canceler parameters have changed and need to restart

            calculateMicWriteBufferSize( resampledCnt );
        }

        pcmData = downSampled.data();
        AudioUtils::dnsamplePcmAudio( pcmDataMic, resampledCnt, dnSampleDivide, pcmData );
        
        sampleCnt = resampledCnt;
    }

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
        lockMicWriteBuffer();
        m_MicWriteBuf.writeSamples( pcmData, sampleCnt );
        unlockMicWriteBuffer();
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
void MiniAudioMgr::callbackToSpeakerRead( int16_t* pcmData, int sampleRequestCnt )
{
    if( !isAudioInitialized() )
    {
        memset( pcmData, 0, sampleRequestCnt * AUDIO_BYTES_PER_SAMPLE );
        m_PeakAudioOutAmplitude = 0;
        return;
    }

    lockSpeakerRead();
    int availableCnt = m_SpeakerReadBuf.getSampleCnt();
    if( availableCnt >= sampleRequestCnt)
    {
        m_SpeakerReadBuf.readSamples( pcmData, sampleRequestCnt );
        if( getIsSpeakerMuted() )
        {
            memset( pcmData, 0, sampleRequestCnt * AUDIO_BYTES_PER_SAMPLE );
            m_PeakAudioOutAmplitude = 0;
        }
        //if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_ERROR, "MiniAudioMgr::callbackToSpeakerRead full read %d samples of %d",
        //           sampleRequestCnt, availableCnt );
    }

    m_SpeakerRequestSize = sampleRequestCnt;
    unlockSpeakerRead();

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

    // this is peak value to assume recieved test sound. Hopefully not so low as to pickup noise as the test sound
    constexpr int16_t sampCompareValue = 32768 / 200;

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
        m_DelayTestDetectList.emplace_back( std::make_pair( sampleTimeMs, samplePosVal ) );
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
    // initialize echo cancel even if not enabled because may get enabled later
    getAudioEchoCancel().echoCancelStartup();

    while( false == m_ProcessAudioThread.isAborted() )
    {
        // wait until miniaudio has read some speaker output
        m_ProcessAudioSemaphore.wait();
        if( m_ProcessAudioThread.isAborted() || VxIsAppShuttingDown() )
        {
            LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded aborting" );
            break;
        }

        if( getDirectLoopbackEnable() || m_AudioTestState != eAudioTestStateNone )
        {
            lockMicWriteBuffer();
            int micSamples = m_MicWriteBuf.getSampleCnt();
            if( micSamples )
            {
                lockSpeakerRead();
                m_SpeakerReadBuf.writeSamples( m_MicWriteBuf.getSampleBuffer(), micSamples );
                unlockSpeakerRead();

                m_MicWriteBuf.samplesWereRead( micSamples );
            }

            unlockMicWriteBuffer();

            if( getEchoCancelEnable() )
            {
                getAudioEchoCancel().setEchoCancelerNeedsReset( true );
            }

            continue;
        }
        else if( isMicrophoneInputWanted() )
        {
            if( getEchoCancelEnable() )
            {
                getAudioEchoCancel().processEchoCancelThreaded();

                lockEchoCanceledBuffer();
                if( m_EchoCanceledBuf.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
                {
                    fromGuiEchoCanceledSamplesThreaded( m_EchoCanceledBuf.getSampleBuffer(), AUDIO_SAMPLES_PER_FRAME, false );
                    m_EchoCanceledBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
                }

                unlockEchoCanceledBuffer();
            }
            else
            {
                // send raw audio to engine to be processed and sent out
                lockMicWriteBuffer();
                if( m_MicWriteBuf.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
                {
                    fromGuiEchoCanceledSamplesThreaded( m_MicWriteBuf.getSampleBuffer(), AUDIO_SAMPLES_PER_FRAME, false );
                    m_MicWriteBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
                }

                unlockMicWriteBuffer();
            }
        }

        processToSpeakerThreaded();
    }

    LogMsg( LOG_VERBOSE, "processAudioThreaded leaving function" );
}

//============================================================================
void MiniAudioMgr::resetMicrophoneBuffers( void )
{
    lockMicWriteBuffer();
    m_MicWriteBuf.clear();
    unlockMicWriteBuffer();

    getAudioEchoCancel().resetMicrophoneBuffers();
}

//============================================================================
void MiniAudioMgr::resetSpeakerBuffers( EAppModule appModule )
{
    lockSpeakerRead();
    m_SpeakerReadBuf.clear();
    unlockSpeakerRead();

    if(eAppModuleMediaPlayer == appModule || eAppModuleAll == appModule)
    {
        lockPlayerCache();
        m_PlayerCacheBuf.clear();
        unlockPlayerCache();
    }

    lockModuleMixerBuffer();
    if( eAppModuleInvalid != appModule )
    {
        if( eAppModuleAll == appModule )
        {
            for( auto& buf : m_AppModuleToSpeakerMap )
            {
                buf.second.clear();
            }
        }
        else
        {
            AudioMixerBuf& mixerBuf = getAudioMixerBuf( appModule );
            mixerBuf.clear();
        }
    }

    unlockModuleMixerBuffer();

    getAudioEchoCancel().resetSpeakerBuffers();
}

//============================================================================
void MiniAudioMgr::processToSpeakerThreaded( void )
{
    lockSpeakerRead();
    int processSampleCnt = m_SpeakerRequestSize ? m_SpeakerRequestSize : AUDIO_SAMPLES_PER_FRAME;
    // miniaudio has just now read from speaker buf so unless thread is really slow free space should be valid
    int speakerFreeSpaceSampleCnt = m_SpeakerReadBuf.freeSpaceSampleCount();
    unlockSpeakerRead();
    if( speakerFreeSpaceSampleCnt < AUDIO_SAMPLES_PER_FRAME )
    {
        // not enough room to process a frame
        return;
    }

    // move player cache into module mixer so can be mixed and sent to speaker
    lockModuleMixerBuffer();
    AudioMixerBuf& mixerBuf = getAudioMixerBuf( eAppModulePlayerNlc );
    int mixerFreeSpace = mixerBuf.freeSpaceSampleCount();
 
    lockPlayerCache();
    int playerCacheSampleCnt = m_PlayerCacheBuf.getSampleCnt();
    int samplesToXfer = std::min(mixerFreeSpace, playerCacheSampleCnt );
    if( samplesToXfer > 0 )
    {
        mixerBuf.writeSamples( m_PlayerCacheBuf.getSampleBuffer(), samplesToXfer );
        m_PlayerCacheBuf.samplesWereRead( samplesToXfer );
    }       

    unlockPlayerCache();

    int mixerSampleCnt = mixerBuf.getSampleCnt();
    unlockModuleMixerBuffer();

    if( m_PlayerNlcActive && mixerSampleCnt < processSampleCnt )
    {
        // wait until at least one frame is available
        if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "Waiting for mixer space" );
        return;
    }

    // mix inputs into buffer
    std::array<int16_t, AUDIO_SAMPLES_PER_FRAME> mixBuf;
    bool firstFrameSet{ false };
    bool haveModuleMixerData{ false };

    lockModuleMixerBuffer();

    for( auto& mapEntry : m_AppModuleToSpeakerMap )
    {
        auto& mixerInBuf = mapEntry.second;

        if( mixerInBuf.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
        {
            if( !mixerInBuf.isSilent() )
            {
                haveModuleMixerData = true;
                if( firstFrameSet )
                {
                    AudioUtils::mixPcmAudio( mixBuf.data(), mixerInBuf.getSampleBuffer(), AUDIO_BUF_SIZE );
                }
                else
                {
                    memcpy( mixBuf.data(), mixerInBuf.getSampleBuffer(), AUDIO_BUF_SIZE );
                    firstFrameSet = true;
                }

            }

            mixerInBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
        }
    }

    unlockModuleMixerBuffer();
    if(!firstFrameSet)
    {
        // fill with silence
        memset( mixBuf.data(), 0, AUDIO_BUF_SIZE );
    }

    lockSpeakerRead();

    m_SpeakerReadBuf.writeSamples( mixBuf.data(), AUDIO_SAMPLES_PER_FRAME, !firstFrameSet );

    unlockSpeakerRead();

    fromGuiAudioOutSpaceAvaiThreaded( AUDIO_SAMPLES_PER_FRAME );

    if( !playerCacheSampleCnt && !haveModuleMixerData && getNeedAudioOutDeviceStop() )
    {
        // see if all is read into speaker also
        lockSpeakerRead();
        int speakerSampleCnt = m_SpeakerReadBuf.getSampleCnt();
        unlockSpeakerRead();

        if( !speakerSampleCnt )
        {
            setNeedAudioOutDeviceStop( false );
            m_AudioOutIo.stopAudioOut();
        }
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

}

//============================================================================
void MiniAudioMgr::addEchoCanceledSamples( int16_t* pcmData, int sampleCnt )
{
    lockEchoCanceledBuffer();
    m_EchoCanceledBuf.writeSamples( pcmData, sampleCnt );
    unlockEchoCanceledBuffer();
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
            lockPlayerCache();

            m_PlayerCacheBuf.clear();

            unlockPlayerCache();

            lockModuleMixerBuffer();

            AudioMixerBuf& mixerBuf = getAudioMixerBuf( eAppModulePlayerNlc );
            mixerBuf.clear();

            unlockModuleMixerBuffer();
        }
        
        toGuiWantSpeakerOutput( eAppModulePlayerNlc, m_PlayerNlcActive );
    }
}

//============================================================================
void  MiniAudioMgr::clearAllBuffers()
{
    lockMicWriteBuffer();
    m_MicWriteBuf.clear();
    unlockMicWriteBuffer();

    m_EchoCanceledBufMutex.lock();
    m_EchoCanceledBuf.clear();
    m_EchoCanceledBufMutex.unlock();

    resetSpeakerBuffers( eAppModuleAll );

}

//============================================================================
int MiniAudioMgr::toGuiPlayerNlcAudio( EAppModule appModule, float* audioDataFloat, int audioDataLenInBytes )
{
    // this assumes 20ms of stereo 48000 hz at a time from kodi based player
    // unfortunately there is no good way to down sample stereo to mono so we have to pick a channel 
    // this is because if the channels are 180 degrees out of phase they cancel each other out

    // TODO rework audio out to handle 48000 hz stereo for higher quality playback

    if( VxIsAppShuttingDown() || !audioDataLenInBytes || !audioDataFloat )
    {
        if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "MiniAudioMgr::toGuiPlayerNlcAudio ignored len %d", audioDataLenInBytes );
        return 0;
    }

    if( eAppModulePlayerNlc == appModule )
    {
        // vx_assert( AUDIO_FRAME_SIZE_KODI == audioDataLenInBytes );

        int kodiSampleCnt = audioDataLenInBytes / sizeof( float );
        int skipCnt = AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
        int pcmSampleCnt = kodiSampleCnt / skipCnt;

        lockPlayerCache();

        if( pcmSampleCnt > m_PlayerCacheBuf.freeSpaceSampleCount() )
        {
            unlockPlayerCache();
            LogMsg( LOG_ERROR, "MiniAudioMgr::toGuiPlayerNlcAudio overrun PlayerNlc" );
            return 0;
        }

        int16_t* pcmBuf = m_PlayerCacheBuf.getSampleBuffer();
        pcmBuf += m_PlayerCacheBuf.getSampleCnt();
        int totalSamples{ 0 };
        for( int i = 0; i < audioDataLenInBytes / sizeof( float ); i += skipCnt )
        {
            pcmBuf[ totalSamples ] = AudioUtils::floatToPcm( audioDataFloat[ i ] );
            totalSamples++;
        }

        m_PlayerCacheBuf.samplesWereWritten( totalSamples );

        unlockPlayerCache();
/*
        if( LogEnabled( eLogAudioIo ) )
        {
            float cachedTime = toGuiGetAudioDelaySeconds( appModule );
            float totalCache = toGuiGetAudioCacheTotalSeconds( appModule );

            if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "MiniAudioMgr::toGuiPlayerNlcAudio player-nlc samples %d cached sec %3.3f total cache %3.3f sec percent %d", 
                       totalSamples, cachedTime, totalCache, (int)((cachedTime / totalCache )*100) );
        }
        */
    }
    else
    {
        LogMsg( LOG_ERROR, "MiniAudioMgr::toGuiPlayerNlcAudio unknown module %s ", DescribeAppModule( appModule ) );
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

    if( eAppModulePlayerNlc == appModule )
    {
        lockPlayerCache();
        int cachedPcmSamples = m_PlayerCacheBuf.getSampleCnt();
        unlockPlayerCache();

        return calculateMsOfSamples( cachedPcmSamples ) * 1000;
    }

    lockModuleMixerBuffer();
    int usedPcmSamples = getAudioMixerBuf( appModule ).getSampleCnt();
    unlockModuleMixerBuffer();

    return calculateMsOfSamples( usedPcmSamples ) * 1000;
}

//============================================================================
float MiniAudioMgr::toGuiGetAudioCacheFreeSpace( EAppModule appModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    if( eAppModulePlayerNlc == appModule )
    {
        lockPlayerCache();
        int availPcmSamplesCache = m_PlayerCacheBuf.freeSpaceSampleCount();
        unlockPlayerCache();

        return availPcmSamplesCache * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
    }

    lockModuleMixerBuffer();
    int availablePcmSampleSpace = getAudioMixerBuf( appModule ).freeSpaceSampleCount();
    unlockModuleMixerBuffer();

    return availablePcmSampleSpace;
}

//============================================================================
float MiniAudioMgr::toGuiGetAudioCacheTotalSeconds( EAppModule appModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    if( eAppModulePlayerNlc == appModule )
    {
        lockPlayerCache();
        int maxPcmSamplesCache = m_PlayerCacheBuf.getMaxSamples();
        unlockPlayerCache();

        return calculateMsOfSamples( maxPcmSamplesCache ) * 1000;
    }

    lockModuleMixerBuffer();
    float mixerMaxSamples = getAudioMixerBuf( appModule ).getMaxSamples();
    unlockModuleMixerBuffer();

    return calculateMsOfSamples( mixerMaxSamples ) * 1000;
}

//============================================================================
int MiniAudioMgr::toGuiModuleAudioFrame( EAppModule appModule, int16_t* pu16PcmData, int pcmDataLenInBytes, bool isSilence )
{
    // assumes must be 80 ms of pcm mono
    vx_assert( pcmDataLenInBytes == AUDIO_BUF_SIZE)
    lockModuleMixerBuffer();

    AudioMixerBuf& mixerBuf = getAudioMixerBuf( appModule );
    int wroteSamples = mixerBuf.writeSamples( pu16PcmData, pcmDataLenInBytes / 2, isSilence );

    unlockModuleMixerBuffer();
    return wroteSamples * 2;
 }

//============================================================================
AudioMixerBuf& MiniAudioMgr::getAudioMixerBuf( EAppModule appModule )
{
    auto iter = m_AppModuleToSpeakerMap.find( appModule );
    if( iter != m_AppModuleToSpeakerMap.end() ) 
    {
        // found
        return iter->second;
    }

    // not found
    AudioMixerBuf mixBuf;
    mixBuf.setAudioIoMgr( this );
    mixBuf.setAppModule( appModule );
    m_AppModuleToSpeakerMap.insert( std::make_pair(appModule, mixBuf ));

    auto newIter = m_AppModuleToSpeakerMap.find( appModule );

    return newIter->second;
}

//============================================================================
void MiniAudioMgr::calculateMicWriteBufferSize( int micSampleCnt )
{
    // ideally the mic would write AUDIO_SAMPLES_PER_FRAME each write to minimize delay
    // but some platforms do not let you change the amount of samples each write
    if( !micSampleCnt )
    {
        LogMsg( LOG_FATAL, "calculateMicWriteBufferSize micSampleCnt cannot be zeor" );
        return;
    }

    if( micSampleCnt > AUDIO_SAMPLES_PER_FRAME )
    {
        // if the mic is writing more than AUDIO_SAMPLES_PER_FRAME 
        // the buffer must be at to hold the max of 2 mic writes or 2 AUDIO_SAMPLES_PER_FRAME
        int bufSampleCnt = std::max( micSampleCnt * 2, AUDIO_SAMPLES_PER_FRAME * 2 );
        lockMicWriteBuffer();
        m_MicWriteBuf.setMaxSamples( bufSampleCnt );
        unlockMicWriteBuffer();
    }
    else if( AUDIO_SAMPLES_PER_FRAME % micSampleCnt )
    {
        // AUDIO_SAMPLES_PER_FRAME is not an even multiple of micSampleCnt
        int bufSampleCnt = AUDIO_SAMPLES_PER_FRAME + ( micSampleCnt * 2 );
        lockMicWriteBuffer();
        m_MicWriteBuf.setMaxSamples( bufSampleCnt );
        unlockMicWriteBuffer();
    }
    else
    {
        lockMicWriteBuffer();
        m_MicWriteBuf.setMaxSamples( AUDIO_SAMPLES_PER_FRAME + micSampleCnt );
        unlockMicWriteBuffer();
    }
}

//============================================================================
void MiniAudioMgr::onAudioDevicesInitialized( bool hasDevices )
{
    m_AudioDevicesInitialized = true;
}
