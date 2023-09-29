
cmake_minimum_required (VERSION 3.22.1)

project(tinyxml, LANGUAGES CXX)

set(tinyxml_SOURCES)

list(APPEND tinyxml_SOURCES 
    tinystr.cpp
    tinyxml.cpp
    tinyxml2.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
)

add_library(tinyxml STATIC ${tinyxml_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET tinyxml
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

