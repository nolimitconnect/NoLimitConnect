//============================================================================
// Copyright (C) 2010 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "PluginNetServices.h"
#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/NetServices/NetServiceHdr.h>

#include <NetLib/VxSktBase.h>

//============================================================================
PluginNetServices::PluginNetServices( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseService( engine, pluginMgr, myIdent, pluginType )
, m_NetServicesMgr( engine.getNetServicesMgr() )
{
	setPluginType( ePluginTypeNetServices );
}

//============================================================================
void PluginNetServices::testIsMyPortOpen( void )
{
	m_NetServicesMgr.addNetActionIsMyPortOpenToQueue();
}
