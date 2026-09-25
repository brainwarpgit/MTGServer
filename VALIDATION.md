# Validation

## 2026-09-25

### Significant startup warning and TRE cleanup

- **Entered validation:** 2026-09-25
- **Status:** Validated — targeted warnings removed; delayed shuttle mapping deferred
- **Validated:** 2026-09-25
- **Evidence:** Static review confirmed one minimal table for each of the eight affected zones, all four battlefield templates now use the ship class, the PCD no longer has deed-only fields, the resource template uses its RCCT parent, and the particle-warning exception is restricted to the exact verified path. The 22 supplied ship-chassis IFFs are present under `mtg_patch_024/datatables/space`. The corrected 1,486-byte shared pilot-chair IFF references the existing `ship_pilot_station.iff` descriptor and has SHA-256 `90221a0a9ba9a9834b3669438fc447cdf403373f1486a477ca568f9a43e08db8`. The deployed `mtg_patch_024.tre` matches all 24 staged paths and payloads byte-for-byte. Core3 initialized in 48 seconds, and the resulting startup log contains none of the eight PlanetManager configuration warnings, six template-type mismatch warnings, `pt_light_indoor_glow.prt` component warnings, 22 supplied chassis-table warnings, or the `shipcontrol_pob.iff` warning. Its 28 remaining TreeArchive warnings are exactly the known unresolved data paths recorded below.
- **Core3 build/runtime validation:** The user rebuilt the TRE archive and Core3, loaded the server, and confirmed that all changes discussed in this cleanup are working.
- **Remaining work:** No further work is required for the implemented fixes. Correct source data is still needed before addressing the two mining-asteroid chassis tables, three ship client-data CDFs, Corellian-corvette POB, four missing snapshots, and `particle_test_31.prt`; their warnings intentionally remain visible. A longer run also produced 38 delayed `ScheduleShuttleTask` errors after the five-minute boot timer for snapshot shuttles in Chandrila, Coruscant, Hoth, Kaas, Mandalore, Moraband, and Taanab. Authoritative `planetTravelPoints` are not available in the current branch, and the user explicitly deferred this shuttle work.

### Startup template and region repairs

- **Entered validation:** 2026-09-25
- **Status:** Validated — targeted startup warnings and errors removed
- **Validated:** 2026-09-25
- **Evidence:** Static review confirmed one shared and one server registration for the missing vehicle parent, the generic lightsaber shared registration and server include, and correctly named files and globals for all five reported zones. The staged vehicle IFF is 1,278 bytes and has SHA-256 `11da6aa2a9a05f83e339c80aefa61af0437c31146bc02c05e59221f136f60aba`, matching the byte-identical common vehicle-component base assets in the configured source archive. The deployed `mtg_patch_024.tre` contains exactly that path and payload. Both Lua configurations list patch 024 first, and the source latest-TRE fallback also names patch 024. Git whitespace validation passed.
- **Core3 build/runtime validation:** The user restarted the server and confirmed that the targeted warnings and errors are gone.
- **Remaining work:** None for actionable startup errors 2 through 4. The empty region files intentionally provide zero scripted regions.

### Legacy MESH/0003 compatibility

- **Entered validation:** 2026-09-25
- **Status:** Validated — former runtime parsing exception removed
- **Validated:** 2026-09-25
- **Evidence:** The current branch's startup diagnosis documents the failing asset as `MESH/0003` with `SPS /0000`, `VTXA/0002`, raw 32-bit indices, and `EXBX/0000` bounds. Static review confirmed that the new parser follows that hierarchy while retaining modern count-prefixed 16-bit and 32-bit index handling. Parser failure cleanup and affected null callers were reviewed, and Git whitespace validation passed. The user subsequently confirmed that the former `InvalidChunkTypeException` no longer occurs.
- **Core3 build/runtime validation:** The user rebuilt and confirmed that `appearance/defaultappearance.msh` loads without the former `InvalidChunkTypeException`.
- **Remaining work:** The synthetic local `MeshAppearanceTest` has not been run; it may be run later in a test-enabled build, but no further work is required for the reported startup error.

### Branch-local source provenance

- **Entered validation:** 2026-09-25
- **Status:** Validated — documentation and source review
- **Validated:** 2026-09-25
- **Evidence:** Confirmed that `AGENTS.md` prohibits using source changes from commits on other branches and requires fixes to rely solely on evidence in the currently checked-out branch. The accompanying legacy mesh changes were reviewed under that constraint.
- **Core3 build/runtime validation:** Not applicable; this is a standing workflow rule.
- **Remaining work:** None for the guidance change.

### Engine3 upstream alignment

- **Entered validation:** 2026-09-25
- **Status:** Validated — build completed and game server loaded
- **Validated:** 2026-09-25
- **Evidence:** Static review confirmed that `.gitmodules` selects the official engine3 `master` branch and that the recorded submodule revision is upstream commit `4cbf39336e0e727dfef861fbe65bd303ce70fd37`. That revision contains the required `Vector4` and `Matrix4` JSON support and the Clang 20-compatible literal-operator declarations. The discarded CMake workaround is absent, and Git whitespace checks passed.
- **Core3 build/runtime validation:** The user reported that the Core3 build completed and the game server loaded successfully with the aligned engine3 revision.
- **Remaining work:** None for this dependency alignment.

### Local server files

- **Entered validation:** 2026-09-25
- **Status:** Validated — file and Git state checks
- **Validated:** 2026-09-25
- **Evidence:** Confirmed that `config-local.lua` matches `config.lua`, both requested local files match explicit ignore rules, and `resource_manager_spawns.lua` remains on disk while being removed from the Git index.
- **Core3 build/runtime validation:** Not run; the user retains responsibility for building and running Core3.
- **Remaining work:** None for this local-file tracking change.

### Project workflow records

- **Entered validation:** 2026-09-25
- **Status:** Validated — documentation and static checks
- **Validated:** 2026-09-25
- **Evidence:** Confirmed that `AGENTS.md`, `UPDATES.md`, `UPDATESFULL.md`, and `VALIDATION.md` are located at the repository root and are readable and writable by the current account. Reviewed the guidance for consistent filenames and wording, checked that the concise and expanded histories describe the same implemented change, and ran Git whitespace validation.
- **Core3 build/runtime validation:** Not applicable; no Core3 source, server/client content, configuration, or database behavior changed.
- **Remaining work:** None for this documentation-only change.
