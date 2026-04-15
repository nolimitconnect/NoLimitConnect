# Publish Beta Releases

This document is the checklist for publishing a new beta build.

Important:

1. The current deploy scripts do not publish to a separate beta-only channel.
2. Each deploy task publishes the newest artifact for that platform to GitLab Generic Package Registry.
3. Each deploy task also updates the stable platform JSON index in `download-index/v1`.
4. That means a "beta publish" here is the normal public publish flow for the next version you want users to download.

Related documents:

1. [README-PACKAGE-DEPLOY.md](../docs/README-PACKAGE-DEPLOY.md)
2. [README-DEPLOY.md](../docs/README-DEPLOY.md)

## Common Preparation

Do these once before publishing any platform build.

1. Update the version in `cmake/version.cmake`.
2. Update `RELEASE-NOTES.md` with the beta summary for the new version.
3. Update `CHANGELOG.txt` if you still want the legacy changelog kept in sync.
4. Update `com.nolimitconnect.NoLimitConnect.metainfo.xml` for the new release entry if the beta will be distributed through Linux desktop metadata paths.
5. Make sure the code you are publishing is committed and tagged the way you want before uploading artifacts.
6. Set `GITLAB_TOKEN` to a GitLab Personal Access Token with `api` scope.
7. Decide whether you are publishing all platforms or only the platforms validated for that beta.

Example PowerShell setup:

```powershell
$env:GITLAB_TOKEN = 'replace-me'
```

## Windows Beta Publish

Run this on the Windows release machine.

Recommended order:

1. Run `Configure Windows Release`.
2. Run `Build Windows Release`.
3. Run `Run Windows Release` if you want a quick final smoke test on the packaged binaries and runtime environment.
4. Run `Package Windows`.
5. Check the newest installer in `package/windows`.
6. Run `Deploy Windows Package`.
7. Confirm the upload completed and the Windows index was updated.

What the deploy step publishes:

1. The newest Windows installer from `package/windows`.
2. A `.sha256` sidecar file for that installer.
3. `download-index/v1/windows.json` in GitLab.

## Android Beta Publish

Android website distribution is signed-only.

Run this on the Windows machine that has the Android SDK, build tools, and release keystore available.

Required environment variables:

1. `NLC_ANDROID_KEYSTORE_PATH`
2. `NLC_ANDROID_KEYSTORE_PASSWORD`
3. `NLC_ANDROID_KEY_ALIAS`
4. `NLC_ANDROID_KEY_PASSWORD`

Example PowerShell setup:

```powershell
$env:NLC_ANDROID_KEYSTORE_PATH = 'F:\keys\nolimitconnect-release.jks'
$env:NLC_ANDROID_KEYSTORE_PASSWORD = 'replace-me'
$env:NLC_ANDROID_KEY_ALIAS = 'nolimitconnect'
$env:NLC_ANDROID_KEY_PASSWORD = 'replace-me'
```

Recommended order:

1. Run `Configure Android Release`.
2. Run `Build Android Release`.
3. Run `Package Android Signed`.
4. Check the newest signed APK in `package/android` and make sure it has the `-signed.apk` suffix.
5. Optionally install the APK on a test device and do a final smoke test before upload.
6. Run `Deploy Android Signed Package`.
7. Confirm the upload completed and the Android index was updated.

What the deploy step publishes:

1. The newest signed APK from `package/android`.
2. A `.sha256` sidecar file for that APK.
3. `download-index/v1/android-signed.json` in GitLab.

## Linux `.deb` Beta Publish

Run this on a Linux host.

Recommended order:

1. Run `Configure Linux Release`.
2. Run `Build Linux Release`.
3. Optionally run the release binary directly if you want a quick host smoke test.
4. Run `Package Linux`.
5. Check the newest `.deb` package in `package/linux`.
6. Run `Deploy Linux Package`.
7. Confirm the upload completed and the Linux index was updated.

What the deploy step publishes:

1. The newest `.deb` package from `package/linux`.
2. A `.sha256` sidecar file for that package.
3. `download-index/v1/linux.json` in GitLab.

## Flatpak Beta Publish

Run this on a Linux host with `flatpak-builder` installed.

Recommended order:

1. Make sure `com.nolimitconnect.NoLimitConnect.yml` still builds the intended release configuration.
2. Run `Package Flatpak`.
3. Check the generated bundle in `package/flatpack`.
4. Optionally install the bundle locally and do a final smoke test.
5. Run `Deploy Flatpak Package`.
6. Confirm the upload completed and the Flatpak index was updated.

What the deploy step publishes:

1. The newest `.flatpak` bundle from `package/flatpack`.
2. A `.sha256` sidecar file for that bundle.
3. `download-index/v1/flatpak.json` in GitLab.

## After Publishing

After each platform publish:

1. Verify the artifact filename matches the intended version.
2. Verify the SHA-256 sidecar exists in GitLab for that artifact.
3. Verify the platform JSON index points to the new file.
4. If website update was intentionally skipped, update the website repository separately if any beta announcement text or download-page wording needs to change.
5. Create or update the beta announcement text using the same version and highlights as `RELEASE-NOTES.md`.

## Practical Publish Order

If you are publishing multiple platforms for one beta, this order keeps failures easier to isolate:

1. Windows
2. Android signed
3. Linux `.deb`
4. Flatpak

That order gets the Windows and Android artifacts out from the primary workstation first, then finishes Linux-host packaging afterward.
