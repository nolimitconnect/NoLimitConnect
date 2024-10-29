#pragma once

// rtcd is only used for intel processors
#if defined(TARGET_OS_LINUX)
# if defined(TARGET_CPU_X86_64)
#  include "vpx_scale_rtcd_linux_x64.h"
# endif // defined(TARGET_CPU_86_X64)
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_WINDOWS)
# include "vpx_scale_rtcd_win_x64.h"
#endif // defined(TARGET_OS_WINDOWS)

#if defined(TARGET_CPU_ARM64) || defined(TARGET_CPU_ARM32)
# include "vpx_scale_rtcd_arm64.h"
#endif // defined(TARGET_CPU_ARM64) || defined(TARGET_CPU_ARM32)
