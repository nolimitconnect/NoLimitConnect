//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VirtStreamMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <Plugins/PluginMgr.h>

#include <CoreLib/VFile.h>
#include <CoreLib/VxFileUtil.h>

#include <NetLib/VxSktBase.h>

#include <PktLib/PktsStreamCtrl.h>

//============================================================================
VirtFileMgr& GetVirtFileMgr()
{
	static VirtStreamMgr virtStreamMgr(GetPtoPEngine());
	return virtStreamMgr;
}

//============================================================================
VirtStreamMgr& GetVirtStreamMgr( void )
{
	return (VirtStreamMgr&)GetVirtFileMgr();
}

//============================================================================
VirtStreamMgr::VirtStreamMgr( P2PEngine& engine )
	: m_Engine( engine )
	, m_Plugin( *((PluginFileShareClient*)engine.getPluginMgr().getPlugin( ePluginTypeFileShareClient ) ) )
{
}

//============================================================================
bool VirtStreamMgr::fromGuiPlayStream( AssetBaseInfo& assetInfo, VxGUID lclSessionId, int pos0to100000 )
{
	// called just before the media player starts playing stream

	lockSteamMgr();
	m_LiveStream.m_StreamAssetInfo = assetInfo;
	m_LiveStream.m_StreamAssetInfo.setIsStream( true );
	m_LiveStream.m_StreamSessionId = lclSessionId;
	if( !m_LiveStream.setConnection( m_Engine.getConnectIdListMgr().findBestUserOnlineConnection( assetInfo.getDestUserId() ) ) )
	{
		unlockSteamMgr();
		return false;
	}

	unlockSteamMgr();
	setIsPlaying( true );

	m_Plugin.wantFileXferCallback( this, true );
	if( !m_Plugin.startStream( m_LiveStream.getConnection(), m_LiveStream.m_StreamAssetInfo, m_LiveStream.m_StreamSessionId ) )
	{
		m_Plugin.wantFileXferCallback( this, false );
		return false;
	}

	return true;
}

//============================================================================
bool VirtStreamMgr::waitForStream( int64_t fileOffset, int64_t readLen )
{
	bool result{ false };
	const double maxTimeoutMs = 20000;
	//const double maxTimeoutMs = 800000; // temp for debug
	VxTimer timer;
	while( timer.elapsedMs() < maxTimeoutMs && !m_LiveStream.m_Error )
	{
		if( !getIsPlaying() )
		{
			LogMsg( LOG_WARN, "VirtStreamMgr::%s play steam stopped", __func__ );
			return false;
		}

		if( readLen == m_LiveStream.m_StreamCache.hasData( fileOffset, readLen ) )
		{
			result = true;
			break;
		}

		if( readLen == m_LiveStream.m_FileTail.hasData( fileOffset, readLen ) )
		{
			result = true;
			break;
		}

		if( !m_LiveStream.isConnected() )
		{
			LogMsg( LOG_WARN, "VirtStreamMgr::%s user %s is no longer connected", __func__,
					m_Engine.describeUser( m_LiveStream.m_StreamAssetInfo.getDestUserId() ).c_str() );
			return false;
		}

		if( !m_LiveStream.m_VFile )
		{
			LogMsg( LOG_WARN, "VirtStreamMgr::%s file was closed", __func__ );
			return false;
		}

		VxSleep( 300 );
	}
	
	if( !result )
	{
		LogModule( eLogMediaStream, LOG_ERROR, "VirtStreamMgr::%s timeout waiting for stream file %s at offs%" PRId64 " len%s" PRId64, __func__,
				   m_LiveStream.m_StreamAssetInfo.getAssetName().c_str(), fileOffset, readLen );
	}

	return result;
}

//============================================================================
void VirtStreamMgr::onPlaybackStopped( VxGUID& feedId )
{
	if( feedId == m_LiveStream.m_StreamSessionId )
	{
		onStreamStop();
	}
}

//============================================================================
void VirtStreamMgr::onPlaybackEnded( VxGUID& feedId )
{
	if( feedId == m_LiveStream.m_StreamSessionId )
	{
		onStreamStop();		
	}
}

//============================================================================
void VirtStreamMgr::onStreamStop( void )
{
	setIsPlaying( false );
	lockSteamMgr();
	m_LiveStream.clear();
	unlockSteamMgr();
}

//============================================================================
bool VirtStreamMgr::sendStreamSeek( int64_t newPos )
{
	PktStreamCtrlReq pktReq;
	pktReq.setSrcOnlineId( m_Engine.getMyOnlineId() );
	pktReq.setDestOnlineId( m_LiveStream.m_StreamAssetInfo.getDestUserId() );
	pktReq.setPluginNum( ePluginTypeFileShareServer );

	lockSteamMgr();
	pktReq.setLclSessionId( m_LiveStream.m_StreamSessionId );
	pktReq.setRmtSessionId( m_LiveStream.m_ServerSessionId );
	pktReq.setAssetId(  m_LiveStream.m_StreamAssetInfo.getAssetUniqueId() );
	unlockSteamMgr();

	pktReq.setStartOffset( newPos );
	pktReq.setStreamCtrl( eStreamSeek );
	return m_LiveStream.getConnection()->txPacketWithDestId( &pktReq ) == 0;
}

//============================================================================
void VirtStreamMgr::onFileXferPktRxed( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	switch( pktHdr->getPktType() )
	{
	case  PKT_TYPE_FILE_GET_REPLY:
		onPktFileGetReply( sktBase, pktHdr );
		break;

	case  PKT_TYPE_FILE_CHUNK_REQ:
		onPktFileChunkReq( sktBase, pktHdr );
		break;

	case  PKT_TYPE_FILE_GET_COMPLETE_REPLY:
		onPktFileGetCompleteReply( sktBase, pktHdr );
		break;

	case  PKT_TYPE_FILE_SHARE_ERR:
		onPktFileShareErr( sktBase, pktHdr );
		break;

	case  PKT_TYPE_STREAM_CTRL_REPLY:	
		onPktStreamCtrlReply( sktBase, pktHdr );
		break;

	default:
		LogMsg( LOG_ERROR, "%s unhandled pkt %s", __func__, pktHdr->describePktHdr().c_str() );
	}
}

//============================================================================
void VirtStreamMgr::onPktFileGetReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	PktFileGetReply* pktReply = (PktFileGetReply*)pktHdr;
	VxGUID lclSessionId = pktReply->getLclSessionId();
	VxGUID rmtSessionId = pktReply->getRmtSessionId();
	if( rmtSessionId != m_LiveStream.m_StreamSessionId )
	{
		return;
	}

	if( lclSessionId.isVxGUIDValid() )
	{
		lockSteamMgr();
		m_LiveStream.m_ServerSessionId = lclSessionId;
		unlockSteamMgr();
	}
}

//============================================================================
void VirtStreamMgr::onPktFileChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	PktFileChunkReq* pktReq = (PktFileChunkReq*)pktHdr;
	VxGUID rmtSessionId = pktReq->getRmtSessionId();
	if( rmtSessionId != m_LiveStream.m_StreamSessionId )
	{
		return;
	}

	m_LiveStream.m_StreamCache.writeData( pktReq->getChunkOffset(), (char*)pktReq->getChunkBuffer(), pktReq->getDataLen() );
}

//============================================================================
void VirtStreamMgr::onPktFileGetCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	PktFileGetCompleteReply* pktReply = (PktFileGetCompleteReply*)pktHdr;
}

//============================================================================
void VirtStreamMgr::onPktFileShareErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	PktFileShareErr* pktReply = (PktFileShareErr*)pktHdr;
}

//============================================================================
void VirtStreamMgr::onPktStreamCtrlReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	PktStreamCtrlReply* pktReply = (PktStreamCtrlReply*)pktHdr;
	if( pktReply->getStreamCtrl() == eStreamReadTail )
	{
		lockSteamMgr();
		m_LiveStream.m_FileTail.writeData( pktReply->getStartOffset(), pktReply->getDataBuf(), pktReply->getEndOffset() - pktReply->getStartOffset() );
		unlockSteamMgr();
	}
}
