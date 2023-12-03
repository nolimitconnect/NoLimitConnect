//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerBase.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "AppCommon.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletPlayerBase::AppletPlayerBase( const char* ObjName, AppCommon& app, QWidget* parent )
: AppletBase( ObjName, app, parent )
, m_ActivityCallbacksEnabled( false )
, m_IsPlaying( false )
, m_SliderIsPressed( false )
{
	initAppletPlayerBase();
}

//============================================================================
void AppletPlayerBase::initAppletPlayerBase( void )
{
}
