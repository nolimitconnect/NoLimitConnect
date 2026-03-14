#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class CamLogic;
class CamRgbVideo;
class CamJpgVideo;

class CamProcessor
{
public:
	const int JPG_CONVERT_QUALITY = 75; // there is very little picture quality improvement above 75 

    CamProcessor( CamLogic& camLogic );
    virtual ~CamProcessor();
	
    void						shutdownCamProcessor( void );

    void                        processCamCapture( int width, int height, std::shared_ptr<uint8_t>& rgbData, int dataLen );

    bool                        isStalled( void ) { return m_ProcessCamRgbQue.size() > 1; }
    size_t                      getRgbQueueSize( void ) { return m_ProcessCamRgbQue.size(); }
    size_t                      getJpgQueueSize( void ) { return 0; }

    void						processCamRgbThreaded( void );

protected:
    void                        processCamVideoRgb( CamRgbVideo* rgbVideo );
    int                         calculateImageMotion( std::shared_ptr<uint8_t> newRgbData, int dataLen );

	//=== vars ===//
    CamLogic&					m_CamLogic;
	
    std::atomic<bool>			m_Abort{ false };
    std::mutex					m_CamRgbMutex;
    std::condition_variable		m_CamRgbCondVar;
    std::thread					m_ProcessCamRgbThread;
    std::queue<CamRgbVideo*>	m_ProcessCamRgbQue;
    std::shared_ptr<uint8_t>    m_LastRgbData;
    int                         m_LastRgbDataLen = 0;

};
