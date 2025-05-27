//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityInformation.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QClipboard>

#include "ui_ActivityInformation.h"

TitleBarWidget *  ActivityInformation::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget * ActivityInformation::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityInformation::ActivityInformation( AppCommon& app, QWidget* parent, EPluginType pluginType )
: ActivityBase( OBJNAME_ACTIVITY_INFORMATION, app, parent, eAppletActivityDialog )
, ui(*(new Ui::InformationDialog))
, m_PluginType( pluginType )
{
    initActivityInformation();
}

//============================================================================
ActivityInformation::ActivityInformation( AppCommon& app, QWidget* parent, EInfoType infoType )
    : ActivityBase( OBJNAME_ACTIVITY_INFORMATION, app, parent, eAppletActivityDialog )
    , ui(*(new Ui::InformationDialog))
    , m_InfoType( infoType )
{
    initActivityInformation();
}

//============================================================================
void ActivityInformation::initActivityInformation( void )
{
    ui.setupUi( this );
    ui.m_TitleBarWidget->setTitleBarText( QObject::tr( "Information " ) );
    connect( ui.m_ClipboardCopyWidget, SIGNAL(clicked()), this, SLOT(slotCopyToClipboardButtonClicked()) );

    connectBarWidgets();
}

//============================================================================
void ActivityInformation::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    updateInformation();
}

//============================================================================
void ActivityInformation::slotCopyToClipboardButtonClicked( void )
{
    ui.m_ClipboardCopyWidget->copyToClipboard( ui.m_InfoText->toPlainText() );
    okMessageBox( QObject::tr( "Clipboard" ), QObject::tr( "Text was copied to clipboard" ) );
}

//============================================================================
void ActivityInformation::updateInformation( void )
{
    ui.m_PictureLabel->setVisible( false );
    ui.m_ServiceInfoButton->setFixedSize( eButtonSizeLarge );

    if( m_PluginType != ePluginTypeInvalid )
    {
        ui.m_ServiceInfoButton->setIcon( m_MyApp.getMyIcons().getPluginIcon( m_PluginType ) );

        switch( m_PluginType )
        {
        case ePluginTypeHostChatRoom:
        case ePluginTypeHostGroup:
        case ePluginTypeHostNetwork:
        case ePluginTypeHostRandomConnect:
            ui.m_PictureLabel->setResourceImage( ":/AppRes/Resources/NetworkDesign.png", true );
            ui.m_PictureLabel->setVisible( true );
            break;
        default:
            break;
        }
    }

    switch( m_InfoType )
    {
    case eInfoTypePermission:
        ui.m_ServiceInfoButton->setIcon( eMyIconPermissions );
        break;
    case eInfoTypeNetworkKey:
        ui.m_ServiceInfoButton->setIcon( eMyIconNetworkKey );
        break;
    case eInfoTypeConnectTestUrl:
        ui.m_ServiceInfoButton->setIcon( eMyIconServiceConnectionTest );
        break;
    case eInfoTypeConnectTestSettings:
        ui.m_ServiceInfoButton->setIcon( eMyIconSettingsConnectionTest );
        break;

    case eInfoTypeNetworkHost:
        ui.m_PictureLabel->setResourceImage( ":/AppRes/Resources/NetworkDesign.png", true );
        ui.m_PictureLabel->setVisible( true );
        ui.m_ServiceInfoButton->setIcon( eMyIconServiceHostNetwork );
        break;
    case eInfoTypeRandomConnectUrl:
        ui.m_ServiceInfoButton->setIcon( eMyIconServiceRandomConnect );
        break;
    case eInfoTypeDefaultGroupHostUrl:
        ui.m_ServiceInfoButton->setIcon( eMyIconSettingsHostGroup );
        break;
    case eInfoTypeDefaultChatRoomHostUrl:
        ui.m_ServiceInfoButton->setIcon( eMyIconSettingsChatRoom );
        break;
    default:
        ui.m_ServiceInfoButton->setIcon( eMyIconInformation );
        break;
    }

    ui.m_InfoText->clear();
    ui.m_InfoText->appendPlainText( getInfoText() );
}

QString ActivityInformation::m_NoInfoAvailable( QObject::tr( "No Information is localy available. please visit https://nolimitconnect.com for latest infomation and help" ) );

QString ActivityInformation::m_NetworkDesign( QObject::tr(
    "=== NETWORK DESIGN ===\n"
    "NOTE: A VPN with port forwarding feature is suggested\n"
    "1.) An Open Port is required for Hosting and recommended for direct connection between users\n"
    "2.) If you plan to host a separate network then a VPN with a fixed address is recommended\n"
    "Pure VPN is a VPN with Port Forwarding and fixed IP Address as pay for add-in features\n"
    " *USER HOST LISTING SERVICE: Provides List of Group, Chat Room and Random Connect Hosts and thier address for users to connect to and search or join.\n"
    " *CONNECTION TEST SERVICE: Service for devices to discover thier web IP Address and if they have a open port for direct connection or require RELAY SERVICE*.\n"
    "\n"
    "=== TIER 1: NOLIMITCONNECT NETWORK HOST SERVICE* ===\n"
    "NOTE: This service requires a fixed IP Address or a DNS url like http://www.nolimitconnect.net\n"
    "REQUIRED SERVICES* AND SUGGESTED PERMISSION LEVELS*\n"
    " NOLIMITCONNECT NETWORK SERVICE - ANYBODY\n"
    " CONNECTION TEST SERVICE - ANYBODY\n"
    "\n"
    "=== TIER 2: USER HOSTED SERVICES* ===\n"
    "NOTE: Users Require and open port to provide hosting\n"
    " GROUP HOSTING - provided communication services for members of the group\n"
    " CHAT ROOM HOSTING - provides chat room for members\n"
    " RANDOM CONNECT HOSTING - provides a way for anyone to connect to anyone (Recommend Anybody Permission Level)\n"
    "\n"
    "=== TIER 3: PERSON TO PERSON Services* ===\n"
    " Friends that can direct connect with open port will have faster communication and less loading of the host\n"
    " Those that cannot direct connect will use whichever host they are connected to as a relay\n"
    " When using relay a user can only communicate with another member of thier host while member is connected to host\n"
    "\n"
    "Person To Person and Person To Host Members And Friends services\n"
    " *ABOUT PAGE SERVICE - Provide a information page about a host or person\n"
    " *CAM STREAM SERVICE: A host or user can provide streaming video live from their device camera or cam\n"
    " *FILE SHARE PLUGIN: Provides user or host file sharing with other users.\n"
    " *FILE TRANSFER PLUGIN: Provides user file transfer to another person.\n"
    " *PUSH TO TALK PLUGIN: Provides user with voice communiction with one button push to talk.\n"
    " *STORY BOARD PAGE SERVICE - Provide a blog like page a host or person can post to for others to view/read\n"
    " *TRUTH OR DARE PLUGIN: provides truth or dare video chat game between individuals\n"
    " *VIDEO CHAT PLUGIN: Provides user video chat to others using the NoLimitConnect app and internet.\n"
    " *VOICE PHONE PLUGIN: Provides user with voice phone calling to others using the NoLimitConnect app and internet.\n"
    "\n"

    "\n"
) );

QString ActivityInformation::m_PluginDefinitions( QObject::tr(
    "\n"
"DEFINITIONS:\n"
" *ABOUT PAGE SERVICE - Provide a information page about a host or person\n"
" *CAM STREAM SERVICE: A host or user can provide streaming video live from their device camera or cam\n"
" *CHAT ROOM HOST SERVICE: Provides group based text chat service.\n"
" *CONNECTION TEST SERVICE: Provide user with connection test to determine users IP address and if has an open port or requires relay\n" 
" *GROUP HOST SERVICE: Provides connection and other services to members who have joined the Group Host.\n"
" *FILE SHARE PLUGIN: Provides user or host file sharing with other users.\n"
" *FILE TRANSFER PLUGIN: Provides user file transfer to another person.\n"
" *NETWORK HOST SERVICE: Provides and user host listing for search of hosts to join.\n"
" *PERMISSION LEVELS: Friendship level required to be allowed to use a service or connect to a person.\n"
" *PUSH TO TALK PLUGIN: Provides user with voice communiction with one button push to talk.\n"
" *RANDOM CONNECT HOST SERVICE: Provides users who join to communicate with others.\n"
" *SEARCH AND SCAN SERVICES: provide user a list of known persons with the desired search critera or service or stream or files\n"
" *STORY BOARD PAGE SERVICE - Provide a blog like page a host or person can post to for others to view/read\n"
" *TRUTH OR DARE PLUGIN: provides truth or dare video chat game between individuals\n"
" *VIDEO CHAT PLUGIN: Provides user video chat to others using the NoLimitConnect app and internet.\n"
" *VOICE PHONE PLUGIN: Provides user with voice phone calling to others using the NoLimitConnect app and internet.\n"
) );

QString ActivityInformation::m_Permissions( QObject::tr(
    "=== PERMISSION LEVELS ===\n"
    " Permission Levels are used for setting either what level of permission is required to access a plugin or"
    " the permission level granted to another person to control what that person has access to."
    "\n"
    "\n"
    " NOTE 1: If you join a group then other members of that group are granted Guest Permission Level automatically and"
    " you can grant individual users a higher permission level if you want to."
    "\n"
    "\n"
    " NOTE 2: If you put your device in RANDOM CONNECT CONNECT mode then others that are in RANDOM CONNECT CONNECT mode"
    " are granted Guest Permission Level automatically and you can grant individual users a higher permission level if you want to."
    "\n"
    "\n"
    "=== ADMINISTRATOR PERMISSION ===\n"
    " This is the highest level of permission.\n"
    " A person granted this permission level can access plugins or services set to permission level Admistrator, Friend, Guest or Anybody.\n"
    " A example of usage would be if you wanted to set up a web cam stream service as a nanny or security cam that only you can access."
    " You would set the web cam service to permission level admistrator and only grant your self administrator permission level."
    "\n"
    "\n"
    "=== FRIEND PERMISSION ===\n"
    " A person granted this permission level can access plugins or services set to permission level Friend, Guest or Anybody.\n"
    " A example of usage would be if you wanted other members of a group you joined to be able to message you and view your about page"
    " but not be able to access your shared files."
    " You would set the file share service to permission level to friend ( or higher ) and set messenger and about page to guest level permission."
    "\n"
    "\n"
    "=== GUEST PERMISSION ===\n"
    " A person granted this permission level can access plugins or services set to permission level Guest or Anybody.\n"
    " A example of usage would be granting a person guest permission even if not a member of your group."
    "\n"
    "\n"
    "=== ANYBODY PERMISSION ===\n"
    " This permission grants anybody that can connect to you access to plugin or service set to this level."
    " A example of usage would be setting STORY BOARD service to anybody so everyone can see your story."
    "\n"
    "\n"
    "=== NOBODY (DISABLED or IGNORED) PERMISSION ===\n"
    " This permission level when applied to a person means that this person will be ignored and cannot access"
    " any of your services or plugins.\n"
    " This permission level when applied to a plugin or service means that the plugin or sevice will be completely"
    " disabled and nobody can access that plugin or service.\n"
    "\n"
) );

QString ActivityInformation::m_NetworkKey( QObject::tr(
    "=== NETWORK KEY ===\n"
    "The network key is a text string used for user data network encryption.\n"
    "The network key should only be changed if connecting to or hosting a private network seperate from NoLimitConnect.\n"
    "If the network key is changed then connecting to the NoLimitConnect network will no longer be possible.\n"
    "A private network can be hosted/connected to without changing the network key, however, changing the "
    "network key will give you the best privacy NoLimitConnect has to offer.\n"
    "\n"
    "\n"
    "Use of a VPN is recommended to improve your privacy."
) );

QString ActivityInformation::m_NetworkHost( QObject::tr(
    "=== NETWORK HOST ===\n"
    "The network host provides group host listing and connection test services for a PtoP Network.\n"
    "The network host URL should only be changed if connecting to or hosting a private network seperate from NoLimitConnect.\n"
    "The network host URL can be one of two formats.\n"
    "\n"
    "Format 1 using host web name and port\n"
    " Example 1 ptop://www.nolimitconnect.net:45124\n"
    "\n"
    "Format 2 using host external IP Adrress and port\n"
    " Example 2 ptop://111.122.133.144:45124\n"
    "\n"
    "NOTE 1: The network host IP port is normally 45124 but can be any open port.\n"
    "NOTE 2: The network host address must eitehr be a fixed/static IP or be a URL\n"
    " that can be resolved to a IP using DNS ( Domain Name Service ).\n"
) );

QString ActivityInformation::m_ConnectTestUrl( QObject::tr(
    "=== CONNECTION TEST URL ===\n"
    "The connection test service provides services to test if your device's port is open.\n"
    "If your port is open then others can connect directly to your device.\n"
    "A open port provides highest possible speed of connect and data transfer.\n"
    "A blocked port requires the device to use a relay service which greatly limits speed and number of connections possible.\n"
    "\n"
    "Format 1 using host web name and port\n"
    " Example 1 ptop://www.nolimitconnect.net:45124\n"
    "\n"
    "Format 2 using host external IP Adrress and port\n"
    " Example 2 ptop://111.122.133.144:45124\n"
    "\n"
) );

QString ActivityInformation::m_ConnectTestSettings( QObject::tr(
    "=== Enable UPNP check box ===\n"
    "If enabled then UPNP protocol will be used to attempt to open a port to your devcice\n"
    "UPNP works well with Hide.me VPN\n"
    "If you are not using a VPN with Port Forwarding you can try to configure Port Forward on your Router\n"
    "Because of the various implementations of router firmware this only sometimes is succesfull in opening your port  \n"
    "Consult your router specific instructions for how to forward a port to your device or visit\n"
    "https://www.wikihow.com/Open-Ports\n\n"
    "=== CONNECTION TEST SETTINGS ===\n\n"
    "Option 1 - Use connection test service.\n"
    "This is the most reliable and recommended method of determining your external IP Address and\n"
    "determining if your device's port is open.\n"
    "\n"
    "Option 2 - Specify your external IP Address and assume your port is open.\n"
    "This setting is only recommended if you have a fixed IP Address because\n"
    "your IP Address may change at the whim of your Internet Service Provider.\n"
    "You can determine your IP Address by doing a google search for \"what is my ip address\"\n"
    "\n"
    "Option 3 - Always assume your device's port is blocked.\n"
    "This option is only recommended for users that always use mobile data service and do not use a VPN.\n"
    "It will bypass testing your connection\n"
    "\n"
    "\n"
) );

QString ActivityInformation::m_RandomConnectUrl( QObject::tr(
    "=== RANDOM CONNECT SERVICE URL ===\n"
    "Provides Service of listing/connecting 2 Persons using the random connect feature.\n"
    "The person listed could be anyone in the world also using the random connect feature.\n"
    "Random connect works by listing persons who pressed the random connect button.\n"
    "Within 20 seconds of the random connect button press.\n"
) );

QString ActivityInformation::m_DefaultGroupHostUrl( QObject::tr(
    "=== Default GROUP HOST URL ===\n"
    "No Limit Connect will attempt to connect the this group\n"
    "When Log In is completed.\n"
    "If connect fails or no url is provided then you can search for a Group to join\n"
) );

QString ActivityInformation::m_DefaultChatRoomHostUrl( QObject::tr(
    "=== Default CHAT ROOM HOST URL ===\n"
    "No Limit Connect will attempt to connect the this Chat Room\n"
    "When Log In is completed.\n"
    "If connect fails or no url is provided then you can search for a Chat Room to join\n"
) );

QString ActivityInformation::m_NetworkSettingsInvite( QObject::tr(
    "=== Network Settings Invite ===\n"
    "By Accepting A Network Settings Invite that changes the Network Host URL:\n"
    "The user will no longer be able to connect to No Limit Connect Network.\n"
    "You should only change the Network Host URL and Network Key if you have\n"
    "A private network with a private Network Host server\n"
    "The Network Key is NOT part of a Invite\n\n"
    "Warning: If accepting a network host be sure you have the correct network key before saving the network settings\n"
    "If you do not have the correct network key you will get banned as a hacker\n"
) );

QString ActivityInformation::m_FriendList( QObject::tr(
    "=== Friends List ===\n"
    "A list showing users set to friend or administrator permission level.\n"
) );

QString ActivityInformation::m_IgnoredList( QObject::tr(
    "=== Ignored List ===\n"
    "A list showing ignored (blocked) users.\n"
    "You can unblock a user by clicking the friendship icon or select Change Friendship from the menu button on right side of list entry.\n"
) );

QString ActivityInformation::m_OfflineList( QObject::tr(
    "=== Offline Friends List ===\n"
    "A list showing offline friends and admins.\n"
    "You can change friendship even when offline (Set to anonymouse or guest so network no longer tries to stay connected).\n"
) );

QString ActivityInformation::m_UserHostRequrements( QObject::tr(
    "=== Hosting Requirements any of these host services Chat Room, Group, Random Connect ===\n"
    "You will need to port forward the listen port you specified in Network Settings\n"
    "The author of No Limit Connect uses https://hide.me VPN because:\n"
    "1. hide.me understands UPNP protocol which allows No Limit Connect to open a port automatically.\n"
    "2. hide.me can be installed on pretty much any OS including Windows, Linux, Android and Raspberry Pi.\n"
    "3. hide.me has a free version but is relatively inexpensive to buy.\n"
    "Articles about port forwarding can be found by search engine or at https://www.jguru.com/vpn-port-forwarding \n"   
) );

QString ActivityInformation::m_NetworkHostRequrements( QObject::tr(
    "=== Hosting Requirements Network Host ===\n"
    "If you want to host your own network the network host must have port forwarding and also a fixed ip address.\n"
) );

QString ActivityInformation::m_MaxMessageHistory( QObject::tr(
    "=== Maximum Message History ===\n"
    "Limits the message history stored to the specified maximum message history.\n"
    "The limit is applied per user for messenger.\n"
    "The limit is applied for the total messages recieved in chat room.\n"
    "The limit removes the message from history but does NOT delete any files such as photos and videos etc.\n"
    "To delete files recieved from a user you can delete them using the Library Applet.\n"
) );

QString ActivityInformation::m_Ipv6( QObject::tr(
    "=== IPv6 vs IPv4 ===\n"
    "IPv4 maximum addresses 4,294,967,296 (2^32)\n"
    "IPv6 maximum addresses 340,282,366,920,938,463,463,374,607,431,768,211,456 (2^128)\n"
    "IPv6 Protocol is different from IPv4 and a connection from IPv6 to IPv4 is not in the protocols.\n"
    "If you choose IPv6 then the Network Host and Connection Test Host must also be IPv6 enabled.\n"
    "This means the host listing on the IPv6 network might not have the same hosts listed as the IPv4 network.\n"
) );

QString ActivityInformation::m_FriendRequest( QObject::tr(
    "=== FRIEND REQUEST ===\n"
    " Friend request is a way for someone you do not know to request friendship or request to join your host."
    " Friend request is defaulted to Anonymous permission level, however, to avoid any requests by people you ."
    " do not know you can set it to disabled ."
    "\n"
    "\n"
) );

QString ActivityInformation::m_WhatIsAInvite( QObject::tr(
    "=== What Is A Invite ===\n"
    "A Invite is text that starts with !Invite! and typically is sent by email or text message\n"
    "A Invite allows users to join a host and/or private network\n"
    "In the case of a personal invites a user can directly connect to another user without joining a host\n"
    "If user is a friend with open port a attempt to reconnect will occur even after restart\n\n"
) );

//============================================================================
QString ActivityInformation::getInfoText( void )
{
    if( m_PluginType != ePluginTypeInvalid )
    {
        switch( m_PluginType )
        {
        case ePluginTypeHostChatRoom:
        case ePluginTypeHostGroup:
        case ePluginTypeHostNetwork:
        case ePluginTypeHostRandomConnect:
            return m_NetworkDesign + m_PluginDefinitions;
            break;
        case ePluginTypeFriendRequest:
            return m_FriendRequest;
            break;
        default:
            return m_NoInfoAvailable;
            break;
        }
    }

    switch( m_InfoType )
    {
    case eInfoTypePermission:
        return m_Permissions;
    case eInfoTypeNetworkKey:
        return m_NetworkKey;
    case eInfoTypeNetworkHost:
        return m_NetworkHost;
    case eInfoTypeConnectTestUrl:
        return m_ConnectTestUrl;
    case eInfoTypeConnectTestSettings:
        return m_ConnectTestSettings;
    case eInfoTypeRandomConnectUrl:
        return m_RandomConnectUrl;
    case eInfoTypeDefaultGroupHostUrl:
        return m_DefaultGroupHostUrl;
    case eInfoTypeDefaultChatRoomHostUrl:
        return m_DefaultChatRoomHostUrl;
    case eInfoTypeNetworkSettingsInvite:
        return m_NetworkSettingsInvite;

    case eInfoTypeFriendsList:
        return m_FriendList;
    case eInfoTypeIgnoredList:
        return m_IgnoredList;
    case eInfoTypeOfflineList:
        return m_OfflineList;

    case eInfoTypeGroupStatus:
    case eInfoTypeHostChatRoom:
    case eInfoTypeHostGroup:
    case eInfoTypeHostRandomConnect:
        return m_UserHostRequrements;
    case eInfoTypeHostNetwork:
        return m_NetworkHostRequrements + m_UserHostRequrements;
    case eInfoTypeMaxMessageHistory:
        return m_MaxMessageHistory;
    case eInfoTypeIpv6:
        return m_Ipv6;
    case eInfoTypeWhatIsAInvite:
        return m_WhatIsAInvite + m_NetworkSettingsInvite;
    default:
        return m_NoInfoAvailable;
    }
}
