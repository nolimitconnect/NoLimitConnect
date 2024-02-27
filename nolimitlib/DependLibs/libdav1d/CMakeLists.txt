
cmake_minimum_required (VERSION 3.22.1)

if (DEFINED TARGET_OS_ANDROID)
    project(dav1d, LANGUAGES C CXX ASM)
else()
    project(dav1d, LANGUAGES C CXX)
endif()

if(DEFINED TARGET_OS_ANDROID)
    #android/aarch64 uses clang as assembler
elseif(DEFINED TARGET_OS_WINDOWS)
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_x86.asm")
else()
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_x86.asm -Fdwarf -DPIC -DARCH_X86_64=1")
endif()

set(dav1d_SOURCES)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND dav1d_SOURCES 
        src/x86/cpu.c
        #src/x86/cdef16_avx2.asm
        #src/x86/cdef16_avx512.asm
        #src/x86/cdef16_sse.asm
        src/x86/cdef_avx2.asm
        src/x86/cdef_sse.asm
        src/x86/cpuid.asm
        #src/x86/filmgrain16_avx2.asm
        #src/x86/filmgrain16_avx512.asm
        #src/x86/filmgrain16_sse.asm
        src/x86/filmgrain_avx2.asm
        src/x86/filmgrain_avx512.asm #included by other .asm src/x86/filmgrain_common.asm
        src/x86/filmgrain_sse.asm
        #src/x86/ipred16_avx2.asm
        src/x86/ipred_avx2.asm
        src/x86/ipred_avx512.asm
        src/x86/ipred_sse.asm
        src/x86/itx16_avx2.asm
        src/x86/itx16_avx512.asm
        src/x86/itx16_sse.asm
        src/x86/itx_avx2.asm
        src/x86/itx_avx512.asm
        src/x86/itx_sse.asm
        #src/x86/loopfilter16_avx2.asm
        #src/x86/loopfilter16_avx512.asm
        #src/x86/loopfilter16_sse.asm
        src/x86/loopfilter_avx2.asm
        src/x86/loopfilter_avx512.asm
        src/x86/loopfilter_sse.asm
        #src/x86/looprestoration16_avx2.asm
        #src/x86/looprestoration16_avx512.asm
        #src/x86/looprestoration16_sse.asm
        src/x86/looprestoration_avx2.asm
        src/x86/looprestoration_avx512.asm
        src/x86/looprestoration_sse.asm
        #src/x86/mc16_avx2.asm
        #src/x86/mc16_avx512.asm
        #src/x86/mc16_sse.asm
        src/x86/mc_avx2.asm
        src/x86/mc_avx512.asm
        src/x86/mc_sse.asm
        src/x86/msac.asm
        src/x86/refmvs.asm
        )
elseif(DEFINED TARGET_CPU_AARCH64)
    list(APPEND dav1d_SOURCES 
        #included by others src/arm/asm.S
        src/arm/cpu.c
        src/arm/64/cdef16.S
        src/arm/64/cdef.S
        src/arm/64/cdef_tmpl.S
        src/arm/64/filmgrain16.S
        src/arm/64/filmgrain.S
        src/arm/64/ipred16.S
        src/arm/64/ipred.S
        src/arm/64/itx16.S
        src/arm/64/itx.S
        src/arm/64/loopfilter16.S
        src/arm/64/loopfilter.S
        src/arm/64/looprestoration16.S
        src/arm/64/looprestoration_common.S
        src/arm/64/looprestoration.S
        src/arm/64/looprestoration_tmpl.S
        src/arm/64/mc16.S
        src/arm/64/mc.S
        src/arm/64/msac.S
        src/arm/64/refmvs.S
        src/arm/64/util.S
     )
elseif(DEFINED TARGET_CPU_ARM)
    list(APPEND dav1d_SOURCES
         #included by others src/arm/asm.S
        src/arm/cpu.c
        src/arm/32/cdef16.S
        src/arm/32/cdef.S
        src/arm/32/cdef_tmpl.S
        src/arm/32/filmgrain16.S
        src/arm/32/filmgrain.S
        src/arm/32/ipred16.S
        src/arm/32/ipred.S
        src/arm/32/itx16.S
        src/arm/32/itx.S
        src/arm/32/loopfilter16.S
        src/arm/32/loopfilter.S
        src/arm/32/looprestoration16.S
        src/arm/32/looprestoration_common.S
        src/arm/32/looprestoration.S
        src/arm/32/looprestoration_tmpl.S
        src/arm/32/mc16.S
        src/arm/32/mc.S
        src/arm/32/msac.S
        src/arm/32/refmvs.S
        src/arm/32/util.S
    )
endif()

list(APPEND dav1d_SOURCES
    src/bitdepth_8_part1.c
    src/bitdepth_8_part2.c
    src/bitdepth_16_part1.c
    src/bitdepth_16_part2.c
    src/cdef_apply_tmpl.c
    src/cdef_tmpl.c
    src/cdf.c
    src/cpu_dav1d.c
    src/data.c
    src/decode.c
    src/dequant_tables.c
    src/fg_apply_tmpl.c
    src/filmgrain_tmpl.c
    src/getbits.c
    src/intra_edge.c
    src/ipred_prepare_tmpl.c
    src/ipred_tmpl.c
    src/itx_1d.c
    src/itx_tmpl.c
    src/lf_apply_tmpl.c
    src/lf_mask.c
    src/lib.c
    src/log.c
    src/loopfilter_tmpl.c
    src/looprestoration_tmpl.c
    src/lr_apply_tmpl.c
    src/mc_tmpl.c
    src/mem.c
    src/msac_dav1d.c
    src/obu.c
    src/picture.c
    src/qm.c
    src/recon_tmpl.c
    src/ref.c
    src/refmvs_dav1d.c
    src/scan.c
    src/tables.c
    src/thread_task.c
    src/warpmv.c
    src/wedge.c
)

set(dav1d_HEADERS)

list(APPEND dav1d_HEADERS
    config_dav1d.h
    config_dav1d_arm_internal.h
    include/vcs_version.h
    include/common/attributes.h
    include/common/bitdepth.h
    include/common/dump.h
    include/common/frame.h
    include/common/intops.h
    include/common/validate.h
    include/compat/getopt.h
    include/compat/gcc/stdatomic.h
    include/compat/msvc/stdatomic.h
    include/dav1d/common.h
    include/dav1d/data.h
    include/dav1d/dav1d.h
    include/dav1d/headers.h
    include/dav1d/picture.h
    include/dav1d/version_dav1d.h
)

add_library(dav1d STATIC ${dav1d_SOURCES} ${dav1d_HEADERS})

target_include_directories(dav1d
  PRIVATE
    src
    include
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET dav1d
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
