#!/bin/bash

set -euo pipefail

WORKSPACE_FOLDER="${WORKSPACE_FOLDER:-.}"
BINARY_PATH=""
DURATION_SECONDS="${DURATION_SECONDS:-10}"
SAMPLE_FREQUENCY="${SAMPLE_FREQUENCY:-97}"
OUTPUT_DIR="${OUTPUT_DIR:-}"
TITLE="${TITLE:-No Limit Connect Linux Flame Graph}"
BUILD_PRESET="${BUILD_PRESET:-linux-x64-debug}"

while [[ $# -gt 0 ]]; do
    case "$1" in
        -WorkspaceFolder) WORKSPACE_FOLDER="$2"; shift 2 ;;
        -BinaryPath) BINARY_PATH="$2"; shift 2 ;;
        -DurationSeconds) DURATION_SECONDS="$2"; shift 2 ;;
        -SampleFrequency) SAMPLE_FREQUENCY="$2"; shift 2 ;;
        -OutputDir) OUTPUT_DIR="$2"; shift 2 ;;
        -Title) TITLE="$2"; shift 2 ;;
        -BuildPreset) BUILD_PRESET="$2"; shift 2 ;;
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

if [[ -z "$BINARY_PATH" ]]; then
    BINARY_CANDIDATES=(
        "$WORKSPACE_FOLDER/out/build/$BUILD_PRESET/nolimitgui/nolimitconnect"
        "$WORKSPACE_FOLDER/build/$BUILD_PRESET/nolimitgui/nolimitconnect"
        "$WORKSPACE_FOLDER/nolimitgui/bin-Linux/nolimitconnect"
    )
    for candidate in "${BINARY_CANDIDATES[@]}"; do
        if [[ -x "$candidate" ]]; then
            BINARY_PATH="$candidate"
            break
        fi
    done
fi

if [[ -z "$BINARY_PATH" ]] || [[ ! -x "$BINARY_PATH" ]]; then
    echo "ERROR: Could not find or execute Linux binary. Tried:" >&2
    for candidate in "${BINARY_CANDIDATES[@]}"; do
        echo "  - $candidate" >&2
    done
    echo "Provide -BinaryPath or ensure the binary is built with preset $BUILD_PRESET" >&2
    exit 1
fi

if [[ -z "$OUTPUT_DIR" ]]; then
    OUTPUT_DIR="$WORKSPACE_FOLDER/build/linux-flamegraph"
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

NDK_ROOT="${ANDROID_NDK:-${CMAKE_ANDROID_NDK:-}}"
if [[ -z "$NDK_ROOT" ]]; then
    NDK_ROOT=$(first_existing_dir \
        "${ANDROID_SDK_ROOT:-}/ndk/27.2.12479018" \
        "${ANDROID_SDK_ROOT:-}/ndk/26.1.10909125" \
        "${HOME}/Android/Sdk/ndk/27.2.12479018" \
        "${HOME}/Android/Sdk/ndk/26.1.10909125") || true
fi

INFERNO=""
if [[ -n "$NDK_ROOT" ]]; then
    INFERNO="$NDK_ROOT/simpleperf/inferno.sh"
fi

if [[ ! -f "${INFERNO}" ]]; then
    echo "WARNING: simpleperf inferno not found; will generate perf.data only." >&2
    INFERNO=""
fi

PYTHON_BIN="${PYTHON_BIN:-$(command -v python3 2>/dev/null || true)}"
if [[ -z "$PYTHON_BIN" ]]; then
    PYTHON_BIN="$(command -v python 2>/dev/null || true)"
fi

PERF_BIN="${PERF_BIN:-$(command -v perf 2>/dev/null || true)}"
if [[ -z "$PERF_BIN" ]]; then
    echo "ERROR: perf not found on PATH. Install linux-tools or equivalent." >&2
    exit 1
fi

echo "Workspace folder: $WORKSPACE_FOLDER"
echo "Output directory: $OUTPUT_DIR"
echo "Linux binary: $BINARY_PATH"
echo "Using perf: $PERF_BIN"
if [[ -n "$INFERNO" ]]; then
    echo "Using inferno: $INFERNO"
fi

pushd "$OUTPUT_DIR" > /dev/null
rm -f perf.data flamegraph.html
trap "popd > /dev/null" EXIT

echo "Recording $DURATION_SECONDS seconds of profiling data..."

set +e
perf_output=$("$PERF_BIN" record -F "$SAMPLE_FREQUENCY" -g -o perf.data -- timeout "$DURATION_SECONDS" "$BINARY_PATH" 2>&1)
perf_status=$?
set -e

if [[ $perf_status -ne 0 ]] && echo "$perf_output" | grep -Eq "Access to performance monitoring and observability operations is limited|perf_event_paranoid setting is"; then
    echo ""
    echo "ERROR: perf profiling is blocked by kernel security settings (perf_event_paranoid)." >&2
    echo "To fix, run one of:" >&2
    echo "  1. Lower the security restriction: sudo sysctl -w kernel.perf_event_paranoid=1" >&2
    echo "  2. Re-run this task with sudo" >&2
    echo "  3. Permanently change setting in /etc/sysctl.conf: kernel.perf_event_paranoid=1" >&2
    exit 1
fi

if [[ ! -f perf.data ]]; then
    if [[ -n "$perf_output" ]]; then
        echo "ERROR: perf record failed:" >&2
        echo "$perf_output" >&2
    else
        echo "ERROR: perf.data was not generated." >&2
    fi
    exit 1
fi

echo "Perf data collected successfully."

if [[ -n "$INFERNO" ]] && [[ -n "$PYTHON_BIN" ]]; then
    echo "Generating flame graph..."
    "$INFERNO" -sc --record_file perf.data --no_browser --one-flamegraph -o flamegraph.html --title "$TITLE"
    echo "Flame graph generated: $OUTPUT_DIR/flamegraph.html"
else
    echo "WARNING: inferno not available; perf.data only." >&2
    echo "To generate flame graph, install Android NDK or use: perf report -g" >&2
fi

echo "Perf data saved: $OUTPUT_DIR/perf.data"
