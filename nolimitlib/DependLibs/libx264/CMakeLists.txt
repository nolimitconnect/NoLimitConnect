
cmake_minimum_required (VERSION 3.22.1)

if (DEFINED TARGET_OS_ANDROID)
    project(x264, LANGUAGES C CXX ASM)
else()
    project(x264, LANGUAGES C CXX)
endif()

if(DEFINED TARGET_OS_ANDROID)
    #android/aarch64 uses clang as assembler
elseif(DEFINED TARGET_OS_WINDOWS)
    enable_language(ASM_NASM)
#    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_ffmpeg_w64.asm ")
else()
    enable_language(ASM_NASM)
#    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_ffmpeg_linux.asm ")
endif()

set(x264_SOURCES)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND x264_SOURCES 
        common/x86/bitstream-a.asm
        common/x86/cabac-a.asm
        common/x86/const-a.asm
        common/x86/cpu-a.asm
        #common/x86/dct-32.asm
        common/x86/dct-64.asm
        common/x86/dct-a.asm
        common/x86/deblock-a.asm
        common/x86/mc-a.asm
        common/x86/mc-a2.asm
        #common/x86/pixel-32.asm
        common/x86/pixel-a.asm
        common/x86/predict-a.asm
        common/x86/quant-a.asm
        common/x86/sad-a.asm
        common/x86/sad16-a.asm
        common/x86/trellis-64.asm
        # common/x86/x86util.asm // included by others
        # common/x86/x86inc.asm // included by others
    )
endif()


if(DEFINED TARGET_OS_ANDROID)
    list(APPEND x264_SOURCES 
        common/aarch64/asm-offsets.c
        common/aarch64/bitstream-a.S
        common/aarch64/cabac-a.S
        common/aarch64/dct-a.S
        common/aarch64/deblock-a.S
        common/aarch64/mc-a.S
        common/aarch64/mc-c.c
        common/aarch64/pixel-a.S
        common/aarch64/predict-a.S
        common/aarch64/predict-c.c
        common/aarch64/quant-a.S
        # common/aarch64/asm.S // included by others

    )
endif()


list(APPEND x264_SOURCES 
    common/cpu_x264.c
    common/common_x264.c
    common/bitstream_x264.c
    common/cabac_x264.c
    common/dct_x264.c
    common/deblock_x264.c
    common/frame_x264.c
    common/macroblock_x264.c
    common/mc_x264.c
    common/mvpred_x264.c
    common/osdep_x264.c
    common/pixel_x264.c
    common/predict_x264.c
    common/quant_x264.c
    common/rectangle_x264.c
    common/set_x264.c
    common/threadpool_x264.c
    common/vlc_x264.c
    common/win32thread_x264.c
    #   x264.c
    # common/opencl_x264.c
)

add_library(x264 STATIC ${x264_SOURCES})

target_compile_definitions(x264 PUBLIC 
    HIGH_BIT_DEPTH=0
    BIT_DEPTH=8
)

target_include_directories(x264 PRIVATE
    ./
    common
    common/x86
    common/aarch64
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET x264
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()


