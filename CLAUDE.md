# STIx Config: rules for working in this repository

STIx Config is the desktop editor (Qt 6.11 / QML, C++17, CMake) of the STIx building-automation platform. It is a visual programming tool that produces a human-readable `Project.xml`, which is uploaded to a controller.

## Read these before changing code
- `bmad-output/architecture.md`: the single source of truth. The ADRs marked **LOCKED** are rules, not suggestions.
- `bmad-output/stories/<id>.<slug>.story.md`: the unit of work. Implement **one story at a time**.
- `bmad-output/decision-log.md`: decisions made so far. Append to it; never rewrite entries.
- `bmad-output/DESIGN.md` (visual system: tokens, components, accessibility) and `bmad-output/EXPERIENCE.md` (journeys, screens, states), once they exist: read them for any UI work.

## Working on a story
1. Read the whole story file, then every ADR or section it cites.
2. Change only the files in its **Owned File/Module Scope**. If you must touch another file, stop and say why.
3. **Never edit** the sections *Acceptance Criteria*, *Dev Notes* or *Testing* (they are LOCKED). You may tick tasks, fill **Dev Agent Record** (what you did, decisions, files changed, open issues), and set `Status` to `in-progress` or `review`, never `done`.
4. Build and run the tests (below) before reporting that you're finished. Report failures honestly, with their output.
5. Don't commit, push or create branches. The owner does that.

## Build and test (macOS, this machine)
```bash
cmake -S . -B build-agent -G Ninja \
  -DCMAKE_MAKE_PROGRAM=$HOME/Qt/Tools/Ninja/ninja \
  -DCMAKE_PREFIX_PATH=$HOME/Qt/6.11.2/macos \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build-agent
ctest --test-dir build-agent --output-on-failure
open build-agent/appSTIx_Config.app      # run the app
```
`build/` belongs to Qt Creator. Don't use or delete it.

## Code rules (summary of ADR-001, 002, 009, 010, 011)
- **Layers:** L1 Document/Logger/Setup/sdk → L2 Project/Schema/BlockManager/DataTypes → L3 services (ProjectManager, ControllerLink, LibraryService, LiveValues, ProjectSync) → L4 QML. A file includes only from its own layer or lower ones.
- **Document:** the `XmlNode_class` tree is the only model. Every edit goes through the undo commands (`InsertNodeCmd`, `RemoveNodeCmd`, `MoveNodeCmd`, `SetAttributeCmd`), usually wrapped in a `cmd*` method on `Project_class`. Never add a new primitive.
- **XML names** come only from the `TOKEN_*` / `ATTR_*` / `TAG_*` macros (`sdk/stix_tokens.h`, `project_tokens.h`); no string literals.
- **QML** only displays and calls `cmd*` / methods. No business rules in QML. State lives in C++.
- **Errors:** no exceptions across boundaries. Return `bool` or `Result_s{ErrorCode_e}`, log once with `S_Logger.write(level, <MODULE>_NAME, …)`.
- **Naming:** `Name_class`, `Name_s`, `Name_e`, globals `S_Name`, members `m_<kind>_Name` (`ptr`, `str`, `vec`, `hash`, `map`, `b`, `int`, `real`), files `lowercase.h/.cpp`, QML `PascalCase.qml`, XML `PascalCase`, versions SemVer.
- **Portability:** Windows, macOS, Linux. Paths via `QStandardPaths` / `QDir`. No `#ifdef Q_OS_*` outside `src/Platform/`. XML is written UTF-8 with LF line endings.
- **Comments and identifiers are in English.**

## Agents
- `story-dev`: implements one story.
- `story-reviewer`: reviews a story's diff against its acceptance criteria and the ADRs (read-only).
- `qml-ui`: implements or restyles QML screens following `DESIGN.md`, `EXPERIENCE.md` and `Theme.qml`.
- For bug hunting, also run `/code-review` on the changes.
