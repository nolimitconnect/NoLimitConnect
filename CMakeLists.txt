
cmake_minimum_required(VERSION 3.22.1)

# Qt Creator can inject a per-build hook script through
# CMAKE_PROJECT_INCLUDE_BEFORE. If that generated file is removed, CMake fails
# inside project(). Clear stale paths so configure can continue.
if(DEFINED CMAKE_PROJECT_INCLUDE_BEFORE
   AND NOT CMAKE_PROJECT_INCLUDE_BEFORE STREQUAL ""
   AND NOT EXISTS "${CMAKE_PROJECT_INCLUDE_BEFORE}")
    message(WARNING "Ignoring missing CMAKE_PROJECT_INCLUDE_BEFORE: ${CMAKE_PROJECT_INCLUDE_BEFORE}")
    unset(CMAKE_PROJECT_INCLUDE_BEFORE CACHE)
    unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endif()

if(WIN32)
    # CMake 4.2+: MSVC is no longer treated as an ASM compiler.
    # Keep Android on clang for ASM and use NASM for non-Android builds.
    if(POLICY CMP0194)
        cmake_policy(SET CMP0194 OLD)
    endif()

    # Keep MSVC runtime selection on legacy flag mode so ASM-based subprojects
    # are not validated against CMAKE_MSVC_RUNTIME_LIBRARY.
    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 OLD)
    endif() 
endif()

project(MainSystem LANGUAGES C CXX)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

function(nlc_target_add_supported_warning_options target language)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "nlc_target_add_supported_warning_options: target '${target}' does not exist")
    endif()

    string(TOUPPER "${language}" _nlc_lang)
    if(NOT _nlc_lang STREQUAL "C" AND NOT _nlc_lang STREQUAL "CXX")
        message(FATAL_ERROR "nlc_target_add_supported_warning_options: language must be C or CXX")
    endif()

    foreach(_nlc_flag IN LISTS ARGN)
        string(REGEX REPLACE "[^A-Za-z0-9]" "_" _nlc_flag_id "${_nlc_flag}")
        set(_nlc_cache_var "NLC_HAS_${_nlc_lang}_${_nlc_flag_id}")

        if(NOT DEFINED ${_nlc_cache_var})
            if(_nlc_lang STREQUAL "C")
                check_c_compiler_flag("${_nlc_flag}" ${_nlc_cache_var})
            else()
                check_cxx_compiler_flag("${_nlc_flag}" ${_nlc_cache_var})
            endif()
        endif()

        if(${_nlc_cache_var})
            target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:${_nlc_lang}>:${_nlc_flag}>)
        endif()
    endforeach()
endfunction()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------
# Detect CPU architecture (only 64-bit allowed)
# --------------------------
set(TARGET_CPU_UNKNOWN TRUE)

if(MSVC)
    # only x64 supported on windows
    set(TARGET_CPU_X64 TRUE)
    set(TARGET_OS_WINDOWS TRUE)
    message(STATUS "*** TARGET_OS_WINDOWS ***")
    add_compile_definitions(TARGET_OS_WINDOWS TARGET_CPU_X64=1)

else()
    set(TARGET_POSIX true)
    add_compile_definitions(TARGET_POSIX=1)

    if(DEFINED ANDROID_ABI)
        # Android ABIs
        message(STATUS "*** TARGET_OS_ANDROID ***")
        if(ANDROID_ABI STREQUAL "arm64-v8a")
            set(TARGET_CPU_ARM64 TRUE)
            set(TARGET_OS_ANDROID TRUE)
            add_compile_definitions(TARGET_OS_ANDROID TARGET_CPU_ARM64=1)
        elseif(ANDROID_ABI STREQUAL "x86_64")
            set(TARGET_CPU_X64 TRUE)
            set(TARGET_OS_ANDROID TRUE)
            add_compile_definitions(TARGET_OS_ANDROID TARGET_CPU_X64=1)
        else()
            message(FATAL_ERROR "Unsupported Android ABI: ${ANDROID_ABI}. Only 64-bit CPUs allowed.")
        endif()

    else()
        # Non-Android or Windows platforms
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
            message(FATAL_ERROR "CMAKE_SYSTEM_PROCESSOR is empty. Unable to determine CPU architecture.")   
        endif()
        message(STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_PROC_LOWER)
        if(SYSTEM_PROC_LOWER MATCHES "amd64|x86_64|x64")
            set(TARGET_CPU_X64 TRUE)
            set(TARGET_OS_LINUX TRUE)
            add_compile_definitions(TARGET_OS_LINUX=1 TARGET_CPU_X64=1)
        elseif(SYSTEM_PROC_LOWER MATCHES "aarch64|arm64")
            set(TARGET_CPU_ARM64 TRUE)
            set(TARGET_OS_LINUX TRUE)
            add_compile_definitions(TARGET_OS_LINUX=1 TARGET_CPU_ARM64=1)
        else()
            message(FATAL_ERROR "Unsupported CPU: ${CMAKE_SYSTEM_PROCESSOR}. Only x64 or ARM64 allowed.")
        endif()

    endif()
endif()

# if 6.9.3 is not available then fallback to 6.7.3
set(QT_PATH_X64_NEW "$ENV{HOME}/Qt/6.9.3/gcc_64")
set(QT_PATH_X64_OLD "$ENV{HOME}/Qt/6.7.3/gcc_64")
set(QT_PATH_ARM_NEW "$ENV{HOME}/Qt/6.9.3/gcc_arm64")
set(QT_PATH_ARM_OLD "$ENV{HOME}/Qt/6.7.3/gcc_arm64")
# Only override Qt path if not set or invalid
if(NOT DEFINED CMAKE_PREFIX_PATH OR NOT EXISTS "${CMAKE_PREFIX_PATH}")
    if(NOT DEFINED FLATPAKBUILD AND TARGET_OS_LINUX)
        if(TARGET_CPU_ARM64)
            if(EXISTS "${QT_PATH_ARM_NEW}")
                set(CMAKE_PREFIX_PATH "${QT_PATH_ARM_NEW}")
            elseif(EXISTS "${QT_PATH_ARM_OLD}")
                set(CMAKE_PREFIX_PATH "${QT_PATH_ARM_OLD}")
            else()
                message(FATAL_ERROR "No suitable Qt installation found")
            endif()
        else()
            if(EXISTS "${QT_PATH_X64_NEW}")
                set(CMAKE_PREFIX_PATH "${QT_PATH_X64_NEW}")
            elseif(EXISTS "${QT_PATH_X64_OLD}")
                set(CMAKE_PREFIX_PATH "${QT_PATH_X64_OLD}")
            else()
                message(FATAL_ERROR "No suitable Qt installation found")
            endif()
        endif()
        message(STATUS "CMAKE_PREFIX_PATH is now ${CMAKE_PREFIX_PATH}")
    endif()
endif()

# webrtc abseil is extreemly picky about how it is built so build it first
include_directories("${CMAKE_CURRENT_LIST_DIR}/libs")
# --------------------------
# Print summary
# --------------------------
message(STATUS "**** Target Summary Begin ****")
if(TARGET_OS_WINDOWS)
    message(STATUS "OS: Windows")
endif()
if(TARGET_OS_LINUX)
    message(STATUS "OS: Linux")
endif()
if(TARGET_OS_ANDROID)
    message(STATUS "OS: Android")
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # Disable C++20 Module Scanning because '"CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS-NOTFOUND"' is not recognized as an internal or external command,
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
message(STATUS "**** Target Summary Summary end ****")
message(STATUS "========================================")

# NOTE: AEC2 uses less than half the CPU processing that AEC3 requires.
# also AEC2 is less sensitive to timing
set(USE_AEC2 1)
if(USE_AEC2)
    add_compile_definitions(USE_AEC2=1)
    message(STATUS "** Echo canceller backend: AEC2 (libs/libwebrtc-aec2)")
else()
    add_compile_definitions(USE_AEC2=0)
    message(STATUS "** Echo canceller backend: AEC3 (libs/libwebrtc-aec)")
endif()
if(USE_AEC2)
    add_subdirectory(libs/libwebrtc-aec2)
else()
    add_subdirectory(libs/libwebrtc-aec)
endif()

unset(TARGET_CPU_X64)
unset(TARGET_CPU_ARM64)
unset(TARGET_OS_WINDOWS)
unset(TARGET_OS_ANDROID)
unset(TARGET_OS_LINUX)

# # end of abseil.. now on to the build of many types mess
message(STATUS "**** start nolimitconnect configure ****")
message(STATUS "========================================")

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
        #"$<$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:C>>:/permissive>" # Allows non-const char* conversion (Legacy behavior).. Usually set by /std:c++17 or later
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
set(BUILD_SYSTEM_QCREATOR false)
set(BUILD_SYSTEM_VISUAL_CODE false)

project(nolimitconnect LANGUAGES C CXX)

# if linux we can use the arch command to determine host cpu and assuming it is not cross compiled then set default
set(NLC_BUILD_HOST_ARCH "")

# Prefer Qt 6.9.3, but transparently fall back to 6.7.3 when configured
# paths point to a non-existent 6.9.3 installation.
set(NLC_QT_PRIMARY_VERSION "6.9.3")
set(NLC_QT_FALLBACK_VERSION "6.7.3")

function(nlc_apply_qt_fallback_for_var var_name)
    if(NOT DEFINED ${var_name})
        return()
    endif()

    set(_updated_values "")
    set(_used_fallback FALSE)

    foreach(_entry IN LISTS ${var_name})
        set(_resolved_entry "${_entry}")

        string(FIND "${_entry}" "${NLC_QT_PRIMARY_VERSION}" _qt_primary_pos)
        if(_qt_primary_pos GREATER -1 AND NOT EXISTS "${_entry}")
            string(REPLACE "${NLC_QT_PRIMARY_VERSION}" "${NLC_QT_FALLBACK_VERSION}" _qt_fallback_entry "${_entry}")
            if(EXISTS "${_qt_fallback_entry}")
                message(WARNING "Qt ${NLC_QT_PRIMARY_VERSION} path not found for ${var_name}: ${_entry}. Falling back to Qt ${NLC_QT_FALLBACK_VERSION}: ${_qt_fallback_entry}")
                set(_resolved_entry "${_qt_fallback_entry}")
                set(_used_fallback TRUE)
            endif()
        endif()

        list(APPEND _updated_values "${_resolved_entry}")
    endforeach()

    set(${var_name} "${_updated_values}" PARENT_SCOPE)

    if(_used_fallback)
        set(NLC_QT_VER "${NLC_QT_FALLBACK_VERSION}" PARENT_SCOPE)
        string(REPLACE "." "_" _qt_version_underscored "${NLC_QT_FALLBACK_VERSION}")
        set(QT_VERSION "${_qt_version_underscored}" PARENT_SCOPE)
    endif()
endfunction()
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(NOT DEFINED ANDROID_ARCH AND NOT DEFINED TARGET_OS_ANDROID AND NOT DEFINED TARGET_OS_WINDOWS )
        EXECUTE_PROCESS( COMMAND arch COMMAND tr -d '\n' OUTPUT_VARIABLE BUILD_HOST_ARCH )
        message(STATUS "Linux Host Arch ${BUILD_HOST_ARCH}")

        if(NOT ${BUILD_HOST_ARCH} STREQUAL "")
            set(NLC_BUILD_HOST_ARCH ${BUILD_HOST_ARCH})
        endif()

        if(NOT DEFINED TARGET_OS_LINUX AND NOT DEFINED TARGET_OS_WINDOWS AND NOT DEFINED TARGET_OS_ANDROID AND
            NOT DEFINED ANDROID_NDK)

            set(TARGET_OS_LINUX true)
            add_compile_definitions(TARGET_OS_LINUX=1)

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

 set(NLC_QT "${NLC_QT_PRIMARY_VERSION}")
 set(NLC_QT_VER "${NLC_QT}")
 if(DEFINED QT_VERSION)
    #replace qt version like "6_9_3" with path variable like "6.9.3"
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

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        enable_language(ASM_NASM)
    endif()
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

    else()
        message(STATUS "Build host OS Linux")
        set(NLC_PREPATH "/home/nolimit/Qt/")
    endif()

    # when run from within QtCreator
    if(DEFINED ANDROID_ABI)
        set(BUILD_SYSTEM_QCreator true)
        set(TARGET_OS_ANDROID true)
        add_compile_definitions(TARGET_OS_ANDROID=1)

        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            message(FATAL_ERROR "32-bit Android builds are not supported. armeabi-v7a ABI is 32-bit.")
        elseif(ANDROID_ABI STREQUAL "arm64-v8a")
            set(NLC_CPU "CPU_ARM64")
            set(NLC_CPU_BITS "64")
            set(TARGET_CPU_ARM64 true)
            add_compile_definitions(TARGET_CPU_ARM64=1)

        else()
            message(FATAL_ERROR "Unknown Android ABI: ${ANDROID_ABI}")
        endif()

        set(POS1 -1)
        if(DEFINED CMAKE_PREFIX_PATH)
            string(FIND "${CMAKE_PREFIX_PATH}" "6.9.3" POS1)
            if(POS1 GREATER -1)
                message(STATUS "PREFIX_PATH contains 6.9.3 ")
                set(NLC_QT_VER "6.9.3")
                set(QT_VERSION "6_9_3")
            else()
                string(FIND "${CMAKE_PREFIX_PATH}" "6.7.3" POS1)
                if(POS1 GREATER -1)
                    message(STATUS "PREFIX_PATH contains 6.7.3 ")
                    set(NLC_QT_VER "6.7.3")
                    set(QT_VERSION "6_7_3")
                else()
                    message(FATAL_ERROR "PREFIX_PATH did not contain a supported Qt version (6.9.3 or 6.7.3)")
                endif()
            endif()
        endif()

    else()
        set(ANDROID_ABI "x64")
    endif()

    message(STATUS "CPU TYPE " ${NLC_CPU})
    message(STATUS "ANDROID ABI " ${ANDROID_ABI})

    if("${QT_VERSION}" STREQUAL "")
        # Default to 6.9.3 when QT_VERSION is not provided. If that Qt path
        # is unavailable, transparently fall back to 6.7.3.
        set(_nlc_default_qt_version "6.9.3")
        set(_nlc_fallback_qt_version "6.7.3")

        if(DEFINED CMAKE_PREFIX_PATH)
            string(FIND "${CMAKE_PREFIX_PATH}" "${_nlc_default_qt_version}" _nlc_default_qt_pos)
            if(_nlc_default_qt_pos GREATER -1)
                set(NLC_QT_VER "${_nlc_default_qt_version}")
                set(QT_VERSION "6_9_3")
            else()
                string(REPLACE "${_nlc_default_qt_version}" "${_nlc_fallback_qt_version}" _nlc_fallback_prefix "${CMAKE_PREFIX_PATH}")
                if(EXISTS "${_nlc_fallback_prefix}")
                    message(WARNING "Qt ${_nlc_default_qt_version} path not found in CMAKE_PREFIX_PATH. Falling back to Qt ${_nlc_fallback_qt_version}: ${_nlc_fallback_prefix}")
                    set(CMAKE_PREFIX_PATH "${_nlc_fallback_prefix}")
                    set(NLC_QT_VER "${_nlc_fallback_qt_version}")
                    set(QT_VERSION "6_7_3")
                else()
                    message(FATAL_ERROR "QT_VERSION is not set and no supported Qt installation was found. Tried Qt ${_nlc_default_qt_version} and fallback Qt ${_nlc_fallback_qt_version} using CMAKE_PREFIX_PATH='${CMAKE_PREFIX_PATH}'.")
                endif()
            endif()
        else()
            message(FATAL_ERROR "QT_VERSION is not set and CMAKE_PREFIX_PATH is not defined. Cannot locate supported Qt 6.9.3 or 6.7.3.")
        endif()
    endif()

    if("${QT_VERSION}" STREQUAL "6_7_3" OR "${QT_VERSION}" STREQUAL "6_9_3")
        set(NLC_NDK_VERSION 26.1.10909125)
    else()
        message(FATAL_ERROR "Unsupported Qt version: ${QT_VERSION}. Only 6.9.3 and 6.7.3 are supported.")
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
    if(DEFINED CMAKE_PREFIX_PATH)
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
    endif()

    if(DEFINED TARGET_OS_ANDROID)
        message(STATUS "*** TARGET_OS_ANDROID ***")
        enable_language(ASM)
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
            add_compile_definitions(TARGET_OS_LINUX=1 TARGET_CPU_ARM64=1 TARGET_POSIX=1 PIC=1)
        else()
            set(TARGET_CPU_X64 true) #so can use in cmake as if(DEFINED TARGET_CPU_X64) statement
            message(STATUS "TARGET_OS_LINUX TARGET_CPU_X64")
            add_compile_definitions(TARGET_OS_LINUX=1 TARGET_CPU_X64=1 TARGET_POSIX=1 PIC=1)
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
        set(CMAKE_ANDROID_ASSETS_DIRECTORIES "${PROJECT_BIN_DIR}/assets")
        message(STATUS "CMAKE_ANDROID_ASSETS_DIRECTORIES " ${CMAKE_ANDROID_ASSETS_DIRECTORIES})

    endif()

    message(STATUS "2 - CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")

    #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}") not relevant because no dll or shared libraries
    if(DEFINED TARGET_OS_LINUX)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}")
    endif()

    # Apply Qt version fallback for preset-provided paths that may still point
    # at a missing 6.9.3 installation.
    nlc_apply_qt_fallback_for_var(CMAKE_PREFIX_PATH)
    nlc_apply_qt_fallback_for_var(CMAKE_FIND_ROOT_PATH)
    nlc_apply_qt_fallback_for_var(QT_QMAKE_EXECUTABLE)
    nlc_apply_qt_fallback_for_var(QT_HOST_PATH)

    set(_nlc_qt_prefix "${CMAKE_PREFIX_PATH}")
    if(CMAKE_PREFIX_PATH)
        list(GET CMAKE_PREFIX_PATH 0 _nlc_qt_prefix)
    endif()

    set(Qt6_DIR "${_nlc_qt_prefix}/lib/cmake/Qt6")  

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


    if(NOT TARGET_OS_WINDOWS)
        # libjpeg-turbo has speed advantage over libjpeg
        # set if want to use libturbo-jpeg instead of libjpeg
        set(USE_LIBJPEG_TURBO true)
        add_compile_definitions(USE_LIBJPEG_TURBO=1)
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

    if(TARGET_OS_WINDOWS)
        set(NLC_INSTALL_RUNTIME_DIR ".")
    elseif(TARGET_OS_LINUX)
        set(NLC_INSTALL_RUNTIME_DIR "usr/bin")
    else()
        set(NLC_INSTALL_RUNTIME_DIR "bin")
    endif()

    install(TARGETS nolimitconnect
        RUNTIME DESTINATION "${NLC_INSTALL_RUNTIME_DIR}"
        BUNDLE DESTINATION "${NLC_INSTALL_RUNTIME_DIR}"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
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
if(BUILD_SYSTEM_QCREATOR)
    message(STATUS "BUILD_SYSTEM_QCREATOR true")
endif()
if(TARGET_OS_WINDOWS)
    message(STATUS "BUILD system using MSVC to compile for Windows")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" NLC_ARCH)
message(STATUS "NLC_ARCH ${NLC_ARCH}")
if(BUILD_SYSTEM_FLATPAK)
    if(NLC_ARCH MATCHES "arm64|aarch64")
        set(PACKAGE_ARCH "aarch64")
    elseif(NLC_ARCH MATCHES "x86_64|amd64")
        set(PACKAGE_ARCH "x86_64")
    else()
        set(PACKAGE_ARCH ${NLC_ARCH})
    endif()
    set(PACKAGE_FILENAME "${NLC_NAME}-${NLC_VERSION}-${PACKAGE_ARCH}.flatpak")
    set(FLATPAK_REF "${NLC_ID}/${PACKAGE_ARCH}/${NLC_VERSION}")
    message(STATUS "package filename ${PACKAGE_FILENAME} with ref ${FLATPAK_REF}")
else()
    if(NLC_ARCH MATCHES "arm64|aarch64")
        set(PACKAGE_ARCH "arm64-v8a")
    elseif(NLC_ARCH MATCHES "x86_64|amd64")
        set(PACKAGE_ARCH "x64")
    else()
        set(PACKAGE_ARCH ${NLC_ARCH})
    endif()
    if(TARGET_OS_WINDOWS)
        set(PACKAGE_FILENAME "${NLC_NAME}-${NLC_VERSION}-${PACKAGE_ARCH}.exe")
        message(STATUS "package filename ${PACKAGE_FILENAME}")
    elseif(TARGET_OS_LINUX)
        set(PACKAGE_FILENAME "${NLC_NAME}-${NLC_VERSION}-${PACKAGE_ARCH}.deb")
        message(STATUS "package filename ${PACKAGE_FILENAME}")
    elseif(TARGET_OS_ANDROID)
        set(PACKAGE_FILENAME "${NLC_NAME}-${NLC_VERSION}-${PACKAGE_ARCH}.apk")
        message(STATUS "package filename ${PACKAGE_FILENAME}")
    else()
        message(FATAL_ERROR "Unknown target OS for packaging")
    endif()
endif()

# --------------------------
# Packaging targets
# --------------------------
if(BUILD_SYSTEM_FLATPAK)
    set(PACKAGE_OUTPUT_DIR "${PROJECT_SRC_ROOT_DIR}/package/flatpack")
elseif(TARGET_OS_ANDROID)
    set(PACKAGE_OUTPUT_DIR "${PROJECT_SRC_ROOT_DIR}/package/android")
elseif(TARGET_OS_LINUX)
    set(PACKAGE_OUTPUT_DIR "${PROJECT_SRC_ROOT_DIR}/package/linux")
elseif(TARGET_OS_WINDOWS)
    set(PACKAGE_OUTPUT_DIR "${PROJECT_SRC_ROOT_DIR}/package/windows")
else()
    message(FATAL_ERROR "Unknown target OS for package output directory")
endif()

file(MAKE_DIRECTORY "${PACKAGE_OUTPUT_DIR}")
set(PACKAGE_FILEPATH "${PACKAGE_OUTPUT_DIR}/${PACKAGE_FILENAME}")
message(STATUS "package output ${PACKAGE_FILEPATH}")

if(BUILD_SYSTEM_FLATPAK)
    add_custom_target(package_flatpak
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E echo "Flatpak package target path: ${PACKAGE_FILEPATH}"
        COMMENT "Preparing flatpak package output path"
    )
    add_custom_target(package_nolimitconnect DEPENDS package_flatpak)

elseif(TARGET_OS_ANDROID)
    add_custom_target(package_android
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND}
            -DAPK_FILE_PRIMARY="${CMAKE_BINARY_DIR}/nolimitgui/android-build-nolimitconnect/nolimitconnect.apk"
            -DAPK_OUTPUT_DIR_RELEASE="${CMAKE_BINARY_DIR}/nolimitgui/android-build-nolimitconnect/build/outputs/apk/release"
            -DAPK_OUTPUT_DIR_DEBUG="${CMAKE_BINARY_DIR}/nolimitgui/android-build-nolimitconnect/build/outputs/apk/debug"
            -DPACKAGE_OUTPUT_DIR="${PACKAGE_OUTPUT_DIR}"
            -DPACKAGE_FILENAME="${PACKAGE_FILENAME}"
            -P "${CMAKE_SOURCE_DIR}/cmake/copy_package_apk.cmake"
        COMMENT "Copying Android APK to package/android"
    )
    add_dependencies(package_android nolimitconnect_make_apk)
    add_custom_target(package_nolimitconnect DEPENDS package_android)

else()
    set(CPACK_PACKAGE_NAME "${NLC_NAME}")
    set(CPACK_PACKAGE_VERSION "${NLC_VERSION}")
    set(CPACK_PACKAGE_VERSION_MAJOR "${NLC_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${NLC_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${NLC_VERSION_PATCH}")
    set(CPACK_RESOURCE_FILE_README "${PROJECT_SRC_ROOT_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SRC_ROOT_DIR}/LICENSE")
    set(CPACK_PACKAGE_DIRECTORY "${PACKAGE_OUTPUT_DIR}")
    set(CPACK_PACKAGE_FILE_NAME "${NLC_NAME}-${NLC_VERSION}-${PACKAGE_ARCH}")
    set(CPACK_SET_DESTDIR OFF)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${NLC_NAME}")
    if(TARGET_OS_WINDOWS)
        set(CPACK_GENERATOR "NSIS")
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    elseif(TARGET_OS_LINUX)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PROJECT_MAINTAINER}")
    else()
        message(FATAL_ERROR "Unsupported CPack platform")
    endif()

    include(CPack)

    add_custom_target(package_cpack
        COMMAND "${CMAKE_CPACK_COMMAND}" --config "${CMAKE_BINARY_DIR}/CPackConfig.cmake" -B "${PACKAGE_OUTPUT_DIR}"
        COMMENT "Running CPack to generate ${CPACK_GENERATOR} package"
    )
    add_dependencies(package_cpack nolimitconnect)
    add_custom_target(package_nolimitconnect DEPENDS package_cpack)
endif()

message(STATUS "**** Root CMakeLists.txt Done ****")
