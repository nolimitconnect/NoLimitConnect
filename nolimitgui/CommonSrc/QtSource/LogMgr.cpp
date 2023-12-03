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
LogMgr::LogMgr( QWidget* parent )
    : QWidget( parent )
    , m_MyApp( GetAppInstance() )
{
    m_VerboseLog = m_MyApp.getAppSettings().getVerboseLog();

    connect( this, SIGNAL( signalLogMsg( uint32_t, const QString& ) ), this, SLOT( slotLogMsg( uint32_t, const QString& ) ) );
}

//============================================================================
LogMgr::~LogMgr()
{
    VxRemoveLogHandler( this );
}

//============================================================================
void LogMgr::startupLogMgr( void )
{
    VxAddLogHandler( this );
    setLogLevels( m_MyApp.getAppSettings().getLogLevels() );
    setLogModules( m_MyApp.getAppSettings().getLogModules() );
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
    VxSetLogFlags( logLevelFlags );
}

//============================================================================
uint32_t LogMgr::getLogLevels( void )
{
    return m_MyApp.getAppSettings().getLogLevels();
}

//============================================================================
void LogMgr::setLogModules( uint32_t logModuleFlags )
{
    m_MyApp.getAppSettings().setLogModules( logModuleFlags );
    VxSetModuleLogFlags( logModuleFlags );
}

//============================================================================
uint32_t LogMgr::getLogModules( void )
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
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
        logStr.remove(QRegularExpression("[\\n\\r]"));
#else
        logStr.remove(QRegExp("[\\n\\r]"));
#endif // QT_VERSION > QT_VERSION_CHECK(6,0,0)
        emit signalLogMsg( u32LogFlags, logStr );
    }
}

//============================================================================
void LogMgr::slotLogMsg( uint32_t u32LogFlags, const QString& text )
{  
    addLogMsgToQue( u32LogFlags, text );
}

//============================================================================
void LogMgr::addLogMsgToQue( uint32_t u32LogFlags, const QString& logMsg )
{
    m_LogMutex.lock();
    m_LogQue.push_back( LogQueEntry( u32LogFlags, logMsg ) );
    if( m_LogQue.size() > m_LogQueMaxLines )
    {
        m_LogQue.pop_front();
    }

    m_LogMutex.unlock();
}
