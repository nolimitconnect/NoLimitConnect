
cmake_minimum_required (VERSION 3.16.3)

project(rtmp, LANGUAGES C CXX)

set(rtmp_SOURCES)

list(APPEND rtmp_SOURCES 
    ACppFile.cpp
)

add_library(rtmp STATIC ${rtmp_SOURCES})
