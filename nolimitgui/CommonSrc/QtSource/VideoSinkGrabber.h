#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <QVideoFrame>
#include <QElapsedTimer>
#include <QVideoSink>

#include <CoreLib/VxMutex.h>

class VideoSinkGrabber : public QVideoSink
{
    Q_OBJECT

public:
    VideoSinkGrabber( QObject* widget );

    void                        setFps( int fps );
    void                        enableGrab( bool enable );
    void                        lockGrabberQueue() { m_GrabberQueueMutex.lock(); }
    void                        unlockGrabberQueue() { m_GrabberQueueMutex.unlock(); }

    std::list<std::pair<QImage, int>> m_availFrames;

protected slots:
    void                        slotVideoFrameChanged( const QVideoFrame& frame );

protected:
    int                         m_Fps{ 15 };
    int64_t                     m_MinFrameIntervalMs{ 1000 / 15 };
    bool                        m_GrabEnabled{ true };
    QElapsedTimer               m_ElapsedTimer;
    VxMutex                     m_GrabberQueueMutex;
    QSize                       m_DesiredFrameSize;
};
