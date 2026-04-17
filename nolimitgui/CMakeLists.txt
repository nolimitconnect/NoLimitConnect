
project(nolimitconnect,
        VERSION ${NLC_VERSION}
        LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_STATIC_LIB_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui LinguistTools Multimedia OpenGL Svg Widgets)

if(COMMAND qt_policy)
    qt_policy(SET QTP0002 NEW)
endif()

if(NOT DEFINED TARGET_OS_ANDROID)
    find_package(OpenGL REQUIRED)
endif()

set (_allTargets "")

macro(add_executable _target)
    _add_executable (${_target} ${ARGN})
    list (APPEND _allTargets ${_target})
endmacro()

if(MSVC)
    # 1. The Preprocessor approach (Most effective for CRT/Standard Library)
    # This stops the "This function or variable may be unsafe" warnings (e.g. strcpy)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(_SCL_SECURE_NO_WARNINGS)

    # 2. The Compiler Flag approach (Broadest hammer)
    # /wd4996 tells the compiler to literally 'Warning Disable' 4996
    add_compile_options(/wd4996)
endif()

#=== SETUP EXECUTABLE ===#
message(STATUS "nolimitconnect gui project prefix ${CMAKE_PREFIX_PATH}")
# Add the forms directory to the AUOUIC search paths
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS
    src/Forms
    )

# Add the executable

set(nlc_HEADERS)
list(APPEND nlc_HEADERS
    src/AccountMgr.h
    AppCompilerConfig.h
    AppCpuArchDefines.h
    AppDependLibrariesConfig.h
    AppTargetOsConfig.h

    src/AccountDb.h
    src/AccountMgr.h
    src/AcceptCancelWidget.h
    src/ActionWidget.h

    src/ActivityBrowseFiles.h
    src/ActivityDownloadItemMenu.h
    src/ActivityGenerateHash.h
    src/ActivityMessageBox.h
    src/ActivityMsgBoxOk.h
    src/ActivityMsgBoxYesNo.h
    src/ActivitySessionOptions.h
    src/ActivityShowHelp.h
    src/ActivityTimedMessage.h
    src/ActivityBase.h

    src/AdminAvailMgr.h
    src/AppCommon.h
    src/AppDefs.h
    src/AppGlobals.h
    src/AppSettings.h
    src/AppletAboutApp.h
    src/AppletAboutFile.h
    src/AppletAboutMeClient.h
    src/AppletAboutUser.h

    src/AppletAboutMeServerViewMine.h
    src/AppletApplicationInfo.h
    src/AppletAssetPlayerBase.h
    src/AppletBase.h
    src/AppletBrowseFiles.h
    src/AppletPlayerCamClip.h
    src/AppletCamClient.h
    src/AppletCamSettings.h
    src/AppletCamServerViewMine.h
    src/AppletChatRoomJoin.h
    src/AppletChatRoomClient.h
    src/AppletChatRoomHostAdmin.h
    src/AppletChatRoomJoinSearch.h
    src/AppletChatRoomListLocalView.h
    src/AppletChooseThumbnail.h
    src/AppletClientAvatarImage.h
    src/AppletClientBase.h
    src/AppletCreateAccount.h

    src/AppletRandomConnectClient.h
    src/AppletDownloads.h
    src/AppletEditAboutMe.h
    src/AppletEditAvatarImage.h
    src/AppletEditStoryboard.h
    src/AppletFileOfferSelect.h
    src/AppletFileShareClientView.h
    src/AppletFileShareServerViewMine.h
    src/AppletFriendList.h
    src/AppletFriendRequest.h
    src/AppletFriendRequestList.h

    src/AppletGalleryImage.h
    src/AppletGalleryEmoticon.h
    src/AppletGalleryThumb.h
    src/AppletGetStarted.h
    src/AppletGroupClient.h
    src/AppletGroupHostAdmin.h
    src/AppletGroupJoinSearch.h
    src/AppletGroupJoin.h
    src/AppletGroupListClient.h
    src/AppletGroupListLocalView.h
    src/AppletHackerList.h
    src/AppletHelpNetSignalBars.h

    src/AppletHostAdminBase.h
    src/AppletHostBase.h
    src/AppletHostChatRoomStatus.h
    src/AppletHostClientBase.h
    src/AppletHostGroupStatus.h
    src/AppletHostRandomConnectStatus.h
    src/AppletHostJoinChoose.h
    src/AppletHostJoinConnect.h
    src/AppletHostJoinRequestList.h
    src/AppletHostLeave.h
    src/AppletHostNetworkStatus.h
    src/AppletIgnoredHosts.h

    src/AppletInformation.h
    src/AppletInviteAccept.h
    src/AppletInviteCreate.h
    src/AppletIsPortOpenTest.h
    src/AppletJoinBase.h
    src/AppletLanguageSelect.h   
    src/AppletLaunchPage.h
    src/AppletLaunchWidget.h
    src/AppletLibrary.h
    src/AppletLog.h
    src/AppletLogSettings.h

    src/AppletMultiMessenger.h
    src/AppletMgr.h
    src/AppletNetHostingPage.h
    src/AppletNetworkSettings.h
    src/AppletOfferInfo.h
    src/AppletOfferList.h
    src/AppletOfferResponse.h
    src/AppletOfferResponseAccept.h
    src/AppletOfferSend.h
    src/AppletOfferView.h
    src/AppletPersonOfferList.h
    src/AppletPeerBase.h
    src/AppletPeerChangeFriendship.h
    src/AppletPeerReplyFileOffer.h
    src/AppletPeerSessionFileOffer.h
    src/AppletPeerTodGame.h
    src/AppletPeerVideoPhone.h
    src/AppletPeerVoicePhone.h
    src/AppletPermissionList.h
    src/AppletPersonalRecorder.h
    src/AppletPlayerBase.h
    src/AppletPlayerPhoto.h
    src/AppletPlayerVideo.h
    src/AppletPlayerNlc.h
    src/AppletPlayerNlcBase.h

    src/AppletOfferRandSession.h
    src/AppletPopupMenu.h
    src/AppletRandomConnectHostAdmin.h
    src/AppletRandomConnectJoin.h
    src/AppletRandomConnectJoinSearch.h
    src/AppletRandomConnectListLocalView.h
    src/AppletSearchPage.h
    src/AppletServiceAboutMe.h

    src/AppletServiceBase.h
    src/AppletServiceBaseSettings.h
    src/AppletServiceConnectionTest.h
    src/AppletServiceHostNetwork.h
    src/AppletServiceShareFiles.h
    src/AppletServiceShareWebCam.h
    src/AppletServiceStoryboard.h
    src/AppletSettingsAboutMe.h
    src/AppletSettingsAvatarImage.h
    src/AppletSettingsBase.h
    src/AppletSettingsConnectionTest.h
    src/AppletSettingsFileXfer.h
    src/AppletSettingsFriendRequest.h
    src/AppletSettingsHostBase.h
    src/AppletSettingsHostChatRoom.h
    src/AppletSettingsHostGroup.h
    src/AppletSettingsHostNetwork.h
    src/AppletSettingsHostRandomConnect.h
    src/AppletSettingsMessenger.h
    src/AppletSettingsPage.h
    src/AppletSettingsPushToTalk.h
    src/AppletSettingsRandomConnect.h
    src/AppletSettingsShareFiles.h
    src/AppletSettingsShareWebCam.h
    src/AppletSettingsStoryboard.h
    src/AppletSettingsTruthOrDare.h
    src/AppletSettingsVideoPhone.h
    src/AppletSettingsVoicePhone.h
    src/AppletShareServicesPage.h
    src/AppletSnapshot.h
    src/AppletSocketList.h
    src/AppletSoundSettings.h
    src/AppletStoryboardClient.h
    src/AppletStoryboardServerViewMine.h

    src/AppletTestAndDebug.h
    src/AppletTestUpnp.h

    src/AppletTheme.h
    src/AppletUploads.h
    src/AppletUserConnections.h
    src/AppletUserIdentity.h
    src/AppletUserPreferences.h
    src/AppTranslate.h
    src/AssetFaceWidget.h
    src/AssetPhotoWidget.h
    src/AssetTextWidget.h
    src/AssetVideoWidget.h
    src/AssetVoiceWidget.h
    src/AssetBaseWidget.h
    src/AvatarBar.h

    src/AudioMgr.h
    src/AudioInWaveformWidget.h
    src/AudioOutWaveformWidget.h

    src/AudioMixerMgr.h

    src/BottomBarWidget.h
    src/CamTitleBarPreview.h

    src/CamFrameProcessor.h
    src/CamJavaClient.h
    src/CamLogic.h
    src/CamProcessor.h
    src/CamV4L2.h

    src/ChatEntryWidget.h
    src/ClipboardCopyWidget.h
    src/ClipboardPasteWidget.h

    src/DialogAddComment.h
    src/DialogConfirmRemoveMessage.h
    src/EventsQtToNlc.h
    src/FileItemInfo.h
    src/FriendList.h
    src/FriendListDelegate.h
    src/FileActionBarWidget.h
    src/FileActionMenu.h
    src/FileFilterSelectWidget.h
    src/FileListReplySession.h
    src/FileMediaSelectWidget.h
    src/FileShareItemWidget.h
    src/FileXferWidget.h

    src/FriendListEntryWidget.h
    src/FriendListWidget.h
    src/GuiConnectIdListMgr.h
    src/GuiConnectIdListCallback.h
    src/GuiFileXferMgr.h
    src/GuiFileXferSession.h
    src/GuiFavoriteMgr.h

    src/GuiFriendRequest/GuiFriendRequest.h
    src/GuiFriendRequest/GuiFriendRequestCallback.h
    src/GuiFriendRequest/GuiFriendRequestListItem.h
    src/GuiFriendRequest/GuiFriendRequestListWidget.h
    src/GuiFriendRequest/GuiFriendRequestMgr.h

    src/GuiGroupie.h
    src/GuiGroupieListCallback.h
    src/GuiGroupieListItem.h
    src/GuiGroupieListMgr.h
    src/GuiGroupieListSession.h
    src/GuiGroupieListWidget.h
    src/GuiHelpers.h
    src/GuiHosted.h
    src/GuiHostedListCallback.h
    src/GuiHostedListItem.h
    src/GuiHostedListMgr.h
    src/GuiHostedListSession.h
    src/GuiHostedListWidget.h
    src/GuiHostJoin.h
    src/GuiHostJoinCallback.h
    src/GuiHostJoinList.h
    src/GuiHostedByMeJoinMgr.h
    src/GuiHostJoinSession.h
    src/GuiHostSession.h

    src/GuiMemberActiveCallback.h
    src/GuiMemberActiveMgr.h
    src/GuiOfferCallback.h
    src/GuiOfferInfo.h
    src/GuiOfferListItem.h
    src/GuiOfferListWidget.h
    src/GuiOfferMgr.h
    src/GuiOfferMgrBase.h
    src/GuiOfferSession.h
    src/GuiPushToTalkMgr.h
    src/GuiParams.h
    src/GuiPlayerCallback.h
    src/GuiPlayerMgr.h
    src/GuiPluginMgr.h
    src/GuiRandConnectCallback.h
    src/GuiRandConnectMgr.h
    src/GuiSendQueueCallback.h
    src/GuiSendQueueMgr.h

    src/GuiThumb.h
    src/GuiThumbList.h
    src/GuiThumbMgr.h

    src/GuiThreadAppLoader.h
    src/GuiThreadMainLoader.h
    src/GuiThreadSettingsLoader.h

    src/GuiUser.h
    src/GuiUserBase.h
    src/GuiUserJoin.h
    src/GuiUserJoinCallback.h
    src/GuiUserJoinList.h
    src/GuiUserJoinListItem.h
    src/GuiUserJoinListWidget.h
    src/GuiUserJoinMgr.h
    src/GuiUserJoinSession.h
    src/GuiUserListItem.h
    src/GuiUserListWidget.h
    src/GuiUserMgr.h
    src/GuiUserMultiListWidget.h
    src/GuiUserSessionBase.h
    src/GuiUserUpdateCallback.h
    src/GuiWebPageCallback.h
    src/GuiWebPageMgr.h
    src/HistoryEntryWidget.h
    src/HistoryListWidget.h
    src/HostedPluginWidget.h
    src/HostJoinRequestListItem.h
    src/HostJoinRequestListWidget.h
    src/HomeWindow.h
    src/IdentLogicInterface.h
    src/IdentOfferWidget.h
    src/IdentWidget.h
    src/InviteUrlWidget.h

    src/ImageListEntry.h
    src/ImagePickBar.h
    src/ImageListRow.h
    src/ImageListWidget.h
    src/InformationWidget.h
    src/InputAllWidget.h
    src/InputBaseWidget.h
    src/InputClientBaseCallback.h
    src/InputClientCallback.h
    src/InputFaceWidget.h
    src/InputPhotoWidget.h
    src/InputTextWidget.h
    src/InputVideoWidget.h
    src/InputVoiceWidget.h
    src/InstMsgWidget.h
    src/ListWidgetBase.h
    src/LogMgr.h
    src/LogoRenderer.h
    src/LogWidget.h
    src/MessengerPage.h
    src/MyIcons.h
    src/MyIconsDefs.h
    src/MultiSessionState.h
    src/NetAvailStatusWidget.h
    src/OfferBarWidget.h
    src/OfferSendWidget.h
    src/OfferSessionLogic.h
    src/PermissionWidget.h
    src/PermissionListItem.h
    src/PersonOfferListItem.h
    src/PersonOfferListWidget.h
    src/PhoneRinger.h
    src/PlayControlWidget.h
    src/PluginSettingsWidget.h
    src/RenderGlOffScreenSurface.h
    src/RenderGlLogic.h
    src/RenderGlShaders.h
    src/RenderGlWidget.h
    src/RenderLogoShaders.h
    src/RenderPlayerNlcThread.h
    src/RenderShaderQt.h
    src/RichTextWidget.h
    src/SearchBarWidget.h
    src/SearchParamsWidget.h
    src/ServiceSettingsWidget.h
    src/SessionWidget.h
    src/StoryWidget.h

    src/miniaudio/AudioFrameAecBuffer.h
    src/miniaudio/AudioMixerBuf.h
    src/miniaudio/AudioMixerInterface.h
    src/miniaudio/AudioSampleBuf.h
    src/miniaudio/AudioTestToneGenerator.h
    src/miniaudio/AudioUtils.h

    src/miniaudio/MiniAudioDevices.h
    src/miniaudio/MiniAudioIn.h
    src/miniaudio/MiniAudioInDevice.h
    src/miniaudio/MiniAudioOut.h
    src/miniaudio/MiniAudioOutDevice.h

    src/miniaudio/TestFileWav.h
    src/miniaudio/TestFileWavMgr.h

    src/SoundDefs.h
    src/SoundFxMgr.h
    src/TestWidget.h
    src/TextChatLogic.h
    src/ThumbnailEditWidget.h
    src/ThumbnailChooseWidget.h
    src/ThumbnailViewWidget.h
    src/TodGameDefs.h
    src/TodGameLogic.h
    src/TodGameMgr.h
    src/TodGameWidget.h
    src/TodStatsWidget.h
    src/ToGuiActivityInterface.h
    src/ToGuiFileXferInterface.h
    src/ToGuiHardwareControlInterface.h
    src/ToGuiUserUpdateInterface.h
    src/ToGuiThumbUpdateInterface.h
    src/TitleBarWidget.h

    src/VxAspectFrame.h
    src/VxContextMenu.h
    src/VxComboBox.h
    src/VxAvatarImage.h
    src/VxPushButton.h

    src/VxLabel.h
    src/VxSndInstance.h
    src/VxFrame.h
    src/VxMenu.h
    src/VxMenuButton.h
    src/VxMenuButtonBottom.h
    src/VxMenuButtonTop.h
    src/VxResourceToRealFile.h
    src/VxScrollArea.h
    src/VxSpinProgress.h
    src/VxTagLabel.h
    src/VxTilePositioner.h
    src/VidWidget.h
    src/VxWidgetBase.h
    src/VxAppStyle.h
    src/VxAppTheme.h
    src/VxAppDisplay.h
    src/VxPlainTextEdit.h
    src/WaitingSpinnerWidget.h
    src/WebsiteWidget.h
    src/VirtStream/VirtProviderFile.h
    src/VirtStream/VirtStorageProvider.h
    src/VirtStream/VirtStreamCache.h
    src/VirtStream/VirtStreamFile.h
    src/VirtStream/VirtStreamMgr.h
   )

set(nlc_SOURCES)
list(APPEND nlc_SOURCES
    NoLimitConnectMain/AppSource/main.cpp
    src/AccountMgr.cpp
    NoLimitConnectMain/AppSource/main.cpp
    src/AcceptCancelWidget.cpp
    src/AccountDb.cpp
    src/AccountMgr.cpp

    src/ActionWidget.cpp
    src/ActivityBase.cpp
    src/ActivityBrowseFiles.cpp
    src/ActivityDownloadItemMenu.cpp
    src/ActivityGenerateHash.cpp
    src/ActivityMessageBox.cpp
    src/ActivityMsgBoxOk.cpp
    src/ActivityMsgBoxYesNo.cpp
    src/ActivitySessionOptions.cpp
    src/ActivityShowHelp.cpp
    src/ActivityTimedMessage.cpp

    src/AdminAvailMgr.cpp
    src/AppCommon.cpp
    src/AppCommonAudio.cpp
    src/AppCommonEvents.cpp
    src/AppCommonFileMenu.cpp
    src/AppCommonFileShare.cpp
    src/AppCommonFriend.cpp
    src/AppCommonLogin.cpp
    src/AppCommonMedia.cpp
    src/AppCommonNotify.cpp
    src/AppCommonOfferToFriend.cpp
    src/AppCommonRender.cpp

    src/AppCommonSignalsAndSlots.cpp
    src/AppDefs.cpp
    src/AppGlobals.cpp
    src/AppModuleState.cpp
    src/AppSettings.cpp
    src/AppletAboutApp.cpp
    src/AppletAboutFile.cpp
    src/AppletAboutMeClient.cpp
    src/AppletAboutUser.cpp
    src/AppletAboutMeServerViewMine.cpp

    src/AppletApplicationInfo.cpp
    src/AppletAssetPlayerBase.cpp
    src/AppletBase.cpp
    src/AppletBrowseFiles.cpp
    src/AppletPlayerCamClip.cpp
    src/AppletCamClient.cpp
    src/AppletCamSettings.cpp
    src/AppletCamServerViewMine.cpp
    src/AppletChatRoomClient.cpp
    src/AppletChatRoomHostAdmin.cpp
    src/AppletChatRoomJoin.cpp
    src/AppletChatRoomJoinSearch.cpp
    src/AppletChatRoomListLocalView.cpp
    src/AppletChooseThumbnail.cpp
    src/AppletClientAvatarImage.cpp
    src/AppletClientBase.cpp
    src/AppletCreateAccount.cpp
    src/AppletRandomConnectClient.cpp
    src/AppletDownloads.cpp
    src/AppletEditAboutMe.cpp
    src/AppletEditAvatarImage.cpp
    src/AppletEditStoryboard.cpp

    src/AppletFileOfferSelect.cpp
    src/AppletFileShareClientView.cpp
    src/AppletFileShareServerViewMine.cpp
    src/AppletFriendList.cpp
    src/AppletFriendRequest.cpp
    src/AppletFriendRequestList.cpp

    src/AppletGalleryImage.cpp
    src/AppletGalleryEmoticon.cpp
    src/AppletGalleryThumb.cpp
    src/AppletGetStarted.cpp
    src/AppletGroupClient.cpp
    src/AppletGroupHostAdmin.cpp
    src/AppletGroupJoin.cpp
    src/AppletGroupJoinSearch.cpp
    src/AppletGroupListClient.cpp
    src/AppletGroupListLocalView.cpp
    src/AppletHackerList.cpp
    src/AppletHelpNetSignalBars.cpp

    src/AppletHostAdminBase.cpp
    src/AppletHostBase.cpp
    src/AppletHostChatRoomStatus.cpp
    src/AppletHostClientBase.cpp
    src/AppletHostGroupStatus.cpp
    src/AppletHostJoinChoose.cpp
    src/AppletHostJoinConnect.cpp
    src/AppletHostJoinRequestList.cpp
    src/AppletHostLeave.cpp

    src/AppletHostNetworkStatus.cpp
    src/AppletHostRandomConnectStatus.cpp
    src/AppletIgnoredHosts.cpp

    src/AppletInformation.cpp
    src/AppletInviteAccept.cpp
    src/AppletInviteCreate.cpp
    src/AppletIsPortOpenTest.cpp
    src/AppletJoinBase.cpp
    src/AppletLanguageSelect.cpp
    src/AppletLaunchPage.cpp
    src/AppletLaunchWidget.cpp
    src/AppletLibrary.cpp
    src/AppletLog.cpp
    src/AppletLogSettings.cpp

    src/AppletMgr.cpp
    src/AppletMultiMessenger.cpp
    src/AppletNetHostingPage.cpp
    src/AppletNetworkSettings.cpp

    src/AppletOfferInfo.cpp
    src/AppletOfferList.cpp
    src/AppletOfferRandSession.cpp
    src/AppletOfferResponse.cpp
    src/AppletOfferResponseAccept.cpp
    src/AppletOfferSend.cpp
    src/AppletOfferView.cpp
    src/AppletPersonOfferList.cpp
    src/AppletPeerBase.cpp
    src/AppletPeerChangeFriendship.cpp
    src/AppletPeerReplyFileOffer.cpp
    src/AppletPeerSessionFileOffer.cpp
    src/AppletPeerTodGame.cpp
    src/AppletPeerVideoPhone.cpp
    src/AppletPeerVoicePhone.cpp
    src/AppletPermissionList.cpp
    src/AppletPersonalRecorder.cpp
    src/AppletPlayerBase.cpp
    src/AppletPlayerPhoto.cpp
    src/AppletPlayerVideo.cpp
    src/AppletPlayerNlc.cpp
    src/AppletPlayerNlcBase.cpp

    src/AppletPopupMenu.cpp
    src/AppletRandomConnectHostAdmin.cpp
    src/AppletRandomConnectJoin.cpp
    src/AppletRandomConnectJoinSearch.cpp
    src/AppletRandomConnectListLocalView.cpp
    src/AppletSearchPage.cpp
    src/AppletServiceAboutMe.cpp

    src/AppletServiceBase.cpp
    src/AppletServiceBaseSettings.cpp
    src/AppletServiceConnectionTest.cpp
    src/AppletSettingsHostBase.cpp
    src/AppletServiceHostNetwork.cpp
    src/AppletServiceShareFiles.cpp
    src/AppletServiceShareWebCam.cpp
    src/AppletServiceStoryboard.cpp
    src/AppletSettingsAboutMe.cpp
    src/AppletSettingsAvatarImage.cpp
    src/AppletSettingsBase.cpp
    src/AppletSettingsConnectionTest.cpp
    src/AppletSettingsFileXfer.cpp
    src/AppletSettingsFriendRequest.cpp
    src/AppletSettingsHostChatRoom.cpp
    src/AppletSettingsHostGroup.cpp
    src/AppletSettingsHostNetwork.cpp
    src/AppletSettingsHostRandomConnect.cpp
    src/AppletSettingsMessenger.cpp
    src/AppletSettingsPage.cpp
    src/AppletSettingsPushToTalk.cpp
    src/AppletSettingsRandomConnect.cpp
    src/AppletSettingsShareFiles.cpp
    src/AppletSettingsShareWebCam.cpp
    src/AppletSettingsStoryboard.cpp
    src/AppletSettingsTruthOrDare.cpp
    src/AppletSettingsVideoPhone.cpp
    src/AppletSettingsVoicePhone.cpp
    src/AppletShareServicesPage.cpp
    src/AppletSnapshot.cpp
    src/AppletSocketList.cpp
    src/AppletSoundSettings.cpp
    src/AppletStoryboardClient.cpp
    src/AppletStoryboardServerViewMine.cpp

    src/AppletTestAndDebug.cpp
    src/AppletTestUpnp.cpp

    src/AppletTheme.cpp
    src/AppletUploads.cpp
    src/AppletUserConnections.cpp
    src/AppletUserIdentity.cpp
    src/AppletUserPreferences.cpp
    src/AppTranslate.cpp
    src/BottomBarWidget.cpp
    src/AssetBaseWidget.cpp
    src/AssetFaceWidget.cpp
    src/AssetPhotoWidget.cpp
    src/AssetTextWidget.cpp
    src/AssetVideoWidget.cpp
    src/AssetVoiceWidget.cpp
    src/AvatarBar.cpp

    src/AudioMgr.cpp
    src/AudioMgrDelayTest.cpp
    src/AudioMgrIn.cpp
    src/AudioMgrOut.cpp
    src/AudioMgrRequests.cpp

    src/AudioMixerMgr.cpp

    src/AudioInWaveformWidget.cpp
    src/AudioOutWaveformWidget.cpp

    src/CamFrameProcessor.cpp
    src/CamJavaClient.cpp
    src/CamLogic.cpp
    src/CamProcessor.cpp
    src/CamV4L2.cpp

    src/CamTitleBarPreview.cpp
    src/ChatEntryWidget.cpp
    src/ClipboardCopyWidget.cpp
    src/ClipboardPasteWidget.cpp
    src/DialogAddComment.cpp
    src/DialogConfirmRemoveMessage.cpp
    src/EventsQtToNlc.cpp
    src/FileActionBarWidget.cpp
    src/FileActionMenu.cpp
    src/FileFilterSelectWidget.cpp
    src/FileItemInfo.cpp
    src/FileListReplySession.cpp
    src/FileMediaSelectWidget.cpp
    src/FileShareItemWidget.cpp
    #src/FileViewSharedActionBar.cpp

    src/FileXferWidget.cpp
    src/FriendList.cpp
    src/FriendListDelegate.cpp
    src/FriendListWidget.cpp

    src/GuiConnectIdListMgr.cpp
    src/GuiFileXferMgr.cpp
    src/GuiFileXferSession.cpp
    src/GuiFavoriteMgr.cpp

    src/GuiFriendRequest/GuiFriendRequest.cpp
    src/GuiFriendRequest/GuiFriendRequestListItem.cpp
    src/GuiFriendRequest/GuiFriendRequestListWidget.cpp
    src/GuiFriendRequest/GuiFriendRequestMgr.cpp

    src/GuiGroupie.cpp
    src/GuiGroupieListItem.cpp
    src/GuiGroupieListMgr.cpp
    src/GuiGroupieListSession.cpp
    src/GuiGroupieListWidget.cpp
    src/GuiHelpers.cpp
    src/GuiHelpersAndroidPermissions.cpp
    src/GuiHosted.cpp
    src/GuiHostedListItem.cpp
    src/GuiHostedListMgr.cpp
    src/GuiHostedListSession.cpp
    src/GuiHostedListWidget.cpp
    src/GuiHostJoin.cpp
    src/GuiHostJoinList.cpp
    src/GuiHostedByMeJoinMgr.cpp
    src/GuiHostJoinSession.cpp
    src/GuiHostSession.cpp

    src/GuiMemberActiveMgr.cpp
    src/GuiOfferInfo.cpp
    src/GuiOfferListItem.cpp
    src/GuiOfferListWidget.cpp
    src/GuiOfferMgr.cpp
    src/GuiOfferMgrBase.cpp
    src/GuiOfferMgrBaseCallbacks.cpp
    src/GuiOfferSession.cpp
    src/GuiParams.cpp
    src/GuiPlayerMgr.cpp
    src/GuiPluginMgr.cpp
    src/GuiPushToTalkMgr.cpp
    src/GuiRandConnectMgr.cpp
    src/GuiSendQueueMgr.cpp

    src/GuiThreadAppLoader.cpp
    src/GuiThreadMainLoader.cpp
    src/GuiThreadSettingsLoader.cpp

    src/GuiThumb.cpp
    src/GuiThumbList.cpp
    src/GuiThumbMgr.cpp
    src/GuiUser.cpp
    src/GuiUserBase.cpp
    src/GuiUserJoin.cpp
    src/GuiUserJoinList.cpp
    src/GuiUserJoinListItem.cpp
    src/GuiUserJoinListWidget.cpp
    src/GuiUserJoinMgr.cpp
    src/GuiUserJoinSession.cpp
    src/GuiUserListItem.cpp
    src/GuiUserListWidget.cpp
    src/GuiUserMgr.cpp
    src/GuiUserMultiListWidget.cpp
    src/GuiUserSessionBase.cpp
    src/GuiWebPageMgr.cpp
    src/HostedPluginWidget.cpp
    src/HostJoinRequestListItem.cpp
    src/HostJoinRequestListWidget.cpp
    src/HomeWindow.cpp
    src/HistoryEntryWidget.cpp
    src/HistoryListWidget.cpp
    src/ImageListEntry.cpp
    src/ImagePickBar.cpp
    src/ImageListRow.cpp
    src/ImageListWidget.cpp
    src/InformationWidget.cpp
    src/InputAllWidget.cpp
    src/InputBaseWidget.cpp
    src/InputClientBaseCallback.cpp
    src/InputFaceWidget.cpp
    src/InputPhotoWidget.cpp
    src/InputTextWidget.cpp
    src/InputVideoWidget.cpp
    src/InputVoiceWidget.cpp
    src/IdentLogicInterface.cpp
    src/IdentOfferWidget.cpp
    src/IdentWidget.cpp
    src/InstMsgWidget.cpp
    src/InviteUrlWidget.cpp

    src/FriendListEntryWidget.cpp
    src/ListWidgetBase.cpp
    src/LogMgr.cpp
    src/LogWidget.cpp
    src/LogoRenderer.cpp

    src/miniaudio/AudioMixerBuf.cpp
    src/miniaudio/AudioSampleBuf.cpp
    src/miniaudio/AudioTestToneGenerator.cpp
    src/miniaudio/AudioUtils.cpp

    src/miniaudio/MiniAudioDevices.cpp
    src/miniaudio/MiniAudioIn.cpp
    src/miniaudio/MiniAudioInDevice.cpp
    src/miniaudio/MiniAudioOut.cpp
    src/miniaudio/MiniAudioOutDevice.cpp
    src/miniaudio/miniaudio-link.cpp

    src/miniaudio/TestFileWav.cpp
    src/miniaudio/TestFileWavMgr.cpp

    src/MessengerPage.cpp
    src/MultiSessionState.cpp
    src/MyIcons.cpp
    src/NetAvailStatusWidget.cpp
    src/OfferBarWidget.cpp
    src/OfferSendWidget.cpp
    src/OfferSessionLogic.cpp
    src/PermissionWidget.cpp
    src/PermissionListItem.cpp
    src/PersonOfferListItem.cpp
    src/PersonOfferListWidget.cpp
    src/PhoneRinger.cpp
    src/PlayControlWidget.cpp
    src/PluginSettingsWidget.cpp
    src/RenderGlLogic.cpp
    src/RenderGlMatrix.cpp
    src/RenderGlOffScreenSurface.cpp
    src/RenderGlWidget.cpp
    src/RenderGlWidgetFonts.cpp
    src/RenderGlWidgetRender.cpp
    src/RenderGlWidgetShaderInterface.cpp
    src/RenderGlWidgetTextures.cpp
    src/RenderGlShaders.cpp
    src/RenderLogoShaders.cpp
    src/RenderPlayerNlcThread.cpp
    src/RenderShaderDefsGl.cpp
    src/RenderShaderQt.cpp
    src/RichTextWidget.cpp
    src/SearchBarWidget.cpp
    src/SearchParamsWidget.cpp
    src/SessionWidget.cpp
    src/ServiceSettingsWidget.cpp
    src/SoundFxMgr.cpp
    src/StoryWidget.cpp

    src/TestWidget.cpp
    src/TextChatLogic.cpp
    src/TitleBarWidget.cpp
    src/TodGameDefs.cpp
    src/TodGameLogic.cpp
    src/TodGameMgr.cpp
    src/TodGameWidget.cpp
    src/TodStatsWidget.cpp
    src/ThumbnailEditWidget.cpp
    src/ThumbnailChooseWidget.cpp
    src/ThumbnailViewWidget.cpp

    src/VxAspectFrame.cpp
    src/VxComboBox.cpp
    src/VxContextMenu.cpp
    src/VxAvatarImage.cpp
    src/VxFrame.cpp
    src/VxMenuButton.cpp
    src/VxMenuButtonBottom.cpp
    src/VxMenuButtonTop.cpp
    src/VxPushButton.cpp
    src/VxLabel.cpp
    src/VxSndInstance.cpp
    src/VxResourceToRealFile.cpp
    src/VxScrollArea.cpp
    src/VxSpinProgress.cpp
    src/VxTagLabel.cpp
    src/VxTilePositioner.cpp
    src/VidWidget.cpp
    src/VxWidgetBase.cpp
    src/VxMenu.cpp
    src/VxAppStyle.cpp
    src/VxAppTheme.cpp
    src/VxAppDisplay.cpp
    src/VxPlainTextEdit.cpp
    src/WaitingSpinnerWidget.cpp
    src/WebsiteWidget.cpp
    src/VirtStream/VirtProviderFile.cpp
    src/VirtStream/VirtStorageProvider.cpp
    src/VirtStream/VirtStreamCache.cpp
    src/VirtStream/VirtStreamFile.cpp
    src/VirtStream/VirtStreamMgr.cpp
    src/VirtStream/VirtStreamMgrFileIo.cpp
    src/VirtStream/VirtStreamMgrProvider.cpp
    ../libs/GuiInterface/ILog.cpp
    ../libs/GuiInterface/IDefs.cpp
   )

set(nlc_FORMS)
list(APPEND nlc_FORMS
    src/Forms/ActionWidget.ui
    src/Forms/ActivityBrowseFiles.ui
    src/Forms/ActivityDownloadItemMenu.ui

    src/Forms/ActivityGenerateHash.ui
    src/Forms/ActivityMessageBox.ui
    src/Forms/ActivityNetworkStartupTest.ui
    src/Forms/ActivitySessionOptions.ui
    src/Forms/ActivityShowHelp.ui
    src/Forms/ActivityTimedMessage.ui
    src/Forms/ActivityYesNo.ui

    src/Forms/AcceptCancelWidget.ui
    src/Forms/AppletAboutApp.ui
    src/Forms/AppletAboutFile.ui
    src/Forms/AppletAboutMeClient.ui
    src/Forms/AppletAboutUser.ui

    src/Forms/AppletApplicationInfo.ui
    src/Forms/AppletBrowseFiles.ui
    src/Forms/AppletCamClient.ui
    src/Forms/AppletCamSettings.ui
    src/Forms/AppletChatRoomJoinSearch.ui
    src/Forms/AppletChatRoomListLocalView.ui

    src/Forms/AppletChooseThumbnail.ui
    src/Forms/AppletCreateAccount.ui
    src/Forms/AppletDownloads.ui
    src/Forms/AppletEditAboutMe.ui
    src/Forms/AppletEditAvatarImage.ui
    src/Forms/AppletEditStoryBoard.ui
    src/Forms/AppletFileOfferSelect.ui
    src/Forms/AppletFileShareClientView.ui
    src/Forms/AppletFriendList.ui
    src/Forms/AppletFriendRequest.ui
    src/Forms/AppletFriendRequestList.ui
    src/Forms/AppletGalleryImage.ui
    src/Forms/AppletGalleryEmoticon.ui
    src/Forms/AppletGalleryThumb.ui
    src/Forms/AppletGetStarted.ui

    src/Forms/AppletGroupJoinSearch.ui
    src/Forms/AppletGroupListClient.ui
    src/Forms/AppletGroupListLocalView.ui
    src/Forms/AppletHackerList.ui
    src/Forms/AppletHelpNetSignalBars.ui

    src/Forms/AppletHostClient.ui
    src/Forms/AppletHostJoinChoose.ui
    src/Forms/AppletHostJoinConnect.ui
    src/Forms/AppletHostJoinRequestList.ui
    src/Forms/AppletHostLeave.ui
    src/Forms/AppletHostNetworkStatus.ui

    src/Forms/AppletIgnoredHosts.ui
    src/Forms/AppletInformation.ui
    src/Forms/AppletInviteAccept.ui
    src/Forms/AppletInviteCreate.ui
    src/Forms/AppletIsPortOpenTest.ui

    src/Forms/AppletJoinHostList.ui
    src/Forms/AppletLanguageSelect.ui
    src/Forms/AppletLaunchWidget.ui
    src/Forms/AppletLibrary.ui
    src/Forms/AppletLog.ui
    src/Forms/AppletLogSettings.ui

    src/Forms/AppletMultiMessenger.ui
    src/Forms/AppletNetworkKey.ui
    src/Forms/AppletNetworkSettings.ui
    src/Forms/AppletOfferInfo.ui
    src/Forms/AppletOfferList.ui

    src/Forms/AppletOfferRandSession.ui
    src/Forms/AppletOfferResponse.ui
    src/Forms/AppletOfferResponseAccept.ui
    src/Forms/AppletOfferSend.ui
    src/Forms/AppletOfferView.ui
    src/Forms/AppletPersonOfferList.ui
    src/Forms/AppletPeerChangeFriendship.ui
    src/Forms/AppletPeerReplyOfferFile.ui
    src/Forms/AppletPeerSessionFileOffer.ui
    src/Forms/AppletPeerTodGame.ui
    src/Forms/AppletPeerVideoPhone.ui
    src/Forms/AppletPeerVoicePhone.ui
    src/Forms/AppletPermissionList.ui
    src/Forms/AppletPersonalRecorder.ui
    src/Forms/AppletPlayerCamClip.ui
    src/Forms/AppletPlayerPhoto.ui
    src/Forms/AppletPlayerNlc.ui
    src/Forms/AppletPlayerVideo.ui
    src/Forms/AppletPopupMenu.ui

    src/Forms/AppletRandomConnectJoinSearch.ui
    src/Forms/AppletRandomConnectListLocalView.ui
    src/Forms/AppletServiceBase.ui
    src/Forms/AppletServiceBaseSettings.ui
    src/Forms/AppletSettingsHostBase.ui
    src/Forms/AppletSettingsNoUserSettings.ui
    src/Forms/AppletSnapshot.ui
    src/Forms/AppletSocketList.ui
    src/Forms/AppletSoundSettings.ui
    src/Forms/AppletStoryBoardClient.ui
    src/Forms/AppletTestAndDebug.ui
    src/Forms/AppletTestUpnp.ui
    src/Forms/AppletTheme.ui
    src/Forms/AppletUploads.ui
    src/Forms/AppletUserConnections.ui
    src/Forms/AppletUserIdentity.ui
    src/Forms/AppletUserPreferences.ui
    src/Forms/AssetFaceWidget.ui
    src/Forms/AssetPhotoWidget.ui
    src/Forms/AssetTextWidget.ui
    src/Forms/AssetVideoWidget.ui
    src/Forms/AssetVoiceWidget.ui
    src/Forms/AvatarBarWidget.ui
    src/Forms/BottomBarWidget.ui
    src/Forms/CentralFrame.ui
    src/Forms/CentralWidget.ui
    src/Forms/ChatEntryWidget.ui
    src/Forms/DialogAddComment.ui
    src/Forms/DialogConfirmRemoveMessage.ui
    src/Forms/FileActionBarWidget.ui
    src/Forms/FileActionMenu.ui
    src/Forms/FileDisplayWidget.ui
    src/Forms/FileFilterSelectWidget.ui
    src/Forms/FileMediaSelectWidget.ui
    src/Forms/FileShareItemWidget.ui
    #src/Forms/FileViewSharedActionBar.ui
    src/Forms/FileXferWidget.ui
    src/Forms/FriendListDlg.ui
    src/Forms/GuiFriendRequestWidget.ui
    src/Forms/GuiGroupieListItem.ui
    src/Forms/GuiHostedListItem.ui
    src/Forms/GuiUserMultiListWidget.ui
    src/Forms/HostJoinRequestListItem.ui
    src/Forms/HistoryEntryWidget.ui
    src/Forms/IdentWidget.ui
    src/Forms/ImageListEntry.ui
    src/Forms/ImageListRow.ui
    src/Forms/ImageListWidget.ui
    src/Forms/ImagePickBar.ui
    src/Forms/InformationWidget.ui
    src/Forms/InputAllWidget.ui
    src/Forms/InputFaceWidget.ui
    src/Forms/InputPhotoWidget.ui
    src/Forms/InputTextWidget.ui
    src/Forms/InputVideoWidget.ui
    src/Forms/InputVoiceWidget.ui
    src/Forms/InstMsgWidget.ui
    src/Forms/InviteUrlWidget.ui
    src/Forms/LogWidget.ui
    src/Forms/OfferBarWidget.ui
    src/Forms/OfferSendWidget.ui
    src/Forms/PermissionWidget.ui
    src/Forms/PermissionListItemWidget.ui
    src/Forms/PersonOfferListItem.ui
    src/Forms/PlayControlWidget.ui
    src/Forms/PluginSettingsWidget.ui
    src/Forms/SearchBarWidget.ui
    src/Forms/SearchParamsWidget.ui
    src/Forms/ServiceSettingsWidget.ui
    src/Forms/SessionWidget.ui
    src/Forms/StoryWidget.ui
    src/Forms/TestWidget.ui
    src/Forms/ThumbnailEditWidget.ui
    src/Forms/ThumbnailChooseWidget.ui
    src/Forms/TitleBarWidget.ui
    src/Forms/TodGameWidget.ui
    src/Forms/TodStatsWidget.ui
    src/Forms/VidWidget.ui
    src/Forms/WebsiteWidget.ui
    src/NoLimitConnect.qrc
    )

if(DEFINED TARGET_OS_WINDOWS)
    # Define the path to your resource file
    set(EXE_ICON_RESOURCE "src/Resources/resources.rc")
    list(APPEND nlc_SOURCES ${EXE_ICON_RESOURCE})
endif()

qt_add_executable(nolimitconnect
    MANUAL_FINALIZATION
    ${nlc_FORMS}
    ${nlc_SOURCES}
    ${nlc_HEADERS}
    )

file(GLOB nlc_TS_FILES CONFIGURE_DEPENDS
    "${PROJECT_SRC_ROOT_DIR}/translations/*.ts"
    "${CMAKE_SOURCE_DIR}/translations/*.ts"
)
list(REMOVE_DUPLICATES nlc_TS_FILES)

set(nlc_QM_FILES)
set(nlc_LUPDATE_EXECUTABLE "")
set(nlc_LRELEASE_EXECUTABLE "")

set(nlc_QT_BIN_HINTS)
if(DEFINED Qt6_DIR)
    get_filename_component(nlc_qt_cmake_dir "${Qt6_DIR}" ABSOLUTE)
    get_filename_component(nlc_qt_root_dir "${nlc_qt_cmake_dir}/../../.." ABSOLUTE)
    list(APPEND nlc_QT_BIN_HINTS "${nlc_qt_root_dir}/bin")
endif()
if(DEFINED QT_QMAKE_EXECUTABLE)
    get_filename_component(nlc_qmake_bin_dir "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    list(APPEND nlc_QT_BIN_HINTS "${nlc_qmake_bin_dir}")
endif()
if(CMAKE_PREFIX_PATH)
    list(GET CMAKE_PREFIX_PATH 0 nlc_qt_prefix_path)
    list(APPEND nlc_QT_BIN_HINTS "${nlc_qt_prefix_path}/bin")
endif()
list(REMOVE_DUPLICATES nlc_QT_BIN_HINTS)

foreach(nlc_qt_bin_dir IN LISTS nlc_QT_BIN_HINTS)
    if(NOT nlc_LUPDATE_EXECUTABLE AND EXISTS "${nlc_qt_bin_dir}/lupdate.exe")
        set(nlc_LUPDATE_EXECUTABLE "${nlc_qt_bin_dir}/lupdate.exe")
    endif()

    if(NOT nlc_LRELEASE_EXECUTABLE AND EXISTS "${nlc_qt_bin_dir}/lrelease.exe")
        set(nlc_LRELEASE_EXECUTABLE "${nlc_qt_bin_dir}/lrelease.exe")
    endif()
endforeach()

if(nlc_TS_FILES AND nlc_LUPDATE_EXECUTABLE AND nlc_LRELEASE_EXECUTABLE)
    foreach(tsFile IN LISTS nlc_TS_FILES)
        get_filename_component(tsFileName "${tsFile}" NAME_WE)
        set(qmFile "${PROJECT_SRC_ROOT_DIR}/translations/${tsFileName}.qm")
        add_custom_command(
            OUTPUT "${qmFile}"
            COMMAND "${nlc_LRELEASE_EXECUTABLE}" "${tsFile}" -qm "${qmFile}"
            DEPENDS "${tsFile}"
            VERBATIM
        )
        list(APPEND nlc_QM_FILES "${qmFile}")
    endforeach()

    add_custom_target(nolimitconnect_lupdate
        COMMAND "${nlc_LUPDATE_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/src"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/Forms"
            -ts ${nlc_TS_FILES}
        WORKING_DIRECTORY "${PROJECT_SRC_ROOT_DIR}"
        VERBATIM
    )

    add_custom_target(nolimitconnect_translations ALL
        DEPENDS ${nlc_QM_FILES}
    )

    add_dependencies(nolimitconnect nolimitconnect_translations)
endif()

target_include_directories(nolimitconnect PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/miniaudio        
)

if(DEFINED BUILD_SYSTEM_FLATPAK)
    # glu is no longer available in flatpak-sdk, so we need to use our own library
    target_include_directories(nolimitconnect PRIVATE
        ${PROJECT_SRC_ROOT_DIR}/libs/libGLU/include       
    )
endif()

if(DEFINED TARGET_OS_LINUX)
    target_compile_definitions(nolimitconnect PRIVATE
        # use v4l2 camera capture on linux instead of qt camera which scrables images when linux is running in a vm
        ENABLE_V4L2_CAM
    )

endif()

if(DEFINED TARGET_OS_WINDOWS)
    target_compile_definitions(nolimitconnect PRIVATE
        NOMINMAX
        WIN64
        WIN32_LEAN_AND_MEAN
        _MBCS
        __STDC_CONSTANT_MACROS
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )

endif()

if(DEFINED TARGET_OS_ANDROID)
    message(STATUS "nolimitconnect cpu ${NLC_CPU}")
    if(${NLC_CPU} STREQUAL "CPU_ARM32")
        message(STATUS "nolimitconnect api armeabi-v7a")
        set_target_properties(nolimitconnect PROPERTIES
            ANDROID_TARGET_ARCH armeabi-v7a
            QT_ANDROID_PACKAGE_SOURCE_DIR "${PROJECT_SRC_ROOT_DIR}/nolimitgui/android"
            QT_ANDROID_ABIS armeabi-v7a
            CMAKE_ANDROID_ASSETS_DIRECTORIES  "${PROJECT_SRC_ROOT_DIR}/bin-Android/assets"
            )
    else()
        message(STATUS "nolimitconnect api arm64-v8a")
        set_target_properties(nolimitconnect PROPERTIES
            ANDROID_TARGET_ARCH arm64-v8a
            QT_ANDROID_PACKAGE_SOURCE_DIR "${PROJECT_SRC_ROOT_DIR}/nolimitgui/android"
            QT_ANDROID_ABIS arm64-v8a
            CMAKE_ANDROID_ASSETS_DIRECTORIES  "${PROJECT_SRC_ROOT_DIR}/bin-Android/assets"
            )
    endif()
else()
    set_target_properties(nolimitconnect PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )
endif()

target_compile_definitions(nolimitconnect
    PRIVATE
    ENABLE_NLC_PLAYER
)

target_include_directories(nolimitconnect PUBLIC
    src
    ../nolimitlib
    ../libs
    ../libs/libplayer-nlc/xbmc
    ../libs/ffmpeg
    ../libs/libechocancel
    ../libs/GuiInterface
    ../libs/libptopengine
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_STATIC_LIB_DIR}
)

# common to all builds
target_link_libraries(nolimitconnect
    PUBLIC
    # Qt libraries
    Qt::Core
    Qt::CorePrivate
    Qt::Gui
    Qt::Multimedia
    Qt::OpenGL
    Qt::Svg
    Qt::Widgets

    # static libraries
    player-nlc
    avdevice
    avfilter
    avformat
    avcodec
    postproc
    swresample
    swscale
    avutil
    ffmpegopus

    miniupnpc

    ptopengine
    netlib

    pktlib

    wav-decoder
    rnnoise
)

if(USE_AEC2)
    target_link_libraries(nolimitconnect
        PUBLIC
        webrtc-aec2 # webrtc audio echo cancel
        #absl::strings # absl support for webrtc audio echo cancel
        #absl::base # absl support for webrtc audio echo cancel
    )
else()
    target_link_libraries(nolimitconnect
        PUBLIC
        webrtc-aec # webrtc audio echo cancel
        absl::strings # absl support for webrtc audio echo cancel
        absl::base # absl support for webrtc audio echo cancel
    )
endif()

if (DEFINED TARGET_OS_WINDOWS)
    target_link_libraries(nolimitconnect
        PUBLIC

        x264
        x265
        dav1d
        vpx

        pcre
        opencore-amr

        mediatools
        speex
        lame
        vorbis2

        gnu
        ass
        fribidi
        freetype

        iconv
        vxvideolib

        tag
        theora
        twolame

        tinyxml
        xml2

        lzma
        lzo
        zip
        CoreLib
        zlib
        crossguid
        pthread
        ogg

        opengl32
        glu32
        ole32
        winmm
        Rpcrt4
        ws2_32
        iphlpapi    # Fixes the unresolved symbols from miniupnpc
        dwmapi      # Fixes DwmIsCompositionEnabled from player-nlc
        cfgmgr32    # Fixes CM_Get_DevNode_Status from player-nlc
        Setupapi    # Fixes SetupDiGetClassDevsA and related functions from player-nlc
        glew        # Fixes glew32s static linking on Windows used by OverlayRendererQt.cpp from player-nlc
    )

endif()

if (DEFINED TARGET_OS_LINUX)
    if (DEFINED TARGET_CPU_ARM64)

        target_link_libraries(nolimitconnect
            PUBLIC
            x264
            x265
            dav1d
            vpx
            pcre

            opencore-amr
            mediatools
            speex
            lame
            vorbis2
            ogg

            gnu
            ass
            fribidi
            freetype

            iconv

            vxvideolib
            tag
            theora
            twolame


            tinyxml
            xml2

            lzma
            lzo
            zip
            CoreLib
            zlib
            crossguid
            miniupnpc

            pthread
            dl
            GLU
            GL
            m
            uuid
            rt
        )
    else()
        target_link_libraries(nolimitconnect
            PUBLIC
            x264
            x265
            dav1d
            vpx
            pcre

            opencore-amr
            mediatools
            speex
            lame
            vorbis2
            ogg

            gnu
            ass
            fribidi
            freetype

            iconv

            vxvideolib
            tag
            theora
            twolame

            freetype

            tinyxml
            xml2

            lzma
            lzo
            zip
            CoreLib
            zlib
            crossguid
            miniupnpc

            pthread
            dl
            GLU
            GL
            m
            uuid
            rt
        )
    endif()

elseif (DEFINED TARGET_OS_ANDROID)
    find_package(Qt6 COMPONENTS Gui REQUIRED)
    if (DEFINED TARGET_CPU_ARM64)
        target_link_libraries(nolimitconnect
            PUBLIC
            # static nlc libraries

            x264
            x265
            dav1d
            vpx
            pcre

            opencore-amr
            mediatools
            speex
            lame
            vorbis2

            gnu
            ass
            fribidi
            freetype

            iconv

            vxvideolib
            speex
            tag
            theora
            twolame

            ogg
            netlib
            pktlib

            fribidi
            freetype

            tinyxml
            xml2

            lzma
            lzo
            zip
            CoreLib
            zlib
            crossguid
            miniupnpc

            dl
            m
            EGL
            GLESv2
            c
            stdc++
            jnigraphics
            android
        )
    elseif(DEFINED TARGET_CPU_ARM32)
        target_link_libraries(nolimitconnect
            PUBLIC
            # static nlc libraries

            x264
            #x265 #cannot compile x265 on arm7a
            dav1d
            vpx
            pcre
            opencore-amr

            mediatools
            speex
            lame
            vorbis2

            gnu
            ass
            fribidi
            freetype

            iconv

            vxvideolib
            speex
            tag
            theora
            twolame


            freetype

            tinyxml
            xml2

            lzma
            lzo
            zip
            CoreLib
            zlib
            crossguid
            miniupnpc

            dl
            m
            EGL
            GLESv2
            c
            stdc++
            jnigraphics
            android
        )
    endif()

elseif (DEFINED TARGET_OS_WINDOWS)
    # do nothing .. was already linked
else()
    message(FATAL_ERROR "Unknown Target OS Defined for target_link_libraries")
endif()

if(USE_LIBJPEG_TURBO)
    target_link_libraries(nolimitconnect
        PUBLIC
        jpegturbo
    )
else()
    target_link_libraries(nolimitconnect
        PUBLIC
        jpg
    )
endif()

qt_finalize_target(nolimitconnect)

if(DEFINED FLATPAKBUILD)


    #set(INSTALL_DIR "") # empty to not complicate run command

    #install(TARGETS nolimitconnect
    #    RUNTIME DESTINATION "${INSTALL_DIR}"
    #    BUNDLE DESTINATION "${INSTALL_DIR}"
    #    LIBRARY DESTINATION "${INSTALL_DIR}"
    #)

    # Install executeable
    install(
        TARGETS nolimitconnect
        DESTINATION bin/
    )

    # Install desktop file
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/com.nolimitconnect.NoLimitConnect.desktop
        DESTINATION share/applications
    )

    # And install all the icons
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/16x16/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/16x16/apps/
    )
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/32x32/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/32x32/apps/
    )
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/48x48/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/48x48/apps/
    )
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/64x64/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/64x64/apps/
    )
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/128x128/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/128x128/apps/
    )
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/256x256/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/256x256/apps/
    )
    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/icons/512x512/com.nolimitconnect.NoLimitConnect.png
        DESTINATION share/icons/hicolor/512x512/apps/
    )
    # the max allowed icon size for flathub is 512x512

    install(
        FILES ${PROJECT_SRC_ROOT_DIR}/com.nolimitconnect.NoLimitConnect.metainfo.xml
        DESTINATION share/metainfo/
    )

else()
    message(STATUS "Post build CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
    message(STATUS "Post build CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

    add_custom_command(TARGET nolimitconnect POST_BUILD
        COMMAND echo "echo CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

    if(DEFINED TARGET_OS_NONE) # CPACK is no longer used. flatpak is used for linux insted
        #=== after compile add qt libraries to bin-TargetOs directory ===#
        file(MAKE_DIRECTORY "${PROJECT_BIN_DIR}/usr/lib")

        #move requred qt package into bin/usr/lib for packaging Core Gui Multimedia OpenGL Svg Widgets)
        message(STATUS "POST_BUID_TARGET copy shared qt libraries from ${CMAKE_PREFIX_PATH}/lib")
        add_custom_command(POST_BUILD TARGET nolimitconnect
            WORKING_DIRECTORY "${PROJECT_BIN_DIR}"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_PREFIX_PATH}/lib/libQt6Core.so*" "${PROJECT_BIN_DIR}/usr/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_PREFIX_PATH}/lib/libQt6Gui.so*" "${PROJECT_BIN_DIR}/usr/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_PREFIX_PATH}/lib/libQt6Multimedia.so*" "${PROJECT_BIN_DIR}/usr/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_PREFIX_PATH}/lib/libQt6OpenGL.so*" "${PROJECT_BIN_DIR}/usr/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_PREFIX_PATH}/lib/libQt6Svg.so*" "${PROJECT_BIN_DIR}/usr/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_PREFIX_PATH}/lib/libQt6Widgets.so*" "${PROJECT_BIN_DIR}/usr/lib"
            COMMAND_EXPAND_LISTS
            )


        #=== after qt libraries are copied then make installer with cpack ===#
        message(STATUS "Setting up CPACK")
        # common to all packages
        set(CPACK_PACKAGE_NAME "${NLC_NAME}")
        set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
        set(CMAKE_PROJECT_HOMEPAGE_URL  "${PROJECT_WEBSITE_URL}")
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
            "No Limit Connect - A Open Source Social Network")
        set(CPACK_PACKAGE_VERSION "${NLC_VERSION}")
        set(CPACK_PACKAGE_VERSION_MAJOR "${NLC_VERSION_MAJOR}")
        set(CPACK_PACKAGE_VERSION_MINOR "${NLC_VERSION_MINOR}")
        set(CPACK_PACKAGE_VERSION_PATCH "${NLC_VERSION_PATCH}")

        set(CPACK_SET_DESTDIR TRUE)
        set(CPACK_RESOURCE_FILE_README "${PROJECT_SRC_ROOT_DIR}/README.md")
        set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SRC_ROOT_DIR}/LICENSE")

        set(CPACK_PACKAGE_DIRECTORY "${PROJECT_PACKAGE_DIR}")


        # generate postinst file in ${CMAKE_BINARY_DIR} from template                        #
        # CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/contrib/postinst.in" "postinst" @ONLY IMMEDIATE)


        if(DEFINED TARGET_OS_LINUX)
            message(STATUS "Creating debian package")
            set(CPACK_GENERATOR "DEB")
            include(GNUInstallDirs)
            set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PROJECT_MAINTAINER}")
            set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpthread, libGL, libGLU libuuid")
            INSTALL( DIRECTORY "${PROJECT_ASSETS_DIR}" DESTINATION usr )


        elseif(DEFINED TARGET_OS_WINDOWS)
            set(CPACK_GENERATOR "NSIS")
            set(CPACK_NSIS_MUI_ICON "${PROJECT_SRC_ROOT_DIR}/nolimitgui/src/Resources/NlcP2P.ico")

        endif()

        # This must always be after all CPACK\_\* variables are defined
        include(CPack)
        message(STATUS "CPACK complete")
    endif()

    # if(DEFINED TARGET_OS_ANDROID)
    #     if(DEFINED DO_MANUAL)
    #         if(MANUAL_PACKAGE)
    #             file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/nolimitgui/android-build")
    #             set(APK_ARGS "--input ${CMAKE_BINARY_DIR}/nolimitgui/android-nolimitconnect-deployment-settings.json --output ${CMAKE_BINARY_DIR}/nolimitgui/android-build --android-platform android-34 --jdk $ENV{JAVA_HOME} --verbose --gradle")
    #             message(STATUS "Post build MANUAL_PACKAGE is defined ${APK_ARGS}")
    #             if(CI_PIPELINE)
    #                 # androiddeployqt is in path
    #                 message(STATUS "Create Apk  androiddeployqt ${APK_ARGS}")
    #                 #add_custom_command(POST_BUILD TARGET nolimitconnect

    #                 add_custom_target(nolimitappsEchoDeploy ALL
    #                     COMMAND echo "echo create apk androiddeployqt ${APK_ARGS}")
    #                 add_custom_target(nolimitappsEchoDeploy ALL
    #                     COMMAND androiddeployqt "${APK_ARGS}")
    #             else()
    #                 # need full path to androiddeployqt
    #                 if(FLATPAKBUILD)
    #                     message("** Enabling Flatpak workarounds")
    #                     add_definitions(-DPQMFLATPAKBUILD)
    #                     include_directories(${GLIB_INCLUDE_DIRS})
    #                     target_link_libraries(nolimitconnect PRIVATE ${GLIB_LIBRARIES})
    #                     target_link_libraries(nolimitconnect PRIVATE "gobject-2.0")
    #                     target_link_libraries(nolimitconnect PRIVATE "gio-2.0")
    #                 endif()
    #                 message(STATUS "Create Apk ${USER_HOME_DIR}/Qt/${NLC_QT_VER}/gcc_64/bin/androiddeployqt ${APK_ARGS}")
    #                 add_custom_target(nolimitappsEchoDeploy ALL
    #                     COMMAND echo "echo create apk ${USER_HOME_DIR}/Qt/${NLC_QT_VER}/gcc_64/bin/androiddeployqt ${APK_ARGS}")
    #                 add_custom_target(nolimitappsEchoDeploy ALL
    #                     COMMAND "${USER_HOME_DIR}/Qt/${NLC_QT_VER}/gcc_64/bin/androiddeployqt" "${APK_ARGS}")
    #             endif()
    #         else()
    #             message(STATUS "Post build MANUAL_PACKAGE is NOT defined")
    #             add_custom_target(nolimitappsEchoDeploy ALL
    #                 COMMAND echo "create apk not required")
    #         endif() # if(MANUAL_PACKAGE)
    #     endif()

    #     # copy package to package directory
    #     set(NLC_APK_ROOT_DIR "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/apk/")
    #     if (CMAKE_BUILD_TYPE MATCHES Debug)
    #         # for debugging only use apk
    #         message(STATUS "POST_BUID_TARGET copy debug android packages from ${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/apk/debug to package directory ${PROJECT_PACKAGE_DIR}")
    #         add_custom_target(nolimitappsCopyDebugApk
    #             COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/apk/debug" "${PROJECT_PACKAGE_DIR}"
    #             COMMAND_EXPAND_LISTS
    #             )
    #         add_dependencies(nolimitappsCopyDebugApk nolimitconnect)

    #     else()
    #         # for release android play store requires aab bundle
    #         message(STATUS "POST_BUID_TARGET copy release android packages from ${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/bundle/release to package directory ${PROJECT_PACKAGE_DIR}")
    #         if(EXISTS "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/bundle/release")
    #             add_custom_target(nolimitappsCopyReleaseBundle
    #                 COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/apk/release" "${PROJECT_PACKAGE_DIR}"
    #                 COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/bundle/release" "${PROJECT_PACKAGE_DIR}"
    #                 COMMAND_EXPAND_LISTS
    #                 )
    #             add_dependencies(nolimitappsCopyReleaseBundlenolimitconnect)
    #         else()
    #             add_custom_target(nolimitappsCopyReleaseApk
    #                 COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/apk/release" "${PROJECT_PACKAGE_DIR}"
    #                 DEPENDS "${CMAKE_BINARY_DIR}/nolimitgui/android-build/build/outputs/apk/release/android-build-release.apk"
    #                 COMMAND_EXPAND_LISTS
    #             )
    #         add_dependencies(nolimitappsCopyReleaseApk nolimitconnect)
    #         endif() # if EXISTS
    #     endif() # if (CMAKE_BUILD_TYPE MATCHES Debug)
    # endif() #if(DEFINED TARGET_OS_ANDROID)


    add_custom_target(nolimitappsApk
        COMMAND echo "echo CMAKE_BINARY_DIR ***** ${CMAKE_BINARY_DIR}")
    add_dependencies(nolimitappsApk nolimitconnect)

    #=== INSTALL FILES ===#
    if(DEFINED TARGET_OS_LINUX)


    endif()
endif() # NOT DEFINED FLATPAKBUILD

# if android release build then copy apk to /deploy/android
if(ANDROID) # AND CMAKE_BUILD_TYPE STREQUAL "Release")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(APK_OUTPUT_DIR
            ${CMAKE_BINARY_DIR}/nolimitgui/android-build-nolimitconnect/build/outputs/apk/debug
        )
    else()
        set(APK_OUTPUT_DIR
            ${CMAKE_BINARY_DIR}/nolimitgui/android-build-nolimitconnect/build/outputs/apk/release
        )
    endif()

    set(DEPLOY_DIR ${CMAKE_SOURCE_DIR}/deploy/android)
    set(APP_VERSION ${NLC_VERSION})  # your version

    message(STATUS "Copy apk version ${APP_VERSION} to  ${DEPLOY_DIR}")

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Windows APK Copy")
        # Find all APK files
        file(GLOB APK_FILES "${APK_OUTPUT_DIR}/*.apk")

        if(APK_FILES)
            # Take the first APK (adjust if multiple ABIs)
            list(GET APK_FILES 0 APK_TO_COPY)

            add_custom_command(
                TARGET nolimitconnect
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_DIR}
                COMMAND ${CMAKE_COMMAND} -E echo "Copying Release APK: ${APK_TO_COPY}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${APK_TO_COPY}"
                        "${DEPLOY_DIR}/nolimitconnect_v${APP_VERSION}.apk"
                COMMENT "Copy Release APK to deploy/android with version"
            )
        else()
            message(STATUS "No APK found in ${APK_OUTPUT_DIR}. Skipping copy.")
        endif()

    else()
        message(STATUS "LINUX APK Copy")
        add_custom_command(
            TARGET nolimitconnect
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    ${CMAKE_SOURCE_DIR}/deploy/android
            COMMAND ${CMAKE_COMMAND} -E echo "Checking for Release APK..."
            COMMAND ${CMAKE_COMMAND} -E
                sh -c
                "if ls \"${CMAKE_BINARY_DIR}/android-build/build/outputs/apk/release\"/*.apk 1>nul 2>nul; then \
                    ${CMAKE_COMMAND} -E copy_if_different \
                        \"${CMAKE_BINARY_DIR}/android-build/build/outputs/apk/release\"/*.apk \
                        \"${CMAKE_SOURCE_DIR}/deploy/android\"; \
                 else \
                    echo \"Release APK not found yet (Gradle not run)\"; \
                 fi"
            COMMENT "Copy Release APK to deploy/android (if present)"
        )

    endif()
endif()


