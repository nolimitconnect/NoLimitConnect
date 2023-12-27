#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/AssetDefs.h>

class AssetBaseInfo;
class VxGUID;

class InputClientCallback
{
public:
	virtual bool				handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) = 0;

	virtual bool				canAcceptInput( EAssetType assetType ) = 0;
	virtual bool				checkIfCanSend( void ) = 0;

	virtual bool				generateFileName( EAssetType assetType, VxGUID& uniqueId, std::string& retFileName ) = 0;
	virtual bool				fillAssetBaseInfo( AssetBaseInfo& assetInfo, bool newAssetId ) = 0;
};
