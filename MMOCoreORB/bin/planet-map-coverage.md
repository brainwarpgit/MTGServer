# Enabled planetary map coverage

Updated 2026-09-25. Changes committed on 2026-09-25. The user confirmed the final
actual-client correction: Mustafar alignment, Kashyyyk Main and Kashyyyk Dead
Forest maps look good. The combined Kashyyyk atlas still looks unusual, which
the user accepts for now. No further map-layout change is made in this follow-up.
Map-click waypoint accuracy was not separately reported. The Mensix port/city
placement is a separate content change: the port now uses a provisional flat
site at -1100, 2400, with arrival/town center -1100, 2365, pending an in-game retest.
Missing artwork and original-art limitations remain documented below.

The subsequent planet-name report is a separate localization correction:
`mtg_patch_024/string/en/planet_n.stf` adds three missing planet names, and
`mtg_patch_024/string/en/zone_n.stf` adds eight missing map-title labels. All 39
configured scene keys and the 32 relevant map-page titles now resolve, while
existing names remain intact. Package these tables for server and client and
restart to verify the names. No Core3 rebuild or map-image change is required;
this new label retest remains pending.

## Prepared map corrections

| Planet | Change | Existing image |
| --- | --- | --- |
| Hoth | Added previously with Frostline Outpost | `texture/ui_map_hoth.dds` |
| Kaas | Added the missing planet page | `texture/ui_map_kaas.dds` |
| Mandalore | Added the missing planet page | `texture/ui_map_mandalore.dds` |
| Taanab | Replaced the incorrect Talus image reference | `texture/ui_map_taanab.dds` |
| Chandrila | Added a page for the user's supplied image | `texture/ui_map_chandrila.dds` |
| Coruscant | Added a page for the user's supplied city backdrop | `texture/ui_map_coruscant.dds` |
| Mustafar | Place the regional artwork in the native terrain frame; use manual zoom | `texture/ui_map_mustafar.dds` |
| Kashyyyk Main / Kachirho | Added a plain scene page using an approximate 2,016m crop centered at -16,100 | `texture/ui_map_kashyyyk_main.dds` |
| Kashyyyk Dead Forest | Added a plain scene page using an approximate 1,056m crop centered at 24,0 | `texture/ui_map_kashyyyk_dead_forest.dds` |
| Combined Kashyyyk | Added a partial atlas placing Kachirho, Dead Forest and Hunting Grounds in their world positions | `texture/ui_map_kashyyyk.dds` plus the two regional textures |
| Kashyyyk Hunting Grounds | Added a scene page using the image's centered 2,844m map range | `texture/ui_map_kashyyyk_hunting.dds` |

The original images are supplied by the configured archives (`mtg_planets.tre`,
`mtg_patch_015.tre`, and `mtg_patch_008_texture_04.tre`). The user subsequently
provided Chandrila, Coruscant, and a replacement Hoth texture in `mtg_patch_024`;
all three match the currently installed highest-priority TRE versions. Their
files are preserved unchanged. Each UI page uses the actual DDS dimensions.
The ten original planets already have corresponding map pages and images.
Coruscant's image is a city backdrop; street-by-street terrain alignment has
not been established.

### Client compatibility: verified against the supplied executable

The supplied **SWGEmu.exe** reads a map page's `MapWidth` and uses a direct,
origin-centered transform for both marker positions and map-click waypoints.
It does **not** read `datatables/planetary_map/map_adjustments.iff` or apply its
offsets. Earlier tests modeled a newer SWG-Source client and therefore missed
this compatibility problem. Their arithmetic was not a runtime compatibility test.
The installed TRE did contain the previous assets; this was not an identified
server-side packaging omission.

The current pages use only the supported width/height transform. Images carry
their actual geographic position inside that frame. The offset table is now a
**valid empty override**, clearing the earlier rows even if they remain in an
older TRE. Both buildout overrides remain empty too. No executable is modified
or run. Local inspection evidence is retained in the ignored `src/tests` folder.

### Kashyyyk: revised regional crops and partial combined coverage

The three plain pages select the existing images without buildout metadata.
Their original `scene__region` aliases and the conceptual **Show Planet** overview
remain unchanged.

| Scene | Map view, centered at world 0,0 | Artwork bounds X / Y |
| --- | --- | --- |
| `kashyyyk_main` | 2304 × 2304m | -1024…992 / -908…1108 |
| `kashyyyk_dead_forest` | 1152 × 1152m | -504…552 / -528…528 |
| `kashyyyk` | Full 16384 × 16384m terrain | Three regional images at the locations below |

Main's **2016m** artwork is centered at **-16,100** and fills **87.5%** of its map
view. Dead Forest's **1056m** artwork is centered at **24,0** and fills **91.67%**.
Their actual terrains are both 4096m wide. The slightly larger centered UI views
contain the off-center artwork without relying on unsupported coordinate offsets.
Revised crop sizes improve agreement with the height/path bitmaps and bridge
landmarks, but are still approximate; the original artwork contains local errors.

The combined `kashyyyk` image is byte-identical to Main's Kachirho image, not a
whole-world map. The combined page therefore remains a **partial atlas**:

- Kachirho: the same bounds as Main above.
- Dead Forest: its revised crop translated **-1500,+1500**, giving bounds
  **X -2004…-948, Y 972…2028**. Eighteen shared snapshot landmarks confirm the
  translation independently of the artwork calibration.
- Hunting Grounds: **2844m** wide, centered **0,-3458**, at
  **X -1422…1422, Y -4880…-2036**.

Small separated pictures and blank areas in the combined map are expected:
there is no aligned full-world texture in the supplied assets. Use manual zoom
for regional detail. Stretching the Kachirho image to fill that terrain would
misrepresent locations. Other dungeon regions remain unmapped.

### Mustafar: correct geography within the supported map frame

Mustafar's terrain is **16384 × 16384m**, like the standard planets. Its artwork
covers only **8000 × 8000m**, at **X -6880…1120, Y -1024…6976**, centered at
**-2880,2976**. World **0,0** belongs near the image's lower-right: **86% across,
87.2% down**. Centering it in this artwork would be geographically incorrect.

The new server setting `Core3.MustafarMapBounds` restricts everyone, including
admins, to this artwork rectangle. It is enabled in both config files and needs
a Core3 build/restart. See [travel and boundary details](planet-travel.md).

The attempted 8192m view required coordinate offsets that this client ignores.
It has been replaced with the native **16384m** frame. The image is placed at
**1312,1216**, with a size of **8000,8000**, in that reference frame. Player/port
markers and map-click waypoints now use the same direct coordinate model supported
by the inspected client. The ticket map already uses this placement and is unchanged.

**The image will still be smaller when fully zoomed out**—48.8% of the full map
width. Use the normal zoom slider and scrollbars. A large automatically centered
regional view cannot be supplied through the ignored offset table. Filling the
whole frame with this one crop would sacrifice geographic alignment. There is
no new terrain, port movement, texture stretch, or coordinate-system change.

### Deployment and checks for these corrections

Repackage these files from `mtg_patch_024` and fully restart the client:

- `ui/ui_planet_map.inc`
- `ui/ui_ticketpurchase.inc` (unchanged in this follow-up; retains the pending ticket fix)
- `datatables/planetary_map/map_adjustments.iff` (**empty**)
- `datatables/buildout/buildout_scenes.iff` (**empty**)
- `datatables/buildout/areas_mustafar.iff` (**empty**)

Also include the five corrected `terrain/colorramp/mustafar_*.tga` files for the
separate [shadow-format correction](planet-weather.md). No Core3 build is needed.
Keep the server/client patch versions synchronized for the other pending content.

Check each map's landmarks, player/port markers, resize/zoom behavior and a
map-click waypoint. Start near Main's Kachirho Port (**-678,-160**) and Dead
Forest's cave/bridges. On Mustafar, compare roads/lava, Mensix and world 0,0;
use manual zoom for the regional artwork. The port relocation test remains
separate. Hunting Grounds keeps its existing centered **-1422…1422** scene view.
The user confirmed the resulting Mustafar alignment and Main/Dead Forest maps.
Combined-map presentation is accepted for now. Keep map-click waypoints and
future artwork edits as regression checks; this confirmation does not validate
every original landmark or every zoom/window size.

## Maps that need additional work

- **Moraband:** no corresponding map image was found, including searches for
  Korriban. The terrain's old source-planet filename does not establish that
  the source planet's map matches this terrain. Chandrila and Coruscant are now
  supplied and connected, as listed above.
- **Combined Kashyyyk beyond the three mapped regions:** the new page has partial
  geographic coverage. Other separated dungeon areas still need independently
  aligned images; the conceptual overview is not a coordinate-bearing terrain map.
- **Rryatt Trail and north/south Kashyyyk dungeons:** the available pictures
  cover individual areas; the terrain files also contain repeated or separated
  areas. One full-scene stretch would misplace player and waypoint markers.
- **Tutorial, generic dungeon zones, and Kashyyyk POB dungeons:** no suitable
  full planetary map was identified.

Among the 19 main planets, **Moraband and Kashyyyk** remain without complete
map coverage. Kashyyyk now has Main, Dead Forest and Hunting Grounds scene
pages plus a partial combined atlas. The new Main/Dead Forest crop calibration
has user-confirmed presentation/alignment; Rryatt Trail and dungeon scenes still
need the work above.
The ticket UI uses neutral backgrounds for Moraband and Kachirho, with travel controls available; these are not terrain maps.

## Installation and verification

### Where map images are stored

The archive-relative location is **`texture/ui_map_<planet>.dds`**, for example
`texture/ui_map_hoth.dds`. Most original/custom whole-planet images are in
`mtg_planets.tre`; the current Mustafar image is in `mtg_patch_015.tre`, and the
Kashyyyk regional images are in `mtg_patch_008_texture_04.tre`. These archives
are under `/home/swgemu/workspace/tre`.

For a future Moraband map, use the project's writable patch directory:

```text
/home/swgemu/workspace/MTGServer/mtg_patch_024/texture/ui_map_moraband.dds
```

An image alone also needs a matching page in **`ui/ui_planet_map.inc`**.
That page selects the texture and its pixel dimensions. Current maps commonly
use 1024 by 1024 or 4096 by 4096 DDS images. Prefer an image that represents the
actual terrain with north at the top; preserve any known coordinate bounds if
it is a cropped region. Moraband has a 16,384m-wide terrain, so a whole-planet
image would cover -8192 to 8192 on both horizontal axes. Kashyyyk regional maps
need their individual coordinate bounds rather than this whole-planet range.

### Deployment and regression reference

Retain these steps for future map changes. Earlier pages have user-confirmed
results, including the final Mustafar/Main/Dead Forest correction. Retain these
checks for later artwork, client or layout changes.

Repackage `mtg_patch_024` and update the client's TRE before starting the client.
No Core3 compilation is needed for these UI changes. Keep the server/client
patch versions synchronized when packaging the other changes in this directory.

Standalone checks cover the actual executable's map math, UI parsing, unique
planet names, texture availability, DDS payloads, terrain dimensions, and exact
preservation of unrelated UI content. Display, zooming, and player/waypoint alignment remain useful
regression checks when map images or UI bounds change. The user's confirmation
does not supply missing map images or establish the approximate Kashyyyk crops
or Coruscant street alignment described above. No Core3 build or run was performed
for this map audit.
