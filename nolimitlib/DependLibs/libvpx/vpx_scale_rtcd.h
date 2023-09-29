#pragma once

#include <NlcDependLibrariesConfig.h>

#if defined(TARGET_OS_LINUX)
# include "vpx_scale_rtcd_linux.h"
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_ANDROID)
# include "vpx_scale_rtcd_android.h"
#endif // defined(TARGET_OS_ANDROID)

#if defined(TARGET_OS_WINDOWS)
# include "vpx_scale_rtcd_win.h"
#endif // defined(TARGET_OS_ANDROID)
