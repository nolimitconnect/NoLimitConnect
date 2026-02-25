//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MiniAudioInDevice.h"
#include "AudioMgr.h"

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QMessageBox>

namespace
{
    void MiniAudioInCallback( ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount )
    {
        if(VxIsAppShuttingDown())
        {
            return;
        }

        MiniAudioInDevice* miniAudioDevice = (MiniAudioInDevice*)pDevice->pUserData;
        vx_assert( miniAudioDevice != nullptr );

        miniAudioDevice->callbackAudioWrite( (int16_t*)pInput, frameCount );

        (void)pOutput;
    }
}

//============================================================================
MiniAudioInDevice::MiniAudioInDevice( AudioMgr& maMgr )
    : m_AudioIoMgr( maMgr )
{
    connect( this, SIGNAL(signalShowErrorFromThread(QString,QString)), this, SLOT(slotShowErrorFromThread(QString,QString)), Qt::QueuedConnection );
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
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::%s SUCCESS index %d rate %d", __func__, deviceIndex, preferredRate );
        retActualRate = preferredRate;
        m_AudioDeviceIndex = deviceIndex;
        return true;
    }
    else
    {
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::%s FAIL index %d rate %d.. will try 48000Hz", __func__, deviceIndex, preferredRate );
    }

    // most devices can at least run at 48000 .. this seems to be android's default
    if( preferredRate != 48000 && initalizeDevice( deviceIndex, 48000 ) )
    {
        retActualRate = 48000;
        m_AudioDeviceIndex = deviceIndex;
        LogMsg( LOG_VERBOSE, "MiniAudioInDevice::%s SUCCESS index %d rate %d", __func__, deviceIndex, retActualRate );
        return true;
    }

    LogMsg( LOG_ERROR, "MiniAudioInDevice::%s FAILED index %d", __func__, deviceIndex );
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

    m_MaDeviceConfig.noPreSilencedOutputBuffer = true;
    //m_MaDeviceConfig.performanceProfile = ma_performance_profile_conservative;
    m_MaDeviceConfig.periodSizeInMilliseconds = AUDIO_MS_PER_FRAME % 3 ? AUDIO_MS_PER_FRAME / 2 : AUDIO_MS_PER_FRAME / 3;

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

//============================================================================
void MiniAudioInDevice::slotShowErrorFromThread( QString title, QString body )
{
    QMessageBox::information( nullptr, title, body, QMessageBox::Ok );
}
