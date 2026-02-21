
cmake_minimum_required(VERSION 3.22.1)

project(MainSystem LANGUAGES C CXX)

# --------------------------
# Detect CPU architecture (only 64-bit allowed)
# --------------------------
set(TARGET_CPU_UNKNOWN TRUE)

if(MSVC)
    # only x64 supported on windows
    set(TARGET_CPU_X64 TRUE)
    set(TARGET_OS_WINDOWS TRUE)
    message(STATUS "*** TARGET_OS_WINDOWS ***")
    add_compile_definitions(TARGET_OS_WINDOWS, TARGET_CPU_X64=1)

else()
    if(DEFINED ANDROID_ABI)
        # Android ABIs
        message(STATUS "*** TARGET_OS_ANDROID ***")
        if(ANDROID_ABI STREQUAL "arm64-v8a")
            set(TARGET_CPU_ARM64 TRUE)
            set(TARGET_OS_ANDROID TRUE)
            add_compile_definitions(TARGET_OS_ANDROID, TARGET_CPU_ARM64=1)
        elseif(ANDROID_ABI STREQUAL "x86_64")
            set(TARGET_CPU_X64 TRUE)
            set(TARGET_OS_ANDROID TRUE)
            add_compile_definitions(TARGET_OS_ANDROID, TARGET_CPU_X64=1)
        else()
            message(FATAL_ERROR "Unsupported Android ABI: ${ANDROID_ABI}. Only 64-bit CPUs allowed.")
        endif()

    else()
        # Non-Android platforms

        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_PROC_LOWER)
        if(SYSTEM_PROC_LOWER MATCHES "amd64|x86_64|x64")
            set(TARGET_CPU_X64 TRUE)
            set(TARGET_OS_LINUX TRUE)
            add_compile_definitions(TARGET_OS_LINUX, TARGET_CPU_X64=1)
        elseif(SYSTEM_PROC_LOWER MATCHES "aarch64|arm64")
            set(TARGET_CPU_ARM64 TRUE)
            set(TARGET_OS_LINUX TRUE)
            add_compile_definitions(TARGET_OS_LINUX, TARGET_CPU_ARM64=1)
        else()
            message(FATAL_ERROR "Unsupported CPU: ${CMAKE_SYSTEM_PROCESSOR}. Only x64 or ARM64 allowed.")
        endif()

    endif()
endif()

# webrtc abseil is extreemly picky about how it is built so build it first
include_directories("${CMAKE_CURRENT_LIST_DIR}/libs")
add_subdirectory(libs/libwebrtc-aec)

unset(TARGET_CPU_X64)
unset(TARGET_CPU_ARM64)
unset(TARGET_OS_WINDOWS)
unset(TARGET_OS_ANDROID)
unset(TARGET_OS_LINUX)

# end of abseil.. now on to the build of many types mess

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(MSVC)
    # Remove default flags from the global strings to prevent D9025
    string(REPLACE "/DUNICODE" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/D_UNICODE" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # Remove default Unicode definitions if they exist
    remove_definitions(-DUNICODE -D_UNICODE)
    # Force Multi-Byte Character Set
    add_definitions(-D_MBCS)

    # add_compile_options(/wd4005 /wd4098 /wd4100 /wd4101 /wd4127 /wd4201 /wd4324 /wd4505 /wd4514 /wd4710 /wd4711 /wd4820)

    add_compile_definitions(
            __STDC_CONSTANT_MACROS
            _CRT_SECURE_NO_WARNINGS
            _CRT_NONSTDC_NO_DEPRECATE
            _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
            _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
            _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )

    add_compile_options(
        "$<$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:C>>:/W0>"  # This turns off all warnings for windows builds
        "$<$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:C>>:/Zc:strictStrings->"  # Disables specific check for string literal const-qualifiers
        "$<$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:C>>:/permissive>" # Allows non-const char* conversion (Legacy behavior).. Usually set by /std:c++17 or later
    )

else()
    # These warnings as errors are required so can fix issues that fail build in FlatPack 6.9 (was allowed in 6.7)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Werror=incompatible-pointer-types>)

endif()

# Setup version of application
include(cmake/version.cmake)

# Split NLC_VERSION into major, minor, patch
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" _version_match "${NLC_VERSION}")

if(_version_match)
    set(NLC_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(NLC_VERSION_MINOR ${CMAKE_MATCH_2})
    set(NLC_VERSION_PATCH ${CMAKE_MATCH_3})
else()
    message(FATAL_ERROR "Invalid NLC_VERSION format: ${NLC_VERSION}. Expected MAJOR.MINOR.PATCH")
endif()

message(STATUS "Version: ${NLC_VERSION} MAJOR ${NLC_VERSION_MAJOR} MINOR ${NLC_VERSION_MINOR} PATCH ${NLC_VERSION_PATCH}")

#for debugging deadlocks
#add_compile_definitions(DEBUG_AUDIO_PROCESSOR_LOCK=1)
#add_compile_definitions(DEBUG_PROCESSOR_LOCK=1)
#add_compile_definitions(DEBUG_SKT_MGR_LOCK=1)

# enable log messages in release buid
# this should only be uncommented temporarily to debug release builds
# add_compile_definitions(LOG_IN_RELEASE_BUILD=1)


# force define of DEBUG if build is debug
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

# print command line parameters
message(STATUS "CMakeArgs: BEGIN")
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
message(STATUS "CMakeArgs: END")

message("CMAKE_HOST_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME} CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}")

# determine build system type and OS and CPU TYPE
set(BUILD_SYSTEM_FLATPAK false)
set(BUILD_SYSTEM_CMDLINE false)
set(BUILD_SYSTEM_QCREATOR false)
set(BUILD_SYSTEM_CI_PIPELINE false)
set(BUILD_SYSTEM_VISUAL_CODE false)

project(nolimitconnect LANGUAGES C CXX ASM)

# if linux we can use the arch command to determine host cpu and assuming it is not cross compiled then set default
set(NLC_BUILD_HOST_ARCH "")
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" AND NOT DEFINED MAKE_ENV)
    if(NOT DEFINED ANDROID_ARCH AND NOT DEFINED TARGET_OS_ANDROID AND NOT DEFINED TARGET_OS_WINDOWS )
        EXECUTE_PROCESS( COMMAND arch COMMAND tr -d '\n' OUTPUT_VARIABLE BUILD_HOST_ARCH )
        message(STATUS "Linux Host Arch ${BUILD_HOST_ARCH}")

        if(NOT ${BUILD_HOST_ARCH} STREQUAL "")
            set(NLC_BUILD_HOST_ARCH ${BUILD_HOST_ARCH})
        endif()

        if(NOT DEFINED TARGET_OS_LINUX AND NOT DEFINED TARGET_OS_WINDOWS AND NOT DEFINED TARGET_OS_ANDROID AND
            NOT DEFINED ANDROID_NDK AND NOT DEFINED MAKE_ENV)

            set(TARGET_OS_LINUX true)
            add_compile_definitions(TARGET_OS_LINUX)

            if(${BUILD_HOST_ARCH} STREQUAL "aarch64" )
                if(NOT DEFINED ARM64 AND NOT DEFINED ARM32)
                    set(ARM64 true)
                    add_compile_definitions(ARM64=1)
                    set(TARGET_CPU_ARM64 true)
                    add_compile_definitions(TARGET_CPU_ARM64=1)
                endif()
            endif()

            if(${BUILD_HOST_ARCH} STREQUAL "x86_64" )
                if(NOT DEFINED X64)
                    set(X64 true)
                    add_compile_definitions(X64=1)
                    set(TARGET_CPU_X64 true)
                    add_compile_definitions(TARGET_CPU_X64=1)
                endif()
            endif()

        endif()
    endif()
endif() # if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")

#=== set root directories ===#
set(PROJECT_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
file(MAKE_DIRECTORY "${PROJECT_SRC_ROOT_DIR}/package")
set(PROJECT_PACKAGE_ROOT_DIR "${PROJECT_SRC_ROOT_DIR}/package")

 set(NLC_QT "6.9.3")
 if(DEFINED QT_VERSION)
    #replace qt version like "6_6_3" with path variable like "6.6.3"
    string(REPLACE "_" "." NLC_QT_VER ${QT_VERSION})
    message(STATUS "QT_VERSION " ${QT_VERSION} " NLC_QT_VERSION " ${NLC_QT_VER} )
else()
    message(STATUS "QT_VERSION NOT DEFINED ")
endif()

if(DEFINED FLATPAKBUILD)
    set(BUILD_SYSTEM_FLATPAK true)

    set(FLATPAKBUILD true)

    set(TARGET_OS_LINUX true)
    add_compile_definitions(TARGET_OS_LINUX=1)
    set(TARGET_POSIX true)
    add_compile_definitions(TARGET_POSIX=1)
    set(TARGET_CPU_64BIT true)
    add_compile_definitions(TARGET_CPU_64BIT=1)
    set(ARCH_32_BITS false)
    add_compile_definitions(ARCH_32_BITS=0)
    set(ARCH_64_BITS true)
    add_compile_definitions(ARCH_64_BITS=1)
    add_compile_definitions(NLC_ARCH_LITTLE_ENDIAN=1)
    add_compile_definitions(HAVE_ALIGNED_STACK=1)
    set(NLC_CPU_BITS "64")
    
    if(DEFINED TARGET_CPU_ARM64)
        message(STATUS "Flatpak TARGET_CPU_ARM64")
        set(NLC_CPU "CPU_ARM64")
        set(NLC_CPU_TYPE arm64)

        set(TARGET_CPU_ARM64 true)
        add_compile_definitions(TARGET_CPU_ARM64=1)
        set(ARCH_AARCH64 true)
        add_compile_definitions(ARCH_AARCH64=1)

    else()
        message(STATUS "Flatpak TARGET_CPU_X64")
        set(NLC_CPU "CPU_X64")
        set(NLC_CPU_TYPE x64)

        set(TARGET_CPU_X64 true)
        add_compile_definitions(TARGET_CPU_X64=1)
        set(ARCH_X86_64 true)
        add_compile_definitions(ARCH_X86_64=1)

    endif()

    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
    
    # module path is needed for ConfigStaticDependLib.cmake
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SRC_ROOT_DIR}/cmake/cmake_helpers")  
    
    set(NLC_OS_TYPE linux)
    # set(NLC_BUILD_TYPE debug)
    set(NLC_BUILD_TYPE release)

    enable_language(ASM_NASM)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
    set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
    
    set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
    set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
    set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -DPIC -F dwarf --DARCH_X86_64=1 ")

    message(STATUS "Adding subdir libs")
    add_subdirectory(libs)

    message(STATUS "Adding subdir nolimitgui")
    add_subdirectory(nolimitgui)
    
    message(STATUS "command line make Flatpak nolimitconnect config done")
else()
    # NOT flatpak build
    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        set(USER_HOME_DIR $ENV{USERPROFILE})
        message(STATUS "User Windows Home Directory: ${USER_HOME_DIR}")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        set(USER_HOME_DIR $ENV{HOME})
        message(STATUS "User Linux Home Directory: ${USER_HOME_DIR}")
    else()
        message(FATAL_ERROR "Build host is Unknown ${CMAKE_HOST_SYSTEM_NAME}")
    endif()

    message(STATUS "1 - CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}")
    message(STATUS "1 - CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")

    #defaults
    set(NLC_POSTPATH "/gcc_64")

    set(NLC_CPU "CPU_UNKNOWN")
    set(NLC_CPU_BITS "64")

    enable_language(ASM_NASM)
    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Build host OS Windows")
        set(NLC_PREPATH "F:/Qt/")
        if(NOT DEFINED TARGET_OS_WINDOWS AND NOT DEFINED TARGET_OS_ANDROID)
            if(MSVC)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)

                set(TARGET_OS_WINDOWS true)
                add_compile_definitions(TARGET_OS_WINDOWS)

                set(NLC_CPU "CPU_X64")
                set(TARGET_CPU_X64 true)
                add_compile_definitions(TARGET_CPU_X64)

                set(TARGET_CPU_64BIT true)
                add_compile_definitions(TARGET_CPU_64BIT=1)
                set(ARCH_64_BITS true)
                add_compile_definitions(ARCH_64_BITS=1)
                add_compile_definitions(NLC_ARCH_LITTLE_ENDIAN=1)
                # always used floating point instead of FIXED_POINT dsp operations
                add_compile_definitions(NLC_FLOATING_POINT=1)

            else()
                set(TARGET_OS_ANDROID true)
            endif()
        endif()

    elseif(DEFINED MAKE_ENV)
        set(BUILD_SYSTEM_CI_CD true)
        message(STATUS "Build host GitLab")

        set(NLC_PREPATH "/opt/qt/")

    elseif(DEFINED QT_VERSION)
        set(BUILD_SYSTEM_CMDLINE true)
        message(STATUS "Build host Command Line Linux")
        message(STATUS "Build host OS Linux")
        set(NLC_PREPATH "/home/nolimit/Qt/")

        #replace qt version like "6_9_3" with path variable like "6.9.3"
        string(REPLACE "_" "." NLC_QT_VER ${QT_VERSION})
        message(STATUS "QT_VERSION " ${QT_VERSION} " NLC_QT_VER " ${NLC_QT_VER} )
    else()
        message(STATUS "Build host OS Linux")
        set(NLC_PREPATH "/home/nolimit/Qt/")
    endif()

    # when run from command line
    if(DEFINED ARM32)
        set(NLC_CPU "CPU_ARM32")
        set(NLC_CPU_BITS "32")
        if(DEFINED TARGET_OS_ANDROID)
            set(NLC_POSTPATH "/android_armv7")
        endif()

        set(TARGET_CPU_ARM32 true)
        add_compile_definitions(TARGET_CPU_ARM32=1)

    elseif(DEFINED ARM64)
        set(NLC_CPU "CPU_ARM64")
        set(NLC_CPU_BITS "64")
        if(DEFINED TARGET_OS_ANDROID)
            set(NLC_POSTPATH "/android_arm64_v8a")
        endif()

        set(TARGET_CPU_ARM64 true)
        add_compile_definitions(TARGET_CPU_ARM64=1)

    elseif(DEFINED X64)
        set(NLC_CPU "CPU_X64")
    endif()

    # when run from within QtCreator
    if(DEFINED ANDROID_ABI)
        set(BUILD_SYSTEM_QCreator true)

        set(TARGET_OS_ANDROID true)
        add_compile_definitions(TARGET_OS_ANDROID=1)

        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(NLC_CPU "CPU_ARM32")
            set(NLC_CPU_BITS "32")

            set(TARGET_CPU_ARM32 true)
            add_compile_definitions(TARGET_CPU_ARM32=1)

        elseif(ANDROID_ABI STREQUAL "arm64-v8a")
            set(NLC_CPU "CPU_ARM64")
            set(NLC_CPU_BITS "64")

            set(TARGET_CPU_ARM64 true)
            add_compile_definitions(TARGET_CPU_ARM64=1)

        else()
            message(STATUS "Unknown Android ABI ")
        endif()

        set(POS1 -1)
        if(DEFINED CMAKE_PREFIX_PATH)
            STRING( FIND "${CMAKE_PREFIX_PATH}" "6.6.3" POS1)
            if( POS1 GREATER -1)
                message(STATUS "PREFIX_PATH contains 6.6.3 ")
                set(NLC_QT_VER "6.6.3")
                set(QT_VERSION "6_6_3")

            else()  
                set(POS2 -1)
                STRING( FIND "${CMAKE_PREFIX_PATH}" "6.9.3" POS2)
                if( POS2 GREATER -1)
                    message(STATUS "PREFIX_PATH contains 6.7.3 ")
                    set(NLC_QT_VER " 6.9.3")
                    set(QT_VERSION "6_9_3")

                else()
                    set(POS2 -1)
                    STRING( FIND "${CMAKE_PREFIX_PATH}" "6.10.0" POS2)
                    if( POS2 GREATER -1)
                        message(STATUS "PREFIX_PATH contains 6.10.0 ")
                        set(NLC_QT_VER " 6.10.0")
                        set(QT_VERSION "6_10_0")

                    else()
                        set(POS2 -1)
                        STRING( FIND "${CMAKE_PREFIX_PATH}" "6.10.1" POS2)
                        if( POS2 GREATER -1)
                            message(STATUS "PREFIX_PATH contains 6.10.1 ")
                            set(NLC_QT_VER " 6.10.1")
                            set(QT_VERSION "6_10_1")
                        else()
                            message(ERROR "PREFIX_PATH did not contain any known QT Version ")
                        endif()
                    endif()
                endif()
            endif()
        endif()

    else()
        set(ANDROID_ABI "x64")
    endif()

    message(STATUS "CPU TYPE " ${NLC_CPU})
    message(STATUS "ANDROID ABI " ${ANDROID_ABI})

    if("${QT_VERSION}" STREQUAL "6_6_3" )
        #set(NLC_NDK_VERSION 25.1.8937393)
        set(NLC_NDK_VERSION 26.1.10909125)
    elseif("${QT_VERSION}" STREQUAL "6_9_3" )
        set(NLC_NDK_VERSION 26.1.10909125)
    else()
        #6.9.0 and higher
        set(NLC_NDK_VERSION 27.2.12479018) #is release r26b
    endif()

    message(STATUS "2 - CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" AND NOT DEFINED TARGET_OS_WINDOWS)
        message(STATUS "Windows Before CMAKE_ANDROID_NDK ${CMAKE_ANDROID_NDK}")
        set(CMAKE_ANDROID_NDK "F:/Android/Sdk/ndk/${NLC_NDK_VERSION}")
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
        message(STATUS "Windows After CMAKE_ANDROID_NDK ${CMAKE_ANDROID_NDK}")
    endif()

    if(DEFINED TARGET_OS_WINDOWS)
        unset(TARGET_OS_ANDROID)
        unset(TARGET_OS_ANDROID CACHE)
    endif()

    #=== detect build enviroment and target os ===#
    if(DEFINED MAKE_ENV)
        message(STATUS "*** MAKE_ENV (GitLab) ***")
        #=== assume called from gitlab ci pipeline.. linux only compile and cross compile of android
        set(BUILD_SYSTEM_CI_PIPELINE true)
        set(CI_PIPELINE true)
        set(MANUAL_PACKAGE true)
        add_compile_definitions(CI_PIPELINE)
        message(STATUS "CI Pipeline Build")
        if(DEFINED TARGET_OS_ANDROID)
            set(TARGET_OS_ANDROID true)

            # turn off warnings or gitlab will stop logging
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
            add_compile_options (-w)

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

            # turn off warnings or gitlab will stop logging
            add_compile_options(-Wno-everything)
            set(NLC_POSTPATH "/gcc_64")

            message(STATUS "CI Build Target OS Linux built on host OS Linux " ${CMAKE_PREFIX_PATH})

            enable_language(ASM_NASM)
            set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
            set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)

            set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <FLAGS> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
            set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
            string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf -DARCH_X86_64=1 ")
        endif()

    elseif(DEFINED CMAKE_PREFIX_PATH)
        message(STATUS "*** CMAKE_PREFIX_PATH (QtCreator) *** ${CMAKE_PREFIX_PATH}")
        #=== assumes cmake was called from QCreator
        set(BUILD_SYSTEM_QCREATOR true)
        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            if(MSVC AND NOT DEFINED ANDROID_NDK)
                set(TARGET_OS_WINDOWS true)
                message(STATUS "QCreator Build Target OS Windows built on host OS Windows (MSVC)")
            else()
                set(TARGET_OS_ANDROID true)
                message(STATUS "QCreator Build Target OS Android built on host OS Windows (non-MSVC or ANDROID_NDK)")
            endif()

        elseif(DEFINED ANDROID_NDK)
            set(TARGET_OS_ANDROID true)
            if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
                message(STATUS "QCreator Build Target OS Android built on host OS Linux")
            else()
                message(FATAL_ERROR "QCreator Build host is Unknown OS attempting to compile Android")
            endif()

        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            set(TARGET_OS_LINUX true)

            if(${NLC_BUILD_HOST_ARCH} STREQUAL "aarch64" )
                message(STATUS "QCreator aarch64 Build Target OS Linux built on host OS Linux")
                set(ARM64 true)
                add_compile_definitions(ARM64=1)
                set(TARGET_CPU_ARM64 true)
                add_compile_definitions(TARGET_CPU_ARM64=1)
                set(ARCH_ARM64 true)
                add_compile_definitions(ARCH_ARM64=1)

                enable_language(ASM_NASM)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
                set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm S)

                set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
                set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
                set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -DPIC -F dwarf")

            elseif(${NLC_BUILD_HOST_ARCH} STREQUAL "x86_64" )
                message(STATUS "QCreator x86_64 Build Target OS Linux built on host OS Linux")
                set(X64 true)
                add_compile_definitions(X64=1)
                set(TARGET_CPU_X64 true)
                add_compile_definitions(TARGET_CPU_X64=1)
                message(STATUS "NLC_BUILD_HOST_ARCH TARGET_CPU_X64")
                set(ARCH_X86_64 true)
                add_compile_definitions(ARCH_X86_64=1)

                enable_language(ASM_NASM)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
                set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)

                set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
                set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
                set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -DPIC -F dwarf --DARCH_X86_64=1 ")
            endif()
        endif()

    else()
        message(STATUS "*** CMD_LINE_MAKE ***")
        #=== assumes cmake was called from command line and needs the enviroment setup
        set(BUILD_SYSTEM_CMDLINE true)

        #set define so can deterimine if need additional enviroment params missing in command line compile
        set(CMD_LINE_MAKE true)
        set(MANUAL_PACKAGE true)
        add_compile_definitions(CMD_LINE_MAKE)

        set(CMAKE_GENERATOR "Unix Makefiles")

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "Build host OS Windows")
            message(STATUS "3 - CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")
            message(STATUS "3 - CMAKE_ANDROID_NDK ${CMAKE_ANDROID_NDK}")
            if(TARGET_OS_LINUX)
                message(FATAL_ERROR "Target OS Linux built project(nolimitconnect LANGUAGES C CXX ASM)on Windows OS is NOT supported")
            elseif(TARGET_OS_ANDROID)
                set(CMAKE_ANDROID_NDK F:/Android/Sdk/ndk/${NLC_NDK_VERSION})
                if(${NLC_CPU_BITS} STREQUAL "32")
                    set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang)
                else()
                    set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang)       
                endif()

                set(ARM_SYSROOT --sysroot=${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
           
            elseif(TARGET_OS_WINDOWS)
                set(NLC_POST_PATH, "msvc2019_64")

                list(APPEND CMAKE_PREFIX_PATH  ${NLC_PREPATH} ${NLC_QT_VER} ${NLC_POSTPATH})
                message(STATUS "qt prefix path ${CMAKE_PREFIX_PATH}")

                set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
                set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
                enable_language(ASM_NASM)
                set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
                set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")

            else()
               message(FATAL_ERROR "TARGET OS is not defined.. caller must define TARGET_OS_LINUX, TARGET_OS_WINDOWS or TARGET_OS_ANDROID")
            endif()

        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message(STATUS "Build host OS Linux")
            if(TARGET_OS_LINUX)

                message(STATUS "qt prefix path ${CMAKE_PREFIX_PATH}")

                enable_language(ASM_NASM)
                project(nolimitconnect LANGUAGES C CXX ASM)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
                set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)

                set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <FLAGS> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
                set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
                string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf -DARCH_X86_64=1 ")

           elseif(TARGET_OS_ANDROID)
     
               set(CMAKE_ANDROID_NDK /home/nolimit/Android/Sdk/ndk/${NLC_NDK_VERSION})
               
               set(ANDROID_SDK_ROOT /home/nolimit/Android/Sdk)

               set(ARM_SYSROOT --sysroot=${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
                if(${NLC_CPU_BITS} STREQUAL "32")
                    set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang)
                else()
                    set(ARM_ASM ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang)       
                endif()

           elseif(TARGET_OS_WINDOWS)
                message(FATAL_ERROR "Target OS Windows built on Linux OS is not yet supported")
           else()
               message(FATAL_ERROR "TARGET OS is not defined.. caller must define TARGET_OS_LINUX, TARGET_OS_WINDOWS or TARGET_OS_ANDROID")
           endif()
        else()
           message(FATAL_ERROR "Build host is Unknown OS")
        endif()
    endif()

    if(DEFINED TARGET_OS_ANDROID)
        message(STATUS "*** TARGET_OS_ANDROID ***")
        add_compile_definitions(ANDROID)
        set(CMAKE_SYSTEM_NAME Android)
        set(ANDROID_PLATFORM "android-23")

        set(ANDROID_STL c++_static)

        set(TARGET_POSIX true)
        set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/bin-Android)
        set(PROJECT_ASSETS_DIR ${PROJECT_SRC_ROOT_DIR}/bin-Android/assets)

        set(ANDROID_APK_OUTPUT_DIR ${PROJECT_PACKAGE_DIR})

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "Build Target OS Android built on host OS Windows")
        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message(STATUS "Build Target OS Android built on host OS Linux")
        else()
            message(FATAL_ERROR "Build host is Unknown OS attempting to compile Android")
        endif()

    elseif(DEFINED TARGET_OS_WINDOWS)
        set(TARGET_CPU_X64 true)
        set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Windows)
        set(PROJECT_ASSETS_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Windows/assets)

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "TARGET_OS_WINDOWS Build Target OS Windows built on host OS Windows")
        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message(STATUS "TARGET_OS_WINDOWS Build Target OS Windows built on host OS Linux is not yet supported")
        else()
            message(FATAL_ERROR "Build host is Unknown OS attempting to compile Windows")
        endif()

    elseif(DEFINED TARGET_OS_LINUX)
        message(STATUS "*** TARGET_OS_LINUX ***")

        set(TARGET_POSIX true)
        if(DEFINED TARGET_CPU_ARM64)
            set(TARGET_CPU_ARM64 true) #so can use in cmake as if(DEFINED TARGET_CPU_X64) statement
            message(STATUS "TARGET_OS_LINUX TARGET_CPU_ARM64")
            add_compile_definitions(TARGET_OS_LINUX TARGET_CPU_ARM64=1 TARGET_POSIX PIC)
        else()
            set(TARGET_CPU_X64 true) #so can use in cmake as if(DEFINED TARGET_CPU_X64) statement
            message(STATUS "TARGET_OS_LINUX TARGET_CPU_X64")
            add_compile_definitions(TARGET_OS_LINUX TARGET_CPU_X64 TARGET_POSIX PIC)
        endif()

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

        set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Linux)
        set(PROJECT_ASSETS_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Linux/assets)

        # create the directories the qt shared libraries will be copied to for debian packaging
        file(MAKE_DIRECTORY "${PROJECT_BIN_DIR}/usr")
        file(MAKE_DIRECTORY "${PROJECT_BIN_DIR}/usr/lib")
        if(WIN32)
            message(STATUS "TARGET_OS_WINDOWS Build Target OS Linux built on host OS Windows is not supported")
        elseif(UNIX)
            message(STATUS "TARGET_OS_LINUX Build Target OS Linux built on host OS Linux")
        else()
            message(FATAL_ERROR "Build host is Unknown OS attempting to compile Linux")
        endif()

    else()
        message(FATAL_ERROR "TARGET OS is not defined.. caller must define TARGET_OS_LINUX, TARGET_OS_WINDOWS or TARGET_OS_ANDROID")
    endif()

    if(CMD_LINE_MAKE)
        message(STATUS "*** CMD_LINE_MAKE ***")

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "HOST OS Windows")

            if(DEFINED TARGET_OS_ANDROID)
                # path should be set correctly already

                message(STATUS "CMD_LINE_MAKE Target Android on os windows")

            elseif(DEFINED TARGET_OS_WINDOWS)
                if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
                    set(NLC_POSTPATH "/msvc2019_64")
                    
                    message(STATUS "CMD_LINE_MAKE Compiling windows with MSVC")
                else()
                    set(NLC_POSTPATH "/mingw_64")
                    message(STATUS "CMD_LINE_MAKE Compiling windows with mingw")
                endif()

                list(APPEND CMAKE_PREFIX_PATH "${NLC_PREPATH}${NLC_QT_VER}${NLC_POSTPATH}")

            else()
                message("CMD_LINE_MAKE TARGET UNKNOWN")
            endif()

        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message("HOST OS Linux")
            if(DEFINED TARGET_OS_ANDROID)
                # path should be set correctly already
                list(APPEND CMAKE_PREFIX_PATH "${NLC_PREPATH}${NLC_QT_VER}${NLC_POSTPATH}")
                
                message(STATUS "CMD_LINE_MAKE Target Android")

            elseif(DEFINED TARGET_OS_LINUX)
                # path should be set correctly already
                message(STATUS "*** BEFORE CMD_LINE_MAKE Target Linux with prefix ${CMAKE_PREFIX_PATH}")
                list(APPEND CMAKE_PREFIX_PATH "${NLC_PREPATH}${NLC_QT_VER}${NLC_POSTPATH}")
                
                message(STATUS "*** AFTER CMD_LINE_MAKE Target Linux with prefix ${CMAKE_PREFIX_PATH}")

            else()
                message("CMD_LINE_MAKE TARGET UNKNOWN")
            endif()

        else()
            message("CMD_LINE_MAKE CMAKE_HOST_SYSTEM_NAME UNKNOWN")
        endif()
    endif()

    if(CI_PIPELINE)   
        message(STATUS "*** CI_PIPELINE ***")

        if(DEFINED TARGET_OS_ANDROID)
            set(CMAKE_ANDROID_NDK "/opt/Sdk/ndk/${NLC_NDK_VERSION}")
            if(DEFINED ARM32)
                set(CMAKE_PREFIX_PATH "/opt/Qt-android-${NLC_QT_VER}/android_armv7")
            else()
                set(CMAKE_PREFIX_PATH "/opt/Qt-android-${NLC_QT_VER}/android_arm64_v8a")
            endif()

            list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
            set(ANDROID_SDK_ROOT "/opt/android-sdk")

            set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
            message(STATUS "CI Pipeline Target Android")

        elseif(DEFINED TARGET_OS_LINUX)
            set(CMAKE_PREFIX_PATH "/opt/Qt-amd64-${NLC_QT_VER}")
            message(STATUS "CI Pipeline Target Linux")
           
        else()
            message("CI_PIPELINE TARGET UNKNOWN")
        endif()
    endif()

    if(DEFINED CMD_LINE_MAKE)
        #fix issue where depend lib player-nlc uses qt but from command line android cannot find the qt packages etc
        if(DEFINED TARGET_OS_ANDROID)
            if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
                list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
                set(ANDROID_SDK_ROOT /home/nolimit/Android/Sdk)
                set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
            endif()
        endif()
    endif()

    message(STATUS "2 - CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")

    #=== project folders setup ===#
    if(DEFINED TARGET_OS_ANDROID)
        set(NLC_OS_TYPE android)
    elseif(DEFINED TARGET_OS_LINUX)
        set(NLC_OS_TYPE linux)
    elseif(DEFINED TARGET_OS_WINDOWS)
        set(NLC_OS_TYPE windows)
    endif()

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(NLC_BUILD_TYPE debug)
    else()
        set(NLC_BUILD_TYPE release)
    endif()

    if(${NLC_CPU} STREQUAL "CPU_ARM32")
        set(NLC_CPU_TYPE arm32)
    elseif(${NLC_CPU} STREQUAL "CPU_ARM64")
        set(NLC_CPU_TYPE arm64)
    elseif(${NLC_CPU_BITS} STREQUAL "32")
        set(NLC_CPU_TYPE x86)
    else()
        set(NLC_CPU_TYPE x64)
    endif()

    # set project static libs directory
    file(MAKE_DIRECTORY "${PROJECT_SRC_ROOT_DIR}/build-staticlibs/${NLC_OS_TYPE}")
    file(MAKE_DIRECTORY "${PROJECT_SRC_ROOT_DIR}/build-staticlibs/${NLC_OS_TYPE}/${NLC_BUILD_TYPE}")
    file(MAKE_DIRECTORY "${PROJECT_SRC_ROOT_DIR}/build-staticlibs/${NLC_OS_TYPE}/${NLC_BUILD_TYPE}/${NLC_CPU_TYPE}")
    set(PROJECT_STATIC_LIB_DIR "${PROJECT_SRC_ROOT_DIR}/build-staticlibs/${NLC_OS_TYPE}/${NLC_BUILD_TYPE}/${NLC_CPU_TYPE}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_STATIC_LIB_DIR}")

    # set project package directory
    file(MAKE_DIRECTORY "${PROJECT_PACKAGE_ROOT_DIR}/${NLC_OS_TYPE}")
    file(MAKE_DIRECTORY "${PROJECT_PACKAGE_ROOT_DIR}/${NLC_OS_TYPE}/${NLC_BUILD_TYPE}")
    file(MAKE_DIRECTORY "${PROJECT_PACKAGE_ROOT_DIR}/${NLC_OS_TYPE}/${NLC_BUILD_TYPE}/${NLC_CPU_TYPE}")
    set(PROJECT_PACKAGE_DIR "${PROJECT_PACKAGE_ROOT_DIR}/${NLC_OS_TYPE}/${NLC_BUILD_TYPE}/${NLC_CPU_TYPE}")


    # defines for all projects
    if(${NLC_CPU} STREQUAL "CPU_ARM32")
        #add_compile_options(-Wno-dev) #suppress dev warnings which even Qt spits out when compiling for armeabi-v7a
        if(DEFINED TARGET_OS_ANDROID)
            set(NLC_POSTPATH "/android_armv7")
            set(ANDROID_ABI "armeabi-v7a")
            set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
            message(STATUS "CMAKE_ANDROID_ARCH_ABI " ${CMAKE_ANDROID_ARCH_ABI})
            set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=armv7-a -mtune=cortex-a9 --target=arm-linux-android23 -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Dstrtod=avpriv_strtod -DPIC -DZLIB_CONST -O3 -fPIC -Qunused-arguments -MMD -MF" )
            add_compile_definitions(TARGET_OS_ANDROID TARGET_POSIX TARGET_CPU_ARM32 TARGET_CPU_32BIT)
        else()
            message(STATUS "COMPILING ARM32 for Linux")
        endif()

        set(TARGET_CPU_ARM32 true) # general cpu type


    elseif(${NLC_CPU} STREQUAL "CPU_ARM64")
        if(DEFINED TARGET_OS_ANDROID)
            set(NLC_POSTPATH "/android_arm64_v8a")
            set(ANDROID_ABI "arm64-v8a")
            set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
            message(STATUS "CMAKE_ANDROID_ARCH_ABI " ${CMAKE_ANDROID_ARCH_ABI})
            set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} --target=aarch64-linux-android23 -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Dstrtod=avpriv_strtod -DPIC -DZLIB_CONST -O3 -fPIC -Qunused-arguments -MMD -MF" )
            add_compile_definitions(TARGET_OS_ANDROID TARGET_POSIX TARGET_CPU_ARM64 TARGET_CPU_64BIT)
        else()
            message(STATUS "COMPILING ARM64 for Linux")
        endif()

        set(TARGET_CPU_ARM64 true)


    elseif(${NLC_CPU} STREQUAL "CPU_ARM64")
        add_compile_definitions(TARGET_CPU_64BIT)
        add_compile_definitions(TARGET_CPU_ARM64)
        message(STATUS "NLC_CPU TARGET_CPU_ARM64")
    endif()

    if(DEFINED TARGET_OS_ANDROID)
        if(MANUAL_PACKAGE)
            message(STATUS "MANUAL_PACKAGE is defined ")
            set(ANDROID_STL c++_static)
            add_compile_definitions(ANDROID_STL=c++_static)
        endif()

        set(CMAKE_ANDROID_ASSETS_DIRECTORIES "${PROJECT_BIN_DIR}/assets")
        message(STATUS "CMAKE_ANDROID_ASSETS_DIRECTORIES " ${CMAKE_ANDROID_ASSETS_DIRECTORIES})

    endif()

    message(STATUS "2 - CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")

    #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}") not relevant because no dll or shared libraries
    if(DEFINED TARGET_OS_LINUX)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}")
    endif()

    set(Qt6_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt6")  

    # module path is needed for ConfigStaticDependLib.cmake
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SRC_ROOT_DIR}/cmake/cmake_helpers")  

    message(STATUS "CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}")
    message(STATUS "CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}")
    message(STATUS "CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message(STATUS "CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")


    if(DEFINED TARGET_OS_ANDROID)
        message(STATUS "CMAKE_ANDROID_ARCH_ABI ${CMAKE_ANDROID_ARCH_ABI}")
        message(STATUS "CMAKE_ANDROID_NDK ${CMAKE_ANDROID_NDK}")
        message(STATUS "android sdk directory ${ANDROID_SDK_ROOT}")
    endif()

    message(STATUS "bin directory ${PROJECT_BIN_DIR}")
    message(STATUS "assets directory ${PROJECT_ASSETS_DIR}")
    message(STATUS "static lib directory ${PROJECT_STATIC_LIB_DIR}")
    message(STATUS "package directory ${PROJECT_PACKAGE_DIR}")
    message(STATUS "source root directory ${PROJECT_SRC_ROOT_DIR}")

    message(STATUS "project-nlc Qt6_DIR ${Qt6_DIR}")
    message(STATUS "project-nlc Qt6Core_DIR ${Qt6Core_DIR}")
    message(STATUS "project-nlc Qt6Widgets_DIR ${Qt6Widgets_DIR}")
    message(STATUS "project nlc is defined")

    #=== project subdirectories ===#
    message(STATUS "project nlc adding dependant libs with prefix ${CMAKE_PREFIX_PATH}")


    message(STATUS "project nlc cpu ${NLC_CPU}")
    message(STATUS "project nlc cpu type ${NLC_CPU_TYPE}")
    message(STATUS "project nlc cpu bits ${NLC_CPU_BITS}")
    message(STATUS "project nlc android api ${ANDROID_ABI}")
    message(STATUS "root project nlc android ndk ${CMAKE_ANDROID_NDK}")
    message(STATUS "project nlc toolchain file ${CMAKE_TOOLCHAIN_FILE}")

    if(DEFINED TARGET_OS_WINDOWS)
        message(STATUS "TARGET_OS_WINDOWS is defined")
    endif()
    if(DEFINED TARGET_OS_LINUX)
        message(STATUS "TARGET_OS_LINUX is defined")
    endif()
    if(DEFINED TARGET_OS_ANDROID)
        message(STATUS "TARGET_OS_ANDROID is defined")
        # use native java camera capture to replace buggy qt android camera
        set(ENABLE_JAVA_CAM true)
        add_compile_definitions(ENABLE_JAVA_CAM=1)
    endif()

    if(DEFINED TARGET_CPU_X64)
        message(STATUS "TARGET_CPU_X64 is defined")
        set(TARGET_CPU_64BIT true)
        add_compile_definitions(TARGET_CPU_64BIT=1)
        set(ARCH_32_BITS false)
        add_compile_definitions(ARCH_32_BITS=0)
        set(ARCH_64_BITS true)
        add_compile_definitions(ARCH_64_BITS=1)

    endif()

    if(DEFINED TARGET_CPU_X86)
        message(STATUS "TARGET_CPU_X86 is defined")
        set(TARGET_CPU_32BIT true)
        add_compile_definitions(TARGET_CPU_32BIT=1)
        set(ARCH_32_BITS true)
        add_compile_definitions(ARCH_32_BITS=1)
        set(ARCH_64_BITS false)
        add_compile_definitions(ARCH_64_BITS=0)
    endif()

    if(DEFINED TARGET_CPU_ARM64)
        message(STATUS "TARGET_CPU_ARM64 is defined")
        set(TARGET_CPU_64BIT true)
        add_compile_definitions(TARGET_CPU_64BIT=1)
        set(ARCH_32_BITS false)
        add_compile_definitions(ARCH_32_BITS=0)
        set(ARCH_64_BITS true)
        add_compile_definitions(ARCH_64_BITS=1)
        if(NOT DEFINED TARGET_OS_WINDOWS)
            # libjpeg-turbo has speed advantage over libjpeg
            # set if want to use libturbo-jpeg instead of libjpeg
            set(USE_LIBJPEG_TURBO true)
            add_compile_definitions(USE_LIBJPEG_TURBO=1)
        endif()
    endif()
    if(DEFINED TARGET_CPU_ARM32)
        message(STATUS "TARGET_CPU_ARM32 is defined")
        set(TARGET_CPU_32BIT true)
        add_compile_definitions(TARGET_CPU_32BIT=1)
        set(ARCH_32_BITS true)
        add_compile_definitions(ARCH_32_BITS=1)
        set(ARCH_64_BITS false)
        add_compile_definitions(ARCH_64_BITS=0)
    endif()

    if(DEFINED ARM64)
        message(STATUS "ARM64 is defined")
    endif()
    if(DEFINED ARM32)
        message(STATUS "ARM32 is defined")
    endif()
    if(DEFINED X64)
        message(STATUS "X64 is defined")
    endif()

    if(DEFINED TARGET_CPU_64BIT)
        message(STATUS "TARGET_CPU_64BIT is defined")
    endif()
    if(DEFINED TARGET_CPU_32BIT)
        message(STATUS "TARGET_CPU_32BIT is defined")
    endif()

    if(ARCH_64_BITS)
        message(STATUS "ARCH_64_BITS is true")
    endif()
    if(ARCH_32_BITS)
        message(STATUS "ARCH_32_BITS is true")
    endif()

    add_subdirectory(libs)

    message(STATUS "project nlc dependant libs were added with prefix ${CMAKE_PREFIX_PATH}")

    add_subdirectory(nolimitgui)

    message(STATUS "project nlc applications was added with prefix ${CMAKE_PREFIX_PATH}")

    install(TARGETS nolimitconnect
        RUNTIME DESTINATION "${PROJECT_BIN_DIR}"
        BUNDLE DESTINATION "${PROJECT_PACKAGE_DIR}"
        LIBRARY DESTINATION "${PROJECT_BIN_DIR}"
        ARCHIVE DESTINATION "${PROJECT_STATIC_LIB_DIR}"
    )

    message(STATUS "project nlc end with prefix")
endif() # flatpak build

# --------------------------
# Print summary
# --------------------------
message(STATUS "**** Target Summary ****")
if(TARGET_OS_WINDOWS)
    message(STATUS "OS: Windows")
endif()
if(TARGET_OS_LINUX)
    message(STATUS "OS: Linux")
endif()
if(TARGET_OS_ANDROID)
    message(STATUS "OS: Android")
endif()
if(TARGET_OS_APPLE)
    message(STATUS "OS: Apple (macOS/iOS)")
endif()

if(TARGET_CPU_X64)
    message(STATUS "CPU: x64 (64-bit)")
elseif(TARGET_CPU_ARM64)
    message(STATUS "CPU: ARM64 (64-bit)")
else()
    message(FATAL_ERROR "Unknown CPU Type for ${CMAKE_SYSTEM_NAME}")
endif()

if(BUILD_SYSTEM_FLATPAK)
    message(STATUS "BUILD_SYSTEM_FLATPAK true")
endif()
if(BUILD_SYSTEM_CMDLINE)
    message(STATUS "BUILD_SYSTEM_CMDLINE true")
endif()
if(BUILD_SYSTEM_QCREATOR)
    message(STATUS "BUILD_SYSTEM_QCREATOR true")
endif()
if(BUILD_SYSTEM_CI_PIPELINE)
    message(STATUS "BUILD_SYSTEM_CI_PIPELINE true")
endif()
if(TARGET_OS_WINDOWS)
    message(STATUS "BUILD system using MSVC to compile for Windows")
endif()

message(STATUS "**** Root CMakeLists.txt Done ****")
