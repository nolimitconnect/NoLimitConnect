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

## Troubleshooting
- Error `unknown option: --interpreter=mi` means MI-mode debugger is being used (`cppdbg`) with NDK LLDB.
- Use `Android Debug nolimitconnect (CodeLLDB)` launch profile (type `lldb`) instead.
- Verify device visibility with:
	- `adb devices`