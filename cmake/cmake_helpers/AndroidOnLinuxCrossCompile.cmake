
set(TARGET_OS_ANDROID true)

if(${NLC_QT} STREQUAL "6_6_0")
    set(CMAKE_SYSTEM_VERSION 21) # API level
    list(APPEND CMAKE_PREFIX_PATH  /home/nolimit/Qt/6.6.0/gcc_64) 
    set(CMAKE_ANDROID_NDK "/home/nolimit/Android/Sdk/ndk/25.1.8937393")
else()
    set(CMAKE_SYSTEM_VERSION 21) # API level
    list(APPEND CMAKE_PREFIX_PATH  /home/nolimit/Qt/6.4.3/gcc_64) 
    set(CMAKE_ANDROID_NDK "/home/nolimit/Android/Sdk/ndk/23.1.7779620")
endif()


set(CMAKE_SYSTEM_NAME Android)

#set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)

set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang)
set(ARM_SYSROOT --sysroot=${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)