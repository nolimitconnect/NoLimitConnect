#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <config_appcorelibs.h>

#include <CoreLib/DbBase.h>
#include <CoreLib/VxGUID.h>

class AssetBaseInfo;

class AssetBaseXferDb : public DbBase
{
public:
	AssetBaseXferDb( const char* stateDbName );
	virtual ~AssetBaseXferDb() = default;

	void						lockAssetBaseXferDb( void )			{ m_AssetBaseXferDbMutex.lock(); }
	void						unlockAssetBaseXferDb( void )			{ m_AssetBaseXferDbMutex.unlock(); }

	virtual RCODE				onCreateTables( int iDbVersion );
	virtual RCODE				onDeleteTables( int iOldVersion );

	void 						addAsset( VxGUID& assetUniqueId );
	void						removeAsset( VxGUID& assetUniqueId );

	void						getAllAssets( std::vector<VxGUID>& assetList );
	void						purgeAllAssetBaseXfer( void ); 

protected:
	VxMutex						m_AssetBaseXferDbMutex;
};

