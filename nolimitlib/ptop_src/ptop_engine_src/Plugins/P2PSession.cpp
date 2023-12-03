//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PSession.h"
#include <PktLib/VxPktHdr.h>

//============================================================================
P2PSession::P2PSession( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType )
: PluginSessionBase( sktBase, netIdent, pluginType )
, m_iOutstandingAckCnt( 0 )
, m_bSendingPkts( false )
, m_VideoCastPkt( 0 )
{
	setSessionType(ePluginSessionTypeP2P);
}

//============================================================================
P2PSession::P2PSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType )
: PluginSessionBase( lclSessionId, sktBase, netIdent, pluginType )
, m_iOutstandingAckCnt( 0 )
, m_bSendingPkts( false )
, m_VideoCastPkt( 0 )
{
	setSessionType(ePluginSessionTypeP2P);
}

//============================================================================
P2PSession::~P2PSession()
{
	delete m_VideoCastPkt;
}
