#pragma once

#if !defined(TARGET_OS_ANDROID)
# include <NlcDependLibrariesConfig.h>
#endif // !defined(TARGET_OS_ANDROID)

#include "x264_config.h"

#if defined(TARGET_OS_ANDROID)
# include "config_x264_android.h"
#elif defined(TARGET_OS_WINDOWS)
# include "config_x264_w64.h"
#else
# include "config_x264_linux.h"
#endif // defined(TARGET_OS_ANDROID)
