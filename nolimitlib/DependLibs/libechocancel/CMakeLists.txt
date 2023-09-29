
cmake_minimum_required (VERSION 3.22.1)

project(echocancel, LANGUAGES C CXX)

set(echocancel_SOURCES)

list(APPEND echocancel_SOURCES 
    EchoCancel.cpp
    EchoUtil.cpp
    aec_core.cpp
    aec_core_mips.cpp
    aec_core_neon.cpp
    aec_core_sse2.cpp
    aec_logging_file_handling.cpp
    aec_rdft.cpp
    aec_rdft_mips.cpp
    aec_rdft_neon.cpp
    aec_rdft_sse2.cpp
    aec_resampler.cpp
    atomic32_posix.cpp
    atomic32_win.cpp
    audio_device_buffer.cpp
    audio_device_generic.cpp
    audio_util.cpp
    base64.cpp
    block_mean_calculator.cpp
    checks.cpp
    common_types.cpp
    cpu_features.cpp
    cpu_features_android.cpp
    cpu_features_linux.cpp
    criticalsection.cpp
    delay_estimator.cpp
    delay_estimator_wrapper.cpp
    echo_cancellation.cpp
    event.cpp
    event_timer_posix.cpp
    event_timer_win.cpp
    event_wrapper.cpp
    fileutils.cpp
    fine_audio_buffer.cpp
    logging.cpp
    pathutils.cpp
    platform_file.cpp
    platform_thread.cpp
    random.cpp
    randomization_functions.cpp
    ring_buffer.cpp
    stringencode.cpp
    stringutils.cpp
    thread_checker_impl.cpp
    tick_util.cpp
    timeutils.cpp
    urlencode.cpp
    wav_file.cpp
    wav_header.cpp
    win32.cpp
    win32filesystem.cpp
)

add_library(echocancel STATIC ${echocancel_SOURCES})

target_include_directories(echocancel
  PRIVATE
    ../openssl-1.0.2p
    ../openssl-1.0.2p/include
    ../../kodi_src/xbmc/xbmc
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET echocancel
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
