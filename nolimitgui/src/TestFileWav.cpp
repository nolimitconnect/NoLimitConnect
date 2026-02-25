//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TestFileWav.h"

#include <libwav-decoder/WavMgr.h>

#include <CoreLib/VxDebug.h>
#include <algorithm>

//============================================================================
TestFileWav::TestFileWav( const QString& filePath )
    : m_FilePath( filePath )
{
    int bitsPerSample{0};
    m_Valid = loadWavFile( filePath, m_SampleRate, m_NumChannels, bitsPerSample, m_PcmData );
    if( m_Valid  )
    {
        if( bitsPerSample != 16 )
        {
            LogMsg( LOG_ERROR, "%s Unsupported bits per sample %d in file %s. Only 16-bit PCM is supported.", __func__, bitsPerSample, filePath.toStdString().c_str() );
            m_Valid = false;
            return;
        }

        if( m_NumChannels != 1 )
        {
            LogMsg( LOG_ERROR, "%s Invalid number of channels %d in file %s.", __func__, m_NumChannels, filePath.toStdString().c_str() );
            m_Valid = false;
            return;
        }

        if( m_SampleRate != 16000 )
        {
            LogMsg( LOG_ERROR, "%s Invalid sample rate %d in file %s. Only 16kHz is supported.", __func__, m_SampleRate, filePath.toStdString().c_str() );
            m_Valid = false;
            return;
        }

        m_NumFrames = static_cast<int>( (m_PcmData.size() ) / m_NumChannels) / (m_SampleRate / 100); // number of 10ms frames
        if( m_NumFrames <= 0 )
        {
            LogMsg( LOG_ERROR, "%s No audio frames found in file %s.", __func__, filePath.toStdString().c_str() );
            m_Valid = false;
            return;
        }

        LogMsg( LOG_DEBUG, "%s Loaded WAV file %s: SampleRate=%d, Channels=%d, BitsPerSample=%d, NumFrames=%d", __func__, filePath.toStdString().c_str(), m_SampleRate, m_NumChannels, bitsPerSample, m_NumFrames );
    }
    else
    {
        LogMsg( LOG_ERROR, "%s Failed to load WAV file %s", __func__, filePath.toStdString().c_str() );
    }
}

//============================================================================
bool TestFileWav::loadWavFile( const QString& filePath, int& sampleRate, int& numChannels, int& bitsPerSample, std::vector<int16_t>& pcmData )
{
    std::string filePathStr = filePath.toStdString();
    return WavMgr::readWavFile( filePathStr, pcmData, sampleRate, numChannels, bitsPerSample );
}

//============================================================================
bool TestFileWav::getNextAudioFrame( int16_t* frameBuffer, int frameSize )
{
    if( !frameBuffer || frameSize <= 0 )
    {
        return false;
    }

    int samplesPerFrame = frameSize * m_NumChannels; // For mono, this is just frameSize
    int16_t* pcmDataPtr = const_cast<int16_t*>( getPcmData() ); // We won't modify the data, just need a non-const pointer for indexing

    if( m_CurrentFrameIndex >= m_NumFrames )
    {
        return false; // No more frames to read
    }

    std::copy_n(pcmDataPtr + (m_CurrentFrameIndex * samplesPerFrame), samplesPerFrame, frameBuffer);
    m_CurrentFrameIndex++;
    return true;
}