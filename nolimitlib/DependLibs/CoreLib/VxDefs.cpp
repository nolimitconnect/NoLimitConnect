//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <NlcDependLibrariesConfig.h>

#if 0 // moved to VxTime.cpp
#ifdef TARGET_OS_ANDROID
#include "VxDefs.h"

#include <time.h>
// android doesn't have millisecond sleep like most linux distributions but does have nano sleep
void VxSleep( int iMilliSec )
{
	struct timespec ts;
	ts.tv_sec = iMilliSec/1000;
	ts.tv_nsec = (iMilliSec%1000)*1000000;
	nanosleep(&ts, NULL);
};	

#endif // TARGET_OS_ANDROID
#endif //0 