//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioTestGenerator.h"

#include <GuiInterface/IAudioDefs.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/IsBigEndianCpu.h>

#include <qmath.h>
#include <qendian.h>

//============================================================================
AudioTestGenerator::AudioTestGenerator( const VxAudioFormat& format, int64_t durationUs, int toneHz )
{
    m_AudioFormat = format;
    if( format.isValid() )
    {
        generateData( format, durationUs, toneHz );
    }
}

//============================================================================
void AudioTestGenerator::setAudioFormat( VxAudioFormat& audioFormat ) 
{ 
    m_AudioFormat = audioFormat; 
    if( m_AudioFormat.isValid() )
    {
        generateData( m_AudioFormat, AUDIO_MS_PER_FRAME * 2 * 1000, 200 );
    }
}

//============================================================================
void AudioTestGenerator::generateData( const VxAudioFormat &format, int64_t durationUs, int toneHz )
{
    int rate = format.sampleRate();
    const int channelBytes = format.bytesPerSample();
    const int sampleBytes = format.channelCount() * channelBytes;
    int64_t length = format.bytesForDuration( durationUs );
    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED( sampleBytes ); // suppress warning in release builds

    m_buffer.resize(length);
    unsigned char* ptr = reinterpret_cast<unsigned char*>(m_buffer.data());
    int sampleIndex = 0;

    while( length ) {
        // Produces value (-1..1)
        qreal x = qSin( 2 * M_PI * toneHz * qreal( sampleIndex++ % rate ) / rate );
        for( int i = 0; i < format.channelCount(); ++i ) {
            switch( format.sampleFormat() ) {
            case VxAudioFormat::UInt8:
                *reinterpret_cast<quint8*>(ptr) = static_cast<quint8>((1.0 + x) / 2 * 255);
                break;
            case VxAudioFormat::Int16:
                *reinterpret_cast<qint16*>(ptr) = static_cast<qint16>(x * 32767);
                break;
            case VxAudioFormat::Int32:
                *reinterpret_cast<qint32*>(ptr) = static_cast<qint32>(x * std::numeric_limits<qint32>::max());
                break;
            case VxAudioFormat::Float:
                *reinterpret_cast<float*>(ptr) = x;
                break;
            default:
                break;
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
    }
}

//============================================================================
int64_t AudioTestGenerator::readData( char *data, int64_t len )
{
    int64_t total = 0;
    if( !m_buffer.isEmpty() ) 
    {
        while( len - total > 0 ) 
        {
            const int64_t chunk = qMin( ( m_buffer.size() - m_pos ), len - total );
            memcpy( data + total, m_buffer.constData() + m_pos, chunk );
            m_pos = ( m_pos + chunk ) % m_buffer.size();
            total += chunk;
        }
    }

    return total;
}

//============================================================================
int16_t AudioTestGenerator::peekNextSample( void )
{
    int16_t nextSample{ 0 };
    if( !m_buffer.isEmpty() && m_AudioFormat.sampleFormat() == VxAudioFormat::Int16 ) 
    {
        if( m_pos <= m_buffer.length() - 2 )
        {
            // read from current position
            nextSample = *((int16_t *)(m_buffer.constData() + m_pos));
        }
        else
        {
            // read from start position
            nextSample = *((int16_t*)(m_buffer.constData()));
        }
    }

    return nextSample;
}

//============================================================================
void AudioTestGenerator::readToneSamples( int16_t* pcmData, int sampleCnt )
{
    readData( (char*)pcmData, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
}