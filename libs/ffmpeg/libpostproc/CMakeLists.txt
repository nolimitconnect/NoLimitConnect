
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

target_compile_options(postproc
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Wno-deprecated-declarations
        -Wno-pointer-sign
#         -Wno-shift-op-parentheses
#         -Wno-missing-prototypes
#         -Wno-strict-prototypes
#         -Wno-undef
#         -Wno-shadow
#         -Wno-unused-but-set-variable
#         -Wno-switch
#         -Wno-parentheses
#         -Wno-implicit-const-int-float-conversion
#         -Wno-dangling-else
    >
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
