
cmake_minimum_required (VERSION 3.22.1)

project(crossguid LANGUAGES C CXX)

set(crossguid_SOURCES)

list(APPEND crossguid_SOURCES 
    guid.cpp
)

add_library(crossguid STATIC ${crossguid_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET crossguid
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
