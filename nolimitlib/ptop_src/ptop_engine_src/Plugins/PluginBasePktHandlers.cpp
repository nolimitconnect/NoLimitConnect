//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBase.h"

#include <ptop_src/ptop_engine_src/ThumbMgr/ThumbMgr.h>

//============================================================================
void PluginBase::onInvalidRxedPacket( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, const char* msg )
{
    // TODO proper invalid packet handling
    LogMsg( LOG_INFO, "PluginBase::onInvalidRxedPacket plugin %s user %s", DescribePluginType( getPluginType() ), netIdent->getOnlineName() );
}

//============================================================================
void PluginBase::onPktUserConnect( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_INFO, "PluginBase::onPktUserConnect" );
}

//============================================================================
void PluginBase::onPktUserDisconnect( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_INFO, "PluginBase::onPktUserConnect" );
}

//============================================================================
void PluginBase::onPktPluginOfferReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktPluginOfferReq" );
}

//============================================================================
//! packet with remote users reply to offer
void PluginBase::onPktPluginOfferReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktPluginOfferReply" );
}

//============================================================================
void PluginBase::onPktSessionStartReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktSessionStartReq" );
}
//============================================================================
void PluginBase::onPktSessionStartReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktSessionStartReply" );
}
//============================================================================
void PluginBase::onPktSessionStopReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktSessionStopReq" );
}

//============================================================================
void PluginBase::onPktSessionStopReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktSessionStopReply" );
}

//============================================================================
void PluginBase::onPktMyPicSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktMyPicSendReq" );
}

//============================================================================
void PluginBase::onPktMyPicSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktMyPicSendReply" );
}

//============================================================================
void PluginBase::onPktWebServerPicChunkTx( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktWebServerPicChunkTx" );
}

//============================================================================
void PluginBase::onPktWebServerPicChunkAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktWebServerPicChunkAck" );
}

//============================================================================
void PluginBase::onPktWebServerGetChunkTx( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktWebServerGetChunkTx" );
}

//============================================================================
void PluginBase::onPktWebServerGetChunkAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktWebServerGetChunkAck" );
}

//============================================================================
void PluginBase::onPktWebServerPutChunkTx( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktWebServerPutChunkTx" );
}

//============================================================================
void PluginBase::onPktWebServerPutChunkAck( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
	LogMsg( LOG_ERROR, "PluginBase::onPktWebServerPutChunkAck" );
}

//============================================================================
void PluginBase::onPktThumbGetReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbGetReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbGetReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbGetReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbSendReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbSendReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbSendReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbSendReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbChunkReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbChunkReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbChunkReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbChunkReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbGetCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbGetCompleteReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbGetCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbGetCompleteReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbSendCompleteReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbSendCompleteReq( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbSendCompleteReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbSendCompleteReply( sktBase, pktHdr, netIdent );
}

//============================================================================
void PluginBase::onPktThumbXferErr( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    m_ThumbXferMgr.onPktThumbXferErr( sktBase, pktHdr, netIdent );
}
