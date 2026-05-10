# Session Flow

This document explains how offer-based sessions move from request to active media, and how that is reflected in UI state.

## Scope

- Phone-like plugins: Video Chat, Voice Phone, Truth Or Dare.
- Offer list handling in GuiOfferMgr.
- Applet/session startup through AppletPeerBase and OfferSessionLogic.
- Random Connect integration with the normal offer pipeline.

## Main Components

- AppCommon
	- Bridges engine callbacks into GUI offer/session handlers.
- GuiOfferMgrBase
	- Owns active offer list and offer history.
	- Validates offers, accepts/rejects, launches applets, tracks state transitions.
- GuiRandConnectMgr
	- Maintains Random Connect offer records.
	- Mirrors Random Connect offers/replies into GuiOfferMgr.
	- Can launch accepted sessions for Random Connect accept events.
- AppletPeerBase and derived applets
	- Bind offer sessions to applet UI.
	- Start plugin session for accepted sessions.
- OfferSessionLogic
	- Owns session-level state and engine start/stop calls.

## Offer Lifecycle

1. Offer created or received
	 - OfferBaseInfo is built with plugin type, IDs, manager role, timestamps, and response state.
	 - GuiOfferMgr creates or updates GuiOfferSession in active list.

2. Offer appears in Offer List
	 - Active offer count is recalculated by GuiOfferMgrBase.updateActiveOfferCount.
	 - UI clients receive callbackActiveOfferCount.

3. User accepts offer
	 - GuiOfferMgrBase.acceptOffer validates the offer.
	 - Reply is sent either through Random Connect response transport or normal plugin reply transport.
	 - Offer state transitions to accepted.
	 - Matching peer applet is launched (or reused) and setOfferSession is called.

4. Session start
	 - For accepted client-side sessions, applet startup calls beginAcceptedSession.
	 - beginAcceptedSession calls engine fromGuiStartPluginSession with plugin type, peer online ID, and offer/session ID.
	 - On success, in-session flags/callbacks are updated.

5. Session active and end
	 - Engine notifies GUI via toGuiPluginSessionStarted / toGuiPluginSessionEnded.
	 - GuiOfferMgr updates offer state to in-session or complete.
	 - Session eventually moves to history.

## Random Connect Bridging

Random Connect offers are intentionally bridged into the same offer/session pipeline to keep behavior consistent with normal offers.

- Incoming and outgoing Random Connect offer requests are mirrored into GuiOfferMgr via toGuiRxedPluginOffer.
- Random Connect terminal actions are mirrored into GuiOfferMgr via toGuiRxedOfferReply.
- Accept/reject from Offer List first attempts session-aware Random Connect response transport, then falls back to normal plugin reply transport.

## Session Identity Rules

- OfferBaseInfo manager role affects directional fields:
	- Host role means creator is local side and history/peer is remote side.
	- Client role means creator is remote side and history/peer is local side.
- Correct creator/history/offer IDs are required for:
	- matching existing sessions,
	- applet offer matching,
	- plugin session started callbacks.

## Single-Session Plugin Behavior

For single-session plugins (video/voice/truth-or-dare):

- If already in session with same peer/plugin, existing applet/session can be reused.
- If already in session with different peer (non-self-test case), accept is blocked with already-in-session behavior.

## Title Bar Counter Clarification

The numeric badge near the camera preview in the title bar is the camera server client count, not the active phone-call count.

- It is updated by cam server plugin status callbacks.
- It does not increment solely because a peer video session offer was accepted.
- Offer list counters and camera viewer count are driven by different subsystems.

## Common Failure Modes

- Offer Not Set
	- OfferBaseInfo missing required validity fields.
- File Hash Is Not Valid for non-file offers
	- File hash validation applied too broadly instead of file-transfer-only.
- User unavailable on relay-connected peer
	- Transport gate used direct-only logic instead of reachable logic.
- Applet opened but media did not start
	- Accepted session attached to applet without actually calling session start path.
- Already In Session on wrong target
	- Existing applet reused without confirming same peer/session match.

## Practical Debug Checklist

1. Confirm offer validity fields (plugin type, offer ID, creator/history IDs, manager role).
2. Confirm accept path transport (Random Connect response first when applicable).
3. Confirm applet setOfferSession is called with expected offer identity.
4. Confirm beginAcceptedSession triggers engine fromGuiStartPluginSession.
5. Confirm toGuiPluginSessionStarted callback resolves matching offer session.
6. Confirm move-to-history timing does not remove required active session state too early.

## Code Jump Table

Core offer manager path:

- Offer intake: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L60)
- Accept action: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L918)
- Accept launch wrapper: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L1091)
- Accept launch implementation: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L1107)
- Session started callback handling: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L272)
- Session ended callback handling: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L300)
- Active offer count update: [nolimitgui/src/GuiOfferMgrBase.cpp](../nolimitgui/src/GuiOfferMgrBase.cpp#L840)

Random Connect bridge path:

- Offer update/mirroring entry: [nolimitgui/src/GuiRandConnectMgr.cpp](../nolimitgui/src/GuiRandConnectMgr.cpp#L336)
- Session-aware accept/reject send: [nolimitgui/src/GuiRandConnectMgr.cpp](../nolimitgui/src/GuiRandConnectMgr.cpp#L165)
- Accepted-session object creation: [nolimitgui/src/GuiRandConnectMgr.cpp](../nolimitgui/src/GuiRandConnectMgr.cpp#L492)
- Accepted-session launch and reuse guard: [nolimitgui/src/GuiRandConnectMgr.cpp](../nolimitgui/src/GuiRandConnectMgr.cpp#L527)

Applet and session-start path:

- Applet session attach: [nolimitgui/src/AppletPeerBase.cpp](../nolimitgui/src/AppletPeerBase.cpp#L182)
- Accepted session start trigger: [nolimitgui/src/AppletPeerBase.cpp](../nolimitgui/src/AppletPeerBase.cpp#L197)
- Offer-to-applet identity match: [nolimitgui/src/AppletPeerBase.cpp](../nolimitgui/src/AppletPeerBase.cpp#L223)
- OfferSession wiring: [nolimitgui/src/OfferSessionLogic.cpp](../nolimitgui/src/OfferSessionLogic.cpp#L38)
- Generic session start helper: [nolimitgui/src/OfferSessionLogic.cpp](../nolimitgui/src/OfferSessionLogic.cpp#L374)

Title bar camera counter path:

- Plugin status callback entry: [nolimitgui/src/TitleBarWidget.cpp](../nolimitgui/src/TitleBarWidget.cpp#L295)
- Camera server client count update: [nolimitgui/src/TitleBarWidget.cpp](../nolimitgui/src/TitleBarWidget.cpp#L304)

