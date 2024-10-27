#pragma once

#if !defined(TARGET_CPU_AARCH64)
#include <NlcDependLibrariesConfig.h>
#endif // defined(TARGET_CPU_AARCH64)

#if defined(TARGET_OS_LINUX)
# if defined(TARGET_CPU_AARCH64)
#  include "vpx_config_linux_arm64.h"
# else
#  include "vpx_config_linux_x64.h"
# endif // defined(TARGET_CPU_AARCH64)
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_ANDROID)
# if defined(TARGET_CPU_AARCH64)
#  include "vpx_config_android_arm64.h"
# else
#  include "vpx_config_android_arm32.h"
# endif // defined(TARGET_CPU_AARCH64)


#endif // defined(TARGET_OS_ANDROID)

#if defined(TARGET_OS_WINDOWS)
# include "vpx_config_win_x64.h"
#endif // defined(TARGET_OS_WINDOWS)
