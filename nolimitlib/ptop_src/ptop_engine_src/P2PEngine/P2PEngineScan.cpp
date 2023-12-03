//============================================================================
// Copyright (C) 2013 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/Search/RcScan.h>

//============================================================================
void P2PEngine::fromGuiStartScan( EScanType eScanType, uint8_t searchFlags, uint8_t fileTypeFlags, const char* pSearchPattern )
{
	m_RcScan.fromGuiStartScan( eScanType, searchFlags, fileTypeFlags, pSearchPattern );
}

//============================================================================
void P2PEngine::fromGuiNextScan( EScanType eScanType )
{
	m_RcScan.fromGuiNextScan( eScanType );
}

//============================================================================
void P2PEngine::fromGuiStopScan( EScanType eScanType )
{
	m_RcScan.fromGuiStopScan( eScanType );
}
