//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <QWidget>

#include <CoreLib/VxDebug.h>

#include <QMutex>
#include <QString>
#include <QVector>

class AppCommon;

class LogQueEntry
{
public:
    LogQueEntry() = default;
    LogQueEntry( uint32_t u32LogFlags, const QString& text )
        : m_LogLevelFlags( u32LogFlags )
        , m_LogMsg( text )
    {
    }

    void                        setLogFlags( uint32_t u32LogFlags ) { m_LogLevelFlags = u32LogFlags; }
    uint32_t                    getLogFlags( void )                 { return m_LogLevelFlags; }
    void                        setLogText( const QString& text )   { m_LogMsg = text; }
    QString&                    getLogText( void )                  { return m_LogMsg; }
    
    uint32_t                    m_LogLevelFlags{0};
    QString                     m_LogMsg;
};

class LogMgr : public QObject, public ILogCallbackInterface
{
    Q_OBJECT
public:
    LogMgr( QObject* parent = nullptr );
    virtual ~LogMgr() override = default;

    void                        startupLogMgr( void );
    void                        shutdownLogMgr( void );

    void                        setLogLevels( uint32_t logLevelFlags );
    uint32_t                    getLogLevels( void );

    void                        setLogModules( uint64_t logModuleFlags );
    uint64_t                    getLogModules( void );

    void                        onLogEvent( uint32_t u32LogFlags, const char* logMsg ) override;

signals:
    void                        signalLogMsg( uint32_t u32LogFlags, const QString& logMsg );

protected:
    AppCommon&                  m_MyApp;
    bool                        m_VerboseLog = false;
};

// get log manager singleton
LogMgr& GetLogMgrInstance();