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
bool AppletBase::handleGroupieAssetAction( GroupieId& adminId, EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	HostedId hostId =  adminId.getHostedId();

	if( !hostId.isValid() )
	{
		okMessageBox( QObject::tr( "Invalid Host Id" ),
						QObject::tr( "Host Id has not been set" ) );
		return false;
	}

	if( eAssetActionAddAssetAndSend != assetAction && eAssetActionAssetSend != assetAction )
	{
		// just pass on to engine
		return getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
	}

	std::set<VxGUID> memberList;
	getMyApp().getMemberActiveMgr().getActiveMembers( hostId, memberList );
	if( memberList.empty() )
	{
		okMessageBox( QObject::tr( "No Members Online" ),
						QObject::tr( "There are no members online to send to" ) );
		return false;
	}

	bool result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAddToAssetMgr, assetInfo );
	if( !result )
	{
		okMessageBox( QObject::tr( "Failed to add asset" ),
						QObject::tr( "Failed to add asset. Please check disk space" ) );
	}

	if( result )
	{
		assetInfo.setHostedId( hostId );
		// first send to admin if we are not the admin
		if( hostId.getHostOnlineId() != getMyApp().getMyOnlineId() )
		{
			assetInfo.setDestUserId( hostId.getHostOnlineId() );
			result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, assetInfo );
			if( !result )
			{
				okMessageBox( QObject::tr( "Failed to send" ),
							  QObject::tr( "Failed to send to " ) + QString( m_MyApp.describeUser( hostId.getHostOnlineId() ).c_str() ) );
				return false;
			}
		}

		// next send to online members
		for( auto memberId : memberList )
		{
			assetInfo.setDestUserId( memberId );
			getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, assetInfo );
		}
	}

	return result;
}
