
cmake_minimum_required (VERSION 3.22.1)

if (DEFINED TARGET_OS_ANDROID)
    project(swscale, LANGUAGES C CXX ASM)
else()
    project(swscale, LANGUAGES C CXX)
endif()

if(DEFINED TARGET_OS_ANDROID)
    #android/aarch64 uses clang as assembler
elseif(DEFINED TARGET_OS_WINDOWS)
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_ffmpeg_w64.asm ")
else()
    enable_language(ASM_NASM)
    string(APPEND CMAKE_ASM_NASM_FLAGS "-Pconfig_ffmpeg_linux.asm ")
endif()

set(swscale_SOURCES)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND swscale_SOURCES 
       x86/hscale_fast_bilinear_simd.c
       x86/rgb2rgb.c
       x86/swscale.c
       x86/yuv2rgb.c
       x86/input.asm
       x86/output.asm
       x86/rgb_2_rgb.asm
       x86/scale.asm
       x86/scale_avx2.asm
       x86/yuv_2_rgb.asm
       x86/yuv2yuvX.asm
    )
endif()

if(DEFINED TARGET_CPU_ARM)
    list(APPEND swscale_SOURCES 
      aarch64/rgb2rgb.c
       aarch64/swscale.c
       aarch64/swscale_unscaled.c
       aarch64/hscale.S
       aarch64/output.S
       aarch64/rgb2rgb_neon.S
       aarch64/yuv2rgb_neon.S
    )
endif()

list(APPEND swscale_SOURCES 
   alphablend.c
   bayer_template.c
   gamma.c
   hscale.c
   hscale_fast_bilinear.c
   input.c
   log2_tab.c
   options.c
   output.c
   rgb2rgb_swscale.c
   rgb2rgb_template_swscale.c
   slice.c
   swscale_swscale.c
   swscale_unscaled_swscale.c
   utils.c
   vscale.c
   yuv2rgb_swscale.c

)

add_library(swscale STATIC ${swscale_SOURCES})

target_include_directories(swscale
  PRIVATE
    ../../libcurl/include
    ../../libcurl/lib
    ../../libplayer-nlc
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET swscale
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
