
cmake_minimum_required (VERSION 3.22.1)

project(flac, LANGUAGES C CXX)

set(flac_SOURCES)

list(APPEND flac_SOURCES 
    bitmath.c
    bitreader.c
    bitwriter.c
    cpu.c
    crc.c
    fixed.c
    fixed_intrin_sse2.c
    fixed_intrin_ssse3.c
    float.c
    format.c
    lpc.c
    lpc_intrin_avx2.c
    lpc_intrin_sse.c
    lpc_intrin_sse2.c
    lpc_intrin_sse41.c
    md5.c
    memory.c
    metadata_iterators.c
    metadata_object.c
    ogg_decoder_aspect.c
    ogg_encoder_aspect.c
    ogg_helper.c
    ogg_mapping.c
    stream_decoder.c
    stream_encoder.c
    stream_encoder_framing.c
    stream_encoder_intrin_avx2.c
    stream_encoder_intrin_sse2.c
    stream_encoder_intrin_ssse3.c
    window.c
    windows_unicode_filenames.c
)

add_library(flac STATIC ${flac_SOURCES})

target_include_directories(flac
  PRIVATE
    FLAC
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET flac
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

