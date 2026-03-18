# Online Status And Membership Visibility

## Status Dot Meaning

- Green: online and direct connection is available.
- Orange: online through relay.
- Grey: offline.

## Relevant Join/Identity Flow

1. A user joins a host (Group/ChatRoom/RandomConnect).
2. Host accepts join and sends `PktHostJoinReply` to the new member.
3. Host sends existing member identities (`PktAnnounce`) to the new member (`sendMemberListToClient`).
4. Host broadcasts the new member's `PktAnnounce` to existing members.
5. Clients receiving a host member identity issue a `PktAnnounce` to that peer with request-reply semantics so both sides learn each other for contact/list visibility.

Note: host-provided member identities use guest friendship (`my/his = eFriendStateGuest`) by design. Friendship is not the transport trigger for visibility in this flow.

## Bug: Missing Members Depending On Join Order

### Symptom

- Sometimes not all members appear in lists after join.
- Behavior appears dependent on who joined most recently.

### Root Cause

In `HostServerMgr::sendMemberListToClient`, the loop used an inverted connection check:

- Before (buggy): break when `sktBase->isConnected()` is true.
- Effect: loop exits after first successful send, so only one prior member identity is delivered.

### Fix

Changed the loop guard to break only when disconnected:

- After (fixed): break when `!sktBase->isConnected()`.

This ensures all currently known members are sent to the newly joined client, regardless of join order.

## Related Relay Authorization Fix

Also fixed a separate relay authorization regression in `MemberActiveMgr::isUserJoinedToRelayHost`:

- Before: user/host match used `||`.
- After: user/host match uses `&&`.

This prevents false-positive relay authorization for users who are not actually joined to the relay host.
