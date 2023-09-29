cmake_minimum_required(VERSION 3.16.3)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(assemblelib_SOURCES)

list(APPEND assemblelib_SOURCES AssemblyLib.c AssemblyLibCpp.cpp AssemblyLib.h)

if(DEFINED TARGET_OS_ANDROID)
    list(APPEND assemblelib_SOURCES LittleSAssemblyAarch64.s BigSAssemblyAarch64.S)
else()
    list(APPEND assemblelib_SOURCES assemblyNasm64.asm)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_library(Assembly1234TestLib STATIC ${assemblelib_SOURCES})

message(STATUS "CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}")

target_include_directories(Assembly1234TestLib
     PUBLIC
     ${CMAKE_CURRENT_LIST_DIR}
     ${CMAKE_CURRENT_LIST_DIR}/..
     ${CMAKE_PREFIX_PATH}
)

target_link_libraries(Assembly1234TestLib Qt6::Core)

