//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include "GuiHelpers.h"
#include "GuiMemberActiveMgr.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletBase::AppletBase( const char* ObjName, AppCommon& app, QWidget* parent )
: ActivityBase( ObjName, app, parent )
, m_ActivityCallbacksEnabled( false )
, m_IsPlaying( false )
, m_SliderIsPressed( false )
{
}

//============================================================================
void AppletBase::setAssetInfo( AssetBaseInfo& assetBaseInfo )
{
    AssetInfo assetInfo( assetBaseInfo );
    m_AssetInfo = assetInfo;
}

//============================================================================
bool AppletBase::isAssetInfoSet( void )
{
    return m_AssetInfo.getAssetUniqueId().isVxGUIDValid();
}

//============================================================================
bool AppletBase::checkIfCanSend( HostedId hostId )
{
	if( !hostId.isValid() )
	{
		GuiHelpers::showInvalidHostIdError();
		return false;
	}

	std::set<VxGUID> memberList;
	getMyApp().getMemberActiveMgr().getActiveMembers( hostId, memberList );
	if( memberList.empty() )
	{
		GuiHelpers::showNoMembersOnlineError();
		return false;
	}

	return true;
}
