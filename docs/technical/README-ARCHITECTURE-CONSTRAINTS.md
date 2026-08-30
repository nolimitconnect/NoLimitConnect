# Standing Architecture Constraints

These are the non-negotiable design invariants of NoLimitConnect's hybrid P2P/relay network. They
predate and outlive any individual feature — check a new feature's design against this list
*before* writing code, especially anything that needs to work across multiple hosts/users (sync,
discovery, notifications, cross-device state, etc.).

---

## 1. Hybrid server/client + P2P — not pure P2P, not pure client/server

NLC is neither a traditional client/server product nor a pure P2P mesh. Infrastructure-level
services (Network Host, Connection Test Host) provide directory/connectivity functions; actual
feature content (chat rooms, groups, file shares, calendars, etc.) is hosted P2P by individual
users' own NLC instances. See `docs/technical/README-NETWORK-HOSTS.md` and
`docs/technical/README-USER-HOSTS.md` for the concrete host types on each side of that split.

## 2. Core goal: no required high-powered central server

Anyone must be able to stand up a completely independent NLC network on any Android/Windows/Linux
device, with no dependency on infrastructure they don't control. This is why the **NLC Network
Host is deliberately kept to a thin listing/directory service** — it stores and serves the list of
currently-announced User Hosts (Group Host, Chat Room Host, Random Connect Host). It is not meant
to become a data store for feature content, and new features should not add persistent
feature-specific load to it without a specific reason.

**Consequence, precisely scoped:** the Network Host is a single point of failure only for **host
search and discovery** — if it's unreachable, clients can't browse/search for User Hosts on that
network, and a client that only knows a host by its network-directory listing can't (re)resolve it.
It is **not** a single point of failure for the network's basic ability to function: the **Invite
Create/Accept system** (`AppletInviteCreate`/`AppletInviteAccept`/`InviteUrlWidget`,
`libs/CoreLib/Invite.h`) lets a user package a host's connection URL(s) — personal, chat room,
group, random connect — plus network settings directly into a shareable invite, which the recipient
imports to connect straight to that host, bypassing the Network Host entirely. So a Network Host
outage degrades discoverability/convenience (no search, no directory-based rejoin) but does not
sever the network — anyone with a valid invite can still connect. This is an accepted tradeoff, not
an oversight: it's mitigated first by the Invite system for a single network, and second at the
network level (point 2 above — anyone can run their own independent network with its own Network
Host, so no single organization's outage can take down NLC as a whole), not by adding
redundancy/replication inside a single network. See `docs/faq.md`'s "Why We Encourage Network
Hosting" section for the user-facing version of this argument.

## 3. No store-and-forward — content lives on the host, not in transit

NLC does not implement store-and-forward messaging. A User Host (Group/ChatRoom/RandomConnect)
only serves content while it is online; there is no intermediary that queues messages for a host
or member that is currently offline. This follows directly from point 2: a store-and-forward relay
would itself need to be an always-available service, reintroducing the central-server dependency
the architecture is designed to avoid. Combined with the transitory-by-design retention rules used
by content-bearing features (see the Event Calendar's retention/purge design), the practical model
is: **if you weren't there while it happened, and nobody saved it, it's gone** — not queued for
later delivery.

## 4. Connection Test service is independently hostable

A separate service from the Network Host — can run on a different device/VPS. Irrelevant to most
feature/content design; only relevant to connectivity/NAT-traversal work. In production, the
public Network Host and Connection Test Host run on separate VPS instances.

## 5. Five permission levels per network plugin

Every NLC instance self-announces a permission level for every network-accessible plugin it runs.
When designing a feature, assume the user already has sufficient permission for the Network Host
and for whatever User Host they want to interact with — the permission system itself doesn't need
to be redesigned per-feature. See `docs/howto-docs/howto-permission-levels.md`.

## 6. No separate server/client binaries

Every NLC install contains both host-side and client-side code for every feature; a feature is
turned on/off per-instance purely via its permission level (including "disabled"). There is no
build variant that strips host code from a "client-only" install or vice versa.

## 7. Exactly one Network Host per network

Within a given NLC network, there is exactly one Network Host that everyone talks to. User Hosts
announce themselves to it every 15 minutes. Clients connect to it to verify reachability and, if
already joined to a User Host, to get that host's latest info/IP so they can rejoin it. This
announce/list/join pattern is the existing precedent for how any new cross-host discovery/sync
should work — prefer extending it over inventing a new sync mechanism.

---

## Why this matters

Points 2, 3, and 7 together are *why* naive "just sync it through the central server" or "queue it
until they come back online" designs don't work for NLC. Any feature with meaningful data (e.g. an
event calendar) should default to being owned/served by the individual User Host, with the Network
Host only ever carrying small discovery-level metadata (if that), mirroring the existing
announce/search/join flow in point 7 — and any offline-delivery expectation needs to be scoped
around "the host has to be online for this to exist," not designed around a store-and-forward
assumption.
