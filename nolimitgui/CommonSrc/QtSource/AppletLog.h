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
    class AppletLogUi;
}
QT_END_NAMESPACE

class QPlainTextEdit;

class AppletLog : public AppletBase, public ILogCallbackInterface
{
    Q_OBJECT
public:
    AppletLog( AppCommon& app, QWidget* parent );
    virtual ~AppletLog();

    void                        logMsg( const char* logMsg, ... );

    void                        onLogEvent( uint32_t u32LogFlags, const char* logMsg ) override;

signals:
    void                        signalLogMsg( const QString& logMsg );

protected slots:
    void                        slotLogSettingButtonClick( void );
    void                        slotLogMsg( const QString& text );

    void                        slotCopyToClipboardClicked( void );

protected:
    void						setupApplet( void );
    void                        clear();
    QPlainTextEdit *            getLogEdit( void );
    void                        fillBasicInfo( void );

    QFile                       m_LogFile;
    QMutex                      m_LogMutex;
    LOG_FUNCTION                m_OldLogFunction{ nullptr };
    void *                      m_OldLogUserData{ nullptr};
    bool                        m_VerboseLog{ false };

    Ui::AppletLogUi&            ui;
};


