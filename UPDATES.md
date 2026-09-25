# Updates

## 2026-09-25

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
