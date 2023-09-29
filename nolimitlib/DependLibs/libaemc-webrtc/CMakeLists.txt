
cmake_minimum_required (VERSION 3.22.1)

project(aecm-webrtc, LANGUAGES C CXX)

set(libaecm-webrtc_SOURCES)

list(APPEND aecm-webrtc_SOURCES 
    aecm_core.c
    AecmWebRtc.cpp
    complex_bit_reverse.c
	complex_fft.c
    cross_correlation.c
    delay_estimator.c
    delay_estimator_wrapper.c
	division_operations.c
	downsample_fast.c
    echo_control_mobile.c
	min_max_operations.c
	randomization_functions.c
    real_fft.c
    ring_buffer.c
    spl_init.c
    spl_sqrt_floor.c
    vector_scaling_operations.c
)

add_library(aecm-webrtc STATIC ${aecm-webrtc_SOURCES})

if(DEFINED TARGET_POSIX)
    target_compile_definitions(aecm-webrtc
        PUBLIC
        WEBRTC_POSIX)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET aecm-webrtc
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
