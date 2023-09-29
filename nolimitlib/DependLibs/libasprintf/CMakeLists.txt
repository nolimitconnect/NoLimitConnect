
cmake_minimum_required (VERSION 3.22.1)

project(asprintf, LANGUAGES C CXX)

set(asprintf_SOURCES)

list(APPEND asprintf_SOURCES 
    alloca.c
    asnprintf.c
    asprintf.c
    autosprintf.cc
    lib-asprintf.c
    printf-args.c
    printf-parse.c
    vasnprintf.c
    vasprintf.c
    xsize_asprintf.c
)

add_library(asprintf STATIC ${asprintf_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET asprintf
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
