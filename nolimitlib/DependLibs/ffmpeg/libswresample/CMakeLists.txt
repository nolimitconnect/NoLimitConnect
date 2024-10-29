
cmake_minimum_required (VERSION 3.22.1)

project(swresample, LANGUAGES C CXX ASM)

if(DEFINED TARGET_OS_ANDROID)
    #android/aarch64 uses clang as assembler
elseif(DEFINED TARGET_OS_WINDOWS)
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_ffmpeg_w64.asm ")
else()
    enable_language(ASM_NASM)
    if(DEFINED FLATPAKBUILD)
        set(CMAKE_ASM_NASM_FLAGS "-fPic")
    else()
        string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_ffmpeg_linux.asm ")
    endif() # if(DEFINED FLATPAKBUILD)
endif()

set(swresample_SOURCES)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND swresample_SOURCES 
        x86/audio_convert_init.c
        x86/rematrix_init.c
        x86/resample_init.c
        x86/w64xmmtest.c
        x86/audio_convert.asm
        x86/rematrix.asm
        x86/resample.asm
    )
endif()

if(DEFINED TARGET_CPU_ARM64)
    list(APPEND swresample_SOURCES 
        aarch64/audio_convert_init.c
        aarch64/neontest.c
        aarch64/resample_init.c
        aarch64/audio_convert_neon.S
        aarch64/resample.S
    )
elseif(DEFINED TARGET_CPU_ARM32)
    list(APPEND swresample_SOURCES
        arm/audio_convert_init.c
        arm/neontest.c
        arm/resample_init.c
        arm/audio_convert_neon.S
        arm/resample.S
    )

endif()

list(APPEND swresample_SOURCES 
    audioconvert.c
    dither.c
    resample.c
    soxr_resample.c
    swresample.c
    log2_tab.c
    noise_shaping_data.c
    options.c
    rematrix.c
    rematrix_template.c
    resample_dsp.c
    resample_template.c
    swresample_frame.c

)

add_library(swresample STATIC ${swresample_SOURCES})

target_compile_definitions(swresample
    PRIVATE
    ${ffmpeg_DEFINES}
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET swresample
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
