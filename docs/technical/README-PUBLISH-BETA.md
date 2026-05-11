# Publish Beta Releases

This checklist describes how to publish a new beta build with GitHub Releases.

Important:

1. Each deploy task publishes the newest artifact for that platform to a GitHub release.
2. Deploy scripts default to release tag v<version> and create it if missing.
3. A beta publish here is the normal public publish flow for the next version users should download.

Related documents:

1. README-PACKAGE-DEPLOY.md
2. README-DEPLOY.md

## Common Preparation

1. Update version in cmake/version.cmake.
2. Update RELEASE-NOTES.md with beta summary.
3. Update com.nolimitconnect.NoLimitConnect.metainfo.xml if needed for Linux metadata paths.
4. Ensure code is committed and tagged appropriately.
5. Set GITHUB_RELEASES_TOKEN or GITHUB_TOKEN.
6. Decide whether you are publishing all platforms or only validated ones.

Example PowerShell setup:

```powershell
$env:GITHUB_RELEASES_TOKEN = 'replace-me'
```

## Windows Beta Publish

Recommended order:

1. Configure Windows Release
2. Build Windows Release
3. Optional smoke test with Run Windows Release
4. Package Windows
5. Verify newest installer in package/windows
6. Deploy Windows Package
7. Verify the asset and .sha256 are present in the GitHub release

## Android Beta Publish

Required environment variables:

1. NLC_ANDROID_KEYSTORE_PATH
2. NLC_ANDROID_KEYSTORE_PASSWORD
3. NLC_ANDROID_KEY_ALIAS
4. NLC_ANDROID_KEY_PASSWORD

Recommended order:

1. Configure Android Release
2. Build Android Release
3. Package Android Signed
4. Verify newest signed APK in package/android
5. Optional install and smoke test
6. Deploy Android Signed Package
7. Verify APK and .sha256 in GitHub release

## Linux .deb Beta Publish

Recommended order on Linux host:

1. Configure Linux Release
2. Build Linux Release
3. Optional binary smoke test
4. Package Linux
5. Verify newest .deb in package/linux
6. Deploy Linux Package
7. Verify .deb and .sha256 in GitHub release

## Flatpak Beta Publish

Recommended order on Linux host:

1. Verify com.nolimitconnect.NoLimitConnect.yml targets intended release config
2. Package Flatpak
3. Verify generated bundle in package/flatpack
4. Optional local install smoke test
5. Deploy Flatpak Package
6. Verify .flatpak and .sha256 in GitHub release

## After Publishing

1. Verify artifact filenames match intended version.
2. Verify each artifact has a matching .sha256 sidecar.
3. Verify docs/download.md resolves to the new release assets.
4. Publish announcement text using the same version/highlights as RELEASE-NOTES.md.

## Practical Publish Order

If publishing multiple platforms for one beta:

1. Windows
2. Android Signed
3. Linux .deb
4. Flatpak
