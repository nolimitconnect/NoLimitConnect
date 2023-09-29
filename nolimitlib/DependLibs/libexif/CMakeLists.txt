
cmake_minimum_required (VERSION 3.22.1)

project(exif, LANGUAGES CXX)

set(exif_SOURCES)

list(APPEND exif_SOURCES 
    ExifParse.cpp
    IptcParse.cpp
    JpegParse.cpp
    libexif.cpp
)

add_library(exif STATIC ${exif_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET exif
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

