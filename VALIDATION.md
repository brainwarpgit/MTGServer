# Validation

## 2026-09-25

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
