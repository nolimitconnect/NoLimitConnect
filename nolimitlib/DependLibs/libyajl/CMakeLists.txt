
cmake_minimum_required (VERSION 3.22.1)

project(yajl, LANGUAGES C CXX)

set(yajl_SOURCES)

list(APPEND yajl_SOURCES 
    yajl.c
    yajl_alloc.c
    yajl_buf.c
    yajl_encode.c
    yajl_gen.c
    yajl_lex.c
    yajl_parser.c
    yajl_tree.c
    yajl_version.c
)

add_library(yajl STATIC ${yajl_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET yajl
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
