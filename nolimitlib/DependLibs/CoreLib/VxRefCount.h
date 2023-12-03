#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#include <CoreLib/config_corelib.h>

class VxRefCount
{
public:
    //=== vars ===//
    int m_iRefCnt;

    //============================================================================
    //=== constructor ===//
    VxRefCount()
        : m_iRefCnt(1)
    {
    }

    //============================================================================
    //=== destructor ===//
    ~VxRefCount()
    {
        m_iRefCnt--;
        //vx_assert( m_iRefCnt == 0 );
    }

    virtual int incRefCnt();

    virtual int decRefCnt();

    virtual int getRefCnt();
};
