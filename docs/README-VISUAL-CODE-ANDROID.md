# VS Code Android Debugging

## Requirement
- Install extension `vadimcn.vscode-lldb` (CodeLLDB).

## Current Workspace Setup
- Launch config: `Android Debug nolimitconnect (CodeLLDB)` in `.vscode/launch.json`.
- Pre-launch task: `Android: Start app + forward LLDB (5039)` in `.vscode/tasks.json`.
- Default device serial is set to `GN42DM044394023L` via task environment variable `ANDROID_SERIAL`.

## How To Debug
1. Build Android target (preset `android-arm64-debug`) so symbols exist.
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
	- `F:/Android/Sdk/platform-tools/adb.exe devices`