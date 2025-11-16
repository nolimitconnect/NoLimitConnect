#!/bin/bash
# setup_nolimitconnect_docs_full.sh
# Script to create nolimitconnect-docs folder structure and populate Markdown files

BASE_DIR="nolimitconnect-docs"
DEVELOPER_DOCS="$BASE_DIR/developer-docs"
USER_DOCS="$BASE_DIR/user-docs"
HOWTO_DOCS="$BASE_DIR/howto-docs"
ASSETS_DIR="$BASE_DIR/assets"

mkdir -p "$DEVELOPER_DOCS" "$USER_DOCS" "$HOWTO_DOCS" "$ASSETS_DIR"

# -------------------------------
# Developer Docs
# -------------------------------

cat > "$DEVELOPER_DOCS/Setup_Ubuntu_Build_Environment.md" << 'EOF'
# Setup Ubuntu Build Environment for NoLimitConnect

## Prerequisite tools
sudo apt install g++ nasm uuid-dev libglu1-mesa-dev git build-essential libxcb-*

## Optional: Docker for builds
sudo apt install docker
sudo groupadd docker
sudo usermod -aG docker ${USER}
docker run hello-world

## Qt Installation
Download Qt Online Installer: https://www.qt.io/download  
Preferred Qt version: 6.6.3 (6.7.3 for Raspberry Pi only)

## Compile Environment Issues
- Missing X11 headers: sudo apt install libxext-dev libxfixes-dev libx11-dev
- Qt platform plugin errors: sudo apt install libxcb-*
EOF

cat > "$DEVELOPER_DOCS/Setup_Windows_Build_Environment.md" << 'EOF'
# Setup Windows Build Environment for NoLimitConnect

1. Install Visual Studio 2022 Community Edition
   - Select C/C++ development module
2. Install NASM
   - Copy nasm.exe to C:\Program Files\Microsoft Visual Studio\2022\Community\VC
3. Install Qt Visual Studio Extension
EOF

cat > "$DEVELOPER_DOCS/Compile_NoLimitConnect_Docker.md" << 'EOF'
# Compile NoLimitConnect Using Docker

## Notes
- Qt 6.6.2 tag: nlcdevimage-1.0.7
- Substitute tag/version as needed

## Steps
1. Run docker image:
docker run -it brettrjonesdocker/nolimitappimage:nlcdevimage-1.0.7 bash

2. Create build folder:
mkdir -p ~/builds/nolimitconnectapps/nolimitapp/build/nlc_build

3. Checkout source code:
git clone https://gitlab.com/nolimitconnectapps/nolimitapp.git
cd nolimitapp

4. Configure build (Linux):
cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_LINUX=true -DQT_VERSION=6_6_2

5. Configure build (Android aarch64):
cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true -DQT_VERSION=6_6_2 -DARM64=true

6. Configure build (Android arm7a):
cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true -DQT_VERSION=6_6_2 -DARM32=true

7. Build:
make -j$(nproc)
EOF

cat > "$DEVELOPER_DOCS/Compile_NoLimitConnect_Flatpak.md" << 'EOF'
# Compile NoLimitConnect Using Flatpak

1. Checkout NoLimitConnect:
git clone https://gitlab.com/nolimitconnectapps/nolimitapp.git
cd nolimitapp

2. Allow flatpak submodules (required for GL/glu lib):
git submodule add -f https://github.com/flathub/shared-modules.git

3. Build and install:
flatpak-builder --user --install --force-clean build-dir com.nolimitconnect.NoLimitConnect.yml

4. Run:
flatpak run com.nolimitconnect.NoLimitConnect

5. List flatpak apps:
flatpak list --app

6. Show NLC info:
flatpak info --show-location com.nolimitconnect.NoLimitConnect

7. Uninstall:
flatpak uninstall --delete-data com.nolimitconnect.NoLimitConnect
EOF

cat > "$DEVELOPER_DOCS/Compile_NoLimitConnect_QCreator.md" << 'EOF'
# Compile NoLimitConnect Using Qt Creator

## Compile in Qt Creator
git clone https://gitlab.com/nolimitconnectapps/nolimitapp.git
Open nolimitapp/CMakeList.txt in Qt Creator
Configure and Compile

## Compile from command line in Linux
git clone https://gitlab.com/nolimitconnectapps/nolimitapp.git
mkdir -p nolimitapp/build/nlc_build
cd nolimitapp/build/nlc_build
rm -rf *

### Linux build
cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_LINUX=true -DQT_VERSION=6_6_3

### Android aarch64
cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true -DQT_VERSION=6_6_3 -DARM64=true

### Android arm7
cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true -DQT_VERSION=6_6_3 -DARM32=true

### Compile
make -j$(nproc)
EOF

cat > "$DEVELOPER_DOCS/Compile_NoLimitConnect_VisualStudio.md" << 'EOF'
# Windows Compile NoLimitConnect in Visual Studio

## Checkout code
git clone https://gitlab.com/nolimitconnectapps/nolimitapp.git

## Compile in Visual Studio
Open nolimitapp/nolimitgui/build/VS2022/NoLimitConnectAll.sln
Right Click NoLimitConnect → Set As Startup Project
Compile

## Android Issues
- Debugging on Galaxy A8 or Galaxy Flip 5G may break Java assembly on touch
- Recommended Android Kit:
  - NDK KIT 23.1.7779620
  - SDK PLATFORM 33
  - ANDROID PLATFORM android-23
  - Optional: set C compiler to `ndk\23.1.7779620\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android23-clang`
EOF

# -------------------------------
# User Docs
# -------------------------------
cat > "$USER_DOCS/README.md" << 'EOF'
# User Documentation

Welcome to NoLimitConnect! This section covers installation, usage, and troubleshooting for end-users.
EOF

# -------------------------------
# How-to Docs
# -------------------------------
cat > "$HOWTO_DOCS/README.md" << 'EOF'
# How-To Guides

This section contains step-by-step tutorials for common tasks and workflows.
EOF

# -------------------------------
# Assets
# -------------------------------
curl -L -o "$ASSETS_DIR/nlc-icon.svg" "https://gitlab.com/nolimitconnectapps/nolimitapp/-/raw/main/nolimitgui/src/Resources/nlc-icon.svg"

echo "All folders, Markdown files, and assets have been created!"
