# Detailed Updates

## 2026-09-25

### Significant startup warning and TRE cleanup — Committed

Eight enabled zones had no matching global table in `scripts/managers/planet/planet_manager.lua`, causing PlanetManager to skip their normal configuration and snapshot-loading path. Minimal entries were added for `kashyyyk`, `hoth`, `kaas`, `coruscant`, `chandrila`, `moraband`, `taanab`, and `mandalore`, with weather and GCW behavior disabled rather than inferred. Supplying these tables removes the missing-configuration warnings and allows each zone to proceed to snapshot loading without inventing unsupported weather or GCW behavior.

Six shared-template inheritance mismatches were corrected using the object types already established by the current branch. The resource-spawn base now reads its resource-container parent as `RCCT`; the four battlefield-station templates use the appropriate shared ship classes for their `SSHP` parent; and the TCG single-pod airspeeder is consistently treated as a `SITN` player control device (PCD). These changes avoid asking the generic shared-object loader to interpret type-specific parent data.

`ComponentAppearanceTemplate` no longer emits its misleading child-appearance warning for the verified, present `appearance/pt_light_indoor_glow.prt` particle effect, which is intentionally unsupported for server-side instantiation. The exception is limited to that exact path; warnings for all other unresolved component children remain visible. Genuine archive gaps such as `appearance/particle_test_31.prt` therefore remain diagnosable.

Twenty-two supplied ship-chassis tables were staged under `mtg_patch_024/datatables/space`: the ARC-170, four Black Sun medium variants, Black Sun transport, generic, Hutt turret ship, light and medium merchant cruisers, player ARC-170, player Jedi starfighter, player test Falcon, Rebel shuttle, smuggler warlord ship, TIE Defender and its five tier variants, and science transport. These are the tables reported by the earlier startup run except for the two mining-asteroid variants. After the user rebuilt and deployed the archive, the 44 corresponding missing-table warning lines were absent from the next startup log.

The supplied corrected `object/tangible/shipcontrol/shared_pilot_chair.iff` was also staged in `mtg_patch_024`. It references the existing `abstract/slot/descriptor/ship_pilot_station.iff` descriptor instead of the missing `shipcontrol_pob.iff`; the staged payload is 1,486 bytes with SHA-256 `90221a0a9ba9a9834b3669438fc447cdf403373f1486a477ca568f9a43e08db8`. The `shipcontrol_pob.iff` warning was absent after the user rebuilt the archive and restarted Core3.

The remaining unresolved ship-chassis paths are `datatables/space/ship_chassis_mining_asteroid_dynamic.iff` and `datatables/space/ship_chassis_mining_asteroid_static.iff`, each reported twice in the validated startup run. Also unresolved are three client-data files reported six times each: `clientdata/ship/client_shared_blacksun_transport.cdf`, `clientdata/ship/client_shared_escape_pod.cdf`, and `clientdata/ship/client_shared_hutt_transport.cdf`.

The other genuine TRE gaps are `appearance/corellian_corvette_pob.pob`, `snapshot/kashyyyk_north_dungeons.ws`, `snapshot/kashyyyk_south_dungeons.ws`, `snapshot/kashyyyk_rryatt_trail.ws`, `snapshot/tutorial.ws`, and `appearance/particle_test_31.prt`. These warnings remain intentionally visible until correct source assets are available.

The new valid planet tables also enable snapshot loading that the former missing-configuration path skipped. A longer run reached the five-minute shuttle boot timer and reported 38 `ScheduleShuttleTask` errors because no authoritative `planetTravelPoints` are defined for snapshot shuttles in Chandrila, Coruscant, Hoth, Kaas, Mandalore, Moraband, and Taanab. Kashyyyk produced no corresponding error. The user explicitly deferred these shuttle-timing errors, so snapshot loading remains enabled and no travel points were invented or warning paths suppressed.

In accordance with project guidance, Codex did not compile or run Core3. The user rebuilt the TRE archive and Core3, loaded the server, and confirmed the discussed changes are working. The deployed `mtg_patch_024.tre` contains all 24 staged assets with matching paths and byte content. Review of the resulting startup log confirmed that Core3 initialized in 48 seconds and that the eight PlanetManager configuration warnings, six template-type mismatch warnings, the misleading `pt_light_indoor_glow.prt` warnings, the 22 supplied chassis-table warnings, and the `shipcontrol_pob.iff` warning are gone. The remaining 28 TreeArchive warnings are exactly the known genuine data gaps above and continue to be reported rather than hidden.

### Startup template and region repairs — Committed

Three current-branch content omissions behind actionable startup errors 2 through 4 have been repaired without changing engine3 or importing source changes from another branch.

The 11 civilian and military vehicle-component templates derive from `object/tangible/component/vehicle/base/shared_base_vehicle.iff`, but the common parent was absent. A parent IFF matching the byte-identical common vehicle-component base assets already present in the configured `mtg_patch_010_object_01.tre` archive is now staged at the corresponding path under `mtg_patch_024`. Shared and server Lua registrations for `shared_base_vehicle.iff` and `base_vehicle.iff` were added and included by the vehicle base object loaders. The staged IFF is 1,278 bytes with SHA-256 `11da6aa2a9a05f83e339c80aefa61af0437c31146bc02c05e59221f136f60aba`.

The generic training-lightsaber server template already inherited from `object_weapon_melee_sword_base_shared_generic_lightsaber_base`, but that shared template was never registered and its server file was never included. The missing shared registration and server include now make the existing inheritance chain available during template loading; the derived template itself was not changed.

Planet region loading constructs a script filename and global table name directly from each enabled zone name. Empty conventional region scripts were added for `coruscant`, `dungeon2`, `kashyyyk_main`, and `moraband`. The existing `tanaab_regions.lua` file and `tanaab_regions` global were corrected to `taanab_regions.lua` and `taanab_regions`, matching the configured zone name. These placeholders intentionally define zero scripted regions; they remove the missing-script errors without inventing region or spawn content. Separate PlanetManager configuration warnings, if any, are outside this repair.

`mtg_patch_024.tre` is now the first archive in both the tracked `config.lua` and ignored local `config-local.lua` `TreFiles` lists, ahead of patch 023 so its content has the intended precedence. The `ConfigManager::getLatestTre()` fallback was advanced from `mtg_patch_023.tre` to `mtg_patch_024.tre`; no `ConfigManager.cpp` change or separate source-side TRE list is needed. The deployed archive was inspected read-only and contains exactly the staged `shared_base_vehicle.iff` with the expected size and hash.

Static registration, path, archive-content, configuration-order, hash, and Git whitespace checks passed. In accordance with project guidance, Codex did not build or run Core3. The user subsequently restarted the server and confirmed that the targeted vehicle-parent, generic-lightsaber, and region-script warnings and errors are gone.

### Legacy MESH/0003 compatibility — Committed

The appearance loader now recognizes the legacy `MESH/0003` structure recorded in the current branch's startup diagnosis for `appearance/defaultappearance.msh`. This path does not expect the modern `APPR` form; it reads `SPS /0000`, `VTXA/0002`, raw little-endian 32-bit triangle indices without a count prefix, the mesh center/radius chunks, and the outer `EXBX/0000` box and sphere bounds. Existing non-legacy parsing remains in place, and count-prefixed modern index data is accepted only when its payload exactly matches either 16-bit or 32-bit indices.

The parser now rejects invalid counts, strides, triangle groupings, and out-of-range indices before constructing collision geometry. Empty meshes no longer attempt to construct an AABB tree. If any appearance parse throws, the template manager deletes the partial template and returns `nullptr`; relevant detail-appearance, collision, and building paths now tolerate a missing appearance or collision tree instead of dereferencing it.

A synthetic `MESH/0003` regression test was added under the project's ignored local test directory. It covers the legacy form hierarchy, vertices, raw 32-bit indices, generated collision tree, and `EXBX/0000` bounds without relying on external TRE files or configuration. In accordance with project guidance, Core3 was not compiled or run by Codex. Static source review and Git whitespace checks passed, and the user subsequently rebuilt and confirmed that the former `InvalidChunkTypeException` for `appearance/defaultappearance.msh` is gone.

No engine3 source or submodule revision was changed, and no asset was replaced.

### Branch-local source provenance — Committed

Project guidance now explicitly prohibits cherry-picking, copying, or recreating source patches from commits on other branches. Source fixes must be developed solely from the source, documentation, diagnostics, and other project evidence present in the currently checked-out branch. The legacy mesh repair described above was reviewed against the current branch's files and startup diagnosis under this rule.

### Engine3 upstream alignment — Committed

The engine3 submodule is now configured to follow the official upstream `master` branch when explicitly updated with Git's `--remote` submodule mode. Its recorded revision was aligned from `34e855c03ffe4bd6431db614aa1c34a4e0b847fa` to upstream commit `4cbf39336e0e727dfef861fbe65bd303ce70fd37`, the engine3 revision paired with the Core3 change that enabled ship JSON serialization. Normal `git submodule update` operations remain reproducible and use the recorded commit; `git submodule update --remote` is required to advance to a later upstream `master` revision.

The aligned engine3 revision supplies JSON serialization for `Vector4` and `Matrix4`, resolving the generated `ShipObject.cpp` conversion failures. It also replaces the deprecated whitespace form of the affected user-defined literal declarations with the Clang 20-compatible form. The temporary Core3 CMake warning exception was discarded, so the existing warning policy remains unchanged.

Project guidance continues to treat engine3 source as immutable and prohibits using revision changes as source-code workarounds. It now permits a recorded-pointer update only when the user explicitly requests an upstream dependency alignment, as occurred here. No files inside engine3 were modified.

Static Git and source review confirmed the upstream branch configuration, recorded commit, Clang-compatible literal declarations, and required JSON support. The user subsequently completed the Core3 build and reported that the game server loaded successfully. No further deployment or validation work remains for this alignment.

### Local server files — Committed

`MMOCoreORB/bin/conf/config-local.lua` was refreshed from the tracked `config.lua` template. Because an existing local configuration differed from the template, it was preserved as an ignored numbered backup before replacement.

`MMOCoreORB/bin/.gitignore` now contains explicit rules for `conf/config-local.lua` and `scripts/managers/resource_manager_spawns.lua`. The resource spawn file remains available in the working tree but has been removed from the Git index so future local changes are not tracked.

This is a local configuration and repository-tracking change. Core3 was not built or run; the file copy and Git ignore/index state were validated directly.

### Project workflow records — Committed

Repository-level guidance now defines the permitted work areas, the future TRE archive staging location, the restriction against unrequested Core3 builds or runs, and the requirement for explicit authorization before creating Git commits.

The project now has two synchronized update histories: `UPDATES.md` provides a concise summary, while `UPDATESFULL.md` records broader context, impact, and remaining work. `VALIDATION.md` separately records validation status and evidence. The guidance was also corrected for filename capitalization, grammar, and the future tracked-asset purpose of `mtg_patch_024`.

This change affects project workflow and documentation only. It does not change Core3 source code, server behavior, client content, configuration, or database state. No deployment action or Core3 build/runtime testing is required.

The four project-record files are committed project history.
