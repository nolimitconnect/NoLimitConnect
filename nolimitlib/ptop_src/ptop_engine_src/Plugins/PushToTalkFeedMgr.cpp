//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PushToTalkFeedMgr.h"
#include "PluginBase.h"
#include "PluginMgr.h"
#include "PluginSessionMgr.h"
#include "P2PSession.h"
#include "PluginCamServer.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <MediaToolsLib/OpusAudioDecoder.h>

#include <PktLib/PktsPushToTalk.h>
#include <NetLib/VxSktBase.h>

#include <memory.h>

//#define DEBUG_AUTOPLUGIN_LOCK 1

//============================================================================
PushToTalkFeedMgr::PushToTalkFeedMgr( P2PEngine& engine, PluginBase& plugin, PluginSessionMgr& sessionMgr )
: m_Engine( engine )
, m_Plugin( plugin )
, m_PluginMgr( engine.getPluginMgr() )
, m_SessionMgr( sessionMgr )
{
}

//============================================================================
bool PushToTalkFeedMgr::fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk, std::shared_ptr<VxSktBase>& sktBase )
{
	return enableAudioCapture( enableTalk, onlineId, eAppModulePushToTalk, sktBase );
}

//============================================================================
bool PushToTalkFeedMgr::enableAudioCapture( bool enable, VxGUID& onlineId, EAppModule appModule, std::shared_ptr<VxSktBase>& sktBase )
{
	if( enable && sktBase )
	{
		if( m_TxOnlineIdList.addGuidIfDoesntExist( onlineId ) )
		{
			if( sendPushToTalkReq( onlineId, sktBase ) )
			{
				if( !m_AudioPktsRequested )
				{
					//LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::enableCapture eMediaInputAudioPkts %d\n", enable );
					m_AudioPktsRequested = true;
					m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, appModule, true );
					m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceMicrophone( eAppModulePushToTalk, onlineId, true );
				}

				updatePushToTalkStatus( onlineId );
			}
			else
			{
				m_Engine.getToGui().toGuiPushToTalkStatus( onlineId, ePushToTalStatuskNoConnection );
				LogModule( eLogMediaStream, LOG_VERBOSE, "PushToTalkFeedMgr::enableCapture failed sendPushToTalkReq %s", m_Engine.describeUser( onlineId ).c_str() );
			}
		}
		else
		{
            LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::enableCapture true GUID already in list %s", m_Engine.describeUser( onlineId ).c_str() );
		}

		m_SessionMgr.findOrCreateTxSessionWithOnlineId( onlineId, sktBase, false );
		sendPushToTalkStart( onlineId, sktBase );
		return true;
	}
	else
	{
		m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceMicrophone( eAppModulePushToTalk, onlineId, false );
		sendPushToTalkStop( onlineId, sktBase );
		removePushToTalkUser( onlineId, true );
		return true;
	}

    LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::enableCapture %d done %s", enable, m_Engine.describeUser( onlineId ).c_str() );
	return false;
}

//============================================================================
void PushToTalkFeedMgr::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	std::map<VxGUID, PluginSessionBase*>& sessionList = m_SessionMgr.getSessions();
	for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* sessionBase = iter->second;
		if( sessionBase->isRxSession() )
		{
			AudioJitterBuffer& jitterBuf = ((PluginSessionBase*)iter->second)->getJitterBuffer();
			//LogMsg( LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail jitterBuf.lockResource sessionIdx %d\n", sessionIdx );
			jitterBuf.lockResource();
			char* audioBuf = jitterBuf.getBufToRead();
			if( audioBuf )
			{
				//LogMsg( LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail playAudio %d\n", sessionIdx );
				m_PluginMgr.getEngine().getMediaProcessor().playAudio( (int16_t*)audioBuf, MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN );
				//VxGUID onlineId = iter->first; // local session id
				VxGUID onlineId = ((PluginSessionBase*)iter->second)->getSendToId();
				// processor mutex was already locked by call to processor fromGuiAudioOutSpaceAvail which calls callbackAudioOutSpaceAvail
				//LogMsg( LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail processFriendAudioFeed %d\n", sessionIdx );
				m_PluginMgr.getEngine().getMediaProcessor().processFriendAudioFeed( onlineId, (int16_t*)audioBuf, MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN, true );
			}

			//LogMsg( LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail jitterBuf.unlockResource sessionIdx %d\n", sessionIdx );
			jitterBuf.unlockResource();
		}
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogMediaStream,  LOG_INFO, "PushToTalkFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PushToTalkFeedMgr::onPktPushToTalkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
	bool allowed = netIdent->isMyAccessAllowedFromHim( ePluginTypePushToTalk ) && netIdent->isHisAccessAllowedFromMe( ePluginTypePushToTalk );
	if( !allowed )
	{
		// should this be a hack offence?
        LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr user %s insufficient permaision", m_Engine.describeUser( srcOnlineId ).c_str() );
		return;
	}

	if( m_RxOnlineIdList.addGuidIfDoesntExist( netIdent->getMyOnlineId() ) )
	{
		m_SessionMgr.findOrCreateRxSessionWithOnlineId( netIdent->getMyOnlineId(), sktBase, false );

		//LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::enableCapture eMediaInputMixer %d\n", enable );
		if( !m_MixerInputRequesed )
		{
			m_MixerInputRequesed = true;
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, eAppModulePushToTalk, true );
            m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceSpeaker( eAppModulePushToTalk, srcOnlineId, true );
			//LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::enableCapture done\n" );
		}

        updatePushToTalkStatus( srcOnlineId );
	}
	else
	{
        LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::enableCapture true GUID already in list %s", m_Engine.describeUser( srcOnlineId ).c_str() );
	}
}

//============================================================================
void PushToTalkFeedMgr::onPktPushToTalkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
}

//============================================================================
void PushToTalkFeedMgr::onPktPushToTalkStart( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
    if( addPushToTalkUser( srcOnlineId, sktBase, true ) )
	{
        updatePushToTalkStatus( srcOnlineId );
	}
	//m_Engine.getToGui().toGuiPushToTalkStop( netIdent->getMyOnlineId() );
}

//============================================================================
void PushToTalkFeedMgr::onPktPushToTalkStop( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
    if( removePushToTalkUser( srcOnlineId ) )
	{
        updatePushToTalkStatus( srcOnlineId );
	}
}

//============================================================================
void PushToTalkFeedMgr::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
	bool allowed = netIdent->isMyAccessAllowedFromHim( ePluginTypePushToTalk ) && netIdent->isHisAccessAllowedFromMe( ePluginTypePushToTalk );
	if( !allowed )
	{
		// should this be a hack offence?
        LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr user %s insufficient permaision", m_Engine.describeUser( srcOnlineId ).c_str() );
		return;
	}

	if( m_Plugin.isAppPaused() )
	{
		return;
	}

#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq PluginBase::AutoPluginLock autoLock start" );
#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq PluginBase::AutoPluginLock autoLock done" );
#endif // DEBUG_AUTOPLUGIN_LOCK

	PluginSessionMgr::SessionIter iter;
	std::map<VxGUID, PluginSessionBase*>& sessionList = m_SessionMgr.getSessions();
	for( iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* poSession = iter->second;
        if( poSession->isRxSession() && srcOnlineId == poSession->getSendToId() )
		{
			AudioJitterBuffer& jitterBuf = poSession->getJitterBuffer();
			//LogMsg( LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq jitterBuf.lockResource" );
			jitterBuf.lockResource();

			char* audioBuf = poSession->getJitterBuffer().getBufToFill();
			if( audioBuf )
			{
				PktVoiceReq* pktReq = (PktVoiceReq*)pktHdr;
				std::vector<uint16_t> opusEncodedLenList;
				opusEncodedLenList.push_back( pktReq->getFrame1Len() );
				opusEncodedLenList.push_back( pktReq->getFrame2Len() );
				opusEncodedLenList.push_back( pktReq->getFrame3Len() );
				opusEncodedLenList.push_back( pktReq->getFrame4Len() );
				bool result = poSession->getAudioDecoder()->decodeToPcmData( pktReq->getCompressedData(), opusEncodedLenList, (int16_t*)audioBuf, (int32_t)MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN );
				if( !result )
				{
					LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq failed to decode opus" );
				}
			}
			else
			{
				LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq failed to get jitter buffer" );
			}

			//LogMsg( LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq jitterBuf.unlockResource" );
			jitterBuf.unlockResource();
			break;
		}
	}

#ifdef DEBUG_AUTOPLUGIN_LOCK
	LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::onPktPushToTalkReq PluginBase::AutoPluginLock autoLock destroy" );
#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PushToTalkFeedMgr::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	onPktPushToTalkReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PushToTalkFeedMgr::callbackOpusPkt( PktVoiceReq * pktOpusAudio )
{
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	PluginSessionMgr::SessionIter iter;
	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* poSession = iter->second;
		if( poSession->isTxSession() )
		{
			bool result = m_Plugin.txPacket( poSession->getSendToId(), poSession->getSkt(), pktOpusAudio );
			if( false == result )
			{
				// TODO handle lost connection
			}
		}
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void PushToTalkFeedMgr::onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )
{
	removePushToTalkUser( netIdent->getMyOnlineId() );
	//m_PushToTalkFeedMgr.onContactWentOffline( netIdent, sktBase );
	//m_PushToTalkFeedMgr.fromGuiStopPluginSession( false, netIdent );
	//m_PluginSessionMgr.onContactWentOffline( netIdent, sktBase );
}

//============================================================================
bool PushToTalkFeedMgr::addPushToTalkUser( VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase, bool rxOnly )
{
	bool userAdded{ false };
	if( m_RxOnlineIdList.addGuidIfDoesntExist( onlineId ) )
	{
		userAdded = true;
		m_SessionMgr.findOrCreateRxSessionWithOnlineId( onlineId, sktBase, false );

		if( 1 == m_RxOnlineIdList.size() )
		{
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, eAppModulePushToTalk, false );
			m_AudioPktsRequested = false;
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, eAppModulePushToTalk, false );
			m_MixerInputRequesed = false;
			m_Engine.getToGui().getAudioRequests().toGuiWantSpeakerOutput( eAppModulePushToTalk, false );
			LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::removePushToTalkUser false done" );
		}
	}

	if( !rxOnly && m_TxOnlineIdList.addGuidIfDoesntExist( onlineId ) )
	{
		userAdded = true;
		m_SessionMgr.findOrCreateTxSessionWithOnlineId( onlineId, sktBase, false );

		if( 1 == m_TxOnlineIdList.size() )
		{
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, eAppModulePushToTalk, false );
			m_AudioPktsRequested = false;
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, eAppModulePushToTalk, false );
			m_MixerInputRequesed = false;
			m_Engine.getToGui().getAudioRequests().toGuiWantSpeakerOutput( eAppModulePushToTalk, false );
			LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::removePushToTalkUser false done" );
		}
	}

	return userAdded;
}

//============================================================================
bool PushToTalkFeedMgr::removePushToTalkUser( VxGUID& onlineId, bool txOnly )
{
	bool userRemoved{ false };
	if( m_TxOnlineIdList.removeGuid( onlineId ) )
	{
		userRemoved = true;
		m_SessionMgr.removeTxSessionByOnlineId( onlineId, false );

		if( 0 == m_TxOnlineIdList.size() )
		{
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, eAppModulePushToTalk, false );
			m_AudioPktsRequested = false;
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, eAppModulePushToTalk, false );
			m_MixerInputRequesed = false;
			m_Engine.getToGui().getAudioRequests().toGuiWantSpeakerOutput( eAppModulePushToTalk, false );
			LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::removePushToTalkUser false done" );
		}
	}

	if( !txOnly && m_RxOnlineIdList.removeGuid( onlineId ) )
	{
		userRemoved = true;
		m_SessionMgr.removeRxSessionByOnlineId( onlineId, false );

		if( 0 == m_RxOnlineIdList.size() )
		{
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, eAppModulePushToTalk, false );
			m_AudioPktsRequested = false;
			m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, eAppModulePushToTalk, false );
			m_MixerInputRequesed = false;
			m_Engine.getToGui().getAudioRequests().toGuiWantSpeakerOutput( eAppModulePushToTalk, false );
			LogModule( eLogMediaStream, LOG_INFO, "PushToTalkFeedMgr::removePushToTalkUser false done" );
		}
	}

	return userRemoved;
}

//============================================================================
bool PushToTalkFeedMgr::sendPushToTalkStart( VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase )
{
	PktPushToTalkStart pktStop;
	return m_Plugin.txPacket( onlineId, sktBase, &pktStop );
}

//============================================================================
bool PushToTalkFeedMgr::sendPushToTalkStop( VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase )
{
	PktPushToTalkStop pktStop;
	return m_Plugin.txPacket( onlineId, sktBase, &pktStop );
}

//============================================================================
bool PushToTalkFeedMgr::sendPushToTalkReq( VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase )
{
	PktPushToTalkReq pktReq;
	return m_Plugin.txPacket( onlineId, sktBase, &pktReq );
}

//============================================================================
void PushToTalkFeedMgr::updatePushToTalkStatus( VxGUID& onlineId )
{
	EPushToTalkStatus status{ ePushToTalkStatusInvalid };
	if( !m_Engine.isUserConnected( onlineId ) )
	{
		status = ePushToTalStatuskNoConnection;
		m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceMicrophone( eAppModulePushToTalk, onlineId, false );
		m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceSpeaker( eAppModulePushToTalk, onlineId, false );
	}
	else
	{
		bool isTx = m_TxOnlineIdList.doesGuidExist( onlineId );
		bool isRx = m_RxOnlineIdList.doesGuidExist( onlineId );
		if( isTx && isRx )
		{
			status = ePushToTalkStatusDuplexEnabled;
		}
		else if( isTx )
		{
			status = ePushToTalkStatusTxEnabled;
		}
		else if( isRx )
		{
			status = ePushToTalkStatusRxEnabled;
		}
		else
		{
			status = ePushToTalkStatusNotActive;
		}

		m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceMicrophone( eAppModulePushToTalk, onlineId, isTx );
		m_Engine.getToGui().getAudioRequests().toGuiWantUserVoiceSpeaker( eAppModulePushToTalk, onlineId, isRx );
	}	

	m_Engine.getToGui().toGuiPushToTalkStatus( onlineId, status );
}

