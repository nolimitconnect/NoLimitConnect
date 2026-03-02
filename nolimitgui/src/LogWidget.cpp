//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "LogWidget.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include <QClipboard>
#include <QRegularExpression>
#include <QScrollBar>

#include "ui_LogWidget.h"

namespace
{
    const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
}

QPlainTextEdit* LogWidget::getLogEdit( void )  { return ui.m_LogPlainTextEdit;  }

//============================================================================
LogWidget::LogWidget( QWidget* parent )
    : QWidget( parent )
    , m_MyApp( GetAppInstance() )
    , ui(*(new Ui::LogWidgetUi))
{
    ui.setupUi( this );
    ui.m_VerboseLogCheckBox->setVisible( false );

    getLogEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
    getLogEdit()->setReadOnly( true );

    connect( ui.m_VerboseLogCheckBox, SIGNAL(clicked()), this, SLOT(slotVerboseCheckBoxClicked()) );
    connect( ui.m_ClearLogButton, SIGNAL(clicked()), this, SLOT(slotClearLogClicked()) );
    connect( ui.m_CopyToClipboardButton, SIGNAL(clicked()), this, SLOT(slotCopyToClipboardClicked()) );

    connect( this, SIGNAL(signalLogMsg(const QString&)), this, SLOT(slotLogMsg(const QString&)) );
}

//============================================================================
LogWidget::~LogWidget()
{
    VxRemoveLogHandler( this );
}

//============================================================================
void LogWidget::initLogCallback()
{
    VxAddLogHandler( this );
}

//============================================================================
void LogWidget::onLogEvent( uint32_t u32LogFlags, const char* logMsg )
{
    m_LogMutex.lock();
    QString logStr( logMsg );

    logStr.remove(QRegularExpression("[\\n\\r]"));

    emit signalLogMsg( logStr );

    m_LogMutex.unlock();
}

//============================================================================
void LogWidget::slotLogMsg( const QString& text )
{  
    getLogEdit()->appendPlainText( text ); // Adds the message to the widget
    getLogEdit()->verticalScrollBar()->setValue( getLogEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}

//============================================================================
void LogWidget::slotVerboseCheckBoxClicked( void )
{
    emit signalVerboseLogEnable( ui.m_VerboseLogCheckBox->isChecked() );
}

//============================================================================
void LogWidget::slotCopyToClipboardClicked( void )
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( getLogEdit()->toPlainText() );
}

//============================================================================
void LogWidget::slotClearLogClicked( void )
{
    getLogEdit()->clear();
}

