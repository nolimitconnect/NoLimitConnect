
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

include(ConfigStaticDependLib)

add_subdirectory(ffmpeg)

add_subdirectory(CoreLib)
add_subdirectory(MediaToolsLib)
add_subdirectory(NetLib)
add_subdirectory(PktLib)
add_subdirectory(VxVideoLib)
#add_subdirectory(androidNdk) # not used as library but used in android builds
add_subdirectory(bzlib)
#add_subdirectory(flatbuffers) # not used directly but headers are used
#add_subdirectory(libUPnP) # used if enabled in player-nlc
add_subdirectory(libaemc-webrtc)
#add_subdirectory(libaom) # not used
add_subdirectory(libarmrwbenc)
add_subdirectory(libasprintf)
add_subdirectory(libass)
#add_subdirectory(libbento4) # not used
add_subdirectory(libbzip2)
#add_subdirectory(libcdio) # removed
add_subdirectory(libcpluff)
add_subdirectory(libcroco)
add_subdirectory(libcrossguid)
#add_subdirectory(libcrystalhd) # not used
#add_subdirectory(libcurl)
add_subdirectory(libdav1d)
#add_subdirectory(libdca) # not used
#add_subdirectory(libdsp-webrtc) # not used
add_subdirectory(libechocancel)
add_subdirectory(libexif)
add_subdirectory(libexpat)
#add_subdirectory(libfaac) # not used
add_subdirectory(libfdk-aac)
add_subdirectory(libflac)
add_subdirectory(libfreetype)
add_subdirectory(libfribidi)
#add_subdirectory(libglew)
add_subdirectory(libgnu)
add_subdirectory(libgsm)
add_subdirectory(libiconv)
add_subdirectory(libilbc)
add_subdirectory(libjpg)
add_subdirectory(liblame)
#add_subdirectory(liblcms2) # not used but headers needed for avcodec
add_subdirectory(liblzma)
add_subdirectory(liblzo)
#add_subdirectory(libmdnsresponder) # only needed if enabled in player-nlc
add_subdirectory(libmfx)
#add_subdirectory(libmicrohttpd) # only needed if enabled in player-nlc
add_subdirectory(libminiaudio)
add_subdirectory(libminiupnpc)
add_subdirectory(libnfs)
add_subdirectory(libogg)
add_subdirectory(libopencore-amr)
#add_subdirectory(libopenh264) # not used yet
add_subdirectory(libopenjpeg)
add_subdirectory(libopenmpt)
add_subdirectory(libpcre)
add_subdirectory(libplayer-nlc)
#add_subdirectory(libplist) # not used ?? pretty sure it is needed
add_subdirectory(libpng)
if(DEFINED TARGET_OS_WINDOWS)
    add_subdirectory(libpthread) # only needed on windows
endif()

#add_subdirectory(librapidjson) # used by player-nlc but does not need built

add_subdirectory(libshine)
add_subdirectory(libsnappy)
add_subdirectory(libspeex)
#add_subdirectory(libssh)
add_subdirectory(libtag)
add_subdirectory(libtheora)
add_subdirectory(libtinyxml)
add_subdirectory(libtwolame)
add_subdirectory(libvorbis)
if(DEFINED TARGET_OS_WINDOWS)
#add_subdirectory(libvpx-win64) # windows only
elseif(DEFINED TARGET_OS_LINUX)
add_subdirectory(libvpx) 
elseif(DEFINED TARGET_OS_ANDROID)
#add_subdirectory(libvpx) # needed to play some video types but does not compile yet
endif()

####add_subdirectory(libvpx_win) # needed to play some videp types but does not compile yet
add_subdirectory(libwavepack)
add_subdirectory(libwebp)

if(DEFINED TARGET_OS_ANDROID)
    #libx265 does not compile for arm7a also kodi does not include in arm builds
    if(DEFINED TARGET_CPU_AARCH64)
        add_subdirectory(libx264)
        add_subdirectory(libx265)
    endif()
else()
 add_subdirectory(libx264)
 add_subdirectory(libx265)
endif()

#add_subdirectory(libxavs) # not used (video codec)
add_subdirectory(libxml2)
add_subdirectory(libxslt)
#add_subdirectory(libxvid) # not used (video codec)
add_subdirectory(libyajl)
add_subdirectory(libzip)
add_subdirectory(libzlib)
add_subdirectory(libzvbi) 
add_subdirectory(openssl-1.0.2p) # needed to avoid link errors but not actually ran by nlc


