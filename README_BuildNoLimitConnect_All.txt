NOTE: 	The qmake/.pro compile system will probably be removed in future versions in favor of cmake system
		To compile using cmake in qcreator just open nolimitapp/CMakeLists.txt in qcreator and configure and build. 
		
compiling BuildNoLimitConnect_All.pro in QCreator takes a huge amount of resources. You propably will need at least 32GB of memory
A better alternative is to build in 3 steps.

Build in the following order
$HOME/nolimitapp/nolimitlib/BuildNlc1_NlcDependLibs.pro
$HOME/nolimitapp/nolimitlib/BuildNlc2_NlcCore.pro
$HOME/nolimitapp/nolimitgui/NoLimitConnectApp.pro
