# Validation

## 2026-09-25

### Moraband boundary compatibility and city-config cleanup

- **Entered validation:** 2026-09-25
- **Status:** Validated — boundary compatibility and city-config cleanup confirmed
- **Validated:** 2026-09-25
- **Evidence:** Static inspection of the current configured Moraband terrain confirmed that `BPOL/0007` is the existing `0005` payload plus one 32-bit field between `shaderSize` and `shaderName`, and `BREC/0004` is the existing `0003` payload with the same addition. All eight available newer-version records contain zero in that field, so the new readers consume it without assigning unverified behavior. Source review confirmed that older readers are unchanged, the two skipped polygons are in the enabled `various mountains` layer, and the rectangle's `playable boundary` parent layer is disabled. The user confirmed that the runtime boundary-format errors are gone. `TerrainBoundaryVersionTest.ParsesPolygonVersionSeven` and `TerrainBoundaryVersionTest.ParsesRectangleVersionFour` both passed in the current test-enabled binary, together with an overall three-test result of zero. An exhaustive source search found no remaining `CityVotingDuration` declaration, configuration lookup, or consumer. A separate fresh filtered startup showed CityManager loading its configuration and reached `READY` without `CityVotingDuration` or unsupported boundary-version output. Git whitespace validation passed.
- **Core3 build/runtime validation:** At the user's explicit request, Codex ran the two boundary regression tests through a controlled GDB wrapper; both passed, and the wrapper stopped before the unrelated parser-test cleanup path. Codex then started Core3 with a fresh filtered console, observed `READY` after 46 seconds with no targeted diagnostics, and stopped it with Ctrl+C.
- **Remaining work:** None for this change.

### Core3 warning and error audit

- **Entered validation:** 2026-09-25
- **Status:** Completed — non-shuttle runtime diagnostics inventoried
- **Validated:** 2026-09-25
- **Evidence:** At the user's explicit request, Codex started Core3 at 15:33:04 (PID 255671). It initialized successfully in 46 seconds, remained running through the five-minute scheduled-task boundary and the normal 352-second database backup, and was then stopped intentionally. In `log/core3.log` lines 143106–144170, excluding the 38 deferred `ScheduleShuttleTask` errors, there are 28 TreeArchive warnings, five INFO-severity Lua load failures containing `ERROR`, and no exceptions. No additional non-shuttle warning, error, or exception appeared after initialization. The live console also reported two unsupported Moraband `BoundaryPolygon/0007` forms and one unsupported `BoundaryRectangle/0004` form; those direct-console messages are not copied into `core3.log`.
- **Core3 build/runtime validation:** Core3 was run and monitored by Codex under the user's explicit authorization. The process was stopped with Ctrl+C only after initialization, the five-minute timer, and the scheduled backup completed.
- **Remaining work:** The 28 known missing-data warnings comprise two mining-asteroid chassis tables reported twice each, three ship client-data CDFs reported six times each, the Corellian-corvette POB, `particle_test_31.prt`, and four snapshots. The five Lua load failures are three engine-configuration fallback probes and two absent optional `custom_scripts` overrides. The unsupported weighted base-player gender parameter produces 41 repeated INFO diagnostics while concrete player templates provide explicit genders. Moraband boundary compatibility and the removed `CityVotingDuration` read are now fully validated. Shuttle scheduling remains explicitly deferred.

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
- **Status:** Validated — runtime repair and parser regression test passed
- **Validated:** 2026-09-25
- **Evidence:** The current branch's startup diagnosis documents the failing asset as `MESH/0003` with `SPS /0000`, `VTXA/0002`, raw 32-bit indices, and `EXBX/0000` bounds. Static review confirmed that the new parser follows that hierarchy while retaining modern count-prefixed 16-bit and 32-bit index handling. Parser failure cleanup and affected null callers were reviewed, and Git whitespace validation passed. The user confirmed that the former `InvalidChunkTypeException` no longer occurs. Codex subsequently ran `MeshAppearanceTest.ParsesLegacyVersionThreeWithoutAppearanceForm` in the current test-enabled binary; it passed, and the combined three-test run returned zero.
- **Core3 build/runtime validation:** The user rebuilt and confirmed that `appearance/defaultappearance.msh` loads without the former `InvalidChunkTypeException`. At the user's explicit request, Codex also ran the synthetic mesh regression test through the controlled GDB wrapper; it passed, and the wrapper stopped before the unrelated parser-test cleanup path.
- **Remaining work:** None for this change.

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
