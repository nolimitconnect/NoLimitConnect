


; NOTE: only for tests on windows.. assumes qt sdk and dlls and redist are installed already

Name "No Limit Connect Installer"
!include MUI.nsh

!define MUI_ICON "F:\nolimitapp\nolimitgui\src\Resources\NlcP2P.ico"


Section "create test directories"
CreateDirectory "F:\NlcInstallerTest"
CreateDirectory "F:\nolimitapp\package\windows"
SectionEnd



OutFile "F:\nolimitapp\package\windows\nolimitconnect_install_w64.exe"
InstallDir F:\NlcInstallerTest

Section "add assets"
;CreateDirectory $INSTDIR\assets
	SetOutPath $INSTDIR      ; extract exe content at this path, you can also specify other path 
	File "F:\nolimitapp\nolimitgui\bin-Windows\NoLimitConnect.exe"
	File  /r "F:\nolimitapp\nolimitgui\bin-Windows\assets"    ;used to include asset files in exe
	File "F:\Qt\6.4.3\msvc2019_64\bin\Qt6Multimedia.dll"
	File "F:\Qt\6.4.3\msvc2019_64\bin\Qt6OpenGL.dll"
	File "F:\Qt\6.4.3\msvc2019_64\bin\Qt6Gui.dll"
	File "F:\Qt\6.4.3\msvc2019_64\bin\Qt6Svg.dll"
	File "F:\Qt\6.4.3\msvc2019_64\bin\Qt6Core.dll"
	File  /r "F:\Qt\6.4.3\msvc2019_64\plugins\platforms"
SectionEnd


