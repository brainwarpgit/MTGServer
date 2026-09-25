# Updates

## 2026-09-25

### Significant startup warning and TRE cleanup — Committed

- Added minimal PlanetManager configurations for Kashyyyk, Hoth, Kaas, Coruscant, Chandrila, Moraband, Taanab, and Mandalore. These configurations remove the missing-settings warnings and allow each zone to proceed to snapshot loading without inventing weather or GCW behavior.
- Corrected six shared-template type mismatches: the resource-spawn base now uses the resource-container type, four battlefield-station templates use ship types, and the single-pod airspeeder is consistently registered as an intangible player control device (PCD).
- Suppressed only the misleading component-child warning for the present `pt_light_indoor_glow.prt` particle appearance that the server intentionally does not instantiate. Warnings for every other unresolved child remain visible.
- Added 22 supplied ship-chassis tables and a corrected shared pilot-chair asset to `mtg_patch_024`, removing those missing-table warnings and the `shipcontrol_pob` descriptor warning.
- The user rebuilt the TRE archive and Core3, loaded the server, and confirmed the discussed changes are working. The remaining genuine data gaps are the two mining-asteroid chassis tables, three ship client-data CDFs, the Corellian-corvette POB, four snapshot files, and `particle_test_31.prt`.
- Thirty-eight delayed shuttle-scheduling errors in seven custom zones remain deferred at the user's request; their snapshots are intentionally left enabled until authoritative travel-point data is added.

### Startup template and region repairs — Committed

- Added the missing common vehicle-component parent template and staged its client IFF under `mtg_patch_024`, restoring the inheritance target used by 11 civilian and military vehicle components.
- Registered the existing generic training-lightsaber base on both the shared and server sides so its derived template can load.
- Added empty region tables for the enabled Coruscant, Dungeon2, Kashyyyk Main, Moraband, and Taanab zones; corrected the existing Tanaab filename and table spelling to Taanab.
- Added `mtg_patch_024.tre` first in the tracked and local `TreFiles` lists and advanced the source default for the latest TRE from patch 023 to patch 024.
- Static validation passed, including the deployed archive contents, and the user confirmed that the targeted startup warnings and errors are gone.

### Legacy MESH/0003 compatibility — Committed

- Added version-aware parsing for the legacy `appearance/defaultappearance.msh` layout documented by the current branch's startup diagnostics, including `SPS /0000`, `VTXA/0002`, raw 32-bit indices, and `EXBX/0000` bounds.
- Preserved modern mesh support, including count-prefixed 16-bit and 32-bit indices, and added validation for malformed vertex/index data.
- Failed appearance parses now return no template instead of a partial object; collision and building callers safely handle absent appearances or empty mesh trees.
- Added a local ignored regression test for the legacy layout. The user rebuilt and confirmed that the former `InvalidChunkTypeException` is gone.

### Branch-local source provenance — Committed

- Updated project guidance to prohibit using source changes from commits on other branches and to require fixes to be developed solely from evidence available in the checked-out branch.

### Engine3 upstream alignment — Committed

- Configured the engine3 submodule to follow its official upstream `master` branch and aligned the recorded revision with the Core3 source that added ship JSON serialization.
- The aligned engine3 revision supplies the required `Vector4` and `Matrix4` JSON support and uses Clang 20-compatible literal-operator declarations; the temporary CMake warning exception was discarded.
- Updated project guidance to keep engine3 source immutable while allowing explicitly requested upstream dependency alignments.
- A user-run build completed and the game server loaded successfully.

### Local server files — Committed

- Refreshed `MMOCoreORB/bin/conf/config-local.lua` from the tracked `config.lua` template while preserving the previous local file as an ignored backup.
- Added explicit ignore rules for `config-local.lua` and `resource_manager_spawns.lua`, and removed the resource spawn file from Git tracking without deleting its local working copy.

### Project workflow records — Committed

- Added repository-level project guidance for working safely within this SWGEmu Core3 project.
- Added concise and expanded update histories plus a validation ledger.
- Clarified the future tracked-asset purpose of `mtg_patch_024` and corrected wording and filename capitalization in the project guidance.
