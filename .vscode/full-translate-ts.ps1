#!/usr/bin/env powershell
# Full translation pipeline: extract -> translate -> compile

param(
    [string]$WorkspaceFolder = ${env:WORKSPACE_FOLDER}
)

$ErrorActionPreference = "Stop"

Write-Host "=== Starting Full Translation Pipeline ==="
Write-Host ""

# Step 1: Extract translations from UI files (lupdate)
Write-Host "Step 1: Extracting translation strings (lupdate)..."
& "$WorkspaceFolder\.vscode\cmake-with-msvc-env.ps1" --build --preset windows-debug --target nolimitconnect_lupdate
if ($LASTEXITCODE -ne 0) {
    Write-Error "lupdate failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}
Write-Host "Extraction complete"
Write-Host ""

# Step 2: Translate to all languages (Python script)
Write-Host "Step 2: Translating to all languages..."
python "$WorkspaceFolder\tools\update_ts_translations.py"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Translation failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}
Write-Host "Translation complete"
Write-Host ""

# Step 3: Compile to QM files (lrelease)
Write-Host "Step 3: Compiling to binary QM files (lrelease)..."
& "$WorkspaceFolder\.vscode\cmake-with-msvc-env.ps1" --build --preset windows-debug --target nolimitconnect_translations
if ($LASTEXITCODE -ne 0) {
    Write-Error "lrelease failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}
Write-Host "Compilation complete"
Write-Host ""

Write-Host "=== Translation Pipeline Complete ==="
Write-Host "All .qm files have been updated"
