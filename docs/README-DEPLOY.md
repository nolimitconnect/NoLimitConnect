# Deploy To nolimitconnect.com

This document defines the release-to-website flow for package artifacts.

## Goal

Each deploy task now performs the full publishing chain for its package family:

1. Run the required release build and package task through existing task dependencies.
2. Find the newest generated package artifact.
3. Generate a SHA-256 sidecar file for that artifact.
4. Upload both the package and hash to the GitLab Generic Package Registry.
5. Update the platform section of the website download page so it links to the published files.

## Website Repository

The website is hosted on GitHub and deployed via Netlify (static HTML generated from MkDocs).

Package binaries must **not** be committed to the website repository — they are too large for
GitHub and are instead hosted on GitLab. The website repository only stores the updated
`docs/download.md` markdown file that links out to GitLab.

Default website repository path:

1. `F:\nlc-nolimitconnect.com`

You can override that path with the `NLC_WEBSITE_ROOT` environment variable if the website repository moves.

## GitLab Package Registry

Packages are uploaded to the GitLab Generic Package Registry for the main project:

- Project: `https://gitlab.com/nolimitcode/nolimitconnect`
- Registry path: `packages/generic/<package-type>/<version>/<filename>`

The deploy script uses the `GITLAB_TOKEN` environment variable (a Personal Access Token with
`api` scope). Set this variable in your shell or user environment before running any deploy task.

Download URLs written to `download.md` are public API URLs:

```
https://gitlab.com/api/v4/projects/nolimitcode%2Fnolimitconnect/packages/generic/<type>/<version>/<filename>
```

If a version is re-deployed (same version number, new build), the existing file in the registry
is overwritten by GitLab.

Retention cleanup is supported by the deploy script via `-KeepLatestVersions <n>`.
When this is set above `0`, the script deletes older GitLab Generic Package versions for that
package type after a successful upload, keeping only the newest `n` versions.

## Deploy Script

Deploy automation is implemented in [.vscode/deploy-package-to-website.ps1](../.vscode/deploy-package-to-website.ps1).

Supported package types:

1. `windows`
2. `linux`
3. `android`
4. `android-signed`
5. `flatpak`

## Website Output Layout

## Download Page Update

The website download page is at `docs/download.md` inside the website repository.

The page contains per-platform section markers. The deploy script replaces only the section
for the platform being deployed, leaving all other platform sections untouched.

Platform section markers:

```html
<!-- BEGIN SECTION: windows -->      <!-- END SECTION: windows -->
<!-- BEGIN SECTION: linux -->        <!-- END SECTION: linux -->
<!-- BEGIN SECTION: android -->      <!-- END SECTION: android -->
<!-- BEGIN SECTION: android-signed --> <!-- END SECTION: android-signed -->
<!-- BEGIN SECTION: flatpak -->      <!-- END SECTION: flatpak -->
```

Each generated section shows:

1. Direct package link
2. SHA-256 file link
3. Last updated timestamp
4. Short package note

Links point directly to the GitLab Generic Package Registry API URL, which is publicly
accessible for the public project without authentication.

## Task Mapping

The existing deploy tasks in [.vscode/tasks.json](../.vscode/tasks.json) call the deploy script.

Task behavior:

1. `Deploy Windows Package` — Windows build → package → upload to GitLab → update download.md.
2. `Deploy Linux Package` — Linux build → package → upload to GitLab → update download.md.
3. `Deploy Android Package` — Android build → package → upload unsigned APK → update download.md.
4. `Deploy Android Signed Package` — Android build → sign → upload signed APK → update download.md.
5. `Deploy Flatpak Package` — Flatpak build → upload bundle → update download.md.

By default, project tasks pass `-KeepLatestVersions 2` so only the latest two versions are kept
per package type in GitLab to control storage usage.

After any deploy task completes, review the changed `download.md` and commit+push the website
repository to trigger a Netlify rebuild.

## Release Sequences

### Windows

1. Run `Deploy Windows Package`.

### Linux

1. Run `Deploy Linux Package` on a Linux host.

### Android unsigned

1. Run `Deploy Android Package`.

### Android signed

1. Set the required `NLC_ANDROID_*` signing environment variables.
2. Run `Deploy Android Signed Package`.

### Prerequisites for all deploy tasks

1. Set the `GITLAB_TOKEN` environment variable to a GitLab Personal Access Token with `api` scope.

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
2. The website repo is not committed automatically. After a deploy task runs, review and commit the website repository changes.
3. Android signed publishing uses the existing signing script before website deployment.
4. The local `flatpack` (typo) folder name is preserved because that is how the build is configured.
5. Package binaries are excluded from the website git repository via `.gitignore`. Run
	`git rm -r --cached docs/downloads/` inside the website repo to untrack any previously
	committed binaries.
