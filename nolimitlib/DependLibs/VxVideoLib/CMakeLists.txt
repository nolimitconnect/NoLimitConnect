
cmake_minimum_required (VERSION 3.22.1)

project(vxvideolib, LANGUAGES CXX)

set(vxvideolib_SOURCES)

list(APPEND vxvideolib_SOURCES 
    GdvBufferUtil.cpp
    VxRescaleRgb.cpp
    VxVidCap.cpp
    VxVidCapImp.cpp
    VxVideoConvert.cpp
)

add_library(vxvideolib STATIC ${vxvideolib_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET vxvideolib
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

