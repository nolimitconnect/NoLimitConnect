
cmake_minimum_required (VERSION 3.22.1)

project(jpegturbo, LANGUAGES C CXX ASM)

if(DEFINED TARGET_OS_ANDROID)
    #android/aarch64 uses clang as assembler
elseif(DEFINED TARGET_OS_WINDOWS)
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_x86.asm")

elseif(DEFINED TARGET_OS_LINUX)  
    if(DEFINED TARGET_CPU_ARM64)
        enable_language(ASM_NASM)
        string(APPEND CMAKE_ASM_NASM_FLAGS "-Fdwarf -DPIC -DARCH_AARCH64=1")
    else()
        enable_language(ASM_NASM)
        string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_x86.asm -Fdwarf -DPIC -DARCH_X86_64=1")
    endif()

else()
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_x86.asm -Fdwarf -DPIC -DARCH_X86_64=1")
endif()

set(jpegturbo_DEFINES)
list(APPEND jpegturbo_DEFINES
	# common to all builds

    RGB_RED=0 
    RGB_GREEN=1
    RGB_BLUE=2 
    RGB_PIXELSIZE=3

    JPEG_NUMCS=17

    EXT_RGB_RED=0
    EXT_RGB_GREEN=1
    EXT_RGB_BLUE=2
    EXT_RGB_PIXELSIZE=3

    EXT_RGBX_RED=0
    EXT_RGBX_GREEN=1
    EXT_RGBX_BLUE=2
    EXT_RGBX_PIXELSIZE=4

    EXT_BGR_RED=2
    EXT_BGR_GREEN=1
    EXT_BGR_BLUE=0
    EXT_BGR_PIXELSIZE=3

    EXT_BGRX_RED=2
    EXT_BGRX_GREEN=1
    EXT_BGRX_BLUE=0
    EXT_BGRX_PIXELSIZE=4

    EXT_XBGR_RED=3
    EXT_XBGR_GREEN=2
    EXT_XBGR_BLUE=1
    EXT_XBGR_PIXELSIZE=4

    EXT_XRGB_RED=1
    EXT_XRGB_GREEN=2
    EXT_XRGB_BLUE=3
    EXT_XRGB_PIXELSIZE=4
)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND jpegturbo_DEFINES
            #it seems the nasm -P does not work in flatpak so add all the defines here for ffmpeg etc.

            )
elseif(DEFINED TARGET_CPU_ARM64)
    list(APPEND jpegturbo_DEFINES
            #it seems the nasm -P does not work in flatpak so add all the defines here for ffmpeg etc.

            )
endif() # if(DEFINED TARGET_CPU_X86_64)

if(DEFINED TARGET_CPU_ARM64)
    list(APPEND jpegturbo_DEFINES
        HAVE_ARMV8_INLINE=1
        #do not know if these are correct
        HAVE_VLD1_S16_X3=1
        HAVE_VLD1_U16_X2=1
        HAVE_VLD1Q_U8_X4=1
    )

    if(DEFINED TARGET_OS_ANDROID)
        list(APPEND jpegturbo_DEFINES
            HAVE_NEON_INLINE=1
            HAVE_VFP_INLINE=1
            HAVE_INTRINSICS_NEON=1
        )

    else()
        list(APPEND jpegturbo_DEFINES
            HAVE_NEON_INLINE=0
            HAVE_VFP_INLINE=0
            HAVE_INTRINSICS_NEON=0
        )

    endif()


elseif(DEFINED TARGET_CPU_ARM32)
    list(APPEND jpegturbo_DEFINES
        HAVE_ARMV8_INLINE=0
        #do not know if these are correct
        HAVE_VLD1_S16_X3=0
        HAVE_VLD1_U16_X2=0
        HAVE_VLD1Q_U8_X4=0
    )

    if(DEFINED TARGET_OS_ANDROID)
        list(APPEND jpegturbo_DEFINES
            HAVE_NEON_INLINE=1
            HAVE_VFP_INLINE=1
            HAVE_INTRINSICS_NEON=1
        )

    else()
        list(APPEND jpegturbo_DEFINES
            HAVE_NEON_INLINE=0
            HAVE_VFP_INLINE=0
            HAVE_INTRINSICS_NEON=0
        )

    endif()

elseif(DEFINED TARGET_CPU_X86_64)
    list(APPEND jpegturbo_DEFINES
        HAVE_ARMV8_INLINE=0
        HAVE_NEON_INLINE=0
        HAVE_VFP_INLINE=0
    )

endif()


if(DEFINED TARGET_POSIX)
    list(APPEND jpegturbo_DEFINES
        HAVE_ARPA_INET_H=1
    )

else()
    list(APPEND jpegturbo_DEFINES
        HAVE_ARPA_INET_H=0
    )

endif()

if(DEFINED TARGET_OS_ANDROID)
    list(APPEND jpegturbo_DEFINES

    )

    if(DEFINED TARGET_CPU_ARM32)
        list(APPEND jpegturbo_DEFINES

        )
    else()
        list(APPEND jpegturbo_DEFINES

        )
    endif()

elseif(DEFINED TARGET_OS_LINUX)
    list(APPEND jpegturbo_DEFINES

    )

elseif(DEFINED TARGET_OS_WINDOWS)
    list(APPEND jpegturbo_DEFINES

    )


endif()

# config_jpegturbo_w64.asm
set(jpegturbo_SOURCES)

list(APPEND jpegturbo_SOURCES 
    src/cderror.h
    src/cdjpeg.h
    #src/cjpeg.c #requires file io
    src/cmyk.h

    #src/djpeg.c #requires file io

    src/jaricom.c
    src/jcapimin.c
    src/jcapistd.c
    src/jcarith.c
    src/jccoefct.c
    #src/jccolext.c #included by others
    src/jccolor.c
    src/jcdctmgr.c
    src/jcdiffct.c
    src/jchuff.c
    src/jchuff.h
    src/jcicc.c
    src/jcinit.c
    src/jclhuff.c
    src/jclossls.c

    src/jcmainct.c
    src/jcmarker.c
    src/jcmaster.h
    src/jcmaster.c
    src/jcomapi.c
    src/jconfig.txt
    src/jcparam.c
    src/jcphuff.c
    src/jcprepct.c

    src/jcsample.c
    src/jctrans.c

    src/jdapimin.c
    src/jdapistd.c
    src/jdarith.c
    src/jdatadst-tj.c
    src/jdatadst.c
    src/jdatasrc-tj.c
    src/jdatasrc.c
    src/jdcoefct.c
    src/jdcoefct.h
    #src/jdcol565.c #included by others
    #src/jdcolext.c #included by others
    src/jdcolor.c
    src/jdct.h
    src/jddctmgr.c
    src/jddiffct.c
    src/jdhuff.c
    src/jdhuff.h
    src/jdicc.c
    src/jdinput.c
    src/jdlhuff.c
    src/jdlossls.c
    src/jdmainct.c
    src/jdmainct.h
    src/jdmarker.c
    src/jdmaster.c
    src/jdmaster.h
    src/jdmerge.c
    src/jdmerge.h
    #src/jdmrg565.c #included by others
    #src/jdmrgext.c #included by others
    src/jdphuff.c
    src/jdpostct.c
    src/jdsample.c
    src/jdsample.h
    src/jdtrans.c
    src/jerror.c
    src/jerror.h
    src/jfdctflt.c
    src/jfdctfst.c
    src/jfdctint.c
    src/jidctflt.c
    src/jidctfst.c
    src/jidctint.c
    src/jidctred.c
    src/jinclude.h
    src/jlossls.h
    src/jmemmgr.c
    src/jmemnobs.c
    src/jmemsys.h
    src/jmorecfg.h
    src/jpegapicomp.h
    src/jpegint.h
    src/jpeglib.h

    src/jpeg_nbits.c
    src/jpeg_nbits.h
    src/jquant1.c
    src/jquant2.c
    src/jsamplecomp.h
    src/jsimd.h
    src/jsimddct.h

    #src/jstdhuff.c #included by others
    src/jutils.c
    src/libjpeg.map.in
    #src/md5
    src/rdbmp.c
    src/rdcolmap.c
    src/rdgif.c
    src/rdjpgcom.c
    src/rdppm.c
    src/rdswitch.c
    src/rdtarga.c

    src/tjutil.c
    src/tjutil.h

    src/transupp.h
    src/transupp.c
    src/turbojpeg-mapfile
    src/turbojpeg-mapfile.jni
    #src/turbojpeg-mp.c #included by others
    src/turbojpeg.c
    src/turbojpeg.h
    src/wrbmp.c
    src/wrgif.c
    src/wrppm.c

    simd/jsimd.h
    #simd/nasm/config_jpegturbo_w64.asm
    simd/nasm/jcolsamp.inc
    simd/nasm/jdct.inc
    simd/nasm/jsimdcfg.inc
    simd/nasm/jsimdcfg.inc.h
    simd/nasm/jsimdext.inc
    VxJpgLib.h
    VxJpg.cpp
)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND jpegturbo_SOURCES
    simd/x86_64/jccolext-avx2.asm
    simd/x86_64/jccolext-sse2.asm
    simd/x86_64/jccolor-avx2.asm
    simd/x86_64/jccolor-sse2.asm
    simd/x86_64/jcgray-avx2.asm
    simd/x86_64/jcgray-sse2.asm
    simd/x86_64/jcgryext-avx2.asm
    simd/x86_64/jcgryext-sse2.asm
    simd/x86_64/jchuff-sse2.asm
    simd/x86_64/jcphuff-sse2.asm
    simd/x86_64/jcsample-avx2.asm
    simd/x86_64/jcsample-sse2.asm
    simd/x86_64/jdcolext-avx2.asm
    simd/x86_64/jdcolext-sse2.asm
    simd/x86_64/jdcolor-avx2.asm
    simd/x86_64/jdcolor-sse2.asm
    simd/x86_64/jdmerge-avx2.asm
    simd/x86_64/jdmerge-sse2.asm
    simd/x86_64/jdmrgext-avx2.asm
    simd/x86_64/jdmrgext-sse2.asm
    simd/x86_64/jdsample-avx2.asm
    simd/x86_64/jdsample-sse2.asm
    simd/x86_64/jfdctflt-sse.asm
    simd/x86_64/jfdctfst-sse2.asm
    simd/x86_64/jfdctint-avx2.asm
    simd/x86_64/jfdctint-sse2.asm
    simd/x86_64/jidctflt-sse2.asm
    simd/x86_64/jidctfst-sse2.asm
    simd/x86_64/jidctint-avx2.asm
    simd/x86_64/jidctint-sse2.asm
    simd/x86_64/jidctred-sse2.asm
    simd/x86_64/jquantf-sse2.asm
    simd/x86_64/jquanti-avx2.asm
    simd/x86_64/jquanti-sse2.asm
    simd/x86_64/jsimd.c
    simd/x86_64/jsimdcpu.asm
    )

endif()

if(DEFINED TARGET_CPU_ARM64 OR DEFINED TARGET_CPU_ARM32)
    list(APPEND jpegturbo_SOURCES 
        simd/arm/align.h
        #simd/arm/jccolor-neon.c
        simd/arm/jcgray-neon.c
        #simd/arm/jcgryext-neon.c #included by others
        simd/arm/jchuff.h
        simd/arm/jcphuff-neon.c
        simd/arm/jcsample-neon.c
        #simd/arm/jdcolext-neon.c #included by others
        #simd/arm/jdcolor-neon.c #included by others
        simd/arm/jdmerge-neon.c
        #simd/arm/jdmrgext-neon.c #included by others
        simd/arm/jdsample-neon.c
        simd/arm/jfdctfst-neon.c
        #simd/arm/jfdctint-neon.c
        simd/arm/jidctfst-neon.c
        #simd/arm/jidctint-neon.c
        simd/arm/jidctred-neon.c
        simd/arm/jquanti-neon.c
        simd/arm/neon-compat.h.in
        simd/arm/neon-compat.h
     )
endif()

if(DEFINED TARGET_CPU_ARM64)
    list(APPEND jpegturbo_SOURCES 
        #simd/arm/aarch64/jccolext-neon.c #included by others
        #simd/arm/aarch64/jchuff-neon.c
        simd/arm/aarch64/jsimd.c
        simd/arm/aarch64/jsimd_neon.S
     )
elseif(DEFINED TARGET_CPU_ARM32)
    list(APPEND jpegturbo_SOURCES
        #simd/arm/aarch32/jccolext-neon.c #included by others
        simd/arm/aarch32/jchuff-neon.c
        simd/arm/aarch32/jsimd.c
        simd/arm/aarch32/jsimd_neon.S
    )
endif()

add_library(jpegturbo STATIC ${jpegturbo_SOURCES})

target_include_directories(jpegturbo 
	PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/simd
    ${CMAKE_CURRENT_SOURCE_DIR}/simd/nasm
    )

if(DEFINED TARGET_CPU_X86_64)
    target_include_directories(jpegturbo
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/simd/x86_64
        )
elseif(DEFINED TARGET_CPU_ARM64)
    target_include_directories(jpegturbo
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/simd/arm
        ${CMAKE_CURRENT_SOURCE_DIR}/simd/arm/aarch64
        )
elseif(DEFINED TARGET_CPU_ARM32)
    target_include_directories(jpegturbo
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/simd/arm
        ${CMAKE_CURRENT_SOURCE_DIR}/simd/arm/aarch32
        )
endif()

target_compile_definitions(jpegturbo
    PRIVATE
    ${jpegturbo_DEFINES}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # turn on optimization even when in debug mode
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET miniupnpc
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()



