
cmake_minimum_required (VERSION 3.22.1)

project(cdio, LANGUAGES C CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/iso9660 ${CMAKE_CURRENT_SOURCE_DIR}/lib/driver )

set(cdio_SOURCES)

list(APPEND cdio_SOURCES 
    lib/cdio++/iso9660_cdio.cpp
    lib/cdio++/cdio_cdio.cpp
    lib/cdio++/devices_cdio.cpp
    lib/driver/FreeBSD/freebsd_cdio.c
    lib/driver/FreeBSD/freebsd_cam_cdio.c
    lib/driver/FreeBSD/freebsd_ioctl_cdio.c
    lib/driver/image/bincue_cdio.c
    lib/driver/image/cdrdao_cdio.c
    lib/driver/image/nrg_cdio.c
    lib/driver/mmc/mmc_cdio.c
    lib/driver/mmc/mmc_hl_cmds_cdio.c
    lib/driver/mmc/mmc_ll_cmds_cdio.c
    lib/driver/mmc/mmc_util_cdio.c
    lib/driver/MSWindows/aspi32_cdio.c
    lib/driver/MSWindows/win32_cdio.c
    lib/driver/MSWindows/win32_ioctl_cdio.c
    lib/driver/_cdio_generic_cdio.c
    lib/driver/_cdio_stdio_cdio.c
    lib/driver/_cdio_stream_cdio.c
    lib/driver/abs_path_cdio.c
    lib/driver/aix_cdio.c
    lib/driver/audio_cdio.c
    lib/driver/bsdi_cdio.c
    lib/driver/cd_types_cdio.c
    lib/driver/cdtext_cdio.c
    lib/driver/device_cdio.c
    lib/driver/disc_cdio.c
    lib/driver/ds_cdio.c
    lib/driver/gnu_linux_cdio.c
    lib/driver/image_common_cdio.c
    lib/driver/logging_cdio.c
    lib/driver/memory_cdio.c
    lib/driver/netbsd_cdio.c
    lib/driver/os2_cdio.c
    lib/driver/osx_cdio.c
    lib/driver/read_cdio.c
    lib/driver/realpath_cdio.c
    lib/driver/sector_cdio.c
    lib/driver/solaris_cdio.c
    lib/driver/track_cdio.c
    lib/driver/utf8_cdio.c
    lib/driver/util_cdio.c
    lib/iso9660/iso9660_cdio2.c
    lib/iso9660/iso9660_fs_cdio2.c
    lib/iso9660/rock_cdio.c
    lib/iso9660/xa_cdio.c
    lib/udf/filemode_cdio.c
    lib/udf/udf_cdio.c
    lib/udf/udf_file_cdio.c
    lib/udf/udf_fs_cdio.c
    lib/udf/udf_time_cdio.c
)

add_library(cdio STATIC ${cdio_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET cdio
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
