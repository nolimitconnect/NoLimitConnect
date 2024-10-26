
add_compile_definitions(
    LIB_STATIC
    HAVE_CONFIG_H
    FILE_OFFSET_BITS=64
    ENABLE_NLC_PLAYER
)

add_compile_definitions(
    FT_DEBUG_LEVEL_ERROR=1
    DEBUG_LEVEL_TRACE=1
    FT2_BUILD_LIBRARY=1
    FT_MAKE_OPTION_SINGLE_OBJECT=1
)

if(DEFINED TARGET_OS_WINDOWS)
    add_compile_definitions(
        DSO_WIN32
    )
endif()
