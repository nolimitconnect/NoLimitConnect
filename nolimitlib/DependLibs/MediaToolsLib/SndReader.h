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

#include "SndDefs.h"

#include <GuiInterface/IFromGui.h>
#include <PktLib/VxCommon.h>

#include <CoreLib/VxTimer.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/MediaCallbackInterface.h>

#include <string>
#include <stdio.h>
#include <memory.h>

enum ESndPlayState
{
	eSndPlayStateDisabled,
	eSndPlayStateStopPlaying,
	eSndPlayStateStartPlaying,
	eSndPlayStateStartPlayInSeekPos,
	eSndPlayStatePausePlaying,
	eSndPlayStateResumePlaying,
	eSndPlayStateCancelPlaying,
	eSndPlayStateError,

	eMaxSndPlayState
};

class P2PEngine;
class MediaProcessor;
class IToGui;
class AssetInfo;
class OpusAudioDecoder;
class OpusFileDecoder;

class SndReader : public MediaCallbackInterface
{
public:
	SndReader( P2PEngine& engine, MediaProcessor& mediaProcessor );
	virtual ~SndReader();

	void						setIsPlaying( bool isPlaying )				{ m_IsPlaying = isPlaying; }
	bool						getIsPlaying( void )						{ return m_IsPlaying; }
	void						setIsPlayingPaused( bool isPaused );
	bool						getIsPlayingPaused( void )					{ return m_IsPlayingPaused; }


	bool						fromGuiIsNoLimitAudioFile( const char* fileName );	
	bool						fromGuiSndPlay( ESndPlayState ePlayState, VxGUID& assetId, int pos0to100000  );
	bool						fromGuiAssetAction( AssetBaseInfo& assetInfo, EAssetAction assetAction, int pos0to100000  );

	bool						startSndRead( const char* fileName, VxGUID& assetId, int pos0to100000 );
	void						stopSndRead( void );

protected:
	void						closeSndFile( void );

	//=== vars ===//
	P2PEngine&					m_Engine; 
	MediaProcessor&				m_MediaProcessor;	

	EPluginType					m_EPluginType{ ePluginTypeSndReader };
	bool						m_IsPlaying{ false };
	bool						m_IsPlayingPaused{ false };
	std::string					m_FileName;
	VxGUID						m_AssetId;

	FILE*						m_FileHandle{ nullptr };
	VxTimer						m_PlayElapseTimer;
	double						m_TotalElapsedMs{ 0 };
	bool						m_IsFirstFrameAfterResumePlaying{ false };

	VxMutex						m_RecMutex;
	OpusAudioDecoder&			m_OpusDecoder;
	OpusFileDecoder&			m_OpusFileDecoder;
};
