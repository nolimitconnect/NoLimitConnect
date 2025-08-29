#pragma once

#if defined(TARGET_CPU_ARM64)
# include "config_opus_aarch64.h"
#elif defined(TARGET_CPU_ARM32)
# include "config_opus_arm.h"
#else
# include "config_opus_x64.h"
#endif // defined(TARGET_CPU_ARM64)

