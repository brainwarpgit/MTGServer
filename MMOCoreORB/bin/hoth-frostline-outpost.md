# Hoth — Frostline Outpost

Added 2026-09-23. **User confirmed successful startup after packaging the city-name
correction. Town refinement and individual service checks remain.**

The town is centered on **671, 2035**, at ground height **0**. It uses modular
outpost buildings, a central service area, and blue streetlights. Existing Hoth
snapshot content remains in place, including the southern outpost and ice caves.

[View the site plan](hoth-frostline-layout.svg). The exact snapshot templates,
positions, headings, and permanent IDs are in
[the placement manifest](hoth-frostline-outpost.json).

## Buildings and services

Coordinates below are world **x, y**; all building origins have height 0.

| Location | Coordinates | Services |
| --- | --- | --- |
| Frostline Starport | 671, 2170 | Ticket terminal inside its one-room office, ticket collector, scheduled transport; local and interplanetary travel |
| Cloning clinic | 595, 2035 | Standard cloning, imprint terminal, insurance, medical building bonus |
| Bank | 747, 2035 | Bank terminal inside the main hall |
| Cantina | 671, 1950 | Entertainer building bonuses, two ambient entertainers |
| Frostline Shuttleport | 860, 1925 | Ticket terminal, collector, scheduled shuttle; local travel |
| Vehicle garage | 820, 2035 | Standard vehicle garage/repair component |
| Workshop | 595, 2110 | Four public crafting stations outside, to the east |
| Survey team quarters | 747, 2110 | Enterable outpost accommodation |
| Expedition quarters | 595, 1950 | Enterable outpost accommodation |
| Bazaar | 700, 2035 | Bazaar terminal with a permanent snapshot ID |

Artisan, Brawler, Marksman, Medic, Scout, and Entertainer trainers stand around
the central area. Pilots, a technician, a scientist, and the cantina entertainers
provide ambient population. The town NPCs are stationary and noncombatant.

The starport office entrance faces west; its ticket terminal is inside. The
clinic and western quarters face east, the bank and eastern quarters face west,
and the cantina faces north. The central waypoint remains open ground.

The settlement is a world town, not a set of player-owned structures. Its city
region prevents player building placement and random encounter spawns. A
navigation area is configured for Frostline. Terrain height 0 was checked at
the planned sites; ground farther east than x900 or south of y1900 starts
leaving the fully flattened area.

Frostline's city circle is centered at **671, 2035**, with a **260m radius**.
Its server definition is in `scripts/managers/planet/hoth_regions.lua`.
The matching client notification circle is included in
`datatables/clientregion/hoth.iff`; it uses the existing **Frostline Outpost**
string to show entering/leaving messages. The southern client region retains its
250m radius. The 2026-09-24 region audit renamed its label to **South Range Outpost**
and matched the server circle to that radius; see [region coverage](planet-regions.md).

## Travel

| Destination name | Arrival coordinates | Interplanetary |
| --- | --- | --- |
| Frostline Starport | 671, 2135 | Yes |
| Frostline Shuttleport | 840, 1925 | No |
| South Range Outpost | 0, -1965 | Yes |

South Range uses the original starport at **0, -2000**. Its original cloning
building and other snapshot objects are preserved.

- All three stops support travel within Hoth; the Hoth base fare is **100 credits**.
- The two starports have **500-credit base fares** to the other planets in the
  expanded 19-planet travel table. Available destinations still depend on
  enabled planets, their registered ports, and normal travel restrictions.
- The later [travel update](planet-travel.md) standardizes fares across all
  19 main planets: **500 credits** between planets and **100 credits** locally.
- Hoth has a galaxy-selector entry and destination map in the ticket window.
- The ticket window's Planet, Location, Travel, and Show Help labels use existing
  client strings, correcting missing or malformed string references.
- This configures scheduled ground-port travel. It does not invent a Hoth space
  zone or a personal-ship launch destination.

## Planetary map

The planetary-map window now selects the existing authored Hoth terrain image
(`texture/ui_map_hoth.dds`) instead of falling back to Dathomir. The ticket map
uses the same Hoth image. No terrain reshaping or substitute map painting was
needed. Normal server map categories supply the town/service markers.

## Deployment

1. Package the current `mtg_patch_024` directory, retaining the previously
   working patch files. This change adds/updates these archive-relative files:
   - `snapshot/hoth.ws`
   - `datatables/travel/travel.iff`
   - `datatables/clientregion/hoth.iff`
   - `ui/ui_planet_map.inc`
   - `ui/ui_ticketpurchase.inc`
   - `string/en/hoth_region_names.stf`
2. Install the resulting patch for **both server and client**, with precedence
   over older versions of these files. Keep the current planet archives, which
   contain Hoth's terrain, map texture, and planet selector appearance.
3. Use the updated `bin/scripts` files and restart the server and client.
   Hoth is already enabled in the current configurations. This change contains
   no C++ changes and does not require a Core3 rebuild.
   The later all-planet travel update includes a separate ticket-command fix
   that **does require a Core3 rebuild**; see [travel deployment](planet-travel.md).
4. Allow the server to generate Frostline's navigation data if needed during
   startup. The screenplay reports missing/mismatched snapshot IDs and skips
   its spawns if the matching server snapshot was not installed.

Snapshot buildings and cells use reserved IDs beginning at `1900100000`.
**Keep bazaar ID `1900100900` stable** when editing this town: auction listings
retain their terminal ID across restarts. The bank, public crafting stations,
and NPCs are recreated by the screenplay on startup.

## Validation and first in-game check

### Region notifications and ticket labels — 2026-09-23

The user reported that southern-outpost entering/leaving messages worked but
Frostline's did not. Frostline already had its server city region; NPC-city
notifications come from a separate client table, which lacked Frostline.
Added the matching client row and preserved the original southern row exactly.
The later 2026-09-24 audit aligned the southern label and server radius as described above.
Standalone checks verified the real Lua geometry, localized name, region
boundaries, and lack of overlapping client regions. Repackage the client patch
and cross Frostline's 260m boundary to verify both messages in game.

Also corrected missing ticket labels by reusing existing `ui.stf` strings.
All 41 ticket-UI string references now resolve in the configured assets, with
unrelated UI content preserved. These corrections require no Core3 rebuild.

### Startup correction — 2026-09-23

The first packaged run loaded all 153 snapshot objects, then stopped during
city navigation setup. The original two city-region definitions used plain
text names, but Core3's city loader requires `@table:key` string references.
The empty parsed names caused an exception and left startup waiting for Hoth.

Both region names now use valid `@hoth_region_names` references. The patch
adds their labels to the existing Hoth string table while preserving its
original entries. Include this STF when rebuilding the patch; the Lua
correction is in `scripts/managers/planet/hoth_regions.lua`. No C++ rebuild is
needed. Regression checks now reject plain/empty city names and require the
referenced labels to exist.

The user subsequently confirmed that the corrected server loaded successfully.
The town remains a first pass for further layout and gameplay refinement.

Completed without building or running Core3:

- Preserved all 94 original snapshot nodes byte-for-byte; added 59 nodes.
- Checked new IDs against every other configured snapshot, all 37 new interior
  cells, seven portal-layout checksums, entrances, and nonoverlapping building
  footprints on flat terrain.
- Evaluated 38 production object definitions and resolved 1,142 associated
  client dependencies, including template-index entries and service children.
- Checked cloning spawn cells, bank/entertainer floor placement, cantina bonuses,
  garage components, trainer registrations, and screenplay failure guards.
- Evaluated planet/region Lua; checked all shuttle-startup orders and nearest
  travel-point bindings; preserved all 169 previous fare cells and prior UI data.
- Verified Hoth map/selector assets, UI structure, dimensions, and world scale.

Still requires the installed client and server:

1. Visit the center and enter the starport office, clinic, bank, cantina, and
   quarters. Check doors, floors, lights, and client/server collision alignment.
2. Buy and use tickets between all three Hoth stops, then travel to another
   planet and back. Confirm Hoth's caption and selection in the ticket window.
3. Bind at the cloning terminal and verify cloning into the clinic.
4. Try the bank, a public crafting station, each trainer conversation, and the
   garage with a damaged vehicle.
5. List/retrieve a bazaar item and confirm its listing remains after a normal
   restart. Check the planetary map image and service markers.

No Core3 build/run or Git commit was performed for this change.
