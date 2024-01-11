//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiAppLoaderThread.h"

#include "AccountMgr.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiFavoriteMgr.h"
#include "LogMgr.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
GuiAppLoaderThread::GuiAppLoaderThread( AppCommon& myApp, QObject* parent )
    : QThread(parent)
    , m_MyApp( myApp )
{
    m_ElapsedTimer.start();
}

//============================================================================
GuiAppLoaderThread::~GuiAppLoaderThread()
{

}

//============================================================================
void GuiAppLoaderThread::run()
{
    LogMsg( LOG_VERBOSE, "GuiAppLoaderThread::run start in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    // create settings database appshortname_settings.db3 in /appshortName/data/
    QString strSettingsDbFileName = VxGetAppNoLimitDataDirectory().c_str() + m_MyApp.getAppShortName() + "_settings.db3";
    m_MyApp.getAppSettings().appSettingStartup( strSettingsDbFileName.toUtf8().constData(), m_MyApp.getAppDefaultMode() );

    setIsSettingsLoaded( true );
    LogMsg( LOG_VERBOSE, "GuiAppLoaderThread::run settings loaded in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    // database of multiple accounts
	// create accounts database appshortname_accounts.db3 in /appshortName/data/
	QString strAccountDbFileName = VxGetAppNoLimitDataDirectory().c_str() + m_MyApp.getAppShortName() + "_accounts.db3";
	m_MyApp.getAccountMgr().startupAccountMgr(strAccountDbFileName.toUtf8().constData());
    setIsAccountMgrLoaded( true );
    LogMsg( LOG_VERBOSE, "GuiAppLoaderThread::run account mgr loaded in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

	// create settings database appshortname_favorites.db3 in /appshortName/data/
	QString strFavoriteMgrDbFileName = VxGetAppNoLimitDataDirectory().c_str() + m_MyApp.getAppShortName() + "_favorites.db3";
	m_MyApp.getFavoriteMgr().startupFavoriteMgr(strFavoriteMgrDbFileName.toUtf8().constData());
    LogMsg( LOG_VERBOSE, "GuiAppLoaderThread::run favorites mgr loaded in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    // load icons from resources
    m_MyApp.getMyIcons().myIconsStartup();
    setIsIconsLoaded( true );

    LogMsg( LOG_VERBOSE, "GuiAppLoaderThread::run icons loaded in %" PRId64 " ms", m_ElapsedTimer.elapsed() );

    // Now that settings are set up we can enable logging with log settings
    GetLogMgrInstance().startupLogMgr();

	SetUseMilitaryTime( m_MyApp.getAppSettings().getUseMilitaryTime() );

    setIsLoadComplete( true );
    LogMsg( LOG_VERBOSE, "GuiAppLoaderThread::run load complete in %" PRId64 " ms", m_ElapsedTimer.elapsed() );
   
}

