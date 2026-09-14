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
// <sys/ioctl.h> is POSIX device control. On Windows the socket equivalent is
// ioctlsocket() from winsock, and the V4L2 camera paths that use ioctl() are
// Linux-only and compile out here.
//
// This used to redirect to libgnu's djgpp/ioctl_gnu.h (gnulib, GPL-heavy).
// <io.h> is kept because callers expect the low-level file functions to come
// with it.
//
//============================================================================

#include <NlcDependLibrariesConfig.h>

#ifdef _MSC_VER
# include <io.h>
#else
# include_next <sys/ioctl.h>
#endif // _MSC_VER
