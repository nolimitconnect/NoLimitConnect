# No Limit Connect Startup Sequence

This document describes the current startup path as implemented in code and highlights thread usage and lazy initialization opportunities.

## Goals

- Improve perceived startup speed (time-to-first-window and time-to-interactive).
- Keep startup work observable with `LogModule(eLogStartup, LOG_VERBOSE, ...)`.
- Separate required startup work from deferable work.

## High-Level Startup Path

1. `main()` creates `QApplication` and calls `runApplication()`.
2. `runApplication()` sets app metadata and creates two startup threads:
	 - `GuiThreadSettingsLoader`
	 - `GuiThreadMainLoader`
3. `runApplication()` waits for both loaders while processing Qt events.
4. `CreateAppInstance()` constructs `AppCommon` and static manager singletons.
5. `AppCommon::loadWithThread()` starts `GuiThreadAppLoader` and the audio devices thread.
6. `AppCommon::loadWithThread()` calls `doLogin()` directly after `startupAppCommon()`, so login begins as early as possible, overlapping with home window initialization.
7. Home window is created and shown.
8. `slotGuiStartupTimer()` continues staged background startup (`fromGuiAppStartup`, player startup, etc.).
9. On account startup/login, engine startup path runs:
	 - `P2PEngine::fromGuiSetUserSpecificDir`
	 - `P2PEngine::fromGuiUserLoggedOn`
	 - `P2PEngine::startupEngine`
	 - plugin and service initialization
10. After `completeLogin()`, `sendAppSettingsToEngine()` is dispatched via `QTimer::singleShot(0, ...)` so network-settings propagation does not block the login critical path.

## Thread Usage (Current)

- Main GUI thread:
	- Creates `QApplication`, UI objects, and drives event loop.
	- Busy-waits on startup thread completion via `processQtEvents()` loops.
- `GuiThreadSettingsLoader`:
	- Loads settings DB and log module flags.
- `GuiThreadMainLoader`:
	- Performs OS/media pre-startup and creates `P2PEngine`.
- `GuiThreadAppLoader`:
	- Loads account DB, favorites DB, icon resources, and starts logging manager.
- Audio devices thread (`AudioDevicesStartupThreadFunc`):
	- Calls `AudioMgr::audioIoSystemStartup()`.

## New Startup Timing Instrumentation

Added detailed startup timing with `LogModule(eLogStartup, LOG_VERBOSE, ...)` in:

- `runApplication()`:
	- root storage setup
	- bundled translations copy
	- loader thread start timings
	- loader wait timings
	- `CreateAppInstance` timing
	- font copy/verification timing
	- total startup summary
- `AppCommon::loadWithThread()`:
	- app loader/audio thread startup timing
	- wait for account manager/icons/app-loader completion
	- home window initialization timing
	- total function duration
- `AppCommon::slotGuiStartupTimer()`:
	- per-step begin/end timing
	- explicit logging when waiting for audio initialization
- `P2PEngine::fromGuiSetUserSpecificDir()`:
	- DB startup duration and total function duration
- `P2PEngine::fromGuiUserLoggedOn()`:
	- join-manager phase
	- `startupEngine()` phase
	- plugin logon phase
	- post-plugin initialization phase
	- total duration and threaded follow-up phase

## Startup Bottleneck Candidates

Based on the startup code path, likely expensive areas are:

- Plugin construction in `PluginMgr::pluginMgrStartup()` (many plugin objects always created).
- Synchronous DB startup calls in `fromGuiSetUserSpecificDir()`.
- Busy-wait loops in startup (`while (...) processQtEvents()`), which keep UI responsive but can burn CPU.
- Asset/resource copy checks at startup.
- Audio initialization waits on some devices (especially Android).

## Lazy Initialization Opportunities

1. Plugin manager
- Keep always-on core plugins eager.
- Defer non-essential plugin construction until first use/open (for example: less frequently used feature plugins).
- Consider a plugin factory map with per-plugin on-demand instantiation.

2. Account-scoped DBs
- Eagerly open only DBs needed for login/launch page.
- Defer feature DBs until corresponding applet/plugin is opened.

3. Asset copy checks
- Gate copy checks behind version marker files (run once per app update) instead of checking every startup.

4. GUI staged startup timer
- Continue using staged startup, but move optional service startup from step-based timer to lower-priority queued tasks after first frame.

5. Audio startup
- For platforms where audio init is slow, allow app interaction before full audio graph startup where safe.

## Threading Opportunities

1. Replace busy-wait loops with signaling
- Use `QWaitCondition`/`QFutureWatcher`/signals to avoid periodic event pumping loops.

2. Parallelize independent startup tasks
- Keep settings loader and main loader parallel.
- Add a low-priority post-window startup worker for optional features.

3. Prioritize startup tasks by UX impact
- Critical path: create UI, load account identity, show home window.
- Non-critical path: optional plugins/services, deep scans, prefetch work.

## Suggested Measurement Procedure

1. Enable startup logging module (`eLogStartup`) in settings.
2. Record cold-start runs and warm-start runs separately.
3. Extract top 5 longest startup phases from logs.
4. Apply one lazy-init change at a time.
5. Re-measure and compare:
	 - time to home window shown
	 - total startup duration
	 - CPU usage during startup

## Notes

- Current instrumentation uses `GetApplicationAliveMs()` to keep timing aligned with existing logs.
- If finer granularity is required, add a follow-up pass using `GetHighResolutionTimeMs()` for sub-millisecond profiling in specific hot functions.

## Observed Startup Timing (Windows Debug)

Representative timings from a Windows Debug build startup trace:

- `main Creating QApplication`: `46 ms`
- `runApplication total`: `440 ms`
- `AppCommon::loadWithThread begin login`: `444 ms`
- `AppCommon::loadWithThread home window initialized`: `514 ms`
- `doAccountStartup total`: `~11 ms`
- `slotApplyStartupSettingsToEngine` (deferred, off critical path): `~181 ms`
- `P2PEngine::fromGuiUserLoggedOn post-plugin init`: `~450–490 ms`
- Window interactive: `~8 s` (Windows Debug)

The largest remaining engine-side cost is post-plugin initialization after login, which opens several SQLite databases (OfferMgr, AssetMgr, ThumbMgr, SendQueueMgr).
