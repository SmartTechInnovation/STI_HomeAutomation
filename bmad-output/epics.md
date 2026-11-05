# Epics: STIx Platform MVP

> The epic MAP: a thin index, not a context object. Each epic lists its goal, the
> requirements it covers, its ordered stories, and cross-epic dependencies. Story detail
> lives in the individual `{epic}.{story}.{slug}.story.md` files.
>
> Track: BMad Method (MVP, one developer)
> Sources: architecture.md v1.8 (§0 holds the requirements baseline, since there is no prd.md), decision-log.md
> Sizing: each story takes one dev-day at most (~2–8 h). There are no story points; progress is counted in stories.

---

## Epic 1: Foundation and conventions

**Goal:** a clean, testable, cross-platform base: one set of tokens, one error convention, tests and CI on 3 OSes, and translations.

**In scope:**
- NFR-07: clear, well-structured code [Source: architecture.md#0-requirements-baseline]
- NFR-13: unified logging and errors [Source: architecture.md#adr-010]
- NFR-04: Windows / macOS / Linux [Source: architecture.md#adr-013]
- FR-16: multi-language UI [Source: architecture.md#0-requirements-baseline]

**Architecture touchpoints:** Document tokens, Logger, Setup, BlockManager, ProjectManager, CMake [Source: architecture.md#adr-011]

**Out of scope:** new features.

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 1.1 | test-harness | Add a Qt Test target, a `tests/` tree and CTest, so every later story can add tests | ready-for-dev |
| 1.2 | tokens-consolidation | Merge the duplicated tokens into `sdk/stix_tokens.h` (program contract) and `project_tokens.h` (editor-only) | ready-for-dev |
| 1.3 | descriptor-attribute-fix | Fix `Garage_Gate` (`ShortTitle`/`Type` → `ShortName`/`DataType`, stray `T`); make all template `Vers` SemVer; warn on unknown port attributes | ready-for-dev |
| 1.4 | header-and-global-fixes | Make `resolveResourceUrl` inline; rename `Application` → `S_Application` | ready-for-dev |
| 1.5 | result-and-error-codes | Add `ErrorCode_e` / `Result_s` and module-name defines; move `ProjectManager` from `int` returns to `bool` + a `failed` signal | ready-for-dev |
| 1.6 | ci-matrix | GitHub Actions: build and test on Windows, macOS and Linux, with warnings as errors | ready-for-dev |
| 1.7 | translations-pipeline | Set up `qt_add_translations` with EN/RU/RO/ES `.ts` files, and reload the UI when the language changes | ready-for-dev |

**Cross-epic dependencies:**
- Blocks: every other epic. 1.1 (tests) and 1.2 (tokens) come first.

---

## Epic 2: Project file format v1.0.0

**Goal:** freeze `Project.xml` 1.0.0: readable, stable output, versioned with migrations, with a `<Requires>` header and `Rev`.

**In scope:**
- NFR-03, NFR-05, FR-01 [Source: architecture.md#adr-003]
- FR-13 (header part), NFR-06 [Source: architecture.md#adr-003]
- FR-22 (the `Rev` attribute) [Source: architecture.md#adr-017]

**Architecture touchpoints:** Document (`XmlNode_class`), Project load/save [Source: architecture.md#component-document-l1]

**Out of scope:** the merge logic (Epic 9).

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 2.1 | stable-xml-writer | Stable attribute order, 4-space indent, normalised UUIDs, unknown data kept | ready-for-dev |
| 2.2 | config-version-and-migrations | Parse `ConfigVers`; add a migration-chain framework; open newer MAJOR versions read-only; write a `.bak` before upgrading | ready-for-dev |
| 2.3 | golden-fixtures | Add a sample project for 1.0.0 under `tests/fixtures/projects/1.0.0/`, plus a round-trip test | ready-for-dev |
| 2.4 | requires-header | Generate `<Requires>` on save from the block and periphery types used; regenerate it if stale | ready-for-dev |
| 2.5 | rev-and-baserev | Read and write `Project/@Rev`; store `BaseRev` per project and controller | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 1 (1.1 tests, 1.2 tokens).
- Blocks: Epic 3 (wiring is saved in this format), Epic 6 (upload sends `baseRev`), Epic 9 (merge).

---

## Epic 3: Wiring and validation in the editor

**Goal:** the integrator connects outputs, signals and variables to block inputs (many-to-many), sees the wires, and is warned about type errors, broken references and loops.

**In scope:**
- FR-03, FR-04, FR-06 [Source: architecture.md#0-requirements-baseline]
- `<Input>` with 0..N `<Source>` elements [Source: architecture.md#5-data-model]
- NFR-15 (editor side: cycle warnings) [Source: architecture.md#adr-005]

**Architecture touchpoints:** Project commands, `Page_class::connections()` (still a stub), DataTypes, the new `ProjectValidator`, `DisplayConnections.qml` [Source: architecture.md#component-project-l2]

**Out of scope:** live values on the wires (Epic 7).

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 3.1 | connect-commands | `cmdConnect` / `cmdDisconnect` as undoable macros that create or remove `<Source Ref>`; several sources per input | ready-for-dev |
| 3.2 | page-connections-model | Implement `Page_class::connections()` and keep it updated from the document signals | ready-for-dev |
| 3.3 | connection-rendering | Draw the wires in `DisplayConnections.qml`; drag from an output port to an input port | ready-for-dev |
| 3.4 | type-check-on-connect | Refuse incompatible connections (`DataTypes::canConnect`) with a clear message | ready-for-dev |
| 3.5 | delete-with-references | Deleting a block or signal also removes the `Source` elements that point to it, in the same undo step | ready-for-dev |
| 3.6 | project-validator | `ProjectValidator`: unresolved references, type mismatches, cycle warnings; a result list in the UI | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 2 (2.1 writer).
- Blocks: Epic 6 (the validator runs before upload), Epic 7 (values drawn on wires).

---

## Epic 4: Library packages and catalogue (editor)

**Goal:** blocks, peripherals and protocols come from versioned `.stixpkg` packages through one `LibraryService`; nothing is hard-wired to `templates/`.

**In scope:**
- FR-07, NFR-06, NFR-12 [Source: architecture.md#adr-004]
- FR-12 (local part), FR-15 (the interface, ready for the controller) [Source: architecture.md#adr-012]
- FR-18 (the `ui/` folder and `Ui` attributes frozen in the format) [Source: architecture.md#adr-015]

**Architecture touchpoints:** BlockManager, SchemaManager, the new LibraryService / LocalRepository [Source: architecture.md#component-libraryservice-l3-new]

**Out of scope:** the controller repository (Epic 8) and the shop (post-MVP).

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 4.1 | manifest-and-descriptor-v1 | Define the `manifest.xml` format; parse `TickMs`, `Ui`, `Ui` ports and properties, and SemVer `Vers`; convert the templates | ready-for-dev |
| 4.2 | library-repository-interface | `LibraryRepository_class` + `LibraryService_class`; a `LocalRepository` over `templates/` and `AppLocalData/libraries` | ready-for-dev |
| 4.3 | versioned-type-catalogue | `BlockManager` / `SchemaManager` keyed by `(Uuid, Vers)`; types resolved through `<Requires>` | ready-for-dev |
| 4.4 | stixpkg-install-local | Unpack `.stixpkg`, verify the SHA-256 of each file, install side by side by version, remove | ready-for-dev |
| 4.5 | package-signature-verify | Verify the Ed25519 `SIGNATURE`; mark unsigned development packages clearly | ready-for-dev |
| 4.6 | library-manager-ui | A screen listing packages (source, version, kind) with install and remove | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 1 (1.2, 1.3).
- Blocks: Epic 2.4 (Requires needs versions; can run in parallel with a stub), Epic 5.3 (packaging), Epic 8.

---

## Epic 5: Plugin SDK and C ABI

**Goal:** a block or driver author can build a plugin against a stable, MIT-licensed SDK and package it as a `.stixpkg`.

**In scope:**
- FR-07, FR-17, NFR-01, NFR-10 [Source: architecture.md#adr-006]
- ABI v1 [Source: architecture.md#6-2-plugin-c-abi-v1]

**Architecture touchpoints:** `sdk/` (moves to its own repo when the runtime starts) [Source: architecture.md#adr-001]

**Out of scope:** the runtime that loads plugins (Epic 10).

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 5.1 | abi-header | `sdk/stix_abi.h` exactly as in §6.2 (MIT): value types, host functions, plugin table | ready-for-dev |
| 5.2 | cpp-wrapper | Header-only `stix::Block` / `stix::Driver` C++ base classes (`onInput`, `run`, `onProperty`, `emit`, `emitIfChanged`) | ready-for-dev |
| 5.3 | plugin-build-template | CMake template, toolchain files (aarch64, armv7hf, x86_64), and a packaging script → `.stixpkg` | ready-for-dev |
| 5.4 | reference-plugins | Example plugins (And, Or, Add, a virtual I/O driver) with descriptors | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 1 (1.2, since the SDK shares the tokens); 4.1 (manifest format) for 5.3.
- Blocks: Epic 10 (the runtime loads these plugins).

---

## Epic 6: Controller connection (editor side)

**Goal:** STIx Config connects securely to a controller, logs in, and uploads or downloads the project. The program then runs all the time and restarts only on upload.

**In scope:**
- FR-08, FR-09, NFR-02 [Source: architecture.md#adr-007] [Source: architecture.md#adr-008]
- STIx Link v1 [Source: architecture.md#6-1-stix-link-messages-protocol-v1]

**Architecture touchpoints:** the new `ControllerLink_class` (L3, own thread), `ControllerConnection.qml` [Source: architecture.md#component-controllerlink-l3-new]

**Out of scope:** monitoring (Epic 7), libraries (Epic 8), merge (Epic 9).

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 6.1 | link-framing-cbor | Frame codec (`uint32 len | CBOR {t,id,p}`), chunked transfers, message structs, unit tests | ready-for-dev |
| 6.2 | mock-controller | `tools/stix-mock-controller`: a development server for the Link messages, so Epics 6–9 don't wait for the runtime | ready-for-dev |
| 6.3 | controller-link-connect | `ControllerLink` on a `QThread`: TLS 1.3, `Hello`, connection states, IP and port from `RootDevice` | ready-for-dev |
| 6.4 | certificate-pinning | Pin the certificate on first connection: fingerprint dialog, trust store, re-pair flow | ready-for-dev |
| 6.5 | auth-and-session | Login dialog, `Auth`, roles; optional keychain for the token | ready-for-dev |
| 6.6 | connection-ui | `ControllerConnection.qml` bound to `ControllerLink` (state, controller info, errors) | ready-for-dev |
| 6.7 | upload-download | Chunked upload with `baseRev`, `RevConflict` hook, download into a tab, `ActivatePrevious` | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 2 (2.5 Rev), Epic 3 (3.6 validator before upload).
- Blocks: Epics 7, 8, 9.

---

## Epic 7: Live monitoring and force

**Goal:** while connected, the integrator sees live values on every port, signal and variable, and can force signals, set variables and change parameters.

**In scope:**
- FR-10, FR-11, NFR-09 [Source: architecture.md#adr-007]
- NFR-15 (fault display) [Source: architecture.md#adr-005]
- `SetParams` [Source: architecture.md#adr-017]

**Architecture touchpoints:** the new `LiveValues_class`, `Port.qml` / `Signal.qml` / `Block.qml`, `LogTab` [Source: architecture.md#component-livevalues-l3-new]

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 7.1 | symbols-and-subscribe | `GetSymbols`, map slots to `IUuid`, subscribe to what is visible on the page | ready-for-dev |
| 7.2 | live-values-model | `LiveValues_class`: the latest value plus force and fault flags, updated at most once per frame | ready-for-dev |
| 7.3 | live-values-on-canvas | Show values on ports, signals and wires, with fault markers on blocks | ready-for-dev |
| 7.4 | force-and-release | Force and release signals from the canvas; forced values clearly flagged | ready-for-dev |
| 7.5 | set-variable-and-params | `SetVariable`, and `SetParams` from the properties panel while connected | ready-for-dev |
| 7.6 | faults-panel | `Fault` push, `ListFaults`, `ClearFault` in `LogTab` | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 6 (6.1–6.5), Epic 3 (3.2, for values on wires).

---

## Epic 8: Controller libraries and dependency check

**Goal:** the editor reads the catalogue from the controller, installs or removes packages there, and before an upload makes sure the controller has exactly the versions the project requires.

**In scope:**
- FR-12, FR-13, FR-15 [Source: architecture.md#adr-012] [Source: architecture.md#adr-004]

**Architecture touchpoints:** `ControllerRepository`, `LibraryService`, the upload flow [Source: architecture.md#6-1-stix-link-messages-protocol-v1]

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 8.1 | controller-repository | `ControllerRepository` over the Link: `ListLibraries`, fetch descriptors, icons and UI files | ready-for-dev |
| 8.2 | catalogue-from-controller | `LibraryService` merges the controller and local catalogues; the UI shows where each package comes from | ready-for-dev |
| 8.3 | install-remove-on-controller | Chunked `InstallLibrary`; `RemoveLibrary` (refused while the running project uses it) | ready-for-dev |
| 8.4 | dependency-check-before-upload | `CheckProject` → missing or mismatched packages → install → upload | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 4 (4.2–4.4), Epic 6 (6.1–6.5, 6.7 for 8.4).

---

## Epic 9: Project sync and merge

**Goal:** changes made on the controller (from the HMI, or saved by blocks) are never silently lost. STIx Config is told about them, and an upload either overwrites or merges, with the engineer's values winning.

**In scope:**
- FR-22 [Source: architecture.md#adr-017]

**Architecture touchpoints:** the new `ProjectSync` (L3), Project undo commands [Source: architecture.md#component-projectsync-l3-new]

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 9.1 | project-changed-notice | Handle the `ProjectChanged` push: "edited on the controller (rev N), download?" | ready-for-dev |
| 9.2 | three-way-merge-engine | A pure function (Base, Mine, Controller) → change set, by `IUuid` and attribute; the engineer wins; a summary | ready-for-dev |
| 9.3 | overwrite-or-merge-flow | The `RevConflict` dialog: Overwrite (double confirmation) or Merge (applied as an undo macro), then re-upload | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 2 (2.5), Epic 6 (6.7).

---

## Epic 10: STIx Runtime MVP (separate repository `stix-runtime`)

**Goal:** a Linux controller that loads the project as object instances, runs it with direct callbacks and ticks, serves the STIx Link, saves parameter changes, and contains faults.

**In scope:**
- FR-09, FR-10, FR-11, FR-17, FR-22, NFR-01, NFR-11, NFR-15 [Source: architecture.md#adr-005] [Source: architecture.md#adr-017]

**Architecture touchpoints:** Loader, RuntimeManager, DriverHost, LinkServer, PackageStore, Persistence, stix-supervisor [Source: architecture.md#stix-runtime-components-contract-level]

**Stories (ordered):**

| ID | Slug | Intent | Status |
|------|------|--------|--------|
| 10.1 | runtime-skeleton | Repository, CMake, pugixml, tinycbor and OpenSSL; CI cross-build for the 3 targets | ready-for-dev |
| 10.2 | package-store | Install, verify, list and remove packages, side by side by version | ready-for-dev |
| 10.3 | loader-instances | Parse the XML, check `Requires`, `dlopen`, create instances with their XML-node pointers, build the `IUuid → instance` map | ready-for-dev |
| 10.4 | subscriber-wiring | SourceIds, slots, subscriber arrays, `bind()` | ready-for-dev |
| 10.5 | dispatch-busy-mailbox | Inline `emit` / `emit_signal`, the `busy` flag and mailbox, the loop guard | ready-for-dev |
| 10.6 | tick-scheduler | Timer wheel for `run()` every `TickMs` | ready-for-dev |
| 10.7 | driver-host | Driver threads, `open`/`close`/`write`, the SDK queue adapter | ready-for-dev |
| 10.8 | link-server | TLS server, framing, `Hello` / `Auth` / RBAC, `GetInfo` | ready-for-dev |
| 10.9 | upload-swap-persistence | `UploadProject` with `baseRev` / `RevConflict`, atomic swap, previous and Base copies, `ActivatePrevious` | ready-for-dev |
| 10.10 | monitoring-server | Symbols, `Subscribe`, `ValueBatch` every 50 ms, `Force` / `Release`, `SetVariable` | ready-for-dev |
| 10.11 | set-params-and-save | `SetParams` / `HmiSetParams` → instance + XML node; debounced atomic save, `Rev`, `ProjectChanged` | ready-for-dev |
| 10.12 | supervisor | `stix-supervisor`: shared-RAM records, crash and hang detection, quarantine, `Fault` | ready-for-dev |
| 10.13 | digital-io-driver | A GPIO digital input/output driver plugin (Linux `gpiod`) for real hardware | ready-for-dev |

**Cross-epic dependencies:**
- Blocked by: Epic 5 (ABI and reference plugins); the Link spec in architecture.md (6.1 can be shared as a reference).
- Can run in parallel with Epics 6–9, which use the mock controller (6.2).

---

## After the MVP (not broken into stories yet)

- FR-14: STIx Library Shop [Source: architecture.md#adr-012]
- FR-18 (preview part): `UiPreview` in the editor [Source: architecture.md#adr-015]
- FR-19, FR-21: the STIx HMI app and its sync on open [Source: architecture.md#adr-015]
- FR-20: the STIx Remote relay [Source: architecture.md#adr-016]

---

## Delivery Tracking (count-based)

There are no story points, velocity or burndown; progress is tracked by count only.

- Total stories: 61 (STIx Config 48 · STIx Runtime 13)
- Ready for dev: 61
- Done: 0
- Remaining: 61
- Completion rate: 0 / 61

## Notes

- **Order:** Epic 1 → 2 → (3 ∥ 4 ∥ 5) → 6 → (7 ∥ 8 ∥ 9). Epic 10 can start once Epic 5 is done, and runs alongside 6–9.
- **The mock controller (6.2)** removes the dependency between the editor and the runtime: the editor can be finished and tested before the runtime exists.
- **Build files are auto-discovered (story 1.1):** sources, QML files and tests are picked up automatically, and each optional dependency has its own `cmake/modules/<name>.cmake`. Later stories therefore never edit `CMakeLists.txt`, which keeps them parallel-safe.
- **Ordering edges added by the scope check:** Epic 1 must be complete (1.3, 1.5, 1.7) before 2.1, 4.1, 5.1 and 6.1 start; 2.4 after 2.2; 3.1 after 2.5; 3.4 after 3.3; 4.3 after 3.6; 6.3 after 4.2; 6.7 after 4.6; 7.4 after 7.3; 8.3 after 8.2; 9.1 after 6.6. After these, no two stories that can run at the same time touch the same file.
- **Critical path:** 18 stories for STIx Config, 12 for the runtime.
- **Biggest risks:** 10.5 (concurrency: busy flag + mailbox) and 10.12 (supervisor). Write a multi-thread stress test and fault-injection plugins early [Source: architecture.md#testing-strategy-planning-only].
- **No `prd.md` exists.** Requirement IDs refer to the baseline in architecture.md §0.
