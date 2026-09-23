# Additional vehicle deeds

Initial audit date: 2026-09-22. Status updated: 2026-09-23.

## Findings

The initial `object/intangible/vehicle` audit found 76 control-device
templates: 47 represented by the earlier vehicle audit, 12 additional vehicle
models, six aliases, eight incomplete entries, two retired light-bending
vehicles, and one base template. The black Swoop Racer has since received its
missing vehicle and appearance setup. The other seven incomplete entries were
disabled on 2026-09-23 and deferred for later work at the user's request.

Eleven of the additional models already have registered TCG deeds. Those deeds
are reused. The USV-5 Modified Landspeeder now has a new shared deed asset, Lua
template, and registration. All 12 deeds have matching vehicle previews, clean
names, and original Star Wars descriptions.

On 2026-09-23, the user reported that the active vehicles work in game and
confirmed that the corrected template-inspection labels display properly.

## Spawn commands

Create fresh deeds as an administrator after installing the updated patch and
Lua files. These paths are already registered; no new loot or vendor entries
are included.

| Vehicle | Create command |
| --- | --- |
| Air-2 Racing Swoop | `/object createitem object/tangible/tcg/series5/vehicle_deed_air2_swoop_speeder.iff` |
| BT310 Podracer | `/object createitem object/tangible/tcg/series4/vehicle_deed_balta_podracer.iff` |
| FG 8T8-Twin Block2 Special Podracer | `/object createitem object/tangible/tcg/series5/vehicle_deed_fg_8t8_podracer.iff` |
| Geonosian Speeder | `/object createitem object/tangible/tcg/series4/vehicle_deed_geonosian_speeder.iff` |
| HK-47 Jetpack | `/object createitem object/tangible/tcg/series6/deed_tcg_hk47_jetpack.iff` |
| IPG-X1131 Longtail Podracer | `/object createitem object/tangible/tcg/series3/vehicle_deed_podracer_longtail.iff` |
| Koro-2 Exodrive Airspeeder | `/object createitem object/tangible/tcg/series3/vehicle_deed_koro2_exodrive_airspeeder.iff` |
| Merr-Sonn JT-12 Jetpack | `/object createitem object/tangible/tcg/series3/deed_tcg_merr_sonn_jt12_jetpack.iff` |
| Republic Gunship | `/object createitem object/tangible/tcg/series7/combine_reward_deed_republic_gunship.iff` |
| Senate Pod | `/object createitem object/tangible/tcg/series4/vehicle_deed_senate_pod.iff` |
| USV-5 Modified Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_usv5_s02_deed.iff` |
| XJ-6 Airspeeder | `/object createitem object/tangible/tcg/series5/vehicle_deed_xj6_air_speeder.iff` |

## Client asset repairs

- Restored the modified USV-5 body LOD using its four existing variant meshes.
  Preserved the original USV-5 distances and rider hardpoints, and checked the
  actual variant geometry against the parent bounds.
- Corrected the modified USV-5 proxy mesh skeleton reference.
- Added missing mount mappings for the modified USV-5 and Merr-Sonn JT-12.
- Added missing female rider poses for Air-2, FG 8T8, Geonosian Speeder, BT310,
  Republic Gunship, and XJ-6 using their existing compatible animation clips.
- Corrected two AT-RT datapad previews so the regular and camouflage models
  match the vehicles their deeds actually create.
- Preserved the working walker, wheel-bike, STAP, and Advanced Tantive mappings.

## Deferred entries

The AT-AT and six other colored swoop racers still lack their intended vehicle
templates or appearances. Their deed registrations are disabled, with source
files retained for later work; they are excluded from active spawn lists. The six
aliases do not add distinct vehicle models. The two light-bending deeds remain
retired. The later black Swoop Racer setup is listed in the
[complete vehicle command reference](vehicle-deed-commands.md). The user confirmed
it working on 2026-09-23 after correcting the TRE package and refining its texture.

## Jetpack follow-up

HK-47 and Merr-Sonn JT-12 now use the original jetpack's automatic mount/store
behavior and hover-height setting. The server change and client customization
registrations must be deployed together. Check automatic mounting on call,
storage on dismount, and height against the original Mandalorian Jetpack.
The user has confirmed that both jetpacks now look and mount/store correctly.

## Validation and future regression checks

Standalone checks passed for all 12 additional deed/control-device/mobile chains,
male and female rider poses, rider attachment points, 509 client dependencies,
and the USV-5 mesh bounds. That initial expanded audit resolved 66 complete
deed chains: 49 ordinary and 17 TCG, with 56 distinct mobile template paths.
After adding the black Swoop and disabling the duplicate and regular AT-RT deeds,
the current checks resolve 54 complete
chains: 37 ordinary and 17 TCG, each with a distinct mobile template. The seven
incomplete deeds are also disabled. The camouflage AT-RT remains available.
The client CRC index already contains the new USV-5 shared deed path; no index
override is needed.

For future deployments, package the complete `mtg_patch_024` overlay and install
it on both server and client with precedence over older assets. Deploy the Lua
registrations, then restart both. The regression checklist is each deed, datapad
preview, call/store, mounting, rider pose, movement, and recall after relogging,
with both male and female characters. The user's current confirmation covers
the active vehicles; it does not establish additional flight, weapon, or
passenger features.

The wheel-bike Jump-key handler requires the updated Core3 build. On 2026-09-23,
the user confirmed jumping working appropriately after the target-turning correction.
Handled mounted jumps now send an untargeted social message while retaining the
player's actual selection. The Jump radial has been removed, and the configured
Jump key and typed `/jump` share the two-second cooldown.

Validation used standalone checks; the user performed Core3 builds and in-game testing.
