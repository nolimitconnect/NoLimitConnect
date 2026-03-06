param(
    [string]$AdbPath = 'F:/Android/Sdk/platform-tools/adb.exe',
    [string]$PackageActivity = 'com.nolimitconnect.nolimitconnect/org.qtproject.qt.android.bindings.QtActivity',
    [int]$LldbPort = 5039,
    [int]$DebugSocketWaitSeconds = 15
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

Write-Host ("Using Android device serial: {0}" -f $serial)

$previousErrorActionPreference = $ErrorActionPreference
$ErrorActionPreference = 'Continue'
& $AdbPath -s $serial forward --remove tcp:$LldbPort 2>$null
$removeForwardExitCode = $LASTEXITCODE
$ErrorActionPreference = $previousErrorActionPreference
if ($removeForwardExitCode -ne 0) {
    Write-Host ("No existing adb forward to remove on tcp:{0}; continuing." -f $LldbPort)
}

$packageName = $PackageActivity.Split('/')[0]

& $AdbPath -s $serial shell am force-stop $packageName
if ($LASTEXITCODE -ne 0) {
    Write-Host ("adb force-stop for {0} returned non-zero; continuing." -f $packageName)
}
Start-Sleep -Milliseconds 500

& $AdbPath -s $serial shell am start -n $PackageActivity
if ($LASTEXITCODE -ne 0) {
    Write-Host 'adb am start returned non-zero; continuing to debugger attach.'
}

function Get-AppPid {
    param(
        [string]$Adb,
        [string]$DeviceSerial,
        [string]$Pkg
    )

    $pidOutput = & $Adb -s $DeviceSerial shell "pidof $Pkg" 2>$null
    if ($LASTEXITCODE -ne 0 -or -not $pidOutput) {
        return $null
    }

    $trimmed = $pidOutput.Trim()
    if (-not $trimmed) {
        return $null
    }

    # pidof can return multiple PIDs; use the first one.
    return ($trimmed -split '\s+')[0]
}

$forwarded = $false
$lastPidTried = $null
for ($attempt = 1; $attempt -le $DebugSocketWaitSeconds; $attempt++) {
    $appPid = Get-AppPid -Adb $AdbPath -DeviceSerial $serial -Pkg $packageName

    if (-not $appPid) {
        Start-Sleep -Seconds 1
        continue
    }

    $lastPidTried = $appPid

    # Start lldb-server inside app context and attach to the running process.
    & $AdbPath -s $serial shell "run-as $packageName sh -c './lldb-server gdbserver --attach $appPid localhost:$LldbPort >/dev/null 2>&1 &'" | Out-Null

    & $AdbPath -s $serial forward tcp:$LldbPort tcp:$LldbPort
    if ($LASTEXITCODE -eq 0) {
        $forwarded = $true
        break
    }

    Start-Sleep -Seconds 1
}

if (-not $forwarded) {
    throw "Failed to start/attach lldb-server for package $packageName (last pid tried: $lastPidTried)"
}

Write-Host ("Forwarded tcp:{0} to device tcp:{0} (attached pid: {1})" -f $LldbPort, $lastPidTried)
exit 0
