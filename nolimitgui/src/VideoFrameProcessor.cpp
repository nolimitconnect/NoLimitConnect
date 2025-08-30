//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VideoFrameProcessor.h"

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
VideoFrameProcessor::VideoFrameProcessor( AppCommon& myApp, CamLogic& camLogic, QObject* parent )
    : QObject( parent )
    , m_MyApp( myApp )
    , m_CamLogic( camLogic )
{
}

//============================================================================
VideoFrameProcessor::~VideoFrameProcessor()
{
    enableProcessing( false );
}

//============================================================================
void VideoFrameProcessor::enableProcessing( bool enable )
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
void VideoFrameProcessor::slotVideoFrameChanged( const QVideoFrame& frame )
{
    static int64_t lastTimeMs = 0;
    int64_t timeNow = GetGmtTimeMs();
    if( timeNow < lastTimeMs + CamLogic::CAM_SNAPSHOT_INTERVAL_MS )
    {
        //LogMsg( LOG_VERBOSE, "%s time ok %d ms", __func__, (int)(timeNow - lastTimeMs) );
        return;
    }
    
    if( !m_ProcessFramesEnabled || !m_CamLogic.canProcessCamCapture() )
    {
        //if( !m_ProcessFramesEnabled )
        //{
        //    LogMsg( LOG_ERROR, "%s !m_ProcessFramesEnabled %d", __func__, GetApplicationAliveMs() );
        //}
        //else
        //{
        //    LogMsg( LOG_ERROR, "%s !m_CamLogic.canProcessCamCapture %d", __func__, GetApplicationAliveMs() );
        //}

        return;
    }

    lastTimeMs = timeNow;
    QImage rgbImage = frame.toImage().convertToFormat( QImage::Format_RGB888 );
    if( !rgbImage.isNull() )
    {
        uint32_t dataLen = 3 * rgbImage.width() * rgbImage.height();
       
        std::shared_ptr<uint8_t> rgbData( new uint8_t[dataLen] );
        memcpy( rgbData.get(), rgbImage.bits(), dataLen);
        // LogMsg( LOG_VERBOSE, "%s getCamProcessor().processCamCapture %d", __func__, GetApplicationAliveMs() );
        m_CamLogic.getCamProcessor().processCamCapture( rgbImage.width(), rgbImage.height(), rgbData, dataLen );
    }
    else
    {
        LogMsg( LOG_ERROR, "%s !rgbImage.isNull() %d", __func__, GetApplicationAliveMs() );
    }
}
