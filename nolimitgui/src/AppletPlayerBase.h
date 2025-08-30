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

#include "AppletBase.h"
#include <AssetMgr/AssetInfo.h>


class AppletPlayerBase : public AppletBase
{
	Q_OBJECT
public:
	AppletPlayerBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletPlayerBase(){};


protected:
	void						initAppletPlayerBase( void );

	//=== vars ===//
	bool						m_ActivityCallbacksEnabled;
	bool						m_IsPlaying;
	bool						m_SliderIsPressed;

};


