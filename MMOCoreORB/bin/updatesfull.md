# Core3 project update history

Last updated: 2026-09-25.

Expanded high-level history of MTGServer's Core3 project work beginning on
2026-09-15, based on project commits, the current working changes, and user
testing reports. [updates.md](updates.md) contains the short version.

**Scope:** project history through the vehicle, inspection, Naboo Medium Style 2,
Hoth settlement, planetary maps, all-planet travel, weather, custom regions, and
shuttle startup notices, sprite diagnostics, planet-name strings, and Mustafar
boundary/placement corrections recorded with this revision.
“Confirmed” below means the user reported the stated behavior working;
standalone asset or source checks do not establish an in-game result.

## 2026-09-25 — planetary update commit

**Status:** included in this commit at the user's request. The commit groups the
pending Frostline settlement and its services; enabled-planet travel/fare,
region, weather, map and localization changes; shuttle startup countdowns,
reporting and administrator-only issue announcements; sprite logging; Mustafar
map/shadow compatibility, provisional port relocation and player boundary.
The implementation entries below now identify their committed status.

Reviewed the pending file list and whitespace. Previously completed standalone
checks and user-confirmed results remain recorded in their feature entries;
this commit does not imply a new Core3 build, deployment or runtime test.
The latest Mustafar placement/boundary and announcement audience tests remain
pending. Local `config-local.lua`, generated resource spawns, and ignored
`src/tests` files are excluded and preserved. No Core3 build or run performed.

## 2026-09-24 — Mustafar boundary and provisional Mensix relocation

**Status:** committed on 2026-09-25; standalone checks and independent source review pass.
Core3 compilation and in-game boundary/port tests remain for the user. No Core3
build or execution was performed.

### Keep players inside the mapped area

- Added `Core3.MustafarMapBounds`, defaulting to `true`, to both configuration
  files while preserving all other local values. The rectangle matches the
  verified Mustafar artwork: **X -6880…1120, Y -1024…6976**. Administrators are
  restricted too, as requested; the option has no effect on other planets.
- Check world and cell movement before position updates, including mounted
  packets. Also check player teleports and zone transfers. Existing zone
  insertion calls the guarded teleport, covering saved positions after login.
  Valid cell destinations use world coordinates for the boundary check.
- Stop movement and recover to an inside position when the player crosses an
  edge. An already-outside character or an invalid cross-planet destination uses
  the live **Mensix Mining Facility arrival point**. Recovery follows later Lua
  port relocations automatically. Coordinates are never clamped onto unknown
  terrain; a missing/invalid recovery point produces an error and rejects the
  outside destination. A valid inside teleport remains possible.
- Standalone checks pass for **2,058** boundary/nonfinite/random cases, map-bounds
  agreement, config values and integration ordering. A local ignored C++
  regression is prepared but unrun. Independent review covered API types,
  mounted correction, cell conversion and login/teleport reentrancy.

### Temporary flat port site

The user confirmed the final map alignment and shadows, but still found the
port/city placement unsuitable. They authorized a provisional flat site while
researching the eventual location. Moved the existing port from **-1760,100** to
**-1100,2400**, base height **37.696545**. Arrival and server/client town center
are now **-1100,2365**, height **37.843140**, with unchanged 3m arrival spread and
400m region radius. No additional city buildings or terrain edits were made.

- The 1m survey grid checked **2,704 points** across the footprint, doorway and
  arrival apron: all at **37.843140**, with no detected lava/impassable overlap
  and at least 37.84m clearance above registered water/lava surfaces. The nearest
  road centerline is about 1.6km away; this is a provisional plateau, not an
  assertion of the canonical town location or runtime navigation quality.
- Preserved the original snapshot rock, port/cell IDs and hierarchy, root
  rotation, portal layout, services, travel name, permissions and fares. Exact
  snapshot comparison permits only the root translation. Server arrival and
  both client/server regions move together.
- Port/region checks pass for terrain, real Lua service bindings, portal/cell
  integrity, IDs across 29 snapshots, 24 arrival squares and 336 region boundary
  samples. The actual-client map regression also passes, permitting only the
  approved port translation while protecting unrelated snapshot bytes.
- Read-only inspection found an unused alternate snapshot with a named Mensix
  facility around **-2420.5,1671.08**. Nothing from that snapshot was imported;
  choosing or restoring the complete town remains separate future work.

**Deployment/testing:** build and restart Core3 for the boundary and announcement
audience change. Package the updated `snapshot/mustafar.ws` and
`datatables/clientregion/mustafar.iff` for server and client, deploy the matching
server Lua, and fully restart both. Check all four edges on foot and mounted,
including admins; outside teleports/saved login; valid interiors; and the option
disabled. Check arrival height, door access, ticket purchase, boarding/return
travel and the town marker at the new site. Earlier successful Mensix travel
does not establish those results for this new placement.
See [travel and boundary guide](planet-travel.md).

## 2026-09-24 — shuttle announcement audiences

**Status:** committed on 2026-09-25, implemented and statically checked; user build/restart
and runtime audience testing remain pending. Codex did not compile or run Core3.

- Kept the ordinary galaxy message focused on shuttle/starport service status.
  It no longer includes failed-port diagnostics or instructions to contact admins.
- If startup recorded issues or no shuttle reached initial boarding, send a
  separate summary and `/server travelstatus` hint only to online administrators
  at level **15+**, matching that command's permission. Issue details and the
  admin summary remain in server logs. No ready shuttle produces a truthful
  public availability-unconfirmed message rather than a false operating notice.
- Copy player references under ChatManager's existing player-map lock, then
  release the lock before inspecting permissions and sending direct messages.
  Startup tracking, one-time release, countdowns, schedules and history remain
  byte-preserved before the announcement block; no new IDL/API was needed.
- A standalone source checker rejects nine privacy/permission/locking mutations.
  Existing timing (2,106 landing cases), report and entry-point integration
  checks pass. Test with a normal player and administrator online together;
  confirm only the admin sees any startup issue summary.

## 2026-09-24 — actual-client map compatibility and Mustafar shadow palettes

**Status:** committed on 2026-09-25; the user subsequently confirmed Mustafar alignment,
Kashyyyk Main/Dead Forest maps, and Mustafar shadows look good. The unusual
combined Kashyyyk atlas is accepted for now. Mensix's physical port/city location
is a separate content issue; map-click waypoint testing was not specifically
reported. Before this correction, the user found Mustafar map size and positions wrong,
Kashyyyk Main landmarks offset more than Dead Forest, and the combined atlas
visually unusual. Those results supersede the earlier pending map conclusions.
The new shadow screenshot was captured near **X -9, height 137, Y 11**.

### Confirmed compatibility causes

The user supplied the regular `SWGEmu.exe` under the read-only TRE directory.
Static inspection confirmed its SHA-256 as
`58012e57cebc499454812ba7ed96b1289db01e520963b4fc364edb41c322b2a8`.
Neither the executable nor Core3 was run or modified.

- **Maps:** this binary reads `MapWidth`, but its forward and inverse coordinate
  functions contain only origin-centered scaling. It does not read the newer
  map-adjustment table. The staged and installed TRE assets matched, so the
  previous Mustafar **360,372** and Main **0,50** offsets were being ignored.
  Prior tests checked newer SWG-Source code, whose required GCW controls are also
  absent from this client's UI; those tests could not establish compatibility.
- **Shadows:** this binary's lighting-ramp validator requires **256×8** pixels.
  All five Mustafar ramps were **256×10**, which follows the newer format. The
  actual rejection path copies a fully opaque middle-gray shadow color. This
  identifies a concrete cause for the pale solid silhouettes in the screenshot.

### Implemented map correction

- Removed the unsupported offset dependency. `map_adjustments.iff` now contains
  zero rows as a valid override of the already packaged table. Both buildout
  overrides remain empty. Existing unrelated map pages and the ticket UI stay
  byte-identical to the preceding working tree.
- **Mustafar:** native **16384m** frame, image at **1312,1216**, size **8000m**.
  This reproduces the real crop **X -6880…1120, Y -1024…6976** without offsets.
  World 0,0 remains near the artwork's lower-right. At minimum zoom the picture
  occupies 48.8% of the frame; use manual zoom/pan. A large automatically centered
  crop cannot be obtained with an offset table this client ignores. No terrain,
  port, snapshot or world coordinates change in this correction.
- **Kashyyyk Main:** a centered **2304m** view contains the revised **2016m**
  artwork centered at **-16,100**. Image placement is **128,44**; it fills 87.5%
  of the canvas width. Bitmap and landmark comparisons favor this crop over the
  previous approximation, but the original art remains locally inaccurate.
- **Dead Forest:** a centered **1152m** view contains **1056m** artwork centered
  at **24,0**. Image placement is **72,48**; it fills 91.67% of the width. This
  also improves agreement with bridge and terrain positions without map offsets.
- **Combined Kashyyyk:** retains the full terrain frame and partial three-region
  atlas, using the revised Main/Dead Forest crops. The separated small pictures
  reflect the real scene layout. No aligned whole-world map exists among these
  textures; stretching the duplicate Kachirho image would misplace locations.

### Implemented shadow correction

Added compatible `terrain/colorramp/mustafar_{global,cool,hot,hot_02,poison}.tga`
files to `mtg_patch_024`. Each is now 256×8 with its original 32-bit color format
and logical channels 0–7 intact. Bottom-origin TGA storage is accounted for;
only the two newer lighting channels are omitted. Original dark, translucent
shadow colors replace the incompatible input. Sun angles, the fixed environment
time, terrain, fog, clouds and weather row values are unchanged.

**Validation/deployment:** read-only executable checks confirm the supported map
transform, rejected original lighting dimensions and opaque-gray fallback.
Independent image decoding verifies all **10,240 retained pixels**, while
**75 unrelated palettes** and terrain/environment assets remain unchanged.
The existing Mustafar weather regression passes for **55 rows and 26 dependencies**.
Map regressions now use the actual-client transform rather than the superseded
newer-client offset model: **13,872** coordinate/packing/inverse cases and **15**
Mustafar ticket cases pass. Existing enabled-map, ticket-page and ticket-label
regressions also pass. Test evidence stays local under ignored `src/tests`.

Repackage `mtg_patch_024`, including the updated planetary UI, the empty offset
and buildout overrides, the pending ticket UI and all five new palettes. Fully
restart the client; no Core3 build is required. Retest map-click waypoints,
player/port landmarks and zooming in all four scenes. Check Mustafar shadows at
the reported point and on flatter terrain: the format error is repaired, but
long/fragmented projections on slopes may involve separate client shadow geometry.
The user subsequently confirmed the map alignment/presentation and shadows as
noted above, closing those reported defects. The provisional Mensix placement
remains separate, and varied waypoint/weather/slope cases remain regression work.
See [map guide](planet-map-coverage.md) and [weather/shadow guide](planet-weather.md).

## 2026-09-24 — Kashyyyk regional and combined scene maps

**Historical status:** the user subsequently reported alignment problems.
The crop sizes and offset approach recorded here were superseded by the
actual-client correction above. No Core3 compilation or execution was performed.

- Connected the existing textures through new plain `kashyyyk_main`,
  `kashyyyk_dead_forest` and `kashyyyk` map pages. Previously only region-suffixed
  pages existed, without the metadata needed to select them. Existing regional
  aliases and the conceptual Show Planet overview remain intact.
- Main's 4096m terrain contains a smaller Kachirho crop. Height/path comparisons
  and snapshot landmarks support an approximate 2048m view centered at **0,100**.
  Added its offset-only **0,50** row to the planetary map adjustment table while
  preserving the pending Mustafar row. Both forward marker placement and inverse
  waypoint conversion use this offset; no buildout coordinate changes are needed.
- Dead Forest's 4096m terrain has height/path layers within a 1000m square.
  Prepared a **1024m** map view centered at **0,0**, checked against rivers,
  bridges, the northern cave and webweaver area. The artwork differs locally:
  its southern gate lies roughly 40m west and 54m north of the snapshot position.
  Neither new crop is claimed to be exact recovered author metadata.
- Confirmed `ui_map_kashyyyk.dds` and `ui_map_kashyyyk_main.dds` are byte-identical
  Kachirho images. The combined 16384m scene now uses a partial atlas with that
  image plus Dead Forest centered at **-1500,1500** and Hunting Grounds centered
  at **0,-3458**. Regional terrain bounds establish the translations; a shared
  Dead Forest snapshot landmark independently confirms its displacement.
  Unmapped areas remain blank and existing zoom controls provide regional detail.
- Existing DDS files, terrain, snapshots, travel points, ticket UI and prior
  Mustafar content are preserved. Only the planetary UI and its offset table
  change for this follow-up.

**Validation:** standalone checks pass for 11,560 marker, packing and inverse
waypoint cases across eight zoom extents, all referenced DDS payloads and titles,
actual terrain sizes, byte preservation of existing pages/offset rows, and the
18 shared Dead Forest snapshot landmarks. Existing enabled-map, Mustafar and
ticket UI regressions also pass. These tests validate the coordinate transform,
not exact landmark accuracy in the original artwork.

**Deployment/testing:** repackage `ui/ui_planet_map.inc` and
`datatables/planetary_map/map_adjustments.iff` alongside the pending Mustafar
assets, then fully restart the client. Existing TRE files supply the textures.
Verify all three scene pages, player/port placement, zoom/resize behavior and
map-click waypoint coordinates. Main's Kachirho Port is **-678,-160**; compare
Dead Forest's cave/bridges as well as its imperfect southern-gate artwork.
Actual client behavior and detailed landmark alignment remain unverified.
See [map coverage](planet-map-coverage.md) for coverage limits and instructions.

## 2026-09-24 — Mensix relocation and Mustafar map alignment

**Historical status:** the port site below is superseded by the provisional
**-1100,2400** placement above. The user confirmed the original Mensix port
supports travel in and out. The map-offset experiment below failed the subsequent
client retest and is superseded by the actual-client correction above.

- Diagnosed the small planetary map: the 8,000m image occupied only 48.8% of each
  dimension of a 16,384m scene canvas. The old Mensix root at 308, -1095 and
  arrival at 308, -1130 were south of the image's -1024 southern edge.
- Moved the same stock outpost starport to **-1760, 100**, base height
  **116.245563**, with arrival **-1760, 65**, height **116.392159**. Its two snapshot
  IDs, cell hierarchy, portal CRC, child definitions, route name, permissions,
  landing range, and fares remain intact. The original snapshot rock is preserved.
- The source-derived terrain survey sampled 2,704 points on and around the
  footprint/arrival apron, all at height 116.392159 and more than 16m above the
  overlapping lava surface. Samples avoid road-height influence and potentially
  active impassable boundaries. A road lies about 68m from the building center;
  road access, client geometry, and navigation remain in-game checks.
- Moved the server and client Mensix town circles together to **-1760, 65**, keeping
  their existing 400m radius, localization, and city/no-build/no-spawn flags.
- Initially supplied regional metadata to select the existing Mustafar regional
  page. The user's subsequent screenshot at world **0,0** still matched the plain
  inset page: the marker was approximately 86% across and 87.2% down the artwork.
  The configured server TRE contained the experimental assets, but the screenshot
  did not demonstrate that the client selected the intended regional page.
- A follow-up centered the existing artwork across the 16,384m world after the
  user requested 0,0 at the image center. That met the centering request but
  distorted its geographic relationship with the terrain. The user's next report
  of locations still being wrong supersedes that approach. Its standalone
  coordinate checks did not establish artwork/terrain alignment.
- The terrain file is **16384m** wide, like the standard planets. The authored
  map image covers **8000m**, from (-6880, -1024) through (1120, 6976), centered
  at **(-2880, 2976)**. Therefore world 0,0 belongs at about **86% across and
  87.2% down** this regional artwork. A centered 0,0 image would need different
  full-world artwork; changing the display does not move the terrain.
- The subsequently superseded plain `mustafar` page used an **8192m** view with the 8000m artwork
  inset by 96m on each edge. Added one offset-only Mustafar row to
  `datatables/planetary_map/map_adjustments.iff`, using offsets **360,372**.
  The client squares its scroll canvas; at its 512-pixel reference scale these
  values reproduce the exact world-center offset without buildout metadata.
  The client applies the offset to markers and reverses it for waypoint creation.
  The image occupies 97.66% of the canvas width/height, with correct geographic
  bounds instead of the former stretched interpretation.
- Corrected the Mustafar ticket page separately. That client uses a full-terrain
  frame rather than the planetary-map offsets, so its artwork must occupy the
  actual 8000m crop within 16384m. The matching ticket image placement is
  1312,1216 on that reference canvas. Travel buttons, route names and fares remain
  unchanged; only the picture behind the Mustafar buttons is repositioned.
- Both buildout tables remain valid empty overrides to disable the already
  packaged regional experiment. No region-relative waypoint or music behavior
  is enabled. The terrain, original DDS, snapshot, relocated port and city circles
  are unchanged by this follow-up. Cached client code supports the map-width and
  offset fields, but its behavior in the user's actual client still needs testing.

**Validation/deployment:** standalone source/geometry tests cover the authored
bounds, map packing across square zoom sizes, forward and inverse waypoint
coordinates, ticket markers on rectangular frames, table schemas, and exact
preservation of unrelated UI. Repackage both `ui/ui_planet_map.inc` and
`ui/ui_ticketpurchase.inc`, the new `datatables/planetary_map/map_adjustments.iff`,
and both empty buildout tables from `mtg_patch_024`. Fully restart the client;
keep server/client patch versions synchronized. No Core3 build is required.
Recheck recognizable terrain landmarks and port/player markers, then create a
map-click waypoint and verify its world coordinates. Earlier Mensix relocation
checks passed independently; placement/travel retesting remains separate.

### Initial Mustafar shadow audit — superseded by executable evidence

The initial read-only audit found structurally valid 25-column weather data,
55 rows and five 256×10 color ramps under newer-client rules. That did not verify
compatibility with the actual executable. The later screenshot and supplied
client exposed its strict 8-row requirement and opaque-gray fallback, now repaired
by the palette conversion above. The initial source-only result is retained in
ignored `src/tests/mustafar_shadow_audit.json` as historical evidence, not a
current compatibility assessment. Actual post-fix shadow rendering remains pending.

## 2026-09-24 — planet-name strings

**Status:** committed on 2026-09-25; standalone string-table and coverage checks pass.
Requires server/client TRE packaging, restart, and in-game label verification.
No Core3 rebuild is required.

- Traced the user's `planet_n:[chandrila]` text to missing entries in the
  configured `string/en/planet_n.stf`, supplied by `mtg_planets.tre`. Chandrila,
  Coruscant, and Moraband were the only missing configured scene keys in that
  table; added their display names to a complete replacement in `mtg_patch_024`.
- Audited the separate `zone_n` table used by planetary-map titles and added its
  eight missing keys: Coruscant, Mandalore, Moraband, Mustafar, Taanab, Kashyyyk
  Hunting Grounds, Rryatt Trail, and Southern Kashyyyk. Reused existing planet
  names for the missing map titles. Existing authored labels such as Hoth
  Wastelands, Dromund Kaas, and Kachirho remain unchanged.
- Preserved all 63 original `planet_n` entries and 89 original `zone_n` entries,
  including their IDs and raw value/name records. Added IDs follow the highest
  existing ID so the original table's stale next-ID field cannot cause collisions.
  The resulting tables contain 66 and 97 entries respectively.
- Both tables now resolve all **39 configured scene keys** (29 ground and ten
  space), using the union of default/local configuration. All 29 relevant
  planetary-map page titles, including the 11 existing regional aliases, resolve.
  Existing literal planet UI references also resolve. No Lua, C++, map layout,
  travel route, or fare changes were necessary.

**Validation:** independently parsed the output tables, verified unique IDs,
existing-record preservation, exact additions, enabled-scene/map-title coverage,
and idempotent generation. Preparation and audit artifacts stay ignored under
`src/tests`. Original TRE archives were read only. Package
`string/en/planet_n.stf` and `string/en/zone_n.stf` from `mtg_patch_024` into the
patch used by server and clients, restart both, and verify the reported labels
and map titles. Codex did not build/run Core3 or create a commit. The earlier
user confirmation of map rendering remains valid; these new label additions
await their own in-game retest.

## 2026-09-24 — sprite appearance diagnostics

**Status:** committed on 2026-09-25; read-only asset audit, source review, and whitespace
checks pass. On 2026-09-24, the user's console output identified
`appearance/ui_destroy.spr`, confirming the diagnostic works and closing the
investigation. No appearance repair is needed.

- Traced `TemplateManager: unknown appearance type SPRT` to the appearance
  dispatch switch, which recognized other visual-only types but omitted sprites.
  The latest three occurrences were immediately before the same character's
  login; no filename was included in the old message.
- Audited the 24 configured TRE archives: 100 sprite appearances are present,
  with UI icon names. Among 44,401 scanned object and appearance reference files,
  `appearance/ui_destroy.spr` is the only sprite linked from object templates.
  Its live TRE references are the mission object, mission-list entry, and faction
  shuttle-extraction intangible. All eight dependencies resolve and parse.
  The user's subsequent runtime output confirmed this appearance as the trigger.
  Deprecated Lua comment blocks were not treated as active overrides.
- Found no scanned detail appearance whose final child resolves to a sprite.
  Recognizing `SPRT` preserves the existing no-collision-mesh result, with no
  changes to geometry loading, client visuals, or other appearance types.
- Replaced the misleading unknown-type error for `SPRT` with an informational
  message, initially visible in both the console and log as requested:
  `Sprite appearance has no server collision mesh: <filename>`.
  The user subsequently supplied the message naming `appearance/ui_destroy.spr`.
  After confirming that asset needs no repair, the user requested log-only output.
  Changed this diagnostic's `info(true)` to `info(false)`, retaining the filename
  in the log while silencing this expected message in the console.
  Genuine unknown appearance errors also now include the originating filename.

This is an expected visual-only UI appearance. Its lack of server collision
geometry is normal; the informational message does not indicate a damaged or
missing asset. The log-only follow-up awaits the user's next Core3 build/restart.
Codex did not build/run Core3 or create a commit, and no Lua/TRE replacement is needed.
The read-only audit artifact is local and ignored under
`src/tests/sprt_asset_candidates.json`.

## 2026-09-24 — shuttle startup notices

**Status:** committed on 2026-09-25. On 2026-09-24, the user confirmed `/server travelstatus`,
the startup countdown, and the announcement work, and reported no errors on the
latest boot. This closes initial build/runtime acceptance for these features.
C++ unit-test execution has not been reported.

- Added a shared startup check to the ticket terminal, purchase command,
  boarding command, and travel-coupon confirmation. Inventory ticket use,
  ticket collectors, and ticket-selection dialogs all reach the boarding check.
  The checks run before charging credits, creating/consuming tickets, using a
  coupon, or moving the player.
- During `Core3.ShuttleZoneComponent.BootDelay`, players see the remaining time
  in minutes and seconds. The existing setting still defaults to five minutes
  measured from server startup. After registration, the message estimates the
  remaining initial landing time. If initialization is still pending, it says
  travel is finishing startup instead of showing a misleading zero countdown.
- Added one transient monitor per server startup. It waits for the initially
  loaded shuttles to finish their first boarding preparation, then broadcasts:
  **“Shuttle and starport services are now available throughout the galaxy.
  Normal boarding schedules apply.”** First departure is also recorded so a
  delayed monitor cannot miss an earlier boarding window.
- Kept the existing boot delay and shuttle-cycle calculations intact. Later
  shuttles and admin lock/unlock operations do not restart the startup check or
  repeat its announcement. Theed retains its normal boarding exception after
  the shared startup check opens.
- Registration failures and removed shuttles do not leave the galaxy waiting
  indefinitely. Completion announces when startup checks recorded issues, or
  explicitly reports that no initial boarding was observed. Both messages direct
  administrators to `/server travelstatus`. Failed startup records do not prove
  that each associated port is unavailable. Exception cleanup covers initial
  scheduling, registration,
  and monitoring; weak references protect delayed tasks during teardown.
  Shutdown cancels the monitor and blocks further startup announcements.
- Corrected the user's reported const-qualification build failure in the monitor:
  engine weak-reference resolution uses a non-const `get()`, so the local snapshot
  loop now accesses mutable reference wrappers. Shared tracking data stays protected
  by the same mutex; scheduling and readiness behavior are unchanged. The user
  subsequently reached the broadcast at runtime.
- Added the admin-only `/server travelstatus` report. Its default view lists
  failed or still-pending startup records; `/server travelstatus all` includes
  successful records and discarded duplicates. Optional scene and page arguments
  filter the report, with 20 records per page. Rows retain planet, matched/nearest
  port name when available, world X/Y, shuttle object ID, status, and reason.
  The report is read-only and describes this boot's startup history, not current
  boarding schedules, later failures, or ports with no startup shuttle at all.
- Retain copied diagnostic metadata after startup while releasing runtime
  references. Failed records are written to the log without individual console
  messages. Explicit reasons cover registration, scheduling, missing references,
  binding failures, and initial boarding checks.
- Fixed a race in which the monitor could observe an intentionally removed
  duplicate before its registration callback reported it as discarded. Pending
  registrations now own their final outcome; the monitor only inspects shuttles
  waiting for their initial boarding. This is a possible cause of the earlier
  warning, not a confirmed diagnosis of that boot.
- Checked the earlier startup log: its September 24, 15:17:17 warning contains
  no affected shuttle identities or associated registration errors. The old
  monitor cleared its records, so that boot's affected objects cannot be
  identified retrospectively from the available log. The user subsequently
  confirmed the new report works and reported an error-free latest boot.

**Validation:** standalone checks verify four gates before travel side effects,
three boarding entry routes, coupon handling, and exact preservation of the
normal travel/credit code. Eight deliberately broken source variants were rejected.
Source-derived arithmetic checks pass for 12 deadline edge cases and 2,106
landing-boundary samples; lifecycle and whitespace checks pass. Added six local,
ignored C++ regression tests for countdown rounding, signed limits, readable
durations, and the existing strict boarding boundary. Added four further local
C++ tests for report arguments, scene names, pagination, and invalid input.
Codex did not build or run these tests; their execution has not been reported.
Standalone reporting
checks cover permissions, retained metadata, duplicate handling, and reference
release, and reject 12 deliberately broken reporting variants. Source checks
do not establish runtime concurrency behavior.

Deployment requires Core3 with IDL regeneration and a restart; the user's
successful runtime report confirms deployment of the startup diagnostics.
No new client/TRE assets or configuration values are required. The startup
section in [planet-travel.md](planet-travel.md) retains repeatable checks.

## 2026-09-24 — custom planet regions

**Status:** committed on 2026-09-25; standalone checks pass. On 2026-09-24, the user confirmed
entering/leaving messages and town markers work, closing initial region UI
validation. Codex did not build or run Core3.

- Audited all 17 enabled custom ground scenes, comparing server region Lua with
  client circles, localized strings, snapshot settlements, and travel points.
  The nine custom travel worlds now have matching definitions for **20 towns
  and one named district**; all **24 travel arrival squares** fit their regions.
- Added the missing server town regions for Chandrila, Coruscant, Kaas,
  Kashyyyk Main, Mandalore, Moraband, and Taanab. Renamed the unused misspelled
  `tanaab_regions.lua` to the loader-required `taanab_regions.lua`. Added an
  explicit empty `dungeon2_regions.lua` matching the existing dungeon convention.
- Reused authored town boundaries where supported by snapshots. Moved Kachirho's
  incorrect origin-centered client circle to its starport compound at **-678,
  -160**, radius **100m**. Matched Mensix's client circle to its existing server
  region initially at **308, -1130**, radius **400m**; both were later moved
  with the port as recorded in the September 24 relocation entry. Added Moraband Outpost at **-1715,
  -641**, radius **125m**, excluding the nearby temple ruins.
- Expanded Norg Bral from **375m to 450m** to cover existing cloning, guild, and
  guard buildings; expanded Bralsin from **125m to 175m** for its western building.
  Unified the southern Hoth region as **South Range Outpost**, preserving its
  **250m** client radius and matching the server boundary. Frostline is unchanged.
- Added Coruscant's five missing district strings and the missing Mensix and
  Moraband labels; corrected Kachirho's string-table reference. All CITY names
  use complete, resolved StringIds, avoiding the earlier Hoth naming failure.
- Settled areas use city, no-spawn, and no-build flags. Existing city navigation
  and city-hall distance restrictions apply; existing structures are not removed.
  Coruscant's authored Palace District has no nearby snapshot settlement, so it
  remains a named area without new city restrictions or navigation generation.
- Preserved the unconfigured combined Kashyyyk scene and empty auxiliary scene
  regions. Seven auxiliary scenes have no authored client-region table in the
  available archives; no speculative towns or spawn groups were created.

**Validation:** evaluated all 29 enabled ground-scene region Lua files with their
exact loader globals. Verified server/client agreement, resolved labels, valid
terrain bounds, no overlapping town circles, 336 boundary samples, full travel
landing-area coverage, and snapshot evidence for corrected compound/boundary
locations. Binary checks verify ten intended field changes and preservation of
unrelated rows and chunks. Hoth and custom travel regression checks pass.
See [planet-regions.md](planet-regions.md) for coordinates, deployment, and testing.
Deploy Lua and the matching server/client TRE, then restart; these region changes
require no Core3 rebuild. Initial startup may generate additional city navigation
data. The audit checks horizontal region coverage; arrival heights require
separate terrain/collision checks. The user confirmed Nayli's corrected arrival
on 2026-09-24; its retest is closed.

## 2026-09-23 — planetary weather

**Status:** committed on 2026-09-25; the user confirmed weather is working on 2026-09-24,
closing initial runtime validation. These configuration changes need no Core3 rebuild.

- Enabled the existing weather manager for all **19 travel planets**, plus the
  Kashyyyk Hunting Grounds, Dead Forest, and Rryatt Trail: **22 outdoor scenes**.
  Each active scene now has an explicit profile keyed by its exact scene name.
- Added calmer weather for Chandrila, Coruscant, Taanab, and MTG's Mandalore;
  more active patterns for Hoth and Dromund Kaas; dry dust-storm conditions for
  Moraband; and volcanic atmospheric variation for Mustafar. Weather stability
  and duration are gameplay choices informed by official setting descriptions
  and the actual effects available in the client archives.
- Preserved the ten original planets' numeric settings and Kachirho's existing
  profile. Set `hasDamagingSandstorms = 0` explicitly throughout, retaining
  atmospheric effects without enabling optional damage or forced dismounts.
- Kept tutorial and dungeon weather disabled. The separate, previously
  unconfigured combined `kashyyyk` scene remains unconfigured because enabling
  its planet configuration would also activate a 4,679-object snapshot. The
  existing Kachirho travel scene and three additional outdoor regions are covered.
- Reorganized weather profiles alphabetically and replaced stale configuration
  comments. The current engine immediately randomizes its startup pattern;
  the old documented transition-time variables are not read by the source.
- Added Mustafar's missing client environment table using existing Mustafar
  assets: 55 rows cover its 11 terrain families and all five weather intensities.
  Clear conditions transition through haze to drifting embers, with stronger
  haze/wind/cloud movement at higher intensities. The ember effect retains its
  original density and 20-particle cap; no rain or new lightning effect is added.
  This replaces generic fallback lighting with authored volcanic color palettes.
  The weather guide retains day/night checks for future tuning.

See [planet-weather.md](planet-weather.md) for every profile, official climate
references, client-effect limitations, and `/server weather` testing commands.
Standalone Lua checks pass for all profiles, supported fields, valid timing
and intensity ranges, preservation of existing climates, and unchanged travel
configuration. Hoth, Mustafar, and custom-planet travel regressions also pass.
Mustafar client-table checks pass for schema, all family/intensity pairs, 26
resolved asset dependencies, and preservation of existing terrain and sky data.
Other planets' precipitation dependencies resolve. Existing Moraband sky/music,
Coruscant music, and Kashyyyk elder-music reference issues were recorded in the
weather guide; those pre-existing environment entries remain unchanged.
Deploy the two Lua files, package the Mustafar environment asset for server and
clients, and restart. The weather changes do not require a C++ rebuild; the
earlier travel-command correction retains its separate build requirement.

## 2026-09-23 — all-planet ticket travel

**Status:** committed on 2026-09-25; standalone checks pass. The user confirmed startup
diagnostics and Nayli's arrival correction on 2026-09-24. Complete fare, boarding,
arrival, and return-trip coverage of the new ports has not yet been reported.

- Replaced the sparse and inconsistent travel-fare matrix with a complete
  19-planet matrix: **100 credits** on the diagonal for local travel and
  **500 credits** for every cross-planet pair. These are one-way base fares;
  round trips cost twice as much, with existing city taxes and coupons retained.
  Kashyyyk uses its established `kashyyyk_main` travel scene. Dungeon/tutorial
  scenes and other Kashyyyk regions are not additional interplanetary destinations.
- Added seven ticket-window bindings, selectors, and destination pages to
  complete the 19-planet selection. Five use existing/newly supplied map images;
  Moraband and Kashyyyk use neutral backgrounds while their map work is pending.
  Preserved the corrected ticket labels and existing Hoth/original planet controls.
- Added named server configurations for Chandrila, Coruscant, Kaas, Mandalore,
  Moraband, and Taanab. Registered **19 existing authored ports**, using proven
  stock landing offsets rotated into each port's actual placement. Existing
  starports/outposts support interplanetary travel; shuttleports remain local.
- Added an outpost starport for Mustafar's existing Mensix Mining Facility
  destination. Its initial building site was **308, -1095**, with arrivals at
  **308, -1130**; the September 24 relocation entry supersedes these coordinates.
  Set its base height to **-12.387912** and arrival height to **-12.003315** after
  checking terrain elevation, slope, water, and impassable areas. It uses the
  existing port template's terminal, ticket collector, and shuttle. The original
  rock and earlier collision fix remain byte-for-byte intact; terrain is unchanged.
- Those configurations enable previously skipped snapshot loads. Before doing
  so, remapped **393 Mandalore IDs** that collided with Dantooine and **six Taanab
  IDs** that collided with Corellia, plus 100 affected parent references. Preserved
  all 1,922 objects' templates, geometry, transforms, and nesting. The original
  Dantooine/Corellia IDs, including the Drall Cave loot crate, remain unchanged.
- Corrected `PurchaseTicketCommand` to preserve underscores in scene identifiers;
  only location names decode underscores as spaces. This addresses the client
  sending `kashyyyk_main` while the server previously looked for `kashyyyk main`.

**Validation:** checked all 361 fare cells against both server and client loading
rules. All 19 destination scenes are enabled, have named Lua configurations,
and have incoming/interplanetary points backed by snapshot ports with travel
service children. Checked all 19 ticket bindings/pages; string and texture availability; button
spacing; and unrelated UI preservation. Evaluated production Lua and checked
300 possible shuttle-binding states and 7,560 terminal/collector/shuttle lookups.
All 419 unique shared templates across 4,136 newly loaded snapshot objects exist,
parse correctly, and have reachable server Lua registrations. Snapshot checks
found no remaining cross-planet ID collisions in available enabled snapshots.
Mustafar checks also pass for terrain heights, new IDs, portal/cell data, service
bindings, and original snapshot preservation. Existing Naboo quest and bartender
spawn checks continue to pass after the additive Mustafar change.
No Core3 build or run was performed. Build for the command correction, package
the matching server/client TRE, deploy the updated Lua, and test purchases,
boarding, arrivals, return trips, and displayed prices. See [travel guide](planet-travel.md).

**Nayli Outpost arrival correction:** the user reported temporarily hovering
after arriving on Chandrila. The stock outpost's exterior apron is marked
non-solid, so its building origin height **18.9** does not represent a physical
floor there. Source-derived terrain calculations put the arrival centre near
**18.09**, about **0.81m lower**. Updated only Nayli's Lua arrival height to
**18.09** and its random landing range from **3m to 1m**, retaining its coordinates
and travel permissions. All 441 possible terrain-cache positions in the smaller
landing square are within **4.5cm** of that height. No nearby snapshot object
supplies a terrain-flattening modification at the site.

Actual Lua, terrain, unrelated-configuration preservation, custom-port bindings,
weather profiles, and Mustafar travel checks pass. The terrain calculation is a
standalone reproduction of the source formulas, not a Core3/client measurement.
On 2026-09-24, the user confirmed arrival no longer leaves the player stuck,
closing this retest. No new TRE packaging or Core3 rebuild was required for
the correction; Codex did not build/run Core3 or commit it.

**Startup snapshot review, 2026-09-23:** the user's 19:06 startup completed, but
logged `nullptr CELL OBJECT` for Mandalore outpost starport **8565664** at
**6285, -6211**. Its installed interior cell ID **8565838** collides with a
decorative bol skull on Dantooine. The snapshot loader skips objects whose IDs
already exist, leaving the starport without cell 1 when creating its ticket
terminal. The earlier prepared Mandalore remap already changes that cell to
**1900200199** while preserving its parent, portal data, and cell number.
No additional snapshot or Lua change is needed for this error.
A focused standalone loader simulation reproduces the installed snapshot's
missing cell after Dantooine loads, and passes with the prepared snapshot.
The full snapshot-ID and hierarchy regression checks also pass.

At that September 23 inspection, 199 of 205 prepared patch files matched the
configured TREs; Mandalore/Taanab/Mustafar snapshots, the travel table, and both
UI files were older installed versions and needed packaging. This is historical
deployment evidence, not a current mismatch claim. On September 24, the user
reported no errors on the latest boot. The snapshot repair itself needs no C++
rebuild; Codex did not build or run Core3 during the investigation.

The four missing-snapshot warnings are separate: none of the 81 available TREs
contains the north/south Kashyyyk dungeon, Rryatt Trail, or tutorial snapshot,
including alternate snapshot/buildout searches. Tutorial's building is created
dynamically and loaded successfully. The three Kashyyyk scenes have terrain but
lack authored snapshot content. These missing assets remain unresolved; no
placeholder snapshots or zone-disable changes were made.

## 2026-09-23 — enabled planetary maps

**Status:** committed on 2026-09-25; standalone checks pass. The user confirmed planetary
maps are working on 2026-09-24, closing initial client validation. Missing images
and unresolved coverage listed below remain deferred content work.

- Compared enabled ground zones with the map pages and images in the configured
  TREs, and searched all 81 available archives for missing custom-planet maps.
- Added the missing `kaas` and `mandalore` UI pages using their existing images.
  Corrected `taanab`, which previously displayed the Talus image. All three
  images are 1024 by 1024 and already supplied by `mtg_planets.tre`; no new
  artwork or duplicated textures were required.
- Added plain scene pages for Mustafar and Kashyyyk Hunting Grounds, whose older
  `scene__region` pages were not selected without buildout metadata. Compared
  water/lava landmarks with authored terrain heightmaps to establish image bounds.
  Mustafar's 8,000m image is inset at its correct offset within the 16,384m
  terrain canvas, with proportional placement during zoom and resize. Hunting
  Grounds uses the image's centered 2,844m range. Existing regional pages remain.
- Preserved the Hoth map, original planet maps, regional pages, fallback page,
  and surrounding UI. This follow-up changes the planetary map, with no new
  ticket-routing or server configuration changes.
- The initial audit found no matching images for Chandrila, Coruscant, or Moraband/Korriban.
  The user subsequently supplied Chandrila and Coruscant images, plus a
  replacement Hoth texture. Connected the two missing pages, validated all three
  DDS files, and preserved the supplied texture bytes. Chandrila and Coruscant
  use 1024-square images; Hoth remains 1024 by 1014. All three supplied files match
  the highest-priority installed TRE. Coruscant is a city backdrop whose exact
  street alignment remains unverified. Moraband still has no matching image.
  Other Kashyyyk images have unresolved cropping, combined-terrain differences,
  or repeated dungeon areas, so their mappings were left unchanged. Details
  are in [planet-map coverage](planet-map-coverage.md), along with archive
  locations and patch texture filenames for additional map images.

**Validation:** checked XML structure and unique map names, configured texture
resolution, DDS dimensions and payloads, terrain widths, region-to-marker
coordinate calculations, and byte preservation outside the intended map changes.
Reviewed client UI packing to preserve Mustafar image proportions across zoom.
The Hoth map regression check also passes.
Codex did not build or run Core3. The user confirmed the planetary maps work on
2026-09-24. Packaging/restart and map checks remain documented for future changes;
these UI changes do not require a Core3 rebuild.

## 2026-09-23 — Hoth: Frostline Outpost

**Status:** committed on 2026-09-25; the user packaged the Hoth changes and confirmed successful
startup after the city-name correction. Town refinement and service checks remain.

- Added a modular world settlement centered at **671, 2035**, height **0**:
  starport, local shuttleport, cloning clinic, bank, cantina, functional vehicle
  garage, workshop, and two accommodation buildings. Twelve lights mark the
  approaches. Preserved all 94 original Hoth snapshot nodes and added 59 nodes,
  including 37 interior cells and a permanent bazaar terminal.
- Added a guarded town screenplay with a bank terminal, four public crafting
  stations, six basic profession trainers, and six neutral ambient NPCs. It
  checks required snapshot IDs, types, positions, and cell parents before
  spawning. Bazaar ID `1900100900` stays in the snapshot so auction listings
  continue to reference the same terminal after restarts.
- Added Hoth's missing planet configuration and protected city regions. Frostline
  also has a navigation area. The existing outpost near **0, -2000** remains and
  is connected as **South Range Outpost**.
- Registered **Frostline Starport**, **Frostline Shuttleport**, and **South Range
  Outpost**. All support local travel; the two starports support interplanetary
  tickets. Added a 100-credit Hoth base fare and 500-credit base fares between
  Hoth and the thirteen existing fare-table planets, preserving other fares.
  Enabled destinations and normal travel restrictions still apply. No Hoth
  space zone or personal-ship launch point was invented.
- Added Hoth entries to the planetary map and ticket-purchase UI, using the
  authored `ui_map_hoth` texture and Hoth planet-selector appearance already
  supplied by the configured planet archives. Hoth no longer depends on the
  planetary map's Dathomir fallback. Other map entries are preserved.
- Added the [settlement guide](hoth-frostline-outpost.md),
  [site plan](hoth-frostline-layout.svg), and
  [placement manifest](hoth-frostline-outpost.json).

**Validation:** standalone Lua evaluation, snapshot/ID preservation, portal
checksums and cell hierarchy, terrain heights, footprint clearance, service
templates/children, client template index, 1,142 object asset dependencies,
screenplay guards and registrations, shuttle startup ordering, travel binding,
fare-table preservation, and map/UI assets were checked. Terrain was checked at
the actual sites; this does not establish that all land around the town is flat.

**Deployment/testing:** package the updated `mtg_patch_024` for both server and
client and restart with the updated `bin/scripts`. Hoth is already enabled;
there are no C++ changes requiring a rebuild. First startup may generate
Frostline navigation data. Building access, ticket purchases/boarding/return
travel, cloning, bank/bazaar persistence, trainer conversations, and garage
repair remain user checks. The user confirmed planetary maps and region
messages/markers work on 2026-09-24. Codex did not build or run Core3;
the user performed startup testing.

**First packaged startup, 2026-09-23:** the installed 198 patch files matched the
prepared files. Hoth loaded all 153 snapshot objects and the travel fares, then
city navigation setup threw `ArrayIndexOutOfBoundsException`. The new city names
had been supplied as plain text to a loader that requires `@table:key` StringIds;
both became empty `@:` names. The failed zone-startup task never signaled that
its managers were ready, leaving startup waiting. Corrected both region entries
and added their labels to `string/en/hoth_region_names.stf`, preserving the two
original strings. Added regression checks for complete city StringIds and
matching localized labels. This data correction does not require a C++ rebuild;
the updated STF must be included in the server/client patch. The user subsequently
reported that the server loaded successfully and that the town is a useful first
pass, with further town work planned. This closes the startup failure, not all
of the individual travel, cloning, auction, and other service checks.

**Region notifications and ticket labels, 2026-09-23:** the user reported that
the southern outpost showed entering/leaving messages but Frostline did not.
Frostline already had a server city circle; NPC-city notifications instead
come from the client region table. Added a row to `datatables/clientregion/hoth.iff`
using the same **671, 2035** center, **260m** radius, and localized Frostline name.
Preserved the original southern row byte-for-byte, including its legacy
**Scavenger Starport** label and **250m** client radius. The server regions,
protection flags, navigation setup, and terrain remain unchanged.
The 2026-09-24 audit subsequently aligned the southern label and server radius;
see the newer custom planet regions entry above.

Corrected the ticket window's missing `travel_planet`, `travel_location`, and
`travel_travel_to` references and its unqualified `show_help` reference. Reused
existing `ui.stf` strings for **Planet**, **Location**, **Travel**, and **Show Help**;
no replacement string table is needed. All 41 ticket-UI string references resolve.
Standalone checks verify client/server Frostline geometry, boundary crossings,
localized labels, original region data, and preservation of all unrelated UI
content. Existing Hoth travel checks also pass. The user confirmed region
messages and town markers work on 2026-09-24. Ticket-label verification remains
part of travel UI testing. Codex did not build or run Core3; these client
corrections do not require a Core3 rebuild.

## 2026-09-20–23 — vehicles, inspection, and housing follow-up

### Vehicle creation and presentation

- Audited deed-to-control-device-to-vehicle links and corrected mismatched or
  incomplete registrations for walkers, Advanced Tantive IV, RIC-920, and other
  custom vehicles. The corresponding client files and mounting data are included
  in `mtg_patch_024`.
- Standardized vehicle names and added Star Wars descriptions. Vehicle names omit
  “Deed for:” and “Vehicle Deed.” Deed and datapad previews use the corresponding
  vehicle models, including the older TCG aliases and Swamp Speeder.
- Added a new USV-5 Modified Landspeeder deed and repaired its body and mount setup.
  Reused existing TCG deeds for eleven additional vehicle models, including
  Air-2, Geonosian Speeder, several podracers, Senate Pod, Republic Gunship,
  XJ-6, and the two additional jetpacks. Added missing female rider poses where
  compatible animations already existed.
- Retired the light-bending BARC and podracer deeds at the user's request.
  Kept their vehicle and control-device templates available for existing saved
  objects. The experimental translucent-material approach was withdrawn.
- Preserved the user's twelve disabled duplicate deed registrations. Current
  checks resolve **54 complete vehicle deed chains**, each with a distinct
  mobile template: 37 ordinary deeds and 17 TCG deeds. No new loot or vendor
  distribution was added.
- Disabled the unfinished AT-AT and blue, gold, green, purple, red, and silver
  Swoop Racer deed registrations on 2026-09-23 at the user's request. Removed
  their commands from the active reference and deferred them for later work.
  Retained their source files and assets; the working black racer stays active.
- Disabled the regular `atrt_deed.iff` registration on 2026-09-23 after the user
  reported that its missing-part appearance issue remained. Removed its active
  spawn command. Retained the vehicle/control-device templates and source assets;
  the camouflage `temp_walker1_deed.iff` stays available.
- Added [vehicle creation commands](vehicle-deed-commands.md), the
  [vehicle audit](vehicle-deed-audit.md), and the
  [additional-vehicle reference](additional-vehicle-deeds.md).

**Validation:** production Lua evaluation, shared-template lookup, localized
text, previews, mount data, and client asset dependencies were checked. The
current audit covers 54 active deed names, 54 complete previews, and 162 shared
templates in the client index. “Complete” here describes the checked asset/template
chain. **User result, 2026-09-23:** the user reported that all remaining active
vehicles appear to work correctly. Their initial in-game confirmation is closed;
the disabled regular AT-RT and other retired/deferred deeds remain inactive.

### Walking vehicles, STAP, and Advanced Tantive IV

- Corrected STAP mounting setup to address the client crash when mounting.
- Restored the Advanced Tantive IV's visible body and mounting links.
- Corrected rendering and rider attachments for AT-PT, regular/camouflage AT-RT,
  AT-ST, AT-XT, and the Grievous wheel bike so movement drives their animations.
  The server continues treating them as vehicles for ownership and storage.
- Attempted to repair the regular AT-RT's two highest-detail meshes after missing
  ankle and mechanical sections were reported. Reused the complete AT-RT geometry and
  joint bindings while preserving the rider attachment and regular main-body
  paint. Restored secondary parts retain the source camouflage material.

**Confirmed:** STAP mounting, Advanced Tantive IV visibility, and walker/wheel-bike
movement animations. **Unresolved, 2026-09-23:** the user reported that the later
regular AT-RT geometry change did not fix its appearance. The user then requested
removal of its deed, which is now disabled. The unsuccessful repair remains
documented for later work; the earlier movement-animation confirmation still stands.

### Wheel-bike jump controls

- Connected the wheel-bike hop to the player's configurable Jump action and typed
  `/jump`, with a two-second cooldown and checks for the linked rider and usable mount.
- Corrected the hop animation's priority so it plays over normal movement.
- Removed the Jump radial-menu entry at the user's request.
- Prevented handled jump social messages from asking the client to face a selected
  target. The player's actual selection is retained.

**Confirmed on 2026-09-23:** the user reported wheel-bike jumping working
appropriately following the target-turning correction. The hop uses the existing
animation; new flight or obstacle traversal mechanics were not implemented.

### Jetpacks and object inspection

- Extended the original jetpack's automatic mounting on call, automatic storage
  on dismount, and hover-height behavior to HK-47 and Merr-Sonn JT-12 jetpacks.
  Added the required client customization mappings. The user confirmed their
  appearance and mount/store behavior.
- Added `Core3.TangibleObject.ShowTemplate` to show **Server Template** and
  **Shared Template** `.iff` paths in tangible-object Examine information,
  including auction/vendor inspection. It defaults to `false`; when enabled,
  the diagnostic rows are visible to all viewers. On 2026-09-23, the user
  confirmed the setting works and clarified that it is primarily for testing.
- Corrected missing display-label strings reported afterward. The client
  lowercases unprefixed attribute names and resolves them through `obj_attr_n`.
  Added `server template` and `shared template` to
  `mtg_patch_024/string/en/obj_attr_n.stf`, preserving all 827 existing entries
  and their IDs. The displayed labels are **Server Template** and **Shared Template**.
  The existing server attribute messages remain unchanged; no Core3 rebuild is
  required. On 2026-09-23, the user confirmed the corrected labels are working.
  No separate attribute-description table is needed.

### Black Swoop Racer and archive verification

- Connected the user-supplied `swoop_black.dds` to a dedicated black Swoop Racer.
  Added the missing mobile registration, client vehicle template, appearance,
  materials, mount links, hover settings, and deed/control-device presentation.
- Used the standard swoop's geometry, rider position, handling, and effects.
  Applied the supplied texture at all four viewing-detail levels without
  changing the texture or recoloring the original swoop.
- Investigated the reported wrong deed icon and invisible but mountable vehicle.
  The configured archive contained the new meshes but old versions of the deed,
  three mount tables, and customization table. Those older files explained the
  symptoms. The working copies were already corrected and were staged together.
- Added a read-only packaged-asset comparison. It checks actual configured TRE
  contents against the working patch files, avoiding a local overlay that could
  hide an outdated archive. At diagnosis, the comparison found 185 matching files and
  seven outdated files: the five black-swoop files and two repaired AT-RT meshes.

**Validation:** the working black-swoop chain passes standalone checks, with
125 asset dependencies resolved. The integration preserved the supplied texture.
**Confirmed on 2026-09-23:** the user reported the black Swoop Racer working
after the package correction and made a further adjustment to its texture.
The user-adjusted texture is the current version; the black swoop is no longer
awaiting its initial in-game confirmation.

**Packaged-asset verification, 2026-09-23, before the Hoth additions:** the
configured server TREs matched all 196 then-current patch files, with zero
missing or different payloads. The new Hoth changes require a fresh package.
This read-only comparison uses the configured archives without a loose-file
overlay; it does not inspect the user's separate game-client installation.

### Naboo Medium Style 2 deployment follow-up — 2026-09-23

- The user confirmed that the existing deed could spawn but could not deploy.
  Its referenced player-building template was missing, so placement stopped
  before a house could be created.
- A broader asset search found the original Style 2 model and interior under
  `appearance/ply_nboo_house_m_s02_fp1.pob`. The earlier missing-building finding
  applied to the player template, not to all geometry for this style.
- Prepared the missing player-building registration and shared client template,
  placement footprint, management terminal, and signs using the original Style 2
  interior. Retained normal player ownership, maintenance, and construction behavior.
- Added a distinct building name and description, preserving all existing
  localization entries. The existing deed path is unchanged.

**Validation:** all 55 registered house deeds resolve to building definitions;
55 portal layouts, 134 child definitions, and 131 distinct client template paths
pass the standalone checks. The new Style 2 setup resolves 147 asset dependencies;
its terminal is within an authored interior floor and its placement footprint contains the
authored floor bounds. Existing player-template behavior and original geometry
are preserved. This is a Lua/TRE change and needs no Core3 rebuild.

**Confirmed on 2026-09-23:** the user reported that the Naboo Style 2 deed works.
The user moved the management terminal to a preferred position; that edit is
preserved. The current terminal is in bedroom cell 1, at `x=6.4, y=2.4, z=1.8`;
standalone checks validate it against that cell's authored floor. Core3 builds
and runtime testing remain under the user's control.

### Generic Medium Window Style 3 retired — 2026-09-23

The client deed asset exists in `mtg_patch_022.tre`, but no matching authored
Style 3 building was found in the available archives. The user chose to remove
this item from testing. No server deed registration or patch asset had been
created for it, so it remains inactive and is removed from the pending-work
and command references. The original read-only archive is unchanged.

### Project maintenance

- Restored local-only Git handling of `resource_manager_spawns.lua`, preserving
  its local contents after it had become tracked again.
- Limited compiled test discovery to files directly in `src/tests`, preventing
  downloaded reference code in subdirectories from entering the Core3 build.
- Added these two running update histories and guidance to maintain their content
  and testing status with future changes.

## 2026-09-20 — housing, placement, and client patch assets

Committed in `47ca1b6202` — **Add housing options, deed fixes, and client patch assets**.

### Placement and demolition options

- Added `Core3.StructureManager.AnyPlanet`, default `false`. Enabling it bypasses
  a structure's allowed-planet list and updates the deed's placement information.
  Other placement, ownership, city, and terrain requirements continue to apply.
- Added `Core3.StructureManager.RequireDestroyCode`, default `true`. Disabling
  it skips the numeric demolition code after the normal Yes/No confirmation.
- Added checks for unsupported placement zones, missing planet managers, and
  invalid city-cap queries. Updated local configuration while preserving the
  user's existing local choices.

### House deeds, construction, and windows

- Audited house deed registrations and their building links, including city,
  garden, barn, and diner definitions.
- Registered missing construction markers and corrected their types and
  navigation behavior. Barn and diner now use the normal construction phase
  instead of appearing instantly.
- Corrected the barn deed name and Medium Corellia House Style 2 naming.
  Added original descriptions to 29 custom deeds and repaired missing building
  descriptions while preserving existing prose.
- Corrected an AT-AT house-deed field encoding and placed the Singing Mountain
  hut management terminal in its valid interior cell.
- Fixed gray windows on Generic Small Window Style 1, Generic Small Window
  Style 2, and Generic Large Window Style 2 by correcting missing material
  references. The house geometry was preserved.
- Added [house, city, and garden creation commands](house-deed-spawn-commands.md).

**Confirmed:** the user reported the tested houses and window fixes working
before the commit. This does not imply every available deed was tested.

**VIP bunker clarification, 2026-09-23:** the user confirmed that this house is
not intended to have a lower level. Its existing single-level layout is correct;
the earlier lower-floor concern is closed and no elevator or interior change is needed.

**Housing follow-up, 2026-09-23:** the user confirmed merchant tents are working
and that no Singing Mountain huts existed before the terminal correction. The
tent-placement concern and older-hut migration note are closed; no footprint
change or existing-hut repair is needed for those reports.

### NPC placement and snapshots

- Corrected a Mustafar rock's snapshot object ID that conflicted with the Keren
  cantina on Naboo. This addressed the source of invalid interior references
  implicated in the NPCs appearing at the world origin.
- Guarded quest-giver and bartender spawning/patrol assignment against missing,
  invalid, and wrong-planet interior cells. Naboo's snapshot and the intended
  quest coordinates were preserved.
- Included the user's Tatooine snapshot correction replacing the wind generator
  near `-1636, -3287` with its decorative alternative.
- Established the tracked `mtg_patch_024` asset directory, replacing the earlier
  `newtre` workflow. Bundled the corrected snapshots, localization, house assets,
  and existing map/travel UI and compatibility assets.

**Validation:** standalone Lua, asset, localization, snapshot, and spawn checks.
**Confirmed on 2026-09-23:** the user reported that the misplaced Naboo NPCs
are fixed. This correction is no longer awaiting an in-game check.

### Local tests

- Removed `src/tests` from Git tracking and kept it ignored, as requested.
- Added guards so checkouts without local tests can still configure and build.
  Local tests remain available for validation; the user handles Core3 builds.

## 2026-09-19 — skill surrender and administrator revocation

Committed in `37908980bf` — **Add confirmed skill surrender and admin revocation dialogs**.

- Added a selectable list of surrenderable skills to `/surrenderSkill`.
- Selecting a prerequisite skill also prepares removal of learned skills that
  depend on it, such as the boxes above Novice Entertainer.
- Added a confirmation showing the full removal list before making the change.
  Selection and confirmation lists use alphabetical display names; confirmation
  also identifies the affected player and returned skill points.
- Applied the same selection, dependency, and confirmation flow to `/revokeSkill`.
  An administrator with no target defaults to themselves; targeting a nonplayer
  produces an on-screen error.
- Both commands also accept a skill name directly before confirmation. Learned
  dependent skills are removed first, including dependencies across professions.
- Added checks for permissions, target availability, and current skills before
  applying the confirmed plan. Changed skill lists require a fresh confirmation. Protected
  progression and Jedi/Village rules remain enforced; an unexpected failure
  stops the operation and reports any removals already completed.

**Confirmed:** the user reported the skill workflows working before requesting
the commit. Supporting dependency, display, and Jedi-skill checks were added.

## 2026-09-17–18 — shutdown, recovery, ownership, and compatibility

### Shutdown and session reporting

- Improved ground/space zone cleanup and its progress/timing reports while
  preserving object cleanup and the final-save process. Larger zones start first,
  ground zones still clear before space zones, and unnecessary repeated
  nearby-object scans are avoided.
- Improved shutdown disconnection coverage for multiple sessions, including
  accounts sharing an IP address.
- Corrected player, account, and distinct-IP reporting around login and shutdown
  so the log reflects sessions more consistently. Shutdown takes a stable list
  of sessions, rejects late character logins, and reports remaining sessions
  instead of always claiming success.

**User results:** one shutdown took about 31 seconds after the changes; this was
an observation, not a controlled performance benchmark. The user also reported
all clients disconnecting before the later cosmetic count correction.

### Recovery and structure ownership

- Added recovery for vehicles left in the world without usable control after an
  improper shutdown. Eligible vehicles are reconciled with their control device
  and returned to a stored state without recreating them, preserving identity,
  damage, paint, and rental uses. Occupied or ambiguous cases are excluded;
  stale decay/database tasks are guarded. Recovery messages go to the log file.
- Added `/server orphanstructures [all|planet] [page]` for administrators of
  level 15 or higher. It scans loaded ground planets and lists suspect ownership,
  planet, coordinates, name, and template, with 50 results per page. Selecting a
  result creates a waypoint; the report does not delete structures.
- Offline owners remain valid. World, temporary, civic, and base-defense objects
  are excluded from the report so they are not mistaken for abandoned player housing.
- Corrected maintenance ownership handling so persistent player structures with
  missing owners still receive the appropriate orphan-maintenance processing.
  Avoided unnecessary credit lookups for structures that do not require a player owner.
- Restored the original installation initialization behavior after the user
  replaced the affected wind generator with a decorative snapshot object.

**Confirmed:** the user reported vehicle recovery working. Orphan reporting is
an inspection aid; it does not establish that every ownerless world object is
an abandoned player structure.

### Terrain, build configuration, and screenplay startup

- Added support for newer terrain boundary versions encountered when enabling
  `moraband` in `Core3.ZonesEnabled`, including polygon versions 0006/0007 and
  rectangle version 0004.
- Raised the minimum CMake version to 3.10, used Boost package configuration,
  and scoped bundled-dependency warning handling in the top-level project file.
- Disabled automatic startup of the custom tutorial screenplays and blank
  screenplay example in `c4584b64e2`. The scripts remain present.

Related commits: `2d60053f7f` (cleanup and vehicle recovery), `4af01c80a0`
(shutdown reporting, terrain, and CMake), and `45de0bd1b4` (orphan structure
reporting and maintenance safeguards).

## 2026-09-15–16 — baseline, asset loading, and configuration

### Baseline and local files

- Realigned selected MTGServer settings with Core3 defaults in `34e4b6b995`:
  resource deed quantity 30,000, resource stack limit 100,000, and structure-component
  factory-crate quantities restored to their authored values of 1–15.
- Re-enabled the Mos Eisley city/no-spawn region and Safety Measures camp
  navigation meshes; corrected small Tatooine-house elevator terminals and
  selected personal-shield and Force-sensitive quest-item settings.
- Created local configuration separately from the distributed defaults and
  added ignore rules for it and generated resource-spawn data. The later
  correction to resource-spawn tracking is recorded under current work.
- Consolidated working guidance in the root `AGENTS.md`, including allowed
  folders, explicit commit requests, and leaving Core3 builds/runs to the user.
- Normalized formatting throughout `bin` and `src` in `6250299076` and
  `3c715608ae`; those changes were whitespace and layout cleanup.

### Appearance, hair, and template compatibility

- Fixed legacy appearance and mesh parsing, including additional bounding-box,
  vertex-buffer, and 16/32-bit index formats. Added validation for malformed
  mesh data and safer collision/navigation use of missing appearances.
  This addressed the default appearance/egg loading investigation
  (`323bf3b7c3`).
- Corrected hairstyle lookup when more than one player template shares a hair
  asset, including Human/Chiss style 11 compatibility (`600e92adf3`).
- Corrected resource-container and battlefield-station inheritance parsing for
  the reported RCCT/SSHP template warnings, retaining inherited data and Lua
  runtime overrides. Also corrected an airspeeder control-device definition
  and registered a missing generic lightsaber base (`d6614688f8`).

The separate character-selection object-broker error and missing ship-chassis
datatable files were investigated. This history does not claim a local code
fix or replacement asset for either where none was implemented.

### Configuration audit

Committed in `9a1491ab28` — **Expand and organize Core3 configuration settings**.

- Audited settings read by source/Lua code against `config.lua`, exposed 94
  additional scalar settings, removed eight unused settings, and grouped the
  result by subsystem. Existing retained scalar values were preserved.
- Covered logging and sessions, zone threads, player creation, PvP and groups,
  missions, maintenance, loot, REST/API, and transaction behavior.
- Corrected `DestoryOrphans` to `Core3.Tweaks.StructureObject.DestroyOrphans`
  and replaced `DeleteCharacters` with `PurgeDeletedCharacters`.
- Connected `Core3.PlayerManager.accountVictimList` to its consumer while
  retaining the legacy unprefixed fallback.
- Updated `config-local.lua` with the new organization and settings while
  preserving local customizations.

## Latest testing results and deferred content

- On 2026-09-24, the user confirmed `/server travelstatus`, the startup countdown
  and announcement, corrected Nayli arrivals, entering/leaving region messages,
  town markers, planetary maps, and weather work. The latest boot had no reported
  errors. They subsequently confirmed Mustafar map alignment and shadows, plus
  Kashyyyk Main and Dead Forest maps; the unusual combined atlas is accepted.
- Mensix travel in/out was confirmed before its relocation; the latest
  provisional port site, boundary and announcement-audience change await testing.
  Complete port/fare/return-trip testing and Frostline's individual services and
  placement have not yet been confirmed. C++ unit-test execution has not been
  reported; working game features do not imply those tests ran. The user's
  console output confirmed the sprite diagnostic and identified the valid
  `appearance/ui_destroy.spr` UI icon; that investigation is closed.
- The subsequent planet-name string correction needs server/client TRE deployment
  and a label retest. It does not reopen the confirmed map-rendering fixes.
- On 2026-09-23, the user confirmed the template-display labels work and reported
  that all remaining active vehicles appear to work correctly. Their initial
  testing items are closed.
- The regular AT-RT, unfinished AT-AT, and six colored racers retain disabled
  deed registrations; their assets can be revisited later. The light-bending
  deeds, disabled duplicates, and Generic Medium Window Style 3 remain inactive.

## Ongoing history maintenance

Both update files are maintained with meaningful project changes and new user
test results, as required by the root `AGENTS.md`. Refinements update the related
entry, while distinct changes receive dated entries. Pending work stays labeled
until verification or deployment is actually reported. Documentation updates
do not authorize builds, runs, or commits.
