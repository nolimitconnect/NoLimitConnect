# Package And Deploy

## Android

Android website distribution is signed-only.

Related tasks are defined in .vscode/tasks.json and available in Run and Debug through .vscode/launch.json.

## Signed Android Package

Use this when you want a release APK that is aligned, signed, and verified.

Available task names:

1. Package Android Signed Release
2. Package Android Signed
3. Deploy Android Signed Package

Signing is performed by .vscode/sign-android-package.ps1.

The script:

1. Finds the newest packaged unsigned APK in package/android.
2. Aligns it with zipalign.
3. Signs it with apksigner.
4. Verifies the signed APK.
5. Writes the final file back to package/android with a -signed.apk suffix.

Current signed output pattern:

1. package/android/NoLimitConnect-<version>-arm64-v8a-signed.apk

## Required Environment Variables

The signed package task fails unless these variables are set:

1. NLC_ANDROID_KEYSTORE_PATH
2. NLC_ANDROID_KEYSTORE_PASSWORD
3. NLC_ANDROID_KEY_ALIAS
4. NLC_ANDROID_KEY_PASSWORD

## How To Run

### From Tasks

1. Package Android Signed
2. Deploy Android Signed Package

### From Run And Debug

1. Task: Package Android Signed
2. Task: Deploy Android Signed Package

## What Deploy Means

Deploy now means publishing the newest packaged artifact and SHA-256 sidecar to GitHub Releases.

For Android signed package tasks, deploy does all of the following:

1. Locates the newest package artifact.
2. Generates a .sha256 file for that artifact.
3. Uploads package and hash as release assets.
4. Optionally updates docs/download.md when website update is enabled.

It does not:

1. Upload to Google Play.
2. Install to a device.

## Linux .deb Package

Available task names:

1. Package Linux Release
2. Package Linux
3. Deploy Linux Package

What happens:

1. Linux release build runs.
2. CPack generates a .deb package.
3. Package is written into package/linux.
4. Deploy uploads the package and .sha256 to GitHub Releases.

## Flatpak Package

Available task names:

1. Package Flatpak
2. Deploy Flatpak Package

What happens:

1. Flatpak task creates local build and repo directories under build.
2. flatpak-builder builds the app using org.nolimitconnect.NoLimitConnect.yml.
3. flatpak build-bundle creates the bundle.
4. Bundle is written into package/flatpack.
5. Deploy uploads bundle and .sha256 to GitHub Releases.

## Linux And Flatpak Deploy Behavior

For Linux and Flatpak package families, deploy publishes the newest package and SHA-256 sidecar to the selected GitHub release tag.

It does not:

1. Publish to an APT repository.
2. Publish to Flathub.
3. Install packages automatically.

## Recommended Linux Host Validation Order

1. Build Linux Release
2. Package Linux
3. Package Flatpak

This order isolates build issues before packaging issues.
