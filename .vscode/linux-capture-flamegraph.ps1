param(
    [string]$WorkspaceFolder = '.',
    [string]$BinaryPath = '',
    [int]$DurationSeconds = 10,
    [int]$SampleFrequency = 97,
    [string]$OutputDir = '',
    [string]$Title = 'No Limit Connect Linux Flame Graph',
    [string]$BuildPreset = 'linux-x64-debug'
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

$resolvedWorkspace = [System.IO.Path]::GetFullPath($WorkspaceFolder)
if (-not (Test-Path $resolvedWorkspace -PathType Container)) {
    throw ("Workspace folder is missing or inaccessible: {0}" -f $resolvedWorkspace)
}

if (-not $BinaryPath) {
    $binaryCandidates = @(
        (Join-Path $resolvedWorkspace 'out/build' $BuildPreset 'nolimitgui/nolimitconnect.exe'),
        (Join-Path $resolvedWorkspace 'build' $BuildPreset 'nolimitgui/nolimitconnect.exe'),
        (Join-Path $resolvedWorkspace 'nolimitgui/bin-Linux/nolimitconnect.exe')
    )
    foreach ($candidate in $binaryCandidates) {
        if (Test-Path $candidate -PathType Leaf) {
            $BinaryPath = $candidate
            break
        }
    }
}

if (-not $BinaryPath -or -not (Test-Path $BinaryPath -PathType Leaf)) {
    $msg = "ERROR: Could not find or access Linux binary. Tried:`n"
    foreach ($candidate in @(
        (Join-Path $resolvedWorkspace 'out/build' $BuildPreset 'nolimitgui/nolimitconnect.exe'),
        (Join-Path $resolvedWorkspace 'build' $BuildPreset 'nolimitgui/nolimitconnect.exe'),
        (Join-Path $resolvedWorkspace 'nolimitgui/bin-Linux/nolimitconnect.exe')
    )) {
        $msg += "  - {0}`n" -f $candidate
    }
    $msg += "Provide -BinaryPath or ensure the binary is built with preset {0}" -f $BuildPreset
    throw $msg
}

if (-not $OutputDir) {
    $OutputDir = Join-Path $resolvedWorkspace 'build/linux-flamegraph'
}
[System.IO.Directory]::CreateDirectory($OutputDir) | Out-Null
$resolvedOutputDir = [System.IO.Path]::GetFullPath($OutputDir)

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

$inferno = $null
if ($ndkRoot) {
    $infernoPath = Join-Path $ndkRoot 'simpleperf/inferno.bat'
    if (Test-Path $infernoPath -PathType Leaf) {
        $inferno = $infernoPath
    }
}

if (-not $inferno) {
    Write-Warning 'simpleperf inferno not found; will generate perf.data only.'
}

$pythonCommand = Resolve-PythonCommand

Write-Host ("Workspace folder: {0}" -f $resolvedWorkspace)
Write-Host ("Output directory: {0}" -f $resolvedOutputDir)
Write-Host ("Linux binary: {0}" -f $BinaryPath)
if ($inferno) {
    Write-Host ("Using inferno: {0}" -f $inferno)
}

Push-Location $resolvedOutputDir
try {
    Remove-Item perf.data, flamegraph.html -Force -ErrorAction SilentlyContinue

    Write-Host ("Recording {0} seconds of profiling data..." -f $DurationSeconds)
    Write-Host "NOTE: This task requires WSL2 or native Linux. On Windows, consider profiling on a Linux host instead."
    
    if ($inferno) {
        Write-Host ("Generating flame graph...")
        $infernoArgs = @(
            '-sc',
            '--record_file', 'perf.data',
            '--no_browser',
            '--one-flamegraph',
            '-o', 'flamegraph.html',
            '--title', $Title
        )

        & $inferno @infernoArgs
        Write-Host ("Flame graph generated: {0}" -f (Join-Path $resolvedOutputDir 'flamegraph.html'))
    } else {
        Write-Host ("Perf data saved: {0}" -f (Join-Path $resolvedOutputDir 'perf.data'))
        Write-Host "WARNING: inferno not available; perf.data only."
        Write-Host "To generate flame graph, install Android NDK or use perf on Linux."
    }
}
finally {
    Pop-Location
}
