#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxDefs.h>
#include <GuiInterface/IAudioDefs.h>

#define MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN		AUDIO_BUF_SIZE				// uncompressed max length of audio packet
#define MY_PCM_SAMPLE_CNT						AUDIO_SAMPLES_PER_FRAME		// expected PCM samples to encode / decode at a time
#define MY_OPUS_PKT_MS_PER_PKT					AUDIO_MS_PER_FRAME			// milliseconds per pkt

#define MY_OPUS_FRAME_SAMPLE_CNT				OPUS_COMPRESSED_SAMPLES_PER_FRAME	// feed into opus encode frame size in samples 
#define MY_OPUS_FRAME_BYTE_LEN					OPUS_COMPRESSED_BYTES_PER_FRAME	    // opus frame size in bytes

#define MY_OPUS_FRAME_CNT						AUDIO_SAMPLES_PER_FRAME / MY_OPUS_FRAME_SAMPLE_CNT	

#define MY_OPUS_TARGET_BIT_RATE					ECHO_SAMPLE_RATE
#define MY_OPUS_SAMPLE_RATE						ECHO_SAMPLE_RATE	// frequency of input samples
#define MY_OPUS_CHANNELS						AUDIO_CHANNELS		// nuber of audio channels


