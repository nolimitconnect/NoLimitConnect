//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "EchoUtil.h"

//============================================================================
void PcmS16ToFloats( int16_t * pcmData, int sampleCnt, float * retFloatBuf )
{
	for( int i = 0; i < sampleCnt; i++ )
	{
		retFloatBuf[i] = PcmS16ToFloat( pcmData[i] );
	}
}

//============================================================================
void FloatsToPcmS16( float * floatBuf, int16_t * pcmRetData, int sampleCnt )
{
	for( int i = 0; i < sampleCnt; i++ )
	{
		pcmRetData[i] = FloatToPcmS16( floatBuf[i] );
	}
}