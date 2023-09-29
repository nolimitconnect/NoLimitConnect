
cmake_minimum_required (VERSION 3.22.1)

project(nfs, LANGUAGES C CXX)

set(nfs_SOURCES)

list(APPEND nfs_SOURCES 
    init.c
    libnfs-raw-mount.c
    libnfs-raw-nfs.c
    libnfs-raw-nlm.c
    libnfs-raw-nsm.c
    libnfs-raw-portmap.c
    libnfs-raw-rquota.c
    libnfs-sync.c
    libnfs-zdr.c
    libnfs.c
    mount.c
    nfs.c
    nfsacl.c
    nlm.c
    nsm.c
    pdu.c
    portmap.c
    rquota.c
    socket.c
    win32_compat.c
)

add_library(nfs STATIC ${nfs_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET nfs
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

