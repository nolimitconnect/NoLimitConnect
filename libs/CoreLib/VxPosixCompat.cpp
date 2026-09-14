//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.org
//============================================================================
//
// POSIX functions MSVC does not provide, declared in NlcCompilerConfig.h.
//
// These replace libgnu (gnulib). gnulib is a mixture, and 734 of its source
// files are GPL-only rather than LGPL, which made it the single largest licence
// liability in the tree for the sake of a handful of functions. Written from
// the POSIX specifications and the documented MSVC equivalents; they share no
// code with gnulib.
//
// Only strchrnul lives here. timegm is already implemented in
// VxFunctionsMissingInWindows.cpp -- defining it again would be a duplicate
// symbol. The rest of what libgnu exported had no callers at all.
//
//============================================================================

#include "VxDebug.h"

#ifdef TARGET_OS_WINDOWS

#include <string.h>

//============================================================================
//! Like strchr, but returns a pointer to the terminating NUL rather than NULL
//! when the character is not found. Used by sqlite's printf implementation.
char* strchrnul( const char* s, int c_in )
{
    if( !s )
    {
        return nullptr;
    }

    const char target = ( char )c_in;
    while( *s && *s != target )
    {
        s++;
    }

    // Either the match or the NUL -- both are the caller's expected result.
    return ( char* )s;
}

#endif // TARGET_OS_WINDOWS
