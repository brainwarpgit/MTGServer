# Custom planet regions

Updated: 2026-09-25. Changes committed on 2026-09-25. The user confirmed that region
entering/leaving messages and town markers work, closing those initial runtime checks.

## Coverage

Audited all 17 enabled custom ground scenes. The nine custom travel worlds now
have matching server and client circles for 20 towns and one named district.
All 24 configured travel arrival squares fit their respective regions.

| Scene | Region | Center (x, y) | Radius |
| --- | --- | --- | ---: |
| `chandrila` | Hanna City | 294, -2938 | 800m |
| `chandrila` | Nayli Outpost | -5255, 292 | 400m |
| `coruscant` | Entertainment District | 2248, -4462 | 300m |
| `coruscant` | Monument Square | 1566, 662 | 300m |
| `coruscant` | Spaceport District | -114, 3227 | 300m |
| `coruscant` | CoCo District | -1918, -134 | 300m |
| `coruscant` | Palace District (name only) | -472, 6679 | 400m |
| `hoth` | Frostline Outpost | 671, 2035 | 260m |
| `hoth` | South Range Outpost | 0, -2000 | 250m |
| `kaas` | an Imperial Garrison | -5117, -2291 | 200m |
| `kashyyyk_main` | Kachirho Starport | -678, -160 | 100m |
| `mandalore` | Keldabe | 1589, -6385 | 500m |
| `mandalore` | Sundari | 6306, -6231 | 500m |
| `mandalore` | Norg Bral | -6621, 5507 | 450m |
| `mandalore` | Bralsin | -5683, -5033 | 175m |
| `mandalore` | Enceri | 4709, 7155 | 125m |
| `mandalore` | Shuror | 1069, 2736 | 125m |
| `moraband` | Moraband Outpost | -1715, -641 | 125m |
| `mustafar` | Mensix Mining Facility | -1100, 2365 | 400m |
| `taanab` | Pandath | 2084, 5395 | 500m |
| `taanab` | Starhunter Station | 3673, -5425 | 250m |

The latest provisional Mensix relocation requires a fresh region/marker retest;
other user-confirmed town checks remain recorded. Its port is at -1100, 2400,
with arrivals and both town centers at -1100, 2365.

## Repairs

- Added missing server town definitions on Chandrila, Coruscant, Kaas,
  Kashyyyk Main, Mandalore, Moraband, and Taanab. Corrected the unused
  `tanaab_regions.lua` spelling to the loader-required `taanab_regions.lua`.
- Restored Coruscant district labels and the Mensix label; corrected Kachirho's
  string-table reference. City names use complete, resolved `@table:key` references.
- Moved Kachirho's client circle from the world origin to its existing starport
  compound at -678, -160. Moved Mensix's client circle to its configured port
  and server region, most recently moved together to -1100, 2365 when the user
  requested a provisional flat site. The moved region awaits a fresh retest.
- Added a 125m Moraband Outpost circle around the existing military compound.
  It excludes the nearby temple ruins.
- Expanded Norg Bral from 375m to 450m to include its cloning, guild, and guard
  buildings; expanded Bralsin from 125m to 175m to include its western building.
- Unified Hoth's southern region under “South Range Outpost,” retaining its
  authored 250m client radius and matching the server circle to it.
- Retained Coruscant's authored Palace District as a named region. No snapshot
  settlement or travel point was found there, so it does not gain city navigation
  or construction/spawn restrictions.

## Server behavior

Town regions use `CITY + NOSPAWNAREA + NOBUILDZONEAREA`: they appear as NPC
cities and protect their settled areas from random spawns and new player construction.
Existing city logic also excludes city halls near NPC cities (at least 1,024m,
or twice the city radius when larger). These definitions do not remove existing structures.
Core3 automatically creates navigation areas for CITY regions. Initial startup may
need to generate the additional city navigation data.

The existing combined `kashyyyk` scene remains unconfigured and its snapshot remains
inactive. Its authored client Kachirho label was checked and preserved. Hunting
Grounds, Dead Forest, Rryatt Trail, north/south/POB dungeons, and `dungeon2` have no
authored client-region tables in the available archives. Their empty server region
definitions are retained; `dungeon2` now has the explicit empty table its loader expects.
No speculative settlements, random spawn groups, or dungeon-wide city restrictions
were added.

## Deployment and regression reference

Retain these steps for future deployments and region changes. Initial region
notifications and town markers were confirmed working on 2026-09-24.

1. Deploy the updated `bin/scripts/managers/planet/*_regions.lua` files, including
   the new files. Remove the obsolete misspelled `tanaab_regions.lua` if copied separately.
2. Repackage `mtg_patch_024` and deploy the matching TRE to server and clients.
   This includes five `datatables/clientregion` files and the three new string tables
   (`coruscant_region_names.stf`, `moraband_region_names.stf`, and `mustafar.stf`).
3. Restart the server and client. **These region changes require no Core3 rebuild.**
4. Travel to the listed ports, check city markers, then cross a region boundary
   to verify entering/leaving labels. Check the expanded Norg Bral and Bralsin edges.

Standalone checks passed: all 29 enabled ground-scene Lua files load with the exact
global names the server expects; client/server circles agree; all region labels
resolve; circles are valid, within terrain bounds, and non-overlapping; 336 boundary
samples and all 24 travel landing squares pass. Snapshot checks confirm the repaired
compound locations and the Mandalore buildings brought inside their town regions.
Binary checks verify the intended field edits and preservation of unrelated rows/data.
Existing Hoth and custom travel checks also pass. Core3 was not built or run.

This audit verifies horizontal region coverage. Arrival heights require separate
terrain/collision checks. The user confirmed on 2026-09-24 that Nayli's corrected
arrival no longer leaves the player stuck; other port-height checks are separate.
