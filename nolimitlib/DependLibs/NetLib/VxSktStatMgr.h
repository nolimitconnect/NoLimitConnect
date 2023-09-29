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

#include "ISktStatCallbackInterface.h"
#include "VxSktStatRecord.h"

#include <CoreLib/VxMutex.h>
#include <PktLib/VxCommon.h>

#include <vector>
#include <map>

class VxSktStatMgr : public ISktStatCallbackInterface
{
public:
	VxSktStatMgr();
	virtual ~VxSktStatMgr();

	virtual void                sktConnected( SOCKET skt ) override;
	virtual void                sktConnected2( SOCKET skt, std::string ipAddr ) override;
	virtual void                sktConnected3( SOCKET skt, std::string ipAddr, ESktType sktType ) override;
	virtual void                sktSetRemoteAddr( SOCKET skt, std::string ipAddr ) override;
	virtual void                sktSetType( SOCKET skt, ESktType sktType ) override;
	virtual void                sktClosed( SOCKET skt ) override;

	bool						isAddressConnected( std::string ipAddr );

	void						getSktStatRecords( std::vector<VxSktStatRecord>& retSktStatList );


	std::map<SOCKET,VxSktStatRecord> m_SktStatList;
	VxMutex						m_SktStatMutex;
};

