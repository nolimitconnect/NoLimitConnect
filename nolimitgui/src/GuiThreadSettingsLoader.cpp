//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiThreadSettingsLoader.h"

#include "AppSettings.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTime.h>

#include <QStandardPaths>

//============================================================================
void GuiThreadSettingsLoader::run()
{
    int timeStart = GetApplicationAliveMs();

    // create settings database appshortname_settings.db3 in /appshortName/data/
    std::string strSettingsDbFileName = VxGetAppNoLimitDataDirectory() + m_AppSettings.getAppShortName() + "_settings.db3";
    m_AppSettings.appSettingStartup( strSettingsDbFileName.c_str() );
   
    VxSetLogLevelFlags( m_AppSettings.getLogLevels() );
    VxSetModuleLogFlags( m_AppSettings.getLogModules() );

    setIsSettingsLoaded( true );
    if( LogEnabled( eLogStartup ) )
    {
        LogMsg( LOG_VERBOSE, "Setup Time app settings %d ms", GetApplicationAliveMs() - timeStart );
    }   
}
