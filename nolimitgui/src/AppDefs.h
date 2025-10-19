#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>
#include "MyIconsDefs.h"

#include <QString>

enum EApplet
{
	eAppletUnknown = 0,             // do not setup base class ui in the case of unknown
    eAppletActivityDialog,          // place dialog over home window.. do not setup base class ui in the case of dialog
    eAppletEditAvatarImage,
    eAppletHomePage,
    // start of home page applets.. shown in order of enumeration
    eAppletGetStarted,

    eAppletFriendList,

    eAppletGroupJoin,
    eAppletChatRoomJoin,
    eAppletRandomConnectJoin,
    eAppletInviteCreate,
    eAppletInviteAccept,
    eAppletUserIdentity,
    eAppletPersonalRecorder,
    eAppletLibrary,
    eAppletPlayerNlc,
    
    eAppletDownloads,
    eAppletUploads,
    eAppletShareServicesPage,       // page to setup share with others services
    eAppletNetHostingPage,	        // page to setup network/group hosts and services
    eAppletSettingsPage,            // page of setting applets
    eAppletAboutNoLimitConnect,

	eMaxBasicApplets,               // marker.. end of home page applets.. start of settings applets

    eAppletTheme,
    eAppletUserPreferences,
    eAppletNetworkSettings,
    eAppletSoundSettings,
    eAppletCamSettings,
    eAppletPermissionList,          // list of all users plugin permission levels
    eAppletTestAndDebug,
    eAppletUserConnections,
    eAppletSocketList,
    eAppletHackerList,

    eMaxSettingsApplets,            // marker.. end of settings applets.. past here is search applets

    eAppletGroupJoinSearch,         // search for group to join
    eAppletChatRoomJoinSearch,      // search for chat room to join
    eAppletRandomConnectJoinSearch, // search for group to join
    eAppletSearchPersons,           // search for person in group by name
    eAppletSearchMood,              // search for mood message

    eAppletScanAboutMe,             // scan group for pictures in about me page
    eAppletScanStoryboard,          // scan group for pictures in story board page
    eAppletScanSharedFiles,         // search group for shared files
    eAppletScanWebCam,              // scan group for shared web cams

    eMaxSearchApplets,              // marker.. end of search applets.. past here is hosting applets

    eAppletGroupHostAdmin,
    eAppletSettingsHostGroup,
    eAppletHostGroupStatus,

    eAppletChatRoomHostAdmin,	       
    eAppletSettingsHostChatRoom,
    eAppletHostChatRoomStatus,	   

    eAppletRandomConnectHostAdmin,
    eAppletSettingsHostRandomConnect,     // connect to random person service
    eAppletHostRandomConnectStatus,

    eAppletServiceHostNetwork,      // network host service for a nolimitconnect network.. requires fixed ip address, group list hosts announce to network host
    eAppletSettingsHostNetwork,
    eAppletHostNetworkStatus,
    eAppletChatRoomListLocalView,       // view chat room hosts announced to hosted network if this node is a network host
    eAppletGroupListLocalView,          // view group hosts announced to hosted network if this node is a network host
    eAppletRandomConnectListLocalView,  // view random connect hosts announced to hosted network if this node is a network host

    eAppletServiceConnectionTest,   // network test service for users to test if they have a open port

    eMaxHostApplets,                // marker.. end of hosting applets.. start of other services

    eAppletSearchPage, // temporarily moved until can be implemented in home page

    eAppletServiceAboutMe,
    eAppletEditAboutMe,
    eAppletAboutMeServerViewMine,
    eAppletServiceStoryboard,
    eAppletEditStoryboard,
    eAppletStoryboardServerViewMine,
    eAppletServiceShareWebCam,
    eAppletCamServerViewMine,
    eAppletServiceShareFiles,
    eAppletFileShareServerViewMine,

    eMaxSharedServicesApplets,      // marker.. end of user share services applets.. start of not launched by user page icon

    // not launched from launch page icon

    eAppletApplicationInfo,
    eAppletBrowseFiles,             // file directory browser

    eAppletPlayerCamClip,
    eAppletPlayerPhoto,

    eAppletPlayerVideo,

    eAppletAvatarImageClient,	    // get users avatar imaage from other user

    eAppletConnectionTestClient,    // network test service for users to test if they have a open port

    eAppletHostNetworkClient,       // network host service for a nolimitconnect network.. requires fixed ip address, group list hosts announce to network host

    eAppletGroupClient,	            // client of joined group host. provide search and relay services for group users
    eAppletChatRoomClient,	        // use chat room
    eAppletRandomConnectClient,     // connect to random person relay service for users without a open port

    eAppletClientRelay,             // network relay service for users without a open port
    eAppletClientShareFiles,
    eAppletCamClient,

    eAppletChooseThumbnail,

    eAppletHostJoinConnect,
    eAppletHostJoinChoose,
    eAppletHostJoinSearch,
    eAppletHostLeave,

    eAppletFriendRequest,
    eAppletFriendRequestList,

    eAppletGalleryEmoticon,
    eAppletGalleryImage,
    eAppletGalleryThumb,
    eAppletGroupListClient,
    eAppletSnapshot,

    eAppletServiceSettings,

    eAppletSettingsAboutMe,
    eAppletSettingsAvatarImage,
    eAppletSettingsWebCamServer,
    eAppletSettingsConnectTest,
    eAppletSettingsShareFiles,
    eAppletSettingsFileXfer,
    eAppletSettingsFriendRequest,

    eAppletSettingsMessenger,
    eAppletSettingsPushToTalk,
    eAppletSettingsRandomConnect,
    eAppletSettingsStoryboard,
    eAppletSettingsTruthOrDare,
    eAppletSettingsVideoPhone,
    eAppletSettingsVoicePhone,

	ePluginAppletCamProvider,
	ePluginAppletNlcStation,
	ePluginAppletNlcNetworkHost,

    eAppletTestHostClient,
    eAppletTestHostService,
    eAppletTestUpnp,

    eAppletLog,                     // logging window.. shown in other frame
    eAppletLogSettings,

    // not lauched by user
    eAppletCreateAccount,
    eAppletSnapShot,
    eActivityBrowseFiles,

    eAppletMultiMessenger,          // messenger for all hosts and friends
    eAppletPeerChangeFriendship,
    eAppletPeerReplyOfferFile,
    eAppletPeerTruthOrDare,
    eAppletPeerVideoPhone,
    eAppletPeerVoicePhone,

    eAppletPeerSessionFileOffer,

    eAppletHostJoinRequestList,
    eAppletOfferList,
    eAppletPersonOfferList,	

    eAppletPopupMenu,
    eAppletHelpNetSignalBars,

    eAppletAboutFile,
    eAppletAboutMeClient,
    eAppletAboutUser,
    eAppletFileShareClientView,
    eAppletStoryboardClient,

    eAppletFileOfferSelect,
    eAppletOfferInfo,
    eAppletOfferSend,
    eAppletOfferResponse,
    eAppletOfferResponseAccept,
    eAppletOfferView,

    eAppletIgnoredHosts,

    eAppletIsPortOpenTest,

    eActivityGenerateHash,
    eAppletInformation,

    eAppletHomeFrame, // left or top frame
    eAppletMessengerFrame, // right or bottom frame

	eMaxApplets // must be last
};

enum EButtonSize
{
    eButtonSizeTiny,
    eButtonSizeSmall,
    eButtonSizeMedium,
    eButtonSizeLarge,

    eMaxButtonSize
};

enum EExpireTime
{
    eExpireTimeWhenResponseRxed = 0,
    eExpireTime20Seconds,
    eExpireTime1Min,
    eExpireTime10Min,
    eExpireTime1Hour,
    eExpireTime24Hour,
    eExpireTime1Week,
    eExpireTime1Month,
    eExpireTime1Year,

    eMaxExpireTime
};

enum EHomeLayout
{
	eHomeLayoutPlayerPlusMessenger,	// player with messenger on side or bottom
	eHomeLayoutPlayerOnly,			// player/home page only
	eHomeLayoutMessengerOnly,		// messenger only

	eMaxHomeLayouts // must be last
};

enum ELaunchFrame
{
	eLaunchFraneUnknown,
	eLaunchFrameHome,
	eLaunchFrameMessenger,

	eMaxLaunchFrame
};

enum EThemeType
{
	eThemeTypeUnknown = 0,
	eThemeTypeLight = 1,
	eThemeTypeDark = 2,
	eThemeTypeBlueOnWhite = 3,
	eThemeTypeGreenOnWhite = 4,

	eMaxThemeType
};

// returns applet icon
EMyIcons						GetAppletIcon( enum EApplet applet );

// returns text to go under applet icon ( is translated )
QString							DescribeApplet( enum EApplet applet );
