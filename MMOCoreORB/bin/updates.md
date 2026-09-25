# Core3 project updates

Last updated: 2026-09-25.

Quick summary of MTGServer project work beginning on 2026-09-15. See
[updatesfull.md](updatesfull.md) for the expanded history, settings, and testing status.

## 2026-09-25 — planetary updates committed

- Committed the pending Frostline Outpost, planetary travel, maps, regions,
  weather, strings, shuttle startup diagnostics and Mustafar fixes together.
- Updated implementation statuses below; previously confirmed behavior and
  remaining build/deployment/in-game checks keep their recorded status.
- Local configuration, generated resource spawns and ignored tests remain
  outside the commit. Final file-scope and whitespace checks pass; no Core3
  build or run performed.

## 2026-09-24 — Mustafar boundary and provisional port placement

- Added `MustafarMapBounds = true` to both config files. Walking, mounted travel,
  teleports and saved-login recovery keep everyone, including admins, within
  Mustafar's mapped area. Outside characters recover to the live Mensix arrival.
- At the user's request, moved the existing port to a provisional flat site at
  **-1100, 2400**, with arrival and town marker at **-1100, 2365**. Preserved its
  services and IDs. The user will research the final location.
- Boundary checks pass for 2,058 cases; 2,704 terrain samples across the port and
  arrival area are flat and clear of detected lava/impassable areas. Region,
  service-binding, snapshot and map regressions pass. No Core3 build/run performed.
- Build/restart Core3 for the boundary, repackage the moved port/client region
  for server and client, and retest foot/mount boundaries, recovery and the port.
  See [travel guide](planet-travel.md). Included in the September 25 commit.

## 2026-09-24 — shuttle announcement audiences

- Ordinary players now receive only the shuttle/starport service announcement.
  Startup issue counts and the `/server travelstatus` hint go separately to
  online level-15 administrators and the server log.
- When no shuttle reached boarding, the public message truthfully reports
  unconfirmed availability. Startup timing, countdowns and schedules are unchanged.
- Source/timing/report checks pass. Requires the user's Core3 build/restart and
  a player/admin message check. Committed on 2026-09-25; no Core3 build/run performed.

## 2026-09-24 — Mustafar shadows and actual-client map compatibility

- An earlier user test found Mustafar's map still incorrect, Main/Dead Forest
  landmarks offset, and the combined Kashyyyk map visually unusual. Inspected
  the supplied regular `SWGEmu.exe` read-only; earlier checks used newer-client
  behavior that does not match this executable.
- Confirmed this client reads `MapWidth` but ignores the offset table. Replaced
  the unsupported offsets with image placement inside ordinary centered frames.
  Revised Main and Dead Forest's approximate crops; both fill most of their views.
- Restored Mustafar's geographic crop in its full terrain frame. It remains
  smaller at minimum zoom because the available image covers only part of the
  planet; use the map's zoom controls. Combined Kashyyyk remains a partial map
  of three separated regions. The user subsequently confirmed the corrected maps.
- Confirmed the client rejects Mustafar's 10-row lighting palettes and substitutes
  opaque gray shadows. Prepared five 8-row palettes preserving all original
  legacy lighting/shadow values. Rendered color and stretched shadows on slopes
  were subsequently confirmed okay by the user.
- The user confirmed Mustafar alignment and Main/Dead Forest maps look good;
  combined Kashyyyk's unusual partial-map layout is accepted for now. Mensix
  building/city placement is handled separately in the provisional move above.
- Binary/asset checks pass: 13,872 map-coordinate cases, 10,240 retained palette
  pixels, and existing map/ticket/weather regressions. Map checks now use the
  actual executable's transform. Repackage `mtg_patch_024` and restart the client; no Core3 build
  required. Committed on 2026-09-25; no executable run performed. See
  [map guide](planet-map-coverage.md) and [shadow details](planet-weather.md).

## 2026-09-24 — planet-name strings

- Fixed missing `planet_n` labels for Chandrila, Coruscant, and Moraband, which
  appeared as unresolved text such as `planet_n:[chandrila]`.
- Added eight missing scene names to the separate map-title table. All 39
  configured ground/space scene keys now resolve in both tables, with existing
  labels and IDs preserved.
- Added `string/en/planet_n.stf` and `string/en/zone_n.stf` to `mtg_patch_024`.
  Standalone checks pass; package both for server and client, restart, and verify
  names in game. No Core3 rebuild required. Committed on 2026-09-25.

## 2026-09-24 — sprite appearance diagnostics

- Traced `unknown appearance type SPRT` to the server loader not recognizing
  client sprites. The user's subsequent console output confirmed the mission
  UI icon `appearance/ui_destroy.spr` as the triggering appearance.
- Recognize sprites and retain their filenames as informational log messages.
  After confirming the UI icon is harmless, disabled console output at the
  user's request; this logging adjustment awaits the user's next build/restart.
  Genuine unknown appearance types now include filenames in their errors.
- Read-only asset checks and source review pass. The user confirmed the named
  diagnostic appears in the console; this investigation and runtime check are
  closed. A UI sprite needs no server collision mesh or asset repair. Committed on 2026-09-25.

## 2026-09-24 — shuttle startup notices

- Added startup countdown messages when opening a ticket terminal, purchasing
  a ticket, or trying to board. Credits, tickets, and coupons remain untouched
  while travel is waiting for startup.
- Added one galaxy announcement when shuttles and starports finish their initial
  startup and landings. Existing delay settings and normal boarding cycles remain.
- Corrected the reported build error when resolving a shuttle's weak reference
  from the monitor's local snapshot. The user subsequently reached the startup
  broadcast and reported its partial-availability warning.
- Added `/server travelstatus` for administrators to review startup issues by
  planet, port, coordinates, shuttle ID, and reason, with detailed log entries.
  Corrected a race that could count an intentionally discarded duplicate as a
  failure. The previous log did not retain identities for its startup warning.
- Standalone source, timing, and report checks pass.
  On September 24, the user confirmed `/server travelstatus`, the countdown,
  and the announcement work, with no errors on the latest boot. Initial runtime
  checks are closed; C++ unit-test execution has not been reported. The report
  records startup history, not later port health. Committed on 2026-09-25; no new TRE files.

## 2026-09-24 — custom planet regions

- Audited all 17 enabled custom ground scenes and repaired server/client regions
  across the nine custom travel worlds: 20 towns and one named district.
- Added missing town definitions and region labels, corrected Taanab's region
  filename, and moved the misplaced Kachirho and Mensix client circles.
- Added Moraband Outpost's region, included overlooked Mandalore buildings in
  town boundaries, and aligned South Range Outpost's Hoth name and boundary.
- All 24 travel arrival areas fit their regions. Standalone Lua, asset, boundary,
  and travel checks pass. On September 24, the user confirmed entering/leaving
  messages and town markers work. Their initial runtime checks are closed.
- Committed on 2026-09-25. See [region coverage and testing](planet-regions.md).

## 2026-09-23 — planetary weather

- Configured weather for all 19 travel planets and three additional outdoor
  Kashyyyk regions, with calmer city/rural profiles and more active Hoth/Kaas storms.
- Preserved the original planets' weather settings and made all profiles
  explicitly non-damaging. Updated the weather configuration's stale comments.
- Added the missing Mustafar client environment setup using existing volcanic
  visual assets, plus [weather settings and testing commands](planet-weather.md).
- Standalone Lua, weather timing, asset, and existing travel checks pass.
  On September 24, the user confirmed weather is working; initial runtime
  validation is closed. These configuration/asset changes need no Core3 rebuild.

## 2026-09-23 — all-planet ticket travel

- Standardized one-way base fares to **500 credits between planets** and
  **100 credits within a planet**, across all 19 enabled main planets.
- Added ticket-window controls for every destination and configured 19 existing
  ports across Chandrila, Coruscant, Kaas, Mandalore, Moraband, and Taanab.
- Corrected Nayli Outpost's arrival height on Chandrila after the user reported
  briefly hovering above ground. Lowered it from 18.9 to 18.09 and narrowed the
  arrival spread to 1m. Terrain/Lua checks pass; the user confirmed on September
  24 that arrivals no longer leave the player stuck. This retest is closed.
- Added a small Mustafar port for the existing Mensix destination, with its
  arrival and building heights matched to the terrain; preserved the original snapshot object.
- Corrected 399 conflicting snapshot object IDs on Mandalore/Taanab before
  enabling their world content; preserved object placement and parent links.
- Fixed ticket purchases losing the underscore in Kashyyyk's scene name.
  This server fix needs the user's Core3 rebuild; TRE changes need server/client
  deployment. See [planet travel](planet-travel.md) for validation and testing.
- The September 23 startup review traced Mandalore's missing-cell error to an
  old installed snapshot colliding with a Dantooine decoration. Six patch assets
  needed packaging at that inspection. The user reported no errors on the latest
  September 24 boot. Missing Kashyyyk region snapshots remain absent from
  available archives; the tutorial builds its content dynamically.

## 2026-09-23 — enabled planetary maps

- Added the existing Kaas and Mandalore maps and corrected Taanab's Talus image
  reference. Hoth and the original planets' mappings are preserved.
- Added Mustafar and Kashyyyk Hunting Grounds map pages, accounting for the
  images' smaller coverage and Mustafar's offset within the full terrain.
- Connected the user's new Chandrila and Coruscant map images and preserved
  their replacement Hoth texture. Moraband still has no image; most Kashyyyk
  regions still need alignment. Coruscant's street alignment remains unverified.
- Audited enabled zones and the available TRE map images; documented missing
  images and regional maps that need coordinate calibration in
  [planet-map coverage](planet-map-coverage.md), including texture locations
  and filenames for additional images.
- Committed on 2026-09-25; standalone UI/asset checks pass. On September 24, the user
  confirmed planetary maps work; initial client validation is closed. Missing
  or uncalibrated maps listed above remain deferred content work.

## 2026-09-23 — Hoth settlement and travel

- Added Frostline Outpost around Hoth **671, 2035**, with a starport, shuttleport,
  cloning clinic, bank, cantina, garage, workshop, quarters, bazaar, and trainers.
- Preserved the original Hoth snapshot and connected its southern outpost to
  the new town; added local and interplanetary ticket travel and fares.
- Connected the planetary and ticket maps to the existing Hoth map image,
  replacing the planetary window's Dathomir fallback for Hoth.
- Added a site plan, placement manifest, and deployment/testing guide.
  Standalone checks pass; the user confirmed successful startup after packaging
  the correction below. Town layout refinement and service testing remain.
- Corrected a startup failure found in the first packaged test: city names now
  use valid string-table references, with their labels included in the patch.
  The initial plain names caused navigation initialization to throw and stall.
  The user confirmed the corrected server loaded successfully on 2026-09-23.
- Added Frostline's missing client region for entering/leaving notifications;
  its server region was already present. Preserved the working southern region.
- Fixed missing ticket-window labels for Planet and Location, plus Travel and
  Show Help. Standalone region/string checks pass; client testing remains.

## 2026-09-20–23 — vehicles, inspection, and housing follow-up

- Repaired vehicle deed links, names, descriptions, and vehicle preview icons;
  the user reports the remaining active vehicles are working.
- Fixed STAP mounting and Advanced Tantive IV visibility; both confirmed working.
- Restored walker leg animations and wheel-bike movement animations; confirmed working.
- Added wheel-bike jumping through the player's configured Jump key and corrected
  turning toward a selected target; the user confirmed it working appropriately.
- Matched HK-47 and Merr-Sonn jetpack height and automatic mount/store behavior to
  the original jetpack; confirmed working.
- Added a modified USV-5 deed and completed presentation and compatibility work for
  eleven additional vehicles with existing TCG deeds.
- Removed the two light-bending deed registrations and preserved the user's twelve
  disabled duplicate registrations, retaining their vehicle/control-device templates.
- Disabled the unfinished AT-AT and six colored Swoop Racer deeds for later work;
  the working black Swoop Racer remains available.
- Disabled the regular AT-RT deed after the attempted mesh repair did not fix
  its missing parts. The camouflage AT-RT remains available.
- Added the black Swoop Racer using the supplied texture. The user confirmed it
  working after correcting the package and making a further texture adjustment.
- Added optional server/shared `.iff` paths to tangible-object Examine information
  for testing; the user confirmed the setting works.
- Added the two missing template-display labels to the client string table;
  the user confirmed the corrected labels work. No Core3 rebuild was required.
- Added vehicle spawn-command references and checks for incomplete or stale TRE packages.
- Added the missing Naboo Medium Style 2 player-house setup using its original
  model and interior; the user confirmed it works and adjusted the terminal position.
- Retired Generic Medium Window Style 3 from testing at the user's request;
  no server deed was registered for it.
- Added these two running update histories and instructions to maintain them.

## 2026-09-20 — housing, placement, and client assets

- Added settings for structure placement on any planet and optional demolition codes.
- Corrected custom house construction, deed names, descriptions, and management-terminal setup.
- Fixed gray windows on three windowed-house variants; user testing confirmed improvement.
- VIP bunker clarification (2026-09-23): its single-level layout is intentional;
  no lower floor or elevator fix is needed.
- The user confirmed merchant tents are working and no older Singing Mountain
  huts existed, closing the tent and hut-migration notes.
- Corrected a conflicting Mustafar snapshot ID and guarded invalid interior NPC spawns;
  the user confirmed the misplaced Naboo NPCs are fixed.
- Added house, city, and garden deed spawn commands and tracked the `mtg_patch_024` assets.
- Kept tests local and ignored while supporting checkouts without local test files.

## 2026-09-17–19 — shutdown, recovery, structures, and skills

- Improved zone cleanup performance and shutdown timing reports.
- Improved shutdown client disconnections and player/account/IP count reporting.
- Added recovery for vehicles left unusable in the world after an improper shutdown.
- Added `/server orphanstructures` to find suspect structures and their locations.
- Corrected structure maintenance ownership checks and unnecessary credit lookups.
- Added alphabetical skill surrender/revocation lists, dependent-skill removal, and confirmation dialogs.
- Added newer terrain-boundary support and reduced CMake configuration warnings.
- Disabled automatic startup of the custom tutorial screenplays and example script.

## 2026-09-15–16 — baseline, compatibility, and configuration

- Realigned MTGServer settings with Core3 defaults and separated local configuration.
- Restored resource deed/stack limits and factory-crate quantities; corrected
  selected world regions, house elevators, shield, and quest-item settings.
- Fixed legacy appearance/mesh loading and supported additional index formats.
- Fixed hairstyle compatibility when multiple player templates share a hair asset.
- Corrected resource-container and battlefield-station template inheritance handling.
- Expanded and organized `config.lua`, removed unused settings, and corrected option names.
- Normalized formatting throughout `bin` and `src`.
- Established project guidance, explicit commit approval, user-controlled Core3 builds,
  and local-only handling of generated resource spawns.

## Latest testing results — 2026-09-24

- The user confirmed `/server travelstatus`, startup countdown/announcement,
  Nayli arrivals, region messages/town markers, planetary maps, and weather work.
  The latest boot had no reported errors. The subsequent Mustafar map size and
  port relocation changes above need their own retest.
- Mensix travel in/out was confirmed before its relocation. Remaining unconfirmed
  items include the new placement/map checks, complete port/fare/return-trip testing, and
  Frostline's individual services and placement. C++ unit-test execution has
  not been reported. The sprite diagnostic identified `appearance/ui_destroy.spr`
  and its investigation is closed; no appearance repair is needed.
- The subsequent planet-name localization fix needs TRE packaging and a label
  retest; the earlier confirmation of map rendering remains recorded.
- On September 23, the user confirmed the template-display labels are working
  and reported that all remaining active vehicles appear to work correctly. Their initial testing
  items are closed; disabled and retired deeds remain inactive.
- Before the Hoth additions, verified all 196 then-current patch files against
  the configured server TREs, with no missing or outdated files. The user has
  since packaged Hoth and confirmed startup after the city-name correction.
