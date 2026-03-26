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

#include "../VxResourceToRealFile.h"

#include <CoreLib/VxDebug.h>

//============================================================================
void TestFileWavMgr::testFileMgrStartup( void )
{
    const QString resourcePath = ":/AppRes/Resources/NlcTestAudio.wav";
    VxResourceToRealFile realFile( resourcePath );
    if( !realFile.getRealFilePathAndName().empty() )
    {
        QString realFilePath = QString::fromStdString( realFile.getRealFilePathAndName() );
        TestFileWav resWav( realFilePath );
        if( resWav.isValid() )
        {
            m_TestFilesList.append( realFilePath );
            m_TestFileWavList.emplace_back( resWav );
            LogMsg( LOG_DEBUG, "TestFileWavMgr::testFileMgrStartup loaded resource test audio: %s", resourcePath.toStdString().c_str() );
        }
        else
        {
            LogMsg( LOG_ERROR, "TestFileWavMgr::testFileMgrStartup failed to load resource test audio extracted to: %s", realFile.getRealFilePathAndName().c_str() );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "TestFileWavMgr::testFileMgrStartup failed to extract resource: %s", resourcePath.toStdString().c_str() );
    }
}