# Decision Log

Cross-workflow decisions, newest last. Full rationale lives in the referenced document.

| Date | ID | Decision | Source | Status |
|------|----|----------|--------|--------|
| 2026-11-05 | ADR-001 | Three products (Config, Runtime, Shop) coupled only by versioned contracts; Config is a layered modular monolith (L1 Foundation → L4 QML) | architecture.md | Accepted |
| 2026-11-05 | ADR-002 | The XmlNode tree is the only in-memory model; all edits go through the 4 undo primitives (Insert/Remove/Move/SetAttribute) | architecture.md | Accepted |
| 2026-11-05 | ADR-003 | Project XML: readable style rules, SemVer `ConfigVers` with a migration chain, generated `<Requires>` header | architecture.md | Accepted |
| 2026-11-05 | ADR-004 | Library package `.stixpkg` = manifest + Descriptor + per-target `.so` + Ed25519 signature; identity = Uuid + SemVer + SHA-256 | architecture.md | Accepted |
| 2026-11-05 | ADR-005 | Runtime compiles XML once into a flat slot/pointer graph; event-driven, rank-ordered propagation; atomic swap on upload | architecture.md | Superseded by ADR-005 rev. 1.1 |
| 2026-11-05 | ADR-006 | Plugins use a versioned C ABI (`stix_plugin_entry`); runtime owns memory; port order = Descriptor order | architecture.md | Superseded by ADR-006 rev. 1.1 |
| 2026-11-05 | ADR-007 | STIx Link = TLS 1.3/TCP, `uint32 len + CBOR {t,id,p}`; request/response + push; `ValueBatch` monitoring by slot | architecture.md | Accepted |
| 2026-11-05 | ADR-008 | Controller certificate pinned on first connect (TOFU); Argon2id accounts; RBAC Admin/Engineer/Viewer; only signed packages | architecture.md | Accepted |
| 2026-11-05 | ADR-009 | C++ owns all state; QML binds to properties and calls `cmd*`; live values in a separate model, never in the document | architecture.md | Accepted |
| 2026-11-05 | ADR-010 | No exceptions across boundaries; `bool` / `Result_s{ErrorCode_e}`; log once with `S_Logger` + module name | architecture.md | Accepted |
| 2026-11-05 | ADR-011 | Naming: `X_class`, `X_s`, `X_e`, `S_X`, `m_<kind>_X`; XML PascalCase via tokens headers only; SemVer everywhere | architecture.md | Accepted |
| 2026-11-05 | ADR-012 | `LibraryRepository` interface (Local → Controller → Shop); the editor brokers packages to the controller | architecture.md | Accepted |
| 2026-11-05 | ADR-013 | CMake + Qt 6.11, C++17, CI on Windows/macOS/Linux; OS code only in `src/Platform/` | architecture.md | Accepted |
| 2026-11-05 | ADR-014 | Licensing: Config GPL-3.0, Runtime LGPL-3.0/Apache-2.0, SDK MIT, packages under their own licence | architecture.md | Accepted |
| 2026-11-05 | ADR-005 rev. 1.1 | Owner's model: XML wired once into per-source subscriber arrays of direct `on_input` callbacks; one RuntimeManager thread; `run()` every `TickMs` from the descriptor; periphery drivers on their own threads publish into a lock-free queue; recursion/hang/crash contained by depth guard, watchdog, crash breadcrumb and quarantine | architecture.md | Superseded by ADR-005 rev. 1.2 |
| 2026-11-05 | ADR-006 rev. 1.1 | ABI: `bind(sources per input)`, `on_input(input, SourceId, value)`, `run(now)`, `host->emit`; fan-in semantics decided by each block | architecture.md | Superseded by ADR-006 rev. 1.2 |
| 2026-11-05 | ADR-003 note | `ConfigVers 1.0.0` baseline allows several `<Source>` per `<Input>` | architecture.md | Accepted |
| 2026-11-05 | ADR-005 rev. 1.2 | Drivers call `emit_signal` (callbacks inline on the driver thread); runtime serialises each instance with an atomic busy flag + mailbox (no locks, no deadlock); crash/hang detection by `stix-supervisor` via shared RAM, so no flash writes except on faults; unsynchronised signals are intended behaviour | architecture.md | Accepted |
| 2026-11-05 | ADR-006 rev. 1.2 | All host functions thread-safe; blocks may run on any thread but never two at once per instance; driver `write` must be thread-safe and non-blocking | architecture.md | Accepted |
| 2026-11-05 | NFR targets | NFR-01/08/09 performance targets confirmed by the owner | architecture.md | Accepted |
| 2026-11-05 | ADR-005/006 rev. 1.3 | `publish` removed: `emit_signal` is the single way to send a signal value (driver threads and RuntimeManager); no runtime event queue; heavy drivers may use their own extra thread | architecture.md | Accepted |
| 2026-11-05 | FR-09 change | No start/stop: the program always runs and restarts only when a new project is uploaded; `Start`/`Stop` removed from STIx Link, `ActivatePrevious` added for rollback | architecture.md | Accepted |
| 2026-11-05 | FR-15 change | The library catalogue read from the controller is an MVP requirement (`ControllerRepository` in the MVP) | architecture.md | Accepted |
| 2026-11-05 | Instance model | Every block, protocol and driver in the project is its own object instance with its own parameters and local variables | architecture.md | Accepted |
| 2026-11-05 | ADR-015 | Block HMI UI: optional `ui/Tile.qml` + `ui/Detail.qml` in the package, sandboxed QML with a single `stixBlock` API, generic fallback UI; fourth product STIx HMI (Qt Quick, Android/iOS/desktop); `HmiUser` role from project Users/Rights; iOS App Store rule to check before release | architecture.md | Accepted |
| 2026-11-05 | ADR-016 | Remote access via STIx Remote relay: controller opens an outbound connection, registers by serial number + claim code, heartbeat every 30 s; relay joins app ↔ controller byte for byte; STIx Link and its TLS run end to end, so the relay cannot read data; local IP first, relay as fallback | architecture.md | Accepted |
| 2026-11-05 | ADR-015 sync | HMI on open: `HmiGetProject {knownHash}` → unchanged, or a filtered **HMI view** XML (no secrets, Rights-filtered) → compare `<Requires>` with the UI cache (Uuid + Vers + Sha256) → download missing/other-version `ui/` files from the controller | architecture.md | Accepted |
| 2026-11-05 | Startup order | XML read once at startup: extract instances as objects → load each instance's parameters → connect its callbacks | architecture.md | Accepted |
| 2026-11-05 | ADR-017 | Controller may change **parameter values** at run time (descriptor `RuntimeEdit="Hmi"` / `"Engineer"`; also the block itself via `set_property`): applied live via `on_property`, saved into `Project.xml` (debounced ≥ 10 s, atomic write, `Rev` incremented), change journal kept; structure changes only by upload; STIx Config compares `BaseRev` with `Rev` before every upload and takes controller changes in by default | architecture.md | Superseded by ADR-017 rev. 1.1 |
| 2026-11-05 | ADR-017 rev. 1.1 | Owner's concept: app sends `{iUuid, <XmlAttr>: value}`; the controller applies it to the instance and writes it through the instance's XML-node pointer (also for state saved by blocks); debounced atomic save, `Rev`++; `ProjectChanged` tells STIx Config to download; upload with older `baseRev` → engineer chooses Overwrite or three-way Merge (Base/Mine/Controller by `IUuid` + attribute; conflicts only when both changed the same attribute) | architecture.md | Accepted |
| 2026-11-05 | ADR-017 merge rule | Same attribute changed by both engineer and user → the engineer's value wins; the merge is fully automatic and shows a summary | architecture.md | Accepted |
| 2026-11-05 | ADR-007 encoding | STIx Link payloads stay CBOR (binary JSON, same keys as the `{iUuid, Attr: value}` object) | architecture.md | Accepted |
| 2026-11-05 | Epics & stories | 10 epics, 61 stories (48 STIx Config, 13 STIx Runtime) in `bmad-output/epics.md` and `bmad-output/stories/`; all `ready-for-dev`; HMI, relay, shop and UI preview stay after the MVP | epics.md | Accepted |
| 2026-11-05 | Build auto-discovery | Story 1.1: sources, QML and tests collected automatically (`GLOB_RECURSE CONFIGURE_DEPENDS`), optional dependencies in `cmake/modules/*.cmake`, `tools/*` and `sdk/tests` auto-added, so stories don't contend on `CMakeLists.txt` | stories/1.1 | Accepted |
| 2026-11-05 | Descriptor normalisation | Fix the 10 template descriptors (`ExtName`→`Title`, `ShortTitle`→`ShortName`, `Type`→`DataType`) rather than the reader, because these are the contract names | stories/1.3 | Accepted |
| 2026-11-05 | STIx Link `GetLibraryFile` | Additive message `{uuid, vers, path}` → chunked file, role Viewer; needed for the catalogue read from the controller (FR-15) | architecture.md | Accepted |
| 2026-11-05 | ProjectSync exposure | `ProjectSync` is reached as `ControllerLink.sync`, not as a new `main.cpp` singleton | stories/9.1 | Accepted |
