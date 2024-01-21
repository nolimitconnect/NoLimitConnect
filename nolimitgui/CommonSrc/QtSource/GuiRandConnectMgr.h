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

#include <RandConnect/RandConnectCallback.h>

#include <CoreLib/VxMutex.h>
#include <PktLib/GroupieId.h>

#include <QObject>

#include <set>

class AppCommon;
class GuiRandConnectCallback;

class GuiRandConnectMgr : public QObject, public RandConnectCallback
{
    Q_OBJECT
public:
    GuiRandConnectMgr();
    GuiRandConnectMgr( const GuiRandConnectMgr& rhs ) = delete;
	virtual ~GuiRandConnectMgr() = default;

    virtual void                onAppCommonCreated( void );

    void                        wantRandConnectCallback( GuiRandConnectCallback* client, bool enable );

    enum ERandAction            getRandAction( VxGUID& onlineId );

signals:
    void                        signalInternalRandConnect( GroupieId groupieId, enum ERandAction randAction );

private slots:
    void                        slotInternalRandConnect( GroupieId groupieId, enum ERandAction randAction );

protected:
    void                        callbackRandConnect( GroupieId& groupieId, enum ERandAction randAction ) override;

    void                        updateRandConnect( GroupieId& groupieId, enum ERandAction randAction );

    void                        announceRandConnect( VxGUID& onlineId, enum ERandAction randAction );

    std::vector<std::pair<GroupieId, enum ERandAction>>      m_MemberList;

    std::vector<GuiRandConnectCallback*> m_MemberClients;

};
