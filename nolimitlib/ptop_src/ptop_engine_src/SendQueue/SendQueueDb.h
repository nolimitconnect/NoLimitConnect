#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SendQueInfo.h"

#include <CoreLib/DbBase.h>

class SendQueueDb : public DbBase
{
public:
	SendQueueDb();
	virtual ~SendQueueDb() = default;

	void						sendQueueDbStartup( std::string& dbName );

	void						lockSendQueueDb( void )				{ m_SendQueueDbMutex.lock(); }
	void						unlockSendQueueDb( void )			{ m_SendQueueDbMutex.unlock(); }

	virtual RCODE				onCreateTables( int iDbVersion );
	virtual RCODE				onDeleteTables( int iOldVersion );

	void 						updateSendQueueInfo( SendQueInfo& sendQueInfo );
    void						removeSendQueueInfo( GroupieId groupieId );

	void						getAllQueInfo( std::vector<SendQueInfo>& sendStateList );
	void						purgeAllSendQueue( void ); 

protected:
	VxMutex						m_SendQueueDbMutex;
};

