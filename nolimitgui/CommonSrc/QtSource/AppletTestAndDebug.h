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
#include "ui_AppletTestAndDebug.h"
#include <GuiInterface/IToGui.h>

#include <CoreLib/VxDebug.h> // needed for ILogCallbackInterface
#include <CoreLib/VxCrypto.h>

#include <QMutex>
#include <QFile>

class VxUrl;

class AppletTestAndDebug : public AppletBase, public ILogCallbackInterface
{
    Q_OBJECT
public:
    AppletTestAndDebug( AppCommon& app, QWidget* parent );
    virtual ~AppletTestAndDebug();

    void                        logMsg( const char* logMsg, ... );
    void                        infoMsg( const char* infoMsg, ... );

    void                        onLogEvent( uint32_t u32LogFlags, const char* logMsg ) override;
    void                        toGuiInfoMsg( char * logMsg );

signals:
    void                        signalLogMsg( const QString& logMsg );
    void                        signalInfoMsg( const QString& logMsg );

protected slots:

    void                        slotShowLogButtonClick( void );
    void                        slotShowAppInfoButtonClick( void );
    void                        slotLogMsg( const QString& text );
    void                        slotInfoMsg( const QString& text );

    void                        slotCopyMyUrlIpv4ToClipboardClicked( void );
    void                        slotCopyMyUrlIpv6ToClipboardClicked( void );

    void                        slotCopyTestUrlToClipboardClicked( void );
    void                        slotCopyResultToClipboardClicked( void );
    void                        slotClearResultsButtonClicked( void );

    void                        slotBrowseFilesButtonClicked( void );
    void                        slotBrowseDownloadsButtonClicked( void );
    void                        slotBrowseBrowseAppDataButtonClicked( void );

    void                        slotPingTestButtonClicked( void );
    void                        slotSktStatViewButtonClicked( void );
    void                        slotHackerViewButtonClicked( void );
    void                        slotQueryHostIdButtonClicked( void );
    void                        slotGenerateGuidButtonClicked( void );
    void                        slotGenKeyButtonClicked( void );
    void                        slotPurgeCacheButtonClicked( void );
    void                        slotListActionButtonClicked( void );
    void                        slotHostClientTestButtonClicked( void );

    void                        slotPurgeEverythingButtonClicked( void );

    void                        slotNewUrlSelected( int comboBoxIdx );

    void                        slotLoopBackAllowStateChange( int checkBoxState );

protected:
    void						updateDlgFromSettings( void );
    void						updateSettingsFromDlg( void );
    QPlainTextEdit *            getInfoEdit( void )     { return ui.m_InfoPlainTextEdit; }
    void                        fillBasicInfo( void );
    void                        fillExtraInfo( void );
    void                        fillCpuInfo( void );

    void                        startUrlTest( ENetCmdType netCmdType );

    QFile                       m_LogFile;
    QMutex                      m_LogMutex;
    LOG_FUNCTION                m_OldLogFunction{ nullptr };
    void *                      m_OldLogUserData{ nullptr};
    bool                        m_VerboseLog{ false };
    VxGUID                      m_SessionId;
#if defined(DEBUG)
    bool                        m_ShowListMsg{ false }; // if debugging it is assumed user does not need instruction message
#else
    bool                        m_ShowListMsg{ true }; 
#endif // defined(DEBUG)

    Ui::AppletTestAndDebugUi    ui;
};


