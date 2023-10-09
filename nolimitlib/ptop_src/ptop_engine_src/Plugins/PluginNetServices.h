#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
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

#include "PluginBaseService.h"

#include <ptop_src/ptop_engine_src/NetServices/NetServicesMgr.h>

class PluginNetServices : public PluginBaseService
{
public:
	PluginNetServices( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginNetServices() = default;
    PluginNetServices() = delete; // don't allow default constructor
    PluginNetServices( const PluginNetServices& ) = delete; // don't allow copy constructor

	void						testIsMyPortOpen( void );

protected:

	virtual void				replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt )	{};
	virtual void				onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase )	{};
	virtual void				onConnectionLost( std::shared_ptr<VxSktBase>& sktBase )	{};

	//=== vars ===//
	NetServicesMgr&				m_NetServicesMgr;
};

