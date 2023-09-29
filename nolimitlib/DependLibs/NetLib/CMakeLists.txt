
cmake_minimum_required (VERSION 3.22.1)

project(netlib, LANGUAGES CXX)

set(netlib_SOURCES)

list(APPEND netlib_SOURCES 
    NetHostSetting.cpp
    InetAddress.cpp
    NetSettings.cpp
    VxBaseSktInfo.cpp
    VxClientMgr.cpp
    VxFileXferInfo.cpp
    VxGetExternalIps.cpp
    VxGetRandomPort.cpp
	VxHackerMgr.cpp
	VxHackerRecord.cpp
    VxHttpParse.cpp
    VxHttpRequest.cpp
    VxHttpValuePair.cpp
    VxParseUrlComponents.cpp
    VxPeerMgr.cpp
    VxProxy.cpp
    VxResolveHost.cpp
    VxServerMgr.cpp
    VxSktAccept.cpp
    VxSktBase.cpp
    VxSktBaseMgr.cpp
    VxSktBuf.cpp
    VxSktConnect.cpp
    VxSktConnectSimple.cpp
    VxSktCrypto.cpp
    VxSktStatMgr.cpp
    VxSktStatRecord.cpp
    VxSktThrottle.cpp
    VxSktUdp.cpp
    VxSktUdpBroadcast.cpp
    VxSktUtil.cpp
    VxSktWaitingList.cpp
    VxXferDefs.cpp
)

add_library(netlib STATIC ${netlib_SOURCES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET netlib
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

