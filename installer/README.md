# Creates a install package for the target os

# Ubuntu And Raspbery Pi installers

Assumes flatpack has been setup

https://nolimitconnect.org/developer-docs/Setup_Flatpak_Build_Enviroment

https://nolimitconnect.org/developer-docs/Compile_NoLimitConnect_Flatpak

Assumes flatpack bundle has been created with the following commands

```bash
flatpak-builder --force-clean --repo=repo build-dir com.nolimitconnect.NoLimitConnect.yml
flatpak build-bundle repo NoLimitConnect.flatpak com.nolimitconnect.NoLimitConnect
```

## Ubuntu And Raspbery Pi installer from GUI

- From QCreator or Visual Code
- Open nolimitapp/installer folder
- Configure for Release Build
- build

## Ubuntu And Raspbery Pi installer from command line

```bash
cmake -S installer -B build-installer -INSTALLER_FLATPAK=ON
cmake --build build-installer
```


## Windows installer

Assumes a release build has already been compiled for windows

Assumes NSIS installer has been install on windows
- https://nsis.sourceforge.io/Main_Page

Assumes makensis is in %PATH% 
- Typically this path is C:\Program Files (x86)\NSIS\Bin

Assumes windeployqt is in %PATH% 
- Typically this path is C:\Qt\6.9.3\msvc2022_64\bin

## Windows installer from GUI

Assumes nolimitapp/installer folder is open in visual code 

Assumes CMake Tools extension is installed in visual code


## Windows installer from command line

Assumes CPack and CMake are available on command line (Visusl Code and Visual Studio have cmake but does not add it to the %PATH%)

From Power Shell or Developer command line check with
- where cpack
- where cmake

If either are empty install the full CMake for Windows (x64) from: https://cmake.org/download/

During install:
- Check the box Add CMake to the PATH enviroment variable

From nolimitapp/installer run

- cmake -S installer -B build-installer -DINSTALLER_WINDOWS=ON
- cmake --build build-installer

# Android Installer 

## TODO android installer does not currently work and must be done manually

**These instructions do not work but left here for future work**

Assumes Android has already been built in release mode

Assumes androiddeployqt is in %PATH% 
- If building android on windows:
- Typically this path is C:\Qt\6.9.3\msvc2022_64\bin
- If building android on linux:
- Typically this path is ${HOME}\Qt\6.9.3\gcc_64\bin

From nolimitapp/installer run
- cmake -S installer -B build-installer -DINSTALLER_ANDROID=ON
- cmake --build build-installer