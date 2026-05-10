# VS Code Android Debugging

## Requirement
- Install extension `vadimcn.vscode-lldb` (CodeLLDB).

## Current Workspace Setup
- Launch config: `Android Debug nolimitconnect (CodeLLDB)` in `.vscode/launch.json`.
- Pre-launch task: `Android: Start app + forward LLDB (5039)` in `.vscode/tasks.json`.
- Default device serial is set to `GN42DM044394023L` via task environment variable `ANDROID_SERIAL`.
- Android build tasks now support both hosts:
	- Linux uses preset `android-arm64-debug`.
	- Windows tasks use preset `android-arm64-debug-windows`.

## How To Debug
1. Build Android target so symbols exist.
	- On Linux, use preset `android-arm64-debug` or the `Build Android Debug` task.
	- On Windows, use the `Build Android Debug` task, which selects preset `android-arm64-debug-windows`.
2. Start launch profile `Android Debug nolimitconnect (CodeLLDB)`.
3. When prompted, confirm or adjust symbol path:
	 - `${workspaceFolder}/build/android-arm64-debug/nolimitgui/libnolimitconnect_arm64-v8a.so`

## Device Selection
- If multiple real devices are attached, task uses `ANDROID_SERIAL`.
- To change device, edit `.vscode/tasks.json` at:
	- `tasks[].options.env.ANDROID_SERIAL`

## Flame Graph

### Android
- Use task `Android: Capture flame graph` or launch entry `Task: Android Capture Flame Graph`.
- The task first installs the latest Android debug APK so call stacks match the host symbols.
- It then runs Android NDK `simpleperf` with call graph capture and writes results under:
	- `${workspaceFolder}/build/android-flamegraph`
- Main artifacts:
	- `flamegraph.html`
	- `perf.data`
	- `binary_cache/`
- Default capture window is `10` seconds at `1000` Hz.
- If native symbols are missing in the graph, rebuild/install Android debug first so the unstripped host library in `build/android-arm64-debug` matches the APK on device.

### Linux
- Use task `Linux: Capture flame graph` or launch entry `Task: Linux Capture Flame Graph`.
- The task builds the Linux debug binary, then runs it under `perf record` with call graph capture.
- Results are written under: `${workspaceFolder}/build/linux-flamegraph`
- Main artifacts:
	- `flamegraph.html` (requires Android NDK `simpleperf` with `inferno` tool)
	- `perf.data` (always generated)
- Default capture window is `10` seconds at `97` Hz.
- **Prerequisite**: `perf` must be enabled on the system:
	- Run: `sudo sysctl -w kernel.perf_event_paranoid=1`
	- Or edit `/etc/sysctl.conf` and set: `kernel.perf_event_paranoid=1`
	- Requires `linux-tools` package on Debian/Ubuntu.
- To visualize flame graph, either install Android NDK or manually convert perf data using: `perf report -g`



## Troubleshooting
- Error `unknown option: --interpreter=mi` means MI-mode debugger is being used (`cppdbg`) with NDK LLDB.
- Use `Android Debug nolimitconnect (CodeLLDB)` launch profile (type `lldb`) instead.
- Verify device visibility with:
	- `adb devices`