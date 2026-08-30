# README - Event Calendar

This document summarizes how the Event Calendar feature works: where it lives in the
host/client split, how events sync, and how event content is purged.

## Scope

Calendar is not a separate plugin/host type. It is a composed feature of the existing
Group/ChatRoom/RandomConnect host and client plugins:

- domain model + persistence in `libs/libptopengine/Calendar`
- wire packets in `libs/PktLib/PktsCalendar.h/.cpp`
- GUI in `nolimitgui/src/Calendar*`, `DialogCalendarEvent`, `GuiCalendarMgr`

See `docs/technical/README-ARCHITECTURE-CONSTRAINTS.md` for the standing design rules this
feature follows (no store-and-forward, Network Host stays a thin directory, etc.).

## Design philosophy: a flash mob, not a business meeting

Several choices below (no RSVP, no reminders, no attendance record, content purged after
retention elapses) read as missing features until you know the model they're built against.
This calendar is designed for **flash-mob-style gathering, not business-meeting-style
scheduling**:

- A business meeting has an invite list, tracked RSVPs, a reminder ping, and minutes that
  persist afterward as a record of who said what.
- A flash mob has a time and a place. You show up or you don't. Nobody is tracking who
  confirmed, nobody reminds you, and once it's over there's no lasting record — unless someone
  specifically chose to keep something.

That second model is the one this feature implements, and it follows directly from
`README-ARCHITECTURE-CONSTRAINTS.md`'s point about anonymous, transitory-by-design use: a
network with no required central server and no store-and-forward isn't well suited to being the
system of record for who's coming to what. So "attending" is a live headcount, not a guest list;
there's no reminder system; and event content survives only as long as its retention window (or
until someone explicitly saves it) — the calendar tells you *when and where*, not *who and
whether*.

## Data model

`CalendarEventInfo` (`libs/libptopengine/Calendar/CalendarEventInfo.h`) is one event definition,
owned by a host. A recurring event is one row; a specific occurrence is identified by
`(eventId, occurrenceStartMs)`.

- Event name — capped at `MAX_EVENT_NAME_LEN` (256 bytes)
- Description — capped at `MAX_EVENT_DESCRIPTION_LEN` (512 bytes)
- `setEventName`/`setEventDescription` silently truncate to these caps, UTF-8-sequence-safe
  (never cuts a multi-byte character in half)
- Start time / duration — `int64_t` ms, GMT (`GetGmtTimeMs()`); local-time conversion happens
  only at the GUI display layer
- Recurrence — `Never | EveryDay | EveryWeek | EveryMonth` (month-end clamped: the 31st becomes
  Feb 28th/29th that month only, resumes on the 31st in longer months)
- Retention — hours after an occurrence ends before its content is purged (see below)
- Video postings allowed — per-event override of the host type's default video policy
- Attendance mode — `Everybody` or `UserList` (invite list). **Currently always `Everybody`** —
  no multi-select member picker exists yet to build the invite-list UI on
  (`DialogCalendarEvent.h`)

Persistence is `CalendarDb : public DbBase` (`Calendar/CalendarDb.h/.cpp`), one sqlite file per
hosted plugin *instance* (a host running both ChatRoom and Group has two separate calendars).

## Host side

`CalendarMgr` (`Calendar/CalendarMgr.h/.cpp`) is composed into `PluginBaseHostService` exactly
like `HostServerMgr`. It is the sole authority: only the host admin
(`netIdent->getMyFriendshipToHim() >= eFriendStateAdmin`) may create, edit, or cancel an event.
Attendance/visibility is checked per-request via `isEventVisibleToUser()`.

**Own-host management never touches the network.** `P2PEngine::fromGuiCalendarEventUpdate/
EventCancel/Refresh` branch on `adminId.getHostOnlineId() == getMyOnlineId()`:

- your own host → `PluginMgr::findHostServicePlugin()` → `CalendarMgr::localEventUpdate/
  EventCancel/ListFetch()` (direct in-process call, no packet)
- someone else's host (including a host where you were remotely granted admin) →
  `findHostClientPlugin()` → the client plugin sends a real packet; the host re-validates admin
  status server-side regardless

## Client side

`CalendarClientMgr` (`Calendar/CalendarClientMgr.h/.cpp`) caches one or more joined hosts'
calendars in memory (no local sqlite cache — the host is always the source of truth and is
re-fetched on every join). It is composed into `PluginBaseHostClient`, which covers
Group/PeerUser/RandomConnect clients.

**ChatRoom is the one exception.** `PluginChatRoomClient` does not derive from
`PluginBaseHostClient` (a pre-existing "inheritance conflict" workaround — see the class comment
in `PluginChatRoomClient.h`), so it carries its own duplicate `CalendarClientMgr` member and
duplicate copies of every calendar override (`fromGuiCalendarEventUpdate/EventCancel/Refresh`,
the 3 `onPktCalendarEvent*Reply` handlers, the on-join auto-fetch, `getActiveCalendarEventExpiresTime`,
`onThreadOncePerMinute`). **Any future change to `PluginBaseHostClient`'s calendar wiring must be
mirrored into `PluginChatRoomClient` by hand** — there is no shared base to inherit it from.

The calendar is fetched automatically on join (`onPktHostJoinReply`, guarded so a relayed
announce of someone *else* joining doesn't also trigger a fetch) and on manual refresh
(`IFromGui::fromGuiCalendarRefresh`, wired to the "Show Events" panel's `showEvent()`).

## Wire protocol and pagination

`PktsCalendar.h`: `PktCalendarEventListReq/Reply`, `PktCalendarEventUpdateReq/Reply`,
`PktCalendarEventCancelReq/Reply`. PktLib has no dependency on libptopengine, so the packets
expose only a raw `PktBlobEntry` — `CalendarEventInfo::serializeToBlob()/deserializeFromBlob()`
is the only bridge between wire and domain type.

A list reply fills events into one packet (~14KB blob) until `PktBlobEntry::haveRoom()` says the
next event won't fit, then sets `MoreEventsExist`. `PktCalendarEventListReq::skipCount` lets a
follow-up request resume where the previous reply's chunk left off, counted over
*visible-to-the-requester* events only. `CalendarClientMgr` automatically sends that follow-up
while `MoreEventsExist` is set — but only up to `MAX_LIST_REQUESTS_PER_HOST` (3) total requests
per fetch cycle. A host still claiming more events after that is logged (`LOG_WARNING`) and the
list is treated as complete anyway, so a host with an unreasonably large calendar (or one
deliberately stringing a client along) cannot make a client fetch indefinitely.

Per-event size is double-guarded against ever overflowing a reply packet: the fill loop checks a
conservative estimate (`160 + nameLen + descLen + inviteCount*16`, comfortably above the real
~107-byte fixed overhead) before serializing, and `PktBlobEntry::setValue()` itself independently
refuses to write past the blob's own bounds regardless of that estimate.

## Content retention and purge

The system is transitory by design: unless a user explicitly saves something, event content is
purged once retention elapses, as if it never happened.

- `AssetBaseInfo` gets `ASSET_ATTRIB_CALENDAR_EVENT` + a nonzero `ExpiresTime` at the moment it's
  posted, if the host has an event occurrence active right now
  (`AssetBaseXferMgr::tagAssetIfDuringCalendarEvent`, hooked into the one generic
  `onPktAssetBaseSendReq` handler shared by every plugin type)
- `AssetBaseMgr::purgeExpiredCalendarAssets()` sweeps every 15 minutes
  (`P2PEngine::onOncePer15Minutes`); an expired asset is purged outright unless
  `isInLibrary() || isPersonalRecord() || isSharedFileAsset()`, in which case it's exempted
  instead (attribute cleared, `ExpiresTime` reset to 0)
- Users (host included) can override the default retention length via
  `EngineSettings::setPurgeEventHistoryType` (`AppletUserPreferences`'s "Purge event history"
  combo) — `2 min / 15 min / 1 hour / 1 day / 1 week / 1 month / 1 year`, or "use host default"
- A running-long session keeps its expiry sliding forward once a minute
  (`{CalendarMgr,CalendarClientMgr}::extendActiveEventExpiryTimes`), but **only while someone is
  actually present**: the host side requires the admin to have their Host Admin screen open
  (`PluginBaseHostService::isAdminViewing()`); the client side requires a live connection to that
  host. Not "still a member" — an open connection specifically.

Saving a message/asset works via two existing mechanisms, both of which exempt content from
purge: the Library button, and Save-to-Personal-Recorder (`HistoryListWidget::
slotSaveAssetToPersonalRecorder`, clones the asset into `ePluginTypePersonalRecorder` rather than
flipping the original in place, since the original must stay visible to everyone still in the
live session).

## GUI

`CalendarEventListPanel` (`nolimitgui/src/CalendarEventListPanel.h/.cpp`) is the single embeddable
widget: a live "N attending now" count, an admin-only "Create Event" button, a "View Existing
Events" button, and a `CalendarEventListWidget` list. It is revealed via a "Show Events" toggle
button, collapsed by default, wired into both:

- `AppletHostClientBase` — the member/client session view
- `AppletHostAdminBase` — the host admin's own management screen

Each row (`CalendarEventRowWidget`) shows the event name, description, formatted local
date/time, and a recurrence label; admin-only Edit/Cancel icon buttons open `DialogCalendarEvent`
(create/edit form) or confirm-then-cancel the whole series.

**There is no RSVP.** An earlier RSVP design (accept/decline, `PktCalendarRsvpReq/Reply`,
`CalendarRsvpWidget`) was removed before real testing began. "Attending" is a live count of
currently-connected members (`IFromGui::fromGuiGetCalendarAttendingCount` → a synchronous read of
`ConnectIdListMgr`'s connection list) — nothing about attendance is recorded or persisted.

## Known limitations

- Attendance mode is fixed to `Everybody` — no invite-list picker UI exists yet
- No reminders/notifications for upcoming events
- Cancel always deletes the whole series — editing/canceling a single occurrence of a recurring
  event isn't supported
- The "Show Events" toggle state isn't persisted across app restarts (unlike the member/session
  eye-toggle buttons, which use `AppSettings`)
