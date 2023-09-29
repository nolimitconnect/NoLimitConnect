//============================================================================
// Copyright (C) 2014 Brett R. Jones 
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

#include "PktTypes.h"
#include "PktVoiceReq.h"

//============================================================================
PktVoiceReq::PktVoiceReq()
{
	setPktType( PKT_TYPE_VOICE_REQ );
}

//============================================================================
void PktVoiceReq::calcPktLen( void )
{
	setPktLength( ROUND_TO_16BYTE_BOUNDRY( (sizeof( PktVoiceReq ) - ( sizeof( m_CompressedData ) ) + (getFrame1Len() + getFrame2Len() + getFrame3Len() + getFrame4Len()) ) ) );
}

