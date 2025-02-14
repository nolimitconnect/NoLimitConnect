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
    : QThread( widget )
    , m_MyApp( myApp )
    , m_MediaProcessor( GetPtoPEngine().getMediaProcessor() )
{
    // 
    setPriority(QThread::HighPriority);
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
            m_ElapsedTimer.start();
            start();
        }
        else
        {
            m_FrameSemaphore.signal();
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

    size_t vidQueSize = m_MediaProcessor.getVideoQueueSize();
    if( vidQueSize > 2 )
    {
        LogModule( eLogWebCam, LOG_WARN, "slotVideoFrameChanged media processor behind.. queue size is %d ", vidQueSize );
        return;
    }

    if( !m_FrameProcessed )
    {
        LogModule( eLogWebCam, LOG_WARN, "slotVideoFrameChanged prev frame not processed" );
        return;
    }

    lockFrameQueue();
    m_CamImage = frame.toImage();
    m_FrameProcessed = false;
    unlockFrameQueue();

    m_FrameSemaphore.signal();
}

//============================================================================
void VideoFrameProcessor::run( void )
{
    while( m_ProcessFramesEnabled )
    {
        m_FrameSemaphore.wait();
        if( !m_ProcessFramesEnabled || VxIsAppShuttingDown() )
        {
            return;
        }

        lockFrameQueue();
        processCapturedImage( m_CamImage );
        m_FrameProcessed = true;  
        unlockFrameQueue();
    }
}

//============================================================================
void VideoFrameProcessor::processCapturedImage( const QImage& imgIn )
{
    if( imgIn.isNull() )
    {
        LogModule( eLogWebCam, LOG_ERROR, "%s null image ", __func__ );
        return;
    }

    if( imgIn.format() == QImage::Format_RGB888 )
    {
        uint32_t imageLen = imgIn.bytesPerLine() * imgIn.height();
        m_MediaProcessor.fromGuiVideoData( FOURCC_RGB, (uint8_t *)imgIn.bits(), imgIn.width(), imgIn.height(), imageLen, m_MyApp.getCamCaptureRotation() );
    }
    else 
    {
        QImage toSendImage = imgIn.convertToFormat( QImage::Format_RGB888 );
        if( !toSendImage.isNull() )
        {
            uint32_t imageLen = toSendImage.bytesPerLine() * toSendImage.height();
            m_MediaProcessor.fromGuiVideoData( FOURCC_RGB, toSendImage.bits(), toSendImage.width(), toSendImage.height(), imageLen, m_MyApp.getCamCaptureRotation() );
        }
        else
        {
            LogModule( eLogWebCam, LOG_ERROR, "%s convert to Format_RGB888 failed", __func__ );
        }
    }
}
