# Network Hosts — Infrastructure Services

Network hosts are **infrastructure-level servers** that the rest of the system depends on during startup.
They are not user-hosted features; they are fixed, always-on services that provide two functions:

| Host | Plugin type | Purpose |
|---|---|---|
| **NetworkHost** | `ePluginTypeHostNetwork` | Directory / registry — stores and serves the list of active user hosts |
| **ConnectionTestHost** | `ePluginTypeHostConnectTest` | Connectivity test — reports the caller's external IP address and whether its TCP port is reachable |

Both hosts are implemented as plugins derived from `PluginBaseNetworkService`
(`libs/libptopengine/Plugins/PluginNetworkHost.cpp`,
`libs/libptopengine/Plugins/PluginConnectionTestHost.cpp`).

---

## Startup and Resolution

Resolution is driven by `NetServicesMgr` (`libs/libptopengine/NetServices/`), which owns a worker
thread that executes a queue of `NetActionBase` actions sequentially.

### Step 1 — Wait for internet (`NetActionWaitForInternet`)
The engine does nothing until a local IP address is detected.

### Step 2 — Resolve ConnectionTest URL (`NetActionResolveConnectTestUrl`)
1. Reads `connectTestUrl` from `EngineSettings`.
2. Calls `VxResolvePtopUrl()` on the URL to get the host IP. If the resolved IP matches our own
   external or local address the engine notes that *we are the ConnectionTest host* and marks
   `connectionTestAvail = true` immediately.
3. Otherwise, calls `ConnectionMgr::applyDefaultHostUrl(eHostTypeConnectTest, url)` which initiates
   a TCP handshake to the URL. During handshake the remote peer's `PktAnnounce` packet supplies its
   `VxGUID` (online ID), which is cached in `HostUrlListMgr`.
4. Polls `ConnectionMgr::getDefaultHostOnlineId(eHostTypeConnectTest)` for up to 20 seconds.
5. As a side-effect, also fires off `applyDefaultHostUrl(eHostTypeNetwork, networkHostUrl)` in
   parallel so both hosts can resolve concurrently.

### Step 3 — Resolve NetworkHost URL (`NetActionResolveNetworkHostUrl`)
Same pattern as step 2 but for `networkHostUrl`. Polls for up to 22 seconds.

### Step 4 — Resolve Default User-Host URLs (`NetActionResolveDefaultUserHosts`)
Applies the configured URL for `eHostTypeRandomConnect`, `eHostTypeGroup`, and
`eHostTypeChatRoom` to `ConnectionMgr` so client plugins can connect to them later.

> **Note:** `EngineSettings` has three separate getters — `getRandomConnectUrl`,
> `getGroupHostUrl`, `getChatRoomHostUrl` — for the three user-service URL settings.

### Re-queuing
`addNetActionToQueue()` deduplicates: if an action is already in the queue it is not added again.
A 15-minute periodic timer fires `onThreadOncePer15Minutes` on each plugin, which re-triggers
`sendHostAnnounce()` on user hosts.

---

## NetworkHost — Directory Service

**Class:** `PluginNetworkHost`  
**Plugin type:** `ePluginTypeHostNetwork`

### Role
The NetworkHost is the **central directory** for all user hosts (GroupHost, ChatRoomHost,
RandomConnectHost). User hosts connect to it and announce themselves. Clients query it to discover
which user hosts are currently available.

### Incoming packet handlers

| Packet | Handler | What it does |
|---|---|---|
| `PktHostInviteAnnounceReq` | `onPktHostInviteAnnReq` | A user host announces itself. Payload contains host type, invite URL, title, description, thumbnail ID, and modification timestamp. Stored in `HostServerSearchMgr` under the appropriate announce list (`m_ChatRoomHostAnnList`, `m_GroupHostAnnList`, `m_RandConnectHostAnnList`). After processing, temporary connections are closed. |
| `PktHostInviteSearchReq` | `onPktHostInviteSearchReq` | A client requests the list of hosts of a given type. Delegates to `HostServerSearchMgr::onPktHostInviteSearchReq` which serialises matching `HostSearchEntry` records into a `PktHostInviteSearchReply` blob and sends it back. |
| `PktHostInviteSearchReply` | `onPktHostInviteSearchReply` | NetworkHost acting as a *client* of another NetworkHost (not current use). |
| `PktHostInviteMoreReq` | `onPktHostInviteMoreReq` | Pagination — client requests the next page of results. |
| `PktHostInviteMoreReply` | `onPktHostInviteMoreReply` | Pagination reply (client role). |

### Announce list maintenance
`HostServerSearchMgr` keeps one `std::map<PluginId, HostSearchEntry>` per host type.
Each `HostSearchEntry` records the last-received announce timestamp.
A host is considered offline if no announce has been received within
`MIN_HOST_RX_UPDATE_TIME_MS` (30 minutes).

### Self-announce shortcut
When the running node is *also* a NetworkHost, user host plugins detect this:

```cpp
if( netHostUrl.getOnlineId() == m_Engine.getMyOnlineId() )
{
    // directly call updateHostSearchList on our own NetworkHost plugin
    netHostPlugin->updateHostSearchList( ... );
    // skip network round-trip
}
```

---

## ConnectionTestHost — Connectivity Test Service

**Class:** `PluginConnectionTestHost`  
**Plugin type:** `ePluginTypeHostConnectTest`

### Role
The ConnectionTestHost answers two questions for its callers:

1. **What is my external IP address?** — The host sees the caller's source IP and echoes it back.
2. **Is my TCP listen port reachable from the internet?** — The host attempts to *dial back* the
   caller on its claimed listen port; success means the port is open.

### Transport
ConnectionTestHost uses the lightweight **NetService** HTTP-like command protocol rather than
the full P2P packet protocol. All commands arrive through `handlePtopConnection()` as
`NetServiceHdr` structs.

### Command types handled

| Command constant | String on wire | Purpose |
|---|---|---|
| `eNetCmdClientPing` | `CMD_CLIENT_PING` | Caller sends a ping; host echoes a pong containing the caller's seen IP address. |
| `eNetCmdClientPong` | `CMD_CLIENT_PONG` | Reply to the host-initiated ping (see port-open test below). |
| `eNetCmdIsMyPortOpenReq` | `CMD_PORT_TEST_REQ` | Caller asks: "is my port N open?" Host attempts an outbound TCP connect to the caller's IP:port, then replies with success/fail. |
| `eNetCmdIsMyPortOpenReply` | `CMD_PORT_TEST_REPLY` | Reply carrying `ENetCmdError` and the caller's external IP string. |

### Port-open test flow
```
Client                           ConnectionTestHost
  |                                        |
  |--- CMD_PORT_TEST_REQ (myPort=N) ------>|
  |                                        |--- TCP connect attempt to Client:N
  |<--- CMD_PORT_TEST_REPLY (ok/fail) -----|
```
`NetServicesMgr::doIsMyPortOpen()` drives this sequence and stores the result in
`NetStatusAccum`. The result is raised via `netActionResultIsMyPortOpen()` → callback →
`toGuiNetworkState()`.

### Loopback sanity check
Before contacting the ConnectionTest host, the engine first performs a loopback ping on the
**local** IP:port to verify the listen socket is working (`testLoobackPing`). If the loopback
fails the port-open test is not attempted.

---

## Network Status Summary

After resolution completes, `NetStatusAccum` tracks:

| Flag | Meaning |
|---|---|
| `isNetHostAvailable()` | NetworkHost URL resolved to a valid online ID |
| `isConnectionTestAvailable()` | ConnectionTest URL resolved to a valid online ID |
| `isRxPortOpen()` | Port-open test confirmed external port is reachable |
| `isDirectConnectTested()` | At least one successful direct connect test done |
| `canAnnounceToNlcHost()` | NetworkHost available AND port is open — user hosts may announce |

User hosts check `canAnnounceToNlcHost()` before every `sendHostAnnounce()` call.
