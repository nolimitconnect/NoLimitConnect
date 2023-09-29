
cmake_minimum_required (VERSION 3.22.1)

project(bz2, LANGUAGES C CXX)

set(bz2lib_SOURCES)

list(APPEND bz2_SOURCES 
    blocksort_bz2.c
    bzlib_bz2.c
    compress_bz2.c
    crctable_bz2.c
    decompress_bz2.c
    huffman_bz2.c
    randtable_bz2.c
)

add_library(bz2 STATIC ${bz2_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET bz2
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
