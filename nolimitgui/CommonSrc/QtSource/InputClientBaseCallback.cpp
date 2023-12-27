//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputClientBaseCallback.h"

#include "AppCommon.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxTimeUtil.h>

//============================================================================
bool InputClientBaseCallback::canAcceptInput( EAssetType assetType )
{
	return true;
}

//============================================================================
bool InputClientBaseCallback::checkIfCanSend( void )
{
    return true;
}

//============================================================================
bool InputClientBaseCallback::generateFileName( EAssetType assetType, VxGUID& uniqueId, std::string& retFileName )
{ 
    if( getInputMediaStorageDirectory( retFileName ) )
    {
        retFileName += getMyApp().getUserMgr().getMyOnlineName();
        retFileName += "_";
        retFileName += VxTimeUtil::getFileNameCompatibleDateAndTime( GetLocalTimeMs() );
        retFileName += "#";
        retFileName += uniqueId.toHexString();
        retFileName += AssetInfo::getDefaultFileExtension( assetType );
        return true;
    }
    else
    {
        LogMsg( LOG_ERROR, "InputClientBaseCallback::generateFileName getInputMediaStorageDirectory failed" );
        return false;
    }
}

//============================================================================
bool InputClientBaseCallback::getInputMediaStorageDirectory( std::string & storageDir )
{
    storageDir = VxGetUserXferDirectory();
    storageDir += "contacts/";
    VxFileUtil::makeDirectory( storageDir );
    storageDir += "me/";
    VxFileUtil::makeDirectory( storageDir );

    return VxFileUtil::directoryExists( storageDir.c_str() );
}

//============================================================================
bool InputClientBaseCallback::fillAssetBaseInfo( AssetBaseInfo& assetInfo, bool newAssetId )	
{ 
	if( newAssetId )
	{
		assetInfo.generateNewUniqueId();
	}
	else
	{
		assetInfo.getAssetUniqueId().assureIsValidGUID();
	}

	assetInfo.setPluginType( getInputClientPluginType() );
	
	assetInfo.setCreationTime( GetTimeStampMs() );
	assetInfo.setCreatorId( getMyApp().getMyOnlineId());
	assetInfo.setHistoryId( getMyApp().getMyOnlineId());

	return true;
}

//============================================================================
bool InputClientBaseCallback::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
    return getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
}