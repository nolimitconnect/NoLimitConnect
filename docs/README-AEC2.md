# AEC2 Overview

The older WebRTC echo cancellation implementation (AEC2) is less timing sensitive than AEC3 for some pipelines.

- AEC3 implementation: `libs/libwebrtc-aec`
- AEC2 implementation: `libs/libwebrtc-aec2`

## Source Extraction Map

AEC2 was extracted from `$(HOME)/webrtc-aec2/src/webrtc` into `libs/libwebrtc-aec2/apm/webrtc` using the same extraction approach as the AEC3 subtree.

Included source subtrees/files:

- `modules/audio_processing/aec` (legacy AEC2 core and C API)
- `modules/audio_processing/utility` (delay estimator support used by AEC2)
- `common_audio/signal_processing` (signal processing primitives used by AEC2)
- `common_audio/ring_buffer.c`, `common_audio/ring_buffer.h`
- `common_audio/wav_file.h` (header dependency in AEC internals)
- `system_wrappers/interface/cpu_features_wrapper.h`
- `system_wrappers/interface/compile_assert_c.h`
- `system_wrappers/source/cpu_features.cc`
- `typedefs.h`

Wrapper API exposed to the app:

- `libs/libwebrtc-aec2/apm/WebRtcAec.h`
- `libs/libwebrtc-aec2/apm/WebRtcAec.cpp`

The wrapper preserves the existing app-facing `WebRtcAec` class shape used by `AudioMgr`.

## Build Notes

The AEC2 target is built as static library `webrtc-aec2` from `libs/libwebrtc-aec2/apm/CMakeLists.txt`.

Architecture-sensitive filtering is required when globbing signal processing sources:

- Exclude `*_mips.c` on non-MIPS builds.
- Exclude `*_neon.c` unless building ARM64.

This avoids non-native asm compilation failures on x64 hosts.

## Selecting AEC2 vs AEC3

The root CMake option controls backend selection:

- `USE_AEC2=ON`: builds and links `libs/libwebrtc-aec2`
- `USE_AEC2=OFF`: builds and links `libs/libwebrtc-aec`

The configure log prints the active backend selection.
