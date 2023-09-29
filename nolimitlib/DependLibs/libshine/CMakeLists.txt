
cmake_minimum_required (VERSION 3.22.1)

project(shine, LANGUAGES C CXX)

set(shine_SOURCES)

list(APPEND shine_SOURCES 
    bitstream.c
    huffman.c
    l3bitstream.c
    l3loop.c
    l3mdct.c
    l3subband.c
    layer3.c
    reservoir.c
    tables.c
)

add_library(shine STATIC ${shine_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET shine
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
