# Detailed Updates

## 2026-09-25

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
