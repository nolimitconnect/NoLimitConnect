
cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#=== application version and info ===#
set(PROJECT_MAINTAINER bjones.engineer@gmail.com)
set(PROJECT_WEBSITE_URL nolimitconnect.com)
set(PROJECT_VENDOR nolimitconnect.com)
set(NLC_NAME "nolimitconnect")
set(NLC_VERSION "1.0.1")
set(NLC_VERSION_MAJOR "1")
set(NLC_VERSION_MINOR "0")
set(NLC_VERSION_PATCH "1")

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

if (CMAKE_BUILD_TYPE MATCHES Debug)
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

#=== set root directories ===#
set(PROJECT_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
file(MAKE_DIRECTORY "${PROJECT_SRC_ROOT_DIR}/package")
set(PROJECT_PACKAGE_ROOT_DIR "${PROJECT_SRC_ROOT_DIR}/package")

if(DEFINED FLATPAKBUILD)
    message(STATUS "Flatpak Build Target OS Linux")

    project(nolimitconnect LANGUAGES C CXX ASM)

    set(FLATPAKBUILD true)

    set(TARGET_OS_LINUX true)
    add_compile_definitions(TARGET_OS_LINUX=1)
    set(TARGET_POSIX true)
    add_compile_definitions(TARGET_POSIX=1)
    set(TARGET_CPU_64BIT true)
    add_compile_definitions(TARGET_CPU_64BIT=1)
    set(TARGET_CPU_X86_64 true)
    add_compile_definitions(TARGET_CPU_X86_64=1)
    set(ARCH_X86_64 true)
    add_compile_definitions(ARCH_X86_64=1)
    set(ARCH_32_BITS false)
    add_compile_definitions(ARCH_32_BITS=0)
    set(ARCH_64_BITS true)
    add_compile_definitions(ARCH_64_BITS=1)
    add_compile_definitions(NLC_ARCH_LITTLE_ENDIAN=1)
    add_compile_definitions(HAVE_ALIGNED_STACK=1)
    
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
    
    # module path is needed for ConfigStaticDependLib.cmake
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SRC_ROOT_DIR}/cmake/cmake_helpers")  
    
    set(NLC_OS_TYPE linux)
    # set(NLC_BUILD_TYPE debug)
    set(NLC_BUILD_TYPE release)
    set(NLC_CPU_TYPE x64)
    set(NLC_CPU "CPU_X64")
    set(NLC_CPU_BITS "64")
    
    enable_language(ASM_NASM)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
    set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
    
    set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
    set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
    #string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf --DARCH_X86_64=1 ")
    set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -DPIC -F dwarf --DARCH_X86_64=1 ")

    message(STATUS "Adding subdir nolimitlib")
    add_subdirectory(nolimitlib)

    message(STATUS "Adding subdir nolimitgui")
    add_subdirectory(nolimitgui)
    
    message(STATUS "command line make Flatpak nolimitconnect config done")
else()
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

    #defaults
    set(NLC_QT "6.4.3")
    set(NLC_POSTPATH "/gcc_64")

    set(NLC_CPU "CPU_X64")
    set(NLC_CPU_BITS "64")

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Build host OS Windows")
        set(NLC_PREPATH "F:/Qt/")
        set(TARGET_OS_ANDROID true)    
        if(DEFINED ANDROID_NDK)
            message(STATUS "Command line NDK " ${ANDROID_NDK})
        else()
            set(CMAKE_ANDROID_NDK F:/Android/android-sdk/ndk/25.1.8937393)
        endif()

    elseif(DEFINED MAKE_ENV)
        message(STATUS "Build host GitLab")
        set(NLC_PREPATH "/opt/qt/")

    elseif(DEFINED QT_VERSION)
        message(STATUS "Build host Command Line Linux")
        message(STATUS "Build host OS Linux")
        set(NLC_PREPATH "/home/nolimit/Qt/")
        if(${QT_VERSION} STREQUAL "7_0_2")
            set(NLC_QT_VER "7.0.2")
        elseif(${QT_VERSION} STREQUAL "7_0_1")
            set(NLC_QT_VER "7.0.1")
        elseif(${QT_VERSION} STREQUAL "6_6_4")
            set(NLC_QT_VER "6.6.4")
        elseif(${QT_VERSION} STREQUAL "6_6_3")
            set(NLC_QT_VER "6.6.3")
        elseif(${QT_VERSION} STREQUAL "6_6_2")
            set(NLC_QT_VER "6.6.2")
        elseif(${QT_VERSION} STREQUAL "6_6_1")
            set(NLC_QT_VER "6.6.1") 
        else()
            message(STATUS "Unknown command line QT_VERSION " ${QT_VERSION})
        endif()

    else()
        message(STATUS "Build host OS Linux")
        set(NLC_PREPATH "/home/nolimit/Qt/")

    endif()

    # when run from command line
    if(DEFINED ARM32)
        set(NLC_CPU "CPU_ARM32")
        set(NLC_CPU_BITS "32")
        set(NLC_POSTPATH "/android_armv7")
    elseif(DEFINED ARM64)
        set(NLC_CPU "CPU_ARM64")
        set(NLC_POSTPATH "/android_arm64_v8a")
    else()
        set(NLC_CPU "CPU_X64")
    endif()

    # when run from within QtCreator
    if(DEFINED ANDROID_ABI)
        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(NLC_CPU "CPU_ARM32")
            set(NLC_CPU_BITS "32")
        elseif(ANDROID_ABI STREQUAL "arm64-v8a")
            set(NLC_CPU "CPU_ARM64")
            set(NLC_CPU_BITS "64")
        else()
            message(STATUS "Unknown Android ABI ")
        endif()

        set(POS1 -1)
        if(DEFINED CMAKE_PREFIX_PATH)
            STRING( FIND "${CMAKE_PREFIX_PATH}" "6.6.2" POS1)
            if( POS1 GREATER -1)
                message(STATUS "PREFIX_PATH contains 6.6.2 ")
                set(NLC_QT_VER " 6.6.2")
            else()  
                set(POS2 -1)
                STRING( FIND "${CMAKE_PREFIX_PATH}" "6.6.3" POS2)
                if( POS2 GREATER -1)
                    message(STATUS "PREFIX_PATH contains 6.6.3 ")
                    set(NLC_QT_VER " 6.6.3")

                else()
                    set(POS2 -1)
                    STRING( FIND "${CMAKE_PREFIX_PATH}" "6.6.4" POS2)
                    if( POS2 GREATER -1)
                        message(STATUS "PREFIX_PATH contains 6.6.4 ")
                        set(NLC_QT_VER " 6.6.4")

                    else()
                        set(POS2 -1)
                        STRING( FIND "${CMAKE_PREFIX_PATH}" "7.0.0" POS2)
                        if( POS2 GREATER -1)
                            message(STATUS "PREFIX_PATH contains 7.0.0 ")
                            set(NLC_QT_VER " 7.0.0")

                        else()
                            set(POS3 -1)
                            STRING( FIND "${CMAKE_PREFIX_PATH}" "7.0.1" POS3)
                            if( POS3 GREATER -1)
                                message(STATUS "PREFIX_PATH contains 7.0.1 ")
                                set(NLC_QT_VER " 7.0.1")

                            else()
                                message(ERROR "PREFIX_PATH did not contain any known QT Version ")
                            endif()
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

    #=== detect build enviroment and target os ===#
    if(DEFINED MAKE_ENV)
        message(STATUS "*** MAKE_ENV (GitLab) ***")
        #=== assume called from gitlab ci pipeline.. linux only compile and cross compile of android
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

            set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <FLAGS> <DEFINES> <INCLUDES> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
            set(CMAKE_DEPFILE_FLAGS_ASM_NASM "-MD <DEP_FILE> -MT <DEP_TARGET>")
            string(APPEND CMAKE_ASM_NASM_FLAGS " -DPIC -F dwarf -DARCH_X86_64=1 ")
        endif()

    else()
        message(STATUS "*** CMD_LINE_MAKE ***")
        #=== assumes cmake was called from command line and needs the enviroment setup

        #set define so can deterimine if need additional enviroment params missing in command line compile
        set(CMD_LINE_MAKE true)
        set(MANUAL_PACKAGE true)
        add_compile_definitions(CMD_LINE_MAKE)

        set(CMAKE_GENERATOR "Unix Makefiles")

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
           message(STATUS "Build host OS Windows")
            if(TARGET_OS_LINUX)
                message(FATAL_ERROR "Target OS Linux built project(nolimitconnect LANGUAGES C CXX ASM)on Windows OS is NOT supported")
            elseif(TARGET_OS_ANDROID)
                set(CMAKE_ANDROID_NDK F:/Android/android-sdk/ndk/25.1.8937393)
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
     
               set(CMAKE_ANDROID_NDK /home/nolimit/Android/Sdk/ndk/25.1.8937393)
               
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

    if(DEFINED TARGET_OS_WINDOWS)
        message(STATUS "*** TARGET_OS_WINDOWS ***")
        add_compile_definitions(TARGET_OS_WINDOWS)
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
        add_compile_definitions(TARGET_OS_LINUX TARGET_CPU_X86_64 TARGET_POSIX PIC)
        set(TARGET_CPU_X86_64 true) #so can use in cmake as if(DEFINED TARGET_CPU_X86_64) statement
        set(TARGET_POSIX true)

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

    elseif(DEFINED TARGET_OS_ANDROID)
        message(STATUS "*** TARGET_OS_ANDROID ***")
        add_compile_definitions(ANDROID)
        set(CMAKE_SYSTEM_NAME Android)
        set(ANDROID_PLATFORM "android-24")
        #set(CMAKE_SYSTEM_VERSION 33) # API level

        set(ANDROID_STL c++_static)

        set(TARGET_POSIX true)
        set(PROJECT_BIN_DIR ${PROJECT_SRC_ROOT_DIR}/bin-Android)
        set(PROJECT_ASSETS_DIR ${PROJECT_SRC_ROOT_DIR}/nolimitgui/bin-Android/assets)

        set(ANDROID_APK_OUTPUT_DIR ${PROJECT_PACKAGE_DIR})

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

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
        if(QT_VERSION STREQUAL 6_6_4)
           if(DEFINED TARGET_OS_ANDROID)
               set(CMAKE_ANDROID_NDK "/opt/android-sdk/ndk/25.1.8937393")
               if(DEFINED ARM32)
                   set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.4/android_armv7")
               else()
                   set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.4/android_arm64_v8a")
               endif()

               list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
               set(ANDROID_SDK_ROOT "/opt/android-sdk")

               set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
               message(STATUS "CI Pipeline Target Android")

           elseif(DEFINED TARGET_OS_LINUX)
               set(CMAKE_PREFIX_PATH "/opt/Qt-amd64-6.6.4")
               message(STATUS "CI Pipeline Target Linux")
           endif()
        elseif(QT_VERSION STREQUAL 6_6_3)
           if(DEFINED TARGET_OS_ANDROID)
               set(CMAKE_ANDROID_NDK "/opt/android-sdk/ndk/25.1.8937393")
               if(DEFINED ARM32)
                   set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.3/android_armv7")
               else()
                   set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.3/android_arm64_v8a")
               endif()

               list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
               set(ANDROID_SDK_ROOT "/opt/android-sdk")

               set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
               message(STATUS "CI Pipeline Target Android")

           elseif(DEFINED TARGET_OS_LINUX)
               set(CMAKE_PREFIX_PATH "/opt/Qt-amd64-6.6.3")
               message(STATUS "CI Pipeline Target Linux")
           endif()
        elseif(QT_VERSION STREQUAL 6_6_2)
            if(DEFINED TARGET_OS_ANDROID)
                set(CMAKE_ANDROID_NDK "/opt/android-sdk/ndk/25.1.8937393")
                if(DEFINED ARM32)
                    set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.2/android_armv7")
                else()
                    set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.2/android_arm64_v8a")
                endif()
                
                list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
                set(ANDROID_SDK_ROOT "/opt/android-sdk")
                
                set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
                message(STATUS "CI Pipeline Target Android")


            elseif(DEFINED TARGET_OS_LINUX)
                set(CMAKE_PREFIX_PATH "/opt/Qt-amd64-6.6.2")
                message(STATUS "CI Pipeline Target Linux")
            endif()
            
        elseif(QT_VERSION STREQUAL 6_6_1)
            if(DEFINED TARGET_OS_ANDROID)
                set(CMAKE_ANDROID_NDK "/opt/android-sdk/ndk/25.1.8937393")
                if(DEFINED ARM32)
                    set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.1/android_armv7")
                else()
                    set(CMAKE_PREFIX_PATH "/opt/Qt-android-6.6.1/android_arm64_v8a")
                endif()
                
                list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
                set(ANDROID_SDK_ROOT "/opt/android-sdk")
                
                set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
                message(STATUS "CI Pipeline Target Android")


            elseif(DEFINED TARGET_OS_LINUX)
                set(CMAKE_PREFIX_PATH "/opt/Qt-amd64-6.6.1")
                message(STATUS "CI Pipeline Target Linux")
            endif()
            
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

    #=== project folders setup ===#
    if(DEFINED TARGET_OS_ANDROID)
        set(NLC_OS_TYPE android)
    elseif(DEFINED TARGET_OS_LINUX)
        set(NLC_OS_TYPE linux)
    elseif(DEFINED TARGET_OS_WINDOWS)
        set(NLC_OS_TYPE linux)
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

        set(NLC_POSTPATH "/android_armv7")
        set(ANDROID_ABI "armeabi-v7a")
        set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
        message(STATUS "CMAKE_ANDROID_ARCH_ABI " ${CMAKE_ANDROID_ARCH_ABI})

        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=armv7-a -mtune=cortex-a9 --target=arm-linux-android23 -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Dstrtod=avpriv_strtod -DPIC -DZLIB_CONST -O3 -fPIC -Qunused-arguments -MMD -MF" )
        set(TARGET_CPU_ARM true) # general cpu type
        add_compile_definitions(TARGET_OS_ANDROID TARGET_POSIX TARGET_CPU_ARM TARGET_CPU_32BIT)

    elseif(${NLC_CPU} STREQUAL "CPU_ARM64")
        set(NLC_POSTPATH "/android_arm64_v8a")
        set(ANDROID_ABI "arm64-v8a")
        set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
        message(STATUS "CMAKE_ANDROID_ARCH_ABI " ${CMAKE_ANDROID_ARCH_ABI})

        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} --target=aarch64-linux-android23 -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Dstrtod=avpriv_strtod -DPIC -DZLIB_CONST -O3 -fPIC -Qunused-arguments -MMD -MF" )
        set(TARGET_CPU_ARM true) # general cpu type
        set(TARGET_CPU_AARCH64 true)
        add_compile_definitions(TARGET_OS_ANDROID TARGET_POSIX TARGET_CPU_ARM TARGET_CPU_AARCH64 TARGET_CPU_64BIT)

    else()
        add_compile_definitions(TARGET_CPU_64BIT)
        add_compile_definitions(TARGET_CPU_X86_64)
    endif()

    if(DEFINED TARGET_OS_ANDROID)
        if(MANUAL_PACKAGE)
            message(STATUS "MANUAL_PACKAGE is defined ")
            set(ANDROID_STL c++_static)
            add_compile_definitions(ANDROID_STL=c++_static)
        endif()

        set(CMAKE_ANDROID_ASSETS_DIRECTORIES "${PROJECT_BIN_DIR}/assets")
        message(STATUS "CMAKE_ANDROID_ASSETS_DIRECTORIES " ${CMAKE_ANDROID_ASSETS_DIRECTORIES})

        project(nolimitconnect LANGUAGES C CXX ASM)

    else()
        project(nolimitconnect LANGUAGES C CXX)
    endif()

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

    message(STATUS "project nlc cpu ${NLC_CPU}")
    message(STATUS "project nlc api ${ANDROID_ABI}")

    message(STATUS "project-nlc Qt6_DIR ${Qt6_DIR}")
    message(STATUS "project-nlc Qt6Core_DIR ${Qt6Core_DIR}")
    message(STATUS "project-nlc Qt6Widgets_DIR ${Qt6Widgets_DIR}")
    message(STATUS "project nlc is defined")

    #=== project subdirectories ===#
    message(STATUS "project nlc adding dependant libs with prefix ${CMAKE_PREFIX_PATH}")

    add_subdirectory(nolimitlib)

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
message(STATUS "Root CMakeLists.txt Done")
