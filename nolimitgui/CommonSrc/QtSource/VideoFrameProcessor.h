#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
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

#include <QObject>
#include <QVideoFrame>

class AppCommon;
class MediaProcessor;

class VideoFrameProcessor : public QObject
{
    Q_OBJECT

public:  
    static const int64_t CAM_SNAPSHOT_INTERVAL_MS = 60;

    VideoFrameProcessor( AppCommon& myApp, QObject* widget );
    ~VideoFrameProcessor();

    void                        enableProcessing( bool enable );

protected slots:
    void                        slotVideoFrameChanged( const QVideoFrame& frame );

protected:

    AppCommon&                  m_MyApp;
    MediaProcessor&             m_MediaProcessor;
    QSize                       m_DesiredFrameSize;

    bool                        m_ProcessFramesEnabled{false};
    bool                        m_FrameProcessed{ true };
};
