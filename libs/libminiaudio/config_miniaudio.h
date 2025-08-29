#pragma once

#if defined(TARGET_OS_WINDOWS)
# define MA_ENABLE_DSOUND
# define MA_NO_WASAPI
# define MA_NO_WINMM
#endif // defined(TARGET_OS_WINDOWS)

