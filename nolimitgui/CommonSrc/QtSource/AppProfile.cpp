//============================================================================
// Copyright (C) 2012 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppProfile.h"

#include <CoreLib/VxProfile.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QString>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif // TARGET_OS_WINDOWS

#include <stdio.h>

namespace
{
	const char* INI_FILE = "NoLimitConnect.ini";
}

//============================================================================
AppProfile::AppProfile()
	: m_u16TcpPort(0)
	, m_u16UdpPort(0)
	, m_u32EnableDebug(0)
	, m_u32DisableUdp(0)
	, m_u32LogFlags(0xff)
	, m_u32LogToFile( 0 )
	, m_strLocalHostIp( "" )
{
}

//============================================================================
void AppProfile::loadProfile( void )
{
	// if has ini file then use settings in it 
	VxFileUtil::getExecuteDirectory( m_strExeDir );

    //=== determine root path to store all application data and settings etc ===//

    QString dataPath =  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation );

    m_strRootUserDataDir = dataPath.toUtf8().constData();

#ifdef DEBUG
    // remove the D from the end so release and debug builds use the same storage directory
    if( !m_strRootUserDataDir.empty() && ( m_strRootUserDataDir.c_str()[m_strRootUserDataDir.length() - 1] == 'D' ) )
    {
        m_strRootUserDataDir = m_strRootUserDataDir.substr( 0, m_strRootUserDataDir.length() - 1 );
    }
#endif // DEBUG

    VxFileUtil::makeForwardSlashPath( m_strRootUserDataDir );
    VxFileUtil::assurePathEndWithSlash( m_strRootUserDataDir );

    VxFileUtil::makeDirectory( m_strRootUserDataDir.c_str() );

    m_NlcPathPrefix = "";

    VxFileUtil::assurePathEndWithSlash( m_strRootUserDataDir );
    VxFileUtil::makeDirectory( m_strRootUserDataDir.c_str() );
    if(!VxFileUtil::directoryExists(m_strRootUserDataDir.c_str()))
    {
        LogMsg( LOG_ERROR, "AppProfile::loadProfile Could not create root data dir %s", m_strRootUserDataDir.c_str());
    }

    m_strRootUserDataDir += m_NlcPathPrefix;
    VxFileUtil::makeDirectory( m_strRootUserDataDir.c_str() );
    if(!VxFileUtil::directoryExists(m_strRootUserDataDir.c_str()))
    {
        LogMsg( LOG_ERROR, "AppProfile::loadProfile Could not create prefixed root data dir %s", m_strRootUserDataDir.c_str());
    }

    // it made it a sub directory of DataLocation
    VxSetRootDataStorageDirectory( m_strRootUserDataDir.c_str() );

    VxSetRootUserDataDirectory( m_strRootUserDataDir.c_str() );

    //=== determine root path for data xfer like incomplete/downloads/uploads etc ===//
#if defined(TARGET_OS_WINDOWS) || defined(TARGET_OS_ANDROID)
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#else
    // linux hides document under .local so use home directory if possible
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if( docsPath.isEmpty() )
    {
        docsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    }
#endif // TARGET_OS_WINDOWS

    m_strRootXferDir = docsPath.toUtf8().constData();

    VxFileUtil::makeForwardSlashPath( m_strRootXferDir );
    VxFileUtil::assurePathEndWithSlash( m_strRootXferDir );

    m_strRootXferDir += VxGetApplicationNameNoSpacesLowerCase();
    VxFileUtil::assurePathEndWithSlash( m_strRootXferDir );

    VxFileUtil::makeDirectory( m_strRootXferDir.c_str() );
    m_strRootXferDir += m_NlcPathPrefix;
    VxFileUtil::assurePathEndWithSlash( m_strRootXferDir );
    VxFileUtil::makeDirectory( m_strRootXferDir.c_str() );
    if(!VxFileUtil::directoryExists(m_strRootXferDir.c_str()))
    {
        LogMsg( LOG_ERROR, "AppProfile::loadProfile Could not create xfer dir %s", m_strRootXferDir.c_str());
    }


    LogMsg( LOG_INFO, "User Data Dir (%s) Xfer Dir (%s)",
			m_strRootUserDataDir.c_str(),
            m_strRootXferDir.c_str()
			);
}

//============================================================================
RCODE AppProfile::saveDebugSettings( void )
{
	RCODE rc = 0;
	VxFileUtil::getExecuteDirectory( m_strExeDir );
	std::string strIniFileName = m_strExeDir + INI_FILE;

	if( VxFileUtil::fileExists( strIniFileName.c_str() ) )
	{
		rc |= setProfileLong( 
			strIniFileName.c_str(),
			"debug",
			"DisableUdp",
			m_u32DisableUdp );

		rc |= setProfileLong( 
			strIniFileName.c_str(),
			"debug",
			"LogFlags",
			m_u32LogFlags );

		rc |= setProfileLong( 
			strIniFileName.c_str(),
			"debug",
			"LogToFile",
			m_u32LogToFile );
	}
	else
	{
		rc = -1;
	}
	return rc;
}

//============================================================================
const char* AppProfile::getLocalHostIp()
{
	return m_strLocalHostIp.c_str();
}

//============================================================================
std::string& AppProfile::getOsSpecificAppDataDir( void )
{
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	QStringList paths = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString dataLocation = paths[0];
#elif QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    QString dataLocation = paths[0];
#else
	QString dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif //QT_5_OR_GREATER
	m_strOsSpecificAppDataDir = dataLocation.toStdString();
	VxFileUtil::makeForwardSlashPath( m_strOsSpecificAppDataDir );
	m_strOsSpecificAppDataDir += "/";
    m_strOsSpecificHomeDir += m_NlcPathPrefix;
    VxFileUtil::makeDirectory( m_strOsSpecificHomeDir.c_str() );
    if(!VxFileUtil::directoryExists(m_strOsSpecificHomeDir.c_str()))
    {
        LogMsg( LOG_ERROR, "AppProfile::getOsSpecificDocumentsDir Could not create app data dir %s", m_strOsSpecificHomeDir.c_str());
    }

	return m_strOsSpecificAppDataDir;
}

//============================================================================
std::string& AppProfile::getOsSpecificHomeDir( void )
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString homeLocation = paths[0];
#else
    QString homeLocation = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif //QT_5_OR_GREATER
	m_strOsSpecificHomeDir = homeLocation.toStdString();
	VxFileUtil::makeForwardSlashPath( m_strOsSpecificHomeDir );
    VxFileUtil::assurePathEndWithSlash( m_strOsSpecificHomeDir );
    m_strOsSpecificHomeDir += m_NlcPathPrefix;
    VxFileUtil::makeDirectory( m_strOsSpecificHomeDir.c_str() );
    if(!VxFileUtil::directoryExists(m_strOsSpecificHomeDir.c_str()))
    {
        LogMsg( LOG_ERROR, "AppProfile::getOsSpecificDocumentsDir Could not create home dir %s", m_strOsSpecificHomeDir.c_str());
    }

	return m_strOsSpecificHomeDir;
}
