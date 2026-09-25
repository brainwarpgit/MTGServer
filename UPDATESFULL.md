# Detailed Updates

## 2026-09-25

### Local server files — Committed

`MMOCoreORB/bin/conf/config-local.lua` was refreshed from the tracked `config.lua` template. Because an existing local configuration differed from the template, it was preserved as an ignored numbered backup before replacement.

`MMOCoreORB/bin/.gitignore` now contains explicit rules for `conf/config-local.lua` and `scripts/managers/resource_manager_spawns.lua`. The resource spawn file remains available in the working tree but has been removed from the Git index so future local changes are not tracked.

This is a local configuration and repository-tracking change. Core3 was not built or run; the file copy and Git ignore/index state were validated directly.

### Project workflow records — Committed

Repository-level guidance now defines the permitted work areas, the future TRE archive staging location, the restriction against unrequested Core3 builds or runs, and the requirement for explicit authorization before creating Git commits.

The project now has two synchronized update histories: `UPDATES.md` provides a concise summary, while `UPDATESFULL.md` records broader context, impact, and remaining work. `VALIDATION.md` separately records validation status and evidence. The guidance was also corrected for filename capitalization, grammar, and the future tracked-asset purpose of `mtg_patch_024`.

This change affects project workflow and documentation only. It does not change Core3 source code, server behavior, client content, configuration, or database state. No deployment action or Core3 build/runtime testing is required.

The four project-record files are committed project history.
