#pragma once
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
// <pwd.h> is the POSIX user database. Windows has no equivalent and nothing in
// this tree calls getpwuid/getpwnam -- verified, zero callers.
//
// This used to redirect to libgnu (gnulib, GPL-heavy). Rather than reimplement
// a user database nobody asks for, the MSVC side is deliberately EMPTY: if a
// caller ever appears it fails loudly at compile time, which is the right
// outcome, instead of silently picking up something unexamined.
//
//============================================================================

#include <NlcDependLibrariesConfig.h>

#ifndef _MSC_VER
# include_next <pwd.h>
#endif // _MSC_VER
