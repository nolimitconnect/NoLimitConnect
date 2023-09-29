#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IAudioDefs.h>

#include <vector>
#include <algorithm>

class AppCommon;
class AudioIoMgr;
class AudioSampleBuf;
class IAudioCallbacks;
class MiniAudioMgr;

class AudioMixerFrame
{
public:
	AudioMixerFrame();

	void						setAudioIoMgr( MiniAudioMgr* audioIoMgr )	{ m_AudioIoMgr = audioIoMgr; }

	void						clearFrame( bool fillSilence = false );
	bool						isEmpty( void )								{ return m_InputIds.empty(); }

	void						setFrameIndex( int frameIndex )				{ m_FrameIndex = frameIndex; }
	int							getFrameIndex( void )						{ return m_FrameIndex; }

	bool						hasModuleAudio( EAppModule appModule )		{ return std::find( m_InputIds.begin(), m_InputIds.end(), appModule ) != m_InputIds.end(); }
	bool						hasAnyAudio( void )							{ return !m_InputIds.empty(); }

	int16_t*					getMixerBuf( void )							{ return m_MixerBuf; }

	int							audioSamplesInUse( void )					{ return m_MixerSamplesWrote; }
	int							audioSamplesFreeSpace()						{ return AUDIO_SAMPLES_PER_FRAME - m_MixerSamplesWrote; }

	// add audio data to mixer.. assumes pcm signed short mono channel.. return total written to buffer
	int							toMixerPcmMonoChannel( EAppModule appModule, int16_t* pcmData, bool isSilenceIn );

protected:
	MiniAudioMgr*				m_AudioIoMgr{ nullptr };
	int							m_FrameIndex{ 0 };
	int16_t						m_MixerBuf[ AUDIO_SAMPLES_PER_FRAME ];
	int							m_MixerSamplesWrote{ 0 };
	std::vector<EAppModule>		m_InputIds;
	bool						m_IsSilentSamples{ true };
};
