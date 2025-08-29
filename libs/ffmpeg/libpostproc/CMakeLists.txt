
cmake_minimum_required (VERSION 3.22.1)

project(postproc, LANGUAGES C CXX)

set(postproc_SOURCES)

list(APPEND postproc_SOURCES 
  postprocess.c

)

add_library(postproc STATIC ${postproc_SOURCES})

target_include_directories(postproc
  PRIVATE
    ../../libcurl/include
    ../../libcurl/lib
    ../../libplayer-nlc
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET postproc
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
