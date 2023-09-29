cmake_minimum_required (VERSION 3.16.3)

# Target OS can be TARGET_OS_WINDOWS or TARGET_OS_LINUX or TARGET_OS_ANDROID
# example cmake command line
# cd ~/nolimitapp/build/cmake
# cmake  -S ../.. -B . -DCMAKE_BUILD_TYPE=Debug -DTARGET_OS_LINUX=true

message(STATUS "included OsDetect")

# print command line parameters
get_cmake_property(CacheVars CACHE_VARIABLES)
foreach(CacheVar ${CacheVars})
  get_property(CacheVarHelpString CACHE ${CacheVar} PROPERTY HELPSTRING)
  if(CacheVarHelpString STREQUAL "No help, variable specified on the command line.")
    get_property(CacheVarType CACHE ${CacheVar} PROPERTY TYPE)
    if(CacheVarType STREQUAL "UNINITIALIZED")
      set(CacheVarType)
    else()
      set(CacheVarType :${CacheVarType})
    endif()
    set(CMakeArgs "${CMakeArgs} -D${CacheVar}${CacheVarType}=\"${${CacheVar}}\"")
  endif()
endforeach()
message("CMakeArgs: ${CMakeArgs}")

message("CMAKE_HOST_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME}")

#=== set source code root directory ===#
set(PROJECT_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

if(DEFINED MAKE_ENV)
    # assume called from gitlab ci pipeline.. linux only compile and cross compile
    set(CI_PIPELINE true)
    add_compile_definitions(CI_PIPELINE)
	message(STATUS "CI Pipeline Build")
    if(DEFINED TARGET_OS_ANDROID)
        set(TARGET_OS_ANDROID true)

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "CI Build Target OS Android built on host OS Windows")
        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message(STATUS "CI Build Target OS Android built on host OS Linux")
        else()
            message(FATAL_ERROR "CI Build host is Unknown OS attempting to compile Android")
        endif()
    elseif(DEFINED TARGET_OS_WINDOWS)
        set(TARGET_OS_WINDOWS true)

        message(STATUS "CI Build Target OS Windows built on host OS Windows")
    elseif(DEFINED TARGET_OS_LINUX)
        set(TARGET_OS_LINUX true)

        message(STATUS "CI Build Target OS Linux built on host OS Linux")
        enable_language(ASM_NASM)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
        set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)

        set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
        set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
        string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf -DARCH_X86_64=1 ")
    endif()
elseif(DEFINED CMAKE_PREFIX_PATH)
    ### assumes cmake was called from QCreator
    if(DEFINED ANDROID_NDK)
        set(TARGET_OS_ANDROID true)

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "QCreator Build Target OS Android built on host OS Windows")
        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message(STATUS "QCreator Build Target OS Android built on host OS Linux")
        else()
            message(FATAL_ERROR "QCreator Build host is Unknown OS attempting to compile Android")
        endif()
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        set(TARGET_OS_WINDOWS true)

        message(STATUS "QCreator Build Target OS Windows built on host OS Windows")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        set(TARGET_OS_LINUX true)

        message(STATUS "QCreator Build Target OS Linux built on host OS Linux")
        enable_language(ASM_NASM)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
        set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)

        set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
        set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
        string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf -DARCH_X86_64=1 ")
    endif()    
else()
    ### assumes cmake was called from command line and needs the enviroment setup

    #set define so can deterimine if need additional enviroment params missing in command line compile
    set(CMD_LINE_MAKE true)
    add_compile_definitions(CMD_LINE_MAKE)

    set(CMAKE_GENERATOR "Unix Makefiles")

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
       message(STATUS "Build host OS Windows")
        if(TARGET_OS_LINUX)
            message(FATAL_ERROR "Target OS Linux built on Windows OS is NOT supported")
        elseif(TARGET_OS_ANDROID)
            list(APPEND CMAKE_PREFIX_PATH  F:/Qt/6.4.3/gcc_64)

            set(CMAKE_SYSTEM_NAME Android)
            set(CMAKE_SYSTEM_VERSION 24) # API level
            set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
            set(CMAKE_ANDROID_NDK F:/Android/android-sdk/ndk/22.1.7171670)

            set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang)
            set(ARM_SYSROOT --sysroot=${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
        elseif(TARGET_OS_WINDOWS)
            list(APPEND CMAKE_PREFIX_PATH  F:/Qt/6.4.3/gcc_64)
            message(STATUS "qt prefix path ${CMAKE_PREFIX_PATH}") 
            
            set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
            set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
            enable_language(ASM_NASM)
            set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
            set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
             
        else()
           message(FATAL_ERROR "TARGET OS is not defined.. caller must define TARGET_OS_LINUX, TARGET_OS_WINDOWS or TARGET_OS_ANDROID")
        endif()
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        if(TARGET_OS_LINUX)
            list(APPEND CMAKE_PREFIX_PATH ~/Qt/6.4.3/gcc_64)

            message(STATUS "qt prefix path ${CMAKE_PREFIX_PATH}") 
            
            enable_language(ASM_NASM)
            set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
            set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)

            set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
            set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
            string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf -DARCH_X86_64=1 ")

            # Load the generic ASMInformation file:
            #set(ASM_DIALECT "_NASM")
            #include(CMakeASMInformation)
            #set(ASM_DIALECT)

            #set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

            # Create a compile option that operates on ASM_NASM files
            # If the target has a property NASM_OBJ_FORMAT, use it, otherwise
            # use the environment variable CMAKE_ASM_NASM_OBJECT_FORMAT
            #add_compile_options(
            #    "$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>, \
            #    $<TARGET_PROPERTY:NASM_OBJ_FORMAT>, ${CMAKE_ASM_NASM_OBJECT_FORMAT}>>"
            #)
       elseif(TARGET_OS_ANDROID)
           list(APPEND CMAKE_PREFIX_PATH  /home/nolimit/Qt/6.4.3/gcc_64)
           
           set(CMAKE_SYSTEM_NAME Android)
           set(CMAKE_SYSTEM_VERSION 23) # API level
           set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
           set(CMAKE_ANDROID_NDK /home/nolimit/Android/Sdk/ndk/22.1.7171670)
           
           set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang)
           set(ARM_SYSROOT --sysroot=${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
       elseif(TARGET_OS_WINDOWS)
            message(FATAL_ERROR "Target OS Windows built on Linux OS is not yet supported")
       else()
           message(FATAL_ERROR "TARGET OS is not defined.. caller must define TARGET_OS_LINUX, TARGET_OS_WINDOWS or TARGET_OS_ANDROID")
       endif()
    else()
       message(FATAL_ERROR "Build host is Unknown OS")  
    endif()
endif()

if(DEFINED TARGET_OS_WINDOWS)
    add_compile_definitions(TARGET_OS_WINDOWS)
    set(TARGET_CPU_X64 true)
    set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Windows)
    set(PROJECT_STATIC_LIB_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitlib/build-staticlibs/Windows)
    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Build Target OS Windows built on host OS Windows")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        message(STATUS "Build Target OS Windows built on host OS Linux is not yet supported")
    else()
        message(FATAL_ERROR "Build host is Unknown OS attempting to compile Windows")
    endif()

elseif(DEFINED TARGET_OS_LINUX)  
    add_compile_definitions(TARGET_OS_LINUX TARGET_CPU_X86_64 TARGET_POSIX PIC)
    set(TARGET_CPU_X86_64 true) #so can use in cmake as if(DEFINED TARGET_CPU_X86_64) statement
    set(TARGET_POSIX true)

    #-fpermissive # to allow address of temp to make VxPeerMgr.cpp
    #sigaction(SIGPIPE, &((struct sigaction){sigpipe_handler}), NULL); compile
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fpermissive")


    set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Linux)
    set(PROJECT_STATIC_LIB_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitlib/build-staticlibs/Linux)
    # create the directories the qt shared libraries will be copied to for debian packaging
    file(MAKE_DIRECTORY "${PROJECT_BIN_DIR}/usr")
    file(MAKE_DIRECTORY "${PROJECT_BIN_DIR}/usr/lib")
    if(WIN32)
        message(STATUS "Build Target OS Linux built on host OS Windows is not supported")
    elseif(UNIX)    
        message(STATUS "Build Target OS Linux built on host OS Linux")
    else()
        message(FATAL_ERROR "Build host is Unknown OS attempting to compile Linux")
    endif()

elseif(DEFINED TARGET_OS_ANDROID)
    add_compile_definitions(TARGET_OS_ANDROID TARGET_POSIX TARGET_CPU_AARCH64 TARGET_CPU_ARM TARGET_CPU_64BIT)
    set(TARGET_CPU_AARCH64 true)
    
    set(TARGET_CPU_ARM true)#??
    set(TARGET_POSIX true)
    set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Android)
    set(PROJECT_STATIC_LIB_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitlib/build-staticlibs/Android)

    #set(CLANG_ASM_TARGET_TRIPLE arm-none-eabi)
    #set(CMAKE_ASM_COMPILER_TARGET ${CLANG_ASM_TARGET_TRIPLE})

    # Don't run the linker on compiler check
    #set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fpermissive")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} --target=aarch64-linux-android23 -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Dstrtod=avpriv_strtod -DPIC -DZLIB_CONST -O3 -fPIC -Qunused-arguments -MMD -MF" )

    #set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,-Map,kernel.map,--gc-sections -fuse-linker-plugin -Wl,--use-blx --specs=nano.specs --specs=nosys.specs" CACHE STRING "")

    
    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Build Target OS Android built on host OS Windows")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        message(STATUS "Build Target OS Android built on host OS Linux")
    else()
        message(FATAL_ERROR "Build host is Unknown OS attempting to compile Android")
    endif()

else()
    message(FATAL_ERROR "TARGET OS is not defined.. caller must define TARGET_OS_LINUX, TARGET_OS_WINDOWS or TARGET_OS_ANDROID")
endif()

# defines for all projects

add_compile_definitions(TARGET_CPU_64BIT)

message(STATUS "bin directory ${PROJECT_BIN_DIR}")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BIN_DIR})
message(STATUS "lib directory ${PROJECT_STATIC_LIB_DIR}")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_STATIC_LIB_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_STATIC_LIB_DIR})

