#pragma once

#include <NlcDependLibrariesConfig.h>

#if defined(TARGET_OS_LINUX)
# if defined(TARGET_CPU_AARCH64)
#  include "vpx_config_aarch64.h"
# else
#  include "vpx_config_linux.h"
# endif // defined(TARGET_CPU_AARCH64)
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_ANDROID)
# include "vpx_config_android.h"
#endif // defined(TARGET_OS_ANDROID)

#if defined(TARGET_OS_WINDOWS)
# include "vpx_config_win.h"
#endif // defined(TARGET_OS_ANDROID)
