#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QObject>

#include "GuiPushToTalkCallback.h"
#include "GuiUserUpdateCallback.h"

#include <CoreLib/VxGUID.h>

class GuiUser;

class GuiPushToTalkMgr : public QObject, public PushToTalkCallback, public GuiUserUpdateCallback
{
    Q_OBJECT
public:
    GuiPushToTalkMgr();
    virtual ~GuiPushToTalkMgr() = default;

    virtual void                onSystemReady( void );

    void                        wantGuiPushToTalkCallbacks( GuiPushToTalkCallback* clientInterface, bool wantCallbacks );  

    void                        setPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus );
    EPushToTalkStatus           getPushToTalkStatus( VxGUID& onlineId );
    void                        togglePushToTalk( VxGUID& onlineId );

signals:
    void                        signalInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus );

protected slots:
    void                        slotInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus );

protected:
    void				        callbackPushToTalkStatus( VxGUID& onlineId, enum EPushToTalkStatus pushToTalkStatus ) override;
    void				        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;
    void                        setUserOffline( VxGUID& onlineId );

    std::vector<GuiPushToTalkCallback*> m_PushToTalkClients;
    std::map<VxGUID, enum EPushToTalkStatus> m_PushToTalkStatusMap;
};
