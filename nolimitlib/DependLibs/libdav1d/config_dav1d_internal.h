

#pragma once

#if defined(TARGET_CPU_AARCH64)
#include "config_dav1d_aarch64_internal.h"
#elif defined(TARGET_CPU_ARM)
#include "config_dav1d_arm_internal.h"
#else
#include "config_dav1d_x86_internal.h"
#endif // defined(TARGET_CPU_ARM)



