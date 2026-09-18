# Running Project Guidance

These instructions apply throughout this project. Paths below are relative to the repository root.

## Workspace boundaries

- Stay within `MMOCoreORB/bin` and `MMOCoreORB/src` for all project work.
- Keep project file access, searches, edits, and command working directories within these two folders.
- The root `AGENTS.md` is an exception: it may be read and updated to maintain this guidance.
- `/home/swgemu/workspace/tre` is an exception for read-only access: its files may be listed, searched, and read whenever needed. Do not create, modify, move, or delete files there. Keep command working directories and any extracted or generated files within `MMOCoreORB/bin` or `MMOCoreORB/src`.
- Do not access other locations unless the user explicitly asks you to do so.

## Git commits

- Do not create commits unless the user explicitly asks you to commit.

## Building, running, and testing

- Leave building and running Core3 to the user. The user will report build errors, warnings, and runtime errors for investigation.
- Do not compile Core3 or its components, link or install a rebuilt executable, or run `core3`, including under GDB or through `runUnitTests`, unless the user explicitly requests it.
- Perform as much relevant validation as possible before that point, within the allowed folders: review changes and callers, check diffs and whitespace, use available syntax or static checks, and run standalone tests that do not build or execute Core3.
- Add or update regression tests when appropriate, even when executing them requires the user's Core3 build. Clearly distinguish checks actually run from build-dependent or runtime checks left for the user.

## Maintaining this guidance

- Keep this root file as the single running record of project instructions and preferences provided by the user.
- Update it when the user adds or changes standing project guidance.
