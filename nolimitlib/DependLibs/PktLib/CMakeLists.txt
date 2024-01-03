
cmake_minimum_required (VERSION 3.22.1)

project(pktlib, LANGUAGES CXX)

set(pktlib_SOURCES)

list(APPEND pktlib_SOURCES 
    ConnectId.cpp
    GroupieId.cpp
    HostedId.cpp
    HostUserSessionId.cpp
    MatchParams.cpp
	PktAnnList.cpp
    PktAnnounce.cpp
	PktBlobEntry.cpp
    PktChatReq.cpp
    PktDebugHelpers.cpp
	PktsOfferXfer.cpp
    PktPluginHandlerBase.cpp
    PktSysHandlerBase.cpp
    PktTcpPunch.cpp
    PktVoiceReply.cpp
    PktVoiceReq.cpp
    PktsAssetXfer.cpp
    PktsBaseXfer.cpp
	PktsBlobXfer.cpp
    PktsFileInfo.cpp
    PktsFileList.cpp
    PktsFileShare.cpp
	PktsFriendOffer.cpp
    PktsGroupie.cpp
    PktsHostInfo.cpp
    PktsHostInvite.cpp
	PktsHostJoin.cpp
    PktsHostSearch.cpp
    PktsHostUser.cpp
    PktsImAlive.cpp
    PktsMembership.cpp
    PktsMultiSession.cpp
    PktsPing.cpp
    PktsPluginOffer.cpp
    PktsPushToTalk.cpp
    PktsQueryHostUrl.cpp
    PktsRelay.cpp
	PktsScan.cpp
	PktsSearch.cpp
    PktsSession.cpp
    PktsTestConnection.cpp
    PktsThumbXfer.cpp
    PktsTodGame.cpp
    PktsVideoFeed.cpp
    PktsWebServer.cpp
	PluginId.cpp
	PluginIdList.cpp
	SearchParams.cpp
    VxCommon.cpp
    VxConnectId.cpp
    VxConnectInfo.cpp
    VxFriendMatch.cpp
    VxNetIdentBase.cpp
    VxOnlineStatusFlags.cpp
    VxPktHdr.cpp
    VxRelayFlags.cpp
    VxSearchFlags.cpp
)

add_library(pktlib STATIC ${pktlib_SOURCES})

target_include_directories(pktlib
  PRIVATE
    ../../ptop_src/ptop_engine_src
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET pktlib
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

