
cmake_minimum_required (VERSION 3.22.1)

project(snappy, LANGUAGES C CXX)

set(snappy_SOURCES)

list(APPEND snappy_SOURCES 
    snappy-c.cc
    snappy-sinksource.cc
    snappy-stubs-internal.cc
    snappy.cc
)

add_library(snappy STATIC ${snappy_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET snappy
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
