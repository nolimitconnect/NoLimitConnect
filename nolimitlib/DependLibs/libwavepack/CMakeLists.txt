
cmake_minimum_required (VERSION 3.22.1)

project(wavepack, LANGUAGES C CXX)

set(wavepack_SOURCES)

list(APPEND wavepack_SOURCES 
    src/pack_wavepack.c
    src/common_utils_wavepack.c
    src/decorr_utils_wavepack.c
    src/entropy_utils_wavepack.c
    src/extra1_wavepack.c
    src/extra2_wavepack.c
    src/open_filename_wavepack.c
    src/open_legacy_wavepack.c
    src/open_raw_wavepack.c
    src/open_utils_wavepack.c
    src/pack_dns_wavepack.c
    src/pack_dsd_wavepack.c
    src/pack_floats_wavepack.c
    src/pack_utils_wavepack.c
    src/read_words_wavepack.c
    src/tag_utils_wavepack.c
    src/tags_wavepack.c
    src/unpack3_open_wavepack.c
    src/unpack_dsd_wavepack.c
    src/unpack3_seek_wavepack.c
    src/unpack3_wavepack.c
    src/unpack_floats_wavepack.c
    src/unpack_seek_wavepack.c
    src/unpack_utils_wavepack.c
    src/unpack_wavepack_wavepack.c
    src/write_words_wavepack.c
)

add_library(wavepack STATIC ${wavepack_SOURCES})

target_compile_definitions(wavepack
    PRIVATE
    ENABLE_DSD
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET wavepack
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

