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

#include "InputClientCallback.h"
#include <GuiInterface/IDefs.h>

class AppCommon;

class InputClientBaseCallback : public InputClientCallback
{
public:
	virtual AppCommon&			getMyApp( void ) = 0;
	virtual EPluginType			getInputClientPluginType( void ) = 0;
	virtual bool 				getInputMediaStorageDirectory( std::string& storageDir );

	bool						canAcceptInput( EAssetType assetType ) override;
	bool						checkIfCanSend( void ) override;

	bool						generateFileName( EAssetType assetType, VxGUID& uniqueId, std::string& retFileName ) override;
	bool						fillAssetBaseInfo( AssetBaseInfo& assetInfo, bool newAssetId ) override;

	bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;
};