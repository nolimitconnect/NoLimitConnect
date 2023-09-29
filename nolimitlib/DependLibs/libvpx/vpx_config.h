#pragma once

#include <NlcDependLibrariesConfig.h>

#if defined(TARGET_OS_LINUX)
# include "vpx_config_linux.h"
#endif // defined(TARGET_OS_LINUX)

#if defined(TARGET_OS_ANDROID)
# include "vpx_config_android.h"
#endif // defined(TARGET_OS_ANDROID)

#if defined(TARGET_OS_WINDOWS)
# include "vpx_config_win.h"
#endif // defined(TARGET_OS_ANDROID)
