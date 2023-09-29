
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#=== project subdirectories ===#

add_subdirectory(opus)
add_subdirectory(libavcodec)
add_subdirectory(libavdevice)
add_subdirectory(libavfilter)
add_subdirectory(libavformat)
add_subdirectory(libavutil)
add_subdirectory(libpostproc)
add_subdirectory(libswresample)
add_subdirectory(libswscale)

