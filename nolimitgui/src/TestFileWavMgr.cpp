//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TestFileWavMgr.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QDir>
#include <QFileInfo>

//============================================================================
void TestFileWavMgr::testFileMgrStartup( void )
{
    QString repoRootPath = getRepoRootPath();
    if( repoRootPath.isEmpty() )
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::testFileMgrStartup failed to find repo root path." );
        return;
    }

    QString testDataFilesPath = repoRootPath + "/testdata/";
    if( !QDir(testDataFilesPath).exists() )
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::testFileMgrStartup test data files path does not exist: %s", testDataFilesPath.toStdString().c_str() );
        return;
    }

    getWavFileList( testDataFilesPath, m_TestFilesList );
    if( m_TestFilesList.empty() )
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::testFileMgrStartup found no wav files in test data files path: %s", testDataFilesPath.toStdString().c_str() );
        return;
    }

    if( !loadTestFiles( m_TestFilesList ) )
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::testFileMgrStartup failed to load test files" );
    }
}

//============================================================================
QString TestFileWavMgr::getRepoRootPath( void )
{
    // this is a bit hacky but we just look for the .git folder which should be in the repo root
    QString currentPath = QDir::currentPath();
    while( !QDir(currentPath + "/.git").exists() )
    {
        int lastSlashIndex = currentPath.lastIndexOf( '/' );
        if( lastSlashIndex == -1 )
        {
            return QString();
        }
        currentPath = currentPath.left( lastSlashIndex );
    }
    return currentPath;
}

//============================================================================
bool TestFileWavMgr::getWavFileList( const QString& testDataFilePath, QStringList& outWavFileList )
{
    QDir testDataDir( testDataFilePath );
    if( !testDataDir.exists() )
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::getWavFileList test data files path does not exist: %s", testDataFilePath.toStdString().c_str() );
        return false;
    }

    QStringList nameFilter( "*.wav" );
    QFileInfoList fileInfoList = testDataDir.entryInfoList( nameFilter, QDir::Files | QDir::NoSymLinks );
    for( const QFileInfo& fileInfo : fileInfoList )
    {
        outWavFileList.append( fileInfo.absoluteFilePath() );
    }

    if( outWavFileList.empty() )
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::getWavFileList found no wav files in test data files path: %s", testDataFilePath.toStdString().c_str() );
        return false;
    }

    return true;
}

//============================================================================
bool TestFileWavMgr::loadTestFiles( QStringList& testFileList )
{
    for( const QString& testFilePath : testFileList )
    {
        TestFileWav testFileWav( testFilePath );
        if( !testFileWav.isValid() )
        {
            LogMsg( LOG_ERROR, "TestFileWavMgr::loadTestFiles failed to load wav file: %s", testFilePath.toStdString().c_str() );
            return false;
        }
        m_TestFileWavList.emplace_back( testFileWav );
    }

    return true;
}