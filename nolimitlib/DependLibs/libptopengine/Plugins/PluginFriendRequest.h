#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseFilesServer.h"

class PluginFriendRequest : public PluginBase
{
public:
	PluginFriendRequest( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* netIdent, EPluginType pluginType );
	virtual ~PluginFriendRequest() = default;

	void						onContactWentOffline( VxNetIdent* netIdent, std::shared_ptr<VxSktBase>& sktBase ) override {};
	void						onConnectionLost(std::shared_ptr<VxSktBase> &) override {};
	void						onContactOnlineStatusChange(VxGUID &,bool) override {};
	void						replaceConnection(VxNetIdent *,std::shared_ptr<VxSktBase> &,std::shared_ptr<VxSktBase> &) override {};	


protected:

};


