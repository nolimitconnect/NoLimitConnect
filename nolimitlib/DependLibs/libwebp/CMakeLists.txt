
cmake_minimum_required (VERSION 3.22.1)

project(webp, LANGUAGES C CXX)

set(webp_SOURCES)

list(APPEND webp_SOURCES 
    src/dsp/argb_mips_dsp_r2.c
    src/dsp/dec_clip_tables.c
    src/dsp/lossless_enc.c
    src/dec/alpha_dec_webp.c
    src/dec/buffer_dec_webp.c
    src/dec/frame_dec_webp.c
    src/dec/idec_dec_webp.c
    src/dec/io_dec_webp.c
    src/dec/quant_dec_webp.c
    src/dec/tree_dec_webp.c
    src/dec/vp8_dec_webp.c
    src/dec/vp8l_dec_webp.c
    src/dec/webp_dec_webp.c
    src/demux/anim_decode_webp.c
    src/demux/demux_webp.c
    src/dsp/alpha_processing_webp.c
    src/dsp/alpha_processing_neon_webp.c
    src/dsp/alpha_processing_sse2_webp.c
    src/dsp/alpha_processing_mips_dsp_r2_webp.c
    src/dsp/alpha_processing_sse41_webp.c
    src/dsp/argb_webp.c
    src/dsp/cost_webp.c
    src/dsp/argb_sse2_webp.c
    src/dsp/cost_mips32_webp.c
    src/dsp/cost_mips_dsp_r2_webp.c
    src/dsp/cost_sse2_webp.c
    src/dsp/cpu_webp.c
    src/dsp/dec_webp.c
    src/dsp/dec_mips32_webp.c
    src/dsp/dec_mips_dsp_r2_webp.c
    src/dsp/dec_msa_webp.c
    src/dsp/dec_neon_webp.c
    src/dsp/dec_sse2_webp.c
    src/dsp/dec_sse41_webp.c
    src/dsp/enc_webp.c
    src/dsp/enc_mips32_webp.c
    src/dsp/enc_avx2_webp.c
    src/dsp/enc_mips_dsp_r2_webp.c
    src/dsp/enc_msa_webp.c
    src/dsp/enc_neon_webp.c
    src/dsp/enc_sse2_webp.c
    src/dsp/enc_sse41_webp.c
    src/dsp/filters_webp.c
    src/dsp/filters_mips_dsp_r2_webp.c
    src/dsp/filters_msa_webp.c
    src/dsp/filters_neon_webp.c
    src/dsp/filters_sse2_webp.c
    src/dsp/lossless_webp.c
    src/dsp/lossless_enc_mips32_webp.c
    src/dsp/lossless_enc_mips_dsp_r2_webp.c
    src/dsp/lossless_enc_msa_webp.c
    src/dsp/lossless_enc_neon_webp.c
    src/dsp/lossless_enc_sse2_webp.c
    src/dsp/lossless_enc_sse41_webp.c
    src/dsp/lossless_mips_dsp_r2_webp.c
    src/dsp/lossless_msa_webp.c
    src/dsp/lossless_neon_webp.c
    src/dsp/lossless_sse2_webp.c
    src/dsp/rescaler_webp.c
    src/dsp/rescaler_mips32_webp.c
    src/dsp/rescaler_mips_dsp_r2_webp.c
    src/dsp/rescaler_msa_webp.c
    src/dsp/rescaler_neon_webp.c
    src/dsp/rescaler_sse2_webp.c
    src/dsp/ssim_webp.c
    src/dsp/ssim_sse2_webp.c
    src/dsp/upsampling_webp.c
    src/dsp/upsampling_msa_webp.c
    src/dsp/upsampling_mips_dsp_r2_webp.c
    src/dsp/upsampling_neon_webp.c
    src/dsp/upsampling_sse2_webp.c
    src/dsp/yuv_webp.c
    src/dsp/yuv_mips_dsp_r2_webp.c
    src/dsp/yuv_mips32_webp.c
    src/dsp/yuv_neon_webp.c
    src/dsp/yuv_sse2_webp.c
    src/enc/alpha_enc_webp.c
    src/enc/analysis_enc_webp.c
    src/enc/backward_references_cost_enc_webp.c
    src/enc/backward_references_enc_webp.c
    src/enc/config_enc_webp.c
    src/enc/cost_enc_webp.c
    src/enc/delta_palettization_enc_webp.c
    src/enc/filter_enc_webp.c
    src/enc/frame_enc_webp.c
    src/enc/histogram_enc_webp.c
    src/enc/iterator_enc_webp.c
    src/enc/near_lossless_enc_webp.c
    src/enc/picture_csp_enc_webp.c
    src/enc/picture_enc_webp.c
    src/enc/picture_psnr_enc_webp.c
    src/enc/picture_rescale_enc_webp.c
    src/enc/picture_tools_enc_webp.c
    src/enc/predictor_enc_webp.c
    src/enc/quant_enc_webp.c
    src/enc/syntax_enc_webp.c
    src/enc/token_enc_webp.c
    src/enc/tree_enc_webp.c
    src/enc/vp8l_enc_webp.c
    src/enc/webp_enc_webp.c
    src/mux/anim_encode_webp.c
    src/mux/muxedit_webp.c
    src/mux/muxinternal_webp.c
    src/mux/muxread_webp.c
    src/utils/bit_reader_utils_webp.c
    src/utils/bit_writer_utils_webp.c
    src/utils/color_cache_utils_webp.c
    src/utils/filters_utils_webp.c
    src/utils/huffman_encode_utils_webp.c
    src/utils/huffman_utils_webp.c
    src/utils/quant_levels_dec_utils_webp.c
    src/utils/quant_levels_utils_webp.c
    src/utils/random_utils_webp.c
    src/utils/rescaler_utils_webp.c
    src/utils/thread_utils_webp.c
    src/utils/utils_webp.c
)

add_library(webp STATIC ${webp_SOURCES})

if(DEFINED TARGET_OS_ANDROID)
    #NOTE $NDK_PATH must be set to build android or will not find cpu-features.h
    target_include_directories(webp
      PRIVATE
        $NDK_PATH
    )
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET webp
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()


