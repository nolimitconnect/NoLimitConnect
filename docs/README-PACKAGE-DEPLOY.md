# Package And Deploy

## Android

There are now two Android packaging flows in the workspace:

1. Unsigned packaging for local build output collection.
2. Signed packaging for distributable release APKs.

The related tasks are defined in [.vscode/tasks.json](.vscode/tasks.json) and are also exposed in the Run and Debug dropdown through [.vscode/launch.json](.vscode/launch.json).

## Unsigned Android Package

Use this when you want the build system to generate an APK and copy it into the package folder without signing it.

Available task names:

1. `Package Android Release`
2. `Package Android`
3. `Deploy Android Package`

What happens:

1. The Android release build runs.
2. The Qt APK target is built.
3. The generated APK is copied into `package/android`.

Current output file pattern:

1. `package/android/NoLimitConnect-<version>-arm64-v8a.apk`

Important:

1. This APK is not signed for release distribution.
2. It is useful as an intermediate artifact for the signed packaging step.

## Signed Android Package

Use this when you want a release APK that is aligned, signed, and verified.

Available task names:

1. `Package Android Signed Release`
2. `Package Android Signed`
3. `Deploy Android Signed Package`

Signing is performed by [.vscode/sign-android-package.ps1](.vscode/sign-android-package.ps1).

The script does the following:

1. Finds the latest packaged unsigned APK in `package/android`.
2. Aligns it with `zipalign`.
3. Signs it with `apksigner`.
4. Verifies the signed APK.
5. Writes the final file back to `package/android` with a `-signed.apk` suffix.

Current signed output file pattern:

1. `package/android/NoLimitConnect-<version>-arm64-v8a-signed.apk`

## Required Environment Variables

The signed package task will fail unless these environment variables are set:

1. `NLC_ANDROID_KEYSTORE_PATH`
2. `NLC_ANDROID_KEYSTORE_PASSWORD`
3. `NLC_ANDROID_KEY_ALIAS`
4. `NLC_ANDROID_KEY_PASSWORD`

Meaning:

1. `NLC_ANDROID_KEYSTORE_PATH`: full path to the keystore file.
2. `NLC_ANDROID_KEYSTORE_PASSWORD`: keystore password.
3. `NLC_ANDROID_KEY_ALIAS`: alias name inside the keystore.
4. `NLC_ANDROID_KEY_PASSWORD`: password for the alias key.

Example PowerShell session setup:

```powershell
$env:NLC_ANDROID_KEYSTORE_PATH = 'F:\keys\nolimitconnect-release.jks'
$env:NLC_ANDROID_KEYSTORE_PASSWORD = 'replace-me'
$env:NLC_ANDROID_KEY_ALIAS = 'nolimitconnect'
$env:NLC_ANDROID_KEY_PASSWORD = 'replace-me'
```

These values are intentionally read from environment variables so secrets are not committed into the repository.

## How To Run

### From Tasks

Use the VS Code task runner and choose one of these:

1. `Package Android`
2. `Package Android Signed`
3. `Deploy Android Package`
4. `Deploy Android Signed Package`

### From Run And Debug

The same actions are available from the Run and Debug dropdown:

1. `Task: Package Android`
2. `Task: Package Android Signed`
3. `Task: Deploy Android Package`
4. `Task: Deploy Android Signed Package`

## What Deploy Means Here

Deploy now means publishing the newest packaged artifact to the website repository.

For Android package tasks, deploy does all of the following:

1. Locates the newest package artifact.
2. Generates a `.sha256` file for that artifact.
3. Copies the package and hash into the website repository download tree.
4. Updates the generated download section on the website download page.

It still does not:

1. Upload to Google Play.
2. Install to a device.

If a fuller release workflow is needed later, this can be extended with a real distribution step.

## Recommended Release Sequence

For a normal Android release build:

1. Set the four `NLC_ANDROID_*` environment variables.
2. Run `Package Android Signed`.
3. Run `Deploy Android Signed Package`.
4. Use the resulting `*-signed.apk` from `package/android`.

## Notes

1. The signed task depends on the unsigned package task, so you do not need to run both manually.
2. The signing script automatically locates the newest Android SDK build-tools directory.
3. If both signed and unsigned APKs exist, the unsigned package step still produces the base APK and the signed step creates a separate `-signed.apk` artifact.

## Linux `.deb` Package

Use this on a Linux host when you want to build the Debian package.

Available task names:

1. `Package Linux Release`
2. `Package Linux`
3. `Deploy Linux Package`

Run and Debug entries:

1. `Task: Package Linux`
2. `Task: Deploy Linux Package`

What happens:

1. The Linux release build runs.
2. CPack generates a `.deb` package.
3. The package is written into `package/linux`.

Current output file pattern:

1. `package/linux/NoLimitConnect-<version>-x64.deb`

Recommended Linux `.deb` sequence:

1. Run `Build Linux Release` if you want to validate the release build separately.
2. Run `Package Linux`.
3. Run `Deploy Linux Package`.
4. Use the newest `.deb` artifact from `package/linux`.

## Flatpak Package

Use this on a Linux host when you want to build the Flatpak bundle.

Available task names:

1. `Package Flatpak`
2. `Deploy Flatpak Package`

Run and Debug entries:

1. `Task: Package Flatpak`
2. `Task: Deploy Flatpak Package`

What happens:

1. The Flatpak task creates local build and repo directories under `build`.
2. `flatpak-builder` builds the app using `com.nolimitconnect.NoLimitConnect.yml`.
3. `flatpak build-bundle` creates a Flatpak bundle file.
4. The bundle is written into `package/flatpack`.

Current output file pattern:

1. `package/flatpack/nolimitconnect.flatpak`

Important:

1. The directory name is currently `flatpack` in the task configuration and package path.
2. That spelling is intentional in the current workspace because it matches the configured task output path.

Recommended Flatpak sequence:

1. Make sure `flatpak-builder` is installed on the Linux machine.
2. Run `Package Flatpak`.
3. Run `Deploy Flatpak Package`.
4. Use the generated `.flatpak` bundle from `package/flatpack`.

## Linux And Flatpak Deploy Behavior

For Linux and Flatpak package families, deploy now means publishing the newest package and its SHA-256 sidecar into the website repository and updating the website download page.

It still does not:

1. Publish to an APT repository.
2. Publish to Flathub.
3. Install the package automatically.

## Recommended Linux Host Validation Order

After switching to Linux, a practical validation order is:

1. `Build Linux Release`
2. `Package Linux`
3. `Package Flatpak`

That order isolates build problems before packaging problems and keeps Flatpak-specific issues separate from Debian packaging issues.
