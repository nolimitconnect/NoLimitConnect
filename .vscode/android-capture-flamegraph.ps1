param(
    [string]$WorkspaceFolder = '.',
    [string]$PackageName = 'org.nolimitconnect.nolimitconnect',
    [string]$ActivityName = 'org.qtproject.qt.android.bindings.QtActivity',
    [int]$DurationSeconds = 10,
    [int]$SampleFrequency = 1000,
    [string]$OutputDir = '',
    [string]$Title = 'No Limit Connect Android Flame Graph',
    [switch]$SkipCollection
)

$ErrorActionPreference = 'Stop'

function Get-FirstExistingPath {
    param(
        [string[]]$Candidates,
        [switch]$Directory
    )

    foreach ($candidate in $Candidates) {
        if (-not $candidate) {
            continue
        }

        if ($Directory) {
            if (Test-Path $candidate -PathType Container) {
                return $candidate
            }
        } elseif (Test-Path $candidate -PathType Leaf) {
            return $candidate
        }
    }

    return $null
}

function Resolve-PythonCommand {
    $commands = @('python', 'python3', 'py')
    foreach ($command in $commands) {
        $resolved = Get-Command $command -ErrorAction SilentlyContinue
        if ($resolved) {
            return $resolved.Source
        }
    }

    return $null
}

function Invoke-PythonScript {
    param(
        [string]$PythonCommand,
        [string[]]$Arguments
    )

    if ([System.IO.Path]::GetFileNameWithoutExtension($PythonCommand).ToLowerInvariant() -eq 'py') {
        & $PythonCommand -3 @Arguments
        return
    }

    & $PythonCommand @Arguments
}

$resolvedWorkspace = [System.IO.Path]::GetFullPath($WorkspaceFolder)
if (-not (Test-Path $resolvedWorkspace -PathType Container)) {
    throw ("Workspace folder is missing or inaccessible: {0}" -f $resolvedWorkspace)
}

if (-not $OutputDir) {
    $OutputDir = Join-Path $resolvedWorkspace 'build/android-flamegraph'
}
[System.IO.Directory]::CreateDirectory($OutputDir) | Out-Null
$resolvedOutputDir = [System.IO.Path]::GetFullPath($OutputDir)

$adbPath = $env:ADB_PATH
if (-not $adbPath) {
    $adbCommand = Get-Command adb -ErrorAction SilentlyContinue
    if ($adbCommand) {
        $adbPath = $adbCommand.Source
    }
}
if (-not $adbPath) {
    $adbPath = Get-FirstExistingPath -Candidates @(
        (Join-Path $env:ANDROID_SDK_ROOT 'platform-tools/adb.exe'),
        'F:/Android/Sdk/platform-tools/adb.exe'
    )
}
if (-not $adbPath) {
    throw 'adb not found. Set ADB_PATH or ANDROID_SDK_ROOT.'
}

$ndkRoot = $env:ANDROID_NDK
if (-not $ndkRoot) {
    $ndkRoot = $env:CMAKE_ANDROID_NDK
}
if (-not $ndkRoot) {
    $ndkRoot = Get-FirstExistingPath -Directory -Candidates @(
        (Join-Path $env:ANDROID_SDK_ROOT 'ndk/27.2.12479018'),
        (Join-Path $env:ANDROID_SDK_ROOT 'ndk/26.1.10909125'),
        'F:/Android/Sdk/ndk/27.2.12479018',
        'F:/Android/Sdk/ndk/26.1.10909125'
    )
}
if (-not $ndkRoot) {
    throw 'Android NDK not found. Set ANDROID_NDK or CMAKE_ANDROID_NDK.'
}

$simpleperfDir = Join-Path $ndkRoot 'simpleperf'
$appProfiler = Join-Path $simpleperfDir 'app_profiler.py'
$inferno = Join-Path $simpleperfDir 'inferno.bat'
if (-not (Test-Path $appProfiler -PathType Leaf)) {
    throw ("simpleperf app_profiler.py not found under: {0}" -f $simpleperfDir)
}
if (-not (Test-Path $inferno -PathType Leaf)) {
    throw ("simpleperf inferno.bat not found under: {0}" -f $simpleperfDir)
}

$pythonCommand = Resolve-PythonCommand
if (-not $pythonCommand) {
    throw 'python/python3/py not found on PATH.'
}

$nativeLibDir = $null
$debugBuildDir = Join-Path $resolvedWorkspace 'build/android-arm64-debug'
$releaseBuildDir = Join-Path $resolvedWorkspace 'build/android-arm64-release'
if (Test-Path $debugBuildDir -PathType Container) {
    $nativeLibDir = $debugBuildDir
} elseif (Test-Path $releaseBuildDir -PathType Container) {
    $nativeLibDir = $releaseBuildDir
}

Write-Host ("Workspace folder: {0}" -f $resolvedWorkspace)
Write-Host ("Output directory: {0}" -f $resolvedOutputDir)
Write-Host ("Using adb: {0}" -f $adbPath)
Write-Host ("Using NDK: {0}" -f $ndkRoot)
if ($nativeLibDir) {
    Write-Host ("Using native libraries: {0}" -f $nativeLibDir)
} else {
    Write-Warning 'Android build directory not found. Profiling will continue without host-side unstripped libraries.'
}

& $adbPath start-server | Out-Null

$adbDeviceArgs = @()
if ($env:ANDROID_SERIAL) {
    Write-Host ("Using Android device serial: {0}" -f $env:ANDROID_SERIAL)
    $adbDeviceArgs = @('-s', $env:ANDROID_SERIAL)
}

$activityComponent = '{0}/{1}' -f $PackageName, $ActivityName
if (-not $SkipCollection) {
    Write-Host ("Starting activity: {0}" -f $activityComponent)
    & $adbPath @adbDeviceArgs shell am start -n $activityComponent | Out-Null

    Push-Location $resolvedOutputDir
    try {
        Remove-Item perf.data, flamegraph.html -Force -ErrorAction SilentlyContinue
        Remove-Item binary_cache -Recurse -Force -ErrorAction SilentlyContinue

        $recordOptions = "-e task-clock:u -f $SampleFrequency -g --duration $DurationSeconds"
        $profilerArgs = @(
            $appProfiler,
            '-p', $PackageName,
            '-r', $recordOptions,
            '-o', 'perf.data',
            '--ndk_path', $ndkRoot
        )
        if ($nativeLibDir) {
            $profilerArgs += @('-lib', $nativeLibDir)
        }

        Invoke-PythonScript -PythonCommand $pythonCommand -Arguments $profilerArgs
    }
    finally {
        Pop-Location
    }
} elseif (-not (Test-Path (Join-Path $resolvedOutputDir 'perf.data') -PathType Leaf)) {
    throw ("-SkipCollection was requested but {0} does not exist." -f (Join-Path $resolvedOutputDir 'perf.data'))
}

Push-Location $resolvedOutputDir
try {
    $infernoArgs = @(
        '-sc',
        '--record_file', 'perf.data',
        '--no_browser',
        '--one-flamegraph',
        '-o', 'flamegraph.html',
        '--title', $Title
    )
    if ($nativeLibDir) {
        $infernoArgs += @('--symfs', $nativeLibDir)
    }

    & $inferno @infernoArgs
}
finally {
    Pop-Location
}

Write-Host ("Flame graph generated: {0}" -f (Join-Path $resolvedOutputDir 'flamegraph.html'))
Write-Host ("Perf data saved: {0}" -f (Join-Path $resolvedOutputDir 'perf.data'))
if (Test-Path (Join-Path $resolvedOutputDir 'binary_cache') -PathType Container) {
    Write-Host ("Collected binaries: {0}" -f (Join-Path $resolvedOutputDir 'binary_cache'))
}