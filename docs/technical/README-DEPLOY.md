# Deploy To GitHub Releases

This document defines the package publishing flow for NoLimitConnect.

## Goal

Each deploy task performs this chain:

1. Run the required release build and package task through existing task dependencies.
2. Find the newest generated package artifact.
3. Generate a SHA-256 sidecar file for that artifact.
4. Upload both files to a GitHub release.
5. Optionally update docs/download.md in the website/docs repository.

## Source Of Truth

GitHub Releases are the source of truth for downloadable artifacts.

Each platform deploy uploads:

1. Package artifact file.
2. Matching .sha256 sidecar file.

Download URL pattern:

```text
https://github.com/nolimitconnect/NoLimitConnect/releases/download/<tag>/<filename>
```

## Authentication

Deploy scripts use one of the following environment variables:

1. GITHUB_RELEASES_TOKEN
2. GITHUB_TOKEN

Token scope requirement:

1. Repository contents write access (to create/update release assets).

## Deploy Scripts

Deploy automation is implemented in these scripts:

1. .vscode/deploy-package-to-website.ps1
2. .vscode/deploy-package-to-website.py

Supported package types:

1. windows
2. linux
3. android-signed
4. flatpak

Optional behavior:

1. -SkipWebsiteUpdate disables markdown updates.
2. -GitHubReleaseTag targets a specific release tag (defaults to v<version>).

If the target release tag does not exist, the script creates it.

## Download Page Update

When website update is enabled and docs/download.md is available, the script updates the matching section marker block:

```html
<!-- BEGIN SECTION: windows -->      <!-- END SECTION: windows -->
<!-- BEGIN SECTION: linux -->        <!-- END SECTION: linux -->
<!-- BEGIN SECTION: android-signed --> <!-- END SECTION: android-signed -->
<!-- BEGIN SECTION: flatpak -->      <!-- END SECTION: flatpak -->
```

Each section includes:

1. Direct package link.
2. SHA-256 file link.
3. Last updated timestamp.
4. Short package note.

## Task Mapping

Deploy tasks are defined in .vscode/tasks.json.

Task behavior:

1. Deploy Windows Package: build/package Windows and upload release assets.
2. Deploy Linux Package: build/package Linux and upload release assets.
3. Deploy Android Signed Package: build/sign Android and upload release assets.
4. Deploy Flatpak Package: build/package Flatpak and upload release assets.

## Release Sequences

### Windows

1. Run Deploy Windows Package.

### Linux

1. Run Deploy Linux Package on a Linux host.

### Android Signed

1. Set required NLC_ANDROID_* signing environment variables.
2. Run Deploy Android Signed Package.

### Flatpak

1. Ensure flatpak-builder is installed on the Linux host.
2. Run Flatpak: Init GPG Key once on the release host.
3. Run Deploy Flatpak Package.

## Flatpak GPG Signing

Flatpak repository metadata is signed with a dedicated GPG key.

Environment variables used by the Flatpak scripts:

1. NLC_FLATPAK_GPG_KEY_ID (optional if auto-detect finds the key in the Flatpak GPG homedir)
2. NLC_FLATPAK_GPG_HOMEDIR (optional, default is build/flatpak-gnupg)
3. NLC_FLATPAK_PUBLIC_KEY_OUT (optional, default is docs/nlc-flatpak-public.gpg)

Public key hosting target:

1. https://nolimitconnect.org/nlc-flatpak-public.gpg

## Notes

1. KeepLatestVersions is currently accepted for compatibility but does not prune GitHub releases.
2. Website update is optional and can be skipped for release-only publishing.
3. The local flatpack directory name is intentionally preserved to match current package output paths.
