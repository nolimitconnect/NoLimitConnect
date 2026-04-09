#!/usr/bin/env bash
# Opens a .ui file in Qt Creator design mode.
# Usage: open-ui-in-qtcreator.sh <path-to-ui-file>

UI_FILE="$1"
LOG="/tmp/open-ui-in-qtcreator.log"

if [ -z "$UI_FILE" ]; then
    echo "Error: No file specified." >&2
    exit 1
fi

if [ ! -f "$UI_FILE" ]; then
    echo "Error: File not found: $UI_FILE" >&2
    exit 1
fi

EXT="${UI_FILE##*.}"
if [ "$EXT" != "ui" ]; then
    echo "Error: Active file is not a .ui file: $UI_FILE" >&2
    exit 1
fi

launch() {
    local EXEC="$1"
    {
        echo "[$(date)] Launching: $EXEC $UI_FILE"
        echo "  DISPLAY=$DISPLAY WAYLAND_DISPLAY=$WAYLAND_DISPLAY"
    } >> "$LOG"
    setsid "$EXEC" "$UI_FILE" >> "$LOG" 2>&1 &
    disown
    echo "[$(date)] PID: $!" >> "$LOG"
}

# Try PATH first
if command -v qtcreator &>/dev/null; then
    launch qtcreator
    exit 0
fi

# Common install locations
CANDIDATES=(
    "/home/nolimit/Qt/Tools/QtCreator/bin/qtcreator"
    "/opt/Qt/Tools/QtCreator/bin/qtcreator"
    "/usr/bin/qtcreator"
    "/usr/local/bin/qtcreator"
)

for CANDIDATE in "${CANDIDATES[@]}"; do
    if [ -x "$CANDIDATE" ]; then
        launch "$CANDIDATE"
        exit 0
    fi
done

echo "Error: Qt Creator not found in PATH or common install locations." >&2
echo "[$(date)] Error: Qt Creator not found." >> "$LOG"
exit 1
