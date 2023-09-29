//============================================================================
// Copyright (C) 2022 Brett R. Jones
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
#pragma once

#include "VxHackerRecord.h"

#include <CoreLib/VxMutex.h>
#include <PktLib/VxCommon.h>

#include <vector>
#include <map>

class VxHackerMgr : public IHackReportCallbackInterface
{
public:
	VxHackerMgr();
	virtual ~VxHackerMgr();

	void						reportHackOffense( EHackerLevel hackerLevel, EHackerReason hackerReason, std::string ipAddr, std::string hackDescription ) override;

	void                        addHackOffense( EHackerLevel hackerLevel, EHackerReason	hackerReason, std::string& ipAddr, VxGUID signature = VxGUID::nullVxGUID() );
	bool						isHacker( std::string& ipAddr );

	void						getHackerList( std::vector<VxHackerRecord>& hackerList );

protected:
	std::map<std::string,VxHackerRecord> m_HackerList;
	VxMutex						m_HackListMutex;
};

