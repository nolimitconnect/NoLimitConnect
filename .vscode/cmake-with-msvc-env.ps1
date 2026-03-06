param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$CMakeArgs
)

if (-not $CMakeArgs -or $CMakeArgs.Count -eq 0) {
    Write-Error "No CMake arguments provided to cmake-with-msvc-env.ps1"
    exit 1
}

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Error "vswhere.exe not found at '$vswhere'. Install Visual Studio 2022 Build Tools with C++ workload."
    exit 1
}

$installPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $installPath) {
    Write-Error "No Visual Studio installation with VC++ tools found."
    exit 1
}

$vsDevCmd = Join-Path $installPath "Common7\Tools\VsDevCmd.bat"
if (-not (Test-Path $vsDevCmd)) {
    Write-Error "VsDevCmd.bat not found at '$vsDevCmd'."
    exit 1
}

$escapedArgs = @()
foreach ($arg in $CMakeArgs) {
    $escaped = $arg.Replace('"', '\"')
    if ($escaped -match '[\s"]') {
        $escapedArgs += '"' + $escaped + '"'
    } else {
        $escapedArgs += $escaped
    }
}
$cmakeArgLine = ($escapedArgs -join ' ')

$cmdLine = '"' + $vsDevCmd + '" -arch=x64 -host_arch=x64 >nul && cmake ' + $cmakeArgLine
& cmd.exe /c $cmdLine
exit $LASTEXITCODE
