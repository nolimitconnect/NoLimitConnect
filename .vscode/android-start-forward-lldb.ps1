param(
    [string]$AdbPath = 'F:/Android/Sdk/platform-tools/adb.exe',
    [string]$PackageActivity = 'com.nolimitconnect.nolimitconnect/org.qtproject.qt.android.bindings.QtActivity',
    [int]$LldbPort = 5039,
    [int]$DebugSocketWaitSeconds = 15,
    [int]$DeviceWaitSeconds = 45,
    [string]$WorkspaceFolder = ''
)

$ErrorActionPreference = 'Stop'

if ($WorkspaceFolder) {
    $resolvedWorkspace = [System.IO.Path]::GetFullPath($WorkspaceFolder)
    Write-Host ("Workspace folder (resolved): {0}" -f $resolvedWorkspace)
    if (-not (Test-Path $resolvedWorkspace -PathType Container)) {
        throw ("Workspace folder is missing or inaccessible: {0}" -f $resolvedWorkspace)
    }
}

if (-not (Test-Path $AdbPath)) {
    throw "adb not found at path: $AdbPath"
}

& $AdbPath start-server | Out-Null

function Get-DeviceState {
    param(
        [string]$Adb,
        [string]$Serial
    )

    if (-not $Serial) {
        return $null
    }

    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    $state = & $Adb -s $Serial get-state 2>$null
    $stateExitCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference

    if ($stateExitCode -ne 0 -or -not $state) {
        return $null
    }

    return $state.Trim()
}

function Get-ConnectedDeviceSerials {
    param(
        [string]$Adb
    )

    $lines = (& $Adb devices) |
        Select-Object -Skip 1 |
        Where-Object { $_ -match '\S' }

    $serials = @()
    foreach ($line in $lines) {
        if ($line -match '^(\S+)\s+device$') {
            $serials += $Matches[1]
        }
    }

    return $serials
}

function Wait-ForDeviceState {
    param(
        [string]$Adb,
        [string]$Serial,
        [int]$TimeoutSeconds
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        $state = Get-DeviceState -Adb $Adb -Serial $Serial
        if ($state -eq 'device') {
            return $true
        }

        if ($state) {
            Write-Host ("Waiting for Android device {0}: current state '{1}'" -f $Serial, $state)
        } else {
            Write-Host ("Waiting for Android device {0}: current state unavailable" -f $Serial)
        }

        Start-Sleep -Seconds 1
    }

    return $false
}

$devices = Get-ConnectedDeviceSerials -Adb $AdbPath

if ($devices.Count -eq 0) {
    if ($env:ANDROID_SERIAL) {
        $serial = $env:ANDROID_SERIAL
        Write-Host ("No device currently in 'device' state; waiting up to {0}s for configured serial: {1}" -f $DeviceWaitSeconds, $serial)
        if (-not (Wait-ForDeviceState -Adb $AdbPath -Serial $serial -TimeoutSeconds $DeviceWaitSeconds)) {
            $devices = Get-ConnectedDeviceSerials -Adb $AdbPath
            if ($devices.Count -gt 0) {
                $serial = $devices[0]
                Write-Host ("Configured serial {0} unavailable; falling back to connected device: {1}" -f $env:ANDROID_SERIAL, $serial)
            } else {
                throw ("Android device '{0}' did not reach state 'device' within {1} seconds." -f $serial, $DeviceWaitSeconds)
            }
        }
    } else {
        throw 'No Android device in state device.'
    }
}

if ($env:ANDROID_SERIAL) {
    if (-not $serial) {
        $serial = $env:ANDROID_SERIAL
    }
} else {
    $serial = $devices[0]
    if ($devices.Count -gt 1) {
        Write-Host ("Multiple devices detected: {0}. Using first device: {1}" -f ($devices -join ', '), $serial)
    }
}

if (-not (Wait-ForDeviceState -Adb $AdbPath -Serial $serial -TimeoutSeconds $DeviceWaitSeconds)) {
    throw ("Android device '{0}' is not in state 'device'. Check USB mode/authorization." -f $serial)
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

Write-Host ("Force-stopping existing app instance for package: {0}" -f $packageName)
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

function Test-LldbServerRunning {
    param(
        [string]$Adb,
        [string]$DeviceSerial,
        [string]$Pkg
    )

    $pidOutput = & $Adb -s $DeviceSerial shell "run-as $Pkg sh -c 'pidof lldb-server'" 2>$null
    if ($LASTEXITCODE -eq 0 -and $pidOutput) {
        $trimmed = $pidOutput.Trim()
        if ($trimmed -match '^\d+(\s+\d+)*$') {
            return $true
        }
    }

    # Fallback for devices where pidof is unavailable in run-as shell.
    $psOutput = & $Adb -s $DeviceSerial shell "run-as $Pkg sh -c 'ps -A | grep lldb-server'" 2>$null
    if ($LASTEXITCODE -eq 0 -and $psOutput -and ($psOutput -match 'lldb-server')) {
        return $true
    }

    return $false
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
    if ($LASTEXITCODE -ne 0) {
        Start-Sleep -Seconds 1
        continue
    }

    & $AdbPath -s $serial forward tcp:$LldbPort tcp:$LldbPort
    if ($LASTEXITCODE -eq 0 -and (Test-LldbServerRunning -Adb $AdbPath -DeviceSerial $serial -Pkg $packageName)) {
        $forwarded = $true
        break
    }

    & $AdbPath -s $serial forward --remove tcp:$LldbPort 2>$null | Out-Null
    Start-Sleep -Seconds 1
}

if (-not $forwarded) {
    throw "Failed to start/attach lldb-server for package $packageName (last pid tried: $lastPidTried)"
}

Write-Host ("Forwarded tcp:{0} to device tcp:{0} (attached pid: {1})" -f $LldbPort, $lastPidTried)
exit 0
