# Deploy To nolimitconnect.com

This document defines the release publishing flow for package artifacts.

## Goal

Each deploy task now performs the publishing chain for its package family:

1. Run the required release build and package task through existing task dependencies.
2. Find the newest generated package artifact.
3. Generate a SHA-256 sidecar file for that artifact.
4. Upload both the package and hash to the GitLab Generic Package Registry.
5. Publish a stable per-platform JSON index in GitLab for website consumption.
6. Optionally update the website download markdown if the website repository is available.

## Source Of Truth

GitLab Generic Package Registry is the source of truth for download metadata.

For each deploy, the script uploads:

1. Package artifact file.
2. Package SHA-256 sidecar file.
3. Stable JSON metadata file for the package type.

Stable JSON index URL pattern:

```text
https://gitlab.com/api/v4/projects/nolimitcode%2Fnolimitconnect/packages/generic/download-index/v1/<package-type>.json
```

The website can fetch this JSON and render download links dynamically, avoiding manual edits for each release.

## Website Repository

The website is hosted on GitHub and deployed via Netlify (static HTML generated from MkDocs).

Package binaries must not be committed to the website repository. Binaries are hosted on GitLab.

Default website repository path (optional):

1. `F:\nlc-nolimitconnect.com`

You can override that path with the `NLC_WEBSITE_ROOT` environment variable if the website repository moves.

If the website repository is unavailable, deploy still succeeds and GitLab publication still completes.

## GitLab Package Registry

Packages are uploaded to the GitLab Generic Package Registry for the main project:

1. Project: `https://gitlab.com/nolimitconnect/NoLimitConnect`
2. Registry path: `packages/generic/<package-type>/<version>/<filename>`

The deploy script uses the `GITLAB_TOKEN` environment variable (a Personal Access Token with `api` scope).

Public artifact URL pattern:

```text
https://gitlab.com/api/v4/projects/nolimitcode%2Fnolimitconnect/packages/generic/<type>/<version>/<filename>
```

If a version is re-deployed (same version number, new build), the existing file in the registry is overwritten by GitLab.

Retention cleanup is supported by `-KeepLatestVersions <n>`. When `n > 0`, older GitLab Generic Package versions for that package type are deleted, keeping only the newest `n` versions.

## Deploy Script

Deploy automation is implemented in [.vscode/deploy-package-to-website.ps1](../.vscode/deploy-package-to-website.ps1).

Supported package types:

1. `windows`
2. `linux`
3. `android-signed`
4. `flatpak`

Optional behavior:

1. `-SkipWebsiteUpdate` disables any local website markdown update.

## Download Page Update

When website update is enabled and the website repository is present, the script updates `docs/download.md` in the website repository.

Platform section markers:

```html
<!-- BEGIN SECTION: windows -->      <!-- END SECTION: windows -->
<!-- BEGIN SECTION: linux -->        <!-- END SECTION: linux -->
<!-- BEGIN SECTION: android-signed --> <!-- END SECTION: android-signed -->
<!-- BEGIN SECTION: flatpak -->      <!-- END SECTION: flatpak -->
```

Each generated section includes:

1. Direct package link.
2. SHA-256 file link.
3. Last updated timestamp.
4. Short package note.

## Task Mapping

The existing deploy tasks in [.vscode/tasks.json](../.vscode/tasks.json) call the deploy script.

Task behavior:

1. `Deploy Windows Package` — build/package Windows, upload to GitLab, publish `download-index/v1/windows.json`.
2. `Deploy Linux Package` — build/package Linux, upload to GitLab, publish `download-index/v1/linux.json`.
3. `Deploy Android Signed Package` — build/sign Android, upload to GitLab, publish `download-index/v1/android-signed.json`.
4. `Deploy Flatpak Package` — build/package Flatpak, upload to GitLab, publish `download-index/v1/flatpak.json`.

By default, project tasks pass `-KeepLatestVersions 2` to keep storage usage bounded.

Project tasks also pass `-SkipWebsiteUpdate` by default so publication is independent of website repository availability.

## Release Sequences

### Windows

1. Run `Deploy Windows Package`.

### Linux

1. Run `Deploy Linux Package` on a Linux host.

### Android signed

1. Set the required `NLC_ANDROID_*` signing environment variables.
2. Run `Deploy Android Signed Package`.

### Prerequisites for all deploy tasks

1. Set `GITLAB_TOKEN` to a GitLab Personal Access Token with `api` scope.

### Flatpak

1. Ensure `flatpak-builder` is installed on the Linux host.
2. Run `Deploy Flatpak Package`.

## SHA-256 Format

Each generated sidecar file uses the standard two-space format:

```text
<sha256>  <filename>
```

## Notes

1. `GITLAB_TOKEN` must be set before running any deploy task.
2. Website repo commits are still manual when markdown update is enabled.
3. Android signed publishing uses the existing signing script before deploy publication.
4. The local `flatpack` folder name is preserved because it matches current build configuration.
