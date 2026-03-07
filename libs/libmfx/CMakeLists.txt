
cmake_minimum_required (VERSION 3.22.1)

project(mfx, LANGUAGES C CXX)

set(mfx_SOURCES)

list(APPEND mfx_SOURCES 
  src/main.cpp
  src/mfx_dispatcher.cpp
  src/mfx_dispatcher_log.cpp
  src/mfx_function_table.cpp
)

add_library(mfx STATIC ${mfx_SOURCES})

nlc_target_add_supported_warning_options(mfx CXX
    -Wno-undefined-bool-conversion
)

if(DEFINED TARGET_OS_WINDOWS)
    target_compile_definitions(mfx
      PRIVATE
        WIN64
    )
endif()

target_compile_definitions(mfx
    PRIVATE
    _LIB
    MFX_VA
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET mfx
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()


