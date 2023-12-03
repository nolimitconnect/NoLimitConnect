//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#include "config_corelib.h"

#include "VxRefCount.h"

//============================================================================
int VxRefCount::incRefCnt()
{
    m_iRefCnt++;
    return m_iRefCnt;
}

//============================================================================
int VxRefCount::decRefCnt()
{
    m_iRefCnt--;
    //vx_assert( m_iRefCnt >= 0 );
    return m_iRefCnt;
}

//============================================================================
int VxRefCount::getRefCnt()
{
    return m_iRefCnt;
}
