//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "LogMgr.h"

#include "AppSettings.h"
#include "AppCommon.h"

#include <QRegularExpression>
#include <QScrollBar>
#include <QClipboard>

//============================================================================
// get log manager singleton
LogMgr& GetLogMgrInstance()
{
    static LogMgr g_LogMgr;
    return g_LogMgr;
}

//============================================================================
LogMgr::LogMgr( QObject* parent )
    : QObject( parent )
    , m_MyApp( GetAppInstance() )
{
    m_VerboseLog = m_MyApp.getAppSettings().getVerboseLog();
}

//============================================================================
void LogMgr::startupLogMgr( void )
{
    VxAddLogHandler( this );
}

//============================================================================
void LogMgr::shutdownLogMgr( void )
{
    VxRemoveLogHandler( this );
}

//============================================================================
void LogMgr::setLogLevels( uint32_t logLevelFlags )
{
    m_MyApp.getAppSettings().setLogLevels( logLevelFlags );
    VxSetLogLevelFlags( logLevelFlags );
}

//============================================================================
uint32_t LogMgr::getLogLevels( void )
{
    return m_MyApp.getAppSettings().getLogLevels();
}

//============================================================================
void LogMgr::setLogModules( uint64_t logModuleFlags )
{
    m_MyApp.getAppSettings().setLogModules( logModuleFlags );
    VxSetModuleLogFlags( logModuleFlags );
}

//============================================================================
uint64_t LogMgr::getLogModules( void )
{
    return m_MyApp.getAppSettings().getLogModules();
}

//============================================================================
void LogMgr::onLogEvent( uint32_t u32LogFlags, const char* logMsg )
{
    if( m_VerboseLog
        || ( u32LogFlags & ~LOG_VERBOSE ) )
    {
        QString logStr( logMsg );

        logStr.remove(QRegularExpression("[\\n\\r]"));

        emit signalLogMsg( u32LogFlags, logStr );
    }
}
