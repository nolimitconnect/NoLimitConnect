
cmake_minimum_required (VERSION 3.16.3)

project(p8-platform, LANGUAGES C CXX)

set(p8-platform_SOURCES)

list(APPEND p8-platform_SOURCES 
    src/utils/StringUtils.cpp
    src/windows/os-threads.cpp
)

add_library(p8-platform STATIC ${p8-platform_SOURCES})
