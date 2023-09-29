//============================================================================
// Copyright (C) 2015 Brett R. Jones 
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
// https://nolimitconnect.com
//============================================================================

#include "AudioJitterBuffer.h"

#include <MediaToolsLib/SndDefs.h>

// implements circular buffer and delays available data for anti jitter reasons
// buffers are MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN long

// #define DEBUG_AUDIO_JITTER_BUF

//============================================================================
AudioJitterBuffer::AudioJitterBuffer( int queDepth )
{
	for( int i = 0; i < queDepth; i++ )
	{
		m_BufList.push_back( new char[ MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN] );
	}
}

//============================================================================
AudioJitterBuffer::~AudioJitterBuffer()
{
	for( char* buf : m_BufList)
	{
		delete buf;
	}

	m_BufList.clear();
}

//============================================================================
void AudioJitterBuffer::clearBuffers( void )  // resets data avail and starts over
{
	m_HeadIdx	= 0;
	m_TailIdx	= 0;
	m_BufsUsed	= 0;
}

//============================================================================
// gets buffer to write to.. if null then overflowed.. increments index if buffer given
char * AudioJitterBuffer::getBufToFill( void )
{
	char * retBuf = nullptr;
	if( m_BufsUsed < m_BufList.size() )
	{
		retBuf = m_BufList[ m_HeadIdx ];
		m_HeadIdx++;
		if( m_HeadIdx >= m_BufList.size() )
		{
			m_HeadIdx = 0;
		}

		m_BufsUsed++;
	}

	if( !retBuf )
	{
		// throw away a buffer
		LogModule( eLogMediaStream, LOG_VERBOSE, "AudioJitterBuffer::getBufToFill overflow this %p throw away idx %d", this, m_TailIdx );
		getBufToRead();
	}
#ifdef DEBUG_AUDIO_JITTER_BUF
	else
	{
		LogMsg( LOG_INFO, "AudioJitterBuffer::getBufToFill success %p next idx %d used %d", this, m_HeadIdx, m_BufsUsed );
	}
#endif // DEBUG_AUDIO_JITTER_BUF

	return retBuf;
}

//============================================================================
// gets buffer to read.. if null then no data available.. increments index if buffer given
char * AudioJitterBuffer::getBufToRead( void )
{
	char * retBuf = nullptr;
	if( m_BufsUsed )
	{
		retBuf = m_BufList[ m_TailIdx ];
		m_TailIdx++;
		if( m_TailIdx >= m_BufList.size() )
		{
			m_TailIdx = 0;
		}

		m_BufsUsed--;
	}

	if( !retBuf )
	{
		LogModule( eLogMediaStream, LOG_VERBOSE, "AudioJitterBuffer::getBufToRead underflow this %p", this );
	}
#ifdef DEBUG_AUDIO_JITTER_BUF
	else
	{
		LogMsg( LOG_INFO, "AudioJitterBuffer::getBufToRead success %p next idx %d used %d", this, m_TailIdx, m_BufsUsed );
	}
#endif // DEBUG_AUDIO_JITTER_BUF

	return retBuf;
}

