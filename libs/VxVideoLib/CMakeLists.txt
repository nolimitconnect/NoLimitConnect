
cmake_minimum_required (VERSION 3.22.1)

project(vxvideolib, LANGUAGES CXX)

set(vxvideolib_SOURCES)

list(APPEND vxvideolib_SOURCES
    VxRescaleRgb.cpp
)

add_library(vxvideolib STATIC ${vxvideolib_SOURCES})


