//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MiniAudioDevices.h"

#include <CoreLib/VxDebug.h>

//============================================================================
bool MiniAudioDevices::startupMiniAudio( void )
{
    if( ma_context_init( NULL, 0, NULL, &m_MaContext ) != MA_SUCCESS ) 
    {
        LogMsg( LOG_ERROR, "MiniAudioDevices::initMiniAudioContext Failed to initialize context." );
        return false;
    }

    ma_result result = ma_context_get_devices( &m_MaContext, &m_MaSpeakerDeviceInfos, &m_MaSpeakerDeviceCount, &m_MaMicDeviceInfos, &m_MaMicDeviceCount );
    if( result != MA_SUCCESS ) 
    {
        LogMsg( LOG_ERROR, "MiniAudioDevices::initMiniAudioContext Failed to retrieve device information." );
        return false;
    }

    for( ma_uint32 iDevice = 0; iDevice < m_MaSpeakerDeviceCount; ++iDevice )
    {
        LogMsg( LOG_VERBOSE, "MiniAudioDevices speaker index %u: %s", iDevice, m_MaSpeakerDeviceInfos[ iDevice ].name );
        m_SpeakerDeviceDescriptions.push_back( m_MaSpeakerDeviceInfos[ iDevice ].name );
    }

    for( ma_uint32 iDevice = 0; iDevice < m_MaMicDeviceCount; ++iDevice )
    {
        LogMsg( LOG_VERBOSE, "MiniAudioDevices Speaker index %u: %s", iDevice, m_MaMicDeviceInfos[ iDevice ].name );
        m_MicDeviceDescriptions.push_back( m_MaMicDeviceInfos[ iDevice ].name );
    }

    return !m_SpeakerDeviceDescriptions.empty();
}

//============================================================================
void MiniAudioDevices::shutdownMiniAudio( void )
{
    ma_context_uninit( &m_MaContext );
}

//============================================================================
std::string MiniAudioDevices::getAudioInDeviceDesc( int deviceIndex )
{
    if( deviceIndex >= 0 && deviceIndex < m_MicDeviceDescriptions.size() )
    {
        return m_MicDeviceDescriptions[ deviceIndex ];
    }
    else
    {
        return "Unknown sound input device";
    }
}

//============================================================================
std::string MiniAudioDevices::getAudioOutDeviceDesc( int deviceIndex )
{
    if( deviceIndex >= 0 && deviceIndex < m_SpeakerDeviceDescriptions.size() )
    {
        return m_SpeakerDeviceDescriptions[ deviceIndex ];
    }
    else
    {
        return "Unknown sound output device";
    }
}

//============================================================================
ma_device_id* MiniAudioDevices::getAudioInDeviceId( int deviceIdx )
{
    return &m_MaMicDeviceInfos[ deviceIdx ].id;
}

//============================================================================
ma_device_id* MiniAudioDevices::getAudioOutDeviceId( int deviceIdx )
{
    return &m_MaSpeakerDeviceInfos[ deviceIdx ].id;
}