//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CamFrameProcessor.h"

#include "AppCommon.h"
#include "CamLogic.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTime.h>


namespace
{
    // from fourcc.org
    #define FOURCC_RGB		0x32424752
};

//============================================================================
CamFrameProcessor::CamFrameProcessor( AppCommon& myApp, CamLogic& camLogic, QObject* parent )
    : QObject( parent )
    , m_MyApp( myApp )
    , m_CamLogic( camLogic )
{
}

//============================================================================
CamFrameProcessor::~CamFrameProcessor()
{
    enableProcessing( false );
}

//============================================================================
void CamFrameProcessor::enableProcessing( bool enable )
{
    if( m_ProcessFramesEnabled != enable )
    {
        m_ProcessFramesEnabled = enable;
        if( m_ProcessFramesEnabled )
        {
            m_DesiredFrameSize = GuiParams::getSnapshotDesiredSize();
        }
    }
}

//============================================================================
void CamFrameProcessor::slotVideoFrameChanged( const QVideoFrame& frame )
{
    static int64_t lastTimeMs = 0;
    int64_t timeNow = GetHighResolutionTimeMs();
    if( timeNow < lastTimeMs + CamLogic::CAM_SNAPSHOT_INTERVAL_MS )
    {
        //LogMsg( LOG_VERBOSE, "%s time ok %d ms", __func__, (int)(timeNow - lastTimeMs) );
        return;
    }
    
    lastTimeMs = timeNow;

    if( !m_ProcessFramesEnabled || !m_CamLogic.canProcessCamCapture() )
    {
        //if( !m_ProcessFramesEnabled )
        //{
        //    LogMsg( LOG_ERROR, "%s !m_ProcessFramesEnabled %d", __func__, GetHighResolutionTimeMs() );
        //}
        //else
        //{
        //    LogMsg( LOG_ERROR, "%s !m_CamLogic.canProcessCamCapture %d", __func__, GetApplicationAliveMs() );
        //}

        return;
    }

    // IMPORTANT: Explicitly map the frame to ensure the buffer is accessible to the CPU
    QVideoFrame cloneFrame(frame); 
    if (!cloneFrame.map(QVideoFrame::ReadOnly)) {
        LogMsg(LOG_ERROR, "%s Failed to map video frame", __func__);
        return;
    }

    QImage rgbImage = cloneFrame.toImage().convertToFormat( QImage::Format_RGB888 );
    // Always unmap after conversion
    cloneFrame.unmap();

    if( !rgbImage.isNull() )
    {
        int rowBytes = 3 * rgbImage.width();
        uint32_t dataLen = rowBytes * rgbImage.height();
        std::shared_ptr<uint8_t> rgbData( new uint8_t[dataLen] );
        if( rgbImage.bytesPerLine() == rowBytes )
        {
            memcpy( rgbData.get(), rgbImage.bits(), dataLen );
        }
        else
        {
            uint8_t* dest = rgbData.get();
            for( int row = 0; row < rgbImage.height(); ++row )
            {
                memcpy( dest, rgbImage.scanLine( row ), rowBytes );
                dest += rowBytes;
            }
        }

        // LogMsg( LOG_VERBOSE, "%s getCamProcessor().processCamCapture %d", __func__, GetApplicationAliveMs() );
        m_CamLogic.getCamProcessor().processCamCapture( rgbImage.width(), rgbImage.height(), rgbData, dataLen );
    }
    else
    {
        LogMsg( LOG_ERROR, "%s !rgbImage.isNull() %d", __func__, GetHighResolutionTimeMs() );
    }
}
