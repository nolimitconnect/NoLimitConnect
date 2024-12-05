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

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <QVideoFrame>
#include <QElapsedTimer>
#include <QVideoSink>

#include <CoreLib/VxMutex.h>

class VideoSinkGrabber : public QVideoSink
{
    Q_OBJECT

public:
    VideoSinkGrabber( QObject* widget );
};
