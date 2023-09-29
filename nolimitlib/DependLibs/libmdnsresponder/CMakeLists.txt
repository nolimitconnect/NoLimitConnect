
cmake_minimum_required (VERSION 3.22.1)

project(mdnssd, LANGUAGES C CXX)

set(mdnssd_SOURCES)

list(APPEND mdnssd_SOURCES 
    mDNSShared/DebugServices.c 
    mDNSShared/GenLinkedList.c 
    mDNSShared/dnssd_clientlib.c 
    mDNSShared/dnssd_clientstub.c 
    mDNSShared/dnssd_ipc.c 
)

add_library(mdnssd STATIC ${mdnssd_SOURCES})

if(DEFINED TARGET_OS_WINDOWS)
    target_compile_definitions(mdnssd
      PRIVATE
        WINDOWS
    )
endif()

if(DEFINED TARGET_OS_LINUX)
    target_compile_definitions(mdnssd
      PRIVATE
        NOT_HAVE_SA_LEN
    )
endif()

target_include_directories(mdnssd
  PRIVATE
    libmdnsresponder
    libmdnsresponder/mDNSShared
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET mdnssd
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

