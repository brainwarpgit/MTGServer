# Core3 project updates

Last updated: 2026-09-23.

Quick summary of MTGServer project work beginning on 2026-09-15. See
[updatesfull.md](updatesfull.md) for the expanded history, settings, and testing status.

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

## Latest testing results — 2026-09-23

- The user confirmed the template-display labels are working and reported that
  all remaining active vehicles appear to work correctly. Their initial testing
  items are closed; disabled and retired deeds remain inactive.
