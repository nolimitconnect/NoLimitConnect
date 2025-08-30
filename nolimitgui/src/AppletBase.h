#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include <AssetMgr/AssetInfo.h>

class AppletBase : public ActivityBase
{
	Q_OBJECT
public:
	AppletBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletBase() = default;

	virtual void				setAssetInfo( AssetInfo& assetInfo )	{ m_AssetInfo = assetInfo; }
	virtual void				setAssetInfo( AssetBaseInfo& assetInfo );
	virtual bool				isAssetInfoSet( void );
	virtual AssetInfo&			getAssetInfo( void )					{ return m_AssetInfo; }

signals:
	void						signalPlayProgress( int pos );
	void						signalPlayEnd( void );

protected:
	bool						handleGroupieAssetAction( GroupieId& adminId, EAssetAction assetAction, AssetBaseInfo& assetInfo );

	//=== vars ===//
	AssetInfo					m_AssetInfo;
	bool						m_ActivityCallbacksEnabled;
	bool						m_IsPlaying;
	bool						m_SliderIsPressed;
    int                         m_LaunchParam;
};


