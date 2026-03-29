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
#include <CoreLib/GroupieId.h>

#include <QObject>
#include <QTimer>

#include <set>

class QWidget;

class AppCommon;
class GuiRandConnectCallback;
class GuiOfferSession;

class GuiRandConnectMgr : public QObject, public RandConnectCallback
{
    Q_OBJECT
public:
    struct GuiRandConnectOffer
    {
        GroupieId                m_GroupieId;
        VxGUID                   m_ToUserOnlineId;
        VxGUID                   m_SessionId;
        ERandAction              m_RandAction{ eRandActionNone };
        uint64_t                 m_TimeRequestedMs{ 0 };
        EOfferType               m_OfferType{ eOfferTypeUnknown };
    };

    GuiRandConnectMgr();
    GuiRandConnectMgr( const GuiRandConnectMgr& rhs ) = delete;
	virtual ~GuiRandConnectMgr() = default;

    virtual void                onAppCommonCreated( void );

    void                        wantRandConnectCallback( GuiRandConnectCallback* client, bool enable );

    enum ERandAction            getRandAction( VxGUID& onlineId );
    bool                        sendRandConnectAction( VxGUID& toUserOnlineId,
                                                       enum ERandAction randAction,
                                                       VxGUID sessionId = VxGUID::nullVxGUID(),
                                                       uint64_t timeRequestedMs = 0,
                                                       EOfferType offerType = eOfferTypeUnknown );
    bool                        sendRandConnectOfferRequest( VxGUID& toUserOnlineId, EOfferType offerType );
    bool                        sendRandConnectOfferResponse( VxGUID& peerOnlineId, enum ERandAction randAction );
    bool                        hasPendingIncomingOffer( VxGUID& peerOnlineId );
    bool                        hasPendingOutgoingOffer( VxGUID& peerOnlineId );

signals:
    void                        signalInternalRandConnect( GroupieId groupieId, enum ERandAction randAction );
    void                        signalInternalRandConnectOffer( GroupieId groupieId,
                                                                VxGUID toUserOnlineId,
                                                                VxGUID sessionId,
                                                                enum ERandAction randAction,
                                                                uint64_t timeRequestedMs,
                                                                EOfferType offerType );

private slots:
    void                        slotInternalRandConnect( GroupieId groupieId, enum ERandAction randAction );
    void                        slotInternalRandConnectOffer( GroupieId groupieId,
                                                              VxGUID toUserOnlineId,
                                                              VxGUID sessionId,
                                                              enum ERandAction randAction,
                                                              uint64_t timeRequestedMs,
                                                              EOfferType offerType );
    void                        slotOfferTimeoutCheck( void );

protected:
    void                        callbackRandConnect( GroupieId& groupieId, enum ERandAction randAction ) override;
    void                        callbackRandConnectOffer( GroupieId& groupieId,
                                                          VxGUID& toUserOnlineId,
                                                          VxGUID& sessionId,
                                                          enum ERandAction randAction,
                                                                                                                    uint64_t timeRequestedMs,
                                                                                                                    EOfferType offerType ) override;

    void                        updateRandConnect( GroupieId& groupieId, enum ERandAction randAction );
    void                        updateRandConnectOffer( GroupieId& groupieId,
                                                        VxGUID& toUserOnlineId,
                                                        VxGUID& sessionId,
                                                        enum ERandAction randAction,
                                                                                                                uint64_t timeRequestedMs,
                                                                                                                EOfferType offerType );
    bool                        isOfferTerminalAction( enum ERandAction randAction );
    GuiRandConnectOffer*        findPendingOfferWithPeer( VxGUID& peerOnlineId, bool incomingOffer );
        EPluginType                 offerTypeToPluginType( EOfferType offerType );
        std::shared_ptr<GuiOfferSession> createAcceptedOfferSession( const GuiRandConnectOffer& offer );
        bool                        launchAcceptedOfferSession( const GuiRandConnectOffer& offer );

    void                        announceRandConnect( VxGUID& onlineId, enum ERandAction randAction );

    std::vector<std::pair<GroupieId, enum ERandAction>>      m_MemberList;
    std::vector<GuiRandConnectOffer> m_OfferList;

    std::vector<GuiRandConnectCallback*> m_MemberClients;
    QTimer*                     m_OfferTimeoutTimer{ nullptr };

};
