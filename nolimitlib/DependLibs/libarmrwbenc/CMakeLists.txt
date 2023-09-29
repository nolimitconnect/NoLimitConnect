
cmake_minimum_required (VERSION 3.22.1)

project(armrwbenc, LANGUAGES C CXX)

set(armrwbenc_SOURCES)

list(APPEND armrwbenc_SOURCES 
    SoftAMRWBEncoder.cpp
    common/cmnMemory.c
    src/autocorr.c
    src/az_isp.c
    src/bits.c
    src/c2t64fx.c
    src/c4t64fx.c
    src/convolve.c
    src/cor_h_x.c
    src/decim54.c
    src/deemph.c
    src/dtx.c
    src/g_pitch.c
    src/gpclip.c
    src/homing.c
    src/hp400.c
    src/hp50.c
    src/hp6k.c
    src/hp_wsp.c
    src/int_lpc.c
    src/isp_az.c
    src/isp_isf.c
    src/lag_wind.c
    src/levinson.c
    src/log2.c
    src/lp_dec2.c
    src/math_op.c
    src/mem_align.c
    src/oper_32b.c
    src/p_med_ol.c
    src/pit_shrp.c
    src/pitch_f4.c
    src/pred_lt4.c
    src/preemph.c
    src/q_gain2.c
    src/q_pulse.c
    src/qisf_ns.c
    src/qpisf_2s.c
    src/random.c
    src/residu.c
    src/scale.c
    src/stream.c
    src/syn_filt.c
    src/updt_tar.c
    src/util.c
    src/voAMRWBEnc.c
    src/voicefac.c
    src/wb_vad.c
    src/weight_a.c
    wavreader.c
    wrapper.c
)

add_library(armrwbenc STATIC ${armrwbenc_SOURCES})

target_include_directories(armrwbenc
  PRIVATE
    inc
    common/include
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET armrwbenc
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
