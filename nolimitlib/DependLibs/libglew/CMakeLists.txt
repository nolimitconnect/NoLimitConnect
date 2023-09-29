
cmake_minimum_required (VERSION 3.22.1)

project(glew, LANGUAGES C CXX)

set(glew_SOURCES)

list(APPEND glew_SOURCES 
    src/glew.c
)

add_library(glew STATIC ${glew_SOURCES})

target_compile_definitions(glew
  PRIVATE
    GLEW_STATIC
    DO_NOT_DEFINE_BOOL
)

target_include_directories(glew
  PRIVATE
    include
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET glew
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

