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

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "AppCommon.h"
#include "AudioMixer.h"
#include "AudioUtils.h"
#include "MiniAudioMgr.h"

#include <GuiInterface/IAudioInterface.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTimer.h>

#include <algorithm>
#include <memory.h>

//============================================================================
AudioMixer::AudioMixer( MiniAudioMgr& audioIoMgr, IAudioCallbacks& audioCallbacks, QWidget* parent )
: QWidget( parent )
, m_AudioIoMgr( audioIoMgr )
, m_MyApp( audioIoMgr.getMyApp() )
, m_AudioCallbacks( audioCallbacks )
, m_MixerFormat()
 {
     m_MixerFormat.setSampleRate( ECHO_SAMPLE_RATE );
     m_MixerFormat.setChannelCount( AUDIO_CHANNELS );
     m_MixerFormat.setSampleFormat( QAudioFormat::Int16 );

     memset( m_MixerBuf, 0, sizeof( m_MixerBuf ) );
     memset( m_QuietEchoBuf, 0, sizeof( m_QuietEchoBuf ) );

     for( int i = 0; i < MAX_GUI_MIXER_FRAMES; i++ )
     {
         m_MixerFrames[ i ].setFrameIndex( i );
         m_MixerFrames[ i ].setAudioIoMgr( &audioIoMgr );
     }

     //m_EchoCanceledBitrate.setLogMessagePrefix( "Echo Canceled " );
     //m_ProcessFrameBitrate.setLogMessagePrefix( "Process Frame " );
     //m_ProcessSpeakerBitrate.setLogMessagePrefix( "Process Speaker " );
     //m_SpeakerReadBitrate.setLogMessagePrefix( "Speaker Read " );

     //m_ProcessAudioMixerThread.startThread( (VX_THREAD_FUNCTION_T)AudioMixerProcessThreadFunc, this, "AudioMixerGuiProcessor" );
 }

//============================================================================
void AudioMixer::shutdownAudioMixer( void )
{
    // m_ProcessAudioMixerThread.abortThreadRun( true );
    // m_AudioMixerSemaphore.signal();
}

//============================================================================
void AudioMixer::resetMixer( void )
{
    // reset everthing to initial start positions
    m_MixerMutex.lock();
    for( int i = 0; i < MAX_PTOP_MIXER_FRAMES; i++ )
    {
        m_MixerFrames[ i ].clearFrame( true );
    }

    m_MixerReadIdx = 0;
    m_WasReset = true;
    m_PrevLerpedSamplesCnt = 0;
    m_PrevLerpedSampleValue = 0;
    m_MixerMutex.unlock();
}

//============================================================================
int AudioMixer::toGuiAudioFrameThreaded( EAppModule appModule, int16_t* pcmData, bool isSilenceIn )
{
    lockMixer();
    AudioMixerFrame& audioFrame = getAudioWriteFrame();
    int result = audioFrame.toMixerPcmMonoChannel( appModule, pcmData, isSilenceIn );
    unlockMixer();
    return result;
}

//============================================================================
int AudioMixer::incrementMixerWriteIndex( void )
{
    m_MixerWriteIdx++;
    if( m_MixerWriteIdx >= MAX_GUI_MIXER_FRAMES )
    {
        m_MixerWriteIdx = 0;
    }

    if( m_AudioIoMgr.getFrameIndexDebugEnable() )
    {
        int64_t timeNow = GetHighResolutionTimeMs();
        static int64_t lastMixerPcmTime{ 0 };
        static int funcCallCnt{ 0 };
        funcCallCnt++;
        if( lastMixerPcmTime )
        {
            int timeInterval = (int)(timeNow - lastMixerPcmTime);
            int avgTimeInterval = timeInterval;
            static std::vector<int> intervalList;
            intervalList.push_back( timeInterval );
            if( intervalList.size() > 20 )
            {
                int totalTime = 0;
                intervalList.erase( intervalList.begin() );
                for( auto interval : intervalList )
                {
                    totalTime += interval;
                }

                avgTimeInterval = totalTime / 20;
            }


            LogMsg( LOG_VERBOSE, "W Frame %d call cnt %d incrementMixerWriteIndex elapsed %d ms avg elapsed %d", m_MixerWriteIdx, funcCallCnt, timeInterval, avgTimeInterval );
        }

        lastMixerPcmTime = timeNow;
    }

    return m_MixerWriteIdx;
}

//============================================================================
int AudioMixer::incrementMixerReadIndex( void )
{
    m_MixerReadIdx++;
    if( m_MixerReadIdx >= MAX_GUI_MIXER_FRAMES )
    {
        m_MixerReadIdx = 0;
    }

    if( m_AudioIoMgr.getFrameIndexDebugEnable() )
    {
        int64_t timeNow = GetHighResolutionTimeMs();
        static int64_t lastMixerPcmTime{ 0 };
        static int funcCallCnt{ 0 };
        funcCallCnt++;
        if( lastMixerPcmTime )
        {
            int timeInterval = (int)(timeNow - lastMixerPcmTime);
            LogMsg( LOG_VERBOSE, "R Frame %d call cnt %d incrementMixerReadIndex elapsed %d ms", m_MixerReadIdx, funcCallCnt, timeInterval );
        }

        lastMixerPcmTime = timeNow;
    }

    return m_MixerReadIdx;
}
