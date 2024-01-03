#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseService.h"

#include <NetServices/NetServicesMgr.h>

class PluginNetServices : public PluginBaseService
{
public:
	PluginNetServices( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginNetServices() = default;
    PluginNetServices() = delete; // don't allow default constructor
    PluginNetServices( const PluginNetServices& ) = delete; // don't allow copy constructor

	void						testIsMyPortOpen( void );

protected:

    virtual void				replaceConnection( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& poOldSkt, std::shared_ptr<VxSktBase>& poNewSkt ) override{};
    virtual void				onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override {};
    virtual void				onConnectionLost( std::shared_ptr<VxSktBase>& sktBase ) override {};

	void						onContactOnlineStatusChange( VxGUID& onlineId, bool isOnline ) override {};

	//=== vars ===//
	NetServicesMgr&				m_NetServicesMgr;
};

