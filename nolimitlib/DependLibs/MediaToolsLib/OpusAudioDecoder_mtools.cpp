//============================================================================
// Copyright (C) 2015 Brett R. Jones7
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "OpusAudioDecoder.h"
#include "SndDefs.h"

#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <math.h>
#include <stdlib.h>
#include <memory.h>

#ifdef HAVE_LRINTF
# define float2int(x) lrintf(x)
#else
# define float2int(flt) ((int)(floor(.5+flt)))
#endif

// 120ms at 48000
#define MAX_FRAME_SIZE (960*6)

#define readint(buf, base) (((buf[base+3]<<24)&0xff000000)| \
	((buf[base+2]<<16)&0xff0000)| \
	((buf[base+1]<<8)&0xff00)| \
	(buf[base]&0xff))
static unsigned int rngseed = 22222;
static NLC_INLINE unsigned int fast_rand(void) {
	rngseed = (rngseed * 96314165) + 907633515;
	return rngseed;
}

#ifndef HAVE_FMINF
# define fminf(_x,_y) ((_x)<(_y)?(_x):(_y))
#endif

#ifndef HAVE_FMAXF
# define fmaxf(_x,_y) ((_x)>(_y)?(_x):(_y))
#endif

#if !defined(__LITTLE_ENDIAN__) && ( defined(NLC_ARCH_BIGENDIAN) || defined(__BIG_ENDIAN__) )
#define le_short(s) ((short) ((unsigned short) (s) << 8) | ((unsigned short) (s) >> 8))
#define be_short(s) ((short) (s))
#else
#define le_short(s) ((short) (s))
#define be_short(s) ((short) ((unsigned short) (s) << 8) | ((unsigned short) (s) >> 8))
#endif 

//============================================================================
OpusAudioDecoder::OpusAudioDecoder( int sampleRate, int channels )
: m_SampleRate( sampleRate )
, m_Channels( channels )
{
	initDecoder();
}

//============================================================================
OpusAudioDecoder::~OpusAudioDecoder()
{
	if( m_OpusDecoder )
	{
		opus_decoder_destroy( m_OpusDecoder );
		m_OpusDecoder = 0;
	}
}

//============================================================================
bool OpusAudioDecoder::initDecoder( void )
{
	m_OpusDecoder = opus_decoder_create( 
		MY_OPUS_SAMPLE_RATE,
			m_Channels,
			&m_OpusErr );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error cannot create decoder: %s", opus_strerror( m_OpusErr ) );
		return false;
	}

	if( ! m_OpusDecoder )
	{
		LogMsg( LOG_ERROR, "Decoder initialization failed: NULL decoder" );
		return false;
	}

	/*
	int err;
	m_Resampler = speex_resampler_init( m_Channels, 48000, m_SampleRate, 5, &err );
	if( err!=0 )
	{
		LogMsg( LOG_ERROR, "m_Resampler error: %s", speex_resampler_strerror( err ) );
	}

	speex_resampler_skip_zeros( m_Resampler );
	*/

	m_DecoderInitialized = true;
	return m_DecoderInitialized;
}

//============================================================================
bool OpusAudioDecoder::decodeToPcmData(	uint8_t*	encodedOpusAudio,	
										std::vector<uint16_t>& opusEncodedLenList,
										int16_t*	pcmDataBuf, 
										int32_t		pcmDataBufLen )
{
	if( ( false == m_DecoderInitialized )
		|| VxIsAppShuttingDown() )
	{
		LogMsg( LOG_ERROR, "ERROR: OpusAudioDecoder::decodeToPcmData not initialized" );
		return false;
	}

	bool result = true;

	int totalDecodedSamples = 0;
	int toDecodeOffs = 0;

	for( auto encodedLen : opusEncodedLenList )
	{
		if( encodedLen )
		{
			int decodedSamples = decodeOneFrameToPcmData( &encodedOpusAudio[ toDecodeOffs ],
				encodedLen,
				&pcmDataBuf[ totalDecodedSamples ],
				MY_OPUS_FRAME_SAMPLE_CNT * AUDIO_BYTES_PER_SAMPLE);
			if( MY_OPUS_FRAME_SAMPLE_CNT != decodedSamples )
			{
				result = false;
				LogMsg( LOG_ERROR, "Decoding failed: %s", opus_strerror( decodedSamples ) );
				break;
			}

			totalDecodedSamples += decodedSamples;
			toDecodeOffs += encodedLen;
		}
	}

	if( result )
	{
		int totalSampleCnt = totalDecodedSamples;
		if( MY_PCM_SAMPLE_CNT >  totalSampleCnt )
		{
			if( ( MY_PCM_SAMPLE_CNT * 2 ) == pcmDataBufLen )
			{
				// need to fill empty part of frame with silence.. either before or after the samples depending on if we are starting or not
				int decodedBytes = totalSampleCnt * 2;
				int fillBytes = (MY_PCM_SAMPLE_CNT  * 2) - decodedBytes;
				uint8_t * dataBuf = (uint8_t *)pcmDataBuf;
				if( m_FirstFillSilence )
				{
					m_FirstFillSilence = false;
					memmove( dataBuf, &dataBuf[fillBytes], decodedBytes );
					memset( dataBuf, 0, fillBytes );
				}
				else
				{
					memset( &dataBuf[decodedBytes], 0, fillBytes );
				}
			}
		}
	}

	return result;
}
	
//============================================================================
int OpusAudioDecoder::decodeOneFrameToPcmData(	uint8_t *	encodedOpusAudio,	
												int32_t		encodedFrameLenBytes,
												int16_t *	pcmDataBuf, 
												int32_t		pcmDataBufLen )
{
	int ret = opus_decode( m_OpusDecoder, (unsigned char*)encodedOpusAudio, encodedFrameLenBytes, (opus_int16 *)pcmDataBuf, pcmDataBufLen, 0);

	// If the decoder returned less than zero, we have an error
	if( ret != MY_OPUS_FRAME_SAMPLE_CNT )
	{
		LogMsg( LOG_ERROR,  "OpusAudioDecoder::decodeOneFrameToPcmData Decoding error: %s", opus_strerror(ret));
		return 0;
	}

	/*
	if( !IsBigEndianCpu() )
	{
		for( int sampIdx = 0; sampIdx < MY_OPUS_FRAME_SAMPLE_CNT; sampIdx++ )
		{
			pcmDataBuf[ sampIdx ] = ntohs( pcmDataBuf[ sampIdx ] );
		}
	}
	*/

	return ret;
}

//============================================================================
int OpusAudioDecoder::opusFloatOutputToPcm(	float *					opusOutput, 
											int						channels, 
											int						frameSampleCnt, 
											SpeexResamplerState *	resampler,
											int *					skip, 
											uint8_t *				pcmOutBuf,
											unsigned int			maxout )
{
	unsigned int sampout = 0;
	int i,tmp_skip;
	uint32_t out_len;
	short *out;
	float *buf;
	float *output;
	out = (short *)alloca(sizeof(short)*MAX_FRAME_SIZE*m_Channels);
	buf = (float *)alloca(sizeof(float)*MAX_FRAME_SIZE*m_Channels);

	do {
		if (skip)
		{
			tmp_skip = (*skip>frameSampleCnt) ? (int)frameSampleCnt : *skip;
			*skip -= tmp_skip;
		} 
		else 
		{
			tmp_skip = 0;
		}

		if (resampler)
		{
			uint32_t in_len;
			output=buf;
			in_len = frameSampleCnt-tmp_skip;
			out_len = (unsigned int)(1024<maxout?1024:maxout);
			speex_resampler_process_interleaved_float( resampler, opusOutput+m_Channels*tmp_skip, &in_len, buf, &out_len);
			opusOutput += m_Channels*(in_len+tmp_skip);
			frameSampleCnt -= in_len+tmp_skip;
		} 
		else 
		{
			output=opusOutput+m_Channels*tmp_skip;
			out_len=frameSampleCnt-tmp_skip;
			frameSampleCnt=0;
		}

		for (i=0;i<(int)out_len*m_Channels;i++)
			out[i]=(short)float2int(fmaxf(-32768,fminf(output[i]*32768.f,32767)));

		if( !IsBigEndianCpu() )
		{
			for (i=0;i<(int)out_len*m_Channels;i++)
				out[i]=le_short(out[i]);
		}

		if( ( maxout > 0 ) && ( sampout < maxout ) )
		{
			int totalLen = (int)(out_len < maxout ? out_len : maxout);
			int amountUsed = 0;
			char * tempOut = (char *)out;

			unsigned int lenToCopy = totalLen * sizeof( int16_t );
			lenToCopy = lenToCopy > ( maxout - sampout ) ? ( maxout - sampout ) : lenToCopy;
			uint8_t * outBuf = &pcmOutBuf[sampout];
			memcpy( outBuf, tempOut, lenToCopy );
			amountUsed = totalLen;
			sampout+=amountUsed;
			maxout-=amountUsed;
		}

	} while (frameSampleCnt>0 && maxout>0);

	return sampout;
}

