
cmake_minimum_required (VERSION 3.22.1)

project(ogg, LANGUAGES C CXX)

set(ogg_SOURCES)

list(APPEND ogg_SOURCES 
    src/bitwise_ogg.c
    src/framing_ogg.c
)

add_library(ogg STATIC ${ogg_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET ogg
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

