
cmake_minimum_required (VERSION 3.22.1)

project(zvbi, LANGUAGES C CXX)

set(zvbi_SOURCES)

list(APPEND zvbi_SOURCES 
   src/export_zvbi.c
   src/search_zvbi.c
   src/bit_slicer_zvbi.c
   src/cache_zvbi.c
   src/caption_zvbi.c
   src/cc608_decoder_zvbi.c
   src/chains_zvbi.c
   src/conv_zvbi.c
   src/decoder_zvbi.c
   src/dvb_demux_zvbi.c
   src/dvb_mux_zvbi.c
   src/event_zvbi.c
   src/exp-gfx_zvbi.c
   src/exp-html_zvbi.c
   src/exp-templ_zvbi.c
   src/exp-txt_zvbi.c
   src/exp-vtx_zvbi.c
   src/hamm_zvbi.c
#   src/hammgen_zvbi.c
   src/idl_demux_zvbi.c
   src/io-bktr_zvbi.c
   src/io-dvb_zvbi.c
   src/io-sim_zvbi.c
   src/io-v4l_zvbi.c
   src/io-v4l2k_zvbi.c
   src/io-v4l2_zvbi.c
   src/io_zvbi_capture_zvbi.c
   src/lang_zvbi.c
   src/misc_zvbi.c
   src/packet-830_zvbi.c
   src/packet_zvbi.c
   src/page_table_zvbi.c
   src/pdc_zvbi.c
   src/pfc_demux_zvbi.c
   src/proxy-client_zvbi.c
   src/proxy-msg_zvbi.c
   src/raw_decoder_zvbi.c
   src/sampling_par_zvbi.c
   src/sliced_filter_zvbi.c
   src/tables_zvbi.c
   src/teletext_zvbi.c
   src/trigger_zvbi.c
   src/ure_zvbi.c
   src/vbi_zvbi.c
   src/vps_zvbi.c
   src/wss_zvbi.c
   src/xds_demux_zvbi.c
   contrib/atsc-cc_zvbi.c
   contrib/dvbsubs_zvbi.c
   contrib/ntsc-cc_zvbi.c
   contrib/x11font_zvbi.c
)

add_library(zvbi STATIC ${zvbi_SOURCES})

target_include_directories(zvbi
  PRIVATE
    src
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET zvbi
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

