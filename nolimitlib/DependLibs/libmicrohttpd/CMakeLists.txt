
cmake_minimum_required (VERSION 3.22.1)

project(microhttpd, LANGUAGES C CXX)

set(microhttpd_SOURCES)

list(APPEND microhttpd_SOURCES 
    base64_microhttpd.c
    basicauth.c
    connection.c
    connection_https.c
    daemon.c
    digestauth.c
    internal_microhttpd.c
    md5.c
    memorypool.c
    mhd_mono_clock.c
    mhd_str.c
    postprocessor.c
    reason_phrase.c
    response.c
    sysfdsetsize.c
    tsearch.c
)

add_library(microhttpd STATIC ${microhttpd_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET microhttpd
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

