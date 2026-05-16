param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('windows', 'linux', 'android-signed', 'flatpak')]
    [string]$PackageType,

    [string]$WorkspaceRoot = '',
    [string]$WebsiteRoot = $(if ($env:NLC_WEBSITE_ROOT) { $env:NLC_WEBSITE_ROOT } else { '' }),
    [string]$GitHubRepository = 'nolimitconnect/NoLimitConnect',
    [string]$GitHubApiBaseUrl = 'https://api.github.com',
    [string]$GitHubReleaseTag = '',
    [switch]$SkipWebsiteUpdate,
    [ValidateRange(0, 100)]
    [int]$KeepLatestVersions = 0
)

$ErrorActionPreference = 'Stop'

$FlatpakRemoteName = 'nlc'
$FlatpakRemoteUrl = 'https://nolimitconnect.org/nlc-repo'
$FlatpakPublicKeyUrl = 'https://nolimitconnect.org/nlc-flatpak-public.gpg'

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

if ([string]::IsNullOrWhiteSpace($WebsiteRoot)) {
    $WebsiteRoot = $WorkspaceRoot
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
            SectionKey  = 'windows'
        }
        'linux' = @{
            SourceDir   = 'package\linux'
            Include     = @('*.deb')
            Exclude     = @()
            DisplayName = 'Linux'
            Notes       = 'Debian package for Linux.'
            SectionKey  = 'linux'
        }
        'android-signed' = @{
            SourceDir   = 'package\android'
            Include     = @('*-signed.apk')
            Exclude     = @()
            DisplayName = 'Android'
            Notes       = 'Signed APK intended for release distribution.'
            SectionKey  = 'android-signed'
        }
        'flatpak' = @{
            SourceDir   = 'package\flatpack'
            Include     = @('*.flatpak')
            Exclude     = @()
            DisplayName = 'Flatpak'
            Notes       = 'GPG-signed Flatpak repository and bundle for Linux desktops.'
            SectionKey  = 'flatpak'
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

function Invoke-GitHubApi {
    param(
        [Parameter(Mandatory = $true)][string]$Method,
        [Parameter(Mandatory = $true)][string]$Url,
        [Parameter(Mandatory = $true)][string]$Token,
        [object]$Body = $null,
        [string]$ContentType = 'application/json',
        [string]$InFile = ''
    )

    $headers = @{
        'Authorization' = "Bearer $Token"
        'Accept'        = 'application/vnd.github+json'
        'X-GitHub-Api-Version' = '2022-11-28'
        'User-Agent' = 'NoLimitConnect-Deploy-Script'
    }

    try {
        if (-not [string]::IsNullOrWhiteSpace($InFile)) {
            return Invoke-RestMethod -Method $Method -Uri $Url -Headers $headers -InFile $InFile -ContentType $ContentType
        }

        if ($null -eq $Body) {
            return Invoke-RestMethod -Method $Method -Uri $Url -Headers $headers
        }

        $json = $Body | ConvertTo-Json -Depth 8
        return Invoke-RestMethod -Method $Method -Uri $Url -Headers $headers -Body $json -ContentType $ContentType
    }
    catch {
        $statusCode = ''
        $detail = ''
        if ($_.Exception.Response -and $_.Exception.Response.StatusCode) {
            $statusCode = [int]$_.Exception.Response.StatusCode
        }

        if ($_.ErrorDetails -and $_.ErrorDetails.Message) {
            $detail = $_.ErrorDetails.Message
        }

        $message = "GitHub API request failed: $Method $Url"
        if (-not [string]::IsNullOrWhiteSpace($statusCode)) {
            $message += " (HTTP $statusCode)"
        }
        if (-not [string]::IsNullOrWhiteSpace($detail)) {
            $message += "`n$detail"
        }

        throw $message
    }
}

function Get-OrCreateGitHubRelease {
    param(
        [string]$ApiBaseUrl,
        [string]$Repository,
        [string]$Tag,
        [string]$Token
    )

    $encodedTag = [Uri]::EscapeDataString($Tag)
    $getByTagUrl = "$ApiBaseUrl/repos/$Repository/releases/tags/$encodedTag"

    try {
        return Invoke-GitHubApi -Method Get -Url $getByTagUrl -Token $Token
    }
    catch {
        if ($_ -notmatch 'HTTP 404') {
            throw
        }
    }

    Write-Host "  Release '$Tag' was not found. Creating it now."

    $createUrl = "$ApiBaseUrl/repos/$Repository/releases"
    $body = @{
        tag_name = $Tag
        name = $Tag
        draft = $false
        prerelease = $false
        generate_release_notes = $true
    }

    return Invoke-GitHubApi -Method Post -Url $createUrl -Token $Token -Body $body
}

function Remove-ExistingReleaseAsset {
    param(
        [string]$ApiBaseUrl,
        [string]$Repository,
        [object]$Release,
        [string]$AssetName,
        [string]$Token
    )

    $asset = @($Release.assets | Where-Object { $_.name -eq $AssetName } | Select-Object -First 1)
    if (-not $asset) {
        return
    }

    Write-Host "  Removing existing asset: $AssetName"
    $deleteUrl = "$ApiBaseUrl/repos/$Repository/releases/assets/$($asset.id)"
    Invoke-GitHubApi -Method Delete -Url $deleteUrl -Token $Token | Out-Null
}

function Upload-GitHubReleaseAsset {
    param(
        [string]$ApiBaseUrl,
        [string]$Repository,
        [object]$Release,
        [string]$FilePath,
        [string]$Token
    )

    $fileName = Split-Path -Leaf $FilePath
    Remove-ExistingReleaseAsset -ApiBaseUrl $ApiBaseUrl -Repository $Repository -Release $Release -AssetName $fileName -Token $Token

    $uploadBase = ($Release.upload_url -replace '\{\?name,label\}', '')
    $uploadUrl = "${uploadBase}?name=$([Uri]::EscapeDataString($fileName))"

    Write-Host "  Uploading: $fileName"
    $asset = Invoke-GitHubApi -Method Post -Url $uploadUrl -Token $Token -InFile $FilePath -ContentType 'application/octet-stream'

    return $asset.browser_download_url
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
    if ($SectionKey -eq 'flatpak') {
        $newSection = $beginMarker + $nl +
            '## Flatpak (Universal - All Architectures)' + $nl +
            '' + $nl +
            '**Recommended for Linux users.** One-click installation via custom remote.' + $nl +
            '' + $nl +
            '<div id="flatpak-signature-warning" class="flatpak-signature-warning" hidden>' + $nl +
            '  <strong>Flatpak remote signature status warning:</strong>' + $nl +
            '  Signed repository metadata is temporarily unavailable on this mirror.' + $nl +
            '  Remote install with mandatory summary verification may fail until signatures are republished.' + $nl +
            '</div>' + $nl +
            '' + $nl +
            '### Install via Remote (Recommended)' + $nl +
            '' + $nl +
            '```bash' + $nl +
            "curl -fsSL $FlatpakPublicKeyUrl -o /tmp/nlc-flatpak-public.gpg && \" + $nl +
            "flatpak remote-add --if-not-exists --gpg-import=/tmp/nlc-flatpak-public.gpg --gpg-verify-summary=false $FlatpakRemoteName $FlatpakRemoteUrl && \" + $nl +
            "flatpak remote-modify --gpg-verify-summary=false $FlatpakRemoteName && \" + $nl +
            'flatpak install -y nlc org.nolimitconnect.NoLimitConnect' + $nl +
            '```' + $nl +
            '' + $nl +
            "Public key URL: <$FlatpakPublicKeyUrl>" + $nl +
            '' + $nl +
            '### Direct Download' + $nl +
            "- Latest package: [$ArtifactName]($ArtifactUrl)" + $nl +
            "- SHA-256: [$HashName]($HashUrl)" + $nl +
            "- Last updated: $Timestamp" + $nl +
            "- Notes: $Notes" + $nl +
            $endMarker
    }
    else {
        $newSection = $beginMarker + $nl +
            "## $DisplayName" + $nl +
            "- Latest package: [$ArtifactName]($ArtifactUrl)" + $nl +
            "- SHA-256: [$HashName]($HashUrl)" + $nl +
            "- Last updated: $Timestamp" + $nl +
            "- Notes: $Notes" + $nl +
            $endMarker
    }

    $pattern = '(?s)' + [regex]::Escape($beginMarker) + '.*?' + [regex]::Escape($endMarker)
    $updated = [regex]::Replace($content, $pattern, [System.Text.RegularExpressions.MatchEvaluator]{ param($m) $newSection }, 1)
    [System.IO.File]::WriteAllText($downloadPagePath, $updated, [System.Text.UTF8Encoding]::new($false))
}

$githubToken = $env:GITHUB_RELEASES_TOKEN
if ([string]::IsNullOrWhiteSpace($githubToken)) {
    $githubToken = $env:GITHUB_TOKEN
}
if ([string]::IsNullOrWhiteSpace($githubToken)) {
    throw "GITHUB_RELEASES_TOKEN (or GITHUB_TOKEN) is not set. Set one before running a deploy task."
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
    }
}

$version = Get-ProjectVersion -Root $WorkspaceRoot
$artifactDir = Join-Path $WorkspaceRoot $config.SourceDir
$artifact = Get-LatestArtifact -Directory $artifactDir -Include $config.Include -Exclude $config.Exclude

$releaseTag = $GitHubReleaseTag
if ([string]::IsNullOrWhiteSpace($releaseTag)) {
    $releaseTag = "v$version"
}

$release = Get-OrCreateGitHubRelease -ApiBaseUrl $GitHubApiBaseUrl -Repository $GitHubRepository -Tag $releaseTag -Token $githubToken

$tempDir = Join-Path ([System.IO.Path]::GetTempPath()) 'nlc-deploy'
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
$sha256FilePath = New-Sha256Sidecar -ArtifactPath $artifact.FullName -OutputDirectory $tempDir
$sha256FileName = Split-Path -Leaf $sha256FilePath

$artifactUrl = Upload-GitHubReleaseAsset -ApiBaseUrl $GitHubApiBaseUrl -Repository $GitHubRepository -Release $release -FilePath $artifact.FullName -Token $githubToken
$release = Invoke-GitHubApi -Method Get -Url "$GitHubApiBaseUrl/repos/$GitHubRepository/releases/$($release.id)" -Token $githubToken
$hashUrl = Upload-GitHubReleaseAsset -ApiBaseUrl $GitHubApiBaseUrl -Repository $GitHubRepository -Release $release -FilePath $sha256FilePath -Token $githubToken

Remove-Item $sha256FilePath -Force -ErrorAction SilentlyContinue

if ($KeepLatestVersions -gt 0) {
    Write-Warning "KeepLatestVersions currently does not prune GitHub releases. No release cleanup was performed."
}

$timestamp = [System.DateTime]::UtcNow.ToString('yyyy-MM-dd HH:mm:ss UTC')
if ($canUpdateWebsite) {
    Update-DownloadPageSection `
        -WebsiteRepoRoot $WebsiteRoot `
        -SectionKey      $config.SectionKey `
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
Write-Host "  Release tag         : $releaseTag"
Write-Host "  Source artifact     : $($artifact.FullName)"
Write-Host "  GitHub repository   : $GitHubRepository"
Write-Host "  Artifact URL        : $artifactUrl"
Write-Host "  SHA-256 URL         : $hashUrl"
if ($canUpdateWebsite) {
    Write-Host "  Download page       : $(Join-Path $WebsiteRoot 'docs\download.md')"
}
Write-Host ""
if ($canUpdateWebsite) {
    Write-Host "Next: review and commit docs/download.md updates."
}
else {
    Write-Host "Next: verify assets on the GitHub release page."
}
