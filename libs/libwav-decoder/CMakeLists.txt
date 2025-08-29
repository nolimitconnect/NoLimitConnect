
cmake_minimum_required (VERSION 3.22.1)

project(libwav-decoder, LANGUAGES CXX)

set(libwav-decoder_SOURCES)

list(APPEND libwav-decoder_SOURCES 
    wav_decoder.h
    wav_decoder.cpp
    WavMgr.h
    WavMgr.cpp
)

add_library(wav-decoder STATIC ${libwav-decoder_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET wav-decoder
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

