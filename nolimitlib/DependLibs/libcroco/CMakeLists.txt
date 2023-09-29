
cmake_minimum_required (VERSION 3.22.1)

project(croco LANGUAGES C CXX)

set(croco_SOURCES)

list(APPEND croco_SOURCES 
    cr-additional-sel.c
    cr-attr-sel.c
    cr-cascade.c
    cr-declaration.c
    cr-doc-handler.c
    cr-enc-handler.c
    cr-fonts.c
    cr-input.c
    cr-num.c
    cr-om-parser.c
    cr-parser.c
    cr-parsing-location.c
    cr-prop-list.c
    cr-pseudo.c
    cr-rgb.c
    cr-sel-eng.c
    cr-selector.c
    cr-simple-sel.c
    cr-statement.c
    cr-string.c
    cr-style.c
    cr-stylesheet.c
    cr-term.c
    cr-tknzr.c
    cr-token.c
    cr-utils.c
)

add_library(croco STATIC ${croco_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET croco
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
