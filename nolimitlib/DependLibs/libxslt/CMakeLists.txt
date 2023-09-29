
cmake_minimum_required (VERSION 3.22.1)

project(xslt, LANGUAGES C CXX)

set(xslt_SOURCES)

list(APPEND xslt_SOURCES 
    attributes_xslt.c
    attrvt_xslt.c
    documents_xslt.c
    extensions_xslt.c
    extra_xslt.c
    functions_xslt.c
    imports_xslt.c
    keys_xslt.c
    namespaces_xslt.c
    numbers_xslt.c
    pattern_xslt.c
    preproc_xslt.c
    security_xslt.c
    templates_xslt.c
    transform_xslt.c
    variables_xslt.c
    xslt_xslt.c
    xsltlocale_xslt.c
    xsltutils_xslt.c
)

add_library(xslt STATIC ${xslt_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET xslt
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
