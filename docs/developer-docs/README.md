# Developer Documentation Overview

Welcome to the NoLimitConnect Developer Documentation.

This section provides everything you need to build, run, and contribute across all supported platforms.

## Start Here

Pick the path that matches what you want to do right now:

- Make a quick first PR: [First Contribution (30 Minutes)](first-contribution.md)
- Set up and build locally on your own machine: [Set Up and Build Locally](#set-up-and-build-locally)
- Work on issues and PRs: [Contributing Workflow](contributing.md)

---

## Set Up and Build Locally

If you want full local development on your own machine, use this order:

1. Install your OS build environment.
2. Open the target build guide.
3. Compile with your preferred tool (VS Code or Qt Creator).

### 1) Install Build Environment

- Windows: [Setup Windows Build Environment](Setup_Windows_Build_Environment.md)
- Linux: [Setup Linux Build Environment](Setup_Linux_Build_Environment.md)
- Flatpak build environment: [Setup Flatpak Build Environment](Setup_Flatpak_Build_Enviroment.md)

### 2) Choose Target Build Guide

- Android: [Build for Android](build-target-android.md)
- Raspberry Pi: [Build for Raspberry Pi](build-target-raspberry-pi.md)
- Linux: [Build for Linux](build-target-linux.md)
- Windows: [Build for Windows](build-target-windows.md)

### 3) Compile

- [Compile with Visual Studio Code](Compile_NoLimitConnect_VisualCode.md)
- [Compile with Qt Creator](Compile_NoLimitConnect_QCreator.md)
- [Compile with Flatpak](Compile_NoLimitConnect_Flatpak.md)

---

New contributor and short on time?

- [First Contribution (30 Minutes)](first-contribution.md)

---

### 🛠 Qt Setup and Recommended Version

Qt provides the graphical UI framework used by **NoLimitConnect**.  
All build environments **except Flatpak** require a local Qt installation.

Although most Qt 6.x.x versions work, it is recommended to use:

- **Qt 6.9.3 or newer**, due to the fix for:  
  **QTBUG-137407** — <https://qt-project.atlassian.net/browse/QTBUG-137407>

---

### Java Requirement

For Android builds, the recommended Java version is:

- **Java 17**

---

### Qt Installation

You can download and install the Qt SDK from:

➡️ **<https://www.qt.io/download>**

*Note:*  
The **Community Edition** may be less visible on the site and could require navigating through several pages to locate it.

Preferred Qt version: 6.9.3 (6.7.3 for Raspberry Pi only if Qt Creator has errors when run)

---

## Contributing

➡️ [Contribute](../contribute.md)

---

## Bug Tracker

➡️ **<https://github.com/nolimitconnect/NoLimitConnect/issues>**
