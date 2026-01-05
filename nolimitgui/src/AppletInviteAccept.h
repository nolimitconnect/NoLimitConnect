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

#include <CoreLib/VxPtopUrl.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletInviteAcceptUi;
}
QT_END_NAMESPACE

class QPlainTextEdit;

class AppletInviteAccept : public AppletBase
{
    Q_OBJECT
public:
    AppletInviteAccept( AppCommon& app, QWidget* parent );
    virtual ~AppletInviteAccept();

protected slots:
    void                        slotPasteFromClipboard( QString clipboardText );
    void                        slotAcceptInviteButtonClicked( void );
    void                        slotRejectInviteButtonClicked( void );
    void                        slotInviteChanged( void );
    void                        slotConnectToHosts( void );
    void                        slotInviteInfoButtonClicked( void );

protected:
    QPlainTextEdit *            getInviteEdit( void );
    VxGUID                      getFromOnlineId( void );

    Ui::AppletInviteAcceptUi&   ui;
        
    std::string                 m_UserMsg;

    std::vector<VxPtopUrl>      m_HostUrls;
    std::vector<VxPtopUrl>      m_NetworkUrls;
    bool                        m_HasPrivateNetworkKey{ false };
    std::string                 m_PastedInviteText;
};


