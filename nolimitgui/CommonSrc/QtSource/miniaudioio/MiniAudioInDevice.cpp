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

#include "MiniAudioInDevice.h"
#include "MiniAudioMgr.h"

#include <CoreLib/VxDebug.h>

namespace
{
    void MiniAudioInCallback( ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount )
    {
        MiniAudioInDevice* miniAudioDevice = (MiniAudioInDevice*)pDevice->pUserData;
        vx_assert( miniAudioDevice != nullptr );

        miniAudioDevice->callbackAudioWrite( (int16_t*)pInput, frameCount );

        (void)pOutput;
    }
}

//============================================================================
MiniAudioInDevice::MiniAudioInDevice( MiniAudioMgr& maMgr )
    : m_AudioIoMgr( maMgr )
{
}

//============================================================================
bool MiniAudioInDevice::initializeAudioInDevice( int& deviceIndex, int preferredRate, int& retActualRate )
{
    if( m_DeviceActive )
    {
        stopAudioInDevice();
    }

    retActualRate = 0;
    // first try with preferredRate
    if( initalizeDevice( deviceIndex, preferredRate ) )
    {
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::initializeAudioInDevice SUCCESS index %d rate %d", deviceIndex, preferredRate );
        retActualRate = preferredRate;
        m_AudioDeviceIndex = deviceIndex;
        return true;
    }
    else
    {
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::initializeAudioInDevice FAIL index %d rate %d", deviceIndex, preferredRate );
    }

    // most devices can at least run at 48000 .. this seems to be android's default
    if( preferredRate != 48000 && initalizeDevice( deviceIndex, 48000 ) )
    {
        retActualRate = 48000;
        m_AudioDeviceIndex = deviceIndex;
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::initializeAudioInDevice SUCCESS index %d rate %d", deviceIndex, retActualRate );
        return true;
    }

    LogMsg( LOG_ERROR, "MiniAudioInDevice::initializeAudioInDevice FAILED index %d", deviceIndex );
    return false;
}

//============================================================================
bool MiniAudioInDevice::startAudioInDevice( void )
{
    if( !m_DeviceActive )
    {
        ma_result result = ma_device_start( &m_MaDevice );
        if( result != MA_SUCCESS )
        {
            ma_device_uninit( &m_MaDevice );
            LogMsg( LOG_VERBOSE, "MiniAudioInDevice::startAudioIn Failed to start device." );
            m_DeviceAvailable = false;
        }
        else
        {
            m_DeviceActive = true;
        }
    }

    return m_DeviceActive;
}

//============================================================================
void MiniAudioInDevice::stopAudioInDevice( void )
{
    if( m_DeviceActive )
    {
        m_DeviceActive = false;
        ma_device_stop( &m_MaDevice );
    }
}

//============================================================================
bool MiniAudioInDevice::initalizeDevice( int deviceIndex, int sampleRate )
{
    if( m_DeviceActive )
    {
        stopAudioInDevice();
    }

    if( m_DeviceAvailable )
    {
        m_DeviceActive = false;
        m_DeviceAvailable = false;
        ma_device_uninit( &m_MaDevice );
    }
    
    m_MaDeviceConfig = ma_device_config_init( ma_device_type_capture );
    m_MaDeviceConfig.capture.pDeviceID = m_AudioIoMgr.getAudioInDeviceId( deviceIndex );
    m_MaDeviceConfig.capture.format = ma_format_s16;
    m_MaDeviceConfig.capture.channels = 1;
    //m_MaDeviceConfig.capture.shareMode = ma_share_mode_shared;
    m_MaDeviceConfig.sampleRate = sampleRate;
    m_MaDeviceConfig.dataCallback = MiniAudioInCallback;
    m_MaDeviceConfig.pUserData = this;

    // m_MaDeviceConfig.periodSizeInMilliseconds = AUDIO_MS_PER_FRAME;

    ma_result result = ma_device_init( NULL, &m_MaDeviceConfig, &m_MaDevice );
    if( result != MA_SUCCESS ) 
    {
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::startAudioIn Failed to initialize capture device." );
        m_DeviceAvailable = false;
        return false;
    }

    // start and stop device just to make sure it works
    result = ma_device_start( &m_MaDevice );
    if( result != MA_SUCCESS ) 
    {
        ma_device_uninit( &m_MaDevice );
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::startAudioIn Failed to start device." );
        m_DeviceAvailable = false;
        return false;
    }

    ma_device_stop( &m_MaDevice );

    m_DeviceAvailable = true;
    return true;
}

