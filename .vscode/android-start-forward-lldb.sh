#!/bin/bash

# Android LLDB debug setup script for Linux/macOS
# Finds Android device, starts lldb-server, and forwards ports for VS Code debugging

set -e

# Parameters
ADB_PATH="${ADB_PATH:=$(which adb 2>/dev/null || echo '')}"
if [[ -z "$ADB_PATH" ]]; then
    ADB_PATH="${ANDROID_SDK_ROOT}/platform-tools/adb"
fi
if [[ -z "$ADB_PATH" ]] || [[ ! -f "$ADB_PATH" ]]; then
    ADB_PATH="${HOME}/Android/Sdk/platform-tools/adb"
fi
PACKAGE_ACTIVITY="org.nolimitconnect.nolimitconnect/org.qtproject.qt.android.bindings.QtActivity"
LLDB_PORT="${LLDB_PORT:-5039}"
DEBUG_SOCKET_WAIT_SECONDS="${DEBUG_SOCKET_WAIT_SECONDS:-15}"
DEVICE_WAIT_SECONDS="${DEVICE_WAIT_SECONDS:-45}"
WORKSPACE_FOLDER="${WORKSPACE_FOLDER:-.}"
WAIT_FOR_DEBUGGER=false
STOP_AFTER_LAUNCH=false
ATTACH_ONLY=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -WorkspaceFolder) WORKSPACE_FOLDER="$2"; shift 2 ;;
        -DebugSocketWaitSeconds) DEBUG_SOCKET_WAIT_SECONDS="$2"; shift 2 ;;
        -DeviceWaitSeconds) DEVICE_WAIT_SECONDS="$2"; shift 2 ;;
        -LldbPort) LLDB_PORT="$2"; shift 2 ;;
        -WaitForDebugger) WAIT_FOR_DEBUGGER=true; shift ;;
        -StopAfterLaunch) STOP_AFTER_LAUNCH=true; shift ;;
        -AttachOnly) ATTACH_ONLY=true; shift ;;
        *) shift ;;
    esac
done

function resolve_lldb_server_host_path() {
    local candidates=(
        "$ANDROID_SDK_ROOT/ndk/27.2.12479018/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/18/lib/linux/aarch64/lldb-server"
        "$ANDROID_SDK_ROOT/ndk/26.1.10909125/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/17/lib/linux/aarch64/lldb-server"
        "$ANDROID_SDK_ROOT/ndk/26.1.10909125/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/17.0.2/lib/linux/aarch64/lldb-server"
        "${HOME}/Android/Sdk/ndk/27.2.12479018/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/18/lib/linux/aarch64/lldb-server"
        "${HOME}/Android/Sdk/ndk/26.1.10909125/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/17/lib/linux/aarch64/lldb-server"
        "${HOME}/Android/Sdk/ndk/26.1.10909125/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/17.0.2/lib/linux/aarch64/lldb-server"
    )

    for path in "${candidates[@]}"; do
        if [[ -f "$path" ]]; then
            echo "$path"
            return 0
        fi
    done

    return 1
}

function get_device_state() {
    local adb="$1"
    local serial="$2"

    if [[ -z "$serial" ]]; then
        return 1
    fi

    local state=$("$adb" -s "$serial" get-state 2>/dev/null || true)
    if [[ -z "$state" ]]; then
        return 1
    fi

    echo "$state"
    return 0
}

function get_connected_device_serials() {
    local adb="$1"

    "$adb" devices | tail -n +2 | grep -E '^\S+\s+device$' | awk '{print $1}' || true
}

function wait_for_device_state() {
    local adb="$1"
    local serial="$2"
    local timeout_seconds="$3"

    local deadline=$(($(date +%s) + timeout_seconds))
    while [[ $(date +%s) -lt $deadline ]]; do
        local state=$(get_device_state "$adb" "$serial" || true)
        if [[ "$state" == "device" ]]; then
            return 0
        fi

        if [[ -n "$state" ]]; then
            echo "Waiting for Android device $serial: current state '$state'"
        else
            echo "Waiting for Android device $serial: current state unavailable"
        fi

        sleep 1
    done

    return 1
}

function get_app_pid() {
    local adb="$1"
    local device_serial="$2"
    local pkg="$3"

    local pid_output=$("$adb" -s "$device_serial" shell "pidof $pkg" 2>/dev/null || true)
    if [[ -z "$pid_output" ]]; then
        return 1
    fi

    # pidof can return multiple PIDs; use the first one.
    echo "$pid_output" | awk '{print $1}'
    return 0
}

function test_lldb_server_running() {
    local adb="$1"
    local device_serial="$2"
    local pkg="$3"

    # Try pidof first
    local pid_output=$("$adb" -s "$device_serial" shell "run-as $pkg sh -c 'pidof lldb-server'" 2>/dev/null || true)
    if [[ -n "$pid_output" && "$pid_output" =~ ^[0-9] ]]; then
        return 0
    fi

    # Fallback: ps grep
    local ps_output=$("$adb" -s "$device_serial" shell "run-as $pkg sh -c 'ps -A | grep lldb-server'" 2>/dev/null || true)
    if [[ -n "$ps_output" && "$ps_output" =~ lldb-server ]]; then
        return 0
    fi

    # Global ps check
    local global_ps=$("$adb" -s "$device_serial" shell "ps -A | grep lldb-server" 2>/dev/null || true)
    if [[ -n "$global_ps" && "$global_ps" =~ lldb-server ]]; then
        return 0
    fi

    return 1
}

function ensure_lldb_server_in_app_sandbox() {
    local adb="$1"
    local device_serial="$2"
    local pkg="$3"

    local host_lldb_server
    if ! host_lldb_server=$(resolve_lldb_server_host_path); then
        echo "ERROR: Could not locate host lldb-server binary in expected Android NDK paths." >&2
        return 1
    fi

    local tmp_path="/data/local/tmp/nlc-lldb-server"
    "$adb" -s "$device_serial" push "$host_lldb_server" "$tmp_path" > /dev/null
    "$adb" -s "$device_serial" shell "chmod 755 $tmp_path"

    # Try to copy into app sandbox
    "$adb" -s "$device_serial" shell "run-as $pkg sh -c 'cp $tmp_path files/lldb-server && chmod 700 files/lldb-server'" 2>/dev/null && {
        echo "./files/lldb-server"
        return 0
    }

    # Fallback to /data/local/tmp
    echo "run-as copy to app sandbox failed; falling back to /data/local/tmp/nlc-lldb-server" >&2
    echo "$tmp_path"
    return 0
}

# Validate inputs
if [[ ! -f "$ADB_PATH" ]]; then
    echo "ERROR: adb not found at path: $ADB_PATH" >&2
    exit 1
fi

if [[ ! -d "$WORKSPACE_FOLDER" ]]; then
    echo "ERROR: Workspace folder is missing or inaccessible: $WORKSPACE_FOLDER" >&2
    exit 1
fi

echo "Workspace folder (resolved): $(cd "$WORKSPACE_FOLDER" && pwd)"

# Start adb server
"$ADB_PATH" start-server > /dev/null 2>&1 || true

# Find device
devices=$(get_connected_device_serials "$ADB_PATH")
serial=""

if [[ -z "$devices" ]]; then
    if [[ -n "${ANDROID_SERIAL:-}" ]]; then
        serial="$ANDROID_SERIAL"
        echo "No device currently in 'device' state; waiting up to ${DEVICE_WAIT_SECONDS}s for configured serial: $serial"
        if ! wait_for_device_state "$ADB_PATH" "$serial" "$DEVICE_WAIT_SECONDS"; then
            # Try one more time to get any device
            devices=$(get_connected_device_serials "$ADB_PATH")
            if [[ -n "$devices" ]]; then
                serial=$(echo "$devices" | head -1)
                echo "Configured serial $ANDROID_SERIAL unavailable; falling back to connected device: $serial"
            else
                echo "ERROR: Android device '$ANDROID_SERIAL' did not reach state 'device' within $DEVICE_WAIT_SECONDS seconds." >&2
                exit 1
            fi
        fi
    else
        echo "ERROR: No Android device in state device." >&2
        exit 1
    fi
else
    if [[ -n "${ANDROID_SERIAL:-}" ]]; then
        serial="$ANDROID_SERIAL"
    else
        serial=$(echo "$devices" | head -1)
        device_count=$(echo "$devices" | wc -l)
        if [[ $device_count -gt 1 ]]; then
            echo "Multiple devices detected: $(echo $devices | tr '\n' ', '). Using first device: $serial"
        fi
    fi
fi

# Wait for selected device
if ! wait_for_device_state "$ADB_PATH" "$serial" "$DEVICE_WAIT_SECONDS"; then
    echo "ERROR: Android device '$serial' is not in state 'device'. Check USB mode/authorization." >&2
    exit 1
fi

echo "Using Android device serial: $serial"

# Remove any existing forward
"$ADB_PATH" -s "$serial" forward --remove tcp:"$LLDB_PORT" 2>/dev/null || {
    echo "No existing adb forward to remove on tcp:$LLDB_PORT; continuing."
}

package_name=$(echo "$PACKAGE_ACTIVITY" | cut -d'/' -f1)

# Start/attach app
if [[ "$ATTACH_ONLY" != "true" ]]; then
    echo "Force-stopping existing app instance for package: $package_name"
    "$ADB_PATH" -s "$serial" shell am force-stop "$package_name" || {
        echo "adb force-stop for $package_name returned non-zero; continuing."
    }
    sleep 0.5

    if [[ "$WAIT_FOR_DEBUGGER" == "true" ]]; then
        echo "Starting app in wait-for-debugger mode (-D)."
        "$ADB_PATH" -s "$serial" shell am start -D -n "$PACKAGE_ACTIVITY" || true
    else
        "$ADB_PATH" -s "$serial" shell am start -n "$PACKAGE_ACTIVITY" || true
    fi
else
    echo "Attach-only mode: leaving running app untouched for package: $package_name"
fi

# Handle early SIGSTOP
if [[ "$STOP_AFTER_LAUNCH" == "true" && "$ATTACH_ONLY" != "true" ]]; then
    stopped_early=false
    max_attempts=$((DEBUG_SOCKET_WAIT_SECONDS * 20))
    for ((attempt=1; attempt<=max_attempts; attempt++)); do
        early_pid=$(get_app_pid "$ADB_PATH" "$serial" "$package_name" || true)
        if [[ -z "$early_pid" ]]; then
            sleep 0.05
            continue
        fi

        # Try run-as first (more reliable)
        "$ADB_PATH" -s "$serial" shell "run-as $package_name sh -c 'kill -STOP $early_pid'" 2>/dev/null && {
            echo "Sent SIGSTOP to app pid: $early_pid before LLDB attach"
            stopped_early=true
            break
        }

        # Fallback to direct kill
        "$ADB_PATH" -s "$serial" shell "kill -STOP $early_pid" 2>/dev/null && {
            echo "Sent SIGSTOP to app pid: $early_pid before LLDB attach"
            stopped_early=true
            break
        }

        if ((attempt % 20 == 0)); then
            echo "Still trying to SIGSTOP app pid $early_pid before attach..."
        fi
        sleep 0.05
    done

    if [[ "$stopped_early" != "true" ]]; then
        echo "Could not confirm early app stop before attach; continuing."
    fi
fi

# Setup lldb-server forwarding
lldb_server_run_path=""
if ! lldb_server_run_path=$(ensure_lldb_server_in_app_sandbox "$ADB_PATH" "$serial" "$package_name"); then
    echo "ERROR: Failed to ensure lldb-server in app sandbox" >&2
    exit 1
fi

forwarded=false
last_pid_tried=""
for ((attempt=1; attempt<=DEBUG_SOCKET_WAIT_SECONDS; attempt++)); do
    app_pid=$(get_app_pid "$ADB_PATH" "$serial" "$package_name" || true)

    if [[ -z "$app_pid" ]]; then
        sleep 1
        continue
    fi

    last_pid_tried="$app_pid"

    # Start lldb-server
    "$ADB_PATH" -s "$serial" shell "run-as $package_name sh -c '$lldb_server_run_path gdbserver --attach $app_pid localhost:$LLDB_PORT >/dev/null 2>&1 &'" > /dev/null || {
        sleep 1
        continue
    }

    # Setup port forward
    "$ADB_PATH" -s "$serial" forward tcp:"$LLDB_PORT" tcp:"$LLDB_PORT" > /dev/null && {
        if test_lldb_server_running "$ADB_PATH" "$serial" "$package_name"; then
            forwarded=true
            break
        fi

        if [[ "$ATTACH_ONLY" == "true" ]]; then
            echo "Attach-only mode: lldb-server health check inconclusive, proceeding with forwarded socket."
            forwarded=true
            break
        fi
    }

    "$ADB_PATH" -s "$serial" forward --remove tcp:"$LLDB_PORT" 2>/dev/null || true
    sleep 1
done

if [[ "$forwarded" != "true" ]]; then
    echo "ERROR: Failed to start/attach lldb-server for package $package_name (last pid tried: $last_pid_tried)" >&2
    exit 1
fi

echo "Forwarded tcp:$LLDB_PORT to device tcp:$LLDB_PORT (attached pid: $last_pid_tried)"
exit 0
