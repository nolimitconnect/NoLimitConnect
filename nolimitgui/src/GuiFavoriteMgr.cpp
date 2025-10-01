//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiFavoriteMgr.h"

#include "AppCommon.h"

namespace
{
	int GUI_FAVORITE_MGR_DBVERSION = 1;
	const char* FAVORITE_DB_KEY = "IsFavoiriteDb";
}

//============================================================================
GuiFavoriteMgr::GuiFavoriteMgr()
: VxSettings( "GuiFavoriteMgrDb" )
{
}

//============================================================================
RCODE GuiFavoriteMgr::startupFavoriteMgr( const char* dbSettingsFile )
{
	return dbStartup( GUI_FAVORITE_MGR_DBVERSION, dbSettingsFile );
}

//============================================================================
void GuiFavoriteMgr::shutdownFavoriteMgr( void )
{
	dbShutdown();
}

//============================================================================
void GuiFavoriteMgr::setIsFavorite( VxGUID& onlineId, bool favorite )
{
	if( onlineId.isVxGUIDValid() )
	{
		m_FavoriteList[ onlineId ] = favorite;
		std::string keyVal = onlineId.toHexString();
		if( favorite )
		{
			setIniValue( FAVORITE_DB_KEY, keyVal.c_str(), favorite );
		}
		else
		{
			removeBoolIniValueFromDb( FAVORITE_DB_KEY, keyVal.c_str() );
		}

		GetAppInstance().getUserMgr().refreshUser( onlineId );
	}
}

//============================================================================
bool GuiFavoriteMgr::getIsFavorite( VxGUID& onlineId )
{
	bool isFavorite{ false };
	if( onlineId.isVxGUIDValid() )
	{
		auto favItem = m_FavoriteList.find( onlineId );
		if( favItem != m_FavoriteList.end() )
		{
			return favItem->second;
		}
		else
		{
			std::string keyVal = onlineId.toHexString();
			getIniValue( FAVORITE_DB_KEY, onlineId.toHexString().c_str(), isFavorite, false );
			m_FavoriteList[ onlineId ] = isFavorite;
		}
	}
	
	return isFavorite;
}

//============================================================================
void GuiFavoriteMgr::toggleIsFavorite( VxGUID& onlineId )
{
	setIsFavorite( onlineId, !getIsFavorite( onlineId ) );
}