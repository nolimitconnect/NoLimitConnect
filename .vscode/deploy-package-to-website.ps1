param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('windows', 'linux', 'android-signed', 'flatpak')]
    [string]$PackageType,

    [string]$WorkspaceRoot = '',
    [string]$WebsiteRoot = $(if ($env:NLC_WEBSITE_ROOT) { $env:NLC_WEBSITE_ROOT } else { 'F:\nlc-nolimitconnect.com' }),
    [string]$GitLabProjectPath = 'nolimitcode/nolimitconnect',
    [string]$GitLabBaseUrl = 'https://gitlab.com',
    [switch]$SkipWebsiteUpdate,
    [ValidateRange(0, 100)]
    [int]$KeepLatestVersions = 0
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($WorkspaceRoot)) {
    if (-not [string]::IsNullOrWhiteSpace($PSScriptRoot)) {
        $WorkspaceRoot = Split-Path -Parent $PSScriptRoot
    }
    elseif ($MyInvocation.MyCommand.Path) {
        $WorkspaceRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
    }
    else {
        $WorkspaceRoot = (Get-Location).Path
    }
}

function Get-PackageConfig {
    param([string]$Name)

    $configs = @{
        'windows' = @{
            SourceDir   = 'package\windows'
            Include     = @('*.exe')
            Exclude     = @()
            DisplayName = 'Windows'
            Notes       = 'NSIS installer for Windows x64.'
        }
        'linux' = @{
            SourceDir   = 'package\linux'
            Include     = @('*.deb')
            Exclude     = @()
            DisplayName = 'Linux'
            Notes       = 'Debian package for Linux.'
        }
        'android-signed' = @{
            SourceDir   = 'package\android'
            Include     = @('*-signed.apk')
            Exclude     = @()
            DisplayName = 'Android'
            Notes       = 'Signed APK intended for release distribution.'
        }
        'flatpak' = @{
            SourceDir   = 'package\flatpack'
            Include     = @('*.flatpak')
            Exclude     = @()
            DisplayName = 'Flatpak'
            Notes       = 'Flatpak bundle for Linux desktops with Flatpak support.'
        }
    }

    return $configs[$Name]
}

function Get-LatestArtifact {
    param(
        [string]$Directory,
        [string[]]$Include,
        [string[]]$Exclude
    )

    if (-not (Test-Path $Directory)) {
        throw "Package directory not found: $Directory"
    }

    $files = Get-ChildItem -Path $Directory -File | Where-Object {
        $includeMatch = $false
        foreach ($pattern in $Include) {
            if ($_.Name -like $pattern) {
                $includeMatch = $true
                break
            }
        }

        if (-not $includeMatch) { return $false }

        foreach ($pattern in $Exclude) {
            if ($_.Name -like $pattern) { return $false }
        }

        return $true
    } | Sort-Object LastWriteTime -Descending

    $latest = $files | Select-Object -First 1
    if (-not $latest) {
        $includeText = ($Include -join ', ')
        throw "No package artifact found in $Directory matching: $includeText"
    }

    return $latest
}

function Get-ProjectVersion {
    param([string]$Root)

    $versionFile = Join-Path $Root 'cmake\version.cmake'
    if (-not (Test-Path $versionFile)) {
        throw "Version file not found: $versionFile"
    }

    $content = Get-Content $versionFile -Raw
    if ($content -match 'set\(NLC_VERSION\s+([^\)]+)\)') {
        return $Matches[1].Trim()
    }

    throw "Could not parse NLC_VERSION from $versionFile"
}

function New-Sha256Sidecar {
    param(
        [string]$ArtifactPath,
        [string]$OutputDirectory
    )

    $artifactName = Split-Path -Leaf $ArtifactPath
    $hashFilePath = Join-Path $OutputDirectory ($artifactName + '.sha256')
    $hash = (Get-FileHash -Algorithm SHA256 -Path $ArtifactPath).Hash.ToLowerInvariant()
    $content = "$hash  $artifactName"
    Set-Content -Path $hashFilePath -Value $content -Encoding ascii
    return $hashFilePath
}

function Invoke-GitLabUpload {
    param(
        [string]$FilePath,
        [string]$BaseUrl,
        [string]$ProjectPath,
        [string]$PackageName,
        [string]$Version,
        [string]$Token
    )

    $encodedProject = [Uri]::EscapeDataString($ProjectPath)
    $fileName = Split-Path -Leaf $FilePath
    $uploadUrl = "$BaseUrl/api/v4/projects/$encodedProject/packages/generic/$PackageName/$Version/$fileName"

    Write-Host "  Uploading: $fileName"
    Write-Host "         to: $uploadUrl"

    $headers = @{ 'PRIVATE-TOKEN' = $Token }
    Invoke-RestMethod -Method Put -Uri $uploadUrl -Headers $headers -InFile $FilePath -ContentType 'application/octet-stream' | Out-Null

    return $uploadUrl
}

function Assert-GitLabPackageRegistryEnabled {
    param(
        [string]$BaseUrl,
        [string]$ProjectPath,
        [string]$Token
    )

    $encodedProject = [Uri]::EscapeDataString($ProjectPath)
    $projectUrl = "$BaseUrl/api/v4/projects/$encodedProject"
    $headers = @{ 'PRIVATE-TOKEN' = $Token }

    try {
        $project = Invoke-RestMethod -Method Get -Uri $projectUrl -Headers $headers
    }
    catch {
        throw "Could not read GitLab project '$ProjectPath'. Verify GITLAB_TOKEN and project path."
    }

    if ($project.package_registry_access_level -eq 'disabled' -or -not $project.packages_enabled) {
        $webProjectUrl = "$BaseUrl/$ProjectPath"
        throw @"
GitLab Generic Package Registry is disabled for project '$ProjectPath'.

Enable it in:
  $webProjectUrl/-/settings/general

Project feature to enable:
  Visibility, project features, permissions -> Package registry -> Enabled (or Everyone With Access)
"@
    }
}

function Remove-OldGitLabGenericPackageVersions {
    param(
        [string]$BaseUrl,
        [string]$ProjectPath,
        [string]$PackageName,
        [string]$Token,
        [int]$KeepLatest
    )

    if ($KeepLatest -le 0) {
        return 0
    }

    $encodedProject = [Uri]::EscapeDataString($ProjectPath)
    $encodedPackageName = [Uri]::EscapeDataString($PackageName)
    $listUrl = "$BaseUrl/api/v4/projects/$encodedProject/packages?package_type=generic&package_name=$encodedPackageName&order_by=created_at&sort=desc&per_page=100"
    $headers = @{ 'PRIVATE-TOKEN' = $Token }

    $packages = @(Invoke-RestMethod -Method Get -Uri $listUrl -Headers $headers)
    if ($packages.Count -eq 0) {
        return 0
    }

    $ordered = $packages |
        Where-Object { $_.name -eq $PackageName -and -not [string]::IsNullOrWhiteSpace($_.version) } |
        Sort-Object created_at -Descending

    $seenVersions = @{}
    $uniqueByVersion = @()
    foreach ($pkg in $ordered) {
        if (-not $seenVersions.ContainsKey($pkg.version)) {
            $seenVersions[$pkg.version] = $true
            $uniqueByVersion += $pkg
        }
    }

    $toDelete = @($uniqueByVersion | Select-Object -Skip $KeepLatest)
    foreach ($pkg in $toDelete) {
        $deleteUrl = "$BaseUrl/api/v4/projects/$encodedProject/packages/$($pkg.id)"
        Write-Host "  Removing old package version: $($pkg.version) (id=$($pkg.id))"
        Invoke-RestMethod -Method Delete -Uri $deleteUrl -Headers $headers | Out-Null
    }

    return $toDelete.Count
}

function New-LatestReleaseIndexJson {
    param(
        [string]$PackageType,
        [string]$DisplayName,
        [string]$Version,
        [string]$ArtifactName,
        [string]$ArtifactUrl,
        [string]$HashName,
        [string]$HashUrl,
        [string]$Notes,
        [string]$OutputDirectory
    )

    $timestamp = [System.DateTime]::UtcNow.ToString('o')
    $jsonObject = [ordered]@{
        schemaVersion = 1
        packageType = $PackageType
        displayName = $DisplayName
        version = $Version
        publishedAtUtc = $timestamp
        artifact = [ordered]@{
            name = $ArtifactName
            url = $ArtifactUrl
        }
        sha256 = [ordered]@{
            name = $HashName
            url = $HashUrl
        }
        notes = $Notes
    }

    $json = $jsonObject | ConvertTo-Json -Depth 6
    $filePath = Join-Path $OutputDirectory ($PackageType + '.json')
    Set-Content -Path $filePath -Value $json -Encoding utf8
    return $filePath
}

function Update-DownloadPageSection {
    param(
        [string]$WebsiteRepoRoot,
        [string]$SectionKey,
        [string]$DisplayName,
        [string]$ArtifactName,
        [string]$ArtifactUrl,
        [string]$HashName,
        [string]$HashUrl,
        [string]$Notes,
        [string]$Timestamp
    )

    $downloadPagePath = Join-Path $WebsiteRepoRoot 'docs\download.md'
    if (-not (Test-Path $downloadPagePath)) {
        throw "Website download page not found: $downloadPagePath"
    }

    $beginMarker = "<!-- BEGIN SECTION: $SectionKey -->"
    $endMarker   = "<!-- END SECTION: $SectionKey -->"
    $content = [System.IO.File]::ReadAllText($downloadPagePath)

    if (-not $content.Contains($beginMarker) -or -not $content.Contains($endMarker)) {
        throw "Per-section markers for '$SectionKey' are missing in $downloadPagePath. Expected: $beginMarker ... $endMarker"
    }

    $nl = [Environment]::NewLine
    $newSection = $beginMarker + $nl +
        "## $DisplayName" + $nl +
        "- Latest package: [$ArtifactName]($ArtifactUrl)" + $nl +
        "- SHA-256: [$HashName]($HashUrl)" + $nl +
        "- Last updated: $Timestamp" + $nl +
        "- Notes: $Notes" + $nl +
        $endMarker

    $pattern = '(?s)' + [regex]::Escape($beginMarker) + '.*?' + [regex]::Escape($endMarker)
    $updated = [regex]::Replace($content, $pattern, [System.Text.RegularExpressions.MatchEvaluator]{ param($m) $newSection }, 1)
    [System.IO.File]::WriteAllText($downloadPagePath, $updated, [System.Text.UTF8Encoding]::new($false))
}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

$gitLabToken = $env:GITLAB_TOKEN
if ([string]::IsNullOrWhiteSpace($gitLabToken)) {
    throw "GITLAB_TOKEN environment variable is not set. Set it to a GitLab Personal Access Token with 'api' scope before running a deploy task."
}

$config = Get-PackageConfig -Name $PackageType
if (-not $config) {
    throw "Unsupported package type: $PackageType"
}

$canUpdateWebsite = $false
if (-not $SkipWebsiteUpdate) {
    if (Test-Path $WebsiteRoot) {
        $canUpdateWebsite = $true
    }
    else {
        Write-Warning "Website repository not found at '$WebsiteRoot'. Continuing without website download page updates."
        Write-Warning "Use -SkipWebsiteUpdate to suppress this warning when running GitLab-only deploys."
    }
}

Assert-GitLabPackageRegistryEnabled -BaseUrl $GitLabBaseUrl -ProjectPath $GitLabProjectPath -Token $gitLabToken

$version     = Get-ProjectVersion -Root $WorkspaceRoot
$artifactDir = Join-Path $WorkspaceRoot $config.SourceDir
$artifact    = Get-LatestArtifact -Directory $artifactDir -Include $config.Include -Exclude $config.Exclude

# Generate SHA-256 sidecar in a temp directory
$tempDir = Join-Path ([System.IO.Path]::GetTempPath()) 'nlc-deploy'
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
$sha256FilePath = New-Sha256Sidecar -ArtifactPath $artifact.FullName -OutputDirectory $tempDir
$sha256FileName = Split-Path -Leaf $sha256FilePath

# Upload artifact and SHA-256 file to GitLab Generic Package Registry
$artifactUrl = Invoke-GitLabUpload -FilePath $artifact.FullName -BaseUrl $GitLabBaseUrl `
    -ProjectPath $GitLabProjectPath -PackageName $PackageType -Version $version -Token $gitLabToken

$hashUrl = Invoke-GitLabUpload -FilePath $sha256FilePath -BaseUrl $GitLabBaseUrl `
    -ProjectPath $GitLabProjectPath -PackageName $PackageType -Version $version -Token $gitLabToken

$indexPackageName = 'download-index'
$indexVersion = 'v1'
$latestIndexJsonPath = New-LatestReleaseIndexJson `
    -PackageType $PackageType `
    -DisplayName $config.DisplayName `
    -Version $version `
    -ArtifactName $artifact.Name `
    -ArtifactUrl $artifactUrl `
    -HashName $sha256FileName `
    -HashUrl $hashUrl `
    -Notes $config.Notes `
    -OutputDirectory $tempDir

$latestIndexUrl = Invoke-GitLabUpload -FilePath $latestIndexJsonPath -BaseUrl $GitLabBaseUrl `
    -ProjectPath $GitLabProjectPath -PackageName $indexPackageName -Version $indexVersion -Token $gitLabToken

Remove-Item $sha256FilePath -Force -ErrorAction SilentlyContinue
Remove-Item $latestIndexJsonPath -Force -ErrorAction SilentlyContinue

$deletedVersionCount = Remove-OldGitLabGenericPackageVersions `
    -BaseUrl $GitLabBaseUrl `
    -ProjectPath $GitLabProjectPath `
    -PackageName $PackageType `
    -Token $gitLabToken `
    -KeepLatest $KeepLatestVersions

# Update the platform section of the website download page (optional)
$timestamp = [System.DateTime]::UtcNow.ToString('yyyy-MM-dd HH:mm:ss UTC')

if ($canUpdateWebsite) {
    Update-DownloadPageSection `
        -WebsiteRepoRoot $WebsiteRoot `
        -SectionKey      $PackageType `
        -DisplayName     $config.DisplayName `
        -ArtifactName    $artifact.Name `
        -ArtifactUrl     $artifactUrl `
        -HashName        $sha256FileName `
        -HashUrl         $hashUrl `
        -Notes           $config.Notes `
        -Timestamp       $timestamp
}

Write-Host ""
Write-Host "Deployed package type : $PackageType"
Write-Host "  Version             : $version"
Write-Host "  Source artifact     : $($artifact.FullName)"
Write-Host "  GitLab project      : $GitLabProjectPath"
Write-Host "  GitLab package      : $PackageType / $version"
Write-Host "  Artifact URL        : $artifactUrl"
Write-Host "  SHA-256 URL         : $hashUrl"
Write-Host "  Index URL           : $latestIndexUrl"
Write-Host "  Retention keep      : $KeepLatestVersions latest version(s)"
Write-Host "  Versions removed    : $deletedVersionCount"
if ($canUpdateWebsite) {
    Write-Host "  Download page       : $(Join-Path $WebsiteRoot 'docs\download.md')"
}
Write-Host ""
if ($canUpdateWebsite) {
    Write-Host "Next: review and commit the website repository changes to publish the updated download page."
}
else {
    Write-Host "Next: have the website fetch the Index URL for this package type (GitLab as source of truth)."
}