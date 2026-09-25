# Enabled-planet ticket travel

Updated 2026-09-25. Changes committed on 2026-09-25. The user confirmed the startup
countdown, announcement, and `/server travelstatus` work, and reported no errors
on the latest boot. Nayli arrivals no longer leave the player stuck.
Mensix travel in and out also works, as confirmed by the user. Its subsequent
placement correction needs a retest; other individual port, return-trip, and
fare checks remain.

## Planet-name labels

The later report of `planet_n:[chandrila]` revealed missing localized names.
Prepared two complete replacement tables in `mtg_patch_024`:

- `string/en/planet_n.stf`: adds Chandrila, Coruscant, and Moraband for ticket
  attributes, planet lists, and other references using `@planet_n:<scene>`.
- `string/en/zone_n.stf`: adds eight missing scene labels for planetary-map titles.

All 39 configured ground/space scene names now resolve in both tables; existing
labels and numeric string IDs are preserved. Repackage these files in the TRE
used by server and clients, restart both, and recheck the names. No Core3 rebuild
is needed. This label correction is committed and awaits in-game verification;
the previously confirmed startup, Nayli, and map-rendering results still stand.

## Fares and destinations

- One-way travel between planets: **500 credits**.
- One-way travel within the same planet: **100 credits**.
- Round trips purchase two tickets: **1,000 / 200 credits** before player-city taxes.
- Existing player-city taxes and travel coupons retain their normal behavior.

`datatables/travel/travel.iff` now contains all 19 enabled main planets, with
direct fares between every pair. Kashyyyk uses the existing **`kashyyyk_main`**
scene and **Kachirho Starport**. Other Kashyyyk regions, tutorial, and generic
dungeon scenes are not separate interplanetary destinations.

The ticket window contains matching controls and destination pages for all 19
planets. Moraband and Kashyyyk use neutral destination-map backgrounds because
their terrain images are missing or not yet aligned. Destination selectors and
travel-point markers remain available without a terrain image.

## Newly connected existing ports

| Planet | Travel points |
| --- | --- |
| Chandrila | Hanna City Starport; Nayli Outpost |
| Coruscant | Coruscant Starport; Spaceport District, Monument Square, CoCo District, and Entertainment District shuttleports |
| Kaas | Kaas Imperial Garrison |
| Mandalore | Keldabe Starport, Outpost, and Shuttleport; Sundari Starport; Bralsin, Norg Bral, Enceri, and Shuror shuttleports |
| Moraband | Moraband Outpost |
| Taanab | Pandath Starport; Starhunter Station |

These 19 points use existing buildings in the authored snapshots. Arrivals use
working stock port offsets transformed to those buildings' positions and
orientations. Starports/outposts support interplanetary travel; shuttleports
remain local connections. Existing restricted destinations keep their restrictions.

Nayli Outpost on Chandrila has a terrain-supported arrival exception. Its
arrival apron is marked non-solid, so using the building's **18.9** height
placed players about **0.81m above the ground**. The arrival height is now
**18.09**, with the same **-5268.64, 263.44** coordinates and a reduced **1m**
landing range. The source-derived terrain check covers all 441 height-cache
positions in that landing square, with heights **18.045–18.128**. This Lua-only
correction needs a server restart; no TRE change or Core3 rebuild is needed.
The user confirmed on 2026-09-24 that Nayli arrivals no longer leave the player stuck.

Adding the six named planet configurations also loads their previously skipped
world snapshots. Mandalore reused 393 Dantooine object IDs and Taanab reused six
Corellia IDs. Corrected those IDs and associated parent references in the patch
snapshots, preserving all 1,922 objects' templates, positions, rotations, and
hierarchies. The original Dantooine/Corellia objects and screenplay references
remain unchanged. Enabled snapshots have no remaining cross-planet ID collisions.

## Mustafar arrival and return port

The user confirmed that Mensix travel in and out works, but still found its
physical placement unsuitable after the map was aligned. At the user's request,
the existing port has moved to a **provisional flat site at -1100, 2400**.
Arrivals now use **-1100, 2365**, just south of the building, with the same 3m range.
The base height is **37.696545** and arrival height **37.843140**. These coordinates
supersede the previous -1760, 100 site; the user will research a final location.

A standalone terrain survey checked 2,704 points across the building footprint,
entrance, and arrival apron. All sampled heights are equal, with at least 37.84m
of clearance above detected water/lava surfaces, outside road-height influence
and potentially active impassable boundaries. This is a temporary plateau,
about 1.6km from the nearest authored road centerline. Client collision and
walking access still need an in-game check; the source-derived survey does not
establish runtime navigation or identify the canonical town location.

The existing port/cell IDs, portal layout, child services, travel destination
name, route permissions, and fares are preserved. Only the root translation,
arrival point, and matching server/client town centers changed. The 400m Mensix
region now centers on **-1100, 2365**. The original snapshot rock is unchanged.

Package `snapshot/mustafar.ws`, `datatables/clientregion/mustafar.iff`, and the
Mustafar map files (including the empty metadata overrides) listed in
[map coverage](planet-map-coverage.md), then
restart server and client with the updated Lua files. Recheck the entrance,
arrival height, town marker, return travel, and map-created waypoints. These
placement changes need no Core3 build and remain pending user testing.

## Mustafar map boundary

`Core3.MustafarMapBounds = true` is now enabled in both `config.lua` and
`config-local.lua`, preserving the other local settings. It confines all players,
**including administrators**, to the map artwork's world rectangle:
**X -6880 through 1120, Y -1024 through 6976**. Other planets are unaffected.

The server checks ordinary and mounted movement, cell movement, teleports,
zone transfers and saved-login insertion. A crossing stops movement and returns
the player to a valid inside position. A character already outside, or arriving
from another planet at an outside destination, returns to the **current configured
Mensix arrival point**. Future port relocations therefore update recovery too.
The guard does not guess terrain height or clamp characters onto unknown ground.
If the configured recovery point is unavailable or outside, it rejects the
outside destination and displays an error; an explicit inside teleport still works.

This server change requires the user's Core3 build and restart. Set the option
to `false` and restart to disable the restriction. Standalone checks cover
2,058 boundary cases and the source integration paths; independent source review
found no blocking issue. Core3 compilation and in-game tests remain pending:
cross each edge on foot and mounted as an admin and regular player, try an outside
teleport/saved login, and confirm valid interiors and other planets still work.

## Server command correction

`PurchaseTicketCommand` now preserves underscores in planet scene identifiers.
The client sends `kashyyyk_main` unchanged, but the previous server code changed
it to `kashyyyk main` and could not find the zone. Location names still decode
their underscores as spaces, as required by the ticket command format.

## Shuttle startup notices

Opening a ticket terminal, purchasing a ticket, or using a ticket during startup
now shows the remaining initial delay. The same check covers ticket collectors,
ticket-selection dialogs, and travel-coupon confirmations. Blocked requests do
not charge credits or consume tickets/coupons.

`Core3.ShuttleZoneComponent.BootDelay` retains its existing value and meaning:
five minutes by default, measured from server startup rather than player login.
The initial landings follow registration, so reaching the five-minute mark alone
does not mean boarding is ready. Messages distinguish the boot countdown,
estimated landing time, and initialization that is still finishing.

Once the initial shuttles have completed boarding preparation, one galaxy message
announces that shuttle and starport services are available and normal schedules
apply. Ordinary players receive only this service-status message. If startup
records issues, online administrators at **level 15+** receive a separate summary
and `/server travelstatus` hint; the permission matches that diagnostic command.
Issue details remain in the server log. If no shuttle reached initial boarding,
the public message says service availability could not be confirmed, rather than
incorrectly announcing working services; admins receive the diagnostic summary.
Issues do not hold up working ports indefinitely, and a failed startup record
does not necessarily mean its associated port is unavailable. Later shuttles and
admin lock/unlock actions do not repeat the announcement. Schedules are unchanged.

This audience split awaits the user's next build/restart and a test with an
ordinary player and administrator online together. Source checks cover nine
privacy/permission/locking mutations; existing timing, report and integration
checks pass. No Core3 compilation or execution was performed by Codex.

### Finding startup issues

Administrators can view the retained report without visiting each planet:

```text
/server travelstatus
/server travelstatus issues naboo
/server travelstatus all
/server travelstatus all kashyyyk_main 2
```

Syntax: `/server travelstatus [issues|all] [planet|all] [page]`.
The default lists issues and pending startup records across all planets. `all`
mode includes successful records and intentionally discarded duplicate shuttles.
Use scene names for the planet filter. Pages start at 1 and contain 20 records.

Each row includes planet, port name if matched (or nearest/unmatched), world
X/Y, shuttle object ID, status, and reason. The summary counts ready, failed,
pending, and discarded records. Issue details also go to the server log under
`TravelStartupTask` as `Startup shuttle issue:` entries.

This is read-only **startup history for the current boot**, not a live check of
later port failures or current boarding windows. Ports that never created a
startup shuttle are not inventoried. Intentionally discarded duplicates are
listed separately and do not count as failures. Existing logs from the earlier
monitor do not contain enough information to identify its unnamed failures.

The user confirmed on 2026-09-24 that the countdown and announcement work and
`/server travelstatus` works; the user reported no errors on the latest boot.

For future deployments, these notices require a **Core3 build with IDL
regeneration and server restart**. They require no new TRE/client files.
Keep the following as a regression checklist:

1. Before the startup delay expires, open a travel terminal and attempt to use
   an existing ticket. Confirm a readable countdown and unchanged credits/tickets.
2. During initial landing, repeat the attempts and check the landing/startup notice.
3. Confirm one galaxy-wide announcement, then buy a ticket and board normally.
4. Verify a normal departure/arrival cycle does not repeat the startup broadcast.
5. Run `/server travelstatus` and `/server travelstatus all` as an administrator.
   Confirm location details for any issues and that discarded duplicates are
   separate from failures. Review matching `Startup shuttle issue:` log entries.

Standalone source and timing checks pass. Local C++ regression tests are prepared
but unexecuted. The user's confirmation closes the initial startup-notice and
report checks; it does not establish coverage of every purchase/coupon path or
concurrent task interleaving.

## Deployment and regression reference

Use these steps when deploying further changes. The confirmed results above
do not establish that every port and fare combination has been tested.

1. **Build Core3 with IDL regeneration** for the ticket-command correction and
   startup notices. Codex did not build or run it.
2. Repackage `mtg_patch_024`, including the travel table, both UI files, supplied
   map textures, corrected Mandalore/Taanab snapshots, and the Mustafar snapshot.
3. Install the same patch on server and clients, and use the updated
   `scripts/managers/planet/planet_manager.lua` on the server.
4. Restart server and clients. Check each new port's ticket terminal, collector,
   shuttle arrival, boarding, and return trip. Check one-way and round-trip prices.

Standalone checks cover every fare cell using server/client loading rules,
actual Lua configuration, snapshot IDs and cell links, port child templates,
shuttle startup ordering, all 19 UI bindings, string references, texture headers,
button overlap, Mustafar terrain/portal alignment, and preservation of unrelated
assets. The user also confirmed planetary maps and town markers work.
Individual destination controls, port boarding/return trips, and fare combinations
remain checks for a complete travel audit.

## Startup snapshot review — 2026-09-23, 19:06

The inspected September 23 startup reached `Core initialized` at 19:07:22. It logged
one snapshot object error: Mandalore building **8565664**, an outpost starport
at **6285, -6211**, could not find its interior cell when creating its ticket
terminal. The installed Mandalore snapshot gives that cell ID **8565838**,
which Dantooine also uses for a decorative bol skull. If the Dantooine object
loads first, the snapshot loader skips the cell because that ID already exists.
The prepared Mandalore snapshot already corrects the cell to **1900200199**,
preserving its parent and interior cell number. No further building or Lua
change is needed for this error. A focused standalone loader simulation
reproduces the failure with the installed snapshot and passes with the prepared
snapshot. The full snapshot-ID and hierarchy checks also pass.

At inspection, 199 of the 205 prepared patch files matched the configured TREs.
These six assets were still older versions in the installed archives:

- `snapshot/mandalore.ws`
- `snapshot/taanab.ws`
- `snapshot/mustafar.ws`
- `datatables/travel/travel.iff`
- `ui/ui_planet_map.inc`
- `ui/ui_ticketpurchase.inc`

Repackage the current patch folder, install it on server and clients, and restart.
The snapshot correction itself needs no C++ rebuild. The earlier ticket-command
correction still requires the build described above if it has not been built yet.

The same startup reported missing `kashyyyk_north_dungeons`,
`kashyyyk_south_dungeons`, `kashyyyk_rryatt_trail`, and `tutorial` snapshots.
None exists in the 81 available archives, including searches for alternate
snapshot/buildout names. Tutorial content is created dynamically, and its
building loaded successfully. The three Kashyyyk scenes have terrain but lack
authored snapshot objects; their warnings do not describe corrupt snapshots.
These assets remain absent. No empty replacement snapshots were created.
