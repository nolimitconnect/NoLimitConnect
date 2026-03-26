#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include "TestFileWav.h"

#include <QString>
#include <QStringList>

#include <vector>   

class TestFileWavMgr
{
public:
    TestFileWavMgr() = default;
    ~TestFileWavMgr() = default;

    void                        testFileMgrStartup( void );

    QStringList                 getTestFileList( void ) const { return m_TestFilesList; }
    const std::vector<TestFileWav>& getTestFileWavList( void ) const { return m_TestFileWavList; }
    bool                        indexIsValid( int index ) const { return index >= 0 && index < m_TestFileWavList.size(); }
    TestFileWav&                getTestFileWav( int index ) { return m_TestFileWavList[index]; }


protected:
    QStringList                 m_TestFilesList;
    std::vector<TestFileWav>    m_TestFileWavList;
};
