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

//============================================================================
VirtFileMgr& GetVirtFileMgr()
{
	static VirtStreamMgr virtStreamMgr(GetPtoPEngine());
	return virtStreamMgr;
}

//============================================================================
VirtStreamMgr& GetVirtStreamMgr( void )
{
	return (VirtStreamMgr&)GetVirtFileMgr;
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

	m_LiveStream.m_StreamAssetInfo = assetInfo;
	m_LiveStream.m_StreamAssetInfo.setIsStream( true );
	m_LiveStream.m_StreamSessionId = lclSessionId;
	if( !m_LiveStream.setConnection( m_Engine.getConnectIdListMgr().findBestUserOnlineConnection( assetInfo.getDestUserId() ) ) )
	{
		return false;
	}

	m_Plugin.wantFileXferCallback( this, true );
	if( !m_Plugin.startStream( m_LiveStream.getConnection(), m_LiveStream.m_StreamAssetInfo, m_LiveStream.m_StreamSessionId ) )
	{
		m_Plugin.wantFileXferCallback( this, false );
		return false;
	}

	return true;
}

//============================================================================
bool VirtStreamMgr::sendStreamSeek( int64_t newPos )
{
	return false;
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

}

//============================================================================
void VirtStreamMgr::onPktFileChunkReq			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{

}

//============================================================================
void VirtStreamMgr::onPktFileGetCompleteReply	( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{

}

//============================================================================
void VirtStreamMgr::onPktFileShareErr			( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{

}

//============================================================================
void VirtStreamMgr::onPktStreamCtrlReply		( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{

}
