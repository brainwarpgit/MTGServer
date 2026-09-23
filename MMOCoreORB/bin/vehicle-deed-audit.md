# Vehicle deed audit

Initial audit date: 2026-09-22. Status updated: 2026-09-23.
Scope: configured vehicle deed assets, server Lua,
control devices, mobile templates, and the repository's `mtg_patch_024` overlay.

## Current results

- On 2026-09-23, the user reported that the active vehicles work in game and
  confirmed the corrected template-inspection labels. Earlier checks also
  confirmed STAP mounting, Advanced Tantive IV, walker movement, and wheel-bike
  jumping.
- 54 active vehicle deeds have complete statically checked creation chains:
  37 ordinary and 17 TCG, each creating a distinct mobile template.
- All 54 complete deeds preview their full vehicle model, including the walkers
  and the TCG Swamp Speeder.
- All 54 active vehicle deeds have clean names and original Star Wars
  descriptions. Names omit “Deed for:” and “Vehicle Deed”.
- The user disabled 12 duplicate deed registrations. Their vehicle and
  control-device templates remain registered; no further duplicate mobile
  destinations remain among the active deeds.
- The broader control-device audit identified 12 additional vehicle models:
  11 with existing TCG deeds and a modified USV-5 needing a new deed. See
  [additional vehicles and spawn commands](/home/swgemu/workspace/MTGServer/MMOCoreORB/bin/additional-vehicle-deeds.md).
- Seven incomplete deed registrations are disabled for later work at the user's
  request. Their source files/assets and the five special/base templates are retained.
- The two light-bending vehicle deeds remain retired.
- The user reports that the regular AT-RT mesh change did not fix its appearance;
  its deed is now disabled at their request. The camouflage AT-RT remains active.

The user confirmed working jump animation, jetpack appearance and automatic
mount/storage, and vehicle names. The wheel bike previously turned when jumping
with a target selected. The server now sends mounted jump socials without a
target; on 2026-09-23, the user confirmed jumping working appropriately.
No Core3 build or execution was performed.

## Walking animation compatibility

The previous mount setup attached a second full skeletal vehicle as its saddle.
Client movement supplies animation velocity to the main creature appearance;
that attached model can stay in its idle animation. In the wheel-bike assets,
idle rotates only the wheel, while the moving animation also articulates the
knees and feet. This matched the original movement-animation symptom.

The prepared compatibility patch uses the client's creature-mount rendering path:

- Six shared mobile templates use client type `1024`, keeping the full animated
  body on the object receiving movement updates.
- Their six server Lua templates explicitly retain type `65536` and `VEHICLE`
  template behavior, preserving vehicle creation, ownership, storage, and decay.
- Twenty-one mesh detail levels gain a `saddle` hardpoint copied exactly from the
  original `player` hardpoint. This animation fix preserves geometry and skinning;
  the subsequent regular AT-RT geometry repair is described below.
- Three invisible saddle models provide the rider attachment without displaying
  a second idle vehicle body. Two mount tables connect them to the correct poses.

Affected mobile templates: `tcg_at_pt_walker`, `temp_walker1`,
`walker_at_rt_reg`, `walker_at_st`, `walker_at_xt`, and `grievous_wheel_bike`.
Their alternate deed paths use these same templates. The existing walk, run,
and jump animation clips are preserved.

The user confirmed that the movement animations work with these changes.
Client-side vehicle hovering dynamics do not apply to the creature-mount path;
continue checking terrain handling, sound, and rider placement during play.
Relevant source: [SaddleManager](https://github.com/SWG-Source/client-tools/blob/master/src/engine/client/library/clientGame/src/shared/mount/SaddleManager.cpp)
and [PlayerCreatureController](https://github.com/SWG-Source/client-tools/blob/master/src/engine/client/library/clientGame/src/shared/controller/PlayerCreatureController.cpp).

## Regular AT-RT — deed disabled after unsuccessful mesh repair

The retained `atrt_deed.iff` definition points to `walker_at_rt_reg.iff`, but its
deed registration is disabled. Its two highest-detail
meshes were incomplete: they lacked the second mechanical material group and
had incorrect joint weights. The next detail level omitted the right knee,
ankle, foot, and toe from its skin bindings. The complete camouflage AT-RT used
by `temp_walker1_deed.iff` has the same skeleton and the missing geometry.

The patch replaces only `appearance/mesh/walker_at_rt_reg_l0.mgn` and
`appearance/mesh/walker_at_rt_reg_l1.mgn` with that complete authored geometry,
UV coordinates, and joint weights. They retain the regular main-body paint and
emissive shaders and the exact existing regular hardpoints, including the rider
attachment. Triangle counts increase from 2,169 to 4,512 and 1,816 to 3,881.
The distant meshes, appearance, skeleton, animation clips, and mount tables
remain unchanged.

The restored secondary mechanical sections use the source model's material,
including its camouflage tint masks. The regular main-body paint is retained;
no matching regular secondary material was available in the TRE files.

Standalone checks verify all triangle and vertex indices, normalized weights,
both legs' hip/knee/ankle/foot/toe bindings, material dependencies, and exact
preservation of the rider attachment. **User result, 2026-09-23:** the AT-RT is
still not fixed. The mesh replacement above is an attempted repair, not a
confirmed solution; the missing-component appearance issue remains unresolved.
The earlier walking-animation fix was confirmed separately.
The user then requested removal of this deed. Its registration and active spawn
commands are disabled; backing vehicle/control-device templates and assets are
retained. The camouflage `temp_walker1_deed.iff` is still available.

## Wheel-bike jump

The player's configured **Jump** key invokes the jump while the linked owner
rides their wheel bike. The Jump radial option has been removed. Eligibility
is checked when triggered: the rider must occupy
this vehicle's rider slot, both objects must be upright in the same active zone,
and the vehicle must be usable. A two-second vehicle cooldown prevents overlap.

The configured client input action `CMD_jump` sends `/jump` through the social
command handler. The server now redirects that animation to the wheel bike.
Changing the Jump key in the client options therefore changes the trigger;
Space is not hardcoded. Text-only socials do not trigger a hop, and other
vehicles keep their existing behavior. Typed `/jump` uses the same cooldown.

A handled wheel-bike jump clears the target ID only in the outgoing social
message. The player's actual selected target is preserved. This prevents the
social message from asking the client to face the selected object, including
when a repeated jump is on cooldown. Normal socials retain their targets.

The action plays the existing wheel-bike `jump` animation. The server sends the
animation without directly changing the vehicle position or collision rules.
The clip contains forward motion that the client can apply; check the actual
travel and terrain interaction in game.

The jump lookup used the undefined `locomotion_emote` priority, which this
client data resolves to zero. Its pose and locomotion priorities now use the
existing `action` group (5), above walking and turning (4). Only this jump
priority leaf changed; the five other lookup branches and original animation
clip are unchanged. The clip plays once and returns to the normal movement loop.

On 2026-09-23, the user confirmed the wheel-bike jump working appropriately,
including the follow-up correction for target-directed turning. Its authored
animation remains unchanged by that follow-up fix.

## Deed presentation

The 54 active complete deed paths use full visible vehicle appearances, rather than
hover-vehicle proxy meshes or generic datapads. Walking mounts use their main
skeletal appearances, never the new invisible saddles. The seven deferred deeds
and five special templates retain their existing preview assets.

The six older TCG deeds retain the corrected names, descriptions, and visible
models for Mechno-Chair, XJ-2 Airspeeder, Ord Pedrovia Podracer, GPE-3130
Podracer, Sith Speeder, and Swamp Speeder. Their duplicate ordinary deed
registrations are now disabled. The X-31 and X-34 retain separate deeds: they
share a model but have different speed and acceleration settings.
An independent check covers every active vehicle-deed registration. The seven
deferred deeds are excluded from the active presentation and command checks.

Text is isolated in `string/en/mtg_vehicle_deed.stf`, with 150 retained name and
description values, including four for the retired light-bending deeds. Text
for disabled duplicate deeds is preserved. Existing string IDs remain unchanged.
Examples:
**AT-RT Walker (Camouflage)**, **TSMEU-6 Personal Wheel Bike**, **XJ-2 Airspeeder**, and
**Advanced STAP-1**. Descriptions provide setting and character without promising
unsupported weapons, flight, or passenger features. Original game strings and
[Star Wars vehicle references](https://www.starwars.com/databank/barc-speeder)
informed the names and lore.

## Jetpack behavior

The original jetpack, HK-47 Jetpack, and Merr-Sonn JT-12 use the same server
classification for automatic mounting on call and storage on dismount. All
three receive `/private/index_hover_height = 40`, matching the original
jetpack's flight-height setting. The two TCG variants also need the client
customization registrations included in the patch for that setting to work.
Their existing models, rider attachments, and thruster effects are preserved.

## Template inspection

`Core3.TangibleObject.ShowTemplate` adds **Server Template** and **Shared Template**
rows to Examine for tangible objects, including vehicles and auction/vendor item
inspection. These are the complete registered server and shared client `.iff`
paths. The option defaults to `false` in `conf/config.lua` and is enabled in the
local config for verification. While enabled, all viewers see the rows.
Missing templates are omitted safely, and existing inventory access checks
continue to apply. Setting the option to `false` hides the diagnostic rows.
On 2026-09-23, the user confirmed the option works and clarified that it is
primarily a testing tool.

The missing labels reported afterward are corrected by adding the lowercase
keys `server template` and `shared template` to `string/en/obj_attr_n.stf` in
the patch. These map to **Server Template** and **Shared Template**, matching
the client's lookup of the existing wire labels. All 827 earlier entries and
their IDs are preserved. On 2026-09-23, the user confirmed that the corrected
labels now work. Deploying this localization change requires the updated string
table and a client restart; it does not require a Core3 rebuild.

## Deployment and future regression checklist

The active vehicles and corrected template labels are user-confirmed as working.
Keep the following checklist for future deployments and regression testing;
the confirmation is not a record of every individual scenario below.

1. Deploy the updated server Lua, including the black Swoop template and
   registrations, together with the TRE patch. The six walking vehicle templates
   must retain their explicit server types when the shared client types change.
2. Package the complete `mtg_patch_024` folder, preserving its relative paths,
   and install the archive with precedence over older assets on both server and
   client. Include `appearance`, `object`, `datatables/mount`, `customization`,
   `shader`, `texture`, and `string/en`.
3. Build Core3 for the Jump handler, jetpack behavior, and template inspection. Deed presentation and walking
   asset changes themselves require the updated Lua/TRE files, not a rebuild.
4. Restart both server and client. Test fresh deeds and call stored vehicles.
5. For each walker, test idle, walking, running, turning, mounting, storing,
   recalling, and relogging. Check the seat and leg motion from the rider and an
   observing character, on level and sloping terrain.
6. Mount the wheel bike and use the configured **Jump** key, then rebind that
   action and test the new key. Test with another object selected and verify
   the hop preserves heading and selection. Check the cooldown and observer
   view, ordinary on-foot jumping, and absence of the Jump radial.
7. Check inventory previews, names, and examine descriptions. Retest STAP and
   Advanced Tantive IV as controls.
8. Call each of the three jetpacks and verify automatic mounting and comparable
   flight height. Dismount and verify that it returns to the datapad. Check
   both freshly created and previously stored jetpacks.
9. Examine a deed, vehicle, wearable, structure, and vendor item with template
   inspection enabled; verify their `.iff` paths. Disable it and request fresh
   attributes to confirm the diagnostic rows disappear.
10. Create the black Swoop Racer deed and check its preview and paint. Call,
    mount, drive, store, and recall it; compare performance with the standard
    Swoop and verify that its original appearance remains unchanged.

Earlier validation, before the 12 duplicate registrations were disabled,
covered production Lua evaluation; 49 ordinary and 17 TCG creation chains;
six separate client/server type overrides; 155 ordinary-chain templates
and 310 localized fields; all six walking animation/skeleton chains; 21 unchanged
rider transforms; 32 preserved compatibility assets; 418 existing mount
dependencies; 509 additional-vehicle dependencies; and the configured client
Jump input route. The client CRC index resolves all 181 deployable shared
deed/control-device/mobile template paths. These were static and standalone
checks; the user has since confirmed the active vehicles and jump behavior
in game.

The earlier presentation audit covered all 74 then-active deed names and
66 complete model previews, all three jetpack templates and their hover
variables, and preservation of
21,785 original customization mappings. The jump check verifies its supported
priority, one changed leaf, preserved movement branches, and unchanged hop clip.

Current registration checks confirm 54 active vehicle deeds, 54 complete chains,
no duplicate mobile destinations, and all backing templates for the disabled
duplicate deeds still registered. The current command list contains exactly
these 54 complete paths. The seven unfinished deeds and regular AT-RT deed are
disabled. Current checks also validate all 54 names, 54 complete model previews,
and 162 active shared templates against the client CRC index.

## Retired light-bending vehicles

Both light-bending deed server registrations and their test commands are removed.
Previously spawned physical deeds also lose their server template registration.
Their vehicle and control-device definitions remain available for saved datapad
objects. No database records were deleted. Decorative garage displays remain.
The proposed translucent shader overrides were removed from the patch folder.

## Deferred incomplete vehicles

On 2026-09-23, the user requested that these seven unfinished deeds be removed
from the active registrations and spawn-command list until they return to them
later. The Lua and client source assets are retained. Their control-device
templates remain registered; no saved objects were deleted.

The AT-AT deed points to an absent `object/mobile/vehicle/vehicle_atat.iff`.
Its datapad asset is also an AT-ST copy. An existing AT-AT NPC appearance is not
an established rideable vehicle template and has not been substituted.

The six other colored swoop racers (blue, gold, green, purple, red, and silver)
have no registered vehicle template or matching configured appearance
(`appearance/swoopracer_<color>.apt`). Their names are repaired, but their
creation links remain incomplete. These require the intended vehicle assets
and templates before re-enabling their deed registrations.

The earlier index search of 58 additional TRE archives found no colored
swoop-racer assets or matching AT-AT mobile-vehicle template variants. The black
variant now uses the separately prepared setup described below.

| Deed filename | Remaining issue |
| --- | --- |
| `atat_deed.iff` | No supported AT-AT vehicle template; datapad is an AT-ST copy. |
| `swoopracer_blue_deed.iff` | Missing colored racer vehicle template and appearance. |
| `swoopracer_gold_deed.iff` | Missing colored racer vehicle template and appearance. |
| `swoopracer_green_deed.iff` | Missing colored racer vehicle template and appearance. |
| `swoopracer_purple_deed.iff` | Missing colored racer vehicle template and appearance. |
| `swoopracer_red_deed.iff` | Missing colored racer vehicle template and appearance. |
| `swoopracer_silver_deed.iff` | Missing colored racer vehicle template and appearance. |

## Black Swoop Racer

The supplied `texture/swoop_black.dds` is now connected to a dedicated Swoop
appearance, material, and four mesh detail levels. The previously missing mobile
Lua and shared client vehicle template are prepared, along with the mount setup,
correct control-device appearance, and full vehicle preview on the existing
black deed. The integration preserved the supplied texture; the user subsequently
refined that texture.

When packaging, replace existing entries as well as adding new files. The
2026-09-23 runtime report was traced to the configured `mtg_patch_024.tre`
containing the new meshes but older supporting files: the black deed still
used the datapad preview, the logical/saddle tables lacked its body mapping,
the rider-pose table retained a BARC pose, and the customization table lacked
its hover settings. These five files already have corrected working versions.
The missing saddle mapping leaves the mounted vehicle's invisible base without
its visible body, explaining the reported behavior.

Rebuild the entire archive from the current `mtg_patch_024` directory, preserving
relative paths and replacing duplicate entries. Install the same archive on the
client and server, then restart both. From `MMOCoreORB/src`, run
`python3 tests/check_patch_archive.py` to compare configured archive payloads
with every working patch file. This read-only check uses no local asset overlay,
so an older packaged file cannot be hidden by a correct working copy.

This variant uses the standard Swoop vehicle as its basis. Speed, acceleration,
turning, rider placement, and other performance settings remain those of the
standard Swoop. Its distinct appearance uses the supplied black paint texture;
the original Swoop assets remain unchanged. On 2026-09-23, the user confirmed
the black Swoop Racer working after the package correction and texture adjustment.

## Special templates

These remain base/component/instant-travel items; their presence in this folder
is not evidence of a working vehicle deployment or instant-travel feature.

- `instant_travel_terminal_royal_ship_deed.iff`
- `instant_travel_terminal_snowspeeder.iff`
- `vehicle_deed_base.iff`
- `vehicular_prototype_deed.iff`
- `vehicular_prototype_bike_deed.iff`

## Active ordinary deeds

These 37 active ordinary deeds have complete checked creation chains and are
included in the user's 2026-09-23 confirmation that the active vehicles work.
The [complete vehicle deed command list](/home/swgemu/workspace/MTGServer/MMOCoreORB/bin/vehicle-deed-commands.md)
also includes all 17 active TCG deeds. The seven unfinished deeds and regular
AT-RT deed remain disabled and are excluded.

| Display name | Create command |
| --- | --- |
| A1 Deluxe Floater | `/object createitem object/tangible/deed/vehicle_deed/a1_deluxe_floater_deed.iff` |
| AB-1 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_ab1_deed.iff` |
| Advanced STAP-1 | `/object createitem object/tangible/deed/vehicle_deed/speeder_stap_deed.iff` |
| Advanced Tantive IV Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_tantive4_adv_deed.iff` |
| AT-PT Walker | `/object createitem object/tangible/deed/vehicle_deed/atpt_deed.iff` |
| AT-RT Walker (Camouflage) | `/object createitem object/tangible/deed/vehicle_deed/temp_walker1_deed.iff` |
| AT-ST Walker | `/object createitem object/tangible/deed/vehicle_deed/atst_deed.iff` |
| AT-XT Walker | `/object createitem object/tangible/deed/vehicle_deed/atxt_deed.iff` |
| AV-21 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_av21_deed.iff` |
| BARC Speeder | `/object createitem object/tangible/deed/vehicle_deed/barc_speeder_deed.iff` |
| Basilisk War Droid | `/object createitem object/tangible/deed/vehicle_deed/basilisk_war_droid.iff` |
| Cloud City Pod Car | `/object createitem object/tangible/deed/vehicle_deed/tcg_8_air_speeder_deed.iff` |
| Desert Skiff | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_desert_skiff_deed.iff` |
| Flare-S Swoop | `/object createitem object/tangible/deed/vehicle_deed/flare_s_swoop.iff` |
| Flash Speeder | `/object createitem object/tangible/deed/vehicle_deed/speederbike_flash_deed.iff` |
| Hover Chair | `/object createitem object/tangible/deed/vehicle_deed/hover_chair_deed.iff` |
| Hover Lifter Speeder | `/object createitem object/tangible/deed/vehicle_deed/hoverlifter_speeder.iff` |
| Imperial Command BARC Speeder | `/object createitem object/tangible/deed/vehicle_deed/barc_speeder_imperial_deed.iff` |
| Lava Skiff | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_lava_skiff_deed.iff` |
| Mandalorian Jetpack | `/object createitem object/tangible/deed/vehicle_deed/jetpack_deed.iff` |
| Mustafarian Panning Droid | `/object createitem object/tangible/deed/vehicle_deed/mustafar_panning_droid.iff` |
| Radon-Ulzer Podracer | `/object createitem object/tangible/deed/vehicle_deed/podracer_anakin_deed.iff` |
| RAM-1511 Transport | `/object createitem object/tangible/deed/vehicle_deed/military_transport_deed.iff` |
| Rebel Command BARC Speeder | `/object createitem object/tangible/deed/vehicle_deed/barc_speeder_rebel_deed.iff` |
| RIC-920 Rickshaw | `/object createitem object/tangible/deed/vehicle_deed/speeder_ric_920_deed.iff` |
| Speeder Bike | `/object createitem object/tangible/deed/vehicle_deed/speederbike_deed.iff` |
| Swoop Bike | `/object createitem object/tangible/deed/vehicle_deed/speederbike_swoop_deed.iff` |
| Swoop Racer (Black) | `/object createitem object/tangible/deed/vehicle_deed/swoopracer_black_deed.iff` |
| T-47 Snowspeeder | `/object createitem object/tangible/deed/vehicle_deed/snowspeeder_deed.iff` |
| Tantive IV Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_tantive4_deed.iff` |
| TSMEU-6 Personal Wheel Bike | `/object createitem object/tangible/deed/vehicle_deed/grievous_wheel_bike_deed.iff` |
| USV-5 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_usv5_deed.iff` |
| USV-5 Modified Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_usv5_s02_deed.iff` |
| V-35 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_v35_deed.iff` |
| X-31 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_x31_deed.iff` |
| X-34 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_x34_deed.iff` |
| XP-38 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_xp38_deed.iff` |
