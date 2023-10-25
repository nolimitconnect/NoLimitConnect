
cmake_minimum_required (VERSION 3.22.1)

project(miniupnpc, LANGUAGES C)

set (MINIUPNPC_VERSION 2.0)
set (MINIUPNPC_API_VERSION 16)

option (UPNPC_BUILD_STATIC "Build static library" TRUE)

set(miniupnpc_SOURCES)

list(APPEND miniupnpc_SOURCES
    addr_is_reserved.c
    igd_desc_parse.c
    miniupnpc.c
    minixml.c
    minisoap.c
    minissdpc.c
    miniwget.c
    upnpc.c
    upnpcommands.c
    upnpdev.c
    upnpreplyparse.c
    upnperrors.c
    connecthostport.c
    portlistingparse.c
    receivedata.c
)

add_library(miniupnpc STATIC ${miniupnpc_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET miniupnpc
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
