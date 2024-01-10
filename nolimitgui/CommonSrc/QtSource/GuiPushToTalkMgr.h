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

#include "GuiPushToTalkCallback.h"

#include <QObject>

#include <CoreLib/VxGUID.h>

class GuiPushToTalkMgr : public QObject, public PushToTalkCallback
{
    Q_OBJECT
public:
    GuiPushToTalkMgr();
    virtual ~GuiPushToTalkMgr() = default;

    virtual void                onSystemReady( void );

    void                        wantGuiPushToTalkCallbacks( GuiPushToTalkCallback* clientInterface, bool wantCallbacks );  

    void                        setPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus );
    EPushToTalkStatus           getPushToTalkStatus( VxGUID& onlineId );

signals:
    void                        signalInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus );

protected slots:
    void                        slotInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus );

protected:
    virtual void				callbackPushToTalkStatus( VxGUID& onlineId, enum EPushToTalkStatus pushToTalkStatus );


    std::vector<GuiPushToTalkCallback*> m_PushToTalkClients;
    std::map<VxGUID, enum EPushToTalkStatus> m_PushToTalkStatusMap;
};
