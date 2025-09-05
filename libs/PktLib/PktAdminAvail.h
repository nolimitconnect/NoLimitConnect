#pragma once

//============================================================================
// Copyright (C) 2025 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxPktHdr.h"

#include <GuiInterface/IDefs.h>

#pragma pack(push)
#pragma pack(1)
class PktAdminAvail : public VxPktHdr
{
public:
	PktAdminAvail();

	void						setAdminAvailFlags( uint8_t adminAvailFlags ) { m_AdminAvailFlags = adminAvailFlags; };
	uint8_t						getAdminAvailFlags( void )						{ return m_AdminAvailFlags; }

private:
	//=== vars ===//
	uint8_t						m_AdminAvailFlags{ 0 };
	uint8_t						m_Res1{ 0 };
	uint32_t					m_Res2{ 0 };
};

#pragma pack(pop)
