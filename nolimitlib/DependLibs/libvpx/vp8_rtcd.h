#pragma once

// rtcd is only used for intel processors
#if defined(TARGET_OS_LINUX)
#  include "vp8_rtcd_linux_x64.h"
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_WINDOWS)
# include "vp8_rtcd_win_x64.h"
#endif // defined(TARGET_OS_ANDROID)
