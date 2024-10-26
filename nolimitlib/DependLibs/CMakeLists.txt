
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

include(ConfigStaticDependLib)

add_subdirectory(libvpx)

add_subdirectory(ffmpeg)

add_subdirectory(CoreLib)
add_subdirectory(MediaToolsLib)
add_subdirectory(NetLib)
add_subdirectory(PktLib)
add_subdirectory(VxVideoLib)
#add_subdirectory(androidNdk) # not used as library but used in android builds

#add_subdirectory(flatbuffers) # not used directly but headers are used
add_subdirectory(libaemc-webrtc)

add_subdirectory(libasprintf)
add_subdirectory(libass)

add_subdirectory(libcrossguid)
add_subdirectory(libdav1d)
add_subdirectory(libechocancel)
add_subdirectory(libfreetype)
add_subdirectory(libfribidi)
#add_subdirectory(libglew)
add_subdirectory(libgnu)

add_subdirectory(libiconv)

add_subdirectory(libjpg)
add_subdirectory(liblame)
#add_subdirectory(liblcms2) # not used but headers needed for avcodec
add_subdirectory(liblzma)
add_subdirectory(liblzo)
add_subdirectory(libmfx)
add_subdirectory(libminiaudio)
add_subdirectory(libminiupnpc)

add_subdirectory(libogg)
add_subdirectory(libopencore-amr)

add_subdirectory(libpcre)
add_subdirectory(libplayer-nlc)
#add_subdirectory(libplist) # not used ?? pretty sure it is needed

if(DEFINED TARGET_OS_WINDOWS)
    add_subdirectory(libpthread) # only needed on windows
endif()

add_subdirectory(libptopengine)

add_subdirectory(libspeex)
add_subdirectory(libtag)
add_subdirectory(libtheora)
add_subdirectory(libtinyxml)
add_subdirectory(libtwolame)
add_subdirectory(libvorbis)

#libx265 does not compile for arm7a also kodi does not include in arm builds
if(NOT DEFINED TARGET_CPU_ARM32)
 add_subdirectory(libx265)
endif()

add_subdirectory(libx264)

#add_subdirectory(libxavs) # not used (video codec)
add_subdirectory(libxml2)
#add_subdirectory(libxvid) # not used (video codec)
add_subdirectory(libzip)
add_subdirectory(libzlib)



