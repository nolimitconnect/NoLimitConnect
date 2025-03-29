//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginFriendRequest.h"
#include "PluginMgr.h"

#include <CoreLib/VxDebug.h>

//============================================================================
PluginFriendRequest::PluginFriendRequest( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBase( engine, pluginMgr, myIdent, pluginType )
{
	LogMsg( LOG_VERBOSE, "PluginFriendRequest::PluginFriendRequest" );
	setPluginType( ePluginTypeFriendRequest );
}
