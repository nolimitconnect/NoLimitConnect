
cmake_minimum_required (VERSION 3.22.1)

project(freetype, LANGUAGES C CXX)

set(freetype_SOURCES)

list(APPEND freetype_SOURCES 
    src/autofit/autofit_freetype.c
    src/autofit/ft-hb.c
    src/base/ftbase_freetype.c
    src/base/ftbbox_freetype.c
    src/base/ftbitmap_freetype.c
    src/base/ftgasp_freetype.c
    src/base/ftglyph_freetype.c
    src/base/ftinit_freetype.c
    src/base/ftmm_freetype.c
    src/base/ftobjs_freetype.c
    src/base/ftpfr_freetype.c
    src/base/ftstroke_freetype.c
    src/base/ftsynth_freetype.c
    src/base/ftsystem_freetype.c
    src/base/fttype1_freetype.c
    src/base/ftwinfnt_freetype.c
    src/bdf/bdf_freetype.c
    src/cache/ftcache_freetype.c
    src/cache/ftcbasic_freetype.c
    src/cache/ftccache_freetype.c
    src/cache/ftccmap_freetype.c
    src/cache/ftcglyph_freetype.c
    src/cache/ftcimage_freetype.c
    src/cache/ftcmanag_freetype.c
    src/cache/ftcmru_freetype.c
    src/cache/ftcsbits_freetype.c
    src/ftlinux_freetype.c
    src/winfonts/winfnt_freetype.c
    src/type42/type42_freetype.c
    src/type1/type1_freetype.c
    src/truetype/truetype_freetype.c
    src/smooth/smooth_freetype.c
    src/sfnt/sfnt_freetype.c
    src/raster/raster_freetype.c
    src/psnames/psmodule_freetype.c
    src/pshinter/pshinter_freetype.c
    src/psaux/psaux_freetype.c
    src/pfr/pfr_freetype.c
    src/pcf/pcf_freetype.c
    src/lzw/ftlzw_freetype.c
    src/gzip/ftgzip_freetype.c
    src/cid/type1cid_freetype.c
    src/cff/cff_freetype.c
    harfbuzz/src/failing-alloc.c
    harfbuzz/src/harfbuzz.cc
    harfbuzz/src/hb-cairo-utils.cc
    harfbuzz/src/hb-cairo.cc
    harfbuzz/src/hb-gobject-structs.cc
)

add_library(freetype STATIC ${freetype_SOURCES})

target_compile_options( freetype
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Wno-undefined-internal
        -Wno-deprecated-declarations
    >
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET freetype
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

