# Asset Send System

Assets are user-generated or user-added content. For asset sending, these types are supported:

1. **Text** — Chat entry
2. **Photo** — Camera snapshot
3. **Video** — Cam recording
4. **Voice** — Opus voice recording
5. **Face** — Emoticon image (built into application)

## Host Types

1. Chat Room
2. Group
3. Random Connect

## Chat Room Asset Send

Chat room sending is a special case:

1. Users send assets **only to host admin** — the host then distributes to all members
2. Host admin can send to an individual user or to all members
3. **Voice and Video assets are not allowed** to avoid large storage requirements on chat room host

## Group and Random Connect Asset Send

Group and Random Connect hosts work identically:

1. Users can send to an individual member or to all members
2. Host admin can send to an individual member or all members

### Multi-Member Send Behavior

When sending to multiple members, sends are performed **sequentially** to avoid network congestion:

1. Set member's name in progress bar text
2. Show send progress if transfer requires multiple packets
3. If user clicks the member's avatar in the avatar bar → send is canceled and a **canceled icon** is displayed
4. If an error occurs during send → it is recorded and the next send starts
5. If **all sends succeed** → the avatar icon is shown
6. If **any sends fail** → the fail icon is shown
7. Clicking a fail icon allows the user to view status and/or retry

## GUI Management

Management of the GUI side of sending assets is handled by `AssetSendMgr`





