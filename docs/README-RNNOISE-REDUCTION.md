# RNNoise Noise Reduction

This project vendors RNNoise source code in `libs/librnnoise` and builds it as
a static library (`rnnoise`) through CMake.

## Goal

- Keep RNNoise integrated in the main build system.
- Keep architecture-specific files separated by CPU target.
- Keep the ARM64 section prepared for future Raspberry Pi 5 optimization work.

## Source Layout

- Library root: `libs/librnnoise`
- Core C sources: `libs/librnnoise/src`
- x64 optimized sources: `libs/librnnoise/src/x86`
- C++ wrapper used by the app: `libs/librnnoise/RNNoise.cpp`

## CMake Integration

The RNNoise build definition is in:

- `libs/librnnoise/CMakeLists.txt`

Current behavior:

- Core RNNoise sources are always compiled.
- x64-only files are compiled only in `if(DEFINED TARGET_CPU_X64)`.
- ARM64-only files must be added only in `if(DEFINED TARGET_CPU_ARM64)`.
- `config_rnnoise.h` is copied to generated `config.h` for upstream-compatible
	includes that expect `#include "config.h"`.

## Architecture Policy

### x64

- Keep x64 SIMD files only in the x64 section.
- Per-file compile options are set for SIMD files:
	- SSE4.1 for `nnet_sse4_1.c`
	- AVX2 for `nnet_avx2.c`

### ARM64

- The ARM64 section is intentionally a placeholder today.
- Add ARM/NEON optimized files there later.
- Do not mix ARM64 files into the generic source list.

## Build Notes

- RNNoise currently builds as C/C++ in this project.
- `HAVE_CONFIG_H` is enabled for RNNoise sources.
- Runtime CPU dispatch is enabled for x64 with `RNN_ENABLE_X86_RTCD`.

## Quick Verification

From repository root:

```bash
cmake -S libs/librnnoise -B /tmp/nlc-rnnoise-check -DTARGET_CPU_X64=ON -DTARGET_OS_LINUX=ON
cmake --build /tmp/nlc-rnnoise-check -j
```

If ARM64 flags/sources are added in the future, use the corresponding target
defines/toolchain for ARM64 and verify the same way.
