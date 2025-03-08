//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "GuiParams.h"

#include <P2PEngine/EngineSettings.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>

#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#include <stdio.h>
#include <array>

////============================================================================
GuiParams& GetGuiParams()
{
    static GuiParams g_GuiParams;
    return g_GuiParams;
}

//============================================================================
AppGlobals::AppGlobals( AppCommon& appCommon )
: m_MyApp( appCommon )
{
}

//============================================================================
VxNetIdent* AppGlobals::getMyNetIdent( void )
{
	return m_MyApp.getEngine().getMyNetIdent();
}

//============================================================================
bool AppGlobals::saveUserIdentToDatabase( void )
{
	if( false == m_MyApp.getAccountMgr().updateAccount( *getMyNetIdent() ) )
	{
		LogMsg( LOG_ERROR, "AppGlobals::saveUserIdentToDatabase: ERROR updating database" );
		return false;
	}

	return true;
}

//============================================================================
void AppGlobals::launchWebBrowser( const char* pUri )
{
	// NOTE:qt crashes studio sometimes when openUrl so use ShellExecute
#ifdef TARGET_OS_WINDOWS
	ShellExecuteA(0, 0, pUri, 0, 0 , SW_SHOW );
#else
	QString strUrl = pUri;
	QDesktopServices::openUrl( QUrl(strUrl, QUrl::TolerantMode) );
#endif //TARGET_OS_WINDOWS
}

//============================================================================
//! update ident and save to database then send permission change to engine
void  AppGlobals::updatePluginPermission( EPluginType pluginType, EFriendState ePluginPermission )
{
	getMyNetIdent()->setPluginPermission( pluginType, ePluginPermission );
	if( false == m_MyApp.getAccountMgr().updateAccount( *getMyNetIdent() ) )
	{
		LogMsg( LOG_ERROR, "UpdatePluginPermissions: ERROR updating database");
	}

    m_MyApp.getEngine().setPluginPermission( pluginType, ePluginPermission );
}

//============================================================================
//! update has picture and save to database then send picture change to engine
void UpdateHasPicture( P2PEngine& engine, int bHasPicture )
{
	engine.getMyNetIdent()->setHasProfilePicture(bHasPicture);
	if( false == GetAppInstance().getAccountMgr().updateAccount( *engine.getMyNetIdent() ) )
	{
		LogMsg( LOG_ERROR, "UpdateHasPicture: ERROR updating database");
	}

	engine.setHasPicture( bHasPicture );
}

//============================================================================
//! show message box to user
void ErrMsgBox( QWidget* parent, int infoLevel, const char* pMsg, ... )
{
	//build message on stack so no out of memory issue
	std::array<char, 4096> szBuffer;
	va_list arg_ptr;
	va_start(arg_ptr, pMsg);
    vsnprintf(szBuffer.data(), szBuffer.size(), pMsg, arg_ptr);
	va_end(arg_ptr);
	szBuffer[szBuffer.size() - 1] = 0;

	QMessageBox msgBox( parent );
	msgBox.setText( szBuffer.data() );
	msgBox.exec();
}

