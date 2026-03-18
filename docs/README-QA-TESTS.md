# Network QA Tests

This document defines repeatable QA tests for network behavior in NoLimitConnect.

Scope:

- Network host resolution and availability.
- User-host announce/search/join flows.
- Online status dot behavior (green/orange/grey).
- Member visibility convergence after joins.
- Relay authorization correctness.

## Test Environment

Minimum setup:

- 3 clients: `A`, `B`, `C`.
- 1 host-capable node `H` (can be one of A/B/C if practical).
- Stable network and one scenario with restricted inbound port (NAT/blocked) for relay tests.

Recommended:

- Separate accounts/online IDs for each node.
- Clean app restart between major test groups.
- Logging enabled for host join and relay categories.

## Status Dot Oracle

Expected icon states:

- Green: peer online with direct connect.
- Orange: peer online through relay.
- Grey: peer offline.

If status differs between two peers, always verify both sides after 5-10 seconds for convergence.

## Test Group 1: Startup Resolution And Infrastructure

### T1.1 Resolve ConnectionTest and NetworkHost

Steps:

1. Start app on node A with internet available.
2. Wait for startup network actions to complete.
3. Open network/debug status UI (or logs) and confirm host resolution state.

Expected:

- ConnectionTest host resolves to an online ID.
- NetworkHost resolves to an online ID.
- No repeated resolution failures in steady state.

### T1.2 Port-open Impact On Announce Eligibility

Steps:

1. On node H, run with inbound port open.
2. Enable a user host (Group/ChatRoom/RandomConnect).
3. Repeat with inbound port blocked.

Expected:

- Open port: host can announce and be discoverable.
- Blocked port: announce is rejected/deferred (requires open port), host not globally discoverable via NetworkHost.

## Test Group 2: User-Host Discovery And Join

Run each test for all three host types:

- GroupHost
- ChatRoomHost
- RandomConnectHost

### T2.1 Search Returns Announced Hosts

Steps:

1. H enables host and announces.
2. A performs host search for the specific host type.

Expected:

- H appears in search results with valid metadata (title/description/host URL).

### T2.2 Join/Leave/Unjoin State Transitions

Steps:

1. A joins H.
2. A leaves host.
3. A rejoins.
4. A unjoins.

Expected:

- Join reply indicates granted/requested/denied correctly.
- Leave transitions session out of active state.
- Unjoin removes persistent join relationship.

## Test Group 3: Online Status Dot Behavior

### T3.1 Direct-connect Status

Steps:

1. Ensure A and B both have open inbound ports.
2. Join same host session and exchange announcements.

Expected:

- A sees B as green.
- B sees A as green.

### T3.2 Relay-required Status

Steps:

1. Keep A reachable; make B not directly reachable (blocked inbound).
2. Join same relay-capable user host session.

Expected:

- Direct path unavailable.
- Peer remains online but shown as orange when traffic routes through relay.

### T3.3 Offline Status

Steps:

1. With A and B visible online, disconnect B.
2. Wait for disconnect propagation.

Expected:

- A shows B as grey.
- B removed from active hosted-user lists.

## Test Group 4: Join-Order Member Visibility (Regression)

Purpose: validate fix for missing members depending on join order.

### T4.1 Deterministic 3-user sequence

Steps:

1. A hosts (or H hosts) and is active in host member list.
2. B joins host; verify A<->B visibility.
3. C joins host last.
4. Check member lists on A, B, C.

Expected:

- C receives identities for both A and B.
- A and B receive C identity.
- All three lists converge to the same membership set (excluding self where UI does so).
- Result must be independent of whether B or C joined last.

### T4.2 Permutation sweep

Run all join permutations for A, B, C:

1. A-B-C
2. A-C-B
3. B-A-C
4. B-C-A
5. C-A-B
6. C-B-A

Expected:

- No permutation leaves a missing member in any participant list after convergence window.

## Test Group 5: Relay Authorization (Regression)

Purpose: validate relay access requires actual membership of the checked user.

### T5.1 Non-member relay denial

Steps:

1. A and B join relay-capable host session on H.
2. C does not join.
3. Attempt relay traffic path that treats C as source or destination through H.

Expected:

- Relay rejects C as not joined.
- No successful relayed packet delivery for C.

### T5.2 Member relay allow

Steps:

1. Now C joins the same host on H.
2. Retry relay traffic.

Expected:

- Relay accepts source/destination only after join is active.

## Test Group 6: Friendship Independence In Host-Known Identity Path

Purpose: verify member discovery does not depend on friendship values in host-sent identity.

### T6.1 Guest friendship still converges

Steps:

1. Use users with no friendship relationship.
2. Join same host and trigger host-known identity exchange.

Expected:

- Membership visibility converges.
- Peer discovery/list population occurs despite guest friendship fields.

## Quick Smoke Matrix

For each host type (Group, ChatRoom, RandomConnect), run:

1. Search and join success.
2. 3-user join-order sequence (A-B-C then A-C-B).
3. Status dot transitions green/orange/grey.
4. Non-member relay denial and member relay allow.

## Failure Reporting Template

When filing a defect, include:

1. Build identifier and platform.
2. Host type under test.
3. Join order used.
4. Which node saw incorrect status/list.
5. Expected vs actual result.
6. Timestamped log snippet around join/announce/relay packets.

## Exit Criteria

Release candidate passes network QA when:

1. All tests in Groups 1-4 pass on at least one clean run.
2. Relay regression tests in Group 5 pass.
3. No unresolved critical issue in membership visibility or online status correctness.
