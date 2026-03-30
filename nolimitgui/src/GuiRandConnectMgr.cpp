//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiRandConnectMgr.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppletPeerBase.h"
#include "GuiRandConnectCallback.h"
#include "GuiHelpers.h"
#include "GuiOfferSession.h"
#include "GuiUser.h"

#include <P2PEngine/P2PEngine.h>
#include <RandConnect/RandConnectMgr.h>

#include <OfferBase/OfferBaseInfo.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>
#include <PktLib/PktsRandConnectDefs.h>

#include <QFrame>

namespace
{
    const uint64_t RAND_CONNECT_OFFER_TIMEOUT_MS = 17000;

    EOfferType NormalizeRandomConnectOfferType( EOfferType offerType )
    {
        switch( offerType )
        {
        case eOfferTypeTruthOrDare:
        case eOfferTypeVideoChat:
        case eOfferTypeVoicePhone:
            return offerType;

        default:
            return eOfferTypeVideoChat;
        }
    }

    EOfferResponse RandActionToOfferResponse( ERandAction randAction )
    {
        switch( randAction )
        {
        case eRandActionOfferAccept:
            return eOfferResponseAccept;

        case eRandActionOfferReject:
            return eOfferResponseReject;

        case eRandActionOfferCancel:
            return eOfferResponseCancelSession;

        case eRandActionOfferNoResponse:
        case eRandActionOfferMissed:
            return eOfferResponseEndSession;

        default:
            return eOfferResponseNotSet;
        }
    }
}

//============================================================================
GuiRandConnectMgr::GuiRandConnectMgr()
    : QObject()
{
}

//============================================================================
void GuiRandConnectMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalRandConnect(GroupieId,ERandAction)), this, SLOT(slotInternalRandConnect(GroupieId,ERandAction)), Qt::QueuedConnection );
    connect( this,
             SIGNAL(signalInternalRandConnectOffer(GroupieId,VxGUID,VxGUID,ERandAction,uint64_t,EOfferType)),
             this,
             SLOT(slotInternalRandConnectOffer(GroupieId,VxGUID,VxGUID,ERandAction,uint64_t,EOfferType)),
             Qt::QueuedConnection );

    m_OfferTimeoutTimer = new QTimer( this );
    m_OfferTimeoutTimer->setInterval( 1000 );
    connect( m_OfferTimeoutTimer, SIGNAL(timeout()), this, SLOT(slotOfferTimeoutCheck()) );
    m_OfferTimeoutTimer->start();

    GetPtoPEngine().getRandConnectMgr().wantRandConnectCallbacks( this, true );
}

//============================================================================
enum ERandAction GuiRandConnectMgr::getRandAction( VxGUID& onlineId )
{
    ERandAction randAction{ eRandActionNone };
    auto iter = std::find_if(m_MemberList.begin(), m_MemberList.end(), 
                              [&](const std::pair<GroupieId, enum ERandAction>& x) { return x.first.getUserOnlineId() == onlineId; });
    if( iter != m_MemberList.end() )
    {
        randAction = iter->second;
    }

    return randAction;
}

//============================================================================
bool GuiRandConnectMgr::sendRandConnectAction( VxGUID& toUserOnlineId,
                                               enum ERandAction randAction,
                                               VxGUID sessionId,
                                               uint64_t timeRequestedMs,
                                               EOfferType offerType )
{
    return GetPtoPEngine().fromGuiSendRandConnectAction( toUserOnlineId, randAction, sessionId, timeRequestedMs, offerType );
}

//============================================================================
bool GuiRandConnectMgr::sendRandConnectOfferRequest( VxGUID& toUserOnlineId, EOfferType offerType )
{
    VxGUID sessionId;
    sessionId.initializeWithNewVxGUID();
    return sendRandConnectAction( toUserOnlineId,
                                  eRandActionOfferRequest,
                                  sessionId,
                                  GetTimeStampMs(),
                                  NormalizeRandomConnectOfferType( offerType ) );
}

//============================================================================
bool GuiRandConnectMgr::sendRandConnectOfferResponse( VxGUID& peerOnlineId, enum ERandAction randAction )
{
    if( randAction != eRandActionOfferAccept && randAction != eRandActionOfferReject && randAction != eRandActionOfferCancel )
    {
        return false;
    }

    GuiRandConnectOffer* incomingOffer = findPendingOfferWithPeer( peerOnlineId, true );
    if( incomingOffer )
    {
        return sendRandConnectAction( peerOnlineId,
                                      randAction,
                                      incomingOffer->m_SessionId,
                                      incomingOffer->m_TimeRequestedMs,
                                      incomingOffer->m_OfferType );
    }

    GuiRandConnectOffer* outgoingOffer = findPendingOfferWithPeer( peerOnlineId, false );
    if( outgoingOffer )
    {
        return sendRandConnectAction( peerOnlineId,
                                      randAction,
                                      outgoingOffer->m_SessionId,
                                      outgoingOffer->m_TimeRequestedMs,
                                      outgoingOffer->m_OfferType );
    }

    return false;
}

//============================================================================
bool GuiRandConnectMgr::sendRandConnectOfferResponse( VxGUID& peerOnlineId, VxGUID& sessionId, enum ERandAction randAction )
{
    if( randAction != eRandActionOfferAccept && randAction != eRandActionOfferReject && randAction != eRandActionOfferCancel )
    {
        return false;
    }

    auto iter = std::find_if( m_OfferList.begin(), m_OfferList.end(),
                              [&]( const GuiRandConnectOffer& offer )
                              {
                                  if( offer.m_RandAction != eRandActionOfferRequest )
                                  {
                                      return false;
                                  }

                                  if( offer.m_SessionId != sessionId )
                                  {
                                      return false;
                                  }

                                  const VxGUID fromOnlineId = offer.m_GroupieId.getUserOnlineId();
                                  const VxGUID toOnlineId = offer.m_ToUserOnlineId;
                                  return fromOnlineId == peerOnlineId || toOnlineId == peerOnlineId;
                              } );

    if( iter == m_OfferList.end() )
    {
        return false;
    }

    return sendRandConnectAction( peerOnlineId,
                                  randAction,
                                  iter->m_SessionId,
                                  iter->m_TimeRequestedMs,
                                  iter->m_OfferType );
}

//============================================================================
bool GuiRandConnectMgr::hasPendingIncomingOffer( VxGUID& peerOnlineId )
{
    return nullptr != findPendingOfferWithPeer( peerOnlineId, true );
}

//============================================================================
bool GuiRandConnectMgr::hasPendingOutgoingOffer( VxGUID& peerOnlineId )
{
    return nullptr != findPendingOfferWithPeer( peerOnlineId, false );
}

//============================================================================
void GuiRandConnectMgr::callbackRandConnect( GroupieId& groupieId, enum ERandAction randAction )
{
    emit signalInternalRandConnect( groupieId, randAction );
}

//============================================================================
void GuiRandConnectMgr::callbackRandConnectOffer( GroupieId& groupieId,
                                                  VxGUID& toUserOnlineId,
                                                  VxGUID& sessionId,
                                                  enum ERandAction randAction,
                                                  uint64_t timeRequestedMs,
                                                  EOfferType offerType )
{
    emit signalInternalRandConnectOffer( groupieId, toUserOnlineId, sessionId, randAction, timeRequestedMs, offerType );
}

//============================================================================
void GuiRandConnectMgr::slotInternalRandConnect( GroupieId groupieId, enum ERandAction randAction )
{
    updateRandConnect( groupieId, randAction );
}

//============================================================================
void GuiRandConnectMgr::slotInternalRandConnectOffer( GroupieId groupieId,
                                                      VxGUID toUserOnlineId,
                                                      VxGUID sessionId,
                                                      enum ERandAction randAction,
                                                      uint64_t timeRequestedMs,
                                                      EOfferType offerType )
{
    updateRandConnectOffer( groupieId, toUserOnlineId, sessionId, randAction, timeRequestedMs, offerType );
}

//============================================================================
void GuiRandConnectMgr::slotOfferTimeoutCheck( void )
{
    const uint64_t nowMs = GetTimeStampMs();
    const VxGUID myOnlineId = GetPtoPEngine().getMyOnlineId();
    std::vector<GuiRandConnectOffer> timeoutOffers;

    for( auto iter = m_OfferList.begin(); iter != m_OfferList.end(); )
    {
        if( iter->m_RandAction != eRandActionOfferRequest )
        {
            ++iter;
            continue;
        }

        if( iter->m_TimeRequestedMs && ( iter->m_TimeRequestedMs + RAND_CONNECT_OFFER_TIMEOUT_MS <= nowMs ) )
        {
            timeoutOffers.emplace_back( *iter );
            iter = m_OfferList.erase( iter );
        }
        else
        {
            ++iter;
        }
    }

    for( auto& timedOutOffer : timeoutOffers )
    {
        bool incomingOffer = timedOutOffer.m_ToUserOnlineId == myOnlineId;
        VxGUID peerOnlineId = incomingOffer ? timedOutOffer.m_GroupieId.getUserOnlineId() : timedOutOffer.m_ToUserOnlineId;
        ERandAction timeoutAction = incomingOffer ? eRandActionOfferMissed : eRandActionOfferNoResponse;
        sendRandConnectAction( peerOnlineId,
                               timeoutAction,
                               timedOutOffer.m_SessionId,
                               timedOutOffer.m_TimeRequestedMs,
                               timedOutOffer.m_OfferType );
    }
}

//============================================================================
void GuiRandConnectMgr::updateRandConnect( GroupieId& groupieId, enum ERandAction randAction )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "RandConnectMgr::updateRandConnect invalid groupieId %s", GetAppInstance().describeGroupieId(groupieId).c_str());
        return;
    }

    LogMsg( LOG_VERBOSE, "RandConnectMgr::updateRandConnect groupieId %s action %d", GetAppInstance().describeGroupieId( groupieId ).c_str(), randAction );

    bool wasUpdated = false;
    bool wasFound = false;
    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); ++iter )
    {
        if( iter->first == groupieId )
        {
            wasFound = true;
            if( randAction == eRandActionNone )
            {
                m_MemberList.erase( iter );
                wasUpdated = true;
            }
            else
            {
                if( iter->second != randAction )
                {
                    iter->second = randAction;
                    wasUpdated = true;
                }
            }

            break;
        }
    }

    if( !wasFound && randAction != eRandActionNone )
    {
        m_MemberList.emplace_back( std::make_pair( groupieId, randAction) );
        wasUpdated = true;
    }

    if( wasUpdated )
    {
        announceRandConnect( groupieId.getUserOnlineId(), randAction);
    }
}

//============================================================================
void GuiRandConnectMgr::updateRandConnectOffer( GroupieId& groupieId,
                                                VxGUID& toUserOnlineId,
                                                VxGUID& sessionId,
                                                enum ERandAction randAction,
                                                uint64_t timeRequestedMs,
                                                EOfferType offerType )
{
    const VxGUID myOnlineId = GetPtoPEngine().getMyOnlineId();
    const VxGUID creatorOnlineId = groupieId.getUserOnlineId();
    const bool iAmOfferTarget = ( toUserOnlineId == myOnlineId );
    const bool iAmOfferCreator = ( creatorOnlineId == myOnlineId );
    const bool iAmOfferParticipant = iAmOfferTarget || iAmOfferCreator;

    if( !sessionId.isVxGUIDValid() )
    {
        return;
    }

    if( !timeRequestedMs )
    {
        timeRequestedMs = GetTimeStampMs();
    }

    const EOfferType normalizedOfferType = NormalizeRandomConnectOfferType( offerType );
    VxGUID peerOnlineId = iAmOfferTarget ? creatorOnlineId : toUserOnlineId;

    OfferBaseInfo offerInfo;
    EPluginType pluginType = offerTypeToPluginType( normalizedOfferType );
    offerInfo.setPluginType( pluginType );
    offerInfo.setOfferType( normalizedOfferType );
    offerInfo.getAssetUniqueId() = sessionId;
    VxGUID offerId = GuiHelpers::isPluginSingleSession( pluginType ) ? peerOnlineId : sessionId;
    offerInfo.setOfferId( offerId );
    offerInfo.setOnlineId( peerOnlineId );
    offerInfo.setCreatorId( creatorOnlineId );
    offerInfo.setHistoryId( peerOnlineId );
    offerInfo.setOfferMgr( iAmOfferCreator ? eOfferMgrHost : eOfferMgrClient );
    offerInfo.setOfferTimestamp( (int64_t)timeRequestedMs );
    offerInfo.setCreationTime( (int64_t)timeRequestedMs );

    LogMsg( LOG_VERBOSE,
            "GuiRandConnectMgr::%s offer action %d from %s to %s session %s time %llu",
            __func__,
            randAction,
            GetAppInstance().describeUser( groupieId.getUserOnlineId() ).c_str(),
            GetAppInstance().describeUser( toUserOnlineId ).c_str(),
            sessionId.toHexString().c_str(),
            (unsigned long long)timeRequestedMs );

    auto iter = std::find_if( m_OfferList.begin(), m_OfferList.end(),
                              [&]( const GuiRandConnectOffer& offer )
                              {
                                  return offer.m_SessionId == sessionId;
                              } );

    if( !iAmOfferParticipant )
    {
        // Only sender and target should track/bridge one-on-one offers on this client.
        return;
    }

    if( randAction == eRandActionOfferRequest )
    {
        offerType = normalizedOfferType;

        if( iter == m_OfferList.end() )
        {
            GuiRandConnectOffer offer;
            offer.m_GroupieId = groupieId;
            offer.m_ToUserOnlineId = toUserOnlineId;
            offer.m_SessionId = sessionId;
            offer.m_RandAction = randAction;
            offer.m_TimeRequestedMs = timeRequestedMs;
            offer.m_OfferType = offerType;
            m_OfferList.emplace_back( offer );
        }
        else
        {
            iter->m_GroupieId = groupieId;
            iter->m_ToUserOnlineId = toUserOnlineId;
            iter->m_RandAction = randAction;
            iter->m_TimeRequestedMs = timeRequestedMs;
            iter->m_OfferType = offerType;
        }

        GetAppInstance().getUserMgr().getUser( peerOnlineId, true );

        if( toUserOnlineId == myOnlineId )
        {
            GetAppInstance().getOfferMgr().toGuiRxedPluginOffer( peerOnlineId, offerInfo );
        }
        else if( groupieId.getUserOnlineId() == myOnlineId )
        {
            // Mirror outgoing random-connect offers into GuiOfferMgr so they are visible in Offer List flow.
            GetAppInstance().getOfferMgr().toGuiRxedPluginOffer( peerOnlineId, offerInfo );
        }
    }
    else if( iter != m_OfferList.end() )
    {
        GuiRandConnectOffer offer = *iter;
        if( offerType != eOfferTypeUnknown )
        {
            offer.m_OfferType = NormalizeRandomConnectOfferType( offerType );
        }

        if( isOfferTerminalAction( randAction ) )
        {
            m_OfferList.erase( iter );

            EOfferResponse offerResponse = RandActionToOfferResponse( randAction );
            if( offerResponse != eOfferResponseNotSet )
            {
                offerInfo.setOfferResponse( offerResponse );

                if( toUserOnlineId == myOnlineId )
                {
                    GetAppInstance().getOfferMgr().toGuiRxedOfferReply( peerOnlineId, offerInfo );
                }
                else if( groupieId.getUserOnlineId() == myOnlineId )
                {
                    GetAppInstance().getOfferMgr().toGuiRxedOfferReply( peerOnlineId, offerInfo );
                }
            }

            if( randAction == eRandActionOfferAccept )
            {
                launchAcceptedOfferSession( offer );
            }
        }
        else
        {
            iter->m_RandAction = randAction;
            if( offerType != eOfferTypeUnknown )
            {
                iter->m_OfferType = NormalizeRandomConnectOfferType( offerType );
            }
        }
    }
}

//============================================================================
EPluginType GuiRandConnectMgr::offerTypeToPluginType( EOfferType offerType )
{
    switch( NormalizeRandomConnectOfferType( offerType ) )
    {
    case eOfferTypeTruthOrDare:
        return ePluginTypeTruthOrDare;

    case eOfferTypeVoicePhone:
        return ePluginTypeVoicePhone;

    case eOfferTypeVideoChat:
    default:
        return ePluginTypeVideoChat;
    }
}

//============================================================================
std::shared_ptr<GuiOfferSession> GuiRandConnectMgr::createAcceptedOfferSession( const GuiRandConnectOffer& offer )
{
    const VxGUID myOnlineId = GetPtoPEngine().getMyOnlineId();
    const bool iStartedOffer = ( offer.m_GroupieId.getUserOnlineId() == myOnlineId );
    const VxGUID peerOnlineId = iStartedOffer ? offer.m_ToUserOnlineId : offer.m_GroupieId.getUserOnlineId();
    GuiUser* guiUser = GetAppInstance().getUserMgr().getUser( peerOnlineId );
    if( !guiUser )
    {
        LogMsg( LOG_ERROR, "GuiRandConnectMgr::%s user not found for accepted offer", __func__ );
        return nullptr;
    }

    OfferBaseInfo offerInfo;
    EPluginType pluginType = offerTypeToPluginType( offer.m_OfferType );
    offerInfo.setPluginType( pluginType );
    offerInfo.setOfferType( NormalizeRandomConnectOfferType( offer.m_OfferType ) );
    offerInfo.getAssetUniqueId() = offer.m_SessionId;
    VxGUID creatorId = offer.m_GroupieId.getUserOnlineId();
    VxGUID historyId = offer.m_ToUserOnlineId;
    offerInfo.setCreatorId( creatorId );
    offerInfo.setHistoryId( historyId );
    VxGUID offerId = GuiHelpers::isPluginSingleSession( pluginType ) ? peerOnlineId : offer.m_SessionId;
    offerInfo.setOfferId( offerId );
    offerInfo.setOfferMgr( iStartedOffer ? eOfferMgrHost : eOfferMgrClient );
    offerInfo.setOfferResponse( eOfferResponseAccept );
    offerInfo.setOfferTimestamp( (int64_t)offer.m_TimeRequestedMs );
    offerInfo.setOfferResponseTimestamp( (int64_t)GetTimeStampMs() );

    GuiOfferInfo guiOfferInfo( offerInfo );
    guiOfferInfo.setUser( guiUser );
    guiOfferInfo.setOfferState( eOfferStateAccepted );
    guiOfferInfo.updateLastActivityTime();

    return std::make_shared<GuiOfferSession>( GuiOfferSession( guiOfferInfo ) );
}

//============================================================================
bool GuiRandConnectMgr::launchAcceptedOfferSession( const GuiRandConnectOffer& offer )
{
    std::shared_ptr<GuiOfferSession> offerSession = createAcceptedOfferSession( offer );
    if( !offerSession )
    {
        return false;
    }

    EApplet appletType = GuiHelpers::pluginTypeToSessionApplet( offerSession->getPluginType() );
    if( eAppletUnknown == appletType )
    {
        LogMsg( LOG_ERROR, "GuiRandConnectMgr::%s unknown applet type", __func__ );
        return false;
    }

    GuiUser* guiUser = offerSession->getUser();
    if( !guiUser )
    {
        return false;
    }

    AppletPeerBase* applet = dynamic_cast<AppletPeerBase*>( GetAppInstance().getAppletMgr().findAppletDialog( appletType ) );
    if( applet )
    {
        // Reuse only if this applet is already for the same peer/plugin session.
        if( !guiUser->isMyself() && !applet->isOfferMatch( offerSession ) )
        {
            GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() ) );
            return false;
        }
    }
    else
    {
        QFrame* contentFrame = GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() );
        applet = dynamic_cast<AppletPeerBase*>( GetAppInstance().getAppletMgr().launchApplet( appletType, contentFrame ) );
    }

    if( !applet )
    {
        return false;
    }

    if( !applet->setOfferSession( offerSession ) )
    {
        return false;
    }

    return applet->beginAcceptedSession();
}

//============================================================================
bool GuiRandConnectMgr::isOfferTerminalAction( enum ERandAction randAction )
{
    switch( randAction )
    {
    case eRandActionOfferAccept:
    case eRandActionOfferReject:
    case eRandActionOfferCancel:
    case eRandActionOfferNoResponse:
    case eRandActionOfferMissed:
        return true;

    default:
        return false;
    }
}

//============================================================================
GuiRandConnectMgr::GuiRandConnectOffer* GuiRandConnectMgr::findPendingOfferWithPeer( VxGUID& peerOnlineId, bool incomingOffer )
{
    const VxGUID myOnlineId = GetPtoPEngine().getMyOnlineId();
    auto iter = std::find_if( m_OfferList.begin(), m_OfferList.end(),
                              [&]( GuiRandConnectOffer& offer )
                              {
                                  if( offer.m_RandAction != eRandActionOfferRequest )
                                  {
                                      return false;
                                  }

                                  if( incomingOffer )
                                  {
                                      return offer.m_ToUserOnlineId == myOnlineId && offer.m_GroupieId.getUserOnlineId() == peerOnlineId;
                                  }

                                  return offer.m_GroupieId.getUserOnlineId() == myOnlineId && offer.m_ToUserOnlineId == peerOnlineId;
                              } );

    if( iter == m_OfferList.end() )
    {
        return nullptr;
    }

    return &( *iter );
}

//============================================================================
void GuiRandConnectMgr::wantRandConnectCallback( GuiRandConnectCallback* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "GuiRandConnectMgr null client" );
        return;
    }

    bool wasFound = false;
    for( auto iter = m_MemberClients.begin(); iter != m_MemberClients.end(); ++iter )
    {
        if( *iter == client )
        {
            wasFound = true;
            if( !enable )
            {
                m_MemberClients.erase( iter );
            }

            break;
        }
    }

    if( enable && !wasFound )
    {
        m_MemberClients.push_back( client );
    }
}

//============================================================================
void GuiRandConnectMgr::announceRandConnect( VxGUID& onlineId, enum ERandAction randAction )
{
    for( auto& client : m_MemberClients )
    {
        client->callbackGuiRandConnect( onlineId, randAction );
    }
}
