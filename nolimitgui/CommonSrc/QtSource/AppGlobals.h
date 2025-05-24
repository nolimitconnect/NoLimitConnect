#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <PktLib/VxCommon.h>

#include <QString>

class AppCommon;
class SoundMgr;
class QWidget;
class P2PEngine;
class IFromGui;
class VxNetIdent;
class GuiParams;

class AppGlobals
{
public:
	AppGlobals( AppCommon& appCommon );

	void						setRequiresProxy( bool bRequiresProxy );
	bool						getRequiresProxy( void );
	VxNetIdent*					getMyNetIdent( void );
	bool						saveUserIdentToDatabase( void );
    //! update identity and save to database then send permission change to engine
    void						updatePluginPermission( enum EPluginType pluginType, enum EFriendState ePluginPermission );

	void						launchWebBrowser( const char* pUri );
protected:
	AppCommon&					m_MyApp;
};

//! gui params and constants
GuiParams&                      GetGuiParams();

//! update has picture and save to database then send picture change to engine
void							UpdateHasAboutMeContent( P2PEngine& engine, bool hasContent );
//! update has storyboard content and save to database then send update to engine
void							UpdateHasStoryboardContent( P2PEngine& engine, bool hasContent );

//! show message box to user
void							ErrMsgBox( QWidget*, int infoLevel, const char* pMsg, ... );

