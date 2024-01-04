//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiMainLoaderThread.h"

#include <AppInterface/INlc.h>

#include <CoreLib/VxDebug.h>

//============================================================================
GuiMainLoaderThread::GuiMainLoaderThread( QObject* parent )
    : QThread(parent)
{
    m_ElapsedTimer.start();
}

//============================================================================
GuiMainLoaderThread::~GuiMainLoaderThread()
{

}

//============================================================================
void GuiMainLoaderThread::run()
{
    LogMsg( LOG_VERBOSE, "GuiMainLoaderThread::run start in %" PRId64 " ms", m_ElapsedTimer.elapsed() );
    INlc& nolimit = INlc::getINlc();
    nolimit.doPreStartup();
    LogMsg( LOG_VERBOSE, "GuiMainLoaderThread::run doPreStartup complete in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    GetPtoPEngine(); // engine first.. there is some interdependencies
    LogMsg( LOG_VERBOSE, "GuiMainLoaderThread::run GetPtoPEngine complete in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    setIsLoadComplete( true );
}

