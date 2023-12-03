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

#include "VxDefs.h"

#include <vector>

class PktVoiceReq;
class PktVideoFeedPic;
class PktVideoFeedPicChunk;
class VxGUID;

class MediaCallbackInterface
{
public:
	virtual void				callbackPcm( void* userData, VxGUID& feedId, int16_t* pcmData, uint16_t pcmDataLen ){};
	virtual void				callbackOpusEncoded( void* userData, uint8_t* encodedAudio, std::vector<uint16_t>& encodedLenList ){};
	virtual void				callbackOpusPkt( void* userData, PktVoiceReq* pktOpusAudio ){};
	virtual void				callbackAudioOutSpaceAvail( int freeSpaceLen ){};

	virtual void				callbackVideoJpgBig( void* userData, VxGUID& vidFeedId, uint8_t* jpgData, uint32_t jpgDataLen ){};
	virtual void				callbackVideoJpgSmall( void* userData, VxGUID& vidFeedId, uint8_t* jpgData, uint32_t jpgDataLen, int motion0to100000 ){};
	virtual void				callbackVideoPktPic( void* userData, VxGUID& onlineId, PktVideoFeedPic* pktVid, int pktsInSequence, int thisPktNum ){};
	virtual void				callbackVideoPktPicChunk( void* userData, VxGUID& onlineId, PktVideoFeedPicChunk* pktVid, int pktsInSequence, int thisPktNum ){};
};
