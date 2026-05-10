# Random Connect Offer/Response

## Purpose

This document defines the random-connect one-on-one offer/response protocol used by random connect members.

It complements host relay behavior documented in `docs/README-USER-HOSTS.md` and focuses on:
- packet action semantics
- state/lifecycle rules
- timeout and conflict behavior
- implementation touch points

## Terminology

- Selection state: random-connect availability/selection state broadcast to members (`ERandAction` select and deselect actions).
- Offer state: one-on-one call offer lifecycle (request, accept, reject, cancel, timeout-like outcomes).
- Source user: user that emits the action in packet `groupieId.userOnlineId`.
- Target user: user in `toUserOnlineId`.

Selection state and offer state are related but distinct. Do not collapse them into one state machine.

## Current Transport Model

Random connect uses a relay host:

1. client sends `PktRandConnectReq` to `PluginRandomConnectHost`
2. host rebroadcasts `PktRandConnectReply` to all connected random-connect clients
3. clients filter payload by target and context

Broadcast is intentional, so observers can maintain consistent distributed state.

## ERandAction Contract

`ERandAction` now contains two groups:

Selection actions:
- `eRandActionNone`
- `eRandActionSelectUser`
- `eRandActionDeselectUser`

Offer lifecycle actions:
- `eRandActionOfferRequest`
- `eRandActionOfferAccept`
- `eRandActionOfferReject`
- `eRandActionOfferCancel`
- `eRandActionOfferNoResponse`
- `eRandActionOfferMissed`

Notes:
- Selection actions update the member availability list.
- Offer actions are tracked separately in `RandConnectMgr` offer state list and announced via offer callbacks.

## Packet Field Rules

For all offer lifecycle actions, these fields are required:

- `groupieId` (source user + host context)
- `toUserOnlineId` (target user)
- `sessionId` (unique per offer lifecycle)
- `timeRequestedMs` (offer lifecycle timestamp)
- `offerType` (`eOfferTypeTruthOrDare`, `eOfferTypeVideoChat`, or `eOfferTypeVoicePhone`)

Field meaning:
- `sessionId`: primary dedupe and lifecycle correlation key.
- `timeRequestedMs`: used for timeout calculations and debugging.
- `offerType`: selected session mode that the target is being invited to join.
- `accessState`: optional context propagated end-to-end through relay.

## Timeout Policy

Offer timeout target is aligned with current behavior in phone ringer flow:

- `17000 ms` (about 17 seconds)

This is the current runtime timeout used by existing offer handling and should be treated as canonical until changed globally.

## Simultaneous Offers Policy

When A offers B and B offers A at nearly the same time:

- both offers remain visible
- no implicit auto-merge
- one side explicitly chooses (accept/reject/cancel)

Follow-up behavior after a choice is implementation-specific by applet/session layer, but protocol must preserve both pending offers until a terminal action is received.

## UI Mapping Guidance

World icon semantics:

- Green: available for random connect selection.
- Orange: transient/active outgoing offer context.
- Red: in one-on-one active session.

Important:
- UI should not infer full lifecycle from icon color alone.
- Internal state should use explicit offer actions and offer/session state enums.

## Manager Responsibilities

`RandConnectMgr`:
- maintains member selection list for select/deselect actions
- maintains offer list keyed by source/target/session for offer actions
- announces selection updates via `callbackRandConnect`
- announces offer updates via `callbackRandConnectOffer`

`GuiRandConnectMgr`:
- continues to drive existing selection UI path
- tracks pending random-connect offers in GUI scope
- emits timeout follow-up actions (`eRandActionOfferNoResponse` / `eRandActionOfferMissed`) at ~17s
- provides send helpers used by popup menu actions for request/accept/reject/cancel
- launches the matching peer applet when an offer is accepted

`AppSettings`:
- stores the last selected random-connect session offer type
- defaults to `eOfferTypeVideoChat`

## Initial Implementation Notes

The first implementation slice includes:

- extended `ERandAction` with offer lifecycle actions
- packet accessor support for `timeRequestedMs` and `offerType`
- relay propagation of `sessionId`, `timeRequestedMs`, `offerType`, and `accessState`
- manager-side separation of selection updates vs offer lifecycle updates
- popup-driven GUI actions for `Offer`, `Accept`, `Reject`, and `Cancel`
- popup-driven session-type selection before sending a random-connect offer
- persistence of the selected session type as the next default in `AppSettings`
- timeout-driven no-response/missed emission from GUI timer check
- accepted-offer launch into `eAppletPeerTruthOrDare`, `eAppletPeerVideoPhone`, or `eAppletPeerVoicePhone`

Future slices should add:

- deterministic closeout behavior when one of two simultaneous offers is accepted
- any deeper random-connect-specific post-connect UX beyond the existing peer session applets

## Test Checklist

1. Selection state still updates correctly for select/deselect actions.
2. Offer request action round trips via host relay with session id preserved.
3. Offer request preserves the selected `offerType` end-to-end.
4. Random Connect offer dialog defaults to the last saved session type and updates that default when changed.
5. Accept/reject/cancel actions remove/transition tracked offer entry.
6. Accepted offers launch the matching peer applet and start the corresponding plugin session.
7. Simultaneous cross offers can coexist until terminal action.
8. Disconnect cleanup emits cancellation-like offer callback and removes stale entries.



