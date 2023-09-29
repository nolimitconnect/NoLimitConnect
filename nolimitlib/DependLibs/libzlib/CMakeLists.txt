
cmake_minimum_required (VERSION 3.22.1)

project(zlib, LANGUAGES C CXX)

set(zlib_SOURCES)

list(APPEND zlib_SOURCES 
    compress_zlib.c
    adler32_zlib.c
    crc32_zlib.c
    deflate_zlib.c
    gzclose_zlib.c
    gzlib_zlib.c
    gzread_zlib.c
    gzwrite_zlib.c
    infback_zlib.c
    inffast_zlib.c
    inflate_zlib.c
    inftrees_zlib.c
#    minigzip_zlib.c
    trees_zlib.c
    uncompr_zlib.c
    zutil_zlib.c
)

add_library(zlib STATIC ${zlib_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET zlib
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
