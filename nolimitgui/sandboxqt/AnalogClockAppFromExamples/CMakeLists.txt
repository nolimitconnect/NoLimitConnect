cmake_minimum_required(VERSION 3.16.3)

# print command line parameters
get_cmake_property(CacheVars CACHE_VARIABLES)
foreach(CacheVar ${CacheVars})
  get_property(CacheVarHelpString CACHE ${CacheVar} PROPERTY HELPSTRING)
  if(CacheVarHelpString STREQUAL "No help, variable specified on the command line.")
    get_property(CacheVarType CACHE ${CacheVar} PROPERTY TYPE)
    if(CacheVarType STREQUAL "UNINITIALIZED")
      set(CacheVarType)
    else()
      set(CacheVarType :${CacheVarType})
    endif()
    set(CMakeArgs "${CMakeArgs} -D${CacheVar}${CacheVarType}=\"${${CacheVar}}\"")
  endif()
endforeach()
message("CMakeArgs: ${CMakeArgs}")

message("CMAKE_HOST_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME}")

#=== setup build paths etc ===#
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PROJECT_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SRC_ROOT_DIR}/cmake_helpers_test")
message(STATUS "cmake modules dir ${CMAKE_MODULE_PATH}")
message(STATUS "cmake build type ${CMAKE_BUILD_TYPE}")

if(DEFINED MAKE_ENV)
    # assume called from gitlab ci pipeline.. linux only compile and cross compile
    set(CI_PIPELINE_MAKE true)
    add_compile_definitions(CI_PIPELINE_MAKE)
	message(STATUS "CI Pipeline Build")

elseif(DEFINED CMAKE_PREFIX_PATH)
    ### assumes cmake was called from QCreator
	set(IDE_TOOL_MAKE true)
    add_compile_definitions(IDE_TOOL_MAKE)
	message(STATUS "QCreator of VS Build")
	
    if(DEFINED ANDROID_NDK)
        set(TARGET_OS_ANDROID true)
        add_compile_definitions(TARGET_OS_ANDROID)

        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            message(STATUS "QCreator Build Target OS Android built on host OS Windows")
        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            message(STATUS "QCreator Build Target OS Android built on host OS Linux")
        else()
            message(FATAL_ERROR "QCreator Build host is Unknown OS attempting to compile Android")
        endif()		
    endif()
	
else()
    ### assumes cmake was called from command line and needs the enviroment setup
    # set define so can deterimine if need additional enviroment params missing in command line compile
    set(CMD_LINE_MAKE true)
    add_compile_definitions(CMD_LINE_MAKE)
	message(STATUS "Command Line Make Build")
	
endif()

#there is a bug in qt.. either set CMAKE_PREFIX_PATH to the qt path or you will need to set t6Core_DIR, Qt6Gui_DIR, Qt6Widgets_DIR etc,

if(CMD_LINE_MAKE)
	if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "HOST OS Windows")

        if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
            set(CMAKE_PREFIX_PATH "F:/Qt/6.6.0/msvc2019_64/")
            message(STATUS "Compiling with MSVC")
        else()
            set(CMAKE_PREFIX_PATH "F:/Qt/6.6.0/mingw_64/")
            message(STATUS "Compiling with mingw")
        endif()

	elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		MESSAGE("HOST OS Linux")	
		set(CMAKE_PREFIX_PATH "~/Qt/6.6.0/gcc_64/")	
	else()	
		MESSAGE("CMAKE_HOST_SYSTEM_NAME UNKNOWN")	
	endif()
	
endif()

#=== project setup ===#

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(AnalogClockTest LANGUAGES C CXX)

add_subdirectory(analogclock)






