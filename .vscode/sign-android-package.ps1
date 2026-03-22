param(
    [string]$WorkspaceRoot = $(Split-Path -Parent $PSScriptRoot),
    [string]$InputApk = "",
    [string]$OutputApk = ""
)

$ErrorActionPreference = 'Stop'

function Get-RequiredEnv {
    param([string]$Name)

    $value = [Environment]::GetEnvironmentVariable($Name)
    if ([string]::IsNullOrWhiteSpace($value)) {
        throw "Required environment variable '$Name' is not set."
    }

    return $value
}

function Get-AndroidSdkRoot {
    $candidates = @(
        [Environment]::GetEnvironmentVariable('ANDROID_SDK_ROOT'),
        [Environment]::GetEnvironmentVariable('ANDROID_HOME'),
        'F:\Android\Sdk'
    )

    foreach ($candidate in $candidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and (Test-Path $candidate)) {
            return $candidate
        }
    }

    throw 'Unable to locate Android SDK root. Set ANDROID_SDK_ROOT or ANDROID_HOME.'
}

function Get-LatestBuildToolsDir {
    param([string]$SdkRoot)

    $buildToolsRoot = Join-Path $SdkRoot 'build-tools'
    if (-not (Test-Path $buildToolsRoot)) {
        throw "Android build-tools directory not found: $buildToolsRoot"
    }

    $latest = Get-ChildItem -Path $buildToolsRoot -Directory |
        Sort-Object { [version]$_.Name } -Descending |
        Select-Object -First 1

    if (-not $latest) {
        throw "No Android build-tools versions found in $buildToolsRoot"
    }

    return $latest.FullName
}

function Get-DefaultInputApk {
    param([string]$PackageDir)

    if (-not (Test-Path $PackageDir)) {
        throw "Android package directory not found: $PackageDir"
    }

    $apk = Get-ChildItem -Path $PackageDir -File -Filter '*.apk' |
        Where-Object { $_.Name -notmatch '-signed\.apk$' } |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1

    if (-not $apk) {
        throw "No unsigned Android APK found in $PackageDir. Run 'Package Android Signed' first so it can build the release APK before signing."
    }

    return $apk.FullName
}

$packageDir = Join-Path $WorkspaceRoot 'package\android'
$sourceApk = if ([string]::IsNullOrWhiteSpace($InputApk)) { Get-DefaultInputApk -PackageDir $packageDir } else { $InputApk }

if (-not (Test-Path $sourceApk)) {
    throw "Input APK not found: $sourceApk"
}

if ([string]::IsNullOrWhiteSpace($OutputApk)) {
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($sourceApk)
    $OutputApk = Join-Path ([System.IO.Path]::GetDirectoryName($sourceApk)) ($baseName + '-signed.apk')
}

$keystorePath = Get-RequiredEnv -Name 'NLC_ANDROID_KEYSTORE_PATH'
$null = Get-RequiredEnv -Name 'NLC_ANDROID_KEYSTORE_PASSWORD'
$keyAlias = Get-RequiredEnv -Name 'NLC_ANDROID_KEY_ALIAS'

# NLC_ANDROID_KEY_PASSWORD is optional - falls back to the keystore password
# (the common case when both were set to the same value during keytool -genkey)
$keyPassword = [Environment]::GetEnvironmentVariable('NLC_ANDROID_KEY_PASSWORD')
if ([string]::IsNullOrWhiteSpace($keyPassword)) {
    $env:NLC_ANDROID_KEY_PASSWORD = [Environment]::GetEnvironmentVariable('NLC_ANDROID_KEYSTORE_PASSWORD')
    Write-Host "  NLC_ANDROID_KEY_PASSWORD not set - falling back to NLC_ANDROID_KEYSTORE_PASSWORD"
}

if (-not (Test-Path $keystorePath)) {
    throw "Keystore file not found: $keystorePath"
}

$sdkRoot = Get-AndroidSdkRoot
$buildToolsDir = Get-LatestBuildToolsDir -SdkRoot $sdkRoot
$zipalignPath = Join-Path $buildToolsDir 'zipalign.exe'
$apksignerPath = Join-Path $buildToolsDir 'apksigner.bat'

if (-not (Test-Path $zipalignPath)) {
    throw "zipalign not found: $zipalignPath"
}
if (-not (Test-Path $apksignerPath)) {
    throw "apksigner not found: $apksignerPath"
}

$alignedApk = Join-Path ([System.IO.Path]::GetDirectoryName($OutputApk)) (([System.IO.Path]::GetFileNameWithoutExtension($OutputApk)) + '.aligned.apk')

Write-Host "Signing Android package"
Write-Host "  Source: $sourceApk"
Write-Host "  Output: $OutputApk"
Write-Host "  Build-tools: $buildToolsDir"

if (Test-Path $alignedApk) {
    Remove-Item -Path $alignedApk -Force
}
if (Test-Path $OutputApk) {
    Remove-Item -Path $OutputApk -Force
}

try {
    & $zipalignPath -f -p 4 $sourceApk $alignedApk
    if ($LASTEXITCODE -ne 0) {
        throw "zipalign failed with exit code $LASTEXITCODE"
    }

    & $apksignerPath sign --ks $keystorePath --ks-key-alias $keyAlias --ks-pass env:NLC_ANDROID_KEYSTORE_PASSWORD --key-pass env:NLC_ANDROID_KEY_PASSWORD --out $OutputApk $alignedApk
    if ($LASTEXITCODE -ne 0) {
        throw "apksigner sign failed with exit code $LASTEXITCODE"
    }

    & $apksignerPath verify --print-certs $OutputApk
    if ($LASTEXITCODE -ne 0) {
        throw "apksigner verify failed with exit code $LASTEXITCODE"
    }
}
finally {
    if (Test-Path $alignedApk) {
        Remove-Item -Path $alignedApk -Force
    }
}

Write-Host "Signed Android package ready: $OutputApk"