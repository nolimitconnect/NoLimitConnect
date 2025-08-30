#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

#include <QMutex>
#include <QFile>

#include <CoreLib/VxDebug.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletTestUpnpUi;
}
QT_END_NAMESPACE

class QPlainTextEdit;

class AppletTestUpnp : public AppletBase, public ILogCallbackInterface
{
    Q_OBJECT
public:
    AppletTestUpnp( AppCommon& app, QWidget* parent );
    virtual ~AppletTestUpnp();

    void                        logMsg( const char* logMsg, ... );

    void                        onLogEvent( uint32_t u32LogFlags, const char* logMsg ) override;

signals:
    void                        signalLogMsg( const QString& logMsg );

protected slots:
    void                        slotLogMsg( const QString& text );

    void                        slotAddPortForward();
    void                        slotRemovePortForward();
    void                        slotListPortForward();
    void                        slotCopyLogToClipboard();
    
    void                        slotUpdateLogFlags();

protected:
    void                        clear();
    QPlainTextEdit *            getLogEdit( void );
    bool                        validateIpAndPort( void );

    QMutex                      m_LogMutex;
    LOG_FUNCTION                m_OldLogFunction{ nullptr };
    void *                      m_OldLogUserData{ nullptr};
    bool                        m_VerboseLog{ false };

    bool                        m_WasUpnpLogEnabled{ false };
    bool                        m_WasPortForwardEnabled{ false };

    uint32_t                    m_LogLevelFlags{ 0 };
    uint64_t                    m_LogModuleFlags{ 0 };

    bool                        m_IsIpv6{ false };
    std::string                 m_IpAddr;
    uint16_t                    m_Port{ 0 };

    Ui::AppletTestUpnpUi&       ui;
};


