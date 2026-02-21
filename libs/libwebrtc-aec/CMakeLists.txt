
cmake_minimum_required(VERSION 3.28.0)

project(webrtc_aec LANGUAGES CXX)

if(MSVC)
	# set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES "")
	# Ensure standard Windows flags and exception handling are included
	set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} /DWIN32 /D_WINDOWS /EHsc")
endif()

get_directory_property(_nlc_webrtc_aec_saved_includes INCLUDE_DIRECTORIES)
get_directory_property(_nlc_webrtc_aec_saved_definitions COMPILE_DEFINITIONS)

set_directory_properties(PROPERTIES
	INCLUDE_DIRECTORIES ""
	COMPILE_DEFINITIONS ""
)
add_subdirectory(abseil-cpp)

set_directory_properties(PROPERTIES
	INCLUDE_DIRECTORIES "${_nlc_webrtc_aec_saved_includes}"
	COMPILE_DEFINITIONS "${_nlc_webrtc_aec_saved_definitions}"
)
add_subdirectory(apm)

