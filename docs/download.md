
# Download - Public Beta Builds

Choose the package that matches your platform. Each published download includes a SHA-256 sidecar file so the package can be verified after download.

These are independent beta builds published for early users and testers.

If you are new to NoLimitConnect, start with public bootstrap hosts first, then set up your own private Network Host and Connection Test Host when ready.

## Not Sure What To Download?

Start here if you want the shortest path:

- Linux desktop users: use [Flatpak (recommended)](#flatpak-universal--all-architectures)
- Windows users: use the latest x64 `.exe` in [Latest Packages (Live)](#latest-packages-live)
- Android users: use the latest `.apk` in [Latest Packages (Live)](#latest-packages-live)

If this is your first time:

1. Install the app for your platform.
2. Join a public host first.
3. Leave network settings on defaults.
4. Set up your own private network later when ready.

Need install help after download?

- [Install on Linux](user-docs/install-linux.md)
- [Install on Windows](user-docs/install-windows.md)
- [Install on Android](user-docs/install-android.md)

<!-- BEGIN SECTION: flatpak -->
## Flatpak (Universal – All Architectures)

**Recommended for Linux users.** One-click installation via custom remote.

<div id="flatpak-signature-warning" class="flatpak-signature-warning" hidden>
    <strong>Flatpak remote signature status warning:</strong>
    Signed repository metadata is temporarily unavailable on this mirror.
    Remote install with mandatory summary verification may fail until signatures are republished.
</div>

### Install via Remote (Recommended)

Copy and run this command in your terminal:

```bash
curl -fsSL https://nolimitconnect.org/install-flatpak.sh | bash
```

Public key URL: <https://nolimitconnect.org/nlc-flatpak-public.gpg>

The installer script uses the signed remote when available and falls back to a direct bundle install when remote signatures are temporarily unavailable.

### Uninstall NoLimitConnect Flatpak

To uninstall the Flatpak, copy and run this command in your terminal:

```bash
flatpak uninstall --delete-data org.nolimitconnect.NoLimitConnect
```


### Direct Download (All Architectures)

- **x86_64**: [nolimitconnect-x64.flatpak](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.2/nolimitconnect-x64.flatpak) | [SHA-256](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.2/nolimitconnect-x64.flatpak.sha256)
- **ARM64**: [nolimitconnect-arm64.flatpak](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.2/nolimitconnect-arm64.flatpak) | [SHA-256](https://github.com/nolimitconnect/NoLimitConnect/releases/download/v1.1.2/nolimitconnect-arm64.flatpak.sha256)
- Last updated: 2026-05-11 22:02:54 UTC
- Flatpak remote metadata is GPG-signed. Import the key above before adding the remote.

**Install from downloaded bundle:**

```bash
flatpak install ./nolimitconnect-x64.flatpak
# or for ARM64:
flatpak install ./nolimitconnect-arm64.flatpak
```

- Notes: Flatpak package for Linux desktops with Flatpak support (x86_64 and ARM64).
<!-- END SECTION: flatpak -->

## Latest Packages (Live)

This section is loaded from GitHub Releases metadata at page load.
If package timestamps look stale, do a hard refresh: `Ctrl+F5` (Windows/Linux) or `Cmd+Shift+R` (macOS).

<div id="dynamic-downloads">Loading latest package metadata from GitHub...</div>

<!-- END GENERATED DOWNLOADS -->

## Verify a Download

PowerShell:

```powershell
Get-FileHash -Algorithm SHA256 .\NoLimitConnect-<version>-x64.exe
Get-Content .\NoLimitConnect-<version>-x64.exe.sha256
```

Linux .deb (x64):

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
