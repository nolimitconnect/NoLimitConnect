#pragma once

#if defined(TARGET_OS_ANDROID)
# include "config_opus_aarch64.h"
#else
# include "config_opus_x64.h"
#endif // defined(TARGET_OS_ANDROID)
