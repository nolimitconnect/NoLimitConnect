
cmake_minimum_required (VERSION 3.22.1)

project(openjpeg, LANGUAGES C CXX)

set(openjpeg_SOURCES)

list(APPEND openjpeg_SOURCES 
    src/lib/openjp2/bench_dwt.c
    src/lib/openjp2/bio.c
    src/lib/openjp2/cidx_manager.c
    src/lib/openjp2/cio.c
    src/lib/openjp2/dwt.c
    src/lib/openjp2/event.c
    src/lib/openjp2/function_list.c
    src/lib/openjp2/image.c
    src/lib/openjp2/invert.c
    src/lib/openjp2/j2k.c
    src/lib/openjp2/jp2.c
    src/lib/openjp2/mct.c
    src/lib/openjp2/mqc.c
    src/lib/openjp2/openjpeg.c
    src/lib/openjp2/opj_clock.c
    src/lib/openjp2/opj_malloc.c
    src/lib/openjp2/phix_manager.c
    src/lib/openjp2/pi.c
    src/lib/openjp2/ppix_manager.c
    src/lib/openjp2/t1.c
#    src/lib/openjp2/t1_generate_luts.c
    src/lib/openjp2/t2.c
    src/lib/openjp2/tcd.c
    src/lib/openjp2/tgt.c
    src/lib/openjp2/thix_manager.c
    src/lib/openjp2/thread.c
    src/lib/openjp2/tpix_manager.c
    thirdparty/liblcms2/src/cmsalpha.c
    thirdparty/liblcms2/src/cmscam02.c
    thirdparty/liblcms2/src/cmscgats.c
    thirdparty/liblcms2/src/cmscnvrt.c
    thirdparty/liblcms2/src/cmserr.c
    thirdparty/liblcms2/src/cmsgamma.c
    thirdparty/liblcms2/src/cmsgmt.c
    thirdparty/liblcms2/src/cmshalf.c
    thirdparty/liblcms2/src/cmsintrp.c
    thirdparty/liblcms2/src/cmsio0.c
    thirdparty/liblcms2/src/cmsio1.c
    thirdparty/liblcms2/src/cmslut.c
    thirdparty/liblcms2/src/cmsmd5.c
    thirdparty/liblcms2/src/cmsmtrx.c
    thirdparty/liblcms2/src/cmsnamed.c
    thirdparty/liblcms2/src/cmsopt.c
    thirdparty/liblcms2/src/cmspack.c
    thirdparty/liblcms2/src/cmspcs.c
    thirdparty/liblcms2/src/cmsplugin.c
    thirdparty/liblcms2/src/cmsps2.c
    thirdparty/liblcms2/src/cmssamp.c
    thirdparty/liblcms2/src/cmssm.c
    thirdparty/liblcms2/src/cmstypes.c
    thirdparty/liblcms2/src/cmsvirt.c
    thirdparty/liblcms2/src/cmswtpnt.c
    thirdparty/liblcms2/src/cmsxform.c
)

add_library(openjpeg STATIC ${openjpeg_SOURCES})

target_include_directories(openjpeg
  PRIVATE
    lib
    lib/openjp2
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET openjpeg
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

