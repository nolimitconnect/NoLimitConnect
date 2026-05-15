# Download - Public Beta Builds

Choose the package that matches your platform. Each published download includes a SHA-256 sidecar file so the package can be verified after download.

These are pre-Flathub and pre-store builds published for early users and testers.

If you are new to NoLimitConnect, start with public bootstrap hosts first, then set up your own private Network Host and Connection Test Host when ready.

## Latest Packages (Live)

This section is loaded from GitHub Releases metadata at page load.

<div id="dynamic-downloads">Loading latest package metadata from GitHub...</div>

## Fallback Snapshot

If the live metadata endpoint is unavailable, use the snapshot below.

<!-- BEGIN GENERATED DOWNLOADS -->

<!-- BEGIN SECTION: windows -->
## Windows
- Latest package: See the latest GitHub release assets above.
- SHA-256: Download the matching `.sha256` sidecar from the same GitHub release.
- Last updated: 2026-03-22 09:39:05 UTC
- Notes: NSIS installer for Windows x64.
<!-- END SECTION: windows -->

<!-- BEGIN SECTION: linux -->
## Linux
- No deployed package yet.
- Notes: Debian package for Linux x64.
<!-- END SECTION: linux -->

<!-- BEGIN SECTION: android-signed -->
## Android
- No deployed package yet.
- Notes: Signed APK intended for release distribution.
<!-- END SECTION: android-signed -->

<!-- BEGIN SECTION: flatpak -->
## Flatpak (Universal – All Architectures)

**Recommended for Linux users.** One-click installation via custom remote.

### Install via Remote (Recommended)

Copy and run this command in your terminal:

```bash
flatpak remote-add --if-not-exists --no-gpg-verify nlc https://nolimitconnect.org/nlc-repo && \
flatpak install -y nlc org.nolimitconnect.NoLimitConnect
```

Or use the one-click remote setup script:

```bash
curl -fsSL https://nolimitconnect.org/install-flatpak.sh | bash
```

### Direct Download (All Architectures)

- **x86_64**: [nolimitconnect-x64.flatpak](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.1/nolimitconnect-x64.flatpak) | [SHA-256](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.1/nolimitconnect-x64.flatpak.sha256)
- **ARM64**: [nolimitconnect-arm64.flatpak](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.1/nolimitconnect-arm64.flatpak) | [SHA-256](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.1/nolimitconnect-arm64.flatpak.sha256)
- Last updated: 2026-05-11 22:02:54 UTC

**Install from downloaded bundle:**

```bash
flatpak install ./nolimitconnect-x64.flatpak
# or for ARM64:
flatpak install ./nolimitconnect-arm64.flatpak
```

- Notes: Flatpak package for Linux desktops with Flatpak support (x86_64 and ARM64).
<!-- END SECTION: flatpak -->

<!-- END GENERATED DOWNLOADS -->

## Verify A Download

PowerShell:

```powershell
Get-FileHash -Algorithm SHA256 .\NoLimitConnect-<version>-x64.exe
Get-Content .\NoLimitConnect-<version>-x64.exe.sha256
```

Linux:

```bash
sha256sum NoLimitConnect-<version>-x64.deb
cat NoLimitConnect-<version>-x64.deb.sha256
```

## Languages Supported By NoLimitConnect

1. English
2. Deutsch (German)
3. Simplified Chinese
4. Espanol (Spanish)
5. Francais (French)
6. Arabic
7. Hindi
8. Portugues (Portuguese)
9. Japanese
10. Korean
11. Russian
12. Bahasa Indonesia
13. Thai
