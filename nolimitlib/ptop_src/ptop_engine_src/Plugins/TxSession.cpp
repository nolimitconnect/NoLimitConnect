//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TxSession.h"

//============================================================================
TxSession::TxSession( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType )
: PluginSessionBase( sktBase, netIdent, pluginType )
, m_iOutstandingAckCnt(0)
, m_bSendingPkts( false )
{
	setSessionType(ePluginSessionTypeTx);
}

//============================================================================
TxSession::TxSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType )
: PluginSessionBase( lclSessionId, sktBase, netIdent, pluginType )
, m_iOutstandingAckCnt(0)
, m_bSendingPkts( false )
{
	setSessionType(ePluginSessionTypeTx);
}

//============================================================================
TxSession::~TxSession()
{
}
