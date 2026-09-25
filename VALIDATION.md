# Validation

## 2026-09-25

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
