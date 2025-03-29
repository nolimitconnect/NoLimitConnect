
project(nolimitconnect, LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_STATIC_LIB_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia OpenGL Svg Widgets)

set (_allTargets "")

macro(add_executable _target)
    _add_executable (${_target} ${ARGN})
    list (APPEND _allTargets ${_target})
endmacro()

#=== SETUP EXECUTABLE ===#
message(STATUS "nolimitconnect gui project prefix ${CMAKE_PREFIX_PATH}")
# Add the forms directory to the AUOUIC search paths
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS
    CommonSrc/Forms
    )

# Add the executable

set(nlc_HEADERS)
list(APPEND nlc_HEADERS
    CommonSrc/QtSource/AccountMgr.h
    AppCompilerConfig.h
    AppCpuArchDefines.h
    AppDependLibrariesConfig.h
    AppTargetOsConfig.h
    CommonSrc/QtSource/AppCommon.h
    CommonSrc/QtSource/AccountDb.h
    CommonSrc/QtSource/AccountMgr.h
    CommonSrc/QtSource/AcceptCancelWidget.h

    CommonSrc/QtSource/ActivityBrowseFiles.h
    CommonSrc/QtSource/ActivityCreateAccount.h
    CommonSrc/QtSource/ActivityDownloadItemMenu.h
    CommonSrc/QtSource/ActivityFileSearch.h
    CommonSrc/QtSource/ActivityGenerateHash.h
    CommonSrc/QtSource/ActivityInformation.h
    CommonSrc/QtSource/ActivityMessageBox.h
    CommonSrc/QtSource/ActivityMsgBoxOk.h
    CommonSrc/QtSource/ActivityMsgBoxYesNo.h
    CommonSrc/QtSource/ActivityScanPeopleSearch.h
    CommonSrc/QtSource/ActivityScanProfiles.h
    CommonSrc/QtSource/ActivityScanStoryBoards.h
    CommonSrc/QtSource/ActivityScanWebCams.h
    CommonSrc/QtSource/ActivitySessionOptions.h
    CommonSrc/QtSource/ActivityShowHelp.h
    CommonSrc/QtSource/ActivityTimedMessage.h
    CommonSrc/QtSource/ActivityBase.h
    
    CommonSrc/QtSource/AppDefs.h
    CommonSrc/QtSource/AppCommon.h
    CommonSrc/QtSource/AppGlobals.h
    CommonSrc/QtSource/AppSettings.h
    CommonSrc/QtSource/AppletAboutApp.h
    CommonSrc/QtSource/AppletAboutMeClient.h
    CommonSrc/QtSource/AppletAboutUser.h
    CommonSrc/QtSource/AppletAboutMeServerViewMine.h
    CommonSrc/QtSource/AppletApplicationInfo.h
    CommonSrc/QtSource/AppletAssetPlayerBase.h
    CommonSrc/QtSource/AppletBase.h
    CommonSrc/QtSource/AppletBrowseFiles.h
    CommonSrc/QtSource/AppletPlayerCamClip.h
    CommonSrc/QtSource/AppletCamClient.h
    CommonSrc/QtSource/AppletCamSettings.h
    CommonSrc/QtSource/AppletCamServerViewMine.h
    CommonSrc/QtSource/AppletChatRoomJoin.h
    CommonSrc/QtSource/AppletChatRoomClient.h
    CommonSrc/QtSource/AppletChatRoomHostAdmin.h
    CommonSrc/QtSource/AppletChatRoomJoinSearch.h
    CommonSrc/QtSource/AppletChatRoomListLocalView.h
    CommonSrc/QtSource/AppletChooseThumbnail.h
    CommonSrc/QtSource/AppletChooseUser.h
    CommonSrc/QtSource/AppletClientAvatarImage.h
    CommonSrc/QtSource/AppletClientBase.h
    CommonSrc/QtSource/AppletRandomConnectClient.h
    CommonSrc/QtSource/AppletDownloads.h
    CommonSrc/QtSource/AppletEditAboutMe.h
    CommonSrc/QtSource/AppletEditAvatarImage.h
    CommonSrc/QtSource/AppletEditStoryboard.h
    CommonSrc/QtSource/AppletFileOfferSelect.h
    CommonSrc/QtSource/AppletFileShareClientView.h
    CommonSrc/QtSource/AppletFileShareServerViewMine.h
    CommonSrc/QtSource/AppletFriendList.h
    CommonSrc/QtSource/AppletFriendRequest.h
    CommonSrc/QtSource/AppletGalleryImage.h
    CommonSrc/QtSource/AppletGalleryEmoticon.h
    CommonSrc/QtSource/AppletGalleryThumb.h
    CommonSrc/QtSource/AppletGetStarted.h
    CommonSrc/QtSource/AppletGroupClient.h
    CommonSrc/QtSource/AppletGroupHostAdmin.h
    CommonSrc/QtSource/AppletGroupJoinSearch.h
    CommonSrc/QtSource/AppletGroupJoin.h
    CommonSrc/QtSource/AppletGroupListClient.h
    CommonSrc/QtSource/AppletGroupListLocalView.h
    CommonSrc/QtSource/AppletHackerList.h
    CommonSrc/QtSource/AppletHelpNetSignalBars.h
    CommonSrc/QtSource/AppletHostBase.h
    CommonSrc/QtSource/AppletHostChatRoomStatus.h
    CommonSrc/QtSource/AppletHostGroupStatus.h
    CommonSrc/QtSource/AppletHostRandomConnectStatus.h
    CommonSrc/QtSource/AppletHostJoinRequestList.h
    CommonSrc/QtSource/AppletHostNetworkStatus.h
    CommonSrc/QtSource/AppletHostSelect.h
    CommonSrc/QtSource/AppletIgnoredHosts.h
    CommonSrc/QtSource/AppletInvites.h
    CommonSrc/QtSource/AppletInviteAccept.h
    CommonSrc/QtSource/AppletInviteCreate.h
    CommonSrc/QtSource/AppletIsPortOpenTest.h
    CommonSrc/QtSource/AppletJoinBase.h
    CommonSrc/QtSource/AppletLaunchPage.h
    CommonSrc/QtSource/AppletLaunchWidget.h
    CommonSrc/QtSource/AppletLibrary.h
    CommonSrc/QtSource/AppletLog.h
    CommonSrc/QtSource/AppletLogSettings.h
    CommonSrc/QtSource/AppletLogView.h
    CommonSrc/QtSource/AppletMultiMessenger.h
    CommonSrc/QtSource/AppletMgr.h
    CommonSrc/QtSource/AppletNetHostingPage.h
    CommonSrc/QtSource/AppletNetworkSettings.h
    CommonSrc/QtSource/AppletOfferList.h
    CommonSrc/QtSource/AppletOfferResponse.h
    CommonSrc/QtSource/AppletOfferResponseAccept.h
    CommonSrc/QtSource/AppletOfferSend.h
    CommonSrc/QtSource/AppletOfferView.h
    CommonSrc/QtSource/AppletPersonOfferList.h
    CommonSrc/QtSource/AppletPeerBase.h
    CommonSrc/QtSource/AppletPeerChangeFriendship.h
    CommonSrc/QtSource/AppletPeerReplyFileOffer.h
    CommonSrc/QtSource/AppletPeerSessionFileOffer.h
    CommonSrc/QtSource/AppletPeerTodGame.h
    CommonSrc/QtSource/AppletPeerVideoPhone.h
    CommonSrc/QtSource/AppletPeerVoicePhone.h
    CommonSrc/QtSource/AppletPermissionList.h
    CommonSrc/QtSource/AppletPersonalRecorder.h
    CommonSrc/QtSource/AppletPlayerBase.h
    CommonSrc/QtSource/AppletPlayerPhoto.h
    CommonSrc/QtSource/AppletPlayerVideo.h
    CommonSrc/QtSource/AppletPlayerNlc.h
    CommonSrc/QtSource/AppletPlayerNlcBase.h

    CommonSrc/QtSource/AppletPopupMenu.h
    CommonSrc/QtSource/AppletRandomConnectHostAdmin.h
    CommonSrc/QtSource/AppletRandomConnectJoin.h
    CommonSrc/QtSource/AppletRandomConnectJoinSearch.h
    CommonSrc/QtSource/AppletRandomConnectListLocalView.h
    CommonSrc/QtSource/AppletSearchPage.h
    CommonSrc/QtSource/AppletServiceAboutMe.h

    CommonSrc/QtSource/AppletServiceBase.h
    CommonSrc/QtSource/AppletServiceBaseSettings.h
    CommonSrc/QtSource/AppletServiceConnectionTest.h
    CommonSrc/QtSource/AppletServiceHostNetwork.h
    CommonSrc/QtSource/AppletServiceShareFiles.h
    CommonSrc/QtSource/AppletServiceShareWebCam.h
    CommonSrc/QtSource/AppletServiceStoryboard.h
    CommonSrc/QtSource/AppletSettingsAboutMe.h
    CommonSrc/QtSource/AppletSettingsAvatarImage.h
    CommonSrc/QtSource/AppletSettingsBase.h
    CommonSrc/QtSource/AppletSettingsConnectionTest.h
    CommonSrc/QtSource/AppletSettingsFileXfer.h
    CommonSrc/QtSource/AppletSettingsFriendRequest.h
    CommonSrc/QtSource/AppletSettingsHostBase.h
    CommonSrc/QtSource/AppletSettingsHostChatRoom.h
    CommonSrc/QtSource/AppletSettingsHostGroup.h
    CommonSrc/QtSource/AppletSettingsHostNetwork.h
    CommonSrc/QtSource/AppletSettingsHostRandomConnect.h
    CommonSrc/QtSource/AppletSettingsMessenger.h
    CommonSrc/QtSource/AppletSettingsPage.h
    CommonSrc/QtSource/AppletSettingsPushToTalk.h
    CommonSrc/QtSource/AppletSettingsRandomConnect.h
    CommonSrc/QtSource/AppletSettingsShareFiles.h
    CommonSrc/QtSource/AppletSettingsShareWebCam.h
    CommonSrc/QtSource/AppletSettingsStoryboard.h
    CommonSrc/QtSource/AppletSettingsTruthOrDare.h
    CommonSrc/QtSource/AppletSettingsVideoPhone.h
    CommonSrc/QtSource/AppletSettingsVoicePhone.h
    CommonSrc/QtSource/AppletShareServicesPage.h
    CommonSrc/QtSource/AppletSnapshot.h
    CommonSrc/QtSource/AppletSocketList.h
    CommonSrc/QtSource/AppletSoundSettings.h
    CommonSrc/QtSource/AppletStoryboardClient.h
    CommonSrc/QtSource/AppletStoryboardServerViewMine.h

    CommonSrc/QtSource/AppletTestAndDebug.h
    CommonSrc/QtSource/AppletTestBase.h
    CommonSrc/QtSource/AppletTestHostClient.h
    CommonSrc/QtSource/AppletTestHostService.h
    CommonSrc/QtSource/AppletTestUpnp.h

    CommonSrc/QtSource/AppletTheme.h
    CommonSrc/QtSource/AppletUploads.h
    CommonSrc/QtSource/AppletUserConnections.h
    CommonSrc/QtSource/AppletUserIdentity.h
    CommonSrc/QtSource/AppletUserPreferences.h
    CommonSrc/QtSource/AppTranslate.h
    CommonSrc/QtSource/AssetFaceWidget.h
    CommonSrc/QtSource/AssetPhotoWidget.h
    CommonSrc/QtSource/AssetTextWidget.h
    CommonSrc/QtSource/AssetVideoWidget.h
    CommonSrc/QtSource/AssetVoiceWidget.h
    CommonSrc/QtSource/AssetBaseWidget.h
    CommonSrc/QtSource/AvatarBar.h
    CommonSrc/QtSource/BottomBarWidget.h
    CommonSrc/QtSource/CamTitleBarPreview.h
    CommonSrc/QtSource/ChatRoomWidget.h

    CommonSrc/QtSource/CamJavaClient.h
    CommonSrc/QtSource/CamLogic.h
    CommonSrc/QtSource/CamProcessor.h

    CommonSrc/QtSource/ChatEntryWidget.h
    CommonSrc/QtSource/DialogAddComment.h
    CommonSrc/QtSource/DialogConfirmRemoveMessage.h
    CommonSrc/QtSource/EventsQtToNlc.h
    CommonSrc/QtSource/FileItemInfo.h
    CommonSrc/QtSource/FriendList.h
    CommonSrc/QtSource/FriendListDelegate.h
    CommonSrc/QtSource/FileActionBarWidget.h
    CommonSrc/QtSource/FileActionMenu.h
    CommonSrc/QtSource/FileFilterSelectWidget.h
    CommonSrc/QtSource/FileListReplySession.h
    CommonSrc/QtSource/FileMediaSelectWidget.h
    CommonSrc/QtSource/FileShareItemWidget.h   
    CommonSrc/QtSource/FileXferWidget.h

    CommonSrc/QtSource/FriendListEntryWidget.h
    CommonSrc/QtSource/FriendListWidget.h
    CommonSrc/QtSource/GuiConnectIdListMgr.h
    CommonSrc/QtSource/GuiConnectIdListCallback.h
    CommonSrc/QtSource/GuiFileXferMgr.h
    CommonSrc/QtSource/GuiFileXferSession.h
    CommonSrc/QtSource/GuiFavoriteMgr.h
    CommonSrc/QtSource/GuiGroupie.h
    CommonSrc/QtSource/GuiGroupieListCallback.h
    CommonSrc/QtSource/GuiGroupieListItem.h
    CommonSrc/QtSource/GuiGroupieListMgr.h
    CommonSrc/QtSource/GuiGroupieListSession.h
    CommonSrc/QtSource/GuiGroupieListWidget.h
    CommonSrc/QtSource/GuiHelpers.h
    CommonSrc/QtSource/GuiHosted.h
    CommonSrc/QtSource/GuiHostedListCallback.h
    CommonSrc/QtSource/GuiHostedListItem.h
    CommonSrc/QtSource/GuiHostedListMgr.h
    CommonSrc/QtSource/GuiHostedListSession.h
    CommonSrc/QtSource/GuiHostedListWidget.h
    CommonSrc/QtSource/GuiHostJoin.h
    CommonSrc/QtSource/GuiHostJoinCallback.h
    CommonSrc/QtSource/GuiHostJoinList.h
    CommonSrc/QtSource/GuiHostedByMeJoinMgr.h
    CommonSrc/QtSource/GuiHostJoinSession.h
    CommonSrc/QtSource/GuiHostSession.h

    CommonSrc/QtSource/GuiMemberActiveCallback.h
    CommonSrc/QtSource/GuiMemberActiveMgr.h
    CommonSrc/QtSource/GuiOfferCallback.h
    CommonSrc/QtSource/GuiOfferInfo.h
    CommonSrc/QtSource/GuiOfferListItem.h
    CommonSrc/QtSource/GuiOfferListWidget.h
    CommonSrc/QtSource/GuiOfferMgr.h
    CommonSrc/QtSource/GuiOfferMgrBase.h
    CommonSrc/QtSource/GuiOfferSession.h
    CommonSrc/QtSource/GuiOnlineStatusCallback.h
    CommonSrc/QtSource/GuiPushToTalkMgr.h
    CommonSrc/QtSource/GuiParams.h
    CommonSrc/QtSource/GuiPlayerCallback.h
    CommonSrc/QtSource/GuiPlayerMgr.h
    CommonSrc/QtSource/GuiPluginMgr.h
    CommonSrc/QtSource/GuiRandConnectCallback.h
    CommonSrc/QtSource/GuiRandConnectMgr.h
    CommonSrc/QtSource/GuiSendQueueCallback.h
    CommonSrc/QtSource/GuiSendQueueMgr.h
    CommonSrc/QtSource/GuiThumb.h
    CommonSrc/QtSource/GuiThumbList.h
    CommonSrc/QtSource/GuiThumbMgr.h

    CommonSrc/QtSource/GuiThreadAppLoader.h
    CommonSrc/QtSource/GuiThreadMainLoader.h
    CommonSrc/QtSource/GuiThreadSettingsLoader.h

    CommonSrc/QtSource/GuiUser.h
    CommonSrc/QtSource/GuiUserBase.h
    CommonSrc/QtSource/GuiUserJoin.h
    CommonSrc/QtSource/GuiUserJoinCallback.h
    CommonSrc/QtSource/GuiUserJoinList.h
    CommonSrc/QtSource/GuiUserJoinListItem.h
    CommonSrc/QtSource/GuiUserJoinListWidget.h
    CommonSrc/QtSource/GuiUserJoinMgr.h
    CommonSrc/QtSource/GuiUserJoinSession.h
    CommonSrc/QtSource/GuiUserListItem.h
    CommonSrc/QtSource/GuiUserListWidget.h
    CommonSrc/QtSource/GuiUserMgr.h
    CommonSrc/QtSource/GuiUserMultiListWidget.h
    CommonSrc/QtSource/GuiUserSessionBase.h
    CommonSrc/QtSource/GuiUserUpdateCallback.h
    CommonSrc/QtSource/GuiWebPageCallback.h
    CommonSrc/QtSource/GuiWebPageMgr.h
    CommonSrc/QtSource/HistoryEntryWidget.h
    CommonSrc/QtSource/HistoryListWidget.h
    CommonSrc/QtSource/HostedPluginWidget.h
    CommonSrc/QtSource/HostJoinRequestListItem.h
    CommonSrc/QtSource/HostJoinRequestListWidget.h
    CommonSrc/QtSource/HomeWindow.h
    CommonSrc/QtSource/IdentLogicInterface.h
    CommonSrc/QtSource/IdentOfferWidget.h
    CommonSrc/QtSource/IdentWidget.h
    CommonSrc/QtSource/ImageListEntry.h
    CommonSrc/QtSource/ImagePickBar.h
    CommonSrc/QtSource/ImageListRow.h
    CommonSrc/QtSource/ImageListWidget.h
    CommonSrc/QtSource/InformationWidget.h
    CommonSrc/QtSource/InputAllWidget.h
    CommonSrc/QtSource/InputBaseWidget.h
    CommonSrc/QtSource/InputClientBaseCallback.h
    CommonSrc/QtSource/InputClientCallback.h
    CommonSrc/QtSource/InputFaceWidget.h
    CommonSrc/QtSource/InputPhotoWidget.h
    CommonSrc/QtSource/InputTextWidget.h
    CommonSrc/QtSource/InputVideoWidget.h
    CommonSrc/QtSource/InputVoiceWidget.h
    CommonSrc/QtSource/InstMsgWidget.h
    CommonSrc/QtSource/ListWidgetBase.h
    CommonSrc/QtSource/LogMgr.h
    CommonSrc/QtSource/LogoRenderer.h
    CommonSrc/QtSource/LogWidget.h
    CommonSrc/QtSource/MessengerPage.h
    CommonSrc/QtSource/MyIcons.h
    CommonSrc/QtSource/MyIconsDefs.h
    CommonSrc/QtSource/MultiSessionState.h
    CommonSrc/QtSource/NetAvailStatusWidget.h
    CommonSrc/QtSource/OfferBarWidget.h
    CommonSrc/QtSource/OfferSendWidget.h
    CommonSrc/QtSource/OfferSessionLogic.h
    CommonSrc/QtSource/PermissionWidget.h
    CommonSrc/QtSource/PermissionListItem.h
    CommonSrc/QtSource/PersonOfferListItem.h
    CommonSrc/QtSource/PersonOfferListWidget.h
    CommonSrc/QtSource/PhoneRinger.h
    CommonSrc/QtSource/PlayControlWidget.h
    CommonSrc/QtSource/PluginSettingsWidget.h
    CommonSrc/QtSource/RenderGlOffScreenSurface.h
    CommonSrc/QtSource/RenderGlLogic.h
    CommonSrc/QtSource/RenderGlShaders.h
    CommonSrc/QtSource/RenderGlWidget.h
    CommonSrc/QtSource/RenderLogoShaders.h
    CommonSrc/QtSource/RenderPlayerNlcThread.h
    CommonSrc/QtSource/RenderShaderQt.h
    CommonSrc/QtSource/RichTextWidget.h
    CommonSrc/QtSource/SearchBarWidget.h
    CommonSrc/QtSource/SearchParamsWidget.h
    CommonSrc/QtSource/ServiceSettingsWidget.h
    CommonSrc/QtSource/SessionWidget.h
    CommonSrc/QtSource/StoryWidget.h
    CommonSrc/QtSource/miniaudioio/AudioBitrate.h
    CommonSrc/QtSource/miniaudioio/AudioEchoBuf.h
    CommonSrc/QtSource/miniaudioio/AudioEchoCancel.h
    CommonSrc/QtSource/miniaudioio/AudioEchoCancelImpl.h
    CommonSrc/QtSource/miniaudioio/AudioMasterClock.h
    CommonSrc/QtSource/miniaudioio/AudioMixerBuf.h
    CommonSrc/QtSource/miniaudioio/AudioMixerInterface.h
    CommonSrc/QtSource/miniaudioio/AudioSampleBuf.h
    CommonSrc/QtSource/miniaudioio/AudioTestGenerator.h
    CommonSrc/QtSource/miniaudioio/AudioUtils.h
    CommonSrc/QtSource/miniaudioio/MiniAudioDevices.h
    CommonSrc/QtSource/miniaudioio/MiniAudioIn.h
    CommonSrc/QtSource/miniaudioio/MiniAudioInDevice.h
    CommonSrc/QtSource/miniaudioio/MiniAudioMgr.h
    CommonSrc/QtSource/miniaudioio/MiniAudioOut.h
    CommonSrc/QtSource/miniaudioio/MiniAudioOutDevice.h
    CommonSrc/QtSource/SoundDefs.h
    CommonSrc/QtSource/SoundMgr.h
    CommonSrc/QtSource/TestWidget.h
    CommonSrc/QtSource/TextChatLogic.h
    CommonSrc/QtSource/ThumbnailEditWidget.h
    CommonSrc/QtSource/ThumbnailChooseWidget.h
    CommonSrc/QtSource/ThumbnailViewWidget.h
    CommonSrc/QtSource/TodGameDefs.h
    CommonSrc/QtSource/TodGameLogic.h
    CommonSrc/QtSource/TodGameWidget.h
    CommonSrc/QtSource/ToGuiActivityInterface.h
    CommonSrc/QtSource/ToGuiFileXferInterface.h
    CommonSrc/QtSource/ToGuiHardwareControlInterface.h
    CommonSrc/QtSource/ToGuiUserUpdateInterface.h
    CommonSrc/QtSource/ToGuiThumbUpdateInterface.h
    CommonSrc/QtSource/TitleBarWidget.h

    CommonSrc/QtSource/VideoFrameProcessor.h

    CommonSrc/QtSource/VxAspectFrame.h
    CommonSrc/QtSource/VxContextMenu.h
    CommonSrc/QtSource/VxComboBox.h
    CommonSrc/QtSource/VxAvatarImage.h
    CommonSrc/QtSource/VxPushButton.h

    CommonSrc/QtSource/VxLabel.h
    CommonSrc/QtSource/VxSndInstance.h
    CommonSrc/QtSource/VxFrame.h
    CommonSrc/QtSource/VxMenu.h
    CommonSrc/QtSource/VxMenuButton.h
    CommonSrc/QtSource/VxMenuButtonBottom.h
    CommonSrc/QtSource/VxMenuButtonTop.h
    CommonSrc/QtSource/VxResourceToRealFile.h
    CommonSrc/QtSource/VxScrollArea.h
    CommonSrc/QtSource/VxSpinProgress.h
    CommonSrc/QtSource/VxTagLabel.h
    CommonSrc/QtSource/VxTilePositioner.h
    CommonSrc/QtSource/VidWidget.h
    CommonSrc/QtSource/VxWidgetBase.h
    CommonSrc/QtSource/VxAppStyle.h
    CommonSrc/QtSource/VxAppTheme.h
    CommonSrc/QtSource/VxAppDisplay.h
    CommonSrc/QtSource/VxPlainTextEdit.h
    CommonSrc/QtSource/WaitingSpinnerWidget.h
    CommonSrc/QtSource/VirtStream/VirtProviderFile.h
    CommonSrc/QtSource/VirtStream/VirtStorageProvider.h
    CommonSrc/QtSource/VirtStream/VirtStreamCache.h
    CommonSrc/QtSource/VirtStream/VirtStreamFile.h
    CommonSrc/QtSource/VirtStream/VirtStreamMgr.h
   )

set(nlc_SOURCES)
list(APPEND nlc_SOURCES
    NoLimitConnectMain/AppSource/main.cpp
    CommonSrc/QtSource/AccountMgr.cpp
    NoLimitConnectMain/AppSource/main.cpp
    CommonSrc/QtSource/AcceptCancelWidget.cpp
    CommonSrc/QtSource/AccountDb.cpp
    CommonSrc/QtSource/AccountMgr.cpp

    CommonSrc/QtSource/ActivityBase.cpp
    CommonSrc/QtSource/ActivityBrowseFiles.cpp
    CommonSrc/QtSource/ActivityCreateAccount.cpp
    CommonSrc/QtSource/ActivityDownloadItemMenu.cpp
    CommonSrc/QtSource/ActivityFileSearch.cpp
    CommonSrc/QtSource/ActivityGenerateHash.cpp
    CommonSrc/QtSource/ActivityInformation.cpp
    CommonSrc/QtSource/ActivityMessageBox.cpp
    CommonSrc/QtSource/ActivityMsgBoxOk.cpp
    CommonSrc/QtSource/ActivityMsgBoxYesNo.cpp
    CommonSrc/QtSource/ActivityScanPeopleSearch.cpp
    CommonSrc/QtSource/ActivityScanProfiles.cpp
    CommonSrc/QtSource/ActivityScanStoryBoards.cpp
    CommonSrc/QtSource/ActivityScanWebCams.cpp
    CommonSrc/QtSource/ActivitySessionOptions.cpp
    CommonSrc/QtSource/ActivityShowHelp.cpp
    CommonSrc/QtSource/ActivityTimedMessage.cpp
    CommonSrc/QtSource/AppCommon.cpp
    CommonSrc/QtSource/AppCommonAudio.cpp
    CommonSrc/QtSource/AppCommonEvents.cpp
    CommonSrc/QtSource/AppCommonFileMenu.cpp
    CommonSrc/QtSource/AppCommonFileShare.cpp
    CommonSrc/QtSource/AppCommonFriend.cpp
    CommonSrc/QtSource/AppCommonLogin.cpp
    CommonSrc/QtSource/AppCommonMedia.cpp
    CommonSrc/QtSource/AppCommonNotify.cpp
    CommonSrc/QtSource/AppCommonOfferToFriend.cpp
    CommonSrc/QtSource/AppCommonRender.cpp
    CommonSrc/QtSource/AppCommonScan.cpp
    CommonSrc/QtSource/AppCommonSearch.cpp
    CommonSrc/QtSource/AppCommonSignalsAndSlots.cpp
    CommonSrc/QtSource/AppDefs.cpp
    CommonSrc/QtSource/AppGlobals.cpp
    CommonSrc/QtSource/AppModuleState.cpp
    CommonSrc/QtSource/AppSettings.cpp
    CommonSrc/QtSource/AppletAboutApp.cpp
    CommonSrc/QtSource/AppletAboutMeClient.cpp
    CommonSrc/QtSource/AppletAboutUser.cpp
    CommonSrc/QtSource/AppletAboutMeServerViewMine.cpp
    CommonSrc/QtSource/AppletApplicationInfo.cpp
    CommonSrc/QtSource/AppletAssetPlayerBase.cpp
    CommonSrc/QtSource/AppletBase.cpp
    CommonSrc/QtSource/AppletBrowseFiles.cpp
    CommonSrc/QtSource/AppletPlayerCamClip.cpp
    CommonSrc/QtSource/AppletCamClient.cpp
    CommonSrc/QtSource/AppletCamSettings.cpp
    CommonSrc/QtSource/AppletCamServerViewMine.cpp
    CommonSrc/QtSource/AppletChatRoomClient.cpp
    CommonSrc/QtSource/AppletChatRoomHostAdmin.cpp
    CommonSrc/QtSource/AppletChatRoomJoin.cpp
    CommonSrc/QtSource/AppletChatRoomJoinSearch.cpp
    CommonSrc/QtSource/AppletChatRoomListLocalView.cpp
    CommonSrc/QtSource/AppletChooseThumbnail.cpp
    CommonSrc/QtSource/AppletChooseUser.cpp
    CommonSrc/QtSource/AppletClientAvatarImage.cpp
    CommonSrc/QtSource/AppletClientBase.cpp
    CommonSrc/QtSource/AppletRandomConnectClient.cpp
    CommonSrc/QtSource/AppletDownloads.cpp
    CommonSrc/QtSource/AppletEditAboutMe.cpp
    CommonSrc/QtSource/AppletEditAvatarImage.cpp
    CommonSrc/QtSource/AppletEditStoryboard.cpp
    
    CommonSrc/QtSource/AppletFileOfferSelect.cpp
    CommonSrc/QtSource/AppletFileShareClientView.cpp
    CommonSrc/QtSource/AppletFileShareServerViewMine.cpp
    CommonSrc/QtSource/AppletFriendList.cpp
    CommonSrc/QtSource/AppletFriendRequest.cpp

    CommonSrc/QtSource/AppletGalleryImage.cpp
    CommonSrc/QtSource/AppletGalleryEmoticon.cpp
    CommonSrc/QtSource/AppletGalleryThumb.cpp
    CommonSrc/QtSource/AppletGetStarted.cpp
    CommonSrc/QtSource/AppletGroupClient.cpp
    CommonSrc/QtSource/AppletGroupHostAdmin.cpp
    CommonSrc/QtSource/AppletGroupJoin.cpp
    CommonSrc/QtSource/AppletGroupJoinSearch.cpp
    CommonSrc/QtSource/AppletGroupListClient.cpp
    CommonSrc/QtSource/AppletGroupListLocalView.cpp
    CommonSrc/QtSource/AppletHackerList.cpp
    CommonSrc/QtSource/AppletHelpNetSignalBars.cpp
    CommonSrc/QtSource/AppletHostBase.cpp
    CommonSrc/QtSource/AppletHostChatRoomStatus.cpp
    CommonSrc/QtSource/AppletHostGroupStatus.cpp
    CommonSrc/QtSource/AppletHostJoinRequestList.cpp
    CommonSrc/QtSource/AppletHostNetworkStatus.cpp
    CommonSrc/QtSource/AppletHostSelect.cpp
    CommonSrc/QtSource/AppletHostRandomConnectStatus.cpp
    CommonSrc/QtSource/AppletIgnoredHosts.cpp
    CommonSrc/QtSource/AppletInvites.cpp
    CommonSrc/QtSource/AppletInviteAccept.cpp
    CommonSrc/QtSource/AppletInviteCreate.cpp
    CommonSrc/QtSource/AppletIsPortOpenTest.cpp
    CommonSrc/QtSource/AppletJoinBase.cpp
    CommonSrc/QtSource/AppletLaunchPage.cpp
    CommonSrc/QtSource/AppletLaunchWidget.cpp
    CommonSrc/QtSource/AppletLibrary.cpp
    CommonSrc/QtSource/AppletLog.cpp
    CommonSrc/QtSource/AppletLogSettings.cpp
    CommonSrc/QtSource/AppletLogView.cpp
    CommonSrc/QtSource/AppletMgr.cpp
    CommonSrc/QtSource/AppletMultiMessenger.cpp
    CommonSrc/QtSource/AppletNetHostingPage.cpp
    CommonSrc/QtSource/AppletNetworkSettings.cpp
    CommonSrc/QtSource/AppletOfferList.cpp
    CommonSrc/QtSource/AppletOfferResponse.cpp
    CommonSrc/QtSource/AppletOfferResponseAccept.cpp
    CommonSrc/QtSource/AppletOfferSend.cpp
    CommonSrc/QtSource/AppletOfferView.cpp
    CommonSrc/QtSource/AppletPersonOfferList.cpp
    CommonSrc/QtSource/AppletPeerBase.cpp
    CommonSrc/QtSource/AppletPeerChangeFriendship.cpp
    CommonSrc/QtSource/AppletPeerReplyFileOffer.cpp
    CommonSrc/QtSource/AppletPeerSessionFileOffer.cpp
    CommonSrc/QtSource/AppletPeerTodGame.cpp
    CommonSrc/QtSource/AppletPeerVideoPhone.cpp
    CommonSrc/QtSource/AppletPeerVoicePhone.cpp
    CommonSrc/QtSource/AppletPermissionList.cpp
    CommonSrc/QtSource/AppletPersonalRecorder.cpp
    CommonSrc/QtSource/AppletPlayerBase.cpp
    CommonSrc/QtSource/AppletPlayerPhoto.cpp
    CommonSrc/QtSource/AppletPlayerVideo.cpp
    CommonSrc/QtSource/AppletPlayerNlc.cpp
    CommonSrc/QtSource/AppletPlayerNlcBase.cpp

    CommonSrc/QtSource/AppletPopupMenu.cpp
    CommonSrc/QtSource/AppletRandomConnectHostAdmin.cpp
    CommonSrc/QtSource/AppletRandomConnectJoin.cpp
    CommonSrc/QtSource/AppletRandomConnectJoinSearch.cpp
    CommonSrc/QtSource/AppletRandomConnectListLocalView.cpp
    CommonSrc/QtSource/AppletSearchPage.cpp
    CommonSrc/QtSource/AppletServiceAboutMe.cpp

    CommonSrc/QtSource/AppletServiceBase.cpp
    CommonSrc/QtSource/AppletServiceBaseSettings.cpp
    CommonSrc/QtSource/AppletServiceConnectionTest.cpp
    CommonSrc/QtSource/AppletSettingsHostBase.cpp
    CommonSrc/QtSource/AppletServiceHostNetwork.cpp
    CommonSrc/QtSource/AppletServiceShareFiles.cpp
    CommonSrc/QtSource/AppletServiceShareWebCam.cpp
    CommonSrc/QtSource/AppletServiceStoryboard.cpp
    CommonSrc/QtSource/AppletSettingsAboutMe.cpp
    CommonSrc/QtSource/AppletSettingsAvatarImage.cpp
    CommonSrc/QtSource/AppletSettingsBase.cpp
    CommonSrc/QtSource/AppletSettingsConnectionTest.cpp
    CommonSrc/QtSource/AppletSettingsFileXfer.cpp
    CommonSrc/QtSource/AppletSettingsFriendRequest.cpp
    CommonSrc/QtSource/AppletSettingsHostChatRoom.cpp
    CommonSrc/QtSource/AppletSettingsHostGroup.cpp
    CommonSrc/QtSource/AppletSettingsHostNetwork.cpp
    CommonSrc/QtSource/AppletSettingsHostRandomConnect.cpp
    CommonSrc/QtSource/AppletSettingsMessenger.cpp
    CommonSrc/QtSource/AppletSettingsPage.cpp
    CommonSrc/QtSource/AppletSettingsPushToTalk.cpp
    CommonSrc/QtSource/AppletSettingsRandomConnect.cpp
    CommonSrc/QtSource/AppletSettingsShareFiles.cpp
    CommonSrc/QtSource/AppletSettingsShareWebCam.cpp
    CommonSrc/QtSource/AppletSettingsStoryboard.cpp
    CommonSrc/QtSource/AppletSettingsTruthOrDare.cpp
    CommonSrc/QtSource/AppletSettingsVideoPhone.cpp
    CommonSrc/QtSource/AppletSettingsVoicePhone.cpp
    CommonSrc/QtSource/AppletShareServicesPage.cpp
    CommonSrc/QtSource/AppletSnapshot.cpp
    CommonSrc/QtSource/AppletSocketList.cpp
    CommonSrc/QtSource/AppletSoundSettings.cpp
    CommonSrc/QtSource/AppletStoryboardClient.cpp
    CommonSrc/QtSource/AppletStoryboardServerViewMine.cpp

    CommonSrc/QtSource/AppletTestAndDebug.cpp
    CommonSrc/QtSource/AppletTestBase.cpp
    CommonSrc/QtSource/AppletTestHostClient.cpp
    CommonSrc/QtSource/AppletTestHostService.cpp
    CommonSrc/QtSource/AppletTestUpnp.cpp

    CommonSrc/QtSource/AppletTheme.cpp
    CommonSrc/QtSource/AppletUploads.cpp
    CommonSrc/QtSource/AppletUserConnections.cpp
    CommonSrc/QtSource/AppletUserIdentity.cpp
    CommonSrc/QtSource/AppletUserPreferences.cpp
    CommonSrc/QtSource/AppTranslate.cpp
    CommonSrc/QtSource/BottomBarWidget.cpp
    CommonSrc/QtSource/AssetBaseWidget.cpp
    CommonSrc/QtSource/AssetFaceWidget.cpp
    CommonSrc/QtSource/AssetPhotoWidget.cpp
    CommonSrc/QtSource/AssetTextWidget.cpp
    CommonSrc/QtSource/AssetVideoWidget.cpp
    CommonSrc/QtSource/AssetVoiceWidget.cpp
    CommonSrc/QtSource/AvatarBar.cpp

    CommonSrc/QtSource/CamJavaClient.cpp
    CommonSrc/QtSource/CamLogic.cpp
    CommonSrc/QtSource/CamProcessor.cpp

    CommonSrc/QtSource/CamTitleBarPreview.cpp
    CommonSrc/QtSource/ChatRoomWidget.cpp
    CommonSrc/QtSource/ChatEntryWidget.cpp
    CommonSrc/QtSource/DialogAddComment.cpp
    CommonSrc/QtSource/DialogConfirmRemoveMessage.cpp
    CommonSrc/QtSource/EventsQtToNlc.cpp
    CommonSrc/QtSource/FileActionBarWidget.cpp
    CommonSrc/QtSource/FileActionMenu.cpp
    CommonSrc/QtSource/FileFilterSelectWidget.cpp
    CommonSrc/QtSource/FileItemInfo.cpp
    CommonSrc/QtSource/FileListReplySession.cpp
    CommonSrc/QtSource/FileMediaSelectWidget.cpp
    CommonSrc/QtSource/FileShareItemWidget.cpp
   
    CommonSrc/QtSource/FileXferWidget.cpp
    CommonSrc/QtSource/FriendList.cpp
    CommonSrc/QtSource/FriendListDelegate.cpp
    CommonSrc/QtSource/FriendListWidget.cpp

    CommonSrc/QtSource/GuiConnectIdListMgr.cpp
    CommonSrc/QtSource/GuiFileXferMgr.cpp
    CommonSrc/QtSource/GuiFileXferSession.cpp
    CommonSrc/QtSource/GuiFavoriteMgr.cpp
    CommonSrc/QtSource/GuiGroupie.cpp
    CommonSrc/QtSource/GuiGroupieListItem.cpp
    CommonSrc/QtSource/GuiGroupieListMgr.cpp
    CommonSrc/QtSource/GuiGroupieListSession.cpp
    CommonSrc/QtSource/GuiGroupieListWidget.cpp
    CommonSrc/QtSource/GuiHelpers.cpp
    CommonSrc/QtSource/GuiHelpersAndroidPermissions.cpp
    CommonSrc/QtSource/GuiHosted.cpp
    CommonSrc/QtSource/GuiHostedListItem.cpp
    CommonSrc/QtSource/GuiHostedListMgr.cpp
    CommonSrc/QtSource/GuiHostedListSession.cpp
    CommonSrc/QtSource/GuiHostedListWidget.cpp
    CommonSrc/QtSource/GuiHostJoin.cpp
    CommonSrc/QtSource/GuiHostJoinList.cpp
    CommonSrc/QtSource/GuiHostedByMeJoinMgr.cpp
    CommonSrc/QtSource/GuiHostJoinSession.cpp
    CommonSrc/QtSource/GuiHostSession.cpp

    CommonSrc/QtSource/GuiMemberActiveMgr.cpp
    CommonSrc/QtSource/GuiOfferInfo.cpp
    CommonSrc/QtSource/GuiOfferListItem.cpp
    CommonSrc/QtSource/GuiOfferListWidget.cpp
    CommonSrc/QtSource/GuiOfferMgr.cpp
    CommonSrc/QtSource/GuiOfferMgrBase.cpp
    CommonSrc/QtSource/GuiOfferMgrBaseCallbacks.cpp
    CommonSrc/QtSource/GuiOfferSession.cpp
    CommonSrc/QtSource/GuiParams.cpp
    CommonSrc/QtSource/GuiPlayerMgr.cpp
    CommonSrc/QtSource/GuiPluginMgr.cpp
    CommonSrc/QtSource/GuiPushToTalkMgr.cpp
    CommonSrc/QtSource/GuiRandConnectMgr.cpp
    CommonSrc/QtSource/GuiSendQueueMgr.cpp

    CommonSrc/QtSource/GuiThreadAppLoader.cpp
    CommonSrc/QtSource/GuiThreadMainLoader.cpp
    CommonSrc/QtSource/GuiThreadSettingsLoader.cpp

    CommonSrc/QtSource/GuiThumb.cpp
    CommonSrc/QtSource/GuiThumbList.cpp
    CommonSrc/QtSource/GuiThumbMgr.cpp
    CommonSrc/QtSource/GuiUser.cpp
    CommonSrc/QtSource/GuiUserBase.cpp
    CommonSrc/QtSource/GuiUserJoin.cpp
    CommonSrc/QtSource/GuiUserJoinList.cpp
    CommonSrc/QtSource/GuiUserJoinListItem.cpp
    CommonSrc/QtSource/GuiUserJoinListWidget.cpp
    CommonSrc/QtSource/GuiUserJoinMgr.cpp
    CommonSrc/QtSource/GuiUserJoinSession.cpp
    CommonSrc/QtSource/GuiUserListItem.cpp
    CommonSrc/QtSource/GuiUserListWidget.cpp
    CommonSrc/QtSource/GuiUserMgr.cpp
    CommonSrc/QtSource/GuiUserMultiListWidget.cpp
    CommonSrc/QtSource/GuiUserSessionBase.cpp
    CommonSrc/QtSource/GuiWebPageMgr.cpp
    CommonSrc/QtSource/HostedPluginWidget.cpp
    CommonSrc/QtSource/HostJoinRequestListItem.cpp
    CommonSrc/QtSource/HostJoinRequestListWidget.cpp
    CommonSrc/QtSource/HomeWindow.cpp
    CommonSrc/QtSource/HistoryEntryWidget.cpp
    CommonSrc/QtSource/HistoryListWidget.cpp
    CommonSrc/QtSource/ImageListEntry.cpp
    CommonSrc/QtSource/ImagePickBar.cpp
    CommonSrc/QtSource/ImageListRow.cpp
    CommonSrc/QtSource/ImageListWidget.cpp
    CommonSrc/QtSource/InformationWidget.cpp
    CommonSrc/QtSource/InputAllWidget.cpp
    CommonSrc/QtSource/InputBaseWidget.cpp
    CommonSrc/QtSource/InputClientBaseCallback.cpp
    CommonSrc/QtSource/InputFaceWidget.cpp
    CommonSrc/QtSource/InputPhotoWidget.cpp
    CommonSrc/QtSource/InputTextWidget.cpp
    CommonSrc/QtSource/InputVideoWidget.cpp
    CommonSrc/QtSource/InputVoiceWidget.cpp
    CommonSrc/QtSource/IdentLogicInterface.cpp
    CommonSrc/QtSource/IdentOfferWidget.cpp
    CommonSrc/QtSource/IdentWidget.cpp
    CommonSrc/QtSource/InstMsgWidget.cpp
    CommonSrc/QtSource/FriendListEntryWidget.cpp
    CommonSrc/QtSource/ListWidgetBase.cpp
    CommonSrc/QtSource/LogMgr.cpp
    CommonSrc/QtSource/LogWidget.cpp
    CommonSrc/QtSource/LogoRenderer.cpp
    CommonSrc/QtSource/miniaudioio/AudioBitrate.cpp
    CommonSrc/QtSource/miniaudioio/AudioEchoBuf.cpp
    CommonSrc/QtSource/miniaudioio/AudioEchoCancel.cpp
    CommonSrc/QtSource/miniaudioio/AudioEchoCancelImpl.cpp
    CommonSrc/QtSource/miniaudioio/AudioMasterClock.cpp
    CommonSrc/QtSource/miniaudioio/AudioMixerBuf.cpp
    CommonSrc/QtSource/miniaudioio/AudioSampleBuf.cpp
    CommonSrc/QtSource/miniaudioio/AudioTestGenerator.cpp
    CommonSrc/QtSource/miniaudioio/AudioUtils.cpp
    CommonSrc/QtSource/miniaudioio/MiniAudioDevices.cpp
    CommonSrc/QtSource/miniaudioio/MiniAudioIn.cpp
    CommonSrc/QtSource/miniaudioio/MiniAudioInDevice.cpp
    CommonSrc/QtSource/miniaudioio/MiniAudioMgr.cpp
    CommonSrc/QtSource/miniaudioio/MiniAudioOut.cpp
    CommonSrc/QtSource/miniaudioio/MiniAudioOutDevice.cpp
    CommonSrc/QtSource/MessengerPage.cpp
    CommonSrc/QtSource/MultiSessionState.cpp
    CommonSrc/QtSource/MyIcons.cpp
    CommonSrc/QtSource/NetAvailStatusWidget.cpp
    CommonSrc/QtSource/OfferBarWidget.cpp
    CommonSrc/QtSource/OfferSendWidget.cpp
    CommonSrc/QtSource/OfferSessionLogic.cpp
    CommonSrc/QtSource/PermissionWidget.cpp
    CommonSrc/QtSource/PermissionListItem.cpp
    CommonSrc/QtSource/PersonOfferListItem.cpp
    CommonSrc/QtSource/PersonOfferListWidget.cpp
    CommonSrc/QtSource/PhoneRinger.cpp
    CommonSrc/QtSource/PlayControlWidget.cpp
    CommonSrc/QtSource/PluginSettingsWidget.cpp
    CommonSrc/QtSource/RenderGlLogic.cpp
    CommonSrc/QtSource/RenderGlMatrix.cpp
    CommonSrc/QtSource/RenderGlOffScreenSurface.cpp
    CommonSrc/QtSource/RenderGlWidget.cpp
    CommonSrc/QtSource/RenderGlWidgetFonts.cpp
    CommonSrc/QtSource/RenderGlWidgetRender.cpp
    CommonSrc/QtSource/RenderGlWidgetShaderInterface.cpp
    CommonSrc/QtSource/RenderGlWidgetTextures.cpp
    CommonSrc/QtSource/RenderGlShaders.cpp
    CommonSrc/QtSource/RenderLogoShaders.cpp
    CommonSrc/QtSource/RenderPlayerNlcThread.cpp
    CommonSrc/QtSource/RenderShaderDefsGl.cpp
    CommonSrc/QtSource/RenderShaderQt.cpp
    CommonSrc/QtSource/RichTextWidget.cpp
    CommonSrc/QtSource/SearchBarWidget.cpp
    CommonSrc/QtSource/SearchParamsWidget.cpp
    CommonSrc/QtSource/SessionWidget.cpp
    CommonSrc/QtSource/ServiceSettingsWidget.cpp
    CommonSrc/QtSource/SoundMgr.cpp
    CommonSrc/QtSource/StoryWidget.cpp
    CommonSrc/QtSource/TestWidget.cpp
    CommonSrc/QtSource/TextChatLogic.cpp
    CommonSrc/QtSource/TitleBarWidget.cpp
    CommonSrc/QtSource/TodGameDefs.cpp
    CommonSrc/QtSource/TodGameLogic.cpp
    CommonSrc/QtSource/TodGameWidget.cpp
    CommonSrc/QtSource/ThumbnailEditWidget.cpp
    CommonSrc/QtSource/ThumbnailChooseWidget.cpp
    CommonSrc/QtSource/ThumbnailViewWidget.cpp

    CommonSrc/QtSource/VideoFrameProcessor.cpp
    CommonSrc/QtSource/VxAspectFrame.cpp
    CommonSrc/QtSource/VxComboBox.cpp
    CommonSrc/QtSource/VxContextMenu.cpp
    CommonSrc/QtSource/VxAvatarImage.cpp
    CommonSrc/QtSource/VxFrame.cpp
    CommonSrc/QtSource/VxMenuButton.cpp
    CommonSrc/QtSource/VxMenuButtonBottom.cpp
    CommonSrc/QtSource/VxMenuButtonTop.cpp
    CommonSrc/QtSource/VxPushButton.cpp
    CommonSrc/QtSource/VxLabel.cpp
    CommonSrc/QtSource/VxSndInstance.cpp
    CommonSrc/QtSource/VxResourceToRealFile.cpp
    CommonSrc/QtSource/VxScrollArea.cpp
    CommonSrc/QtSource/VxSpinProgress.cpp
    CommonSrc/QtSource/VxTagLabel.cpp
    CommonSrc/QtSource/VxTilePositioner.cpp
    CommonSrc/QtSource/VidWidget.cpp
    CommonSrc/QtSource/VxWidgetBase.cpp
    CommonSrc/QtSource/VxMenu.cpp
    CommonSrc/QtSource/VxAppStyle.cpp
    CommonSrc/QtSource/VxAppTheme.cpp
    CommonSrc/QtSource/VxAppDisplay.cpp
    CommonSrc/QtSource/VxPlainTextEdit.cpp
    CommonSrc/QtSource/WaitingSpinnerWidget.cpp
    CommonSrc/QtSource/VirtStream/VirtProviderFile.cpp
    CommonSrc/QtSource/VirtStream/VirtStorageProvider.cpp
    CommonSrc/QtSource/VirtStream/VirtStreamCache.cpp
    CommonSrc/QtSource/VirtStream/VirtStreamFile.cpp
    CommonSrc/QtSource/VirtStream/VirtStreamMgr.cpp
    CommonSrc/QtSource/VirtStream/VirtStreamMgrFileIo.cpp
    CommonSrc/QtSource/VirtStream/VirtStreamMgrProvider.cpp
    ../nolimitlib/GuiInterface/ILog.cpp
    ../nolimitlib/GuiInterface/IDefs.cpp
   )

set(nlc_FORMS)
list(APPEND nlc_FORMS
    CommonSrc/Forms/ActivityBrowseFiles.ui
    CommonSrc/Forms/ActivityCreateAccount.ui
    CommonSrc/Forms/ActivityDownloadItemMenu.ui
    CommonSrc/Forms/ActivityFileSearch.ui
    CommonSrc/Forms/ActivityGenerateHash.ui
    CommonSrc/Forms/ActivityInformation.ui
    CommonSrc/Forms/ActivityMessageBox.ui
    CommonSrc/Forms/ActivityNetworkStartupTest.ui
    CommonSrc/Forms/ActivityScanPeopleSearch.ui
    CommonSrc/Forms/ActivityScanProfiles.ui
    CommonSrc/Forms/ActivityScanStoryBoards.ui
    CommonSrc/Forms/ActivityScanWebCams.ui
    CommonSrc/Forms/ActivitySessionOptions.ui
    CommonSrc/Forms/ActivityShowHelp.ui
    CommonSrc/Forms/ActivityTimedMessage.ui
    CommonSrc/Forms/ActivityYesNo.ui
    CommonSrc/Forms/AcceptCancelWidget.ui
    CommonSrc/Forms/AppletAboutApp.ui
    CommonSrc/Forms/AppletAboutMeClient.ui
    CommonSrc/Forms/AppletAboutUser.ui
    CommonSrc/Forms/AppletApplicationInfo.ui
    CommonSrc/Forms/AppletBrowseFiles.ui
    CommonSrc/Forms/AppletCamClient.ui
    CommonSrc/Forms/AppletCamSettings.ui
    CommonSrc/Forms/AppletChatRoomClient.ui
    CommonSrc/Forms/AppletChatRoomJoinSearch.ui
    CommonSrc/Forms/AppletChatRoomListLocalView.ui
    CommonSrc/Forms/AppletChooseThumbnail.ui
    CommonSrc/Forms/AppletChooseUser.ui
    CommonSrc/Forms/AppletDownloads.ui
    CommonSrc/Forms/AppletEditAboutMe.ui
    CommonSrc/Forms/AppletEditAvatarImage.ui
    CommonSrc/Forms/AppletEditStoryBoard.ui
    CommonSrc/Forms/AppletFileOfferSelect.ui
    CommonSrc/Forms/AppletFileShareClientView.ui
    CommonSrc/Forms/AppletFriendList.ui
    CommonSrc/Forms/AppletFriendRequest.ui
    CommonSrc/Forms/AppletGalleryImage.ui
    CommonSrc/Forms/AppletGalleryEmoticon.ui
    CommonSrc/Forms/AppletGalleryThumb.ui
    CommonSrc/Forms/AppletGetStarted.ui
    CommonSrc/Forms/AppletGroupClient.ui
    CommonSrc/Forms/AppletGroupJoinSearch.ui
    CommonSrc/Forms/AppletGroupListClient.ui
    CommonSrc/Forms/AppletGroupListLocalView.ui
    CommonSrc/Forms/AppletHackerList.ui
    CommonSrc/Forms/AppletHelpNetSignalBars.ui
    CommonSrc/Forms/AppletHostJoinRequestList.ui
    CommonSrc/Forms/AppletHostNetworkStatus.ui
    CommonSrc/Forms/AppletHostSelect.ui
    CommonSrc/Forms/AppletIgnoredHosts.ui
    CommonSrc/Forms/AppletInviteAccept.ui
    CommonSrc/Forms/AppletInviteCreate.ui
    CommonSrc/Forms/AppletIsPortOpenTest.ui
    CommonSrc/Forms/AppletJoinHostList.ui
    CommonSrc/Forms/AppletLaunchWidget.ui
    CommonSrc/Forms/AppletLibrary.ui
    CommonSrc/Forms/AppletLog.ui
    CommonSrc/Forms/AppletLogSettings.ui
    CommonSrc/Forms/AppletLogView.ui
    CommonSrc/Forms/AppletMultiMessenger.ui
    CommonSrc/Forms/AppletNetworkKey.ui
    CommonSrc/Forms/AppletNetworkSettings.ui
    CommonSrc/Forms/AppletOfferList.ui
    CommonSrc/Forms/AppletOfferResponse.ui
    CommonSrc/Forms/AppletOfferResponseAccept.ui
    CommonSrc/Forms/AppletOfferSend.ui
    CommonSrc/Forms/AppletOfferView.ui
    CommonSrc/Forms/AppletPersonOfferList.ui
    CommonSrc/Forms/AppletPeerChangeFriendship.ui
    CommonSrc/Forms/AppletPeerReplyOfferFile.ui
    CommonSrc/Forms/AppletPeerSessionFileOffer.ui
    CommonSrc/Forms/AppletPeerTodGame.ui
    CommonSrc/Forms/AppletPeerVideoPhone.ui
    CommonSrc/Forms/AppletPeerVoicePhone.ui
    CommonSrc/Forms/AppletPermissionList.ui
    CommonSrc/Forms/AppletPersonalRecorder.ui
    CommonSrc/Forms/AppletPlayerCamClip.ui
    CommonSrc/Forms/AppletPlayerPhoto.ui
    CommonSrc/Forms/AppletPlayerNlc.ui
    CommonSrc/Forms/AppletPlayerVideo.ui
    CommonSrc/Forms/AppletPopupMenu.ui
    CommonSrc/Forms/AppletRandomConnectClient.ui
    CommonSrc/Forms/AppletRandomConnectJoinSearch.ui
    CommonSrc/Forms/AppletRandomConnectListLocalView.ui
    CommonSrc/Forms/AppletServiceBase.ui
    CommonSrc/Forms/AppletServiceBaseSettings.ui
    CommonSrc/Forms/AppletSettingsHostBase.ui
    CommonSrc/Forms/AppletSettingsNoUserSettings.ui
    CommonSrc/Forms/AppletSnapshot.ui
    CommonSrc/Forms/AppletSocketList.ui
    CommonSrc/Forms/AppletSoundSettings.ui
    CommonSrc/Forms/AppletStoryBoardClient.ui
    CommonSrc/Forms/AppletTestAndDebug.ui
    CommonSrc/Forms/AppletTestHostClient.ui
    CommonSrc/Forms/AppletTestHostService.ui
    CommonSrc/Forms/AppletTestUpnp.ui
    CommonSrc/Forms/AppletTheme.ui
    CommonSrc/Forms/AppletUploads.ui
    CommonSrc/Forms/AppletUserConnections.ui
    CommonSrc/Forms/AppletUserIdentity.ui
    CommonSrc/Forms/AppletUserPreferences.ui
    CommonSrc/Forms/AssetFaceWidget.ui
    CommonSrc/Forms/AssetPhotoWidget.ui
    CommonSrc/Forms/AssetTextWidget.ui
    CommonSrc/Forms/AssetVideoWidget.ui
    CommonSrc/Forms/AssetVoiceWidget.ui
    CommonSrc/Forms/AvatarBarWidget.ui
    CommonSrc/Forms/BottomBarWidget.ui
    CommonSrc/Forms/CentralFrame.ui
    CommonSrc/Forms/CentralWidget.ui
    CommonSrc/Forms/ChatEntryWidget.ui
    CommonSrc/Forms/DialogAddComment.ui
    CommonSrc/Forms/DialogConfirmRemoveMessage.ui
    CommonSrc/Forms/FileActionBarWidget.ui
    CommonSrc/Forms/FileActionMenu.ui
    CommonSrc/Forms/FileDisplayWidget.ui
    CommonSrc/Forms/FileFilterSelectWidget.ui
    CommonSrc/Forms/FileMediaSelectWidget.ui
    CommonSrc/Forms/FileShareItemWidget.ui
    CommonSrc/Forms/FileXferWidget.ui
    CommonSrc/Forms/FriendListDlg.ui
    CommonSrc/Forms/GuiGroupieListItem.ui
    CommonSrc/Forms/GuiHostedListItem.ui
    CommonSrc/Forms/GuiUserMultiListWidget.ui
    CommonSrc/Forms/HostJoinRequestListItem.ui
    CommonSrc/Forms/HistoryEntryWidget.ui
    CommonSrc/Forms/IdentWidget.ui
    CommonSrc/Forms/ImageListEntry.ui
    CommonSrc/Forms/ImageListRow.ui
    CommonSrc/Forms/ImageListWidget.ui
    CommonSrc/Forms/ImagePickBar.ui
    CommonSrc/Forms/InformationWidget.ui
    CommonSrc/Forms/InputAllWidget.ui
    CommonSrc/Forms/InputFaceWidget.ui
    CommonSrc/Forms/InputPhotoWidget.ui
    CommonSrc/Forms/InputTextWidget.ui
    CommonSrc/Forms/InputVideoWidget.ui
    CommonSrc/Forms/InputVoiceWidget.ui
    CommonSrc/Forms/InstMsgWidget.ui
    CommonSrc/Forms/LogWidget.ui
    CommonSrc/Forms/OfferBarWidget.ui
    CommonSrc/Forms/OfferSendWidget.ui
    CommonSrc/Forms/PermissionWidget.ui
    CommonSrc/Forms/PermissionListItemWidget.ui
    CommonSrc/Forms/PersonOfferListItem.ui
    CommonSrc/Forms/PlayControlWidget.ui
    CommonSrc/Forms/PluginSettingsWidget.ui
    CommonSrc/Forms/SearchBarWidget.ui
    CommonSrc/Forms/SearchParamsWidget.ui
    CommonSrc/Forms/ServiceSettingsWidget.ui
    CommonSrc/Forms/SessionWidget.ui
    CommonSrc/Forms/StoryWidget.ui
    CommonSrc/Forms/TestWidget.ui
    CommonSrc/Forms/ThumbnailEditWidget.ui
    CommonSrc/Forms/ThumbnailChooseWidget.ui
    CommonSrc/Forms/TitleBarWidget.ui
    CommonSrc/Forms/TodGameWidget.ui
    CommonSrc/Forms/VidWidget.ui
    CommonSrc/NoLimitConnect.qrc
    )

qt_add_executable(nolimitconnect
    MANUAL_FINALIZATION
    ${nlc_FORMS}
    ${nlc_SOURCES}
    ${nlc_HEADERS}
    )

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
    CommonSrc/QtSource
    ../nolimitlib
    ../nolimitlib/DependLibs
    ../nolimitlib/DependLibs/libplayer-nlc/xbmc
    ../nolimitlib/DependLibs/ffmpeg
    ../nolimitlib/DependLibs/libechocancel
    ../nolimitlib/GuiInterface
    ../nolimitlib/DependLibs/libptopengine
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
    ptopengine
    miniaudio
    wav-decoder
)

if (DEFINED USE_LIBJPEG_TURBO)
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

if (DEFINED TARGET_OS_LINUX)
    if (DEFINED TARGET_CPU_ARM64)
        #libvpx will not compile on raspberry pi
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
            #glew
            iconv

            vxvideolib
            tag
            theora
            twolame

            ogg
            netlib
            pktlib

            echocancel
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

            aecm-webrtc
            miniaudio
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

            gnu
            ass
            fribidi
            #glew
            iconv

            vxvideolib
            tag
            theora
            twolame

            ogg
            netlib
            pktlib

            echocancel
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

            aecm-webrtc
            miniaudio
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
            #glew
            iconv

            vxvideolib
            speex
            tag
            theora
            twolame

            ogg
            netlib
            pktlib
            echocancel
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

            aecm-webrtc
            miniaudio
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
            #glew
            iconv

            vxvideolib
            speex
            tag
            theora
            twolame

            ogg
            netlib
            pktlib
            echocancel
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

            aecm-webrtc
            miniaudio
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
    target_link_libraries(nolimitconnect
        PUBLIC
        # static nlc libraries

        x264
        x265
        dav1d
        #vpx #needed but does not compile on windows yet

        pcre
        opencore-amr

        mediatools
        speex
        lame
        vorbis2

        gnu
        ass

        fribidi
        #glew
        iconv
        ilb

        vxvideolib

        speex
        tag
        theora
        twolame
        ogg

        netlib
        pktlib

        echocancel
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

        aecm-webrtc
        miniaudio
        miniupnpc
        opengl32
        glu32
        ole32
        winmm
        Rpcrt4
    )

else()
    message(FATAL_ERROR "Unknown Target OS Defined for target_link_libraries")
endif()


qt_finalize_target(nolimitconnect)

if(DEFINED FLATPAKBUILD)

    #message("** Enabling Flatpak workarounds")
    #add_definitions(-DPQMFLATPAKBUILD)
    #include_directories(${GLIB_INCLUDE_DIRS})
    #target_link_libraries(nolimitconnect PRIVATE ${GLIB_LIBRARIES})
    #target_link_libraries(nolimitconnect PRIVATE "gobject-2.0")
    #target_link_libraries(nolimitconnect PRIVATE "gio-2.0") 
    
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
            set(CPACK_NSIS_MUI_ICON "${PROJECT_SRC_ROOT_DIR}/nolimitgui/CommonSrc/Resources/NlcP2P.ico")

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

