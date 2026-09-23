# Core3 project update history

Last updated: 2026-09-23.

Expanded high-level history of MTGServer's Core3 project work beginning on
2026-09-15, based on project commits, the current working changes, and user
testing reports. [updates.md](updates.md) contains the short version.

**Scope:** project history through the vehicle, inspection, Naboo Medium Style 2,
and documentation update recorded with this revision.
“Confirmed” below means the user reported the stated behavior working;
standalone asset or source checks do not establish an in-game result.

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
