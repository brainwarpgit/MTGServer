# Running Project Guidance

These instructions apply throughout this project. Paths below are relative to the repository root.

## Workspace boundaries

- Stay within `MMOCoreORB/bin` and `MMOCoreORB/src` for project work unless an exception below applies.
- Keep project file access, searches, edits, and command working directories within these two folders except as explicitly allowed below.
- The root `AGENTS.md` is an exception: it may be read and updated to maintain this guidance.
- `/home/swgemu/workspace/tre` is an exception for read-only access: its files may be listed, searched, and read whenever needed. Do not create, modify, move, or delete files there. Keep command working directories in the writable locations authorized here.
- `mtg_patch_024` at the repository root (`/home/swgemu/workspace/MTGServer/mtg_patch_024`) is an exception with full read/write access, including searches, file creation, edits, and command working directories. Save new or corrected files intended for a TRE archive here, preserving their archive-relative directory paths (for example, `mtg_patch_024/object/tangible/deed/player_house_deed/<filename>.iff`). Files placed in this directory are intended to be tracked by Git. Keep other audit or test artifacts within `MMOCoreORB/bin` or `MMOCoreORB/src`.
- Do not access other locations unless the user explicitly asks you to do so.
- The root `UPDATES.md` and `UPDATESFULL.md` are exceptions: they may be read and updated to maintain work progress.
- The root `VALIDATION.md` is an exception: it may be read and updated to maintain current change validation.

## Git commits

- Do not create commits unless the user explicitly asks you to commit.

## Building, running, and testing

- Leave building and running Core3 to the user. The user will report build errors, warnings, and runtime errors for investigation.
- Do not compile Core3 or its components, link or install a rebuilt executable, or run `core3`, including under GDB or through `runUnitTests`, unless the user explicitly requests it.
- Perform as much relevant validation as possible before that point, within the allowed folders: review changes and callers, check diffs and whitespace, use available syntax or static checks, and run standalone tests that do not build or execute Core3.

## Maintaining this guidance

- Keep this root file as the single running record of project instructions and preferences provided by the user.
- Update it when the user adds or changes standing project guidance.

## Maintaining the update history

- Keep `UPDATES.md` and `UPDATESFULL.md` current as project work progresses.
- `UPDATES.md` is the short, plain-language list of meaningful Core3 and related server/client content updates.
- `UPDATESFULL.md` is the expanded high-level history: explain what changed, why it matters, relevant commands or configuration options, and any remaining deployment or testing work. Summarize related fixes together rather than listing every edited file.
- Update both files in the same working change as each meaningful feature, fix, content change, removal, or standing workflow change.
- Record actual implemented work and distinguish committed changes from uncommitted changes. Do not present a proposal, reverted experiment, or unresolved issue as a completed feature.
- Use dated sections, preserve earlier history, and update an existing entry when refining the same change. Identify related user-supplied asset or configuration changes when relevant, without including private local settings or secrets.
- Keep these files synchronized with each other and the final implementation. Maintaining them does not authorize a Git commit or a Core3 build/run.

## Maintaining validation history

- Keep `VALIDATION.md` current as project work progresses.
- `VALIDATION.md` tracks items awaiting validation and items that have been validated.
- Update this file for meaningful changes and record whether each change has been validated as working as intended.
- Record both the date an item enters validation and the date it is validated.
- Maintaining this file does not authorize a Git commit or a Core3 build/run.
