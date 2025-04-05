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

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletInviteCreateUi;
}
QT_END_NAMESPACE

class QPlainTextEdit;

class AppletInviteCreate : public AppletBase
{
    Q_OBJECT
public:
    AppletInviteCreate( AppCommon& app, QWidget* parent );
    virtual ~AppletInviteCreate();

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

    bool                        setInviteType( EHostType hostType );

signals:
    void                        signalInfoMsg( const QString& logMsg );

protected slots:
    void                        slotCopyInviteButtonClicked( void );
    void                        slotNetworkSettingsInfoButtonClicked( void );
    void                        slotSelectGroupHostButtonClicked( void );
    void                        slotGroupHostSelected( QString hostUrl );

    void                        slotUpdateInvite( void );

protected:
    QPlainTextEdit *            getInviteMessageEdit( void );
    QPlainTextEdit*             getInviteTextEdit( void );
    void                        addInviteText( QString text );
    void                        updateUrls( void );
    bool                        populateNetSettingUrl( EHostType hostType, std::string& ptopUrl );

    Ui::AppletInviteCreateUi&   ui;
    EHostType                   m_HostType{ eHostTypePeerUser };
};


