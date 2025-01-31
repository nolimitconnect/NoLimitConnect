//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginSessionMgr.h"

#include <OfferBase/OfferBaseInfo.h>
#include <P2PEngine/P2PEngine.h>

#include "PluginBase.h"
#include "PluginMgr.h"
#include "PluginSessionBase.h"


#include <NetLib/VxSktBase.h>

#include <PktLib/PktAnnounce.h>
#include <PktLib/PktsPluginOffer.h>
#include <PktLib/PktsSession.h>
#include <CoreLib/PktBlobEntry.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>

//#define DEBUG_AUTOPLUGIN_LOCK 1

//============================================================================ 
PluginSessionMgr::PluginSessionMgr( P2PEngine& engine, PluginBase& plugin, PluginMgr& pluginMgr )
: SessionMgrBase( engine, plugin, pluginMgr )
{
}

//============================================================================ 
PluginSessionMgr::~PluginSessionMgr()
{
}

//============================================================================
PluginSessionBase* PluginSessionMgr::findPluginSessionBySessionId( VxGUID& sessionId, bool pluginIsLocked )
{
	SessionIter iter;
	iter = m_aoSessions.find( sessionId );
	if( iter != m_aoSessions.end() )
	{
		return (PluginSessionBase*)(*iter).second;
	}

	return NULL;
}

//============================================================================
PluginSessionBase*	 PluginSessionMgr::findPluginSessionByOnlineId( VxGUID& onlineId, bool pluginIsLocked )
{
	SessionIter iter;
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
		#ifdef DEBUG_AUTOPLUGIN_LOCK
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::findPluginSessionByOnlineId pluginMutex.lock start" );
		#endif //DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
		#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::findPluginSessionByOnlineId pluginMutex.lock done" );
		#endif //DEBUG_AUTOPLUGIN_LOCK
	}

	for( iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->getSendToId() == onlineId )
		{
			if( false == pluginIsLocked )
			{
				#ifdef DEBUG_AUTOPLUGIN_LOCK
								LogMsg( LOG_VERBOSE, "PluginSessionMgr::findPluginSessionByOnlineId pluginMutex.unlock" );
				#endif //DEBUG_AUTOPLUGIN_LOCK
				pluginMutex.unlock();
			}

			return session;
		}
	}

	if( false == pluginIsLocked )
	{
		#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::findPluginSessionByOnlineId pluginMutex.unlock" );
		#endif //DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
	}

	return NULL;
}

//============================================================================
void PluginSessionMgr::replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt )
{
	SessionIter iter;
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_VERBOSE, "PluginSessionMgr::replaceConnection autoLock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock pluginMutexLock( &m_Plugin );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_VERBOSE, "PluginSessionMgr::replaceConnection autoLock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	for( iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( poOldSkt == session->getSkt() )
		{
			session->setSkt( poNewSkt );
		}
	}

#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_VERBOSE, "PluginSessionMgr::replaceConnection autoLock destroy" );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
//! called when connection is lost
void PluginSessionMgr::onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	bool sktIsDisconnected = !sktBase->isConnected();
	// deadlock occurs here if use continuous lock so get the session to erase but dont end session until lock is remove 
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	bool bErased = true;
	while( bErased ) 
	{
		bErased = false;
		PluginSessionBase* sessionBase = nullptr;
		#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::onContactWentOffline pluginMutex.lock start" );
		#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::onContactWentOffline pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK

		for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
		{
			sessionBase = ((*iter).second);
			if( ( netIdent->getMyOnlineId() == sessionBase->getSendToId() )
				|| ( sktIsDisconnected && ( sessionBase->getSkt() == sktBase ) ) )
			{
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::onContactWentOffline erasing session for %s", m_Engine.describeUser( sessionBase->getSendToId() ).c_str() );
				bErased = true;
				break;
			}
		}

		#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::onContactWentOffline pluginMutex.unlock" );
		#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
		if( bErased )
		{
			doEndAndEraseSession( sessionBase, eOfferResponseUserOffline, false );
		}
	}
}

//============================================================================
void PluginSessionMgr::onContactOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
	// TODO implement
}

//============================================================================
//! called when connection is lost
void PluginSessionMgr::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	bool bErased = true;

	while( bErased ) 
	{
		bErased = false;
		PluginSessionBase* sessionBase = nullptr;
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::onConnectionLost pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::onConnectionLost pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK

		for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
		{
			sessionBase = ((*iter).second);
			if( sktBase == sessionBase->getSkt() )
			{
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::onConnectionLost erasing session for %s", m_Engine.describeUser( sessionBase->getSendToId() ).c_str() );
				bErased = true;
				break;
			}
		}

#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::onConnectionLost pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
		if( bErased )
		{
			doEndAndEraseSession( sessionBase, eOfferResponseUserOffline, false );
		}
	}
}

//============================================================================
void PluginSessionMgr::cancelSessionByOnlineId( VxGUID& onlineId )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	bool bErased = true;

	while( bErased ) 
	{
		bErased = false;
		PluginSessionBase* sessionBase = 0;
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::cancelSessionByOnlineId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::cancelSessionByOnlineId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK

		for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
		{
			sessionBase = ((*iter).second);
			if( sessionBase->getSendToId() == onlineId )
			{
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::cancelSessionByOnlineId erasing session for %s", m_Engine.describeUser( sessionBase->getSendToId() ).c_str() );
				bErased = true;
				break;
			}
		}

#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::cancelSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
		if( bErased )
		{
			doEndAndEraseSession( sessionBase, eOfferResponseCancelSession, false );
		}
	}
}

//============================================================================
void PluginSessionMgr::doEndAndEraseSession( PluginSessionBase* sessionBase, EOfferResponse offerResponse, bool pluginIsLocked )
{
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	m_Plugin.onSessionEnded( sessionBase, pluginIsLocked, eOfferResponseUserOffline );

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::doEndAndEraseSession pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::doEndAndEraseSession pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		if( sessionBase == ((*iter).second) )
		{
			m_aoSessions.erase(iter);
			delete sessionBase;
			break;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::doEndAndEraseSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
	}
}

//============================================================================
bool PluginSessionMgr::fromGuiIsPluginInSession( bool pluginIsLocked, VxGUID& onlineId, int pvUserData, VxGUID lclSessionId )
{
	bool isInSesion = false;
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::fromGuiIsPluginInSession pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::fromGuiIsPluginInSession pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	if( lclSessionId.isVxGUIDValid() )
	{
		auto iter = m_aoSessions.find( lclSessionId );
		if( iter != m_aoSessions.end() )
		{
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::doEndAndEraseSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
				pluginMutex.unlock();
			}

			return true;
		}
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->getSendToId() == onlineId )
		{
			isInSesion = true;
			break;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::doEndAndEraseSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
	}

	return isInSesion;
}

//============================================================================
//! user wants to send offer to friend.. return false if cannot connect
bool PluginSessionMgr::fromGuiMakePluginOffer( bool pluginIsLocked, VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	bool offerSentResult = false;

	VxGUID& lclSessionId = offerInfo.getOfferId();
	std::shared_ptr<VxSktBase> sktBase = m_Engine.getConnectIdListMgr().findBestUserOnlineConnection( onlineId );
	if( !sktBase || !sktBase->isConnected() )
	{
		return false;
	}

	PluginSessionBase* pluginSession{ nullptr };
	if( lclSessionId.isVxGUIDValid() && ( false == isPluginSingleSession() ) )
	{
		pluginSession = findOrCreateP2PSessionWithSessionId( lclSessionId, sktBase, onlineId, pluginIsLocked );
	}
	else
	{
		pluginSession = findOrCreateP2PSessionWithOnlineId( onlineId, sktBase, pluginIsLocked, lclSessionId );
	}

	if( pluginSession )
	{
		PktPluginOfferReq pktReq;

		pktReq.setLclSessionId( lclSessionId );
		pktReq.setRmtSessionId( lclSessionId );

		offerInfo.addToBlob( pktReq.getBlobEntry() );
		pktReq.calcPktLen();

		offerSentResult = m_Plugin.txPacket( pluginSession->getSendToId(), pluginSession->getSkt(), &pktReq );
	}

	return offerSentResult;
}

//============================================================================
//! handle reply to offer
bool PluginSessionMgr::fromGuiOfferReply( bool pluginIsLocked, VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	PluginSessionBase* poOffer = nullptr;
	VxGUID& lclSessionId = offerInfo.getOfferId();
	EOfferResponse offerResponse = offerInfo.getOfferResponse();
	if( lclSessionId.isVxGUIDValid() && ( false == isPluginSingleSession() ) )
	{
		poOffer = findPluginSessionBySessionId( lclSessionId, pluginIsLocked );
	}
	else
	{
		poOffer = findPluginSessionByOnlineId( onlineId, pluginIsLocked );
	}

	bool bResponseSent = false;
	if( poOffer )
	{
		poOffer->setOfferResponse( offerResponse );
		poOffer->setLclSessionId( lclSessionId );

		PktPluginOfferReply pktReply;
		pktReply.setOfferResponse( offerResponse );
		pktReply.setLclSessionId( poOffer->getLclSessionId() );
		pktReply.setRmtSessionId( poOffer->getRmtSessionId() );
		offerInfo.addToBlob( pktReply.getBlobEntry() );
		if( m_Plugin.txPacket( poOffer->getSendToId(), poOffer->getSkt(), &pktReply ) )
		{
			bResponseSent = true;
		}

		if( ( false == bResponseSent ) 
			|| ( eOfferResponseReject == offerResponse )
			|| ( eOfferResponseCancelSession == offerResponse )
			|| ( eOfferResponseEndSession == offerResponse ) )
		{
			removeSession( pluginIsLocked, onlineId, lclSessionId, offerResponse );
		}
		else if( eOfferResponseAccept == offerResponse )
		{
			m_Plugin.onSessionStart( poOffer, pluginIsLocked );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, " PluginBase::fromGuiOfferReply: OFFER NOT FOUND");
	}

	return bResponseSent;
}

//============================================================================
void PluginSessionMgr::fromGuiStopPluginSession( bool pluginIsLocked, VxGUID& onlineId, int pvUserData, VxGUID lclSessionId )
{
	removeSession( pluginIsLocked, onlineId, lclSessionId, eOfferResponseEndSession );
}

//============================================================================
void PluginSessionMgr::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdentIn )
{
    VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
    VxNetIdent* srcNetIdent = m_Engine.getBigListMgr().findNetIdent( srcOnlineId );
    if( !srcNetIdent )
    {
        LogMsg( LOG_ERROR, "PluginSessionMgr::%s: unknown src ident %s from %s %s", __func__,
               srcOnlineId.toOnlineIdString().c_str(),
               netIdentIn->getOnlineName(), sktBase->describeSktType().c_str() );
        // TODO should this be a hack offense?
        return;
    }

    LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s: offer from %s %s", __func__,
           srcNetIdent->getOnlineName(), sktBase->describeSktType().c_str() );
	
	PktPluginOfferReq* pktReq = (PktPluginOfferReq *)pktHdr;

	OfferBaseInfo offerInfo;
	if( !offerInfo.extractFromBlob( pktReq->getBlobEntry() ) )
	{
		LogMsg( LOG_ERROR, "PluginSessionMgr::%s: could not extract offer from %s", __func__, srcNetIdent->getOnlineName() );
		return;
	}

	if( IsPluginSingleSession( offerInfo.getPluginType() ) && m_Plugin.getIsPluginInSession() )
	{
		offerInfo.setOfferResponse( eOfferResponseBusy );

		PktPluginOfferReply pktReply;

		pktReply.setLclSessionId( pktReq->getLclSessionId() );
		pktReply.setRmtSessionId( pktReq->getRmtSessionId() );
		pktReply.setPluginType( getPluginType() );
		offerInfo.addToBlob( pktReply.getBlobEntry() );
		pktReply.calcPktLen();
		if( !m_Plugin.txPacket( srcOnlineId, sktBase, &pktReply ) )
		{
			LogMsg( LOG_ERROR, "PluginSessionMgr::%s: failed send to %s", __func__, m_Engine.describeUser( srcOnlineId ).c_str() );
			return;
		}

		// for call missed
		IToGui::getIToGui().toGuiRxedPluginOffer(srcOnlineId, offerInfo );

		return;
	}

	VxGUID lclSessionId = offerInfo.getOfferId();
	PluginSessionBase* pluginSession = nullptr;
	PluginBase::AutoPluginLock pluginMutexLock( &m_Plugin );

	if( lclSessionId.isVxGUIDValid() && (false == isPluginSingleSession()) )
	{
		pluginSession = findOrCreateP2PSessionWithSessionId( lclSessionId, sktBase, srcOnlineId, true);
	}
	else
	{
		pluginSession = findOrCreateP2PSessionWithOnlineId( srcOnlineId, sktBase, true, lclSessionId );
	}

	pluginSession->setSkt( sktBase );
	pluginSession->setSendToId( srcOnlineId );
	pluginSession->setLclSessionId( lclSessionId );
	pluginSession->setRmtSessionId( pktReq->getLclSessionId() );
	pluginSession->setOfferInfo( offerInfo, false );

	IToGui::getIToGui().toGuiRxedPluginOffer( srcOnlineId, offerInfo );
}

//============================================================================
void PluginSessionMgr::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    VxGUID onlineId = pktHdr->getSrcOnlineId();
	PluginBase::AutoPluginLock pluginMutexLock( &m_Plugin );

	PktPluginOfferReply* pktReply = (PktPluginOfferReply*)pktHdr;

	OfferBaseInfo offerInfo;
	if( offerInfo.extractFromBlob( pktReply->getBlobEntry() ) )
	{
		VxGUID lclSessionId = offerInfo.getOfferId();
		PluginSessionBase* poOffer = findOrCreateP2PSessionWithSessionId( lclSessionId, sktBase, pktReply->getSrcOnlineId(), true);
		if( poOffer )
		{
			poOffer->setOfferInfo( offerInfo, false );

			poOffer->setLclSessionId( pktReply->getLclSessionId() );
			poOffer->setRmtSessionId( pktReply->getRmtSessionId() );
			poOffer->setOfferResponse( pktReply->getOfferResponse() );
			if( eOfferResponseAccept == poOffer->getOfferResponse() )
			{
				m_Plugin.onSessionStart( poOffer, true );
			}
			else
			{
                removeSession( true, onlineId, poOffer->getRmtSessionId(), poOffer->getOfferResponse(), false );
			}
		}

        IToGui::getIToGui().toGuiRxedOfferReply( onlineId, offerInfo);
	}
	else
	{
		LogMsg( LOG_ERROR, "PluginSessionMgr::%s: could not extract offer from %s", __func__, m_Engine.describeUser( onlineId ).c_str() );
	}
}

//============================================================================
void PluginSessionMgr::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
	PktSessionStopReq * pkt = (PktSessionStopReq *)pktHdr;
    removeSession( false, srcOnlineId, pkt->getRmtSessionId(), eOfferResponseEndSession);
}

//============================================================================
P2PSession* PluginSessionMgr::findP2PSessionBySessionId( VxGUID& sessionId, bool pluginIsLocked )
{
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.lock start", __func__ );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.lock done", __func__ );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	SessionIter iter;
	for( iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		if( sessionId == (*iter).first )
		{
			PluginSessionBase* session = (*iter).second;
			if( session->isP2PSession() )
			{
				if( false == pluginIsLocked )
				{
#ifdef DEBUG_AUTOPLUGIN_LOCK
					LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.unlock", __func__ );
#endif // DEBUG_AUTOPLUGIN_LOCK
					pluginMutex.unlock();
				}

				return (P2PSession*)session;
			}
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.unlock", __func__ );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
	}

	return NULL;
}

//============================================================================
P2PSession* PluginSessionMgr::findP2PSessionByOnlineId( VxGUID& onlineId, bool pluginIsLocked )
{
	SessionIter iter;
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.lock start", __func__ );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.lock done", __func__ );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->isP2PSession()
			&& ( session->getSendToId() == onlineId ) )
		{
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.unlock", __func__  );
#endif // DEBUG_AUTOPLUGIN_LOCK
				pluginMutex.unlock();
			}

			return (P2PSession*)session;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s pluginMutex.unlock", __func__  );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
	}

	return NULL;
}
//============================================================================
P2PSession* PluginSessionMgr::findOrCreateP2PSessionWithSessionId( VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, bool pluginIsLocked )
{
	if( !sktBase || !sktBase->isConnected() )
	{
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s user %s not connected", __func__, m_Engine.describeUser( onlineId ).c_str() );
		return nullptr;
	}

	P2PSession* session = findP2PSessionBySessionId( sessionId, pluginIsLocked );
	if( !session )
	{
		session = m_Plugin.createP2PSession( sessionId, sktBase, onlineId );
		addSession( session->getLclSessionId(), session, pluginIsLocked );
	}
	else
	{
		session->setSkt( sktBase );
	}

	return session;
}

//============================================================================
P2PSession* PluginSessionMgr::findOrCreateP2PSessionWithOnlineId( VxGUID onlineId, std::shared_ptr<VxSktBase>& sktBase, bool pluginIsLocked, VxGUID lclSessionId )
{
	if( !sktBase || !sktBase->isConnected() )
	{
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::%s user %s not connected", __func__, m_Engine.describeUser( onlineId ).c_str() );
		return nullptr;
	}

	P2PSession* session = findP2PSessionByOnlineId( onlineId, pluginIsLocked );
	if( !session )
	{
        session = m_Plugin.createP2PSession( sktBase, onlineId );
		if( false == lclSessionId.isVxGUIDValid() )
		{
			lclSessionId.initializeWithNewVxGUID();
			session->setLclSessionId( lclSessionId );
			addSession( onlineId, session, pluginIsLocked );
		}
		else
		{
			session->setLclSessionId( lclSessionId );
			addSession( onlineId, session, pluginIsLocked );
		}
	}
	else
	{
		session->setSkt( sktBase );
	}

	return session;
}

//============================================================================
TxSession * PluginSessionMgr::findTxSessionBySessionId( bool pluginIsLocked, VxGUID& sessionId )
{
	SessionIter iter;
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionBySessionId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionBySessionId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto sessionPair : m_aoSessions )
	{
		if( sessionId == sessionPair.first )
		{
			PluginSessionBase* session = sessionPair.second;
			if( session->isTxSession() )
			{
				if( false == pluginIsLocked )
				{
#ifdef DEBUG_AUTOPLUGIN_LOCK
					LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK	
					pluginMutex.unlock();
				}

				return (TxSession *)session;
			}
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK					
		pluginMutex.unlock();
	}

	return NULL;
}

//============================================================================
TxSession * PluginSessionMgr::findTxSessionByOnlineId( bool pluginIsLocked, VxGUID& onlineId )
{
	SessionIter iter;
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionByOnlineId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionByOnlineId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->isTxSession()
			&& ( session->getSendToId() == onlineId ) )
		{
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK					
				pluginMutex.unlock();
			}

			return (TxSession *)session;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findTxSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK					
		pluginMutex.unlock();
	}

	return NULL;
}

//============================================================================
TxSession * PluginSessionMgr::findOrCreateTxSessionWithSessionId( VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, bool pluginIsLocked )
{
	TxSession * session = findTxSessionBySessionId( pluginIsLocked, sessionId );
	if( NULL == session )
	{
        session = m_Plugin.createTxSession( sessionId, sktBase, onlineId );
		addSession( sessionId, session, pluginIsLocked );
	}
	else
	{
		session->setSkt( sktBase );
	}

	return session;
}

//============================================================================
TxSession * PluginSessionMgr::findOrCreateTxSessionWithOnlineId( VxGUID onlineId, std::shared_ptr<VxSktBase>& sktBase, bool pluginIsLocked, VxGUID lclSessionId )
{
	TxSession * session = findTxSessionByOnlineId( pluginIsLocked, onlineId );
	if( NULL == session )
	{
        session = m_Plugin.createTxSession( sktBase, onlineId );
		if( ( false == lclSessionId.isVxGUIDValid() ) && ( false == isPluginSingleSession() ) )
		{
			addSession( session->getLclSessionId(), session, pluginIsLocked );
		}
		else
		{
			session->setLclSessionId( lclSessionId );
			addSession( lclSessionId, session, pluginIsLocked );
		}
	}
	else
	{
		session->setSkt( sktBase );
	}

	return session;
}

//============================================================================
int PluginSessionMgr::getTxSessionCount( bool pluginIsLocked )
{
	int txSessionCnt = 0;
	if( !pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::getTxSessionCount lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->isTxSession() )
		{
			txSessionCnt++;
		}
	}

	if( !pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::getTxSessionCount lock end" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}

	return txSessionCnt;
}

//============================================================================
RxSession * PluginSessionMgr::findRxSessionBySessionId( VxGUID& sessionId, bool pluginIsLocked  )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionBySessionId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionBySessionId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		if( sessionId == (*iter).first )
		{
			PluginSessionBase* session = (*iter).second;
			if( session->isRxSession() )
			{
				if( false == pluginIsLocked )
				{
#ifdef DEBUG_AUTOPLUGIN_LOCK
					LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
					m_Plugin.unlockPlugin();
				}

				return (RxSession *)session;
			}
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}

	return NULL;
}

//============================================================================
RxSession * PluginSessionMgr::findRxSessionByOnlineId( VxGUID& onlineId, bool pluginIsLocked  )
{
	SessionIter iter;
	VxMutex& pluginMutex = m_Plugin.getPluginMutex();
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionByOnlineId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.lock();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionByOnlineId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->isRxSession()
			&& ( session->getSendToId() == onlineId ) )
		{
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
				pluginMutex.unlock();
			}

			return (RxSession *)session;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::findRxSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		pluginMutex.unlock();
	}

	return NULL;
}

//============================================================================
RxSession * PluginSessionMgr::findOrCreateRxSessionWithSessionId( VxGUID& sessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, bool pluginIsLocked )
{
	RxSession * session = findRxSessionBySessionId( sessionId, pluginIsLocked );
	if( NULL == session )
	{
        session = m_Plugin.createRxSession( sessionId, sktBase, onlineId );
		addSession( sessionId, session, pluginIsLocked );
	}

	return session;
}

//============================================================================
RxSession * PluginSessionMgr::findOrCreateRxSessionWithOnlineId( VxGUID onlineId, std::shared_ptr<VxSktBase>& sktBase, bool pluginIsLocked, VxGUID lclSessionId )
{
	RxSession * session = findRxSessionByOnlineId( onlineId, pluginIsLocked );
	if( NULL == session )
	{
        session = m_Plugin.createRxSession( sktBase, onlineId );
		if( false == lclSessionId.isVxGUIDValid() )
		{
			addSession( session->getLclSessionId(), session, pluginIsLocked );
		}
		else
		{
			session->setLclSessionId( lclSessionId );
			addSession( lclSessionId, session, pluginIsLocked );
		}
	}

	return session;
}


//============================================================================ 
void PluginSessionMgr::addSession( VxGUID& sessionId, PluginSessionBase* session, bool pluginIsLocked )
{
	if( false == session->getLclSessionId().isVxGUIDValid() )
	{
		if( sessionId.isVxGUIDValid() )
		{
			session->setLclSessionId( sessionId );
		}
		else
		{
			session->getLclSessionId().initializeWithNewVxGUID();
		}
	}

	if( sessionId != session->getLclSessionId() )
	{
		LogMsg( LOG_WARNING, "WARNING SESSION IDS DONT MATCH PluginSessionMgr::addSession %s session id %s connect info %s", 
				m_Engine.describeUser( session->getSendToId() ).c_str(), sessionId.toHexString().c_str(), session->getSkt()->describeSktType().c_str());
	}

	if( false == sessionId.isVxGUIDValid() )
	{
		sessionId = session->getLclSessionId();
	}

	LogMsg( LOG_VERBOSE, "PluginSessionMgr::addSession %s session id %s connect info %s", 
			m_Engine.describeUser( session->getSendToId() ).c_str(), sessionId.toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
	if( pluginIsLocked )
	{
		m_aoSessions.insert(  std::make_pair( sessionId, session ) );
	}
	else
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::addSession autoLock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		PluginBase::AutoPluginLock pluginMutexLock( &m_Plugin );
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::addSession autoLock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_aoSessions.insert(  std::make_pair( sessionId, session ) );
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::addSession autoLock destroy" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}
}

//============================================================================ 
void PluginSessionMgr::endPluginSession( PluginSessionBase* session, bool pluginIsLocked )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		if( session == (*iter).second )
		{
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession %s session id %s connect info %s", 
					m_Engine.describeUser( session->getSendToId() ).c_str(), session->getLclSessionId().toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
			delete (*iter).second;
			m_aoSessions.erase(iter);
			break;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}
}

//============================================================================ 
void PluginSessionMgr::endPluginSession( VxGUID& sessionId, bool pluginIsLocked )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	SessionIter iter = m_aoSessions.find( sessionId );
	if( iter != m_aoSessions.end() )
	{
		PluginSessionBase* session = (*iter).second;
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession %s session id %s connect info %s", 
				m_Engine.describeUser( session->getSendToId() ).c_str(), session->getLclSessionId().toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
		m_aoSessions.erase(iter);
		delete session;
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::endPluginSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}
}

//============================================================================ 
void PluginSessionMgr::removeTxSessionBySessionId( VxGUID& sessionId, bool pluginIsLocked )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionBySessionId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionBySessionId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		if( (sessionId == (*iter).first) )
		{
			PluginSessionBase* session = (*iter).second;
			if( session->isTxSession() )
			{
				m_aoSessions.erase(iter);	
				delete session;
				break;
			}
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}
}

//============================================================================ 
void PluginSessionMgr::removeTxSessionByOnlineId( VxGUID& onlineId, bool pluginIsLocked )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionByOnlineId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionByOnlineId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->isTxSession()
			&& ( session->getSendToId() == onlineId ) )
		{
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionByOnlineId %s session id %s connect info %s", 
					m_Engine.describeUser( session->getSendToId() ).c_str(), session->getLclSessionId().toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
			delete session;
			m_aoSessions.erase(iter);	
			break;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeTxSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}
}

//============================================================================ 
void PluginSessionMgr::removeRxSessionBySessionId( VxGUID& sessionId, bool pluginIsLocked )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionBySessionId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionBySessionId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		if( (sessionId == (*iter).first) )
		{
			PluginSessionBase* session = (*iter).second;
			if( session->isRxSession() )
			{
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionBySessionId %s session id %s connect info %s", 
						m_Engine.describeUser( session->getSendToId() ).c_str(), session->getLclSessionId().toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
				delete session;
				m_aoSessions.erase(iter);	
				break;
			}
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}
}

//============================================================================ 
void PluginSessionMgr::removeRxSessionByOnlineId( VxGUID& onlineId, bool pluginIsLocked )
{
	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionByOnlineId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionByOnlineId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->isRxSession()
			&& ( session->getSendToId() == onlineId ) )
		{
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionByOnlineId %s session id %s connect info %s", 
				m_Engine.describeUser( session->getSendToId() ).c_str(), session->getLclSessionId().toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
			m_aoSessions.erase( iter );
			delete session;
			break;
		}
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeRxSessionByOnlineId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}
}

//============================================================================ 
// returns true if found and removed session
bool PluginSessionMgr::removeSessionBySessionId( bool pluginIsLocked, VxGUID& sessionId, EOfferResponse offerResponse )
{
	bool wasRemoved = false;
	if( sessionId.isVxGUIDValid() )
	{
		if( false == pluginIsLocked )
		{
#ifdef DEBUG_AUTOPLUGIN_LOCK
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSessionBySessionId pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
			m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSessionBySessionId pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		}

		auto iter = m_aoSessions.find( sessionId );
		if( iter != m_aoSessions.end() )
		{
			PluginSessionBase* session = (*iter).second;
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
				m_Plugin.unlockPlugin();
			}

			doEndAndEraseSession( session, offerResponse, pluginIsLocked );
			wasRemoved = true;
		}
		else
		{
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSessionBySessionId pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
				m_Plugin.unlockPlugin();
			}
		}
	}

	return wasRemoved;
}

//============================================================================
bool PluginSessionMgr::removeSession( bool pluginIsLocked, VxGUID& onlineId, VxGUID& sessionId, EOfferResponse offerResponse, bool fromGui )
{
	if( removeSessionBySessionId( pluginIsLocked, sessionId, offerResponse ) )
	{
		return true;
	}

	if( false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSession pluginMutex.lock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.lockPlugin();
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSession pluginMutex.lock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	}

	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;
		if( session->getSendToId() == onlineId )
		{
			if( false == fromGui )
			{
				// notify gui session removed ??
			}
			
			LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSession %s session id %s connect info %s", 
				m_Engine.describeUser( session->getSendToId() ).c_str(), session->getLclSessionId().toHexString().c_str(), session->getSkt()->describeSktType().c_str() );
			if( false == pluginIsLocked )
			{
#ifdef DEBUG_AUTOPLUGIN_LOCK
				LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
				m_Plugin.unlockPlugin();
			}

			doEndAndEraseSession( session, offerResponse, pluginIsLocked );
			return true;
		}
	}

	if(  false == pluginIsLocked )
	{
#ifdef DEBUG_AUTOPLUGIN_LOCK
		LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeSession pluginMutex.unlock" );
#endif // DEBUG_AUTOPLUGIN_LOCK
		m_Plugin.unlockPlugin();
	}

	return false;
}

//============================================================================ 
void PluginSessionMgr::removeAllSessions( bool testSessionsOnly )
{
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeAllSessions autoLock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock pluginMutexLock( &m_Plugin );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeAllSessions autoLock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	for( auto iter = m_aoSessions.begin(); iter != m_aoSessions.end(); ++iter )
	{
		PluginSessionBase* session = (*iter).second;

		if( ( false == testSessionsOnly )
			|| ( ( true == testSessionsOnly ) && session->isInTest() ) )
		{
			delete session;
			m_aoSessions.erase(iter);
		}
		else
		{
			++iter;
		}
	}

#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogMsg( LOG_VERBOSE, "PluginSessionMgr::removeAllSessions autoLock destroy" );
#endif // DEBUG_AUTOPLUGIN_LOCK
}
