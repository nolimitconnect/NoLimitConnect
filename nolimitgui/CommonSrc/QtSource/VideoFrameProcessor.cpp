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
VideoFrameProcessor::VideoFrameProcessor( AppCommon& myApp, QObject* widget )
    : QObject( widget )
    , m_MyApp( myApp )
    , m_MediaProcessor( GetPtoPEngine().getMediaProcessor() )
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
    if( !m_ProcessFramesEnabled || VxIsAppShuttingDown() )
    {
        return;
    }

    static int64_t lastTimeMs{0};
    int64_t timeNowMs = GetGmtTimeMs();
    if( timeNowMs - lastTimeMs < CAM_SNAPSHOT_INTERVAL_MS )
    {
        //LogModule( eLogWebCam, LOG_WARN, "%s skip frame at %lld ms", __func__, elapsedMs );
        return;
    }

    lastTimeMs = timeNowMs;

    if( m_MediaProcessor.getVideoQueueSize() > 2 )
    {
        LogModule( eLogWebCam, LOG_WARN, "%s media processor behind", __func__ );
        return;
    }

    if( !m_FrameProcessed )
    {
        LogModule( eLogWebCam, LOG_WARN, "%s prev frame not processed", __func__ );
        return;
    }

    QImage rgbImage = frame.toImage().convertToFormat( QImage::Format_RGB888 );
    if( !rgbImage.isNull() )
    {
        uint32_t dataLen = 3 * rgbImage.width() * rgbImage.height();
       
        std::shared_ptr<uint8_t> sharedData( new uint8_t[dataLen] );
        memcpy( sharedData.get(), rgbImage.bits(), dataLen);
        m_MediaProcessor.fromGuiVideoData( FOURCC_RGB, sharedData, rgbImage.width(), rgbImage.height(), dataLen, m_MyApp.getCamCaptureRotation() );
    }
}
