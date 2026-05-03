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
int32_t GuiFavoriteMgr::startupFavoriteMgr( const char* dbSettingsFile )
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
	if( onlineId.isValid() )
	{
		m_FavoriteList[ onlineId ] = favorite;
		std::string keyVal = onlineId.toHexString();
		if( favorite )
		{
			setIniValue( keyVal.c_str(), favorite );
		}
		else
		{
			removeBoolIniValueFromDb( keyVal.c_str() );
		}

		GetAppInstance().getUserMgr().refreshUser( onlineId );
	}
}

//============================================================================
bool GuiFavoriteMgr::getIsFavorite( VxGUID& onlineId )
{
	bool isFavorite{ false };
	if( onlineId.isValid() )
	{
		auto favItem = m_FavoriteList.find( onlineId );
		if( favItem != m_FavoriteList.end() )
		{
			return favItem->second;
		}
		else
		{
			std::string keyVal = onlineId.toHexString();
			getIniValue( onlineId.toHexString().c_str(), isFavorite, false );
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