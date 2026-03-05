param(
    [string]$AdbPath = 'F:/Android/Sdk/platform-tools/adb.exe',
    [string]$PackageActivity = 'com.nolimitconnect.nolimitconnect/org.qtproject.qt.android.bindings.QtActivity',
    [int]$LldbPort = 5039
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path $AdbPath)) {
    throw "adb not found at path: $AdbPath"
}

& $AdbPath start-server | Out-Null

$deviceLines = (& $AdbPath devices) |
    Select-Object -Skip 1 |
    Where-Object { $_ -match '\S' }

$devices = @()
foreach ($line in $deviceLines) {
    if ($line -match '^(\S+)\s+device$') {
        $devices += $Matches[1]
    }
}

if ($devices.Count -eq 0) {
    throw 'No Android device in state device.'
}

if ($env:ANDROID_SERIAL) {
    $serial = $env:ANDROID_SERIAL
} else {
    $serial = $devices[0]
    if ($devices.Count -gt 1) {
        Write-Host ("Multiple devices detected: {0}. Using first device: {1}" -f ($devices -join ', '), $serial)
    }
}

& $AdbPath -s $serial forward --remove tcp:$LldbPort 2>$null

& $AdbPath -s $serial shell am start -n $PackageActivity
if ($LASTEXITCODE -ne 0) {
    Write-Host 'adb am start returned non-zero; continuing to debugger attach.'
}

& $AdbPath -s $serial forward tcp:$LldbPort tcp:$LldbPort
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
exit 0
