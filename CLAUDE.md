# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

NoLimitConnect (NLC) is a C/C++/Qt desktop and Android application implementing a hybrid P2P/relay
social network: users host their own chat rooms, groups, file shares, video/voice chat, etc. from
their own machine, and other user-hosted nodes act as directory/relay infrastructure for peers behind
NAT/firewalls. The media engine is built on a Kodi/FFmpeg-derived stack with OpenGL rendering, and
audio uses Opus + WebRTC AEC + RNNoise. Windows, Linux (x64/ARM64), and Android are the supported
targets; only 64-bit CPUs are supported.

## Build System

This is a single top-level CMake project (`CMakeLists.txt`, `MainSystem`) that pulls in the GUI
(`nolimitgui`) and roughly 40 static libraries under `libs/` (in-house code plus vendored third-party
libs: ffmpeg, libvpx, libx264/x265, dav1d, freetype, libxml2, etc.). C++ standard is C++11. Qt 6.9.3
is the recommended/pinned version (6.7.3 is an accepted fallback, mainly for Raspberry Pi).

### Windows (primary dev platform, via VS Code)

CMakePresets.json gates the Windows/Android/Linux VS Code presets behind
`$env{NLC_ENABLE_VSCODE_PRESETS} == "1"` — this variable is only set by
`.vscode/cmake-with-msvc-env.ps1`, which also loads the MSVC dev environment (`VsDevCmd.bat`) before
invoking `cmake`. **Never call `cmake --preset ...` directly on Windows** — always go through that
script or the VS Code tasks that wrap it, otherwise preset resolution fails/behaves unexpectedly.

Normal flow is VS Code tasks (`Terminal > Run Task`), which also works from the CLI via
`.vscode/cmake-with-msvc-env.ps1`:
- `Configure Windows Debug` / `Configure Windows Release` — fresh CMake configure with the `windows-debug`/`windows-release` preset
- `Build Windows Debug` / `Build Windows Release` — builds the `nolimitconnect` target
- `Run Windows Debug` / `Run Windows Release` — runs `build/windows-<config>/nolimitgui/nolimitconnect.exe`
- `Package Windows Release` — builds the `package_nolimitconnect` target

Debugging: `Windows Debug nolimitconnect` / `Windows Release nolimitconnect` launch configs in
`.vscode/launch.json`.

### Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_LINUX=true -DQT_VERSION=6_9_3
cmake --build build -j$(nproc)
```
(matches `.gitlab-ci.yml`; add `-DARM64=true` for ARM64 targets, e.g. Raspberry Pi.)
CLion/VS Code Linux presets (`linux-x64-debug`/`linux-x64-release`) assume Qt is installed at
`~/Qt/6.9.3/gcc_64` — adjust `CMAKE_PREFIX_PATH` if your Qt lives elsewhere.

### Android (cross-compiled from Linux or Windows)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true -DARM64=true -DQT_VERSION=6_9_3
cmake --build build -j$(nproc)
```
Produces the APK under `nolimitgui/android-build/build/outputs/apk/...`. Requires NDK/SDK paths
configured (see `android-arm64-*` presets in `CMakePresets.json` for the expected layout, and
`docs/developer-docs/build-target-android.md` / `qt-android-kit-setup.md`).

### Qt Creator

Open `CMakeLists.txt` directly, pick a Qt 6.9.x kit, configure, build.

### Flatpak

See `docs/developer-docs/Compile_NoLimitConnect_Flatpak.md` and `docs/developer-docs/Setup_Flatpak_Build_Enviroment.md`;
manifest is `org.nolimitconnect.NoLimitConnect.yml`, packaging scripts are `package-flatpak-signed.sh` /
`flatpak-*.sh` in `.vscode/`.

## Testing

There is no automated unit/integration test suite (no CTest/gtest targets in the tree). Network/session
behavior is validated with a manual, multi-client procedure documented in
`docs/technical/README-QA-TESTS.md` (host/join/relay/online-status scenarios across 3+ clients). When
changing networking, host, membership, or relay logic, check that document for the relevant regression
scenarios rather than expecting a runnable test command.

## Architecture

The codebase is split into two halves connected only through an abstract interface boundary — the GUI
and the P2P engine never call each other's concrete classes directly:

```
nolimitgui/src (Qt GUI)  <-->  libs/GuiInterface (pure-virtual boundary)  <-->  libs/libptopengine (P2P engine)
```

- **`libs/GuiInterface`** — the contract between GUI and engine:
  - `IFromGui` — calls the GUI makes into the engine (login, startup, file ops, requests, ...).
  - `IToGui` — callbacks the engine fires back into the GUI.
  - `IAppImplementation`, `INlcRender`, `IAudioInterface` — other cross-boundary service interfaces.
  - `OsWin32/`, `OsLinux/`, `OsAndroid/`, `OsApple/`, `OsInterface/` — per-OS implementations behind
    a common `OsInterface`.
- **`libs/libptopengine`** — the P2P engine itself (`P2PEngine`), organized as manager classes
  (`*Mgr` suffix, mostly singleton-style) per concern: `Plugins/` (plugin lifecycle, `PluginMgr`,
  `PluginBase`), `HostListMgr`, `HostServerJoinMgr`, `ConnectMgr`, `Connections`, `Relay`,
  `RandConnect`, `AssetMgr`/`AssetBase`, `OfferBase`/`OfferHostMgr`/`OfferClientMgr`,
  `SendQueue`, `FriendRequestMgr`, `NetworkMonitor`, `WebPageMgr`, etc. Most managers pair with a
  `*Db` class for SQLite-backed persistence (via `CoreLib/DbBase`).
- **`libs/PktLib`** — wire-protocol packet structs (`Pkts*`, `VxPktHdr`, `VxConnectInfo`, ...).
  Protocol invariants (16-byte packet alignment, explicit source/destination online IDs, relay must
  preserve end-to-end identity) are documented in `docs/technical/README-NETWORK-PROTOCOL.md` — read
  that before touching packet structs or relay/forwarding logic.
- **`libs/NetLib`** — low-level sockets (`VxSktBase` and friends), NAT traversal, port forwarding,
  proxy/relay plumbing.
- **`libs/CoreLib`** — shared base utilities used by everything: `Vx*` string/threading/time/crypto
  helpers, `VxGUID` (the identity type used throughout as online/session IDs), the embedded
  `sqlite3` amalgamation plus `DbBase`/`DbDataset`/`DbQueryData` wrappers, `VxSettings`, virtual file
  abstractions (`VFile*`).
- **`libs/VirtStream`** — virtual/streaming storage abstraction (`VirtStreamMgr`) used for file
  transfer and media caching across providers.
- **`libs/MediaToolsLib`, `libs/VxVideoLib`, `libs/CamCapture`** — media capture/encode/decode glue
  around the vendored FFmpeg/libvpx/etc. libraries.
- **Everything else under `libs/lib*`** (ffmpeg, libx264, libx265, libvpx, dav1d, freetype, fribidi,
  libjpeg-turbo, libtag, libtheora, libtwolame, libvorbis, libxml2, libzip, libzlib, ...) is vendored
  third-party code built as static libs — not typically edited, only wired up in `libs/CMakeLists.txt`.

### GUI layer conventions (`nolimitgui/src`)

The GUI source tree is flat with a naming convention instead of subfolders:
- `Activity*` — modal/full-flow dialog screens (subclass `ActivityBase`).
- `Applet*` — self-contained feature panels embedded in the main window.
- `*Widget` — reusable `QWidget` components.
- `*Session` (e.g. `GuiHostSession`, `GuiFileXferSession`) — client-side state machines tracking an
  in-progress network session/transfer, mirroring engine-side `*XferSession`/`*Mgr` state.
- `AppCommon` is the central GUI application object; startup sequencing (thread order, deferred
  work) is documented in `docs/technical/README-STARTUP-SEQUENCE.md`.

### Plugin system

Host/client features (chat room, group, random connect, file share, video chat, push-to-talk,
storyboard, "About Me" page, truth-or-dare, ...) are implemented as plugins registered in a fixed
slot scheme: slots 0–47 are "announced" (permission-checked, network-visible) plugins, 49–58 are the
matching client-side plugins, 256+ are internal-only. Full slot table and how to add a new plugin
type live in `docs/technical/plugin-system-overview.md`.

## Subsystem documentation

`docs/technical/README-*.md` has one file per subsystem (audio flow, camera capture, chat room,
network hosts, offer system, plugins, session flow, startup sequence, video stream, AEC/RNNoise
audio processing, deploy/packaging, etc.) — check for a matching doc before making non-trivial
changes to that subsystem. `docs/developer-docs/` covers environment setup and per-platform build
instructions instead.

## Conventions

- Source files carry a standard copyright header block (`Copyright (C) <year> Brett R. Jones`, dual
  license similar to Ruby's — see `COPYING`/`LEGAL` at repo root); match this on new files in `libs/`
  and `nolimitgui/`.
- Cross-platform code branches on `TARGET_OS_WINDOWS` / `TARGET_OS_LINUX` / `TARGET_OS_ANDROID` /
  `TARGET_OS_APPLE` and `TARGET_CPU_X64` / `TARGET_CPU_ARM64`, defined by the root `CMakeLists.txt`
  based on the CMake toolchain/ABI in use.
- Contribution workflow is fork + PR against `main` (see `CONTRIBUTING.md` /
  `docs/developer-docs/contributing.md`); keep PRs small and focused, prefer follow-up PRs over one
  large PR.
