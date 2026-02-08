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

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <CoreLib/VxSettings.h>
#include <CoreLib/VxGUID.h>

#include <map>

class GuiFavoriteMgr : public VxSettings
{
public:
	GuiFavoriteMgr();
	virtual ~GuiFavoriteMgr() = default;

	int32_t						startupFavoriteMgr( const char* dbSettingsFile );
	void						shutdownFavoriteMgr( void );

	void						setIsFavorite( VxGUID& onlineId, bool confirmIsDisabled );
	bool						getIsFavorite( VxGUID& onlineId );
	void						toggleIsFavorite( VxGUID& onlineId );

protected:
	std::map<VxGUID, bool>		m_FavoriteList;
};

