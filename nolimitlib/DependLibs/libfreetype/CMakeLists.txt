
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
    harfbuzz/src/hb-aat-layout.cc
    harfbuzz/src/hb-aat-map.cc
    harfbuzz/src/hb-blob.cc
    harfbuzz/src/hb-buffer-serialize.cc
    harfbuzz/src/hb-buffer-verify.cc
    harfbuzz/src/hb-buffer.cc
    harfbuzz/src/hb-cairo-utils.cc
    harfbuzz/src/hb-cairo.cc
    harfbuzz/src/hb-common.cc
    harfbuzz/src/hb-coretext.cc
    harfbuzz/src/hb-directwrite.cc
    harfbuzz/src/hb-draw.cc
    harfbuzz/src/hb-face-builder.cc
    harfbuzz/src/hb-face.cc
    harfbuzz/src/hb-fallback-shape.cc
    harfbuzz/src/hb-font.cc
    harfbuzz/src/hb-ft.cc
    harfbuzz/src/hb-gdi.cc
    harfbuzz/src/hb-glib.cc
    harfbuzz/src/hb-gobject-structs.cc
    harfbuzz/src/hb-graphite2.cc
    harfbuzz/src/hb-icu.cc
    harfbuzz/src/hb-map.cc
    harfbuzz/src/hb-number.cc
    harfbuzz/src/hb-ot-cff1-table.cc
    harfbuzz/src/hb-ot-cff2-table.cc
    harfbuzz/src/hb-ot-color.cc
    harfbuzz/src/hb-ot-face.cc
    harfbuzz/src/hb-ot-font.cc
    harfbuzz/src/hb-ot-layout.cc
    harfbuzz/src/hb-ot-map.cc
    harfbuzz/src/hb-ot-math.cc
    harfbuzz/src/hb-ot-meta.cc
    harfbuzz/src/hb-ot-metrics.cc
    harfbuzz/src/hb-ot-name.cc
    harfbuzz/src/hb-ot-shape-fallback.cc
    harfbuzz/src/hb-ot-shape-normalize.cc
    harfbuzz/src/hb-ot-shape.cc
    harfbuzz/src/hb-ot-shaper-arabic.cc
    harfbuzz/src/hb-ot-shaper-default.cc
    harfbuzz/src/hb-ot-shaper-hangul.cc
    harfbuzz/src/hb-ot-shaper-hebrew.cc
    harfbuzz/src/hb-ot-shaper-indic-table.cc
    harfbuzz/src/hb-ot-shaper-indic.cc
    harfbuzz/src/hb-ot-shaper-khmer.cc
    harfbuzz/src/hb-ot-shaper-myanmar.cc
    harfbuzz/src/hb-ot-shaper-syllabic.cc
    harfbuzz/src/hb-ot-shaper-thai.cc
    harfbuzz/src/hb-ot-shaper-use.cc
    harfbuzz/src/hb-ot-shaper-vowel-constraints.cc
    harfbuzz/src/hb-ot-tag.cc
    harfbuzz/src/hb-ot-var.cc
    harfbuzz/src/hb-outline.cc
    harfbuzz/src/hb-paint-extents.cc
    harfbuzz/src/hb-paint.cc
    harfbuzz/src/hb-set.cc
    harfbuzz/src/hb-shape-plan.cc
    harfbuzz/src/hb-shape.cc
    harfbuzz/src/hb-shaper.cc
    harfbuzz/src/hb-static.cc
    harfbuzz/src/hb-style.cc
    harfbuzz/src/hb-ucd.cc
    harfbuzz/src/hb-unicode.cc
    harfbuzz/src/hb-uniscribe.cc
    harfbuzz/src/hb-wasm-api.cc
    harfbuzz/src/hb-wasm-shape.cc
)

add_library(freetype STATIC ${freetype_SOURCES})

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

