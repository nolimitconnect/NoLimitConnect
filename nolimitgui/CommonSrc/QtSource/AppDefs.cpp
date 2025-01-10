//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppDefs.h"
#include <QObject>

//============================================================================
EMyIcons GetAppletIcon( EApplet applet )
{
	switch( applet )
	{
    case eActivityAppSetup:                 return eMyIconApp;
    case eAppletAboutNoLimitConnect:	    return eMyIconApp;
    case eAppletApplicationInfo:	        return eMyIconUnknown;
    case eAppletAvatarImageClient:          return eMyIconAvatarImage;
    case eAppletBrowseFiles:	            return eMyIconFileOpen;
        
    case eAppletChatRoomClient:             return eMyIconChatRoomClient;
    case eAppletChatRoomJoinSearch:	        return eMyIconSearchJoinChatRoom;
    case eAppletChatRoomJoin:	            return eMyIconChatRoomClient;
    case eAppletChatRoomListLocalView:	    return eMyIconEyeAnnouncedChatRooms;
    case eAppletChooseUser:	                return eMyIconSearchPerson;

    case eAppletCreateAccount:			    return eMyIconUserIdentity;

    case eAppletFriendListClient:	        return eMyIconFriendJoined;
    case eAppletGetStarted:                 return eMyIconInformation;

    case eAppletGroupClient:                return eMyIconGroupClient;
    case eAppletGroupHostAdmin:	            return eMyIconGroupClient;

    case eAppletGroupJoin:                  return eMyIconGroupClient;
    case eAppletGroupJoinSearch:	        return eMyIconSearchJoinGroup;
    case eAppletGroupListClient:	        return eMyIconGroupJoined;
    case eAppletGroupListLocalView:	        return eMyIconEyeAnnouncedGroups;

    case eAppletHomePage:				    return eMyIconHome;
    case eAppletChatRoomHostAdmin:	        return eMyIconChatRoomClient;
    case eAppletHostChatRoomStatus:	        return eMyIconChatRoomInfo;
    case eAppletHostGroupStatus:	        return eMyIconGroupInfo;
    case eAppletHostNetworkStatus:	        return eMyIconNetworkInfo;
    case eAppletHostRandomConnectStatus:	return eMyIconRandomConnectInfo;

    case eAppletIgnoredHosts:	            return eMyIconIgnored;
    case eAppletInviteAccept:	            return eMyIconInviteAccept;
    case eAppletInviteCreate:	            return eMyIconInviteCreate;
    case eAppletInvites:	                return eMyIconInvite;

    case eAppletLibrary:				    return eMyIconLibraryNormal;
    case eAppletLog:	                    return eMyIconDebug;
    case eAppletLogSettings:                return eMyIconDebug;
    case eAppletLogView:                    return eMyIconDebug;

    case eAppletMessengerFrame:				return eMyIconMessenger;
    case eAppletMultiMessenger:				return eMyIconMessenger;

    case eAppletNetHostingPage:             return eMyIconServiceHostNetwork;
    case eAppletNetworkSettings:	        return eMyIconNetworkSettings;

    case eAppletPersonalRecorder:		    return eMyIconNotepadNormal;
    case eAppletPlayerCamClip:	            return eMyIconVideo;
    case eAppletPlayerPhoto:	            return eMyIconPhoto;
    case eAppletPlayerVideo:			    return eMyIconVideo;
    case eAppletPlayerNlc:			        return eMyIconPlayerNlc;

    case eAppletRandomConnectClient:	    return eMyIconSearchRandomConnect;
    case eAppletRandomConnectHostAdmin:	    return eMyIconRandomConnectClient;
    case eAppletRandomConnectJoin:	        return eMyIconRandomConnectClient;
    case eAppletRandomConnectJoinSearch:    return eMyIconSearchJoinRandomConnect;
    case eAppletRandomConnectListLocalView:	return eMyIconEyeAnnouncedRandomConnect;

    case eAppletSettingsPage:		        return eMyIconSettingsGear;
   
    case eAppletSearchPage:	                return eMyIconSearch;
    case eAppletSearchPersons:	            return eMyIconSearchPerson;
    case eAppletSearchMood:	                return eMyIconSearchHeart;
    case eAppletScanAboutMe:	            return eMyIconSearchImage;
    case eAppletScanStoryboard:	            return eMyIconSearchStoryboard;
    case eAppletScanSharedFiles:	        return eMyIconSearchFolder;
    case eAppletScanWebCam:	                return eMyIconSearchWebCam;

    case eAppletTestAndDebug:	            return eMyIconDebug;
    case eAppletTestHostClient:	            return eMyIconDebug;
    case eAppletTestHostService:	        return eMyIconDebug;
    case eAppletTestUpnp:	                return eMyIconDebug;

    case eAppletTheme:					    return eMyIconTheme;

    case eAppletUserIdentity:			    return eMyIconUserIdentity;
    case eAppletUserPreferences:			return eMyIconUserPreferences;

    case eAppletServiceAboutMe:             return eMyIconServiceShareAboutMe;

    case eAppletServiceShareFiles:          return eMyIconServiceShareFiles;
    case eAppletServiceShareWebCam:         return eMyIconServiceShareWebCam;
    case eAppletServiceStoryboard:          return eMyIconServiceShareStoryboard;

    case eAppletServiceHostNetwork:         return eMyIconServiceHostNetwork;
    case eAppletServiceConnectionTest:      return eMyIconServiceConnectionTest;
    case eAppletServiceSettings:            return eMyIconUnknown;

    case eAppletSettingsAboutMe:            return eMyIconSettingsAboutMe;
    case eAppletSettingsAvatarImage:        return eMyIconSettingsAvatarImage;
    case eAppletSettingsWebCamServer:       return eMyIconSettingsShareWebCam;
    case eAppletSettingsConnectTest:        return eMyIconSettingsConnectionTest;
    case eAppletSettingsShareFiles:         return eMyIconSettingsShareFiles;
    case eAppletSettingsFileXfer:           return eMyIconSettingsFileXfer;

    case eAppletSettingsHostChatRoom:       return eMyIconSettingsChatRoom;
    case eAppletSettingsHostGroup:          return eMyIconSettingsHostGroup;
    case eAppletSettingsHostNetwork:        return eMyIconSettingsHostNetwork;
    case eAppletSettingsHostRandomConnect:  return eMyIconSettingsRandomConnect;

    case eAppletSettingsMessenger:          return eMyIconSettingsMessenger;
    case eAppletSettingsPushToTalk:         return eMyIconSettingsPushToTalk;
    case eAppletSettingsRandomConnect:      return eMyIconSettingsRandomConnect;
    case eAppletSettingsStoryboard:         return eMyIconSettingsShareStoryboard;
    case eAppletSettingsTruthOrDare:        return eMyIconSettingsTruthOrDare;
    case eAppletSettingsVideoPhone:         return eMyIconSettingsVideoPhone;
    case eAppletSettingsVoicePhone:         return eMyIconSettingsVoicePhone;

    case eAppletShareServicesPage:          return eMyIconShareServices;

    case eAppletFileOfferSelect:            return eMyIconServiceShareFiles;
    case eAppletFileShareClientView:        return eMyIconDebug;
    case eAppletOfferSend:                  return eMyIconOfferEye;
    case eAppletOfferResponse:              return eMyIconOfferEye;
    case eAppletOfferResponseAccept:        return eMyIconOfferEye;
    case eAppletOfferView:                  return eMyIconOfferEye;

    case eAppletPermissionList:             return eMyIconKeyEnabled;
    case eAppletPeerChangeFriendship:       return eMyIconKeyEnabled;
    case eAppletPeerReplyOfferFile:         return eMyIconDebug;
    case eAppletPeerTruthOrDare:            return eMyIconTruthOrDareNormal;
    case eAppletPeerVideoPhone:             return eMyIconVideoPhoneNormal;
    case eAppletPeerVoicePhone:             return eMyIconVoicePhoneNormal;

    case eAppletPeerSessionFileOffer:       return eMyIconDebug;

    case eAppletDownloads:                  return eMyIconFileDownloadNormal;
    case eAppletUploads:                    return eMyIconSendFileNormal;
    case eAppletOfferList:                  return eMyIconOfferList;
    case eAppletHostJoinRequestList:        return eMyIconPersonsOfferList;
    case eAppletHostSelect:                 return eMyIconPersonsOfferList;
    case eAppletPersonOfferList:            return eMyIconPersonsOfferList;
    case eAppletPopupMenu:	                return eMyIconMenu;

    case eAppletCamClient:	                return eMyIconClientWebCam;
    case eAppletCamSettings:	            return eMyIconCamSetupNormal;
    case eAppletSoundSettings:	            return eMyIconSoundSettings;

    case eAppletClientShareFiles:		    return eMyIconFileServer;

    case eAppletAboutMeServerViewMine:		return eMyIconShareAboutMeEye;
    case eAppletCamServerViewMine:		    return eMyIconShareWebCamEye;
    case eAppletFileShareServerViewMine:    return eMyIconShareFilesEye;
    case eAppletStoryboardServerViewMine:   return eMyIconShareStoryboardEye;

    case eAppletEditAboutMe:                return eMyIconAboutMeEdit;
    case eAppletEditStoryboard:             return eMyIconStoryboardEdit;

    case eAppletHackerList:	                return eMyIconIgnored;
    case eAppletSocketList:	                return eMyIconNetworking;
    case eAppletIsPortOpenTest:	            return eMyIconNetworking;

    case eAppletUserConnections:	        return eMyIconUserConnections;

    // case eAppletStreamViewerVideo:		return eMyIconVideoStreamViewer;
    // case eAppletStreamViewerCam:		return eMyIconCamStreamViewer;
    // case eAppletStreamViewerMusic:		return eMyIconMusicStreamPlayer;
    // case eAppletRemoteControl:			return eMyIconRemoteControl;
    // case eAppletPlugins:				return eMyIconPlugins;

	case ePluginAppletCamProvider:		    return eMyIconCamStreamProvider;
	case ePluginAppletNlcStation:		    return eMyIconNlcStation;
	case ePluginAppletNlcNetworkHost:	    return eMyIconServiceHostNetwork;

	case eAppletUnknown:
	default:
		return eMyIconUnknown;
	}
}

//============================================================================
QString DescribeApplet( EApplet applet )
{
	switch( applet )
	{
    case eActivityAppSetup:				    return QObject::tr( "Application Initialize" );
    case eAppletEditAboutMe:		        return QObject::tr( "Edit My About Me Page" );
    case eAppletEditAvatarImage:		    return QObject::tr( "Edit My Avatar Image" );
    case eAppletEditStoryboard:		        return QObject::tr( "Edit My Story Board (Blog)" );
    case eAppletStoryboardClient:		    return QObject::tr( "View Story Page (Blog)" );
    case eAppletStoryboardServerViewMine:   return QObject::tr( "View My Story Page (Blog)" );
    case eAppletSoundSettings:		        return QObject::tr( "Sound Settings" );
    case eAppletAboutMeClient:              return QObject::tr( "View About Me Page" );
    case eAppletAboutMeServerViewMine:      return QObject::tr( "View My About Me Page" );

    case eAppletHomePage:				    return QObject::tr( "Home" );
    case eAppletAboutNoLimitConnect:        return QObject::tr( "About No Limit Connect" );
    case eAppletApplicationInfo:            return QObject::tr( "Application Information" );
    case eAppletBrowseFiles:			    return QObject::tr( "Browse Files" );
    case eAppletOfferList:                  return QObject::tr( "Offer List" );

    case eAppletPlayerCamClip:              return QObject::tr( "Cam Video Player" );
    case eAppletPlayerPhoto:                return QObject::tr( "Photo Viewer" );
    case eAppletPlayerNlc:                  return QObject::tr( "Media Player" );

    case eAppletCamClient:                  return QObject::tr( "View Shared Web Cam" );
    case eAppletCamServerViewMine:          return QObject::tr( "View My Shared Web Cam" );
    case eAppletCamSettings:                return QObject::tr( "Cam Settings" );
    case eAppletChatRoomJoin:               return QObject::tr( "Join Chat Room" );
    case eAppletChatRoomJoinSearch:			return QObject::tr( "Search For Chat Room To Join" );
    case eAppletChatRoomListLocalView:		return QObject::tr( "Chat Room Hosts Announced To Network" );

    case eAppletChooseThumbnail:		    return QObject::tr( "Choose Thumbnail" );
    case eAppletChooseUser:		            return QObject::tr( "Choose User" );

    case eAppletChatRoomClient:		        return QObject::tr( "Chat Room" );
    case eAppletRandomConnectClient:		return QObject::tr( "Random Person Connect" );
    case eAppletAvatarImageClient:          return QObject::tr( "Get Persons Avatar Image For Contact List" );
    case eAppletCreateAccount:			    return QObject::tr( "Create Account" );
    case eAppletFriendListClient:	        return QObject::tr( "Friend List" );
    case eAppletGalleryEmoticon:			return QObject::tr( "Emoticon Gallery" );
    case eAppletGalleryImage:			    return QObject::tr( "Image Gallery" );
    case eAppletGalleryThumb:			    return QObject::tr( "Thumbnail Gallery" );
    case eAppletGetStarted:			        return QObject::tr( "Get Started" );

    case eAppletGroupClient:                return QObject::tr( "Group Client" );
    case eAppletGroupHostAdmin:             return QObject::tr( "Group Host Admin" );
    case eAppletGroupJoin:                  return QObject::tr( "Join Group" );
    case eAppletGroupJoinSearch:			return QObject::tr( "Search For Group To Join" );
    case eAppletGroupListClient:	        return QObject::tr( "Group List" );
    case eAppletGroupListLocalView:			return QObject::tr( "Group Hosts Announced To Network" );

    case eAppletHelpNetSignalBars:			return QObject::tr( "Network Signal Bars" );

    case eAppletChatRoomHostAdmin:          return QObject::tr( "Chat Room Host Admin" );
    case eAppletHostChatRoomStatus:         return QObject::tr( "Chat Room Host Status" );
    case eAppletHostGroupStatus:            return QObject::tr( "Group Host Status" );
    case eAppletHostNetworkStatus:          return QObject::tr( "Network Host Status" );
    case eAppletHostRandomConnectStatus:    return QObject::tr( "Random Connect Host Status" );

    case eAppletIgnoredHosts:               return QObject::tr( "Ignored Hosts List" );

    case eAppletInviteAccept:               return QObject::tr( "Open Invite" );
    case eAppletInviteCreate:               return QObject::tr( "Create Invite" );
    case eAppletInvites:                    return QObject::tr( "Invites" );

    case eAppletLibrary:				    return QObject::tr( "My Library Of Media" );
    case eAppletLog:                        return QObject::tr( "Application Log" );
    case eAppletLogSettings:		        return QObject::tr( "Log Settings" );
    case eAppletLogView:	                return QObject::tr( "View Log" );
	case eAppletMessengerFrame:				return QObject::tr( "Messenger" );
    case eAppletMultiMessenger:				return QObject::tr( "Messenger" );
    case eAppletNetworkSettings:		    return QObject::tr( "Network Settings" );
    case eAppletHostJoinRequestList:        return QObject::tr( "Join Host Requests" );
    case eAppletHostSelect:                 return QObject::tr( "Select A Host" );
    case eAppletPersonOfferList:            return QObject::tr( "Persons Offers" );
	case eAppletPlayerVideo:			    return QObject::tr( "Video Player" );
    case eAppletPersonalRecorder:		    return QObject::tr( "Personal Notes And Records" );

    case eAppletRandomConnectHostAdmin:     return QObject::tr( "Random Connect Host Admin" );
    case eAppletRandomConnectJoin:          return QObject::tr( "Join Random Connect" );
    case eAppletRandomConnectJoinSearch:	return QObject::tr( "Search For Random Connect Host To Join" );
    case eAppletRandomConnectListLocalView:	return QObject::tr( "Random Connect Announced Hosts" );

    case eAppletSnapshot:				    return QObject::tr( "Take Snapshot" );

    case eAppletTestAndDebug:			    return QObject::tr( "Test And Debug" );
    case eAppletTestHostClient:			    return QObject::tr( "Test Host Client" );
    case eAppletTestHostService:			return QObject::tr( "Test Host Service" );
    case eAppletTestUpnp:			        return QObject::tr( "Test UPNP Port Forward" );
	case eAppletTheme:					    return QObject::tr( "Theme" );

    case eAppletUserConnections:			return QObject::tr( "User Connections" );
    case eAppletUserIdentity:			    return QObject::tr( "User Identity" );
    case eAppletUserPreferences:            return QObject::tr( "User Settings" );

    case eAppletSettingsPage:				return QObject::tr( "Settings" );

    case eAppletSearchPage:				    return QObject::tr( "Search" );
    case eAppletSearchPersons:			    return QObject::tr( "Search Group For Person Name" );
    case eAppletSearchMood:			        return QObject::tr( "Search Group For Mood Message" );
    case eAppletScanAboutMe:			    return QObject::tr( "Search Group For About Me Pictures" );
    case eAppletScanStoryboard:			    return QObject::tr( "Search Group For Story Boards" );
    case eAppletScanSharedFiles:		    return QObject::tr( "Search Group For Shared Files" );
    case eAppletScanWebCam:				    return QObject::tr( "Search Group For Shared Web Cams" );

    case eAppletNetHostingPage:             return QObject::tr( "Hosting Services" );
    case eAppletPermissionList:             return QObject::tr( "Share Permission List" );

    case eAppletShareServicesPage:          return QObject::tr( "Share With Others Services" );
    case eAppletServiceAboutMe:             return QObject::tr( "Share About Me Page" );
    
    case eAppletServiceConnectionTest:      return QObject::tr( "Service Connection Test" );
    case eAppletServiceHostNetwork:         return QObject::tr( "Host A NoLimitConnect Network" );
    case eAppletServiceShareFiles:			return QObject::tr( "Share Media Files" );
    case eAppletServiceShareWebCam:         return QObject::tr( "Share your web cam" );
    case eAppletServiceStoryboard:          return QObject::tr( "Share Story Page (Blog)" );

    case eAppletServiceSettings:            return QObject::tr( "Service Settings" );

    case eAppletSettingsAboutMe:            return QObject::tr( "About Me Page Settings" );
    case eAppletSettingsAvatarImage:        return QObject::tr( "Avatar Image Settings" );
    case eAppletSettingsWebCamServer:       return QObject::tr( "Shared Web Cam Settings" );
    case eAppletSettingsConnectTest:        return QObject::tr( "Connection Test Service Settings" );
    case eAppletSettingsShareFiles:         return QObject::tr( "Shared Files Settings" );
    case eAppletSettingsFileXfer:           return QObject::tr( "Person To Person File Xfer Settings" );

    case eAppletSettingsHostChatRoom:       return QObject::tr( "Chat Room Host Settings" );
    case eAppletSettingsHostGroup:          return QObject::tr( "Group Host Settings" );
    case eAppletSettingsHostNetwork:        return QObject::tr( "Network Host Settings" );
    case eAppletSettingsHostRandomConnect:  return QObject::tr( "Random Connect Host Settings" );

    case eAppletSettingsMessenger:          return QObject::tr( "Messenger Settings" );
    case eAppletSettingsPushToTalk:         return QObject::tr( "Push To Talk Settings" );
    case eAppletSettingsRandomConnect:      return QObject::tr( "Random Connect Service Settings" );
    case eAppletSettingsStoryboard:         return QObject::tr( "Shared Story (Blog) Settings" );
    case eAppletSettingsTruthOrDare:        return QObject::tr( "Truth Or Dare Video Chat Game Settings" );
    case eAppletSettingsVideoPhone:         return QObject::tr( "Video Chat Settings" );
    case eAppletSettingsVoicePhone:         return QObject::tr( "Voice Only Phone Call Settings" );

	case ePluginAppletCamProvider:		    return QObject::tr( "Provide Cam Stream" );
	case ePluginAppletNlcStation:		    return QObject::tr( "Manage Station" );
	case ePluginAppletNlcNetworkHost:	    return QObject::tr( "Manage Station Network Host" );

    case eAppletPeerChangeFriendship:	    return QObject::tr( "Change Friendship" );
    case eAppletPeerReplyOfferFile:         return QObject::tr( "Reply Offer File" );
    case eAppletPeerTruthOrDare:            return QObject::tr( "Truth Or Dare Game" );
    case eAppletPeerVideoPhone:             return QObject::tr( "Video Chat" );
    case eAppletFileShareClientView:        return QObject::tr( "View Shared Files" );
    case eAppletFileOfferSelect:            return QObject::tr( "Offer File" );
    case eAppletOfferSend:                  return QObject::tr( "Send Offer" );
    case eAppletOfferResponse:              return QObject::tr( "Respond To Offer" );
    case eAppletOfferResponseAccept:        return QObject::tr( "Accept Offer Response" );
    case eAppletOfferView:                  return QObject::tr( "Offer Details" );
    case eAppletPeerVoicePhone:             return QObject::tr( "Voice Phone (VOIP)" );

    case eAppletPeerSessionFileOffer:       return QObject::tr( "File Offer" );

    case eAppletDownloads:                  return QObject::tr( "Downloads" );
    case eAppletUploads:                    return QObject::tr( "Uploads" );

    case eAppletFileShareServerViewMine:	return QObject::tr( "View My Shared Files" );

    case eAppletPopupMenu:                  return QObject::tr( "Menu" );

    case eAppletHackerList:                 return QObject::tr( "Blocked Users" );
    case eAppletSocketList:                 return QObject::tr( "Socket Connections" );
    case eAppletIsPortOpenTest:             return QObject::tr( "Test Connection" );

	case eMaxBasicApplets:
	case eAppletUnknown:
	default:
		return QObject::tr( "Unknown" );
	}
}
