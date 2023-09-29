
cmake_minimum_required (VERSION 3.22.1)

project(cpluff, LANGUAGES C CXX)

set(cpluff_SOURCES)

list(APPEND cpluff_SOURCES 
    context.c
    cpluff.c
    kazlib/hash.c
    kazlib/list_cp.c
#    loader/loader.c
    logging_cpluff.c
    pcontrol.c
    pinfo.c
    ploader.c
    pscan.c
    psymbol.c
    serial.c
    thread_posix.c
    thread_windows.c
    util_cpluff.c
)

add_library(cpluff STATIC ${cpluff_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET cpluff
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
