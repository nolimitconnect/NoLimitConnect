#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginSessionBase.h"

class TxSession : public PluginSessionBase
{
public:
	TxSession( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType );
	TxSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType );
	virtual ~TxSession();

	void						setOutstandingAckCnt( int cnt )				{ m_iOutstandingAckCnt = cnt; }
	int							getOutstandingAckCnt( void )				{ return m_iOutstandingAckCnt; }
	void						incrementOutstandingAckCnt( void )			{ m_iOutstandingAckCnt++; }
	void						decrementOutstandingAckCnt( void )			{ if( m_iOutstandingAckCnt ) m_iOutstandingAckCnt--; }

	void						setIsSendingPkts( bool isSending )			{ m_bSendingPkts = isSending; }
	int							getIsSendingPkts( void )					{ return m_bSendingPkts; }

protected:
	//=== vars ===//
	int							m_iOutstandingAckCnt; // how many receive acks are outstanding
	bool						m_bSendingPkts;
};
