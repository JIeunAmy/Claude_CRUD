# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project state

This is a Visual Studio C++ solution (`Cluade_CRUD.slnx`) containing a single project, `Cluade_CRUD` (a Win32 console application). The project currently contains no source files (`.cpp`/`.h`) — only the `.vcxproj`/`.vcxproj.filters` scaffolding exists. No implementation has been started yet.

## Assignment requirements (from `docs/[CRA_AI] Day2_1_Agentic Engineering.pdf`, p.32)

Build a CRUD (Create/Read/Update/Delete) console application that manages data in a JSON file, reusing the code structure/logic of a prior PoC (Proof of Concept) — the PoC itself is not included in this repo, so its structure must be inferred or asked about if referenced later.

- Keep the code structure used in the PoC while implementing CRUD.
- **Create**: accept input for a new record and save it to the JSON file.
- **Read**: list all records, and support lookup by a specific ID/key.
- **Update**: select an existing record and modify specific fields.
- **Delete**: safely remove a specific record.

## JSON library

Use `ClaudeJson` (sibling repo `C:\Users\User\source\repos\ClaudeJson`), not a third-party library:

- Header: `C:\Users\User\source\repos\ClaudeJson\ClaudeJson\Json.h` (namespace `ClaudeJson`, type `JsonValue`).
- Prebuilt static lib: `C:\Users\User\source\repos\ClaudeJson\x64\Debug\ClaudeJson.lib` (Debug|x64 only, currently).
- API: `JsonValue::Parse`/`Load`/`Save`/`Dump`; object access via `value["key"]`, array via `value[idx]`/`PushBack`/`Size`; extraction via `AsString`/`AsNumber`/`AsBool`/`AsArray`/`AsObject`. Numbers are always `double`; objects preserve key insertion order (backed by `std::vector<std::pair<...>>`, linear-scan lookup). See `PLAN.md` for how this project links against it.

## Build

- Solution file: `Cluade_CRUD.slnx`, project: `Cluade_CRUD/Cluade_CRUD.vcxproj`.
- Toolset: `v145`, target Windows 10 SDK, C++ standard: `stdcpp20`.
- Configurations: `Debug`/`Release`, Platforms: `x86`/`x64` (only `Debug|x64` is currently viable end-to-end, since the `ClaudeJson.lib` dependency is only built for that configuration).
- Build via Visual Studio (open `Cluade_CRUD.slnx`) or MSBuild, e.g.:
  ```
  msbuild Cluade_CRUD.slnx /p:Configuration=Debug /p:Platform=x64
  ```
- New source files must be added to `Cluade_CRUD.vcxproj` (and typically `Cluade_CRUD.vcxproj.filters`) to be picked up by the build — MSBuild C++ projects do not glob-include files automatically.

## Notes

- `docs/[CRA_AI] Day2_1_Agentic Engineering.pdf` is the assignment brief for this project (see requirements above); it is not part of the build.
