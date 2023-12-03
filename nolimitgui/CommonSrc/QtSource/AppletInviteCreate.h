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
#include "ui_AppletInviteCreate.h"

class AppletInviteCreate : public AppletBase
{
    Q_OBJECT
public:
    AppletInviteCreate( AppCommon& app, QWidget* parent );
    virtual ~AppletInviteCreate();

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

signals:
    void                        signalInfoMsg( const QString& logMsg );

protected slots:
    void                        slotCopyInviteButtonClicked( void );
    void                        slotNetworkSettingsInfoButtonClicked( void );
    void                        slotSelectGroupHostButtonClicked( void );
    void                        slotGroupHostSelected( QString hostUrl );

    void                        slotUpdateInvite( void );

protected:
    QPlainTextEdit *            getInviteMessageEdit( void )    { return ui.m_InviteMessageTextEdit; }
    QPlainTextEdit*             getInviteTextEdit( void )       { return ui.m_InviteTextEdit; }
    void                        addInviteText( QString text );
    void                        updateUrls( void );
    bool                        populateNetSettingUrl( EHostType hostType, std::string& ptopUrl );

    Ui::AppletInviteCreateUi    ui;
};


