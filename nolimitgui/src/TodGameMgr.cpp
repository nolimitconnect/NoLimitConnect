//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TodGameMgr.h"

#include <CoreLib/VxDebug.h>

//============================================================================
void TodGameMgr::toGuiTodGameAction( EPluginType	pluginType, VxGUID& onlineId, ETodGameAction todGameAction )
{
    LogMsg( LOG_DEBUG, "TodGameMgr %s", __func__ );

	for( auto client : m_TodClients )
	{
		client->toGuiTodGameAction( pluginType, onlineId, todGameAction );
	}
}

//========================================================================
void TodGameMgr::wantTodGamCallbacks( TodGameCallback* clientInterface, bool wantCallbacks )
{
	for( auto iter = m_TodClients.begin(); iter != m_TodClients.end(); ++iter )
	{
		TodGameCallback* todInterface = ( *iter );
		if( todInterface == clientInterface )
		{
			if( wantCallbacks )
			{
				// already in list
				return;
			}
			else
			{
				// remove from list
				m_TodClients.erase( iter );
				return;
			}
		}
	}

	if( wantCallbacks )
	{
		m_TodClients.emplace_back( clientInterface );
	}
}
