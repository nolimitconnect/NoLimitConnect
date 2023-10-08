
#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

enum EMyIcons
{
	eMyIconNone			= 0,		// no icon to show
	eMyIconUnknown,					// Icon with question mark
	eMyIconApp,
    eMyIconAvatarImage,
	
    eMyIconNotifyJoinOverlay,		// joined overlay
    eMyIconNotifyOnlineOverlay,		// online dot
	eMyIconNotifyOfflineOverlay,	// offline dot
	eMyIconNotifyDirectConnectOverlay, // direct connect overlay
	eMyIconNotifyInGroupOverlay,	// in group overlay
    eMyIconNotifyOfferOverlay,		// hand offer overlay
    eMyIconNotifyForbbidenOverlay,	// permission denied

	eMyIconEyeOverlay,				// eye overlay
	eMyIconPlusOverlay,				// add to shared or library icon overlay
	eMyIconMinusOverlay,			// remove from shared or library
	eMyIconFriendOverlay,			// is friend
	eMyIconGlobeOverlay,			// is global shared or unknown person
	eMyIconNlcFavoriteOverlay,		// nlc preferred host or user overlay

    eMyIconInformation,
    eMyIconPeople,
	eMyIconServers,
    eMyIconSettingsGear,
	eMyIconFileServer,

    eMyIconEyeAll,
	eMyIconEyeAnnouncedChatRooms,
	eMyIconEyeAnnouncedGroups,
	eMyIconEyeAnnouncedRandomConnect,
    eMyIconEyeChatRoom,
    eMyIconEyeFriends,
    eMyIconEyeGroup,
    eMyIconEyeHide,
    eMyIconEyeRandomConnect,
	eMyIconEyeSearchDisabled,
	eMyIconEyeSearchEnabled,
    eMyIconEyeShow,

    eMyIconSearch,
    eMyIconSearchJoinGroup,
    eMyIconSearchJoinChatRoom,
    eMyIconSearchJoinRandomConnect,
    eMyIconSearchPerson,
    eMyIconSearchHeart,
    eMyIconSearchWebCam,
    eMyIconSearchImage,
    eMyIconSearchStoryboard,
    eMyIconSearchFolder,

    eMyIconShareWithPerson,

	eMyIconAdministrator,
	eMyIconFriend,
	eMyIconGuest,
	eMyIconAnonymous,
	eMyIconIgnored,

	eMyIconFriendBroadcast,
	eMyIconFriendJoined,

    eMyIconClientChatRoom,
    eMyIconClientGroup,
    eMyIconClientRandomConnect,
    eMyIconClientWebCam,

	eMyIconProfile,
	eMyIconProfileLocked,
	eMyIconProfileDisabled,
	eMyIconProfileIgnored,

	eMyIconWebCamServer,
	eMyIconWebCamServerLocked,
	eMyIconWebCamServerDisabled,
	eMyIconWebCamServerIgnored,

	eMyIconFolder,
	eMyIconFolderDisabled,

	eMyIconMultiSession,
	eMyIconMultiSessionLocked,
	eMyIconMultiSessionDisabled,
	eMyIconMultiSessionIgnored,
	eMyIconMultiSessionCancel,

	eMyIconVoicePhoneCancel,
	eMyIconVoicePhoneNormal,
	eMyIconVoicePhoneDisabled,
	eMyIconVoicePhoneLocked,
	eMyIconVoicePhoneIgnored,

	eMyIconPushToTalkCancel,
	eMyIconPushToTalkOff,
	eMyIconPushToTalkOn,
	eMyIconPushToTalkDisabled,
	eMyIconPushToTalkLocked,
	eMyIconPushToTalkIgnored,

	eMyIconVideoPhoneCancel,
	eMyIconVideoPhoneNormal,
	eMyIconVideoPhoneDisabled,
	eMyIconVideoPhoneLocked,
	eMyIconVideoPhoneIgnored,

	eMyIconTruthOrDareCancel,
	eMyIconTruthOrDareNormal,
	eMyIconTruthOrDareDisabled,
	eMyIconTruthOrDareLocked,
	eMyIconTruthOrDareIgnored,

	eMyIconPermissions,
    eMyIconKeyEnabled,
    eMyIconKeyDisabled,

	eMyIconNotifyStarWhite,
	eMyIconNotifyStarGreen,

	eMyIconRefresh,

	eMyIconPhoto,
	eMyIconMusic,
	eMyIconVideo,
	eMyIconDocument,
	eMyIconArcOrIso,
	eMyIconOther,

	eMyIconUpOneDirectory,
	eMyIconFileUpload,
	eMyIconFileDownload,
	eMyIconFileAdd,
	eMyIconFileRemove,

	eMyIconStoryBoardNormal,
	eMyIconStoryBoardDisabled,
	eMyIconStoryBoardLocked,
	eMyIconStoryBoardIgnored,

	eMyIconMicrophoneOff,
	eMyIconMicrophoneOn,

	eMyIconMicrophoneCancelDisabled,
	eMyIconMicrophoneCancelNormal,

	eMyIconDebug,
	eMyIconFileShareOptions,
	eMyIconFileShareAddRemove,

	eMyIconShareFilesDisabled,
	eMyIconShareFilesNormal,
	eMyIconShareFilesCancel,

	eMyIconPauseDisabled,
	eMyIconPauseNormal,

	eMyIconPlayDisabled,
	eMyIconPlayNormal,

	eMyIconRedX,
	eMyIconCheckMark,

    eMyIconCancelRecord,
	eMyIconTrash,

	eMyIconShredderDisabled,
	eMyIconShredderNormal,

	eMyIconFileDownloadDisabled,
	eMyIconFileDownloadCancel,
	eMyIconFileDownloadNormal,

	eMyIconSendFileDisabled,
	eMyIconSendFileCancel,
	eMyIconSendFileNormal,
	eMyIconSendFileLocked,
	eMyIconSendFileIgnored,

	eMyIconRecordMovieDisabled,
	eMyIconRecordMovieNormal,

	eMyIconRecordMotionDisabled,
	eMyIconRecordMotionCancel,
	eMyIconRecordMotionNormal,

	eMyIconMotionAlarmDisabled,
	eMyIconMotionAlarmCancel,
	eMyIconMotionAlarmRed,
	eMyIconMotionAlarmWhite,
	eMyIconMotionAlarmYellow,

	eMyIconAcceptNormal,
	eMyIconAcceptDisabled,
	eMyIconCancelNormal,
	eMyIconCancelDisabled,

	eMyIconSendArrowNormal,
	eMyIconSendArrowDisabled,
	eMyIconSendArrowCancel,

	eMyIconCameraDisabled,
	eMyIconCameraNormal,
	eMyIconCameraCancel,

	eMyIconCamcorderNormal,
	eMyIconCamcorderDisabled,
	eMyIconCamcorderCancel,

	eMyIconSelectCameraDisabled,
	eMyIconSelectCameraNormal,

	eMyIconSpeakerOn,
	eMyIconSpeakerOff,

	eMyIconCameraFlashDisabled,
	eMyIconCameraFlashNormal,
	eMyIconCameraFlashCancelNormal,

	eMyIconSnapshotDisabled,
	eMyIconSnapshotNormal,
	eMyIconSnapshotCancelNormal,

	eMyIconNotepadDisabled,
	eMyIconNotepadNormal,

	eMyIconGalleryDisabled,
	eMyIconGalleryNormal,
	eMyIconGalleryCancel,

	eMyIconEmoticonSmile,

	eMyIconFaceSelectDisabled,
	eMyIconFaceSelectNormal,
	eMyIconFaceCancelNormal,

	eMyIconCamPreviewNormal,
	eMyIconCamPreviewDisabled,
	eMyIconCamPreviewCancelNormal,
	eMyIconCamPreviewCancelDisabled,

	eMyIconCamRotateNormal,
	eMyIconCamRotateDisabled,
	eMyIconCamSetupNormal,
	eMyIconCamSetupDisabled,
	eMyIconCamSetupCancelNormal,
	eMyIconCamSetupCancelDisabled,
	eMyIconCamSelectNormal,
	eMyIconCamSelectDisabled,

	eMyIconImageRotateNormal,
	eMyIconImageRotateDisabled,

	eMyIconFileBrowseNormal,
	eMyIconFileBrowseDisabled,
	eMyIconFileBrowseCancel,

	eMyIconMoveUpDirNormal,
	eMyIconMoveUpDirDisabled,
	eMyIconMoveUpDirCancel,

	eMyIconLibraryDisabled,
	eMyIconLibraryNormal,
	eMyIconLibraryCancel,

	eMyIconEchoCancelNormal,
	eMyIconEchoCancelCancel,
	eMyIconSendFailed,
	eMyIconPersonUnknownNormal,

	eMyIconMenuNormal,
	eMyIconMenuDisabled,

	eMyIconAdministratorDisabled,

	eMyIconNetworkStateDiscover,
	eMyIconNetworkStateDisconnected,
	eMyIconNetworkStateDirectConnected,
	eMyIconNetworkStateHostFail,
	eMyIconNetworkStateRelayConnected,


	//=== title bar icons ===//
	eMyIconPowerOff,
	eMyIconHome,
	eMyIconShare,
	eMyIconMenu,
	eMyIconBack,

	//=== bottom bar icons ===//
	eMyIconArrowLeft,
	eMyIcon30SecBackward,
	eMyIcon30SecForward,
	eMyIconArrowRight,
    eMyIconMediaRepeat,
    eMyIconWindowExpand,
    eMyIconWindowShrink,

    //=== applet icons ===//
    eMyIconKodi,
    eMyIconUserIdentity,
    eMyIconSharedContent,

    eMyIconMessenger,
    eMyIconGallery,

    eMyIconVideoStreamViewer,
    eMyIconCamStreamViewer,
    eMyIconMusicStreamPlayer,

    eMyIconRemoteControl,

    eMyIconPlugins,
    eMyIconTheme,

    eMyIconNetworkKey,
    eMyIconNetworkSettings,
    eMyIconSearchRandomConnect,


    // built in plugin options
    eMyIconCamStreamProvider,
    eMyIconNlcStation,

    eMyIconShareServices,

    eMyIconServiceAvatarImage,
    eMyIconServiceChatRoom,
    eMyIconServiceConnectionTest,
    eMyIconServiceHostGroup,
    eMyIconServiceHostGroupListing,
    eMyIconServiceHostNetwork,
    eMyIconServiceShareAboutMe,
    eMyIconServiceShareFiles,
    eMyIconServiceShareStoryboard,
    eMyIconServiceShareWebCam,
    eMyIconServiceRandomConnect, 

    eMyIconSettingsAboutMe,
    eMyIconSettingsAvatarImage,
    eMyIconSettingsChatRoom,
    eMyIconSettingsConnectionTest,
    eMyIconSettingsHostGroup,
    eMyIconSettingsHostGroupListing,
    eMyIconSettingsHostNetwork,
	eMyIconSettingsSendFile,
    eMyIconSettingsShareFiles,
    eMyIconSettingsShareStoryboard,
    eMyIconSettingsShareWebCam,
    eMyIconSettingsRandomConnect,
    eMyIconSettingsRandomConnectRelay,
    eMyIconSettingsRelay,
    eMyIconFileXfer,
    eMyIconSettingsFileXfer,
    eMyIconSettingsMessenger,
    eMyIconTruthOrDare,
    eMyIconSettingsTruthOrDare,
    eMyIconSettingsVoicePhone,
    eMyIconSettingsVideoPhone,
	eMyIconSettingsPushToTalk,

    eMyIconEditRedo,
    eMyIconEditUndo,
    eMyIconEditAlignCenter,
    eMyIconEditAlignLeft,
    eMyIconEditAlignLeftAndRight,
    eMyIconEditAlignRight,
    eMyIconEditBold,
    eMyIconEditCopy,
    eMyIconEditCut,
    eMyIconEditFileNew,
    eMyIconEditHeader,
    eMyIconEditItalic,
    eMyIconEditPaperClip,
    eMyIconEditPaste,
    eMyIconEditPrint,
    eMyIconEditText,
    eMyIconEditUnderline,

    eMyIconEditHyperLink,

    eMyIconFileOpen,
    eMyIconFileSave,
    eMyIconFileSaveAs,
    eMyIconZoomIn,
    eMyIconZoomOut,

    eMyIconOfferFile,
    eMyIconOfferFriendship,
	eMyIconOfferHistory,
    eMyIconOfferList,
    eMyIconOfferTruthOrDare,
    eMyIconOfferVideoChat,
    eMyIconOfferVoicePhone,

	eMyIconPersonAdd,
	eMyIconPersonsOfferList,
	eMyIconGroupJoined,

	eMyIconDirectConnected,

	eMyIconConnect,
	eMyIconDisconnect,

	eMyIconInvite,
	eMyIconInviteAccept,
	eMyIconInviteCreate,

	eMyIconArrowUpRight,
	eMyIconBoot,
	eMyIconStarEmpty,
	eMyIconStarFull,

	eMyIconChatRoomInfo,
	eMyIconGroupInfo,
	eMyIconNetworkInfo,
	eMyIconRandomConnectInfo,

    eMyIconSoundSettings,

	eMyIconShareAboutMeEye,
	eMyIconShareFilesEye,
	eMyIconShareStoryboardEye,
	eMyIconShareWebCamEye,

	eMyIconAboutMeEdit,
	eMyIconStoryboardEdit,

	eMyIconOfferEye,

	eMyIconUserPreferences,

	eMyIconNetworking,

	eMyIconVlc,

	eMaxMyIcon	// must be last
};
