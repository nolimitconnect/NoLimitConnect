#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
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

#include <vector>
#include <string>

#include <libminiaudio/miniaudio.h>

class MiniAudioDevices
{
public:
	MiniAudioDevices() = default;
	~MiniAudioDevices() = default;

	bool						startupMiniAudio( void );
	void						shutdownMiniAudio( void );

	bool                        isMicrophoneDeviceAvailable( void )	{ return m_MaMicDeviceCount != 0; }
	bool                        isSpeakerDeviceAvailable( void )	{ return m_MaSpeakerDeviceCount != 0; }

	std::vector<std::string>&	getAudioInDevices( void )			{ return m_MicDeviceDescriptions; }
	std::vector<std::string>&   getAudioOutDevices( void )			{ return m_SpeakerDeviceDescriptions; }

	int							getAudioInDeviceCount( void )		{ return (int)m_MicDeviceDescriptions.size(); }
	int							getAudioOutDeviceCount( void )		{ return (int)m_SpeakerDeviceDescriptions.size(); }

	std::string					getAudioInDeviceDesc( int deviceIndex );
	std::string					getAudioOutDeviceDesc( int deviceIndex );

	ma_device_id*				getAudioInDeviceId( int deviceIdx );
	ma_device_id*				getAudioOutDeviceId( int deviceIdx );

protected:
	ma_context					m_MaContext;
	ma_device_info*				m_MaSpeakerDeviceInfos{ nullptr };
	ma_uint32					m_MaSpeakerDeviceCount{ 0 };
	ma_device_info*				m_MaMicDeviceInfos{ nullptr };
	ma_uint32					m_MaMicDeviceCount{ 0 };

	std::vector<std::string>	m_SpeakerDeviceDescriptions;
	std::vector<std::string>	m_MicDeviceDescriptions;
};

