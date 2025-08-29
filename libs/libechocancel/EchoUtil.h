#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxDefs.h>

static NLC_INLINE int16_t FloatToPcmS16( float v ) 
{
	if (v > 0)
		return v >= 1.0f ? S16_MAXVAL
		: static_cast<int16_t>(v * S16_MAXVAL + 0.5f);
	return v <= -1.0f ? S16_MINVAL
		: static_cast<int16_t>(-v * S16_MINVAL - 0.5f);
}

static NLC_INLINE float PcmS16ToFloat( int16_t v )
{
	static const float kMaxInt16Inverse = 1.f / S16_MAXVAL;
	static const float kMinInt16Inverse = 1.f / S16_MINVAL;
	return v * (v > 0 ? kMaxInt16Inverse : -kMinInt16Inverse);
}

void PcmS16ToFloats( int16_t* pcmData, int sampleCnt, float* retFloatBuf );
void FloatsToPcmS16( float* floatBuf, int16_t* pcmRetData, int sampleCnt );
