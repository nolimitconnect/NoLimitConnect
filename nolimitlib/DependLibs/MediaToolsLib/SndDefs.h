#pragma once
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

#include <CoreLib/VxDefs.h>
#include <GuiInterface/IAudioDefs.h>

#define MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN		AUDIO_BUF_SIZE				// uncompressed max length of audio packet
#define MY_PCM_SAMPLE_CNT						AUDIO_SAMPLES_PER_FRAME		// expected PCM samples to encode / decode at a time
#define MY_OPUS_PKT_MS_PER_PKT					AUDIO_MS_PER_FRAME			// milliseconds per pkt

#define MY_OPUS_FRAME_SAMPLE_CNT				960		// feed into opus frame size in samples needs frame of 960 or returns bad argument error in most cases
#define MY_OPUS_FRAME_BYTE_LEN					MY_OPUS_FRAME_SAMPLE_CNT * AUDIO_BYTES_PER_SAMPLE	// opus frame size in bytes

#define MY_OPUS_FRAME_CNT						AUDIO_SAMPLES_PER_FRAME / MY_OPUS_FRAME_SAMPLE_CNT	

#define MY_OPUS_TARGET_BIT_RATE					ECHO_SAMPLE_RATE
#define MY_OPUS_SAMPLE_RATE						ECHO_SAMPLE_RATE	// frequency of input samples
#define MY_OPUS_CHANNELS						AUDIO_CHANNELS	// frequency of input samples


