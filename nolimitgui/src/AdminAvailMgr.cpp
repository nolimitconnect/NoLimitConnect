//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AdminAvailMgr.h"

#include <CoreLib/VxDebug.h>

//============================================================================
bool AdminAvailMgr::toGuiAdminAvail( GroupieId& adminGroupieId, bool adminAvail )
{
	bool wasChanged{ false };
	bool found{ false };
	for( auto& pair : m_AdminList )
	{
		if( pair.first == adminGroupieId )
		{
			found = true;
			if( pair.second != adminAvail )
			{
				pair.second = adminAvail;
				wasChanged = true;
			}

			break;
		}
	}

	if( !found )
	{
		m_AdminList.emplace_back( std::make_pair( adminGroupieId, adminAvail ) );
		wasChanged = true;
	}

	return wasChanged;
}

//============================================================================
bool AdminAvailMgr::isUserAHostAdmin( VxGUID& onlineId )
{
	for( auto& pair : m_AdminList )
	{
		if( pair.first.getHostOnlineId() == onlineId )
		{
			return true;
		}
	}

	return false;
}

//============================================================================
bool AdminAvailMgr::isAdminAvail( VxGUID& onlineId )
{
	bool avail{ false };
	for( auto& pair : m_AdminList )
	{
		if( pair.first.getHostOnlineId() == onlineId )
		{
			if( pair.second )
			{
				avail = true;
			}
		}
	}

	return avail;
}
