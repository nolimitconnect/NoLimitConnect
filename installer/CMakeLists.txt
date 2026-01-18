cmake_minimum_required(VERSION 3.21)

#NOTE: is NOT part of the normal NoLimitConnect Build
# Open repo/installer directory instead  of the repo root directory

include(../cmake/version.cmake)
project(${NLC_NAME}-installer VERSION  ${NLC_VERSION})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(FATAL_ERROR "Debug builds are not allowed. Use Release.")
endif()

message("CMAKE_HOST_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME} CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}")

set(PROJECT_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../)
#set(CMAKE_BINARY_DIR "${PROJECT_SRC_ROOT_DIR}/bin")
set(DEPLOY_DIR "${PROJECT_SRC_ROOT_DIR}/deploy")

file(MAKE_DIRECTORY "${DEPLOY_DIR}")


option(INSTALLER_WINDOWS "Build Windows installer" OFF)
option(INSTALLER_FLATPAK "Build Flatpak" OFF)
option(INSTALLER_ANDROID "Build Android APK" OFF)

set(NEED_COMPILED ON)

if(WIN32 AND INSTALLER_WINDOWS)
    include(nsis/nsis.cmake)
    set(NEED_COMPILED OFF)
endif()

if(UNIX AND INSTALLER_FLATPAK)
    include(flatpak/flatpak.cmake)
    set(NEED_COMPILED OFF)
endif()

if(ANDROID AND INSTALLER_ANDROID)
    include(android/apk.cmake)
    set(NEED_COMPILED OFF)
endif()

if(NEED_COMPILED)
    message("Command line not set so guessing")
    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
            message("Building android apk")
            include(android/apk.cmake)
        else()
            message("Building windows nsis installer")
            include(nsis/nsis.cmake)
        endif()
    else()
        include(flatpak/flatpak.cmake)
    endif()
else()
    message("Compiled from Command line")
endif()




