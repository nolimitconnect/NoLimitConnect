//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CamProcessor.h"

#include "CamLogic.h"
#include "CamRgbVideo.h"

#if defined(USE_LIBJPEG_TURBO)
#include <libjpeg-turbo/VxJpgLib.h>
#else
#include <libjpg/VxJpgLib.h>
#endif // defined(USE_LIBJPEG_TURBO)

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxTimer.h>

#include <MediaProcessor/CamJpgVideo.h>

#include <VxVideoLib/VxRescaleRgb.h>

#include <stdlib.h>

namespace
{
	const int VIDEO_MAX_MOTION_VALUE				    = 100000;

	//============================================================================
    static void * CamProcessRgbThreadFunc( void * pvContext )
	{
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );
        CamProcessor * processor = (CamProcessor *)poThread->getThreadUserParam();
        if( processor && false == poThread->isAborted() )
        {
            processor->processCamRgbThreaded();
        }

		poThread->threadAboutToExit();
        return nullptr;
	}

    //============================================================================
    static void * CamProcessJpgThreadFunc( void * pvContext )
    {
        VxThread* poThread = (VxThread*)pvContext;
        poThread->setIsThreadRunning( true );
        CamProcessor * processor = (CamProcessor *)poThread->getThreadUserParam();
        if( processor && false == poThread->isAborted() )
        {
            processor->processCamJpgThreaded();
        }

        poThread->threadAboutToExit();
        return nullptr;
    }
};

//============================================================================
CamProcessor::CamProcessor( CamLogic& camLogic )
: m_CamLogic( camLogic )
{
    m_ProcessCamRgbThread.startThread( (VX_THREAD_FUNCTION_T)CamProcessRgbThreadFunc, this, "CamRgbProcessor" );
    m_ProcessCamJpgThread.startThread( (VX_THREAD_FUNCTION_T)CamProcessJpgThreadFunc, this, "CamJpgProcessor" );
}

//============================================================================
void CamProcessor::shutdownCamProcessor( void )
{
    m_ProcessCamRgbThread.abortThreadRun( true );
    m_CamRgbSemaphore.signal();
    m_ProcessCamJpgThread.abortThreadRun( true );
    m_CamJpgSemaphore.signal();
}

//============================================================================

//============================================================================
void CamProcessor::processCamCapture( int width, int height, std::shared_ptr<uint8_t>& rgbData, int dataLen )
{
    CamRgbVideo * rawVideo = new CamRgbVideo( rgbData, dataLen, width, height );

    m_CamRgbMutex.lock();
    m_ProcessCamRgbQue.emplace_back( rawVideo );
    m_CamRgbMutex.unlock();
    // LogMsg( LOG_WARN, "CamProcessor::%s signal %d", __func__, GetApplicationAliveMs() );
    m_CamRgbSemaphore.signal();
}

//============================================================================
void CamProcessor::processCamRgbThreaded( void )
{
    while( false == m_ProcessCamRgbThread.isAborted() )
	{
        m_CamRgbSemaphore.wait();

        //LogMsg( LOG_WARN, "CamProcessor::%s release %d", __func__, GetApplicationAliveMs() );
        if( m_ProcessCamRgbThread.isAborted() )
		{
            LogMsg( LOG_INFO, "CamProcessor::%s aborting1", __func__ );
			break;
		}

        while( m_ProcessCamRgbQue.size() )
		{
            int64_t startTime;
            //if( LogEnabled( eLogWebCam ) )
            {
               startTime = GetGmtTimeMs();
            }

            m_CamRgbMutex.lock();
            CamRgbVideo * rawVideo = m_ProcessCamRgbQue[0];
            m_ProcessCamRgbQue.erase( m_ProcessCamRgbQue.begin() );
            m_CamRgbMutex.unlock();

            processCamVideoRgb( rawVideo );
			delete rawVideo;
            //if( LogEnabled( eLogWebCam ) )
            {
                static int frameCnt = 0;
                frameCnt++;
                if( frameCnt >= 100 )
                {
                    frameCnt = 0;
                    int64_t endTime = GetGmtTimeMs();
                    LogMsg( LOG_VERBOSE, "CamProcessor::%s took %d ms at %d", __func__,
                           (int)(endTime - startTime), GetApplicationAliveMs() );
                }
            }

            if( m_ProcessCamRgbThread.isAborted() )
			{
                LogMsg( LOG_INFO, "CamProcessor::%s aborting2", __func__ );
				break;
			}
		}

        if( m_ProcessCamRgbThread.isAborted() )
		{
            LogMsg( LOG_INFO, "CamProcessor::%s aborting3", __func__ );
			break;
		}
	}

    LogMsg( LOG_INFO, "CamProcessor::%s leaving function", __func__ );
}

//============================================================================
int CamProcessor::calculateImageMotion( std::shared_ptr<uint8_t> rgbData, int dataLen )
{
    if( dataLen != m_LastRgbDataLen || m_LastRgbData.empty() )
    {
        m_LastRgbData.emplace_back( rgbData );
        m_LastRgbDataLen = dataLen;
        return 0;
    }

    std::shared_ptr<uint8_t> lastMotion = m_LastRgbData[0];
    m_LastRgbData.clear();
    const uint8_t* motionBuf = lastMotion.get();
    const uint8_t* cmpData = rgbData.get();
    unsigned int vidDiff = 0;
    for( int i = 0; i < dataLen; i++ )
    {
        vidDiff += cmpData[i] > motionBuf[i] ? ( cmpData[i] - motionBuf[i] ) : ( motionBuf[i] - cmpData[i] );
    }

    double videoSensitivity = (dataLen*64);
    double difNormalized = ((double)vidDiff * VIDEO_MAX_MOTION_VALUE) / videoSensitivity;
    int motion0To100000 = difNormalized > VIDEO_MAX_MOTION_VALUE ? VIDEO_MAX_MOTION_VALUE : (int)difNormalized;
    m_LastRgbData.emplace_back( rgbData );
    m_LastRgbDataLen = dataLen;
    return motion0To100000;
}

//============================================================================
void CamProcessor::processCamVideoRgb( CamRgbVideo* rgbVideo )
{
    //LogMsg( LOG_WARN, "CamProcessor::%s start %d", __func__, GetApplicationAliveMs() );
    int motion = calculateImageMotion( rgbVideo->m_VidData, rgbVideo->m_VidDataLen );

    uint8_t* pu8VidData	= rgbVideo->m_VidData.get();
    int width			= rgbVideo->m_Width;
    int height			= rgbVideo->m_Height;
    int rotation		= m_CamLogic.getCamCaptureRotation();

	bool needToDeleteVidData{ false };
    bool bResize = (( 320 != width ) || ( 240 != height ));
	if( bResize )
	{
		// scale to 320x200.. the reason we don't use the GdvBufferUtil to scale at same time as convert is that
		// it looks like crap. here we use averaging
		pu8VidData = VxResizeRgbImage(	pu8VidData, 
                                        width,
                                        height,
			                            320, 
			                            240,
                                        rotation );

		needToDeleteVidData = true;
        width = 320;
        height = 240;
	}
    else if( 0 != rotation )
	{
		// need to rotate
		pu8VidData = VxResizeRgbImage(	pu8VidData, 
                                        width,
                                        height,
			                            320, 
			                            240,
                                        rotation );
		needToDeleteVidData = true;
	}

	// compress from rgb to jpg for sending
	long s32JpgDataLen = 0;
	// take a guess at what size the jpg will be
    int iMaxJpgSize = width * height * 3;
    // seems like a waste of memory but we have no way of knowing jpg size before converting and memcpy after creation is cpu drain
    std::shared_ptr<uint8_t> jpgData( new uint8_t[iMaxJpgSize] );

    int32_t rc = VxBmp2Jpg(	 24,							// number of bits each pixel..(For now must be 24)
						     pu8VidData,					// bits of bmp to convert
                             width,						// width of image in pixels
                             height,						// height of image in pixels
                             JPG_CONVERT_QUALITY,			// quality of image
						     iMaxJpgSize,					// maximum length of pu8RetJpg
                             jpgData.get(),                 // buffer to return Jpeg image
						     &s32JpgDataLen );				// return length of jpeg image
	#if defined( LOG_JPG_SIZE )
        LogMsg( LOG_VERBOSE, "VxBmp2Jpg processCamVideoIn size %d", s32JpgDataLen );
	#endif //defined( LOG_JPG_SIZE )
    if( needToDeleteVidData )
    {
        delete[] pu8VidData;
    }

	if( 0 == rc )
	{
        // LogMsg( LOG_WARN, "CamProcessor::%s m_CamLogic.processCamCapture %d", __func__, GetApplicationAliveMs() );
        std::shared_ptr<CamJpgVideo> jpgVideo( new CamJpgVideo( jpgData, s32JpgDataLen, motion ) );
        m_CamJpgMutex.lock();
        m_ProcessCamJpgQue.emplace_back( jpgVideo );
        m_CamJpgMutex.unlock();
        m_CamJpgSemaphore.signal();
	}
	else
	{
        LogMsg( LOG_ERROR, "CamProcessor::%s JPEG Conversion error %d", __func__, rc );
	}
}


//============================================================================
void CamProcessor::processCamJpgThreaded( void )
{
    while( false == m_ProcessCamJpgThread.isAborted() )
    {
        m_CamJpgSemaphore.wait();

        //LogMsg( LOG_WARN, "CamProcessor::%s release %d", __func__, GetApplicationAliveMs() );
        if( m_ProcessCamJpgThread.isAborted() )
        {
            LogMsg( LOG_INFO, "CamProcessor::%s aborting1", __func__ );
            break;
        }

        while( m_ProcessCamJpgQue.size() )
        {
            m_CamJpgMutex.lock();
            std::shared_ptr<CamJpgVideo> jpgVideo = m_ProcessCamJpgQue[0];
            m_ProcessCamJpgQue.erase( m_ProcessCamJpgQue.begin() );
            m_CamJpgMutex.unlock();

            m_CamLogic.processCamCapture( jpgVideo );

            if( m_ProcessCamJpgThread.isAborted() )
            {
                LogMsg( LOG_INFO, "CamProcessor::%s aborting2", __func__ );
                break;
            }
        }

        if( m_ProcessCamJpgThread.isAborted() )
        {
            LogMsg( LOG_INFO, "CamProcessor::%s aborting3", __func__ );
            break;
        }
    }

    LogMsg( LOG_INFO, "CamProcessor::%s leaving function", __func__ );
}
