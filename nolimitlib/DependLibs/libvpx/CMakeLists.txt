
cmake_minimum_required (VERSION 3.16.3)

project(vpx, LANGUAGES C CXX ASM)

set(vpx_SOURCES)

if(DEFINED TARGET_CPU_X86_64)
    list(APPEND vpx_SOURCES 

        vp9/common/x86/vp9_idct_intrin_sse2.c
        vp9/common/x86/vp9_highbd_iht4x4_add_sse4.c

        vp8/common/x86/bilinear_filter_sse2.c
        vp8/common/x86/dequantize_mmx.asm
        vp8/common/x86/idct_blk_mmx.c
        vp8/common/x86/idct_blk_sse2.c
        vp8/common/x86/idctllm_mmx.asm
        vp8/common/x86/idctllm_sse2.asm
        vp8/common/x86/iwalsh_sse2.asm
        vp8/common/x86/loopfilter_block_sse2_x86_64.asm
        vp8/common/x86/loopfilter_sse2.asm
        vp8/common/x86/loopfilter_x86.c
        vp8/common/x86/mfqe_sse2.asm
        vp8/common/x86/recon_mmx.asm
        vp8/common/x86/recon_sse2.asm
        vp8/common/x86/subpixel_mmx.asm
        vp8/common/x86/subpixel_sse2.asm
        vp8/common/x86/subpixel_ssse3.asm
        vp8/common/x86/vp8_asm_stubs.c

        vp8/encoder/x86/block_error_sse2.asm
        vp8/encoder/x86/copy_sse2.asm
        vp8/encoder/x86/copy_sse3.asm
        vp8/encoder/x86/dct_sse2.asm
        vp8/encoder/x86/denoising_sse2.c
        vp8/encoder/x86/fwalsh_sse2.asm
        vp8/encoder/x86/quantize_sse4.c
        vp8/encoder/x86/temporal_filter_apply_sse2.asm
        vp8/encoder/x86/vp8_enc_stubs_sse2.c
        vp8/encoder/x86/vp8_quantize_sse2.c
        vp8/encoder/x86/vp8_quantize_ssse3.c

        #highbd files are for if CONFIG_VP9_HIGHBITDEPTH=1
        #vp9/common/x86/vp9_highbd_iht16x16_add_sse4.c
        #vp9/common/x86/vp9_highbd_iht4x4_add_sse4.c
        #vp9/common/x86/vp9_highbd_iht8x8_add_sse4.c

        vp9/common/x86/vp9_idct_intrin_sse2.c
        vp9/common/x86/vp9_mfqe_sse2.asm

        #highbd files are for if CONFIG_VP9_HIGHBITDEPTH=1
        #vp9/encoder/x86/highbd_temporal_filter_avx2.c
        #vp9/encoder/x86/highbd_temporal_filter_sse4.c
        #vp9/encoder/x86/highbd_temporal_filter_ssse3.c

        vp9/encoder/x86/temporal_filter_avx2.c
        vp9/encoder/x86/temporal_filter_sse4.c
        vp9/encoder/x86/temporal_filter_ssse3.c
        vp9/encoder/x86/vp9_dct_intrin_sse2.c
        vp9/encoder/x86/vp9_dct_sse2.asm
        vp9/encoder/x86/vp9_denoiser_sse2.c
        vp9/encoder/x86/vp9_error_avx2.c
        vp9/encoder/x86/vp9_error_sse2.asm
        vp9/encoder/x86/vp9_frame_scale_ssse3.c
        vp9/encoder/x86/vp9_highbd_block_error_intrin_sse2.c
        vp9/encoder/x86/vp9_quantize_avx2.c
        vp9/encoder/x86/vp9_quantize_sse2.c
        vp9/encoder/x86/vp9_quantize_ssse3.c

        vpx_dsp/x86/add_noise_sse2.asm
        vpx_dsp/x86/avg_intrin_avx2.c
        vpx_dsp/x86/avg_intrin_sse2.c
        vpx_dsp/x86/avg_pred_avx2.c
        vpx_dsp/x86/avg_pred_sse2.c
        vpx_dsp/x86/avg_ssse3_x86_64.asm
        vpx_dsp/x86/bitdepth_conversion_avx2.h
        vpx_dsp/x86/bitdepth_conversion_sse2.asm
        vpx_dsp/x86/bitdepth_conversion_sse2.h
        vpx_dsp/x86/convolve_avx2.h
        vpx_dsp/x86/convolve.h
        vpx_dsp/x86/convolve_sse2.h
        vpx_dsp/x86/convolve_ssse3.h
        vpx_dsp/x86/deblock_sse2.asm
        vpx_dsp/x86/fwd_dct32x32_impl_avx2.h
        vpx_dsp/x86/fwd_dct32x32_impl_sse2.h
        vpx_dsp/x86/fwd_txfm_avx2.c
        vpx_dsp/x86/fwd_txfm_impl_sse2.h
        vpx_dsp/x86/fwd_txfm_sse2.c
        vpx_dsp/x86/fwd_txfm_sse2.h
        vpx_dsp/x86/fwd_txfm_ssse3_x86_64.asm

        #highbd files are for if CONFIG_VP9_HIGHBITDEPTH=1
        #vpx_dsp/x86/highbd_convolve_avx2.c
        #vpx_dsp/x86/highbd_idct16x16_add_sse2.c
        #vpx_dsp/x86/highbd_idct16x16_add_sse4.c
        #vpx_dsp/x86/highbd_idct32x32_add_sse2.c
        #vpx_dsp/x86/highbd_idct32x32_add_sse4.c
        #vpx_dsp/x86/highbd_idct4x4_add_sse2.c
        #vpx_dsp/x86/highbd_idct4x4_add_sse4.c
        #vpx_dsp/x86/highbd_idct8x8_add_sse2.c
        #vpx_dsp/x86/highbd_idct8x8_add_sse4.c
        #vpx_dsp/x86/highbd_intrapred_intrin_sse2.c
        #vpx_dsp/x86/highbd_intrapred_intrin_ssse3.c
        #vpx_dsp/x86/highbd_intrapred_sse2.asm
        #vpx_dsp/x86/highbd_inv_txfm_sse2.h
        #vpx_dsp/x86/highbd_inv_txfm_sse4.h
        #vpx_dsp/x86/highbd_loopfilter_sse2.c
        #vpx_dsp/x86/highbd_quantize_intrin_avx2.c
        #vpx_dsp/x86/highbd_quantize_intrin_sse2.c
        #vpx_dsp/x86/highbd_sad4d_avx2.c
        #vpx_dsp/x86/highbd_sad4d_sse2.asm
        #vpx_dsp/x86/highbd_sad_avx2.c
        #vpx_dsp/x86/highbd_sad_sse2.asm
        #vpx_dsp/x86/highbd_subpel_variance_impl_sse2.asm
        #vpx_dsp/x86/highbd_variance_impl_sse2.asm
        #vpx_dsp/x86/highbd_variance_sse2.c

        vpx_dsp/x86/intrapred_sse2.asm
        vpx_dsp/x86/intrapred_ssse3.asm
        vpx_dsp/x86/inv_txfm_avx2.c
        vpx_dsp/x86/inv_txfm_sse2.c
        vpx_dsp/x86/inv_txfm_sse2.h
        vpx_dsp/x86/inv_txfm_ssse3.c
        vpx_dsp/x86/inv_txfm_ssse3.h
        vpx_dsp/x86/inv_wht_sse2.asm
        vpx_dsp/x86/loopfilter_avx2.c
        vpx_dsp/x86/loopfilter_sse2.c
        vpx_dsp/x86/mem_sse2.h
        vpx_dsp/x86/post_proc_sse2.c
        vpx_dsp/x86/quantize_avx2.c
        vpx_dsp/x86/quantize_avx.c
        vpx_dsp/x86/quantize_sse2.c
        vpx_dsp/x86/quantize_sse2.h
        vpx_dsp/x86/quantize_ssse3.c
        vpx_dsp/x86/quantize_ssse3.h

        vpx_dsp/x86/sad4d_avx2.c
        vpx_dsp/x86/sad4d_avx512.c
        vpx_dsp/x86/sad4d_sse2.asm
        vpx_dsp/x86/sad_avx2.c
        vpx_dsp/x86/sad_sse2.asm
        vpx_dsp/x86/sse_avx2.c
        vpx_dsp/x86/sse_sse4.c
        vpx_dsp/x86/ssim_opt_x86_64.asm
        vpx_dsp/x86/subpel_variance_sse2.asm
        vpx_dsp/x86/subtract_avx2.c
        vpx_dsp/x86/subtract_sse2.asm
        vpx_dsp/x86/sum_squares_sse2.c
        vpx_dsp/x86/transpose_sse2.h
        vpx_dsp/x86/txfm_common_sse2.h
        vpx_dsp/x86/variance_avx2.c
        vpx_dsp/x86/variance_sse2.c
        vpx_dsp/x86/vpx_convolve_copy_sse2.asm
        vpx_dsp/x86/vpx_high_subpixel_8t_sse2.asm
        vpx_dsp/x86/vpx_high_subpixel_bilinear_sse2.asm
        vpx_dsp/x86/vpx_subpixel_4t_intrin_sse2.c
        vpx_dsp/x86/vpx_subpixel_8t_intrin_avx2.c
        vpx_dsp/x86/vpx_subpixel_8t_intrin_ssse3.c
        vpx_dsp/x86/vpx_subpixel_8t_sse2.asm
        vpx_dsp/x86/vpx_subpixel_8t_ssse3.asm
        vpx_dsp/x86/vpx_subpixel_bilinear_sse2.asm
        vpx_dsp/x86/vpx_subpixel_bilinear_ssse3.asm


        vpx_ports/emms_mmx.asm
        vpx_ports/emms_mmx.c

    )

    if(DEFINED TARGET_OS_WINDOWS)
        list(APPEND vpx_SOURCES
            vpx_dsp/sse.c
            vpx_config_win_x64.c
            vpx_config_win_x64.asm
        )
    else()
        if(DEFINED TARGET_OS_LINUX)
            list(APPEND vpx_SOURCES
                vpx_dsp/sse.c
                vpx_config_linux_x64.c
                vpx_config_linux_x64.asm
            )
        endif()
    endif()
else()
    list(APPEND vpx_SOURCES 
        vpx_ports/arm_cpudetect.c
        vp8/common/arm/loopfilter_arm.c
        vp9/common/arm/neon/vp9_iht8x8_add_neon.c
        vp9/common/arm/neon/vp9_highbd_iht8x8_add_neon.c
        vp9/common/arm/neon/vp9_iht4x4_add_neon.c
        vp9/common/arm/neon/vp9_highbd_iht4x4_add_neon.c
        vp9/common/arm/neon/vp9_highbd_iht16x16_add_neon.c
        vp9/common/arm/neon/vp9_iht16x16_add_neon.c
        vp8/common/arm/neon/dequantizeb_neon.c
        vp8/common/arm/neon/copymem_neon.c
        vp8/common/arm/neon/iwalsh_neon.c
        vp8/common/arm/neon/idct_blk_neon.c
        vp8/common/arm/neon/sixtappredict_neon.c
        vp8/common/arm/neon/dc_only_idct_add_neon.c
        vp8/common/arm/neon/loopfiltersimplehorizontaledge_neon.c
        vp8/common/arm/neon/loopfiltersimpleverticaledge_neon.c
        vp8/common/arm/neon/vp8_loopfilter_neon.c
        vp8/common/arm/neon/mbloopfilter_neon.c
        vp8/common/arm/neon/dequant_idct_neon.c
        vp8/common/arm/neon/shortidct4x4llm_neon.c
        vp8/common/arm/neon/bilinearpredict_neon.c
        vp8/common/arm/loopfilter_arm.c
        vpx_dsp/arm/vpx_convolve_avg_neon.c
        vpx_dsp/arm/hadamard_neon.c
        vpx_dsp/arm/idct32x32_135_add_neon.c
        vpx_dsp/arm/avg_neon.c
        vpx_dsp/arm/vpx_scaled_convolve8_neon.c
        vpx_dsp/arm/vpx_convolve_neon.c
        vpx_dsp/arm/subtract_neon.c
        vpx_dsp/arm/highbd_avg_neon.c
        vpx_dsp/arm/highbd_avg_pred_neon.c
        vpx_dsp/arm/highbd_subpel_variance_neon.c
        vpx_dsp/arm/deblock_neon.c
        #vpx_dsp/arm/highbd_sad4d_neon.c
        vpx_dsp/arm/avg_pred_neon.c
        vpx_dsp/arm/highbd_idct4x4_add_neon.c
        vpx_dsp/arm/idct8x8_1_add_neon.c
        #vpx_dsp/arm/highbd_sad_neon.c
        vpx_dsp/arm/highbd_vpx_convolve8_neon.c
        vpx_dsp/arm/idct32x32_1_add_neon.c
        vpx_dsp/arm/highbd_vpx_convolve_avg_neon.c
        vpx_dsp/arm/highbd_idct16x16_add_neon.c
        vpx_dsp/arm/sum_squares_neon.c
        #vpx_dsp/arm/sad4d_neon.c
        vpx_dsp/arm/idct16x16_add_neon.c
        vpx_dsp/arm/variance_neon.c
        vpx_dsp/arm/highbd_vpx_convolve_copy_neon.c
        vpx_dsp/arm/highbd_idct32x32_1024_add_neon.c
        vpx_dsp/arm/quantize_neon.c
        vpx_dsp/arm/vpx_convolve8_neon.c
        vpx_dsp/arm/highbd_hadamard_neon.c
        vpx_dsp/arm/highbd_idct32x32_add_neon.c
        vpx_dsp/arm/highbd_idct32x32_135_add_neon.c
        vpx_dsp/arm/fdct32x32_neon.c
        vpx_dsp/arm/fdct4x4_neon.c
        vpx_dsp/arm/vpx_convolve8_neon_asm.c
        vpx_dsp/arm/highbd_intrapred_neon.c
        vpx_dsp/arm/highbd_idct32x32_34_add_neon.c
        vpx_dsp/arm/idct4x4_1_add_neon.c
        vpx_dsp/arm/idct32x32_34_add_neon.c
        vpx_dsp/arm/fdct_partial_neon.c
        vpx_dsp/arm/idct8x8_add_neon.c
        vpx_dsp/arm/highbd_quantize_neon.c
        vpx_dsp/arm/fdct8x8_neon.c
        vpx_dsp/arm/idct16x16_1_add_neon.c
        vpx_dsp/arm/highbd_loopfilter_neon.c
        vpx_dsp/arm/intrapred_neon.c
        vpx_dsp/arm/highbd_vpx_convolve_neon.c
        #vpx_dsp/arm/sad_neon.c
        vpx_dsp/arm/highbd_idct8x8_add_neon.c
        vpx_dsp/arm/loopfilter_neon.c../androidNdk/android/cpufeatures/cpu-features_ndk.c
        vpx_dsp/arm/vpx_convolve_copy_neon.c
        vpx_dsp/arm/highbd_variance_neon.c
        vpx_dsp/arm/subpel_variance_neon.c
        vpx_dsp/arm/idct4x4_add_neon.c
        vpx_dsp/arm/fdct16x16_neon.c
        vpx_dsp/arm/idct32x32_add_neon.c
        vpx_dsp/arm/vpx_convolve8_avg_vert_filter_type2_neon.asm
        vpx_dsp/arm/vpx_convolve8_horiz_filter_type1_neon.asm
        vpx_dsp/arm/vpx_convolve8_vert_filter_type2_neon.asm
        vpx_dsp/arm/vpx_convolve8_horiz_filter_type2_neon.asm
        vpx_dsp/arm/vpx_convolve_avg_neon_asm.asm
        vpx_dsp/arm/loopfilter_4_neon.asm
        vpx_dsp/arm/idct_neon.asm
        vpx_dsp/arm/idct4x4_1_add_neon.asm
        vpx_dsp/arm/idct4x4_add_neon.asm
        vpx_dsp/arm/vpx_convolve8_vert_filter_type1_neon.asm
        vpx_dsp/arm/save_reg_neon.asm
        vpx_dsp/arm/vpx_convolve8_avg_horiz_filter_type1_neon.asm
        vpx_dsp/arm/loopfilter_8_neon.asm
        vpx_dsp/arm/intrapred_neon_asm.asm
        vpx_dsp/arm/loopfilter_16_neon.asm
        vpx_dsp/arm/vpx_convolve8_avg_vert_filter_type1_neon.asm
        vpx_dsp/arm/vpx_convolve_copy_neon_asm.asm
        vpx_dsp/arm/vpx_convolve8_avg_horiz_filter_type2_neon.asm
    )

    if(DEFINED TARGET_OS_LINUX)
        list(APPEND vpx_SOURCES
            vpx_config_linux_arm64.c
            )
    endif()
    
    if(DEFINED TARGET_OS_ANDROID)
        list(APPEND vpx_SOURCES 
            ../androidNdk/android/cpufeatures/cpu-features_ndk.c
            )
        if(DEFINED TARGET_CPU_AARCH64)
            list(APPEND vpx_SOURCES
                vpx_config_android_arm64.c
                )
        else()
            list(APPEND vpx_SOURCES
                vpx_config_android_arm32.c
                )
        endif()

    endif()
endif()

list(APPEND vpx_SOURCES 
    rate_hist.c
    vpx_scale/vpx_scale_rtcd.c
    vpx_scale/generic/vpx_scale.c
    vpx_scale/generic/yv12extend.c
    vpx_scale/generic/yv12config.c
    vpx_scale/generic/gen_scalers.c
    vpx_mem/vpx_mem.c
    vpxenc.c
    #only if have libyuv vpx_util/vpx_write_yuv_frame.c
    vpx_util/vpx_thread.c
    vpx_util/vpx_debug_util.c
    y4menc.c
    vp9/decoder/vp9_decodeframe.c
    vp9/decoder/vp9_decodemv.c
    vp9/decoder/vp9_decoder.c
    vp9/decoder/vp9_job_queue.c
    vp9/decoder/vp9_detokenize.c
    vp9/decoder/vp9_dsubexp.c
    vp9/encoder/vp9_svc_layercontext.c
    vp9/encoder/vp9_treewriter.c
    vp9/encoder/vp9_ratectrl.c
    vp9/encoder/vp9_subexp.c
    vp9/encoder/vp9_rd.c
    vp9/encoder/vp9_mbgraph.c
    vp9/encoder/vp9_cost.c
    vp9/encoder/vp9_ethread.c
    vp9/encoder/vp9_firstpass.c
    vp9/encoder/vp9_aq_variance.c
    vp9/encoder/vp9_frame_scale.c
    vp9/encoder/vp9_tokenize.c
    # vp9/encoder/vp9_denoiser.c
    vp9/encoder/vp9_skin_detection.c
    vp9/encoder/vp9_mcomp.c
    vp9/encoder/vp9_encodeframe.c
    vp9/encoder/vp9_aq_360.c
    vp9/encoder/vp9_encodemv.c
    vp9/encoder/vp9_blockiness.c
    vp9/encoder/vp9_multi_thread.c
    vp9/encoder/vp9_encodemb.c
    vp9/encoder/vp9_encoder.c
    vp9/encoder/vp9_quantize.c
    vp9/encoder/vp9_segmentation.c
    vp9/encoder/vp9_aq_complexity.c
    vp9/encoder/vp9_resize.c
    vp9/encoder/vp9_tpl_model.c
    vp9/encoder/vp9_dct.c
    vp9/encoder/vp9_alt_ref_aq.c
    vp9/encoder/vp9_context_tree.c
    vp9/encoder/vp9_picklpf.c
    vp9/encoder/vp9_rdopt.c
    vp9/encoder/vp9_extend.c
    vp9/encoder/vp9_non_greedy_mv.c
    vp9/encoder/vp9_pickmode.c
    vp9/encoder/vp9_ext_ratectrl.c
    vp9/encoder/vp9_lookahead.c
    vp9/encoder/vp9_temporal_filter.c
    vp9/encoder/vp9_aq_cyclicrefresh.c
    vp9/encoder/vp9_noise_estimate.c
    vp9/encoder/vp9_speed_features.c
    vp9/encoder/vp9_bitstream.c
    vp9/vp9_cx_iface.c
    vp9/vp9_dx_iface.c
    vp9/vp9_iface_common.c
    vp9/common/vp9_pred_common.c
    vp9/common/vp9_postproc.c
    vp9/common/vp9_loopfilter.c
    vp9/common/vp9_entropymode.c
    vp9/common/vp9_filter.c
    vp9/common/vp9_scale.c
    vp9/common/vp9_seg_common.c
    vp9/common/vp9_entropymv.c
    vp9/common/vp9_reconintra.c
    vp9/common/vp9_quant_common.c
    vp9/common/vp9_mvref_common.c
    vp9/common/vp9_scan.c
    vp9/common/vp9_common_data.c
    vp9/common/vp9_thread_common.c
    vp9/common/vp9_blockd.c
    vp9/common/vp9_tile_common.c
    vp9/common/vp9_alloccommon.c
    vp9/common/vp9_reconinter.c
    vp9/common/vp9_rtcd.c
    vp9/common/vp9_frame_buffers.c
    vp9/common/vp9_idct.c
    #vp9/common/vp9_mfqe.c
    vp9/common/vp9_debugmodes.c
    vp9/common/vp9_entropy.c
    video_writer.c
    vpx/src/vpx_codec.c
    vpx/src/vpx_decoder.c
    vpx/src/vpx_image.c
    vpx/src/vpx_encoder.c
    warnings.c
    #tools_common.c // not used in new vpx
    #vp8/decoder/error_concealment.c
    vp8/decoder/threading.c
    vp8/decoder/decodeframe.c
    vp8/decoder/dboolhuff.c
    vp8/decoder/decodemv.c
    vp8/decoder/detokenize.c
    vp8/decoder/onyxd_if.c
    vp8/vp8_dx_iface.c
    vp8/encoder/onyx_if.c
    vp8/encoder/treewriter.c
    vp8/encoder/copy_c.c
    #vp8/encoder/mr_dissim.c
    vp8/encoder/lookahead.c
    vp8/encoder/denoising.c
    vp8/encoder/segmentation.c
    vp8/encoder/encodeframe.c
    vp8/encoder/vp8_quantize.c
    vp8/encoder/pickinter.c
    vp8/encoder/boolhuff.c
    vp8/encoder/encodemb.c
    vp8/encoder/dct.c
    vp8/encoder/rdopt.c
    vp8/encoder/encodeintra.c
    vp8/encoder/bitstream.c
    vp8/encoder/firstpass.c
    vp8/encoder/modecosts.c
    vp8/encoder/ethreading.c
    vp8/encoder/temporal_filter.c
    vp8/encoder/mcomp.c
    vp8/encoder/encodemv.c
    vp8/encoder/picklpf.c
    vp8/encoder/tokenize.c
    vp8/encoder/ratectrl.c
    vp8/vp8_cx_iface.c
    vp8/common/findnearmv.c
    vp8/common/reconintra.c
    vp8/common/loopfilter_filters.c
    vp8/common/swapyv12buffer.c
    vp8/common/context.c
    vp8/common/postproc.c
    vp8/common/blockd.c
    vp8/common/vp8_loopfilter.c
    vp8/common/mbpitch.c
    vp8/common/rtcd.c
    vp8/common/setupintrarecon.c
    vp8/common/extend.c
    vp8/common/entropymode.c
    vp8/common/alloccommon.c
    vp8/common/entropy.c
    vp8/common/reconintra4x4.c
    vp8/common/idct_blk.c
    vp8/common/arm/loopfilter_arm.c
    vp8/common/idctllm.c
    vp8/common/debugmodes.c
    vp8/common/filter.c
    vp8/common/mfqe.c
    vp8/common/reconinter.c
    vp8/common/treecoder.c
    vp8/common/modecont.c
    vp8/common/entropymv.c
    vp8/common/generic/systemdependent.c
    vp8/common/vp8_skin_detection.c
    vp8/common/dequantize.c
    vp8/common/quant_common.c
    #tools/tiny_ssim.c not used in new vpx
    y4minput.c
    vpxstats.c
    md5_utils.c
    args.c
    vpxdec.c
    video_reader.c
    #vpx_config.c
    ivfdec.c
    ivfenc.c
    vpx_dsp/fwd_txfm.c
    vpx_dsp/add_noise.c
    vpx_dsp/bitwriter_buffer.c
    vpx_dsp/vpx_convolve.c
    vpx_dsp/intrapred.c
    vpx_dsp/psnrhvs.c
    vpx_dsp/variance.c
    vpx_dsp/loopfilter.c
    vpx_dsp/bitreader.c
    vpx_dsp/inv_txfm.c
    vpx_dsp/ssim.c
    vpx_dsp/psnr.c
    vpx_dsp/skin_detection.c
    vpx_dsp/deblock.c
    vpx_dsp/fastssim.c
    vpx_dsp/bitwriter.c
    vpx_dsp/prob.c
    vpx_dsp/sad.c
    vpx_dsp/sum_squares.c
    vpx_dsp/bitreader_buffer.c
    vpx_dsp/quantize.c

    vpx_dsp/subtract.c
    vpx_dsp/avg.c
)

if(DEFINED TARGET_OS_LINUX)
    if(DEFINED TARGET_CPU_X86_64)
        list(APPEND vpx_SOURCES
            vpx_dsp/vpx_dsp_rtcd.c
        )
    endif()
endif()

if(DEFINED TARGET_OS_WINDOWS)
    if(DEFINED TARGET_CPU_X86_64)
        list(APPEND vpx_SOURCES
            vpx_dsp/vpx_dsp_rtcd.c
        )
    endif()
endif()

add_library(vpx STATIC ${vpx_SOURCES})

if(DEFINED TARGET_CPU_X86_64)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mmmx -msse2 -msse3 -msse4.1 -mavx -mavx2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mmmx -msse2 -msse3 -msse4.1 -mavx -mavx2")
endif()

target_include_directories(vpx
  PRIVATE
    vpx
    vpx_dsp
)

if(DEFINED TARGET_OS_ANDROID)
    target_include_directories(vpx
      PRIVATE
        ../androidNdk/android/cpufeatures
    )
endif()
