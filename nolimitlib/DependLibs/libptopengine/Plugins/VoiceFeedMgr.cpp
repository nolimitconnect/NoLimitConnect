//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VoiceFeedMgr.h"
#include "PluginBase.h"
#include "PluginMgr.h"
#include "PluginSessionMgr.h"
#include "P2PSession.h"
#include "PluginCamServer.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <MediaToolsLib/OpusAudioDecoder.h>

#include <CoreLib/VxDebug.h>
#include <PktLib/PktVoiceReq.h>
#include <NetLib/VxSktBase.h>

#include <memory.h>

//#define DEBUG_AUTOPLUGIN_LOCK 0 // define to enable lock logging

//============================================================================
VoiceFeedMgr::VoiceFeedMgr( P2PEngine& engine, PluginBase& plugin, PluginSessionMgr& sessionMgr )
: m_Engine( engine )
, m_Plugin( plugin )
, m_PluginMgr( engine.getPluginMgr() )
, m_SessionMgr( sessionMgr )
, m_Enabled( false )
, m_CamServerEnabled( false )
{
}

//============================================================================
bool VoiceFeedMgr::fromGuiStartPluginSession( bool pluginIsLocked, EAppModule appModule, VxGUID onlineId, bool wantAudioCapture )
{
	enableAudioCapture( true, onlineId, appModule, wantAudioCapture );
    return true;
}

//============================================================================
void VoiceFeedMgr::fromGuiStopPluginSession( bool pluginIsLocked, EAppModule appModule, VxGUID onlineId, bool wantAudioCapture )
{
	enableAudioCapture( false, onlineId, appModule, wantAudioCapture );
}

//============================================================================
void VoiceFeedMgr::enableAudioCapture( bool enable, VxGUID& onlineId, EAppModule appModule, bool wantAudioCapture )
{
	int prevNeedTxCount = m_Plugin.needVoiceTxCount( m_Plugin.getPluginType() );
	bool isMyself = onlineId == m_PluginMgr.getEngine().getMyOnlineId(); 
	if( enable )
	{
		if( !m_Plugin.addVoicePairTx( m_Plugin.getPluginType(), onlineId ) )
		{
            LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::enableAudioCapture add GUID already in list %s", m_Engine.describeUser( onlineId ).c_str() );
		}
	}
	else
	{
		if( !m_Plugin.removeVoicePairTx( m_Plugin.getPluginType(), onlineId ) )
		{
			LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::enableAudioCapture remove GUID not found %s", m_Engine.describeUser( onlineId ).c_str() );
		}
	}

	int needTxCount = m_Plugin.needVoiceTxCount( m_Plugin.getPluginType() );
	m_Plugin.updateRequestMicrophone( m_Plugin.getPluginType(), prevNeedTxCount, needTxCount );
}

//============================================================================
void VoiceFeedMgr::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	if( false == m_Enabled )
	{
		if( LogEnabled( eLogVoice ) ) LogModule( eLogVoice, LOG_DEBUG, "VoiceFeedMgr::%s  not enabled %s", __func__, m_Plugin.describePlugin() );
		return;
	}

	PktVoiceReq* pktReq = (PktVoiceReq*)pktHdr;
	VxGUID srcOnlineId = pktReq->getSrcOnlineId();

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice,  LOG_INFO, "VoiceFeedMgr::onPktVoiceReq PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* poSession = iter->second;
		if( poSession->isRxSession() && srcOnlineId == poSession->getSendToId() )
		{
			if( m_Plugin.isFirstVoicePairRx( m_Plugin.getPluginType(), srcOnlineId ) )
			{
				if( LogEnabled( eLogVoice ) ) LogModule( eLogVoice, LOG_DEBUG, "VoiceFeedMgr::%s  %s", __func__, m_Engine.describeUser( srcOnlineId ).c_str() );

				AudioJitterBuffer& jitterBuf = poSession->getJitterBuffer();
				jitterBuf.lockResource();

				char* audioBuf = poSession->getJitterBuffer().getBufToFill();
				if( audioBuf )
				{
					std::vector<uint16_t> opusEncodedLenList;
					opusEncodedLenList.emplace_back( pktReq->getFrame1Len() );
					opusEncodedLenList.emplace_back( pktReq->getFrame2Len() );
					opusEncodedLenList.emplace_back( pktReq->getFrame3Len() );
					opusEncodedLenList.emplace_back( pktReq->getFrame4Len() );
					bool result = poSession->getAudioDecoder()->decodeToPcmData( pktReq->getCompressedData(), opusEncodedLenList, (int16_t*)audioBuf, (int32_t)MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN );
					if( !result )
					{
						LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq failed to decode opus" );
					}
				}
				else
				{
					LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq failed to get jitter buffer" );
				}

				//LogMsg( LOG_INFO, "VoiceFeedMgr::onPktVoiceReq jitterBuf.unlockResource\n" );
				jitterBuf.unlockResource();
				break;
			}
			else if( LogEnabled( eLogVoice ) )
			{
				LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::%s not first rx pair for online id %s", __func__, m_Engine.describeUser( srcOnlineId ).c_str() );
			}
		}
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void VoiceFeedMgr::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		AudioJitterBuffer& jitterBuf = ((PluginSessionBase*)iter->second)->getJitterBuffer();
		//LogMsg( LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail jitterBuf.lockResource sessionIdx %d\n", sessionIdx );
		jitterBuf.lockResource();
		char * audioBuf = jitterBuf.getBufToRead();
		if( audioBuf )
		{
			//LogMsg( LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail playAudio %d\n", sessionIdx );
			m_PluginMgr.getEngine().getMediaProcessor().playAudio( (int16_t *)audioBuf, MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN );
			//VxGUID onlineId = iter->first; // local session id
			VxGUID onlineId = ((PluginSessionBase*)iter->second)->getSendToId();
			// processor mutex was already locked by call to processor fromGuiAudioOutSpaceAvail which calls callbackAudioOutSpaceAvail
			//LogMsg( LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail processFriendAudioFeed %d\n", sessionIdx );
			m_PluginMgr.getEngine().getMediaProcessor().processFriendAudioFeed( onlineId, (int16_t *)audioBuf, MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN, true );
		}
		else
		{
			LogModule( eLogVoice, LOG_VERBOSE, "VoiceFeedMgr::callbackAudioOutSpaceAvail no buffer to read" );
		}

		//LogMsg( LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail jitterBuf.unlockResource sessionIdx %d\n", sessionIdx );
		jitterBuf.unlockResource();
		//sessionIdx++;
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice,  LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void VoiceFeedMgr::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	if( LogEnabled( eLogVoice ) )
	{
		VxGUID srcOnlineId = pktHdr->getSrcOnlineId();
		LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::%s from %s", __func__, m_Engine.describeUser( srcOnlineId ).c_str() );
	}
}

//============================================================================
void VoiceFeedMgr::callbackOpusPkt( PktVoiceReq* pktOpusAudio )
{
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* poSession = iter->second;
		if( poSession->isRxSession() )
		{
			if( m_Plugin.isFirstVoicePairTx( m_Plugin.getPluginType(), poSession->getSendToId() ) )
			{
				bool result = m_Plugin.txPacket( poSession->getSendToId(), poSession->getSkt(), pktOpusAudio );
				if( LogEnabled( eLogVoice ) )
				{
					LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::%s result %d to %s", __func__, result, m_Engine.describeUser( poSession->getSendToId() ).c_str() );
				}

				if( false == result )
				{
					// TODO handle lost connection
				}
			}
			else if( LogEnabled( eLogVoice ) )
			{
				LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::%s Plugin %s not first tx pair for id %s", __func__, 
					DescribePluginType( m_Plugin.getPluginType() ), m_Engine.describeUser(poSession->getSendToId()).c_str());
			}
		}
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogVoice, LOG_INFO, "VoiceFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void VoiceFeedMgr::stopAllSessions( EAppModule appModule, EPluginType pluginType )
{
	if( pluginType == m_Plugin.getPluginType() )
	{
		std::vector<VxGUID> onlineIdList;
		m_Plugin.getVoiceTxList( m_Plugin.getPluginType(), onlineIdList );
		PluginBase::AutoPluginLock autoLock( &m_Plugin );
		for( auto& onlineId : onlineIdList )
		{
			fromGuiStopPluginSession( true, appModule, onlineId );
		}

		m_Plugin.getVoiceRxList( m_Plugin.getPluginType(), onlineIdList );
		for( auto& onlineId : onlineIdList )
		{
			fromGuiStopPluginSession( true, appModule, onlineId );
		}
	}
}
