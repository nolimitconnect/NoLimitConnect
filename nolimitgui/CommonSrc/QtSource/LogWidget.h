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

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <CoreLib/VxDebug.h>

#include <QPlainTextEdit>
#include <QFile>
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui {
    class LogWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;

class LogWidget : public QWidget, public ILogCallbackInterface
{
    Q_OBJECT
public:
    LogWidget( QWidget* parent );
    ~LogWidget() override;

    void                        initLogCallback();

    void                        onLogEvent( uint32_t u32LogFlags, const char* logMsg ) override;

    void                        clear()                 { getLogEdit()->clear(); }

signals:
    void                        signalLogMsg( const QString& logMsg );

protected slots:
    void                        slotLogMsg( const QString& text );
    void                        slotVerboseCheckBoxClicked( void );
    void                        slotCopyToClipboardClicked( void );

protected:
    QPlainTextEdit*             getLogEdit( void );

    Ui::LogWidgetClass&	        ui;
    AppCommon&                  m_MyApp;
    QFile                       m_LogFile;
    QMutex                      m_LogMutex;
};
