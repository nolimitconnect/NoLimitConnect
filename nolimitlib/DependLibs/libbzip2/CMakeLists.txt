
cmake_minimum_required (VERSION 3.22.1)

project(bzip2, LANGUAGES C CXX)

set(bzip2_SOURCES)

list(APPEND bzip2_SOURCES 
    blocksort.c
    bzip2.c
    bzip2recover.c
    bzlib.c
    compress.c
    crctable.c
    decompress.c
    huffman.c
    mk251.c
    randtable.c
    spewG.c
)

add_library(bzip2 STATIC ${bzip2_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET bzip2
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
