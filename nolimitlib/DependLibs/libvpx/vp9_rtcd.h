#pragma once

#if defined(TARGET_OS_LINUX)
# include "vp9_rtcd_linux.h"
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_ANDROID)
# include "vp9_rtcd_android.h"
#endif // defined(TARGET_OS_ANDROID)

#if defined(TARGET_OS_WINDOWS)
# include "vp9_rtcd_windows.h"
#endif // defined(TARGET_OS_ANDROID)
