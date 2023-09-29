
cmake_minimum_required (VERSION 3.22.1)

project(miniaudio, LANGUAGES C CXX)

set(miniaudio_SOURCES)

list(APPEND miniaudio_SOURCES 
    miniaudio.c
)

add_library(miniaudio STATIC ${miniaudio_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET miniaudio
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()


