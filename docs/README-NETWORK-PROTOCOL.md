# Network Protocol Overview

This document defines protocol rules that all No Limit Connect packet paths must follow.

## Purpose

Use this as a developer checklist when creating, extending, or reviewing packet structures and send/relay logic.

## Protocol Invariants

1. Packet length must be a multiple of 16 bytes.
Reason: packet encryption/decryption uses a block cipher and requires 16-byte alignment.

2. Every packet must carry a valid source online id.
Reason: receivers and relays must preserve original sender identity.

3. Every packet must carry a valid destination online id.
Reason: routing must always be explicit for both direct and relayed traffic.

## Routing Rules

Direct routing:

- connected peer id and destination id are usually the same.

Relay routing:

- connected peer id can be the relay host.
- destination id can be a different final recipient behind that host.

Always preserve end-to-end identity semantics:

- source id is the original sender
- destination id is the intended receiver for that hop/packet

Do not rewrite source id to the relay host identity while forwarding.

## Broadcast Rule (Random Connect)

Some relay paths are intentionally broadcast to host members (for example, Random Connect status/offer updates) so all clients can maintain consistent UI state, including availability/session world-icon color updates.

## Examples

### Example 1: Random Connect Relay + Broadcast

Entities:

- A: member sending offer/status action
- B: target member
- H: random-connect host relay

Flow:

1. A sends request to H with source/destination set for the network hop.
2. H relays to connected members as broadcast.
3. Clients use payload source/target fields to determine logical sender/target and update UI state.

Why broadcast is required:

- Random Connect member UI (world icon availability/session color) must stay synchronized for all members, not only A and B.

Key rule:

- Preserve original source and intended target semantics in payload fields when relaying.

### Example 2: Standard Plugin Offer/Reply (Direct)

Entities:

- A: offer sender
- B: offer receiver

Flow:

1. A sends offer packet directly to B.
2. B sends reply packet directly to A.

Key expectations:

- Offer and reply each set valid source and destination ids.
- Offer/reply correlation id is preserved end-to-end.
- Packet struct remains 16-byte aligned after any field changes.

## Developer Checklist

Before merging packet protocol changes:

1. Confirm packet struct size is 16-byte aligned.
2. Confirm source id is explicitly set before transmit.
3. Confirm destination id is explicitly set before transmit.
4. Confirm relay logic preserves source/destination semantics.
5. Confirm broadcast-vs-targeted behavior is intentional and documented.

## Scope Note

This is an overview rules document.
Detailed packet-type flow documentation should live in feature-specific docs when needed.

