# System Architecture: STIx Platform (STIx Config · STIx Runtime · STIx HMI · STIx Remote · STIx Library Shop)

**Document Version:** 1.9
**Date:** 2026-11-05
**Author:** Smart Tech Innovation
**Track:** BMad Method (MVP, single developer)
**Status:** Draft
**Source PRD:** none yet. The requirements baseline in §0 comes from the stakeholder interview of 2026-11-05 and the existing `STIx_Config` codebase (28 commits, up to `a8f2571`).

> This is the single source of truth for cross-cutting technical decisions. Every
> story inherits the LOCKED decisions recorded here. Catching alignment at this layer
> is ~10x cheaper than during implementation.

---

## Table of Contents

0. [Requirements Baseline](#0-requirements-baseline)
1. [System Overview](#1-system-overview)
2. [Architecture Pattern](#2-architecture-pattern)
3. [Architecture Decision Records](#3-architecture-decision-records)
4. [Component Design](#4-component-design)
5. [Data Model](#5-data-model)
6. [API Specifications](#6-api-specifications)
7. [FR / NFR Coverage Matrix](#7-fr--nfr-coverage-matrix)
8. [Technology Stack](#8-technology-stack)
9. [Trade-off Analysis](#9-trade-off-analysis)
10. [Deployment Architecture](#10-deployment-architecture)
11. [Future Considerations](#11-future-considerations)

---

## 0. Requirements Baseline

There is no `prd.md` yet, so these IDs are the reference set. If a PRD is written later, it should reuse these IDs.

### Functional requirements

| ID | Requirement |
|----|-------------|
| FR-01 | Create, open, save and "save as" projects. Several projects are open as tabs, with a list of recent projects. |
| FR-02 | A project tree driven by the schema configures the controller: digital and virtual I/O, physical interfaces (RS485, CAN, I2C…), network protocols (Modbus TCP, MQTT), messages, rooms, users and variables. |
| FR-03 | Visual programming on pages (A4–A0): place blocks, input/output signal references and variables by drag and drop. |
| FR-04 | Connect a block output to block inputs or signal outputs, with data-type compatibility checks. |
| FR-05 | A properties panel generated from the schema and block descriptors (groups, enums, references, units, read-only flags). |
| FR-06 | Undo and redo for every document change. |
| FR-07 | Blocks, peripherals and protocols are **library packages**, each with a descriptor (for the editor) and a native binary (for the controller). |
| FR-08 | Connect to a controller over an encrypted TCP link. |
| FR-09 | Upload the project XML to the controller and download it back. The program runs all the time: there is no start or stop. It restarts only when a new project is uploaded (atomic swap, ADR-005). |
| FR-10 | Monitor live values of every signal, every variable, and every block input and output. |
| FR-11 | Simulate and force signals, and change variables, in real time. |
| FR-12 | List the libraries installed on the controller, and install, update or remove them. |
| FR-13 | The project declares the libraries it requires, with **exact versions**. Before an upload, these are compared with the controller's libraries and any missing ones are installed. |
| FR-14 | Library Shop: browse, search, buy (paid items), download and update library packages. |
| FR-15 | The editor reads the library catalogue (descriptors, icons, UI files) from the connected controller, as well as from the local PC. |
| FR-16 | Multi-language UI (EN, RU, RO, ES). |
| FR-17 | The controller runtime parses the project XML and runs it as a native program, using the library binaries. |
| FR-18 | A block package may include its own HMI UI, written in QML, through which end users interact with that block. |
| FR-19 | STIx HMI: a separate mobile and tablet app where end users watch and operate the project's blocks by room and category, with per-user rights. |
| FR-20 | Remote access: a remote connection server acts like a VPN. Controllers registered by serial number push heartbeats to it, and on request it connects the app to the controller. The data is exactly the same as on a local connection. |
| FR-21 | When STIx HMI opens, it compares its cached project hash with the controller's. If the project changed, it fetches the XML, extracts the UI layout from it, checks the UI dependencies, and downloads from the controller any UI component that is missing or in another version. |
| FR-22 | Parameters can change on the controller: the HMI app sends `{iUuid, <AttributeName>: value}`, and the controller applies it to the instance and to that instance's XML node (also when a block saves its own state). STIx Config is told the project was edited and can download it. An upload over such changes asks the engineer to Overwrite or Merge. |

### Non-functional requirements

| ID | Category | Requirement (targets confirmed by the owner, 2026-11-05) |
|----|----------|------------------------------------------------------------|
| NFR-01 | Performance (runtime) | No interpreter at run time. Native code, direct callbacks through pointer arrays, minimal CPU. *Target:* < 1 ms from a driver event to the output signal through a chain of 1,000 callbacks on a Cortex-A53-class CPU; idle CPU < 2 %; tick jitter ±10 ms. |
| NFR-02 | Security | Encrypted, authenticated config link (TLS). Only signed library packages may be installed on a controller. |
| NFR-03 | Compatibility | Every project file written by an older STIx Config release opens in newer releases. |
| NFR-04 | Portability | STIx Config runs on Windows, macOS and Linux (x86-64; macOS also arm64). |
| NFR-05 | Usability / maintainability | Project XML is easy for a person to read and edit by hand when debugging. |
| NFR-06 | Data integrity | Library identity is UUID plus exact version, checked with a hash. A project never runs against a different library build than the one it declares. |
| NFR-07 | Maintainability | The code is very clearly written and well structured: one convention set, clear module boundaries, a single developer can keep track of it. |
| NFR-08 | Performance (editor) | *Target:* 60 fps pan and zoom with 500 blocks on a page; a 5 MB project opens in < 1 s. |
| NFR-09 | Performance (monitoring) | *Target:* ≤ 100 ms from a value change on the controller to the change on screen; monitoring costs < 5 % CPU on the controller. |
| NFR-10 | Licensing / business | STIx Config and the runtime are open source (community edition). Paid, closed-source library packages must be possible. |
| NFR-11 | Reliability | Uploads are atomic: the controller keeps the last good configuration and rolls back on failure. |
| NFR-12 | Extensibility | New blocks and peripherals are added by dropping in a package, without recompiling STIx Config. |
| NFR-13 | Observability | Errors go to one logger, with the module name and severity. The runtime's errors reach the editor. |
| NFR-14 | Scalability | The design supports from 1 to N controllers per project in the future, but the MVP targets 1 controller (`RootDevice`) per project. |
| NFR-15 | Reliability (fault containment) | A block that loops endlessly, hangs or crashes is detected and quarantined, and reported to STIx Config with its instance; the rest of the program keeps running. |

---

## 1. System Overview

### Purpose
STIx is a visual programming platform for building and home automation. An integrator draws the control logic in **STIx Config** as blocks, signals and variables on pages. The result is saved as one human-readable **Project XML** file. That file is uploaded to a controller, where **STIx Runtime** wires it into a native graph of direct callbacks and runs it, loading one native library per block, peripheral or protocol type. Library packages are distributed through the **STIx Library Shop**. End users operate the running installation from phones and tablets with **STIx HMI**, through UIs shipped inside the block packages: locally by IP, or from anywhere through the **STIx Remote** relay.

**Everything on the controller is an object instance.** Each block, protocol and driver placed in the project becomes its own instance when the project loads, like an object. It has its own parameters (the `Property` values from the XML) and its own local variables (its internal state, owned by the plugin). Two `Garage_Gate` blocks share the plugin code, but nothing else. A Modbus TCP driver configured for two devices is two instances, each with its own connection, thread and channels.

```
 ┌──── Integrator PC (Win / macOS / Linux) ───────────────┐      ┌──── STIx Library Shop (cloud) ───┐
 │ STIx Config (Qt 6 / QML, this repo)                    │ HTTPS│ Catalogue · Accounts · Licences  │
 │  reads Descriptor.xml, icons, ui/*.qml (preview)       │ ◄──► │ Signing · Package storage        │
 │  never loads plugin binaries                           │      └──────────────────────────────────┘
 └────────────────────────────────────────────────────────┘
                              │ STIx Link (TLS 1.3 / TCP): upload, monitor, force, libraries
                              ▼
 ┌──── Controller (Linux) ────────────────────────────────┐      ┌──── STIx HMI (phone / tablet) ───┐
 │ STIx Runtime                                           │ Link │ Qt Quick: Android / iOS / kiosk  │
 │  Link server · Package store (signed)                  │ ◄──► │ rooms → block tiles → detail     │
 │  Loader: XML → instances + callbacks                   │ (LAN)│ UI from each package ui/*.qml    │
 │  RuntimeManager (ticks) + driver threads               │      └──────────────────────────────────┘
 │  stix-supervisor (crash / hang watch)                  │
 │  Plugins: *.so via C ABI, one object each              │
 └────────────────────────────────────────────────────────┘
                              │ outbound: register, heartbeat, tunnels            │ remote Link
                              ▼                                                   ▼
 ┌──── STIx Remote (cloud relay, optional) ─────────────────────────────────────────────────────────┐
 │ controllers registered by serial number · heartbeat every 30 s                                   │
 │ joins app ↔ controller byte for byte; TLS stays end to end (relay cannot read data)              │
 └──────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### Scope
**In scope (this document):**
- STIx Config, in full detail (this repository).
- The **contracts** between the five products: the Project XML format, the library package format (including the block UI), the plugin C ABI, the `STIx.Hmi` QML API, the STIx Link protocol, the relay protocol, and the Shop REST API.
- The runtime execution model, specified to the level where the XML format and ABI can be frozen.

**Out of scope:**
- Implementing the controller runtime, the HMI app, the relay and the shop (separate repositories, planned later).
- Controller hardware and OS image design.

### MVP slice
1. Editor: FR-01…FR-07 and FR-16, using the local library folder and the controller's library store (FR-15).
2. Frozen v1 contracts: Project XML `ConfigVers 1.x`, package format v1 (including the `ui/` folder, FR-18), ABI v1, Link protocol v1.
3. A minimal runtime on Linux: load, wire and run; Link server with upload (which restarts the program), monitor and force (FR-08…FR-11, FR-17).
4. Library management on the controller, the catalogue read from the controller, and the dependency check (FR-12, FR-13, FR-15).
5. **Post-MVP:** the Shop (FR-14), the STIx HMI app (FR-19, FR-21) and the STIx Remote relay (FR-20).

### Architectural drivers
1. **NFR-01: native, uninterpreted runtime.** This rules out running the XML directly. XML is only the *transport and source* format, read once at startup in three steps (ADR-005, ADR-006):
   1. **Extract instances as objects:** for each block, protocol and driver in the XML, the runtime creates one instance of its plugin.
   2. **Load each instance's parameters** from its XML element (`Property` values, title, room…). Each instance therefore works on its own: two instances of the same block type can behave differently.
   3. **Connect that instance's callbacks:** its inputs are added to the subscriber arrays of the sources wired to them, and its outputs get their own subscriber arrays.

   After startup, only the objects and their callbacks run; the fast path never reads the XML. Each instance keeps a pointer to its own XML node, so a parameter changed at run time (from STIx HMI, or saved by the block itself) is written straight into that node, and the project is saved if it was edited (ADR-017).
2. **NFR-06 / FR-13: exact library versions.** Package identity and the project's `<Requires>` header become part of the file format (ADR-003, ADR-004).
3. **NFR-02: secured link and signed packages.** These shape the Link protocol and the package format (ADR-007, ADR-008).
4. **NFR-03 + NFR-05: backward compatibility and readable XML.** These freeze the XML style rules and require a migration chain (ADR-003).
5. **NFR-07: clarity for a single developer.** This decides a modular monolith with strict layering and one naming convention (ADR-001, ADR-011).
6. **NFR-10: open-source core with paid plugins.** This requires a C ABI and a permissive SDK licence (ADR-006, ADR-014).

### Stakeholders and constraints
- **Users:** installers and integrators of building automation, and advanced home owners.
- **Team:** 1 developer, strong in C++/Qt. Fewer moving parts beats extra power (rubric: operational simplicity).
- **Existing constraints:** Qt 6.11, C++17, CMake, QML Basic style. The XML document model and undo-command architecture already exist and work.

---

## 2. Architecture Pattern

**Pattern:** a **layered modular monolith** for STIx Config, with a **load-and-wire, then callback-driven** execution model in STIx Runtime. The products exchange data only through versioned contracts.

**Justification:**
- With one developer and an MVP, a single desktop executable with internal module boundaries gives most of the clarity at almost no operational cost.
- The runtime must be fast, so every connection is resolved once at load into arrays of function pointers. Execution is then only direct callbacks plus a periodic tick. See ADR-005.
- The products have different release cycles and platforms, so they are coupled only through file and wire contracts, never through shared code at run time. The exception is the small `stix-sdk` header set: tokens, ABI and value types.

**Alternatives considered:**
- **Plugin-loading editor** (the editor loads block DLLs to draw or simulate them). Rejected: the editor would need a binary for each of 3 desktop OSes for every package, it would run untrusted code on the PC, and paid plugins would leak. The editor uses descriptors only.
- **Microservices / backend for the editor.** Rejected: no NFR needs it. It is an offline-capable desktop tool.

**Application:** STIx Config is split into four layers. Dependencies point **downwards only**.

```
 L4  UI (QML)            resources/qml/**            view only, no business rules
 L3  App services        ProjectManager, ControllerLink, LibraryService, ShopClient
 L2  Domain              Project (+Page, Tree, commands), SchemaManager, BlockManager, DataTypes
 L1  Foundation          Document (XmlNode, tokens), Logger, Setup/paths, stix-sdk headers
```

---

## 3. Architecture Decision Records

| ADR | Title | Status | Drives |
|-----|-------|--------|--------|
| ADR-001 | Five products, contract-coupled; STIx Config is a layered modular monolith | Accepted | NFR-07, NFR-04 |
| ADR-002 | One XML document tree is the in-memory model; all edits go through undo commands | Accepted | FR-01…FR-06, NFR-03 |
| ADR-003 | Project XML format: readable style rules, `ConfigVers` migrations, `<Requires>` header | Accepted | NFR-03, NFR-05, FR-13 |
| ADR-004 | Library package format `.stixpkg` with exact identity (UUID + SemVer + SHA-256) | Accepted | FR-07, FR-12, NFR-06, NFR-12 |
| ADR-005 | Runtime: XML wired once into direct-callback arrays; drivers call callbacks inline; per-instance serialisation; per-type tick; supervisor-based fault quarantine | Accepted (rev. 1.2) | FR-17, NFR-01, NFR-15 |
| ADR-006 | Plugin interface is a versioned C ABI (`on_input(input, source, value)`, `run`, `emit`) | Accepted (rev. 1.1) | FR-07, FR-17, NFR-01, NFR-10 |
| ADR-007 | STIx Link: TLS 1.3 over TCP, length-prefixed CBOR messages | Accepted | FR-08…FR-13, NFR-02, NFR-09 |
| ADR-008 | AuthN/AuthZ and trust: pinned device certificates, role-based accounts, signed packages | Accepted | NFR-02 |
| ADR-009 | State management: C++ owns all state; QML binds to properties and calls `cmd*` methods | Accepted | NFR-07, NFR-08 |
| ADR-010 | Error, result and logging convention | Accepted | NFR-13, NFR-07 |
| ADR-011 | Naming and code conventions (C++, QML, XML) | Accepted | NFR-07, NFR-05 |
| ADR-012 | Library sources behind one `LibraryRepository` interface (Local + Controller in the MVP, then Shop) | Accepted | FR-14, FR-15 |
| ADR-013 | Build and platforms: CMake + Qt 6.11, CI on 3 OSes, platform code isolated | Accepted | NFR-04 |
| ADR-014 | Licensing split: core GPL-3.0, SDK/ABI headers MIT, packages under their own licence | Accepted | NFR-10 |
| ADR-015 | Block HMI UI: optional QML in the package (`ui/`), sandboxed, shown by the STIx HMI app; hash-based sync on open | Accepted | FR-18, FR-19, FR-21, NFR-02 |
| ADR-016 | Remote access through the STIx Remote relay: registered by serial number, heartbeats, end-to-end TLS | Accepted | FR-20, NFR-02 |
| ADR-017 | Parameters edited on the controller: `{iUuid, Attr: value}` → instance + its XML node (pointer); `Rev`; STIx Config asks Overwrite or three-way Merge on upload | Accepted (rev. 1.1) | FR-22 |

---

### ADR-001: Five products, contract-coupled; STIx Config is a layered modular monolith

**Status:** Accepted  **Drives:** NFR-07, NFR-04

**Context:** One developer is building an editor, a controller runtime, an HMI app, a remote relay and a shop. Shared code between them would couple their release cycles, and the editor already exists as a single Qt executable.

**Decision:**
- There are five deliverables: **STIx Config** (this repo), **STIx Runtime** (separate repo, Linux), **STIx HMI** (separate repo, Qt Quick for Android, iOS and desktop; ADR-015), **STIx Remote** (separate repo, cloud relay; ADR-016) and **STIx Library Shop** (separate repo, web).
- They share only the **`stix-sdk`** directory: plain C/C++ headers holding XML tokens, value types and the plugin ABI. It starts inside this repo at `sdk/` and moves to its own repo once the runtime starts.
- Inside STIx Config the layers are L1…L4 (§2). A file in layer N may include only files from layers ≤ N. QML (L4) talks only to objects registered from L3 or L2.

**Consequences, LOCKED for all stories:**
- No component includes a header from a higher layer. Domain classes never include anything from `QtQuick`.
- The editor never `dlopen`s or `LoadLibrary`s a plugin binary.
- Easier: each product can be released and tested on its own. Accepted cost: contracts need explicit versioning (ADR-003, ADR-006, ADR-007).

**Alternatives:** a single monorepo with a shared runtime core linked into the editor for simulation. Rejected for now; see §11 "Offline simulation".

**Revisit when:** offline simulation in the editor becomes a requirement.

---

### ADR-002: One XML document tree is the in-memory model; all edits go through undo commands

**Status:** Accepted (formalises existing code)  **Drives:** FR-01…FR-06, NFR-03

**Context:** `Project_class` already holds the project as a tree of `XmlNode_class` nodes. It indexes them by `IUuid` and changes them only through `InsertNodeCmd`, `RemoveNodeCmd`, `MoveNodeCmd` and `SetAttributeCmd`. Views (tree, pages, properties) react to `nodeInserted`, `nodeRemoved` and `attributeChanged`. This gives undo for free and keeps unknown tags.

**Decision:**
- The **only** source of truth for an open project is its `XmlNode_class` tree. There are no parallel C++ domain objects that hold copies of the data. View models such as `Page_class` and `ProjectTree_class` are *caches* rebuilt from signals.
- The **only** mutation path is a `QUndoCommand` built from the 4 primitives. Compound operations (place block, connect, delete with references) are `QUndoStack` macros made of primitives.
- Every entity has a unique `IUuid` (instance) and, where it has a type, a `Uuid` (type). References always point to `IUuid` values (`Ref`, `Io`).
- Unknown elements and attributes are kept 1:1 on save.

**Consequences, LOCKED for all stories:**
- Never call `XmlNode_class::setValue`, `insertChild` or `takeChild` on a node that belongs to the document from outside the 4 command classes.
- Every new feature that edits data adds a `cmd*` method on `Project_class` (or a helper that builds a macro) and **no** new primitive.
- Deleting a node must also remove or clear the references to it (`Ref`, `Io`) inside the same macro, so undo restores everything.
- Accepted cost: the code works with string-typed attributes. Mitigation: typed accessors in `SchemaManager` / `DataTypes`, and the tokens header (ADR-011).

**Alternatives:** a typed C++ domain model with XML serializers. Rejected: it would duplicate the schema in C++, which contradicts NFR-12, and it would lose unknown data (NFR-03).

**Revisit when:** profiling shows the attribute hash lookups break NFR-08.

---

### ADR-003: Project XML format: readable style rules, `ConfigVers` migrations, `<Requires>` header

**Status:** Accepted  **Drives:** NFR-03, NFR-05, FR-13, NFR-06

**Context:** The file must be readable and editable by hand (NFR-05), must keep opening across versions forever (NFR-03), and must state exactly which libraries it needs (FR-13).

**Decision:**
1. **Encoding and style:** UTF-8, 4-space indent, one element per entity, scalar values as attributes, no base64 or binary blobs, no default namespaces. The writer emits attributes in a **stable order** (`IUuid`, `Uuid`, `Title`, then the others in schema order) so files diff cleanly in git.
2. **Identity:** `Project/@Rev` is the revision number, which the controller increments on every save (ADR-017). `IUuid` values are lowercase RFC-4122 strings without braces. Coordinates (`Px`, `Py`) are integers in workspace units (`WORKSPACE_SCALE`).
3. **Versioning:** `Project/@ConfigVers` is SemVer `MAJOR.MINOR.PATCH`.
   - MINOR or PATCH: only additive changes. Older editors still open the file and keep the unknown parts (ADR-002).
   - MAJOR: a structural change. The editor contains an ordered **migration chain** (`Migration_1_to_2`, …). Each migration is a pure function on the `XmlNode_class` tree and runs on load.
   - The editor always **writes** the current version and keeps a backup `Project.xml.bak-<oldVersion>` before it first overwrites an older file.
   - A file with a newer MAJOR version than the editor supports opens **read-only**, with a clear warning.
4. **Dependency header:** `Project/Requires/Library` lists every package used, with `Uuid`, `Name`, `Kind`, `Vers` and `Sha256` (see §5). It is **generated on save** from the block and periphery types actually used. It is never maintained by hand, and the editor regenerates it if a hand edit makes it stale.
5. **Baseline:** no format has been released yet, so `ConfigVers 1.0.0`, frozen at the MVP release, already includes `<Requires>` and **several `<Source>` elements per `<Input>`** (many-to-many, ADR-005).
6. **Golden files:** every released `ConfigVers` has a sample project committed under `tests/fixtures/projects/<version>/`, and the testing strategy requires each one to open.

**Consequences, LOCKED for all stories:**
- No feature may remove or rename an element or attribute without a MAJOR bump and a migration.
- No feature may store derived or binary data in `Project.xml`. View state (`viewX`, `viewY`, `viewScale`) stays out of the file, as it does today.
- Easier: reviewing in git, debugging by hand. Accepted cost: XML is larger and slower to parse than binary. Mitigation: parsing happens once; the runtime wires it into callback arrays in memory (ADR-005).

**Alternatives:** JSON (less readable for deep trees with attributes, no comments); a binary format (fails NFR-05); embedding the version per element (too noisy).

**Revisit when:** project files regularly exceed 20 MB.

---

### ADR-004: Library package format `.stixpkg` with exact identity (UUID + SemVer + SHA-256)

**Status:** Accepted  **Drives:** FR-07, FR-12, FR-13, NFR-06, NFR-12

**Context:** Blocks and peripherals already exist as folders with `Descriptor.xml`, `icon.svg`, `lib/` and `src/` (e.g. `templates/blocks/Access/Garage_Gate`). The editor needs the descriptor, the controller needs the binary, the shop needs to sell and sign them, and NFR-06 requires exact versions.

**Decision:**
- A **package** is a zip archive named `<Name>-<Vers>.stixpkg` with this layout:
  ```
  manifest.xml              ← identity, kind, ABI, targets, licence, dependencies
  Descriptor.xml            ← editor-facing description (ports, properties, docs) – current format
  icon.svg
  docs/                     ← optional
  ui/Tile.qml, ui/Detail.qml← optional HMI components (ADR-015)
  lib/linux-aarch64/libstix_<name>.so
  lib/linux-armv7hf/libstix_<name>.so
  lib/linux-x86_64/libstix_<name>.so
  SIGNATURE                 ← Ed25519 signature over manifest + SHA-256 of every file
  ```
- **Kinds:** `Block`, `Periphery` (a hardware channel or interface), `Protocol` (a network driver such as Modbus TCP or MQTT).
- **Identity** = `Uuid` (stable for the life of the type) + `Vers` (SemVer **MAJOR.MINOR.PATCH**) + `Sha256` (of the canonical zip). Two packages are the same only if all three match.
- `manifest.xml` declares `AbiVersion` (ADR-006) and the supported `Target` triples.
- The root element of a block's `Descriptor.xml` declares `TickMs` (the period of `run()`; `0`, the default, means never ticked). See ADR-005.
- In the editor, a package installed locally is unpacked to `AppLocalDataLocation/libraries/<Kind>/<Uuid>/<Vers>/`. Several versions can live side by side. A project resolves each type by its `Requires` entry.
- During the MVP, the existing `templates/` folders are treated as **unsigned development packages** (`Vers` normalised to `x.y.0`).

**Consequences, LOCKED for all stories:**
- `BlockManager` and `SchemaManager` load types from the library store, keyed by `(Uuid, Vers)`, not only by `Uuid`.
- The editor reads only `manifest.xml`, `Descriptor.xml`, icons and docs.
- A controller installs only signed packages (ADR-008), except in a "developer mode" that is clearly flagged.
- Accepted cost: each binary must be built for every controller target. Mitigation: SDK CMake template plus CI cross-builds.

**Alternatives:** loose folders (no integrity check); version only, without a hash (cannot detect a rebuild under the same version).

**Revisit when:** non-Linux controllers appear (add target triples) or package dependencies become common.

---

### ADR-005: Runtime: wire the XML once into arrays of direct callbacks; drivers call them inline; tick interval per block type

**Status:** Accepted (rev. 1.2, follows the owner's execution concept)  **Drives:** FR-17, NFR-01, NFR-09, NFR-11, NFR-15

**Context:** NFR-01 requires maximum speed and minimal CPU, with no interpreted language. The project owner's concept:
- a value change directly calls the input callbacks of everything connected to it;
- each block also has a periodic `run()` for state machines;
- peripherals react to hardware on their own threads, and a driver may call its signals' callbacks directly so the value reaches the targets instantly;
- connections are many-to-many;
- signals are not globally synchronised: each event travels on its own.

Crash detection must not write to flash storage during normal operation.

**Decision:** STIx Runtime handles a project in two phases.

1. **Load and wire (once per upload):**
   - Parse `Project.xml`, check `Requires` against the package store, and `dlopen` each required plugin. Then `create` one **instance (object)** per `BlockInstance` and per configured periphery or protocol node (for example, each Modbus TCP connection), passing its properties. The instance holds its own parameters and local variables. A node's channels are that instance's signals.
   - Give every *source* (block output, input signal, variable) a numeric `SourceId` (`uint32`) and a value slot. The `IUuid` and title are kept only in a symbol table, used for monitoring, logs and fault reports.
   - For every source, build a contiguous **subscriber array** with one entry `{instance, on_input, input_idx}` per connection. This covers every many-to-many case:
     - one output → several inputs: several entries in its array;
     - several sources → one input: that input appears in several arrays;
     - one output → two inputs of the same block: two entries.
   - Before the first event, each instance receives through `bind` the list of `SourceId`s connected to each of its inputs, so a block like AND can prepare its map of inputs.
   - Build the tick schedule from each block type's `TickMs` (from its descriptor).
2. **Execute:**
   - **Threads that run block code:**
     - every **driver thread**, which calls `emit_signal` when a value arrives from hardware;
     - the **RuntimeManager** thread, which calls `run()` on the ticks and applies values coming from STIx Config (`Force`, `SetVariable`) with the same `emit_signal`.

     Different instances can therefore run in parallel.
   - **Per-instance serialisation (done by the runtime, not by the block author):** each instance has an atomic `busy` flag and a small lock-free mailbox.
     - When a callback targets an instance that is free, the calling thread sets `busy` and runs the callback **inline**, immediately.
     - When the instance is busy (another thread is in it, or the same chain came back to it), the event goes into its mailbox. The thread that holds the instance drains the mailbox before releasing it.
     - As a result, one instance never runs on two threads at once, so block authors still write simple single-threaded code. No thread ever waits for a lock, so deadlock is impossible.
     - The cost when there is no contention is one atomic compare-and-swap per call.
   - **Propagation is direct:** when a block calls `host->emit(out_idx, value)`, the runtime stores the value and walks that output's subscriber array, calling `on_input(self, input_idx, source_id, value)` on each entry. Those blocks may emit in turn (synchronous, depth-first). There is no queue or sorting step between blocks.
   - **Fan-in is the block's decision:** `on_input` always says which source sent the value. A block either reacts only to the last event (e.g. Lighting) or keeps a map of all sources on that input and evaluates them together (e.g. AND).
   - **Event or change:** the runtime never filters. Each block or signal decides whether to emit on every event or only when the value changed. The SDK provides a helper `stix_emit_if_changed`, and periphery channels expose this as a property (as `Logger/@OnChangeOnly` already does).
   - **Tick:** a block type declares `TickMs` in its descriptor (for example `100`). The RuntimeManager calls `run(now)` on each instance of that type every `TickMs`, using a timer wheel with 10 ms resolution. `TickMs="0"` (the default) means the block is never ticked, so simple blocks like And or Add cost nothing while idle.
   - **One way to send a signal: `host->emit_signal(channel, value)`.** Each driver (UART, CAN, Modbus, digital-input interrupts…) runs its own thread(s). When a value arrives, the driver calls `emit_signal`, and the subscriber callbacks run **inline on the calling thread**, so the value reaches its targets at once.
     - The same call is used wherever the value comes from: a driver thread, or the RuntimeManager (forced values, variables set from STIx Config). Blocks don't need to know which thread called them; the per-instance `busy` flag takes care of that.
     - There is no runtime event queue. **Scheduling the reads is the driver's job,** driven by the properties of each channel. Example: a Modbus TCP driver polls register `0x200` every 1 s and `0x201` every 2 s (a per-channel `PollMs` property in its periphery descriptor). When a response arrives, it calls `emit_signal` for that channel.
     - **Why the runtime uses a `busy` flag and not locks:** with an ordinary lock per block, two drivers crossing paths (driver 1 inside A calls B, while driver 2 inside B calls A) would wait for each other forever. With the flag and mailbox, no thread ever waits, so this can't happen. LOCKED: the runtime must not replace this with mutexes.
   - **Output signals:** when a chain reaches an output signal, the runtime calls that driver's `write(channel, value)` directly, from whichever thread is running. `write` must therefore be thread-safe and must not block. The SDK provides a ready-made adapter that queues the value to the driver's own thread.
   - **Values crossing threads:** a slot's type is fixed at load, so only its 8-byte payload changes. It is stored with a single atomic 64-bit write: no locks, and no torn reads by monitoring. Strings are immutable and reference-counted, and are passed as handles.
   - When there are no events and no tick is due, the RuntimeManager sleeps. There is no heap allocation in callbacks or ticks; all memory, including the mailboxes, is allocated during load.
3. **Recursion and crash handling (contain, don't prevent).** The runtime does not try to forbid loops. Block developers are responsible for loop-safe logic, and the system detects, isolates and reports faults:
   - **Editor warning:** `ProjectValidator` finds cycles in the wiring and shows a warning on the instances involved. Upload is still allowed.
   - **Loop guard:** each thread keeps two counters, held in thread-local RAM: the depth of the current call chain and the number of calls caused by the current event.
     - A loop that comes back to an instance already in the chain doesn't grow the stack: that instance is `busy`, so the event goes to its mailbox (see above). An endless loop therefore shows up as an endless number of calls, not as a stack overflow.
     - If the chain goes deeper than `MaxDepth` (default 64), or one event causes more than `MaxCalls` callbacks (default 10,000), the runtime stops the chain, **quarantines** the instance that was re-entered most, and sends a `Fault` to STIx Config with the chain of `IUuid`s.
   - **Crash and hang detection with no writes to storage:** a small **`stix-supervisor`** process starts the runtime as its child and shares one page of **RAM** with it (POSIX shared memory in `/dev/shm`, which is not flash).
     - **Per-thread record:** in that page, each thread that runs block code has a record with `current instance id` and a `call counter`. Before each plugin call, the runtime makes two plain stores into that RAM. There is no system call, no clock read and no file I/O, so it costs a few nanoseconds.
     - **Crash:** the child process dies (SIGSEGV, SIGBUS, SIGFPE, SIGABRT…) and the supervisor is notified by `waitpid`. Because the shared page belongs to the supervisor, it survives the crash. The supervisor then:
       - reads which instance the crashed thread was inside and quarantines it;
       - writes **one** fault record to persistent storage (the only flash write, and only when something has failed);
       - restarts the runtime, which skips the quarantined instance;
       - reports `Fault{iuuid, reason:"crash", lastLog}` once the editor connects.
     - **Hang:** every 100 ms the supervisor samples the per-thread records. If a thread is still inside the same instance with the same call counter for longer than `MaxCallMs` (default 500), it kills and restarts the runtime through the same quarantine path (`reason:"hang"`).
     - **Recent log:** the last log lines are kept in a ring buffer in the same shared RAM. The supervisor saves them with the fault record, so the editor receives the context of the crash.
     - The supervisor itself is tiny and contains no plugin code. systemd restarts it if it ever dies.
   - **Quarantined instance:**
     - its `on_input` and `run` are no longer called;
     - its outputs keep their last value, flagged `Fault`;
     - the quarantine survives restarts until the user sends `ClearFault` or uploads a new project;
     - STIx Config shows a fault marker on the instance and lists it in `LogTab`.
4. **Atomic deployment (NFR-11):** the new project is loaded and wired next to the running one, then swapped in between two events. If loading fails, the old project keeps running and the errors go back to the editor. The last good project is stored on disk.

**Consequences, LOCKED for all stories:**
- `on_input` and `run` return quickly: no blocking I/O, no sleep, no waiting. Long processes are state machines advanced by `run`, and hardware I/O belongs in driver threads. This matters even more now, because a slow block delays the driver thread that called it inline.
- Block authors write single-threaded code per instance. The runtime guarantees that one instance never runs on two threads at once. Blocks must not share mutable global or static state between instances.
- A driver's `write` must be thread-safe and must not block. Use the SDK queue adapter unless the hardware access is itself non-blocking.
- No feature may add per-call writes to storage. Persistent writes happen only on upload, on parameter changes (debounced saves of the edited project: ADR-017), on faults, and for retained variables (and those are batched).
- Every reference in the XML can be resolved at load time: no dynamic names, no expressions in attributes. An expression would be a `Formula` block that parses its text once, in `create`.
- Subscriber order follows the order of the `<Source>` elements. It is deterministic, but blocks must not rely on it.
- **By design (signals are not synchronised):** a block reached by two paths from the same event (A→B→D and A→C→D) runs twice, and the first time it sees the old value from the second path. Events from different drivers interleave in any order. Each block works with the latest values it has received.

**Alternatives:**
- Dirty-marking with rank-ordered execution (v1.0 of this ADR): no double evaluation, but an extra bookkeeping step per event. Replaced by direct callbacks at the owner's request.
- Cyclic PLC-style scan of all blocks every N ms: wastes CPU while idle and adds latency.
- A thread per block: every block would need locks.
- All block code only on one RuntimeManager thread (v1.1 of this ADR): blocks need no serialisation, but every hardware event pays a thread hand-over. Rejected by the owner: drivers must reach targets instantly.
- A global lock around all block code: simple, but drivers would block each other.
- Crash breadcrumb in a memory-mapped file (v1.1): it would cause flash writes. Replaced by supervisor plus shared RAM.
- An embedded script engine (Lua, JS): breaks NFR-01.

**Revisit when:** the double-evaluation effect causes real bugs (then add an optional ordered mode per page), or hard real-time (< 1 ms jitter) is required.

---

### ADR-006: Plugin interface is a versioned C ABI (`extern "C"`, function tables)

**Status:** Accepted (rev. 1.2)  **Drives:** FR-07, FR-17, NFR-01, NFR-10

**Context:** Plugins come from the community and from paid vendors. They may be built with different compilers or compiler versions, and may be closed source. C++ class ABIs are not stable across compilers.

**Decision:**
- Every plugin exports one symbol: `extern "C" const stix_plugin_v1* stix_plugin_entry(void);`.
- **Block functions:**
  - `create` / `destroy`;
  - `bind`: receives the sources connected to each input;
  - `on_input(self, input_idx, source_id, value)`: one call per incoming value;
  - `run(self, now)`: the tick;
  - `on_property(self, idx, value)`: a parameter changed at run time; apply it live (ADR-017).
- A block saves its own state into its parameters with `host->set_property(ctx, idx, value)` (for example "save the current lights as scene 2"). The runtime writes it into the instance's XML node, like a change from the HMI.
- **Driver functions:** `open`, `close`, `write`. A driver sends every incoming value with `host->emit_signal`, and the callbacks run inline at once. See §6.2.
- The callback receives the source as a numeric `SourceId`, not as a name string, because comparing integers on every event is much cheaper than comparing strings. When a block needs the name for a log, `host->source_name(id)` returns the `IUuid` and title.
- The **runtime owns the value storage**. Blocks send outputs only through `host->emit(ctx, out_idx, value)`, which calls the subscribers (ADR-005).
- `abi_version` is checked on load. A MAJOR mismatch rejects the plugin with a clear error.

**Consequences, LOCKED for all stories:**
- No C++ types, exceptions or STL containers cross the plugin boundary.
- Port order in the ABI equals the order of the `Input`, `Output` and `Property` elements in `Descriptor.xml`. **That order is part of the package's public contract**, and reordering ports needs a MAJOR version bump.
- The SDK ships a header-only C++ wrapper (a `stix::Block` base class with `onInput`, `run`, `emit`, `emitIfChanged`), so block authors don't write the C table by hand.
- Easier: plugins can be closed source, or written in any language that can export a C function (C, C++, Rust…).

**Alternatives:**
- C++ abstract interfaces: the ABI breaks across compilers.
- Out-of-process plugins with IPC: safe, but adds latency and CPU.
- WASM: safe and portable, but slower than native and adds a runtime.
- Passing the source as a string: easier to read, but a string compare on every event.

**Revisit when:** untrusted third-party plugins need isolation. Then consider sandboxing driver plugins in a separate process.

---

### ADR-007: STIx Link: TLS 1.3 over TCP, length-prefixed CBOR messages

**Status:** Accepted  **Drives:** FR-08…FR-13, NFR-02, NFR-09

**Context:** The editor needs request/response operations (upload, list libraries) and high-rate push streams (monitoring) over an encrypted channel. Qt provides `QSslSocket` and `QCborValue` without extra dependencies.

**Decision:**
- **Transport:** TCP plus TLS 1.3 (`QSslSocket` on the editor side, OpenSSL on the controller). The port is configured on `RootDevice/@PortLink` (new attribute; default 9443).
- **Framing:** `uint32 length (big-endian) | CBOR map`. Each message has `t` (type), `id` (request id, 0 for push messages) and `p` (payload).
- **Pattern:** request → response carrying the same `id`, plus server **push** messages (`ValueBatch`, `Log`, `State`).
- **Monitoring:** the client sends `Subscribe {slots:[…]}` using the indexes from the symbol table. The controller sends `ValueBatch` at most every 50 ms (configurable), with only the slots that changed since the last batch: `[slot, value, ts]`. This keeps both bandwidth and CPU proportional to change (NFR-09).
- **Large transfers** (project upload, package install) are chunked into 64 KiB pieces, and the receiver verifies a final SHA-256.
- **Protocol version** is negotiated in `Hello`. Within MAJOR 1 only additive changes are allowed.
- In the editor everything is handled by `ControllerLink_class` (L3) on its own `QThread`. The UI sees only properties and signals.

**Consequences, LOCKED for all stories:**
- All editor↔controller features add message types to the §6.1 table; nobody opens a second socket or protocol.
- Values on the wire use the same type tags as the ABI `stix_value`.
- Accepted cost: a custom protocol to maintain. Mitigation: the message table is the spec, and an echo test server exists for development.

**Alternatives:** HTTPS REST plus WebSocket (heavier stack on the controller, two channels); gRPC (needs a large dependency and is awkward with Qt); MQTT (needs a broker, and the semantics fit request/response poorly); OPC UA (very heavy for the MVP, but a candidate *Protocol package* for third-party SCADA integration).

**Revisit when:** browsers need to connect to the controller directly. Then add a WebSocket gateway that carries the same CBOR messages.

---

### ADR-008: AuthN/AuthZ and trust: pinned device certificates, role-based accounts, signed packages

**Status:** Accepted  **Drives:** NFR-02

**Context:** Community users cannot be expected to run a PKI, yet commercial controllers should be verifiable. Controlling a building is security-critical.

**Decision:**
- **Controller identity:** on first boot each controller generates a key pair and a self-signed certificate (later, STI hardware may ship with a certificate signed by the STI Device CA). The first time the editor connects, it shows the fingerprint, and once the user accepts, the certificate is **pinned**: stored per controller serial in `AppLocalDataLocation/trust/`. A changed certificate is a hard error with an explicit re-pair flow.
- **User authentication:** a controller account (username plus password, stored on the controller as an Argon2id hash) is exchanged for a session token in `Auth`. The password is only ever sent inside TLS.
- **Authorisation (RBAC):** the roles are:
  - `Admin`: accounts, packages, upload;
  - `Engineer`: upload, force, set variables;
  - `Viewer`: monitor only;
  - `HmiUser`: end users of STIx HMI, limited by the project's `Rights` to some rooms, categories or instances (ADR-015).

  The controller enforces roles per message type (§6.1). There is no start or stop: the program always runs, and it restarts only on upload.
- **Packages:** the controller verifies the Ed25519 `SIGNATURE` against its trusted keys (the STI shop key plus any keys the user adds). Unsigned packages need the controller to be in developer mode.
- **Project `Users` / `PswH`** become the `HmiUser` accounts on the controller (ADR-015). They are separate from the maintenance accounts. `PswH` must hold a salted Argon2id hash, never a plain or unsalted hash.

**Consequences, LOCKED for all stories:**
- Every Link request except `Hello` and `Auth` must carry an authorised session. The role table in §6.1 is the policy.
- The editor never stores passwords. It may keep the session token in the OS keychain (`QtKeychain`, optional dependency).
- Forced values are visibly flagged in the UI and the controller's audit log, and are released on disconnect unless set as "persistent".

**Alternatives:** mutual TLS with client certificates (stronger, but certificate management is harder for users; can be added later); password only, without pinning (open to a man-in-the-middle).

**Revisit when:** there is multi-site or fleet management. Then add an STI CA and optional mutual TLS.

---

### ADR-009: State management: C++ owns all state; QML binds to properties and calls `cmd*` methods

**Status:** Accepted (formalises existing practice)  **Drives:** NFR-07, NFR-08

**Context:** QML makes it easy to put logic in the views. The current code already uses C++ singletons (`ProjectManager`, `BlockManager`, `SchemaManager`, `Logger`) with `Q_PROPERTY` and `Q_INVOKABLE`.

**Decision:**
- All state lives in C++. QML holds only **transient view state** (hover, drag ghost, pan and zoom, which the `Page_class` view properties already hold).
- Data flows down to QML through `Q_PROPERTY` with `NOTIFY`. Intents flow up as `Q_INVOKABLE cmd*` (document edits) or verbs (`connect`, `upload`, …) on L3 services.
- Long-running work (network, file I/O over 50 ms, package unpacking) runs off the GUI thread. Results come back as queued signals.
- Live monitor values go into a dedicated `LiveValues_class` model keyed by `IUuid`, updated **at most once per frame** from `ValueBatch`. They never go into the document tree (no undo entries, no `modified` flag).
- Singletons are registered in `main.cpp` under `STI.<Name>` URIs, as today.

**Consequences, LOCKED for all stories:**
- No QML file computes business rules (type compatibility, naming, references). It asks C++ (`DataTypes.canConnect`, …).
- `ControllerConnection.qml` loses its local `connectionState` and binds to `ControllerLink.state`.

**Alternatives:** a JavaScript state store in QML. Rejected: it would duplicate state and is hard to test.

**Revisit when:** never expected.

---

### ADR-010: Error, result and logging convention

**Status:** Accepted  **Drives:** NFR-13, NFR-07

**Context:** Today the code mixes `bool` and `int` return codes (`ProjectManager::*` return `int`) and logs through `S_Logger.write(Log::Level, MODULE, text)`.

**Decision:**
- **No exceptions** cross module boundaries or Qt event-loop boundaries.
- Fallible operations return `bool` when the only outcome is success or failure. When a caller has to react to the reason, they return `Result_s { ErrorCode_e code; QString message; }`. `ErrorCode_e` is one shared enum (in L1), and its values are mirrored by the Link `Error` message codes.
- Every failure is logged **once**, where it is detected, with `S_Logger.write(Log::Warning|Error, <MODULE>_NAME, text)`. `<MODULE>_NAME` is a `#define` in each component, as `RESOURCE_URL_NAME` already is.
- The user sees errors through the `LogTab` / notification surface, not through modal dialogs, except for data-loss prompts.
- Runtime `Log` push messages are shown in the same `LogTab`, tagged with the controller name.

**Consequences, LOCKED for all stories:** new `Q_INVOKABLE` methods return `bool` or emit a `failed(code, message)` signal, and never use magic `int`s. Existing `int` returns are migrated in a cleanup story.

**Alternatives:** exceptions (they don't mix well with Qt/QML); `std::expected` (C++23, and the codebase is on C++17).

**Revisit when:** the project moves to C++23.

---

### ADR-011: Naming and code conventions (C++, QML, XML)

**Status:** Accepted (codifies the existing style)  **Drives:** NFR-07, NFR-05

**Decision:**

| Item | Convention | Example |
|------|-----------|---------|
| C++ class | `PascalCase_class` | `Project_class` |
| Struct / enum | `PascalCase_s` / `PascalCase_e`; enum values `Prefix_Name` | `NodeType_s`, `Lang_EN` |
| Global singleton | `S_PascalCase` | `S_ProjectManager` |
| Members | `m_<kind>_PascalCase`: `ptr`, `str`, `vec`, `hash`, `map`, `b`, `int`, `real` | `m_hash_NodeByIUuid` |
| Public struct fields | `PascalCase` | `TagName`, `Default` |
| QML-exposed methods | `camelCase`; document edits start with `cmd` | `cmdPlaceBlock` |
| Files | folder per component `src/Components/<Name>/`, file `lowercase.h/.cpp` | `blockmanager.cpp` |
| QML files | `PascalCase.qml`, folder per page/part | `EditorPage/Parts/...` |
| XML tags and attributes | `PascalCase`; abbreviations only from the tokens list | `BlockInstance`, `IUuid`, `Px` |
| XML string literals in C++ | only through `TOKEN_*` / `ATTR_*` / `TAG_*` macros in **one** header per contract | `ATTR_REF` |
| Versions | SemVer `MAJOR.MINOR.PATCH` everywhere | `1.0.0` |
| Comments / docs | English | |

**Consequences, LOCKED for all stories:** a new XML name is first added to the tokens header and then used. Known violations go into a cleanup story:
1. The block descriptors under `templates/blocks/` don't match what `BlockManager` reads. Across the 10 descriptors, 70 ports use `Type=` instead of `DataType=`, 44 use `ExtName=` instead of `Title=`, and 26 use `ShortTitle=` instead of `ShortName=`, so every port loses its data type on load today. `Garage_Gate` also contains a stray attribute `T` on the "Off / Lock" input. (Story 1.3.)
2. `document_tokens.h` and `project_tokens.h` define overlapping macros (`ATTR_REF` is defined twice; `TOKEN_PROJECT`, `ATTR_ROOM` and others are duplicated). Merge them into `sdk/stix_tokens.h` (the program/runtime contract) and `project_tokens.h` (editor-only metadata).
3. `Application` (global) lacks the `S_` prefix. Block `Vers="1.0"` is not SemVer.
4. `resourceurl.h` defines a non-inline function in a header, which causes ODR and link errors once two translation units include it.

**Alternatives:** Qt-style `camelCase` members without prefixes. Rejected: it would mean churning existing code for no gain.

**Revisit when:** a second developer joins and the team agrees to change the convention.

---

### ADR-012: Library sources behind one `LibraryRepository` interface (Local + Controller in the MVP, then Shop)

**Status:** Accepted  **Drives:** FR-12, FR-14, FR-15, NFR-12

**Decision:**
- L3 defines `LibraryRepository_class` (abstract) with `list()`, `fetchDescriptor(uuid, vers)`, `fetchPackage(uuid, vers)`, `install(pkg)` and `remove(uuid, vers)`. Implementations: `LocalRepository` (a folder; MVP), `ControllerRepository` (over Link, reading the controller's installed packages; **MVP**, FR-15), and `ShopRepository` (HTTPS REST; post-MVP).
- `LibraryService_class` combines them and feeds `BlockManager` and `SchemaManager`, so the editor catalogue no longer depends on where packages come from.
- **The editor is the broker:** packages travel Shop → Editor → Controller. Controllers often have no internet access, and this keeps licence checks in one place. A direct Shop → Controller download may come later.
- The **Shop** is a separate service: REST/JSON over HTTPS, OAuth2 accounts, and licences bound to the controller serial for paid packages. The shop signs every package it publishes.

**Consequences, LOCKED for all stories:** UI code never reads library folders directly. It goes through `LibraryService`.

**Revisit when:** a fleet of controllers needs unattended updates.

---

### ADR-013: Build and platforms: CMake + Qt 6.11, CI on 3 OSes, platform code isolated

**Status:** Accepted  **Drives:** NFR-04

**Decision:**
- One `CMakeLists.txt` per target, using `qt_add_executable` / `qt_add_qml_module` as today. C++17, with warnings as errors in CI.
- CI matrix: Windows (MSVC 2022, x64), macOS (arm64 + x86-64 universal), Linux (GCC, x86-64).
- Packaging: `windeployqt` + an installer (Inno Setup or WiX), `macdeployqt` + codesign + notarisation (`.dmg`), and AppImage on Linux.
- OS-specific code lives only in `src/Platform/<os>/` behind one interface. Paths always come from `QStandardPaths`, as `Setup.hpp` already does.
- Third-party dependencies beyond Qt need an ADR. The only one expected is `QtKeychain` (optional).

**Consequences, LOCKED for all stories:** no `#ifdef Q_OS_*` outside `src/Platform/`. File paths use `QDir` / `QFileInfo`, never hard-coded separators.

---

### ADR-014: Licensing split: core GPL-3.0, SDK/ABI headers MIT, packages under their own licence

**Status:** Accepted (2026-11-05, by the project owner; a legal review is still recommended before the first paid package ships)  **Drives:** NFR-10

**Context:** Qt Open Source is LGPL-3.0/GPL-3.0. The editor and runtime should be open source, and paid closed-source plugins must be allowed.

**Decision:**
- STIx Config: **GPL-3.0** (compatible with Qt, links Qt dynamically).
- STIx Runtime: **LGPL-3.0 or Apache-2.0**, so that loading proprietary plugins does not create GPL obligations.
- `stix-sdk` (tokens, ABI headers, CMake template): **MIT**, so plugin authors can use any licence.
- Packages: licence declared in `manifest.xml`, and enforced by the shop for paid packages.

**Revisit when:** a commercial edition of the editor is planned (dual licensing).


---

### ADR-015: Block HMI UI: optional QML components inside the package, shown by the STIx HMI app

**Status:** Accepted  **Drives:** FR-18, FR-19, FR-21, NFR-02

**Context:** End users (home owners, facility staff) must watch and operate blocks from a phone or tablet, as in commercial building-automation apps. A block's UI is specific to that block (a gate needs open/stop/close, a climate block a set-point dial), so it must ship with the block. Qt Quick runs on Android, iOS and desktop, and the editor already uses QML.

**Decision:**
- **STIx HMI is a separate product:** a Qt Quick app for Android, iOS and desktop/kiosk tablets. It connects to the controller over STIx Link with an end-user account, locally by IP or remotely through STIx Remote (ADR-016).
- **UI files in the package:** a package may contain `ui/Tile.qml` (a compact card in a room or category list) and `ui/Detail.qml` (the full control panel), plus their assets. The descriptor declares them:
  ```xml
  <Ui Tile="ui/Tile.qml" Detail="ui/Detail.qml" ApiVersion="1.0"/>
  ```
- **What the UI may touch:** each port in the descriptor carries `Ui="Read"`, `Ui="Write"` or `Ui="ReadWrite"` (default: not visible to the HMI). The HMI app injects a single object, `stixBlock`, into the component:
  - `stixBlock.title`, `stixBlock.room`;
  - `stixBlock.value("Op")`: a live value that QML can bind to;
  - `stixBlock.property("TempComfort")`: a parameter, by its XML attribute name;
  - `stixBlock.setProperty("TempComfort", "22.5")`: only for parameters marked `Ui="Write"`; sends `HmiSetParams` (ADR-017);
  - `stixBlock.send("Tg", true)`: only for ports marked `Write`.

  The UI cannot reach other blocks, the network or files.
- **Path of a command:** `send` → Link `HmiCommand {iuuid, input, value}` → the RuntimeManager calls that input's `on_input`, with a dedicated `SourceId` for the HMI user. A block sees an HMI command like any other source, and can still tell it apart (for example, to log who opened the gate).
- **Fallback:** a block without `ui/` gets a generic UI built from its `Ui` ports: Bool → switch or button, Int/Float → value or slider, Enum → list.
- **Sandbox for package QML:** the HMI app loads it in a dedicated `QQmlEngine` with:
  - an import whitelist: only `QtQuick`, `QtQuick.Controls`, `QtQuick.Layouts`, `QtQuick.Shapes` and `STIx.Hmi 1.x`;
  - a URL interceptor that allows only files inside the same package;
  - no network access, and no `XMLHttpRequest`, `Qt.openUrlExternally` or `LocalStorage`.

  Only signed packages are loaded (ADR-008).
- **Sync when the app opens (FR-21),** and again whenever a `State` push reports a new project hash:
  1. The app connects and sends `HmiGetProject {knownHash}`.
  2. If the hash matches its cached copy, nothing is downloaded, and the app starts immediately from its cache.
  3. Otherwise the controller sends the **HMI view** of the project XML and its new hash. The app extracts the UI layout from it: rooms, categories, block instances with their `Room` and `Category`, and their `Ui` ports.
  4. The app compares the view's `<Requires>` with its UI cache, which is keyed by `Uuid` + `Vers` + `Sha256`. For every block type whose UI is missing, or cached in another version, it downloads the `ui/` files from the controller (`HmiGetUiFile`) and checks their `Sha256`. Versions no longer used are deleted from the cache.
  5. The app builds the screens.
- **HMI view of the project:**
  - It uses the same XML format as `Project.xml`, so the same reader parses it.
  - The controller generates it per user. It keeps `Rooms`, `Categories`, `Requires`, and only the block instances and `Ui` ports that the user's Rights allow.
  - It leaves out `Users`/`PswH`, the `RootDevice` network details, and driver settings and passwords.
  - Reason: the HMI runs on personal phones, which can be lost. The full project would expose credentials and the whole installation.
  - The layout is derived from the rooms, categories and instances. An explicit HMI layout section (tile order, favourites) can be added later as a MINOR format change.
- **End-user accounts:** the project's `Users`, `UserGroups` and `Rights` (these tokens already exist) become controller accounts with the role `HmiUser`. Rights decide which rooms, categories or instances each user can see and operate.
- **Editor preview:** STIx Config can render `Tile.qml` and `Detail.qml` with simulated values in a preview panel, using the same sandbox settings.

**Consequences, LOCKED for all stories:**
- A block's UI is presentation only. Its logic stays in the block's native code. The QML may only read `Ui` ports and send to `Write` ports.
- The `STIx.Hmi` QML API is versioned like the ABI. A MAJOR change means a new `ApiVersion`.
- QML's JavaScript runs only in the HMI app and in the editor preview, never on the controller, so NFR-01 is not affected.
- `HmiUser` accounts can never upload, force values or manage libraries.
- **Risk to check before release (iOS):** App Store rules limit apps that download code which changes their features, and QML downloaded from packages may fall under this. Check the current App Store guideline 2.5.2 before committing to iOS. The fallback on iOS is the generic UI, or a fixed set of UI templates driven by data. Android and desktop have no such limit.

**Alternatives:**
- A web UI (HTML/JS) served by the controller: works in any browser, but adds a second UI technology and a web server on the controller.
- Only the generic, generated UI: the safest option, but it can't express rich controls such as a gate animation or a climate dial.
- Declarative JSON layouts rendered by the app: no code, so store-friendly, but much less flexible. Kept as the iOS fallback.

**Revisit when:** the App Store blocks downloaded QML, or users need custom screen layouts beyond rooms and categories.


---

### ADR-016: Remote access through the STIx Remote relay (registered by serial number, end-to-end TLS)

**Status:** Accepted  **Drives:** FR-20, NFR-02

**Context:** On the local network, STIx HMI (and STIx Config) connect to the controller by IP. Away from the building, the controller usually sits behind NAT with no public IP. The owner's concept:
- a remote server that acts like a VPN;
- controllers registered by serial number, which push heartbeats to the server;
- when a user wants to connect, the server connects to both the controller and the app;
- all data flows exactly as it does locally.

**Decision:**
- **A fifth product, STIx Remote:** a cloud relay service.
- **Registration:**
  - Remote access is off by default. When the owner turns it on, the controller opens an **outbound** TLS connection to the relay, which works behind NAT with no port forwarding. It registers with its serial number and certificate fingerprint.
  - The owner then claims the controller on the relay, using the serial number plus a one-time claim code shown in STIx Config. Knowing the serial number alone is not enough.
- **Heartbeat:** the controller sends a heartbeat every 30 s (`{serial, ts, runtimeVers}`) on that control connection. The relay uses it to show the controller as online or offline, and to deliver session requests.
- **Opening a session:**
  1. The app signs in to the relay and sends `Connect {serial}`.
  2. The relay checks that this relay account may reach that serial. It then asks the controller, over the control connection, to open a new outbound data connection for this session (`OpenTunnel {tunnelId}`).
  3. The relay joins the two connections byte for byte.
- **Same protocol end to end:** inside the tunnel, the app and the controller run the normal STIx Link: TLS 1.3 handshake, certificate pinning, `Auth`, roles. It is exactly the same as on the LAN. The relay only forwards encrypted bytes, so it cannot read or change project data, values or passwords.
- **Two independent checks:** the relay account decides whether the user can *reach* the controller; the controller account (`HmiUser`, `Engineer`…) decides what the user may *do*.
- **Any Link client can use it:** STIx HMI, and STIx Config for remote maintenance.
- **Choice of path:** the app first tries the controller's local IP (stored per controller). If it can't reach it, it falls back to the relay automatically.

**Consequences, LOCKED for all stories:**
- The relay stays a pipe. No feature may depend on the relay reading the payload.
- The controller never accepts connections from the internet. Remote access is outbound only.
- Messages and behaviour are identical over the LAN and the relay; only latency differs. On slow links the client may raise the `ValueBatch` period (`periodMs` in `Subscribe`).
- Turning remote access off closes the outbound connection immediately.
- Relay accounts are separate from controller accounts.

**Alternatives:**
- Port forwarding or a public IP: needs router configuration, and exposes the controller to the internet.
- A real VPN (WireGuard) per site: strong, but needs setup and key management for every user. It could be offered later as an advanced option.
- A relay that terminates TLS (a cloud proxy): would allow cloud features, but the cloud could read everything. Rejected for privacy and security.
- A cloud MQTT broker: different semantics from the Link, so two protocols to maintain.

**Revisit when:**
- relay bandwidth costs grow: add direct peer-to-peer connections (NAT traversal), keeping the relay as fallback;
- cloud features are needed (push notifications for the `Notification` block, history): add them as separate, opt-in data flows.


---

### ADR-017: Parameters edited on the controller: applied to the instance, written into its XML node, merged by STIx Config

**Status:** Accepted (rev. 1.1, the owner's concept)  **Drives:** FR-22, NFR-03, NFR-05, NFR-11

**Context:** The HMI app downloads the project XML and loads the UI components it needs. When a user changes a value on a UI card (for example a temperature from 20 to 22.5 °C), the controller must apply it to that block instance at once, and the project XML on the controller must reflect it. A block can also save its own state into the XML. Meanwhile, an engineer may be editing the same project in STIx Config. An upload must not silently erase what the users changed.

**Decision:**
- **Message from the app:** one object, keyed exactly like the XML the app loaded:
  ```json
  { "iUuid": "01a0f27a-2395-75bf-8bfc-9dfc17f00f47", "TempComfort": "22.5" }
  ```
  - `iUuid` selects the instance.
  - Every other key is the **exact attribute name** from that instance's XML element, and its value is the new value as text, as in the XML.
  - Several keys may be sent in one message.
  - It travels as the `HmiSetParams` message of the STIx Link (ADR-007). STIx Config uses the same shape with `SetParams`.
- **Each instance is linked to its XML node:**
  - When the project loads, every instance gets a pointer to its own XML element (pugixml node), and the runtime builds a hash map `IUuid → instance`.
  - Applying a change is: find the instance by `iUuid` → for each key, check it → call the instance's `on_property(idx, value)` → write the attribute on its node through the pointer → mark the project **modified**.
  - There is no search through the XML, and the fast path never touches it.
- **Check on each key:**
  - the attribute must be a parameter declared in the block descriptor;
  - the value must match the descriptor type;
  - for an HMI user, the parameter must be marked `Ui="Write"` in the descriptor (the same attribute used for ports, ADR-015), and the user's Rights must allow that instance.

  Otherwise the key is refused, and the error names it.
- **State saved by the block itself:** a block calls `host->set_property(ctx, idx, value)`. It follows the same path: node updated, project marked modified.
- **Saving:** when the project is marked modified, the controller writes `Project.xml` at most once every 10 s, and on clean shutdown. The save is atomic: a temporary file, then rename. `Project/@Rev` increases by 1. The writer follows the ADR-003 style rules, so the file stays readable.
- **Telling the clients:** after each save the controller pushes `ProjectChanged {rev, hash}`.
  - **STIx Config** shows: "The project was edited on the controller (rev N). Download the latest version?"
  - **STIx HMI** re-syncs through the hash (ADR-015).
- **Uploading over changes (Overwrite or Merge):**
  - The controller keeps **Base**, the exact XML of the last upload, next to the current one.
  - STIx Config stores the `Rev` it last uploaded or downloaded (`BaseRev`).
  - `UploadProject` carries `baseRev`. If the controller's `Rev` is newer, the controller refuses with `RevConflict {rev}`, and STIx Config asks the engineer: **Overwrite** or **Merge**.
  - **Overwrite:** a second confirmation, listing how many values will be lost. Then the upload runs with `force:true`.
  - **Merge (three-way, by `IUuid`):** STIx Config downloads **Base** and **Controller** (`GetBase`, `DownloadProject`) and compares them with **Mine** (the open project), element by element (matched by `IUuid`) and attribute by attribute:

    | Base → Mine | Base → Controller | Result |
    |-------------|-------------------|--------|
    | unchanged | changed | take the controller's value (e.g. the user's 22.5 °C) |
    | changed | unchanged | keep mine |
    | changed | changed, same value | no conflict |
    | changed | changed, different values | **the engineer's value wins** (rule set by the owner); listed in the summary so the engineer sees which user values were replaced |
    | element added or deleted in Mine | — | structure always comes from Mine; if I deleted an instance the users had changed, it is listed in the summary |

  - The merge is fully automatic, with no questions. The result is applied to the open project as **undoable** edits and shown as a summary ("14 values taken from the controller; 2 user values replaced by yours"). The engineer saves and uploads with the new `baseRev`.
  - If the controller changed again in the meantime, the upload is refused once more, and the merge repeats on the new difference.
  - This works automatically in almost every case, because the controller and the app change **only attribute values, never the structure**. When the engineer and a user changed the same parameter, the engineer's value is kept.

**Consequences, LOCKED for all stories:**
- Only attribute values of existing elements can change on the controller. Adding or removing instances, wiring, periphery or users happens only by upload.
- The keys in `SetParams` / `HmiSetParams` are exactly the XML attribute names; there are no other aliases.
- Every runtime instance holds its XML node pointer for the life of the loaded project. The pointers are rebuilt on every upload.
- An upload never overwrites changes on the controller without the engineer choosing **Overwrite** or **Merge**.
- `on_property` must apply the value without blocking (ADR-005). Saves are debounced and never happen per message.

**Alternatives:**
- Last upload wins: users lose their settings after every maintenance visit. Rejected.
- Always forcing a download before editing: safe, but the engineer often works offline for hours.
- A change journal replayed onto the engineer's file: more data to keep, and the same result as the three-way merge. Not needed.
- A text-based merge (like git): breaks on attribute order and formatting, and doesn't understand `IUuid`. Rejected.

**Revisit when:** the HMI must add or remove elements (for example creating a new scene). Then the merge must also handle child elements, matched by `IUuid`.

---

## 4. Component Design

### Component overview (STIx Config)

```
 QML (L4): Main · HomePage · EditorPage{TopToolBar, ProjectTree, Page/Block/Port/Signal, Properties, ControllerConnection} · LogTab
   │ properties / cmd* / signals
 L3: ProjectManager ── ControllerLink ── LibraryService ── ShopClient (post-MVP) ── LiveValues
   │
 L2: Project{Page, ProjectTree, commands, Validator} ── SchemaManager ── BlockManager ── DataTypes
   │
 L1: Document{XmlNode, tokens, resourceurl} ── Logger ── Setup/paths ── sdk/ (stix_tokens.h, stix_abi.h)
```

### Component: Document (L1)
**Responsibility:** read and write XML into a generic `XmlNode_class` tree, keeping everything.
**Interfaces provided:** `XmlNode_class::readDoc/write`, tree and attribute API, tokens.
**Data owned:** none (a utility). **ADRs:** 002, 003, 011. **NFRs:** NFR-03, NFR-05 (stable attribute order lives here).

### Component: SchemaManager (L2)
**Responsibility:** load `NodeType` descriptors (project schema and periphery) and answer "what can this node contain and edit?".
**Interfaces provided:** `getNode(uuid|tag|node)`, `sections()`. **Required:** Document, LibraryService (post-MVP).
**ADRs:** 004, 012. **NFRs:** NFR-12.

### Component: BlockManager (L2)
**Responsibility:** the catalogue of block types (ports, properties, categories) from block descriptors, keyed by `(Uuid, Vers)`.
**Interfaces provided:** `categories` (QML), `getBlockTypeByUuid`. **ADRs:** 004, 006 (port order), 011. **NFRs:** NFR-12.

### Component: DataTypes (L2)
**Responsibility:** the data type registry (`Void`, `Bool`, `Int`, `Float`, `String`, `Enum`, `Date`, `Custom`), colours and the conversion matrix for `canConnect`.
**Constraint:** the type names are the same strings as the ABI type tags (ADR-006). **NFRs:** FR-04.

### Component: Project (L2)
**Responsibility:** one open document: index, selection, undo stack, pages, tree model, properties.
**Interfaces provided:** the QML API (`cmd*`, `undo`/`redo`, properties); C++ signals `nodeInserted`, `nodeRemoved`, `attributeChanged`, `documentReset`.
**Sub-parts:** `Page_class` (blocks, signals, connections per page; `connections()` is still a stub), `ProjectTree_class`, the 4 commands, and **`ProjectValidator_class` (new)**: type checks, unresolved references, cycle **warnings**, and `Requires` regeneration. It is used by save and by upload.
**ADRs:** 002, 003, 005 (same reference rules as the runtime loader). **NFRs:** NFR-03, NFR-08.

### Component: ProjectSync (L3, new)
**Responsibility:**
- Track `BaseRev` per project and controller.
- On `ProjectChanged`, tell the engineer the project was edited on the controller and offer to download it.
- On `RevConflict`, ask **Overwrite** or **Merge**. For Merge, run the three-way merge (Base / Mine / Controller, by `IUuid` and attribute; the engineer wins when both changed the same attribute), show the summary, and apply the result as undoable `SetAttribute` commands.

**ADRs:** 002, 017.

### Component: ProjectManager (L3)
**Responsibility:** tabs of open projects, recent list, new/open/save/close orchestration.
**ADRs:** 009, 010 (move to `bool` + `failed` signal).

### Component: LibraryService (L3, new)
**Responsibility:** combine the `LibraryRepository` sources, install and remove packages locally, compare a project's `Requires` with a target repository, and produce the diff `{missing, mismatched, extra}`.
**ADRs:** 004, 012. **NFRs:** NFR-06, FR-13.

### Component: ControllerLink (L3, new)
**Responsibility:** connect, pair (pin), authenticate, send requests, receive pushes. It exposes `state` (Disconnected, Connecting, Connected, Error), `controllerInfo`, `programDifferent` and `roles`.
**Interfaces required:** `QSslSocket`, `QCborStreamReader/Writer`, LibraryService (for install).
**ADRs:** 007, 008, 009. **NFRs:** NFR-02, NFR-09.

### Component: LiveValues (L3, new)
**Responsibility:** map symbol-table slots to `IUuid`, keep the latest value plus the force and fault flags, and publish per-frame updates to `Port.qml`, `Signal.qml`, `Block.qml` (fault marker) and the properties panel.
**NFRs:** NFR-09 (frame-coalesced updates), NFR-08, NFR-15.

### Component: ShopClient (L3, post-MVP)
**Responsibility:** sign in to the shop, browse and search the catalogue, buy, download packages into LibraryService. **ADRs:** 012.

### Component: UiPreview (L3/L4, optional)
**Responsibility:** render a package's `ui/Tile.qml` and `ui/Detail.qml` with simulated values, in a sandboxed `QQmlEngine` (ADR-015). **ADRs:** 015.

### STIx HMI components (contract level)
`HmiLink` (STIx Link client, `HmiUser` session) · `UiCache` (UI files by `Sha256`) · `SandboxEngine` (QML import whitelist, URL interceptor) · `StixBlockApi` (the `stixBlock` object: `value`, `property`, `send`) · `GenericUi` (fallback built from `Ui` ports) · `RoomsView` (navigation by room and category).

### STIx Runtime components (contract level)
`LinkServer` (TLS, auth, RBAC) · `PackageStore` (verify signature, install side by side by version) · `Loader` (XML → instances, subscriber arrays, symbol table) · `RuntimeManager` (ticks via timer wheel, `Force`/`SetVariable` from the Link, per-instance busy flag and mailbox, loop guard) · `DriverHost` (driver threads calling `emit_signal`) · `stix-supervisor` (separate process: shared-RAM per-thread records, crash and hang detection, quarantine list, restart) · `Persistence` (last good project, retained variables, quarantine) · `AuditLog`.

---

## 5. Data Model

> Governed by ADR-002 and ADR-003. All stories share these entity shapes. Element and attribute names come only from the tokens headers.

### Project.xml (ConfigVers 1.0.0, illustrative)
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project IUuid="…" Title="Villa Codru" Author="…" ConfigVers="1.0.0" CDate="2026-11-05" MDate="2026-11-05"
         Language="EN" PaperFormat="A3" Timezone="Europe/Chisinau" UTemp="C">
    <Requires>
        <Library Uuid="42764a08-e037-4d17-a2ab-52fff0eed2a8" Name="Garage_Gate" Kind="Block"
                 Vers="1.0.0" Sha256="9f2c…"/>
        <Library Uuid="01a10094-e4fe-755f-bae9-8cf6076b9518" Name="ModbusTcp" Kind="Protocol"
                 Vers="2.3.1" Sha256="ab41…"/>
    </Requires>
    <Rooms> <Room IUuid="…" Title="Garage"/> </Rooms>
    <Variables> <Variable IUuid="…" Title="NightMode" DataType="Bool" Value="0"/> </Variables>
    <RootDevice IUuid="…" Title="Main controller" DeviceModel="…" Serial="…" IpAddress="192.168.1.10" PortLink="9443">
        <DigitalIn  IUuid="…"> <DigitalInCh  IUuid="di-1" Title="Gate open switch" Channel="1"/> </DigitalIn>
        <DigitalOut IUuid="…"> <DigitalOutCh IUuid="do-1" Title="Gate motor"       Channel="1"/> </DigitalOut>
        <!-- VirtualIn, VirtualOut, NetworkPeriphery, Messages, Periphery sections -->
    </RootDevice>
    <Program>
        <Page IUuid="…" Title="Garage" MDate="…">
            <SignalInRef  IUuid="s-1" Io="di-1" Px="40"  Py="120"/>
            <BlockInstance IUuid="b-1" Uuid="42764a08-…" Title="Gate" Px="300" Py="100" Room="…">
                <Input  ShortName="Io"><Source Ref="s-1"/></Input>
                <Input  ShortName="Tg"><Source Ref="s-3"/><Source Ref="v-1"/></Input>   <!-- 2 sources on one input -->
                <Output ShortName="Op" IUuid="b-1.Op"/>
                <Property ShortName="MotorTime" Value="25"/>
            </BlockInstance>
            <SignalOutRef IUuid="s-2" Io="do-1" Px="800" Py="120"><Source Ref="b-1.Op"/></SignalOutRef>
        </Page>
    </Program>
</Project>
```

### Entities

| Entity | Key attributes | Relationships | Constraints |
|--------|----------------|---------------|-------------|
| **Project** | `IUuid`, `ConfigVers`, `Rev`, metadata, units | root; contains Requires, Rooms, Users, Variables, RootDevice, Program | exactly 1 per file |
| **Requires/Library** | `Uuid`, `Name`, `Kind`, `Vers`, `Sha256` | referenced by `BlockInstance/@Uuid`, periphery `NodeType` Uuid | generated on save; one entry per `(Uuid)`, so one version per project |
| **RootDevice** | `IUuid`, `DeviceModel`, `Serial`, `IpAddress`, `PortLink` | contains periphery sections | MVP: exactly 1 |
| **Channel / periphery node** | `IUuid`, `Uuid` (type), `Title`, `Channel`, typed properties | targeted by `SignalInRef/@Io`, `SignalOutRef/@Io` | `IoType` decides In, Out or InOut |
| **Variable** | `IUuid`, `Title`, `DataType`, `Value` (initial), `Retain` (new, optional) | targeted by signal references (InOut) | |
| **User / UserGroup / Rights** | `IUuid`, `Username`, `PswH`, `Rights` | become `HmiUser` accounts on the controller; Rights → rooms, categories, instances | `PswH` = salted Argon2id (ADR-008) |
| **Page** | `IUuid`, `Title`, `MDate` | contains BlockInstance and SignalRefs | title unique in Program |
| **BlockInstance** | `IUuid`, `Uuid` (type), `Title`, `Px`, `Py`, `Width`, `Room`, `Category` | contains Input, Output, Property | `Uuid` must appear in Requires |
| **Input** | `ShortName` | **0..N** `Source/@Ref` → an Output, SignalInRef or Variable `IUuid` | each source's type must pass `canConnect`; how several sources combine is decided by the block (ADR-005) |
| **Output** | `ShortName`, `IUuid` | referenced by any number of `Source/@Ref` | |
| **Property** | `ShortName`, `Value` | | value validated against the descriptor type; may change on the controller (ADR-017), and from the HMI only if the descriptor marks it `Ui="Write"` |
| **SignalInRef / SignalOutRef** | `IUuid`, `Io`, `Px`, `Py` | `Io` → channel or variable; SignalOutRef has 1 `Source` | |

### Storage strategy
- **Primary store:** one `Project.xml` per project folder. Optional sibling folders `assets/` (logos, icons) and `.stix/` (editor cache, not versioned).
- **Editor settings:** `AppLocalDataLocation/config/Config.xml`, `Recent.xml`, `trust/`, `libraries/`.
- **Controller:** `/var/lib/stix/project/current/Project.xml`, `…/previous/`, packages in `/var/lib/stix/packages/<Uuid>/<Vers>/`, retained variables in a small append-only file (strategy only).
- **Backups:** a `.bak-<version>` file before migrating; git is recommended for project folders.

---

## 6. API Specifications

### 6.1 STIx Link messages (protocol v1)

**Protocol:** TLS 1.3 / TCP, `uint32 len | CBOR {t, id, p}`. **AuthN:** session from `Auth` (ADR-008). **Versioning:** `Hello.p.proto = [1, minor]`.

| `t` | Direction | Payload (p) | Response | Min role |
|-----|-----------|-------------|----------|----------|
| `Hello` | C→S | `{proto, client, clientVers}` | `{proto, serial, model, runtimeVers, abi, certFp}` | — |
| `Auth` | C→S | `{user, password}` | `{token, roles, expires}` | — |
| `GetInfo` | C→S | `{}` | `{state, projectHash, projectTitle, uptime, cpu, mem}` | Viewer |
| `ListLibraries` | C→S | `{}` | `[{uuid, name, kind, vers, sha256, licence}]` | Viewer |
| `GetLibraryFile` | C→S | `{uuid, vers, path}` | chunked file (descriptor, manifest, icon, `docs/`, `ui/`) from an installed package | Viewer |
| `InstallLibrary` | C→S | chunked `{uuid, vers, sha256, size}` + `Chunk` | `{ok}` or `Error` | Admin |
| `RemoveLibrary` | C→S | `{uuid, vers}` | `{ok}` (refused if the running project uses it) | Admin |
| `CheckProject` | C→S | `{requires:[…]}` | `{missing:[…], mismatched:[…]}` | Engineer |
| `UploadProject` | C→S | chunked XML + `{sha256, baseRev, force}` | `{ok, rev, symbolTable}`, `RevConflict{rev}` if the controller's project changed since `baseRev` (and `force` is false), or `Error{loadErrors[]}`; on success the program restarts with the new project | Engineer |
| `DownloadProject` | C→S | `{}` | chunked XML | Engineer |
| `ActivatePrevious` | C→S | `{}` | `{ok, symbolTable}`: re-deploys the kept previous project (same restart path as an upload) | Engineer |
| `GetSymbols` | C→S | `{}` | `{projectHash, symbols:[[iuuid, slot, type]]}` | Viewer |
| `Subscribe` | C→S | `{slots:[…], periodMs}` | `{ok}` | Viewer |
| `Unsubscribe` | C→S | `{slots:[…]}` | `{ok}` | Viewer |
| `Force` | C→S | `{slot, value, persistent}` | `{ok}` | Engineer |
| `Release` | C→S | `{slot}` or `{all:true}` | `{ok}` | Engineer |
| `ListFaults` | C→S | `{}` | `[{iuuid, reason, chain, ts}]` | Viewer |
| `ClearFault` | C→S | `{iuuid}` or `{all:true}` | `{ok}` (the instance runs again) | Engineer |
| `SetVariable` | C→S | `{slot, value}` | `{ok}` | Engineer |
| `SetParams` | C→S | `{iUuid, <Attr>: value, …}` (keys = XML attribute names) | `{ok}` or `Error` naming the refused key | Engineer |
| `GetBase` | C→S | `{}` | chunked XML of the last upload (Base, for the three-way merge) | Engineer |
| `ValueBatch` | S→C push | `{ts, v:[[slot, value, flags]]}` | — | — |
| `Log` | S→C push | `{level, module, text, ts}` | — | — |
| `HmiGetProject` | C→S | `{knownHash}` | `{unchanged:true}`, or `{hash}` + chunked **HMI view** XML (filtered by Rights), plus `{symbols:[[iuuid.port, slot, type]]}` for the `Ui` ports | HmiUser |
| `HmiGetUiFile` | C→S | `{uuid, vers, path}` | chunked file | HmiUser |
| `HmiCommand` | C→S | `{iuuid, input, value}` | `{ok}`; checked against Rights and the port's `Ui="Write"` | HmiUser |
| `HmiSetParams` | C→S | `{iUuid, <Attr>: value, …}`, e.g. `{"iUuid":"01a0f27a-…","TempComfort":"22.5"}` | `{ok}`; only `Ui="Write"` parameters the Rights allow | HmiUser |
| `State` | S→C push | `{state, projectHash, rev}` | — | — |
| `ProjectChanged` | S→C push | `{rev, hash}` after each save | — | — |
| `Fault` | S→C push | `{iuuid, reason: recursion\|crash\|hang, chain:[iuuid…], lastLog}` | — | — |
| `Error` | S→C | `{code, message, details}` (codes = `ErrorCode_e`) | — | — |

**Roles:** `Admin` ⊃ `Engineer` ⊃ `Viewer`. `HmiUser` is outside that chain: it may use only `Hello`, `Auth`, the `Hmi*` messages, and `Subscribe` / `Unsubscribe` for the slots its Rights allow.

**Upload flow (FR-13):** `CheckProject` → for each missing or mismatched package, `InstallLibrary` (Shop → Editor → Controller) → `UploadProject` → the controller loads, wires and swaps atomically → `GetSymbols` → `Subscribe`.

**NFRs:** `ValueBatch` period ≥ 50 ms by default, carrying only changed slots. An idle project produces no traffic.

### 6.2 Plugin C ABI v1 (`sdk/stix_abi.h`, contract sketch)

```c
#define STIX_ABI_MAJOR 1
typedef enum { STIX_VOID, STIX_BOOL, STIX_INT, STIX_FLOAT, STIX_STRING, STIX_ENUM, STIX_DATE } stix_type;
typedef struct { uint8_t type; uint8_t flags; uint16_t _pad; uint32_t _pad2;
                 union { int64_t i; double f; uint64_t str_handle; } v; } stix_value;   /* 16 bytes */
typedef uint32_t stix_source_id;

typedef struct {                          /* given to bind(): what is connected to each input */
    uint16_t                     n_inputs;
    const uint16_t*              n_sources;   /* n_sources[i] = number of sources on input i */
    const stix_source_id* const* sources;     /* sources[i][k] = k-th source on input i      */
} stix_bind_info;

typedef struct stix_host {                /* provided by the runtime */
    /* all functions are safe from any thread */
    void        (*emit)(void* ctx, uint16_t out_idx, stix_value v);         /* block output: calls subscribers inline */
    void        (*set_property)(void* ctx, uint16_t idx, stix_value v);     /* block changes its own parameter (ADR-017) */
    void        (*emit_signal)(void* ctx, uint32_t channel, stix_value v);  /* driver: callbacks inline on this thread */
    const char* (*source_name)(void* ctx, stix_source_id id);               /* IUuid / title, for logs */
    uint64_t    (*now_us)(void);
    void        (*log)(void* ctx, int level, const char* text);             /* lock-free ring buffer   */
} stix_host;

typedef struct stix_plugin_v1 {
    uint32_t    abi_major, abi_minor;
    const char* type_uuid;                /* == Descriptor / manifest Uuid */
    uint32_t    kind;                     /* BLOCK | PERIPHERY | PROTOCOL */
    /* blocks and drivers */
    void* (*create)  (const stix_host*, void* ctx, const stix_value* props, uint16_t n_props);
    void  (*destroy) (void* self);
    /* blocks: any thread, never two threads in the same instance at once */
    void  (*bind)    (void* self, const stix_bind_info* info);
    void  (*on_input)(void* self, uint16_t input_idx, stix_source_id from, stix_value v);
    void  (*run)     (void* self, uint64_t now_us);                    /* every TickMs, if > 0 */
    void  (*on_property)(void* self, uint16_t idx, stix_value v);      /* parameter changed at run time */
    /* drivers (their own threads) */
    int   (*open)    (void* self);
    void  (*close)   (void* self);
    void  (*write)   (void* self, uint32_t channel, stix_value v);     /* any thread; thread-safe, non-blocking (SDK queue adapter) */
} stix_plugin_v1;

const stix_plugin_v1* stix_plugin_entry(void);   /* exported, extern "C" */
```
Indexes into inputs, outputs and `props` follow the element order in `Descriptor.xml` (ADR-006). `TickMs` is read from the descriptor rather than from the plugin, so the editor can display it.

**How the two fan-in styles use this:**
- **AND:** in `bind`, allocate one bool per source per input. In `on_input`, update the entry for `from`, compute the AND of all entries, then `emit_if_changed`.
- **Lighting:** in `on_input`, act on `v` whatever `from` is (the last event wins).
### 6.3 Library Shop REST API (post-MVP, outline)
`/api/v1` · JSON · OAuth2 bearer · errors `{error:{code, message}}`
- `GET /packages?query=&kind=&page=` · `GET /packages/{uuid}` · `GET /packages/{uuid}/versions/{vers}/download` (signed URL)
- `POST /orders` · `GET /licences?serial=` · `POST /licences/{id}/activate {serial}`


### 6.4 STIx Remote relay protocol (post-MVP, outline)
Same framing as the Link (`uint32 len | CBOR {t, id, p}`), over TLS 1.3. Once a tunnel is joined, the relay passes the raw bytes through, and the STIx Link (with its own TLS) runs inside unchanged.

| `t` | Direction | Payload | Purpose |
|-----|-----------|---------|---------|
| `Register` | Controller → Relay | `{serial, certFp, runtimeVers}` | opens the outbound control connection |
| `Heartbeat` | Controller → Relay | `{serial, ts}` every 30 s | online/offline status; keeps NAT mappings open |
| `OpenTunnel` | Relay → Controller | `{tunnelId}` | the controller opens a new outbound connection and sends `TunnelReady {tunnelId}` on it |
| `SignIn` | App → Relay | `{token}` | relay account (OAuth2) |
| `Claim` | App → Relay | `{serial, claimCode}` | binds a controller to the owner's relay account |
| `ListControllers` | App → Relay | `{}` | `[{serial, name, online, lastSeen}]` |
| `Connect` | App → Relay | `{serial}` | the relay asks the controller for a tunnel, then joins it; from then on the bytes are the STIx Link (`Hello`…) |

---

## 7. FR / NFR Coverage Matrix

| ID | Type | Requirement | Component(s) | ADR(s) | Status |
|----|------|-------------|--------------|--------|--------|
| FR-01 | FR | Project lifecycle, tabs, recent | ProjectManager, Project | ADR-002, ADR-009 | Addressed (exists) |
| FR-02 | FR | Schema-driven configuration tree | SchemaManager, ProjectTree | ADR-002, ADR-004 | Addressed (exists) |
| FR-03 | FR | Place blocks, signals, variables on pages | Project, Page, QML EditorPage | ADR-002, ADR-009 | Addressed (exists) |
| FR-04 | FR | Wiring with type checks | Page, DataTypes, ProjectValidator | ADR-002, ADR-005 | Partial (`Page::connections()` stub) |
| FR-05 | FR | Schema-driven properties | Project.properties, SchemaManager | ADR-002, ADR-011 | Addressed (exists) |
| FR-06 | FR | Undo / redo | Project, project_commands | ADR-002 | Addressed (exists) |
| FR-07 | FR | Library packages (descriptor + native binary) | BlockManager, LibraryService | ADR-004, ADR-006 | Addressed (design) |
| FR-08 | FR | Encrypted controller connection | ControllerLink | ADR-007, ADR-008 | Addressed (design) |
| FR-09 | FR | Upload/download project; always running, restart on upload | ControllerLink, Runtime LinkServer/Loader | ADR-005, ADR-007 | Addressed (design) |
| FR-10 | FR | Real-time monitoring of all values | LiveValues, ControllerLink, Runtime symbol table | ADR-005, ADR-007, ADR-009 | Addressed (design) |
| FR-11 | FR | Force or simulate signals, set variables | ControllerLink, Runtime RuntimeManager | ADR-007, ADR-008 | Addressed (design) |
| FR-12 | FR | Manage controller libraries | LibraryService, ControllerRepository | ADR-004, ADR-012 | Addressed (design) |
| FR-13 | FR | Requires header + exact-version install | ProjectValidator, LibraryService | ADR-003, ADR-004, ADR-007 | Addressed (design) |
| FR-14 | FR | Library Shop | ShopClient, Shop service | ADR-012, ADR-014 | Deferred (post-MVP) |
| FR-15 | FR | Catalogue from controller | ControllerRepository, LibraryService | ADR-012 | Addressed (design, MVP) |
| FR-16 | FR | Multi-language UI | Application, QML `qsTr` | ADR-009 | Partial (`.ts`/`.qm` pipeline not set up yet) |
| FR-17 | FR | Native runtime execution | Runtime Loader / RuntimeManager / plugins | ADR-005, ADR-006 | Addressed (design) |
| FR-18 | FR | Block UI (QML) inside the package | Package `ui/`, UiPreview, HMI SandboxEngine | ADR-004, ADR-015 | Addressed (design; format in MVP) |
| FR-19 | FR | STIx HMI app for end users | STIx HMI, Link `Hmi*` messages | ADR-008, ADR-015 | Deferred (post-MVP) |
| FR-20 | FR | Remote access through a relay | STIx Remote, controller relay client, `HmiLink` | ADR-016, ADR-007, ADR-008 | Deferred (post-MVP) |
| FR-21 | FR | HMI sync on open (hash → XML → UI dependencies) | STIx HMI `UiCache`, Link `HmiGetProject` / `HmiGetUiFile` | ADR-015 | Deferred (post-MVP) |
| FR-22 | FR | Parameters edited on the controller; notify STIx Config; Overwrite or Merge on upload | Runtime instance → XML node pointer, Persistence, ProjectSync | ADR-017, ADR-006 | Addressed (design) |
| NFR-01 | NFR | Native, minimal-CPU runtime | Runtime Loader, RuntimeManager | ADR-005, ADR-006 | Addressed |
| NFR-02 | NFR | Security: TLS, auth, signed packages | ControllerLink, LinkServer, PackageStore | ADR-007, ADR-008 | Addressed |
| NFR-03 | NFR | Backward-compatible project files | Document, Project (migrations) | ADR-002, ADR-003 | Addressed |
| NFR-04 | NFR | Windows / macOS / Linux | Build, Platform | ADR-013 | Addressed |
| NFR-05 | NFR | Human-readable XML | Document writer | ADR-003, ADR-011 | Addressed |
| NFR-06 | NFR | Exact library identity | LibraryService, PackageStore | ADR-004 | Addressed |
| NFR-07 | NFR | Clear, well-structured code | all | ADR-001, ADR-009, ADR-010, ADR-011 | Addressed |
| NFR-08 | NFR | Editor responsiveness | Page, LiveValues, QML | ADR-002, ADR-009 | Partial (targets confirmed; profiling story needed) |
| NFR-09 | NFR | Monitoring latency and CPU | LiveValues, Link `ValueBatch` | ADR-007 | Addressed |
| NFR-10 | NFR | Open core + paid plugins | SDK, Shop | ADR-006, ADR-014 | Addressed |
| NFR-11 | NFR | Atomic upload / rollback | Runtime Loader, Persistence | ADR-005 | Addressed |
| NFR-12 | NFR | Extensible without recompiling | SchemaManager, BlockManager, LibraryService | ADR-004, ADR-012 | Addressed |
| NFR-13 | NFR | Unified logging | Logger, Link `Log` | ADR-010 | Addressed |
| NFR-14 | NFR | 1→N controllers per project | Data model (RootDevice) | ADR-003 | Deferred (MVP = 1 RootDevice) |
| NFR-15 | NFR | Fault containment (recursion, hang, crash) without per-call storage writes | Runtime loop guard, stix-supervisor, ProjectValidator, LiveValues | ADR-005, ADR-007 | Addressed |

### Detailed NFR notes (per driver)
- **NFR-01 (performance / scalability of the runtime):** the fast path is: the driver thread calls `emit_signal` → it walks the signal's subscriber array **on the same thread** → per connection, one atomic compare-and-swap on the instance's `busy` flag, two RAM stores for the supervisor, and one indirect `on_input` call → the block may `emit` → and so on. With no thread hand-over, the delay is set only by the callbacks themselves. There are no strings, hashes or allocations, so the cost scales with the number of *connections an event touches*, not with the size of the project. Ticks cost only for block types with `TickMs > 0`. Idle CPU is about 0: drivers wait on hardware, and the RuntimeManager sleeps until the next tick or Link command. Load and scaling: thousands of blocks fit in a few hundred KiB.
- **NFR-02 (security):** encryption in transit (TLS 1.3), authentication (Argon2id accounts and sessions), authorisation (RBAC per message), integrity of code (Ed25519-signed packages), and an audit trail of forces and uploads on the controller. Secrets are never stored in `Project.xml` except as salted hashes.
- **NFR-03 / NFR-05 (compatibility and readability):** the reader keeps unknown data, migrations are versioned, golden fixture files exist per version, and the writer output is stable and diff-friendly.
- **NFR-15 (fault containment):** the loop guard (integer compares in thread-local RAM), the supervisor records (two stores into shared RAM per plugin call) and the supervisor's crash and hang detection make a looping, hung or crashing block visible and isolated, without slowing the fast path and **without any flash writes in normal operation**. Prevention is the block developer's responsibility, helped by the editor's cycle warnings.
- **NFR-08 (editor performance):** caching per page (`Page_class`) avoids walking the whole tree. Live values are coalesced per frame. Large pages may need `Repeater` → `ListView`/`TableView`-style virtualisation; a profiling story is recommended.

### Testing strategy (planning only)
- Unit: the undo commands (do/undo symmetry), migrations (golden files), `canConnect` matrix, validator rules including cycle detection, CBOR framing.
- Contract: a mock Link server replaying recorded sessions; an ABI conformance test plugin.
- Integration (runtime): load and run sample projects with expected output traces; fault-injection plugins (endless loop, hang, segfault) that must end up quarantined and reported; a multi-thread stress test (several drivers emitting inline into shared blocks) checking that no instance is ever entered twice at once; a micro-benchmark that guards the NFR-01 targets.

---

## 8. Technology Stack

| Layer | Choice | Version | Rationale (→ driver) | ADR |
|-------|--------|---------|----------------------|-----|
| Editor UI | Qt Quick / QML, Controls Basic | Qt 6.11 | already in use; GPU-accelerated canvas for NFR-08; cross-platform for NFR-04 | ADR-009, ADR-013 |
| Editor core | C++17, Qt Core/Xml/Svg | Qt 6.11 | team expertise; `QXmlStream*` keeps documents 1:1 (NFR-03) | ADR-002 |
| Undo | `QUndoStack` | Qt 6.11 | proven, already integrated (FR-06) | ADR-002 |
| Link client | `QSslSocket` + `QCborStreamReader/Writer` | Qt 6.11 | TLS and CBOR without new dependencies (NFR-02, NFR-07) | ADR-007 |
| Secrets (optional) | QtKeychain | 0.14+ | OS keychain for session tokens | ADR-008 |
| Runtime | C++17, no Qt, OpenSSL 3, a small XML parser (pugixml) | — | small footprint and fast load (NFR-01) | ADR-005 |
| Runtime CBOR | tinycbor | 0.6 | small C library, same wire format | ADR-007 |
| Plugin ABI | C (`extern "C"`) | ABI 1.0 | stable across compilers, allows closed source (NFR-10) | ADR-006 |
| Package signing | Ed25519 (libsodium or OpenSSL) | — | small, fast signatures (NFR-02) | ADR-008 |
| HMI app | Qt Quick (Android, iOS, desktop) | Qt 6.11 | same QML as the editor and the block UIs; one codebase for all devices (FR-19) | ADR-015 |
| Remote relay | small TCP/TLS relay service; language chosen in a later ADR | — | only joins connections, so it needs little CPU per session (FR-20) | ADR-016 |
| Shop | REST/JSON over HTTPS, OAuth2; stack chosen in a later ADR | — | standard and easy to integrate | ADR-012 |
| Build / CI | CMake ≥ 3.16, GitHub Actions on 3 OSes | — | NFR-04 | ADR-013 |

**Alternatives considered:** Qt Widgets with QGraphicsView (mature node editors exist, but the QML UI is already built); Protobuf or gRPC (heavier toolchain); Lua or JS runtime scripting (breaks NFR-01).

---

## 9. Trade-off Analysis

### Trade-off: XML on disk vs binary
**Decision:** XML on disk and on the wire; binary only in runtime memory (ADR-003, ADR-005).
**Rationale:** NFR-05 and NFR-03 outweigh the parse cost, which is paid once per upload.
**Accepted:** larger files and slower load. **Revisit when:** files exceed 20 MB or upload takes more than 2 s.

### Trade-off: inline driver callbacks vs a single execution thread
**Decision:** drivers may run callbacks inline on their own thread, with the runtime serialising each instance through a `busy` flag and mailbox (ADR-005 rev. 1.2).
**Benefit:** the value reaches its targets with no thread hand-over. **Cost:** the runtime has to manage concurrency, and a slow block can delay the driver that called it. **Mitigation:** blocks must be fast (LOCKED); drivers may use a second thread of their own for heavy data; the hang detection in the supervisor. **Revisit when:** profiling shows contention on hot blocks.

### Trade-off: direct callbacks vs ordered propagation vs cyclic scan
**Decision:** direct callbacks, plus a per-type tick (ADR-005).
**Options:**
- Cyclic scan: simple, deterministic timing, but uses CPU even while idle.
- Ordered propagation: no double evaluation, but extra bookkeeping per event.
- Direct callbacks: the lowest latency and CPU use, and the simplest model for block authors.

**Rationale:** this is the owner's concept, and NFR-01 asks for minimal CPU in a system that is mostly idle. **Accepted cost:** a block reached by two paths can run twice per event, and loops are possible. Mitigation: blocks tolerate intermediate values; the depth guard and quarantine (NFR-15). **Revisit when:** double evaluation causes real bugs, or hard real-time is required.

### Trade-off: C ABI vs C++ interfaces for plugins
**Decision:** C ABI (ADR-006). **Benefit:** compiler independence and closed-source plugins. **Cost:** more verbose plugin code. Mitigation: a header-only C++ helper in the SDK wraps the C table.

### Trade-off: editor as package broker vs direct controller download
**Decision:** the editor is the broker (ADR-012). **Benefit:** works with offline controllers and keeps licence checks in one place. **Cost:** the editor must be present to update a controller. **Revisit when:** fleet or unattended updates are needed.

### Trade-off: certificate pinning (TOFU) vs a PKI
**Decision:** pinning by default, with an optional STI CA later (ADR-008). **Benefit:** no setup for community users. **Cost:** the first connection must be checked by the user. Mitigation: the fingerprint is printed on the device label or console.

---

## 10. Deployment Architecture

### Environments
- **Development:** the editor runs against a local runtime (Linux x86-64 or a Docker container) in developer mode, which accepts unsigned packages.
- **Staging:** a test controller with signed packages from a "beta" channel of the shop.
- **Production:** installer sites with STI or third-party controllers.

### Topology
Editor on the integrator's PC ↔ (LAN or VPN, TLS) ↔ one controller per site ↔ (LAN, TLS) ↔ the end users' phones and tablets running STIx HMI. Away from the site, the apps reach the controller through STIx Remote: the controller keeps one outbound connection to the relay, and the relay joins each session to it. The controller needs internet access only when remote access is turned on. The shop is a single cloud service with package storage and a CDN. The controller does not need internet access.

### Strategy
- **Editor releases:** signed installers per OS (ADR-013), SemVer releases, and an update check against GitHub Releases or the shop.
- **Runtime releases:** a Debian package / OS image. systemd starts `stix-supervisor`, which starts the runtime. Upgrades keep `/var/lib/stix`.
- **Relay:** a cloud service in several regions, scaled horizontally; it stores accounts, claims and online status, but no project data.
- **HMI releases:** Google Play, the App Store (subject to the iOS risk in ADR-015) and desktop installers for kiosk tablets.
- **Project deployment:** the program always runs, and an upload restarts it with an atomic swap, keeping a previous-version slot (ADR-005). Rollback is `ActivatePrevious` from the editor.
- **Scaling:** one controller per project for now (NFR-14). The shop scales horizontally behind the CDN.

---

## 11. Future Considerations

- **Near term:** set up the translation pipeline (`qt_add_translations`); the cleanup story for the ADR-011 violations; `ProjectValidator`.
- **Medium term:** the shop (FR-14); the STIx HMI app (FR-19, FR-21); the STIx Remote relay (FR-20); push notifications for the `Notification` block through the relay; multiple `RootDevice`s with cross-controller signals over the Link; mutual TLS.
- **Offline simulation:** linking the runtime executor into the editor would need native plugin builds for desktop. A possible path is plugins that ship an optional `lib/<desktop-target>/` build and run in a separate simulator process (keeping ADR-001).
- **Interoperability:** an OPC UA or Modbus-server *Protocol package* that exposes variables to SCADA systems.
- **Revisit triggers (from the ADRs):** project files > 20 MB; hard real-time needs; double evaluation causing bugs; untrusted plugins needing a sandbox; fleet management; a second developer joining; a move to C++23; browser clients.

---

## Appendix

### Glossary
| Term | Definition |
|------|------------|
| IUuid | Instance UUID of one element in a project. Target of every reference. |
| Uuid | Type UUID of a block, periphery or schema NodeType; stable across versions of a package. |
| Slot | The runtime's storage cell for one value (output, signal or variable). |
| SourceId | Numeric id of a source (block output, input signal, variable), passed to every `on_input`. |
| Subscriber array | Per source, the list of `{instance, on_input, input}` entries it calls when it emits. |
| TickMs | Period of a block type's `run()` callback, declared in its descriptor; 0 means never ticked. |
| Quarantine | The state of a block instance stopped after a recursion, hang or crash; cleared with `ClearFault` or a new upload. |
| stix-supervisor | A small parent process that watches the runtime through shared RAM, detects crashes and hangs, quarantines the faulty instance and restarts the runtime. |
| Mailbox | A per-instance lock-free queue that holds events arriving while the instance is busy. |
| Instance | One object created per block, protocol or driver in the project, with its own parameters and local variables. |
| HMI | Human-machine interface: the STIx HMI app where end users operate the installation. |
| `stixBlock` | The single object a block's QML UI receives: `value`, `property`, `send`. |
| Rev | The project's revision number, incremented by the controller each time it saves parameter values changed at run time. |
| HMI view | A filtered copy of the project XML, generated by the controller for one HMI user: layout, allowed instances and `Ui` ports, with no secrets. |
| STIx Remote | The cloud relay that joins a remote app to a controller registered by serial number; it only forwards encrypted bytes. |
| Package (`.stixpkg`) | A signed zip holding a block, periphery or protocol: descriptor plus binaries. |
| STIx Link | The editor↔controller TLS protocol (ADR-007). |
| Requires | The project header listing exact package versions (ADR-003). |

### References
- PRD: not yet written (the requirements baseline is in §0)
- Decision log: `bmad-output/decision-log.md`
- Project context: not yet written; consider `bmad-output/project-context.md`

### Document History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-11-05 | Smart Tech Innovation | Initial architecture from the stakeholder interview and code review |
| 1.1 | 2026-11-05 | Smart Tech Innovation | ADR-014 accepted; ADR-005/006 revised to the owner's direct-callback model (fan-in decided by the block, per-type `TickMs`, periphery threads, fault quarantine); many sources per input; NFR-15 added |
| 1.2 | 2026-11-05 | Smart Tech Innovation | ADR-005/006 rev. 1.2: drivers run callbacks inline (`emit_signal`); per-instance busy flag + mailbox; crash and hang detection by `stix-supervisor` over shared RAM (no per-call flash writes); unsynchronised signals confirmed as intended; NFR targets confirmed |
| 1.3 | 2026-11-05 | Smart Tech Innovation | `publish` removed: `emit_signal` is the only way to send a signal value, from driver threads and from the RuntimeManager alike; no runtime event queue |
| 1.4 | 2026-11-05 | Smart Tech Innovation | FR-09: no start/stop, restart only on upload (`ActivatePrevious` added); FR-15 moved into the MVP; every block, protocol and driver is an object instance; FR-18/FR-19 and ADR-015: QML UI in packages + STIx HMI app; `HmiUser` role |
| 1.5 | 2026-11-05 | Smart Tech Innovation | ADR-016 + FR-20: STIx Remote relay (registered by serial, heartbeats, end-to-end TLS); FR-21: HMI sync on open (project hash → HMI view XML → UI dependencies → download from controller); `HmiGetModel` replaced by `HmiGetProject`; relay protocol §6.4 |
| 1.6 | 2026-11-05 | Smart Tech Innovation | Driver 1: XML → instances (objects) → parameters → callbacks. ADR-017 + FR-22: the controller changes parameter values at run time (`RuntimeEdit`, `on_property`, `set_property`), saves them into the XML (debounced, atomic, `Rev`), keeps a change journal; STIx Config's `ProjectSync` takes them in before uploading |
| 1.7 | 2026-11-05 | Smart Tech Innovation | ADR-017 rev. 1.1 (owner's concept): message `{iUuid, <Attr>: value}`; each instance holds a pointer to its XML node; `ProjectChanged` tells STIx Config to download; upload over changes → Overwrite or three-way Merge by `IUuid`; journal and `RuntimeEdit` removed (`Ui="Write"` used instead) |
| 1.8 | 2026-11-05 | Smart Tech Innovation | Merge rule: when the engineer and a user changed the same attribute, the engineer's value wins (no conflict dialog). STIx Link stays on CBOR. |
| 1.9 | 2026-11-05 | Smart Tech Innovation | `GetLibraryFile` added to STIx Link (additive, needed by FR-15 / story 8.1); ADR-011 note on descriptors corrected to the full scan result |

---

**END OF DOCUMENT.** Ready for handoff to bmad-epics-and-stories once validation passes.
