#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
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

#include <ConnectIdListMgr/ConnectIdListCallback.h>

#include <CoreLib/ConnectId.h>

#include <set>
#include <map>

#include <QObject>

class AppCommon;
class GuiConnectIdListCallback;

class GuiConnectIdListMgr : public QObject, public ConnectIdListCallback
{
    Q_OBJECT
public:
    GuiConnectIdListMgr() = delete;
    GuiConnectIdListMgr( AppCommon& app );
    GuiConnectIdListMgr( const GuiConnectIdListMgr& rhs ) = delete;
	virtual ~GuiConnectIdListMgr() = default;
    virtual void                onAppCommonCreated( void );
    virtual void                onMessengerReady( bool ready ) { }
    virtual bool                isMessengerReady( void );
    virtual void                onSystemReady( bool ready ) { }

    void                        wantGuiConnectIdCallbacks( GuiConnectIdListCallback* callback, bool wantCallback );

    // callbackConnectionStatusChange should happen before callbackOnlineStatusChange when user disconnects from host
    virtual void				callbackConnectionStatusChange( ConnectId& connectId, bool isConnected ) override;
    
    virtual void				callbackConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason, bool enableReason ) override;
    virtual void				callbackConnectionLost( VxGUID& sktConnectId ) override;

    bool                        isOnline( VxGUID& onlineId );
    bool                        isDirectConnect( VxGUID& onlineId );
    bool                        isRelayed( VxGUID& onlineId );
    bool                        isConnected( GroupieId& groupieId );

signals:
    void				        signalInternalConnectionStatusChange( ConnectId connectId, bool isConnected );
    void				        signalInternalConnectionReason( VxGUID sktConnectId, EConnectReason connectReason, bool enableReason );
    void				        signalInternalConnectionLost( VxGUID sktConnectId );

private slots:
    void				        slotInternalConnectionStatusChange( ConnectId connectId, bool isConnected );
    void				        slotInternalConnectionReason( VxGUID sktConnectId, EConnectReason connectReaso, bool enableReasonn );
    void				        slotInternalConnectionLost( VxGUID sktConnectId );

protected: 
    void                        onConnectionStatusChange( ConnectId& connectId, bool isConnected );

    void                        announceConnectionStatusChange( ConnectId& connectId, bool isConnected );

    void                        dumpOnlineUsers( void );

    AppCommon&                  m_MyApp;

    std::vector<GuiConnectIdListCallback*> m_GuiConnectIdClientList;

    std::set<ConnectId>         m_ConnectIdList;

    std::map<VxGUID, std::set<EConnectReason>>      m_ConnectReasonList;
};
