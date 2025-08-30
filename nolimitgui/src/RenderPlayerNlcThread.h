#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones 
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

#include <QThread>
#include <QOpenGLContext>
#include <QOffscreenSurface>

class RenderGlOffScreenSurface;
class QOpenGLFramebufferObject;
class RenderGlWidget;
class RenderGlLogic;
class AppCommon;

class RenderPlayerNlcThread : public QThread
{
    Q_OBJECT
public:
    RenderPlayerNlcThread() = delete;
    explicit RenderPlayerNlcThread( RenderGlLogic& renderLogic );
    virtual ~RenderPlayerNlcThread();

    void						setRenderThreadShouldRun(bool shouldRun) { m_ShouldRun = shouldRun; }

    bool                        isRenderThreadStarted() { return m_IsThreadStarted; }
    void                        startRenderThread();
    void                        stopRenderThread();

protected:
    virtual void                run() override;

    RenderGlLogic&				m_RenderLogic;
    bool						m_ShouldRun = true;
    bool                        m_IsThreadStarted = false;
    bool                        m_IsKodiRunning = false;
};
