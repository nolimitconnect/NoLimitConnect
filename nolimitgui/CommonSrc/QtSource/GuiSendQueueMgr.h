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

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <GuiInterface/IDefs.h>

#include <SendQueue/SendQueueCallback.h>

#include <CoreLib/VxMutex.h>
#include <PktLib/GroupieId.h>

#include <QObject>

#include <set>
#include <memory>

class AppCommon;
class GuiSendQueueCallback;

class GuiSendQueueMgr : public QObject, public SendQueueCallback
{
    Q_OBJECT
public:
    GuiSendQueueMgr();
    GuiSendQueueMgr( const GuiSendQueueMgr& rhs ) = delete;
	virtual ~GuiSendQueueMgr() = default;

    virtual void                onAppCommonCreated( void );

    void                        wantSendQueueCallback( GuiSendQueueCallback* client, bool enable );

    bool                        isInSendQueue( VxGUID& onlineId, VxGUID& assetId );

signals:
    void                        signalInternalSendQueue( SendQueInfo sendInfo );

private slots:
    void                        slotInternalSendQueue( SendQueInfo sendInfo );

protected:
    void                        callbackSendQueInfo( SendQueInfo& sendInfo ) override;

    void                        updateSendQueue( SendQueInfo& groupieId );

    void                        announceSendQueue( SendQueInfo& sendQueInfo );

    std::vector<SendQueInfo>    m_SendList;

    std::vector<GuiSendQueueCallback*> m_MemberClients;

};
