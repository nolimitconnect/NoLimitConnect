//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "TextChatLogic.h"
#include "AppCommon.h"
#include "MyIcons.h"
#include "AppGlobals.h"

#include <P2PEngine/P2PEngine.h>
#include <P2PEngine/EngineSettings.h>

namespace
{
	#define GAME_SETTINGS_KEY "TODGAME"
}

//============================================================================
TextChatLogic::TextChatLogic( AppCommon& myApp, P2PEngine& engine, EPluginType pluginType, QWidget* parent )
: QWidget( parent )
, m_MyApp( myApp )
, m_Engine( engine )
, m_ePluginType( pluginType )
{
}

//============================================================================
void TextChatLogic::setGuiWidgets(	VxNetIdent*	hisIdent )
{
	m_HisIdent			= hisIdent;
}

//============================================================================
void TextChatLogic::handleUserWentOffline( void )
{

}
