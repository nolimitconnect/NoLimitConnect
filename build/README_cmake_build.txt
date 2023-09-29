
# nolimitap/build/build_nlc is the preferred directory for cmake command line builds

# see nolimitap/cmake/docs_cmake_setup/CMakeSetup.txt for notes about setting up cmake command line builds

# nolimitap/cmake/cmake_helpers/OsDetect.cmake contains most of the hard coded paths to qt/assembly/compile tools

# see nolimitap/README_BuildNoLimitConnect_All.txt for notes about Qt qmake build from .pro files

# for Visual Studio 2019 builds open nolimitgui/build/VS2019/NoLimitConnectAll.sln


# cmake only requires one special parameter that defines TARGET_OS_WINDOWS or TARGET_OS_LINUX or TARGET_OS_ANDROID

# example command line from this directory

=== build No Limit Connect ===

mkdir build_nlc
cd build_nlc
rm -rf *
cmake  -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_LINUX=true
make (Optional parameter -jXX where XX is the number of CPUs/threads to build with)

=== create installer package ===

rm -rf ../../package/linux
cpack -C CPackConfig.cmake
 
# the installer .deb package noimitconnect-X.X.X-Linux.deb for this example will be written to nolimitapp/package/linux
 
