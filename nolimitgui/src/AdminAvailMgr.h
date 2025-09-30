#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/GroupieId.h>

#include <vector>

class AdminAvailMgr
{
public:
    AdminAvailMgr() = default;
	virtual ~AdminAvailMgr() = default;

    bool                        toGuiAdminAvail( GroupieId& adminGroupieId, bool adminAvail ); // return true if new or avail state changed

    bool                        isUserAHostAdmin( VxGUID& onlineId );
    bool                        isAdminAvail( VxGUID& onlineId );


    std::vector<std::pair<GroupieId, bool>>  m_AdminList;
};
