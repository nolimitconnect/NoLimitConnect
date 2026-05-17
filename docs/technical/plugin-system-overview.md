---
title: Plugin System Overview
summary: Technical overview of the NoLimitConnect plugin architecture, plugin types, categories, and extensibility for developers and hackers.
---

# NoLimitConnect Plugin System Overview

## Introduction

NoLimitConnect supports a flexible plugin architecture, allowing for a wide range of features and extensibility. Plugins are modular components that provide specific services, user experiences, or network capabilities. The system is designed to be extensible, with up to 48 announced plugin slots (0–47) available for permission-based plugins, of which 17 are currently in use. This leaves 31 slots open for new plugin development.

This document provides an overview of the current plugin types, their categories, and how they fit into the NoLimitConnect ecosystem. It is intended for developers and hackers interested in extending the platform.

---

## Plugin Categories

Plugins are grouped into four main categories:

### 1. Network Services

These plugins provide core network infrastructure and are unlikely to change.

- **HostConnectTest (1):** Connection Test Service (Host)
- **HostNetwork (2):** Master network hosting

### 2. User Hosts

These plugins enable users to host or join various types of sessions.

- **HostChatRoom (3):** Chat room hosting
- **HostGroup (4):** Group hosting
- **HostRandomConnect (5):** Random connect to another person (host)
- **HostPeerUser (6):** Peer user host (mainly for avatar image)

### 3. Peer Server/Client

These plugins provide server or client functionality for specific features.

- **AboutMePageServer (7):** "About Me" web page plugin (server)
- **Messenger (8):** Text, voice, and video messaging (session-based)
- **PushToTalk (9):** VOIP audio push-to-talk
- **PersonFileXfer (10):** Person-to-person file transfer
- **CamServer (11):** Web cam broadcast plugin
- **FileShareServer (12):** Shared files server
- **StoryboardServer (13):** User-editable storyboard web page server
- **TruthOrDare (14):** Video chat "Truth or Dare" game
- **VideoChat (15):** Video chat with motion detection and recording
- **VoicePhone (16):** VOIP audio-only phone call
- **FriendRequest (17):** Controls who can send join host or friend requests

### 4. Client Plugins

Each host plugin typically has a corresponding client plugin (slots 49–58), enabling users to join or interact with hosted services. For example:

- **ClientConnectTest (49):** Connection Test Client
- **ClientNetwork (50):** Network client
- **ClientChatRoom (51):** Chat room user client
- **ClientGroup (52):** Group client
- **ClientRandomConnect (53):** Random connect client
- **ClientPeerUser (54):** Peer user client
- **AboutMePageClient (55):** "About Me" web page client
- **CamClient (56):** Cam server client
- **FileShareClient (57):** Shared files client
- **StoryboardClient (58):** Storyboard web page client

---

## Plugin Slot Allocation

- **Announced Plugins:** Slots 0–47 are reserved for plugins that are announced with permission levels. These are visible to the network and can be used for access control.
- **Unused Slots:** Of the 48 announced slots, 17 are currently used, leaving 31 available for new plugins.
- **Internal Plugins:** Slots 48+ are reserved for internal or non-announced plugins.

---

## Extending the Plugin System

Developers are encouraged to propose and implement new plugins. Potential areas for new plugins include:

- New communication modes (e.g., whiteboard, collaborative editing)
- Advanced media sharing or streaming
- Security and privacy enhancements
- Integration with external services or protocols

When adding a new plugin:
- Assign it to an unused slot within the announced range (0–47) if it requires permission management.
- Update the EPluginType enum and related documentation.
- Implement corresponding DescribePluginType and GetPluginName functions for UI and database integration.

---

## Current Plugin Type Reference

| Enum Name                  | Value | Description                                      | Category         |
|----------------------------|-------|--------------------------------------------------|------------------|
| ePluginTypeInvalid         | 0     | Unknown or disabled                              | -                |
| ePluginTypeHostConnectTest | 1     | Connection Test Service (Host)                   | Network Services |
| ePluginTypeHostNetwork     | 2     | Master network hosting                           | Network Services |
| ePluginTypeHostChatRoom    | 3     | Chat room hosting                                | User Hosts       |
| ePluginTypeHostGroup       | 4     | Group hosting                                    | User Hosts       |
| ePluginTypeHostRandomConnect| 5    | Random connect host                              | User Hosts       |
| ePluginTypeHostPeerUser    | 6     | Peer user host (avatar image)                    | User Hosts       |
| ePluginTypeAboutMePageServer| 7    | "About Me" web page server                       | Peer Server      |
| ePluginTypeMessenger       | 8     | Messaging (text, voice, video, games)            | Peer to Peer     |
| ePluginTypePushToTalk      | 9     | VOIP push-to-talk                                | Peer to Peer     |
| ePluginTypePersonFileXfer  | 10    | Person-to-person file transfer                   | Peer to Peer     |
| ePluginTypeCamServer       | 11    | Web cam broadcast                                | Peer Server      |
| ePluginTypeFileShareServer | 12    | Shared files server                              | Peer Server      |
| ePluginTypeStoryboardServer| 13    | Storyboard web page server                       | Peer Server      |
| ePluginTypeTruthOrDare     | 14    | Video chat "Truth or Dare" game                  | Peer to Peer     |
| ePluginTypeVideoChat       | 15    | Video chat with motion detection/recording       | Peer to Peer     |
| ePluginTypeVoicePhone      | 16    | VOIP audio-only phone call                       | Peer to Peer     |
| ePluginTypeFriendRequest   | 17    | Friend request management                        | Peer to Peer     |

*Client plugin types (49–58) mirror the host/server plugins for user-side functionality.*

---

## Reserved and Internal Plugin Types

- **eMaxPermissionPluginType (18):** Marks the end of permission plugins.
- **eMaxAnnouncedPluginType (48):** Marks the end of announced plugins.
- **ePluginTypeMaxNetRange (255):** End of network-transmittable plugin types.
- **ePluginTypeInternalStart (256):** Start of internal-only plugin types.

---

## How to Add a New Plugin

*This section will be expanded with a step-by-step guide for plugin development, registration, and integration.*

---

## Conclusion

The NoLimitConnect plugin system is designed for extensibility and innovation. With many available slots and a clear structure, developers and hackers are encouraged to contribute new ideas and features to the platform.

---
