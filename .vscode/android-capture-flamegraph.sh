#!/bin/bash

set -euo pipefail

WORKSPACE_FOLDER="${WORKSPACE_FOLDER:-.}"
PACKAGE_NAME="${PACKAGE_NAME:-com.nolimitconnect.nolimitconnect}"
ACTIVITY_NAME="${ACTIVITY_NAME:-org.qtproject.qt.android.bindings.QtActivity}"
DURATION_SECONDS="${DURATION_SECONDS:-10}"
SAMPLE_FREQUENCY="${SAMPLE_FREQUENCY:-1000}"
OUTPUT_DIR="${OUTPUT_DIR:-}"
TITLE="${TITLE:-No Limit Connect Android Flame Graph}"
SKIP_COLLECTION=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -WorkspaceFolder) WORKSPACE_FOLDER="$2"; shift 2 ;;
        -PackageName) PACKAGE_NAME="$2"; shift 2 ;;
        -ActivityName) ACTIVITY_NAME="$2"; shift 2 ;;
        -DurationSeconds) DURATION_SECONDS="$2"; shift 2 ;;
        -SampleFrequency) SAMPLE_FREQUENCY="$2"; shift 2 ;;
        -OutputDir) OUTPUT_DIR="$2"; shift 2 ;;
        -Title) TITLE="$2"; shift 2 ;;
        -SkipCollection) SKIP_COLLECTION=true; shift ;;
        *)
            echo "Unknown argument: $1" >&2
            exit 1
            ;;
    esac
done

if [[ ! -d "$WORKSPACE_FOLDER" ]]; then
    echo "ERROR: Workspace folder is missing or inaccessible: $WORKSPACE_FOLDER" >&2
    exit 1
fi

WORKSPACE_FOLDER="$(cd "$WORKSPACE_FOLDER" && pwd)"
if [[ -z "$OUTPUT_DIR" ]]; then
    OUTPUT_DIR="$WORKSPACE_FOLDER/build/android-flamegraph"
fi
mkdir -p "$OUTPUT_DIR"
OUTPUT_DIR="$(cd "$OUTPUT_DIR" && pwd)"

function first_existing_file() {
    local candidate
    for candidate in "$@"; do
        if [[ -f "$candidate" ]]; then
            echo "$candidate"
            return 0
        fi
    done
    return 1
}

function first_existing_dir() {
    local candidate
    for candidate in "$@"; do
        if [[ -d "$candidate" ]]; then
            echo "$candidate"
            return 0
        fi
    done
    return 1
}

ADB_PATH="${ADB_PATH:-$(command -v adb 2>/dev/null || true)}"
if [[ -z "$ADB_PATH" ]]; then
    ADB_PATH=$(first_existing_file \
        "${ANDROID_SDK_ROOT:-}/platform-tools/adb" \
        "${HOME}/Android/Sdk/platform-tools/adb") || true
fi
if [[ -z "${ADB_PATH:-}" ]] || [[ ! -x "$ADB_PATH" ]]; then
    echo "ERROR: adb not found. Set ADB_PATH or ANDROID_SDK_ROOT." >&2
    exit 1
fi

NDK_ROOT="${ANDROID_NDK:-${CMAKE_ANDROID_NDK:-}}"
if [[ -z "$NDK_ROOT" ]]; then
    NDK_ROOT=$(first_existing_dir \
        "${ANDROID_SDK_ROOT:-}/ndk/27.2.12479018" \
        "${ANDROID_SDK_ROOT:-}/ndk/26.1.10909125" \
        "${HOME}/Android/Sdk/ndk/27.2.12479018" \
        "${HOME}/Android/Sdk/ndk/26.1.10909125") || true
fi
if [[ -z "${NDK_ROOT:-}" ]]; then
    echo "ERROR: Android NDK not found. Set ANDROID_NDK or CMAKE_ANDROID_NDK." >&2
    exit 1
fi

SIMPLEPERF_DIR="$NDK_ROOT/simpleperf"
APP_PROFILER="$SIMPLEPERF_DIR/app_profiler.py"
INFERNO="$SIMPLEPERF_DIR/inferno.sh"
if [[ ! -f "$APP_PROFILER" ]] || [[ ! -f "$INFERNO" ]]; then
    echo "ERROR: simpleperf tooling not found under: $SIMPLEPERF_DIR" >&2
    exit 1
fi

PYTHON_BIN="${PYTHON_BIN:-$(command -v python3 2>/dev/null || true)}"
if [[ -z "$PYTHON_BIN" ]]; then
    PYTHON_BIN="$(command -v python 2>/dev/null || true)"
fi
if [[ -z "$PYTHON_BIN" ]]; then
    echo "ERROR: python3/python not found on PATH." >&2
    exit 1
fi

NATIVE_LIB_DIR=""
if [[ -d "$WORKSPACE_FOLDER/build/android-arm64-debug" ]]; then
    NATIVE_LIB_DIR="$WORKSPACE_FOLDER/build/android-arm64-debug"
elif [[ -d "$WORKSPACE_FOLDER/build/android-arm64-release" ]]; then
    NATIVE_LIB_DIR="$WORKSPACE_FOLDER/build/android-arm64-release"
fi

echo "Workspace folder: $WORKSPACE_FOLDER"
echo "Output directory: $OUTPUT_DIR"
echo "Using adb: $ADB_PATH"
echo "Using NDK: $NDK_ROOT"
if [[ -n "$NATIVE_LIB_DIR" ]]; then
    echo "Using native libraries: $NATIVE_LIB_DIR"
else
    echo "WARNING: Android build directory not found. Profiling will continue without host-side unstripped libraries." >&2
fi

"$ADB_PATH" start-server > /dev/null

adb_args=()
if [[ -n "${ANDROID_SERIAL:-}" ]]; then
    echo "Using Android device serial: $ANDROID_SERIAL"
    adb_args=( -s "$ANDROID_SERIAL" )
fi

ACTIVITY_COMPONENT="$PACKAGE_NAME/$ACTIVITY_NAME"
if [[ "$SKIP_COLLECTION" != "true" ]]; then
    echo "Starting activity: $ACTIVITY_COMPONENT"
    "$ADB_PATH" "${adb_args[@]}" shell am start -n "$ACTIVITY_COMPONENT" > /dev/null || true

    pushd "$OUTPUT_DIR" > /dev/null
    rm -f perf.data flamegraph.html
    rm -rf binary_cache

    record_options="-e task-clock:u -f $SAMPLE_FREQUENCY -g --duration $DURATION_SECONDS"
    profiler_args=("$APP_PROFILER" -p "$PACKAGE_NAME" -r "$record_options" -o perf.data --ndk_path "$NDK_ROOT")
    if [[ -n "$NATIVE_LIB_DIR" ]]; then
        profiler_args+=( -lib "$NATIVE_LIB_DIR" )
    fi

    "$PYTHON_BIN" "${profiler_args[@]}"
    popd > /dev/null
else
    if [[ ! -f "$OUTPUT_DIR/perf.data" ]]; then
        echo "ERROR: -SkipCollection was requested but $OUTPUT_DIR/perf.data does not exist." >&2
        exit 1
    fi
fi

pushd "$OUTPUT_DIR" > /dev/null
inferno_args=( -sc --record_file perf.data --no_browser --one-flamegraph -o flamegraph.html --title "$TITLE" )
if [[ -n "$NATIVE_LIB_DIR" ]]; then
    inferno_args+=( --symfs "$NATIVE_LIB_DIR" )
fi
"$INFERNO" "${inferno_args[@]}"
popd > /dev/null

echo "Flame graph generated: $OUTPUT_DIR/flamegraph.html"
echo "Perf data saved: $OUTPUT_DIR/perf.data"
if [[ -d "$OUTPUT_DIR/binary_cache" ]]; then
    echo "Collected binaries: $OUTPUT_DIR/binary_cache"
fi