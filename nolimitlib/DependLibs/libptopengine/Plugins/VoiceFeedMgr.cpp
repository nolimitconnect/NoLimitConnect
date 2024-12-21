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
, m_AudioPktsRequested( false )
, m_MixerInputRequesed( false )
{
}

//============================================================================
void VoiceFeedMgr::fromGuiStartPluginSession( bool pluginIsLocked, EAppModule appModule, VxGUID onlineId, bool wantAudioCapture )
{
	enableAudioCapture( true, onlineId, appModule, wantAudioCapture );
}

//============================================================================
void VoiceFeedMgr::fromGuiStopPluginSession( bool pluginIsLocked, EAppModule appModule, VxGUID onlineId, bool wantAudioCapture )
{
	enableAudioCapture( false, onlineId, appModule, wantAudioCapture );
}

//============================================================================
void VoiceFeedMgr::enableAudioCapture( bool enable, VxGUID& onlineId, EAppModule appModule, bool wantAudioCapture )
{
	bool isMyself = onlineId == m_PluginMgr.getEngine().getMyOnlineId(); 
	if( enable )
	{
		if( m_GuidList.addGuidIfDoesntExist( onlineId ) )
		{
			m_Enabled = true;

			if( ePluginTypeCamServer == m_Plugin.getPluginType() )
			{
				if( isMyself )
				{
					// web cam server.. need audio packets to send out but not mixer input
					m_CamServerEnabled = true;
					if( !m_AudioPktsRequested )
					{
						m_AudioPktsRequested = true;
						m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, appModule, onlineId, true );
					}
				}
				else
				{
					if( !m_MixerInputRequesed )
					{
						m_MixerInputRequesed = true;
						m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, appModule, onlineId, true );
					}
				}
			}
			else
			{
				if( wantAudioCapture && !m_AudioPktsRequested )
				{
					//LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture eMediaInputAudioPkts %d\n", enable );
					m_AudioPktsRequested = true;
					m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, appModule, onlineId, true );
				}

				//LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture eMediaInputMixer %d\n", enable );
				if( !m_MixerInputRequesed )
				{
					m_MixerInputRequesed = true;
					m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, appModule, onlineId, true );
					//LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture done\n" );
				}
			}
		}
		else
		{
            LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture true GUID already in list %s", m_Engine.describeUser( onlineId ).c_str() );
		}
	}
	else
	{
		if( m_GuidList.removeGuid( onlineId ) )
		{
			if( ePluginTypeCamServer == m_Plugin.getPluginType() )
			{
				if( isMyself )
				{
					m_CamServerEnabled = false;
					// web cam server.. need audio packets to send out but not mixer input
					if( m_AudioPktsRequested && ( 0 == m_GuidList.size() ) )
					{
						m_AudioPktsRequested = false;
						m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, appModule, onlineId, false );
					}
				}
				else
				{
					if( m_MixerInputRequesed )
					{
						if(  ( 0 == m_GuidList.size() ) 
							|| ( m_CamServerEnabled && ( 1 == m_GuidList.size() ) ) )
						{
							m_MixerInputRequesed = false;
							m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, appModule, onlineId, false );
						}
					}
				}
			}
			else
			{
				if( 0 == m_GuidList.size() ) 
				{
					m_Enabled = false;

                    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture false eMediaInputAudioPkts %d", enable );
					m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputAudioPkts, appModule, onlineId, false );
					m_AudioPktsRequested = false;
                    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture false eMediaInputMixer %d", enable );
					m_PluginMgr.pluginApiWantMediaInput( m_Plugin.getPluginType(), eMediaInputMixer, appModule, onlineId, false );
					m_MixerInputRequesed = false;
                    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture false done" );
				}
				else
				{
                    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture false GUID list not empty %s", m_Engine.describeUser( onlineId ).c_str() );
				}
			}
		}
		else
		{
            LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::enableCapture false GUID not found %s", m_Engine.describeUser( onlineId ).c_str() );
		}
	}
}

//============================================================================
void VoiceFeedMgr::onPktVoiceReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	if( false == m_Enabled )
	{
		return;
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams,  LOG_INFO, "VoiceFeedMgr::onPktVoiceReq PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	PluginSessionMgr::SessionIter iter;
	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* poSession = iter->second;
		if( netIdent->getMyOnlineId() == poSession->getSendToId() )
		{
			AudioJitterBuffer& jitterBuf = poSession->getJitterBuffer();
			//LogMsg( LOG_INFO, "VoiceFeedMgr::onPktVoiceReq jitterBuf.lockResource\n" );
			jitterBuf.lockResource();

			char * audioBuf = poSession->getJitterBuffer().getBufToFill();
			if( audioBuf )
			{
				PktVoiceReq* pktReq = (PktVoiceReq * )pktHdr;
				std::vector<uint16_t> opusEncodedLenList;
				opusEncodedLenList.push_back( pktReq->getFrame1Len() );
				opusEncodedLenList.push_back( pktReq->getFrame2Len() );
				opusEncodedLenList.push_back( pktReq->getFrame3Len() );
				opusEncodedLenList.push_back( pktReq->getFrame4Len() );
				bool result = poSession->getAudioDecoder()->decodeToPcmData( pktReq->getCompressedData(), opusEncodedLenList, (int16_t *)audioBuf, (int32_t)MY_OPUS_PKT_UNCOMPRESSED_DATA_LEN );
				if( !result )
				{
					LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq failed to decode opus" );
				}
			}
			else
			{
				LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq failed to get jitter buffer" );
			}

			//LogMsg( LOG_INFO, "VoiceFeedMgr::onPktVoiceReq jitterBuf.unlockResource\n" );
			jitterBuf.unlockResource();
			break;
		}
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::onPktVoiceReq PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void VoiceFeedMgr::callbackAudioOutSpaceAvail( int freeSpaceLen )
{
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

    //int sessionIdx = 0;
	PluginSessionMgr::SessionIter iter;
	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( iter = sessionList.begin(); iter != sessionList.end(); ++iter )
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
			LogModule( eLogStreams, LOG_VERBOSE, "VoiceFeedMgr::callbackAudioOutSpaceAvail no buffer to read" );
		}

		//LogMsg( LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail jitterBuf.unlockResource sessionIdx %d\n", sessionIdx );
		jitterBuf.unlockResource();
		//sessionIdx++;
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams,  LOG_INFO, "VoiceFeedMgr::callbackAudioOutSpaceAvail PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void VoiceFeedMgr::onPktVoiceReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
}

//============================================================================
void VoiceFeedMgr::callbackOpusPkt( PktVoiceReq* pktOpusAudio )
{
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock start" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
	PluginBase::AutoPluginLock autoLock( &m_Plugin );
	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock done" );
	#endif // DEBUG_AUTOPLUGIN_LOCK

	std::map<VxGUID, PluginSessionBase*>&	sessionList = m_SessionMgr.getSessions();
	for( auto iter = sessionList.begin(); iter != sessionList.end(); ++iter )
	{
		PluginSessionBase* poSession = iter->second;
		if( false == poSession->isRxSession() )
		{
			bool result = m_Plugin.txPacket( poSession->getSendToId(), poSession->getSkt(), pktOpusAudio );
			if( false == result )
			{
				// TODO handle lost connection
			}
		}
	}

	#ifdef DEBUG_AUTOPLUGIN_LOCK
    LogModule( eLogStreams, LOG_INFO, "VoiceFeedMgr::callbackOpusPkt PluginBase::AutoPluginLock autoLock destroy" );
	#endif // DEBUG_AUTOPLUGIN_LOCK
}

//============================================================================
void VoiceFeedMgr::stopAllSessions( EAppModule appModule, EPluginType pluginType )
{
	if( pluginType == m_Plugin.getPluginType() )
	{
		PluginBase::AutoPluginLock autoLock( &m_Plugin );
		for( auto onlineId : m_GuidList.getGuidList() )
		{
			fromGuiStopPluginSession( true, appModule, onlineId );
		}

		m_GuidList.clearList();
	}
}
