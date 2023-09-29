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

#include <libminiaudio/miniaudio.h>

#include <inttypes.h>

#include <QObject>

class MiniAudioMgr;
class MiniAudioInDevice : public QObject
{
	Q_OBJECT
public:
	MiniAudioInDevice( MiniAudioMgr& maMgr );
	~MiniAudioInDevice() = default;

	virtual bool				initializeAudioInDevice( int& deviceIndex, int preferredRate, int& retActualRate );

	virtual bool				startAudioInDevice( void );
	virtual void				stopAudioInDevice( void );

	virtual int					callbackAudioWrite( int16_t* pcmData, int lenBytes ) = 0;

protected:
	bool						initalizeDevice( int deviceIndex, int sampleRate );

	MiniAudioMgr&				m_AudioIoMgr;

	ma_context					m_MaContext;
	ma_device_info*				m_MaMicDeviceInfos{ nullptr };
	ma_uint32					m_MaMicDeviceCount{ 0 };
	ma_device_config			m_MaDeviceConfig;
	ma_device					m_MaDevice;

	bool						m_DeviceAvailable{ false };
	bool						m_DeviceActive{ false };
	int                         m_AudioDeviceIndex{ 0 };
};

