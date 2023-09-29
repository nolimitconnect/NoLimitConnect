
cmake_minimum_required (VERSION 3.22.1)

project(gsm, LANGUAGES C CXX)

set(gsm_SOURCES)

list(APPEND gsm_SOURCES 
    src/add.c
    src/code.c
    src/debug.c
    src/decode.c
    src/gsm_create.c
    src/gsm_decode.c
    src/gsm_destroy.c
    src/gsm_encode.c
    src/gsm_explode.c
    src/gsm_implode.c
    src/gsm_option.c
    src/gsm_print.c
    src/long_term.c
    src/lpc.c
    src/preprocess.c
    src/rpe.c
    src/short_term.c
    src/table.c
    src/toast.c
    src/toast_alaw.c
    src/toast_audio.c
    src/toast_lin.c
    src/toast_ulaw.c
)

add_library(gsm STATIC ${gsm_SOURCES})

target_include_directories(gsm
  PRIVATE
    inc
    src
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET gsm
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

