#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

#include "GuiUserJoinCallback.h"

#include <CoreLib/VxPtopUrl.h>

#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletHostJoinConnectUi;
}
QT_END_NAMESPACE

class GuiUserSessionBase;
class GuiUserListItem;
class QPlainTextEdit;

class AppletHostJoinConnect : public AppletBase, public GuiUserJoinCallback
{
	Q_OBJECT
public:
    AppletHostJoinConnect( AppCommon& app, QWidget* parent, std::string url );
	virtual ~AppletHostJoinConnect() override;

signals:
    void                        signalJoinedHost( HostedId hostId, bool isJoined );

protected:
    void				        callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinLeaveHost( GroupieId& groupieId ) override;
    void				        callbackGuiUserJoinRemoved( GroupieId& groupieId ) override;

    void				        callbackGuiUserJoinToHostState( EHostType hostType, bool isJoined ) override; ///< just my join to host state for me and not other members

    void				        callbackGuiUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus ) override;

    void                        logMsg( const char* logMsg, ... );
    QPlainTextEdit*             getInfoEdit( void );
    void                        joinHost( void );

    //=== vars ===//
    Ui::AppletHostJoinConnectUi&	ui;

    GroupieId                   m_AdminGroupieId;
    GroupieId                   m_MyGroupieId;

    std::string					m_HostUrl;
    VxPtopUrl					m_HostPtopUrl;
    VxGUID                      m_HostOnlineId;

    VxGUID                      m_JoinHostSessionId;

    EHostType                   m_HostType{ eHostTypeUnknown };
    EConnectStatus              m_ConnectStatus{ eConnectStatusUnknown };

    bool                        m_IsClosing{ false };

    
};


