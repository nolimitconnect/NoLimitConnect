
set(TARGET_OS_ANDROID true)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)

if(${NLC_QT} STREQUAL "6_6_0")
    set(CMAKE_SYSTEM_VERSION 34) # API level
    list(APPEND CMAKE_PREFIX_PATH  F:/Qt/6.6.0/gcc_64) 
    set(CMAKE_ANDROID_NDK F:/Android/android-sdk/ndk/25.1.8937393)
else()
    set(CMAKE_SYSTEM_VERSION 21) # API level
    list(APPEND CMAKE_PREFIX_PATH  F:/Qt/6.4.3/gcc_64) 
    set(CMAKE_ANDROID_NDK F:/Android/android-sdk/ndk/23.1.7779620)
endif()

set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang)
set(ARM_SYSROOT --sysroot=${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
elseif(TARGET_OS_WINDOWS)

message(STATUS "qt prefix path ${CMAKE_PREFIX_PATH}") 

set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")