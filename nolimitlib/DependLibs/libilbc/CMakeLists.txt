
cmake_minimum_required (VERSION 3.22.1)

project(ilbc, LANGUAGES C CXX)

set(ilbc_SOURCES)

list(APPEND ilbc_SOURCES 
    ilbc/cb_mem_energy_augmentation_ilbc.c
    ilbc/cb_search_core_ilbc.c
    ilbc/abs_quant_ilbc.c
    ilbc/abs_quant_loop_ilbc.c
    ilbc/augmented_cb_corr_ilbc.c
    ilbc/bw_expand_ilbc.c
    ilbc/cb_construct_ilbc.c
    ilbc/cb_mem_energy_calc_ilbc.c
    ilbc/cb_mem_energy_ilbc.c
    ilbc/cb_search_ilbc.c
    ilbc/cb_update_best_index_ilbc.c
    ilbc/chebyshev_ilbc.c
    ilbc/comp_corr_ilbc.c
    ilbc/constants_ilbc.c
    ilbc/create_augmented_vec_ilbc.c
    ilbc/decode_ilbc.c
    ilbc/decode_residual_ilbc.c
    ilbc/decoder_interpolate_lsf_ilbc.c
    ilbc/do_plc_ilbc.c
    ilbc/encode_ilbc.c
    ilbc/energy_inverse_ilbc.c
    ilbc/enh_upsample_ilbc.c
    ilbc/enhancer_ilbc.c
    ilbc/enhancer_interface_ilbc.c
    ilbc/filtered_cb_vecs_ilbc.c
    ilbc/frame_classify_ilbc.c
    ilbc/gain_dequant_ilbc.c
    ilbc/gain_quant_ilbc.c
    ilbc/get_cd_vec_ilbc.c
    ilbc/get_lsp_poly_ilbc.c
    ilbc/get_sync_seq_ilbc.c
    ilbc/hp_input_ilbc.c
    ilbc/hp_output_ilbc.c
    ilbc/ilbc_ilbc.c
    ilbc/index_conv_dec_ilbc.c
    ilbc/index_conv_enc_ilbc.c
    ilbc/init_decode_ilbc.c
    ilbc/init_encode_ilbc.c
    ilbc/interpolate_ilbc.c
    ilbc/interpolate_samples_ilbc.c
    ilbc/lpc_encode_ilbc.c
    ilbc/lsf_check_ilbc.c
    ilbc/lsf_interpolate_to_poly_dec_ilbc.c
    ilbc/lsf_interpolate_to_poly_enc_ilbc.c
    ilbc/lsf_to_lsp_ilbc.c
    ilbc/lsf_to_poly_ilbc.c
    ilbc/lsp_to_lsf_ilbc.c
    ilbc/my_corr_ilbc.c
    ilbc/nearest_neighbor_ilbc.c
    ilbc/pack_bits_ilbc.c
    ilbc/poly_to_lsf_ilbc.c
    ilbc/poly_to_lsp_ilbc.c
    ilbc/refiner_ilbc.c
    ilbc/simple_interpolate_lsf_ilbc.c
    ilbc/simple_lpc_analysis_ilbc.c
    ilbc/simple_lsf_dequant_ilbc.c
    ilbc/simple_lsf_quant_ilbc.c
    ilbc/smooth_ilbc.c
    ilbc/smooth_out_data_ilbc.c
    ilbc/sort_sq_ilbc.c
    ilbc/split_vq_ilbc.c
    ilbc/state_construct_ilbc.c
    ilbc/state_search_ilbc.c
    ilbc/swap_bytes_ilbc.c
    ilbc/unpack_bits_ilbc.c
    ilbc/vq3_ilbc.c
    ilbc/vq4_ilbc.c
    ilbc/window32_w32_ilbc.c
    ilbc/xcorr_coef_ilbc.c
)

add_library(ilbc STATIC ${ilbc_SOURCES})

target_include_directories(ilbc
  PRIVATE
    include
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET ilbc
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
