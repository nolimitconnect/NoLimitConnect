
cmake_minimum_required (VERSION 3.22.1)

project(upnp2lib, LANGUAGES CXX)

set(upnp2_SOURCES)

list(APPEND upnp2_SOURCES 
    UpnpParser.cpp
    UpnpPortForward.cpp
    UpnpRootDevice.cpp
    UpnpService.cpp
    UpnpTcpSkt.cpp
    UpnpAltXmlParser.cpp
)

add_library(upnp2lib STATIC ${upnp2_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET upnp2lib
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

