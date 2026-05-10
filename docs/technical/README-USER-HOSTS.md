# User Hosts — Group, Chat Room, and Random Connect

User hosts are **services run by ordinary users** on their own machines.  
Any user with an open TCP port can start a user host.  
All three host types share the same announce/join/leave architecture through the common base
classes `PluginBaseHostService` and `HostServerMgr`.

| Host | Plugin type | Client plugin type | Host type enum |
|---|---|---|---|
| **GroupHost** | `ePluginTypeHostGroup` | `ePluginTypeGroupClient` | `eHostTypeGroup` |
| **ChatRoomHost** | `ePluginTypeHostChatRoom` | `ePluginTypeChatRoomClient` | `eHostTypeChatRoom` |
| **RandomConnectHost** | `ePluginTypeHostRandomConnect` | `ePluginTypeRandomConnectClient` | `eHostTypeRandomConnect` |

Source: `libs/libptopengine/Plugins/PluginGroupHost.cpp`,
`PluginChatRoomHost.cpp`, `PluginRandomConnectHost.cpp`.

---

## Shared Architecture

```
PluginBase
 └── PluginBaseMultimedia
      └── PluginBaseHostService          (common host logic)
           ├── HostServerMgr             (client list + announce sessions)
           │    └── HostServerSearchMgr  (announce lists per host type)
           │         └── HostBaseMgr     (connect-to-host helpers)
           └── PluginGroupHost / PluginChatRoomHost / PluginRandomConnectHost
```

`PluginBaseHostService` holds:

* `m_PktHostInviteAnnounceReq` — the current announce packet (built once, re-sent periodically)
* `m_HostServerMgr` — tracks connected clients and pending announce sessions
* `m_HostInviteUrl`, `m_HostTitle`, `m_HostDescription` — content from settings

---

## Phase 1 — Building and Sending the Announce

### Prerequisites
Before a host can announce it needs:
1. `isPluginEnabled()` — the plugin must be enabled in settings.
2. An open RX port — `m_Engine.getNetStatusAccum().isRxPortOpen()`.
3. A valid online URL — `m_PktHostInviteIsValid` must be `true`.

If the port-open test has not yet completed, `sendHostAnnounce()` returns silently.

### Building the announce packet (`buildHostAnnounce`)
Called from `setPluginSetting()` or `onPluginSettingChange()` when settings are saved.
Reads invite URL, title, description and thumbnail ID from the `PluginSetting` record and
serialises them into `m_PktHostInviteAnnounceReq` using `PktBlobEntry`.  
Sets `m_PktHostInviteIsValid = true` on success.

### Sending the announce (`sendHostAnnounce`)
Two paths:

**Path A — We are also the NetworkHost**
```
if( our online ID == NetworkHost online ID )
    call PluginNetworkHost::updateHostSearchList() directly (loopback connection, no TCP)
```

**Path B — Remote NetworkHost**
```
HostServerMgr::sendHostAnnounceToNetworkHost()
  → check isNetHostAvailable() and isRxPortOpen()
  → save copy of PktHostInviteAnnounceReq in m_AnnList[sessionId]
  → ConnectionMgr::connectToHost(eHostTypeNetwork, sessionId, url, eConnectReasonXxxAnnounce)
    → TCP connect + crypto handshake to NetworkHost
  → HostServerMgr::onConnectToHostSuccess()
    → txPacket(PktHostInviteAnnounceReq) to NetworkHost
    → ConnectionMgr::doneWithConnection()   (connection released immediately after sending)
```

`EConnectReason` values used for announce:
| Host type | Connect reason |
|---|---|
| GroupHost | `eConnectReasonGroupAnnounce` |
| ChatRoomHost | `eConnectReasonChatRoomAnnounce` |
| RandomConnectHost | `eConnectReasonRandomConnectAnnounce` |

### Re-announce timer
`onThreadOncePer15Minutes()` calls `sendHostAnnounce()` unconditionally (subject to preconditions
above). The NetworkHost drops entries that have not re-announced within 30 minutes
(`MIN_HOST_RX_UPDATE_TIME_MS`).

---

## Phase 2 — Client Discovery (Search)

A client plugin (e.g. `PluginGroupClient`) that wants to find available hosts calls
`HostClientMgr::sendHostSearchToNetworkHost()`.

```
Client node                          NetworkHost node
    |                                        |
    |--- TCP connect to NetworkHost -------->|
    |--- PktHostInviteSearchReq (hostType) ->|
    |<-- PktHostInviteSearchReply (blob) ----|  (up to N HostedInfo entries)
    |                                        |
    | if more results:                       |
    |--- PktHostInviteMoreReq -------------->|
    |<-- PktHostInviteMoreReply (blob) ------|
    | ...repeat until no more...             |
```

Each `PktHostInviteSearchReply` / `PktHostInviteMoreReply` carries a `PktBlobEntry` blob
containing serialised `HostedInfo` records. The client de-serialises them and adds entries to
its local list (`m_ChatRoomHostAnnList`, etc. on the client side).

`PktHostInviteSearchReq` supports targeting a **specific host** by setting
`m_SpecificOnlineId`, which the NetworkHost uses to look up a single entry rather than
returning the full list.

---

## Phase 3 — Joining a User Host

Once a client has the host's invite URL from the search result it connects directly to the
user host (not through NetworkHost).

```
Client node                          User Host node
    |                                        |
    |--- TCP connect to Host URL ----------->|
    |--- PktHostJoinReq (hostType, ...) ---->|
    |                                        | HostServerMgr::onJoinRequested()
    |                                        | → HostJoinMgr::onHostJoinRequestedByUser()
    |                                        |   permission check
    |<--- PktHostJoinReply (state) ----------|
```

`EMembershipState` values that can come back:
* `eMembershipStateJoinGranted` — client is now active in the host session.
* `eMembershipStateJoinRequested` — pending admin approval (host requires manual accept).
* `eMembershipStateJoinDenied` — permission refused.

On grant the host calls `HostServerMgr::onUserJoined()`, which adds the client's online ID to
`m_ClientList` and `m_ContactList`.

### Leaving / Un-joining
| Packet | Meaning |
|---|---|
| `PktHostLeaveReq` | Temporary departure (session ends but membership record kept) |
| `PktHostUnJoinReq` | Permanent removal of the join record from the host database |

Replies mirror the request type (`PktHostLeaveReply`, `PktHostUnJoinReply`).

---

## Phase 4 — Active Session Traffic

Once a client is joined, the host delivers session traffic to **all connected clients** via
`broadcastToClients(pkt, excludeId)`. The `excludeId` is typically the sender's own ID to
avoid echo.

### GroupHost and ChatRoomHost
Text chat, media streams, and user-info traffic is relayed through the host to all joined
members. The host maintains a `UserList` (also in `HostServerMgr`) that tracks all members
with their join state.

**User-info sync on join**  
When a new client connects, the host calls `sendMemberListToClient()` which sends `PktHostUserListReply` packets listing all currently connected members. The client then requests `PktHostUserInfoReq` for each member it does not yet know.

**Admin availability broadcast**  
When the host admin opens the host admin view:
```cpp
GroupieId groupieId( myOnlineId, myOnlineId, eHostTypeGroup );
PktAdminAvail pktAdminAvail;
pktAdminAvail.setAdminAvailable( adminIsViewing );
broadcastToClients( &pktAdminAvail, excludeId );
```

### RandomConnectHost
RandomConnectHost is a **relay** — it does not maintain persistent chat sessions.

**Random connect flow:**
```
UserA                      RandomConnectHost                     UserB
  |                                |                               |
  |--- PktRandConnectReq --------->|                               |
  |    (groupieId, toUserId,       |--- PktRandConnectReply ------>|
  |     randAction)                |    broadcast to ALL clients   |
  |                                |    (including UserA)          |
```

`PktRandConnectReq` fields:
* `m_RandAction` — `ERandAction` enum (e.g. offer to call, accept call, etc.)
* `m_UserOnlineId` / `m_HostOnlineId` — form the `GroupieId`
* `m_ToUserOnlineId` — the intended recipient (broadcast still goes to all; recipient filters by ID)

The host simply rebroadcasts `PktRandConnectReply` to every connected client with the same
fields. Clients filter by `toUserOnlineId` to determine if the action is for them.

---

## Connection Lifecycle Summary

```
Plugin startup
  └── pluginStartup() → PluginBaseHostService::pluginStartup()

Setting saved / plugin enabled
  └── buildHostAnnounce() → sendHostAnnounce()
                              └── connectToHost(NetworkHost)
                                    └── txPacket(PktHostInviteAnnounceReq)

Every 15 minutes
  └── onThreadOncePer15Minutes() → sendHostAnnounce()

Client connects (inbound TCP)
  └── onPktHostJoinReq()
       └── HostServerMgr::onJoinRequested()
            └── HostJoinMgr::onHostJoinRequestedByUser()  [permission check]
                 └── reply PktHostJoinReply
                      └── if granted → onUserJoined() → add to m_ClientList

Client sends message / media
  └── host's plugin packet handler
       └── broadcastToClients(pkt, excludeSender)

Client disconnects / goes offline
  └── onContactWentOffline() → onConnectionLost()
       └── HostServerMgr::removeClient()
       └── broadcast departure notice to remaining clients
```

---

## Membership and Permission Checks

`getMembershipState(VxNetIdent*)` on each host plugin checks:

1. `getPluginPermission()` — if `eFriendStateIgnore` the host is disabled; returns `eMembershipStateJoinDenied`.
2. Delegates to `HostServerMgr::getMembershipState()` which looks up the user's `EJoinState`
   in `HostJoinInfoDb`.

The join database persists between sessions so returning members are recognised immediately.

---

## Packet Reference

| Packet class | Direction | Purpose |
|---|---|---|
| `PktHostInviteAnnounceReq` | UserHost → NetworkHost | Register / refresh host listing |
| `PktHostInviteAnnounceReply` | NetworkHost → UserHost | Acknowledgement |
| `PktHostInviteSearchReq` | Client → NetworkHost | Request list of hosts |
| `PktHostInviteSearchReply` | NetworkHost → Client | First page of host entries |
| `PktHostInviteMoreReq` | Client → NetworkHost | Request next page |
| `PktHostInviteMoreReply` | NetworkHost → Client | Next page of host entries |
| `PktHostJoinReq` | Client → UserHost | Request to join |
| `PktHostJoinReply` | UserHost → Client | Join state result |
| `PktHostLeaveReq` | Client → UserHost | Temporary leave |
| `PktHostLeaveReply` | UserHost → Client | Leave acknowledgement |
| `PktHostUnJoinReq` | Client → UserHost | Permanent unjoin |
| `PktHostUnJoinReply` | UserHost → Client | Unjoin acknowledgement |
| `PktHostOfferReq` | Client → UserHost | Offer (e.g. to join) with optional message |
| `PktHostOfferReply` | UserHost → Client | Offer response |
| `PktHostInfoReq` | Client ↔ UserHost | Request host metadata |
| `PktHostInfoReply` | UserHost → Client | Host title, description, thumbnail |
| `PktHostSearchReq` | Client → UserHost | Search for content within the host |
| `PktHostUserInfoReq` | Client ↔ Host | Request identity/info for a specific member |
| `PktHostUserInfoReply` | Host → Client | Member identity data |
| `PktHostUserListReq` | Client → Host | Request current member list |
| `PktHostUserListReply` | Host → Client | Member list (first page) |
| `PktHostUserListMoreReq` | Client → Host | Next page of member list |
| `PktHostUserListMoreReply` | Host → Client | Member list continuation |
| `PktAdminAvail` | Host → All clients | Admin opened/closed the host view |
| `PktRandConnectReq` | Client → RandomConnectHost | Initiate or respond to random connect action |
| `PktRandConnectReply` | RandomConnectHost → All clients | Broadcasted relay of the request |
