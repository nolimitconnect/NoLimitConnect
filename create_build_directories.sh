#! /bin/bash


mkdir build
mkdir build/nlc_build
rm -rf build/nlc_build/*
echo " "
echo "Compile Instructions:"
echo "cd ./build/nlc_build"
echo "If building CI/CD from docker then add -DMAKE_ENV=CI to the following commands"
echo "To configure build for Linux:"
echo "cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_LINUX=true, -DQT_VERSION=6_9_3"
echo " "
echo "To configure build for 64bit arm cpu aarch64 (arm64-v8a):"
echo "cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true, -DQT_VERSION=6_9_3 -DARM64=true"
echo " "
echo "To configure build for 32bit arm cpu arm7 (armeabi-v7a):"
echo "cmake -S ../.. -B . -DCMAKE_BUILD_TYPE=Release -DTARGET_OS_ANDROID=true, -DQT_VERSION=6_9_3 -DARM32=true"
echo " "
echo "To Compile Build:"
echo "make -j$(nproc)"


