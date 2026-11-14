# EXPERIENCE.md — User Experience Plan

> **LOCKED PLANNING ARTIFACT.** The external dev tool reads this document but must
> not edit it. All UX changes go through the bmad-ux skill and are recorded in
> `decision-log.md`.

**Project:** STIx Config
**Track:** BMad Method (MVP)
**Date:** 2026-11-05
**Version:** 1.0

---

## Overview

STIx Config is a graphical programming tool for building automation, from homes to industrial sites. It is in the spirit of the Arduino IDE, but visual: the user configures the controller's hardware, draws logic with blocks on paper-like pages, uploads it, and watches it run.

It is **open and extensible**. Every block, peripheral and protocol is a package from a library, so the tool grows without app updates. Projects and groups of blocks can start from templates.

The reference for the domain is the commercial building-automation configurators the users already know, especially Loxone Config: page-based logic, hardware tree, live monitoring. STIx differs in three ways: it is open, extensible by packages, and decentralised (no fixed vendor catalogue).

**Personas covered**

| Persona | Who | Needs | Typical session |
|---------|-----|-------|-----------------|
| **P1 · Integrator** | professional who designs and programs installations, often several projects in parallel | speed, keyboard, large projects (dozens of pages, hundreds of blocks), clean printouts for the customer | 1–4 h at a desk, big screen |
| **P2 · Installer** | on site with a laptop, commissioning and fixing | connect fast, see live values, force signals, small edits, safe upload | 10–30 min, laptop, often on a poor network |
| **P3 · Technical home owner** | automates their own house, learns as they go | guidance, templates, clear errors, no fear of breaking things | evenings, short sessions |

**Platform targets:** Windows, macOS, Linux desktop; minimum window 1280 × 720; mouse and keyboard (a trackpad is common for P2).
**Design system reference:** `bmad-output/DESIGN.md`

---

## User Journeys

### Journey J1: First start and create a project

**Goal:** go from an empty app to a saved project with the controller declared.
**Primary persona:** P3 (and P1 the first time)
**Estimated time:** 2 min
**Entry points:** first launch; Home → "New project"; Ctrl/Cmd + N.
**Success criteria:** a project folder exists with `Project.xml`, one page, and a `RootDevice`.

#### Happy Path

```
[Home: empty state "Create your first project"]
      |  New project
      v
[New project tab opens: Project node selected, Properties shows Title / Author / Location]
      |  type Title
      v
[Tree shows RootDevice with its sections (Digital In, Digital Out, Virtual In/Out, Network, Messages, Periphery)]
      |  Ctrl/Cmd + S → folder dialog (first save)
      v
[Saved: tab dot cleared, project appears in Recent]
```

#### Decision Points & Alternative Paths

| Trigger | Display | Recovery |
|---------|---------|---------|
| The chosen folder already contains a project | inline error in the folder dialog result: "A project already exists in …" | choose another folder |
| No write permission | error notice with the OS reason | choose another folder |
| Closing the tab with unsaved changes | dialog: Save / Discard / Cancel | — |

#### Drop-off Risk Notes

P3 may not know what a `RootDevice` is. The tree shows a one-line description under each section the first time (from the schema `Description`), and the Properties panel shows the docs.

---

### Journey J2: Configure the hardware

**Goal:** declare the controller's inputs and outputs, and the network devices.
**Primary persona:** P1, P2
**Estimated time:** 5–20 min
**Entry points:** the Project tree, RootDevice sections; the context menu "Add …" (`MenuName` from the schema).
**Success criteria:** the channels exist with names and properties, ready to be used as signals.

#### Happy Path

```
[Tree: select "Digital Inputs"] → [context menu "Add DI" / toolbar +]
      → [new channel row in rename mode] → [type "Gate open switch"] → Enter
      → [Properties: Channel number, options] → [repeat; Ctrl/Cmd + D duplicates]
      → [Network Periphery → Protocols → Modbus TCP → set IP / port / poll times per channel]
```

#### Decision Points & Alternative Paths

| Trigger | Display | Recovery |
|---------|---------|---------|
| `Max` children reached (e.g. one protocol per port) | the add action is disabled; tooltip "Only one … allowed here" | — |
| A protocol package is missing | the "Add" menu shows it as unavailable with "Install from library" | J6 |
| Deleting a channel used by signals on pages | dialog: "Used by 3 signals on 2 pages. Delete them too?" (one undo step) | Undo |

#### Drop-off Risk Notes

Long lists of channels: support multi-select, rename with F2, and paste a list of names (one per line) to create several channels.

---

### Journey J3: Program the logic

**Goal:** build the behaviour with blocks, signals and variables, wired on pages.
**Primary persona:** P1, P3
**Estimated time:** minutes to hours
**Entry points:** Program → page tabs; the toolbar "Blocks"; Space on the canvas; dragging a channel from the tree onto the page.
**Success criteria:** the page is wired, with no validation errors.

#### Happy Path

```
[Page canvas]
  → drag "Gate open switch" from the tree onto the page  → SignalInRef pill appears
  → Space → Blocks menu → type "gate" → Enter            → Garage/Gate block at the cursor
  → drag from the pill's output to the block's input "Is open" (compatible ports highlight)
  → drag the block's "Open" output to a DigitalOut channel → SignalOutRef pill + wire
  → status bar: 0 errors · 0 warnings
```

#### Decision Points & Alternative Paths

| Trigger | Display | Recovery |
|---------|---------|---------|
| Incompatible types while dragging | incompatible ports dim; dropping there shows a notice "String cannot connect to Bool" | drop on a compatible port |
| Two sources into one input | both wires drawn, fanned in; the block decides how it combines them (docs say how) | — |
| A wiring loop | warning in the validator and an amber marker on the instances in the loop: "Loop: A → B → A. Make sure a block in the loop has memory or a delay." | fix, or accept the warning |
| Deleting a block with wires | the wires go too, in the same undo step | Undo |
| The page is too small | page properties → Paper format (A4–A0) | — |

#### Drop-off Risk Notes

- **P3:** hovering a block in the menu shows its docs; ports show their docs in a tooltip; an empty page teaches "Drag a signal from the tree or press Space to add a block".
- **P1:** every step has a shortcut, and the canvas stays at 60 fps with 500 blocks.

---

### Journey J4: Connect and deploy

**Goal:** get the project running on the controller.
**Primary persona:** P2, P1
**Estimated time:** 1–3 min
**Entry points:** the controller panel "Connect to controller"; toolbar Upload (Ctrl/Cmd + U).
**Success criteria:** the controller runs the uploaded project; `BaseRev` equals the controller's `Rev`.

#### Happy Path

```
[Connect] → (first time) [Trust dialog: serial + fingerprint → Trust]
         → [Login dialog] → [Connected badge: serial, role]
[Upload]  → [validation OK] → [dependency check: all packages present]
          → [progress bar] → [notice "Uploaded · rev 12 · the program restarted"]
```

#### Decision Points & Alternative Paths

| Trigger | Display | Recovery |
|---------|---------|---------|
| Certificate changed since pairing | blocking error "This controller's identity changed" with the old and new fingerprints | Re-pair (type the serial to confirm) or Cancel |
| Wrong password | inline error in the login dialog | retry |
| Role too low (Viewer) | Upload disabled; tooltip "Requires Engineer" | log in with another account |
| Validation errors | dialog listing them, with "Show" links; upload blocked | fix (J3) |
| Missing or mismatched packages | dependency dialog: the list (needed vs installed) → "Install and upload" | install from the local library; if unavailable → explain + link to the library |
| **The controller changed since the last upload** (`RevConflict`) | dialog: "Changed on the controller since your last upload (rev N)" → **Merge** (default) / Overwrite | Merge: automatic; summary "14 values taken from the controller · 2 user values replaced by yours" (the engineer's values win). Overwrite: second confirmation with the count of values lost |
| The controller can't load the project | dialog with the load errors linked to instances; "The controller keeps running the previous project." | fix and upload again |
| Connection lost during upload | error notice; the controller keeps the old project | Retry |

#### Drop-off Risk Notes

The merge dialog must stay understandable for P3. It shows plain sentences ("Lighting › Living room › Comfort temperature: the user set 22.5 °C, you set 21 °C → yours is kept"), not XML.

---

### Journey J5: Monitor and debug live

**Goal:** see the program run, find faults, test by forcing signals.
**Primary persona:** P2
**Estimated time:** 5–30 min
**Entry points:** toolbar "Live" (Ctrl/Cmd + L) while connected and in sync.
**Success criteria:** the user sees live values and can force signals, change parameters, and clear faults.

#### Happy Path

```
[Live on] → values appear next to ports and pills; true Bool wires glow
  → right-click an input pill → Force… → value editor → Force
       → "F" badge, the wire dashes orange; the logic reacts at once
  → Release (or Release all in the toolbar)
  → select a block → Properties → change "Comfort temperature" → "Apply on controller"
```

#### Decision Points & Alternative Paths

| Trigger | Display | Recovery |
|---------|---------|---------|
| The local project differs from the controller's | banner "Monitoring paused: the project differs from the controller" + Upload / Download | J4 or download |
| A block is quarantined (loop, crash, hang) | red outline + badge on the block; a log entry with the reason and the chain | fix the logic → Clear fault, or upload |
| A user edits a value in STIx HMI meanwhile | notice "Project edited on the controller (rev N). Download?" | Download (or merge at the next upload) |
| A force is still active at disconnect | it is released, unless "persistent" was chosen; persistent forces are listed when reconnecting | Release |

#### Drop-off Risk Notes

Forcing can be dangerous on real equipment (gates, heaters). The Force dialog shows the target and its current value, and active forces stay visible in the status bar until released.

---

### Journey J6: Manage libraries

**Goal:** add, update or remove blocks, peripherals and protocols.
**Primary persona:** P1, P3
**Estimated time:** 1–5 min
**Entry points:** Home → Library; toolbar → Library; "Install from library" links.
**Success criteria:** the package is installed locally and/or on the controller in the needed version.

#### Happy Path

```
[Library page: search] → [package row: name, kind, version, source, signed]
  → Install package… (.stixpkg) → [signature OK] → [installed]
  → (connected) "Install on controller" → [progress] → [source column shows Controller]
```

#### Decision Points & Alternative Paths

| Trigger | Display | Recovery |
|---------|---------|---------|
| Invalid signature | error "This package is damaged or not from a trusted publisher"; nothing installed | — |
| Unsigned package | warning dialog: "Unsigned development package. Install only if you trust its author." | Install anyway / Cancel |
| Same version, different build | error "A different build of 1.2.0 is already installed" | remove the other one first |
| Remove a version used by an open project or by the running program | the action is disabled with the reason | — |

---

### Journey J7: Recover from problems

| Situation | Display | Recovery |
|-----------|---------|---------|
| A project made by a newer STIx Config | the project opens **read-only**, with the banner "Made with a newer STIx Config (format 2.x). Editing is disabled." | update STIx Config |
| A project from an older format | it opens normally; at the first save a backup `Project.xml.bak-<version>` is written, and a notice says so | — |
| A broken XML file | error notice with the line number; the project doesn't open | fix the file by hand (it is readable) |
| Unknown block type (missing package) | hatched blocks with "missing package"; the validator lists them | J6 |

### Journey J8 (post-MVP): Start from a template

**Goal:** start a project, or a group of blocks, from a template (e.g. "Apartment: lighting + shading + climate", "Garage gate with photocell").
**Primary persona:** P3, P1
**Note:** templates fit the extensible, library-first vision (templates could be packages too). This needs a requirement and an ADR before stories; it is **not** in the MVP backlog. The Home page and the Blocks menu reserve space for it ("From template…", a "Templates" category).

---

## Screen / State Inventory

### Screen S1: Home

**Purpose:** start or reopen work.
**Entry from:** app start; closing the last tab.
**Exits to:** S2 Editor, S3 Library.

#### Layout Wireframe (Regular class, 1600 × 900)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ ▣ STIx Config                                                    (tab strip) │
├──────────────┬───────────────────────────────────────────────────────────────┤
│  STIx        │  Recent projects                          [ Search projects ] │
│              │  ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐  │
│ [+ New]      │  │ thumbnail  │ │ thumbnail  │ │ thumbnail  │ │ thumbnail  │  │
│  Open…       │  │ Villa Codru│ │ Office 3F  │ │ Garage     │ │ Test bench │  │
│  From        │  │ ~/Projects │ │ ~/Clients  │ │ ~/Home     │ │ ~/Lab      │  │
│  template…   │  │ edited 2 h │ │ edited 3 d │ │ edited 1 w │ │ edited 1 mo│  │
│              │  └────────────┘ └────────────┘ └────────────┘ └────────────┘  │
│  Library     │                                                               │
│  Settings    │                                                               │
├──────────────┴───────────────────────────────────────────────────────────────┤
│ ○ Not connected                                                 v0.1 · EN    │
└──────────────────────────────────────────────────────────────────────────────┘
```

Compact class: four cards per row become three; the left column stays at 240.

#### Component Hierarchy

1. Tab strip (Home tab fixed at the left)
2. Left column: primary "New project", secondary actions, navigation
3. Recent grid: search + cards (DESIGN §2.11)
4. Status bar

#### Named States

- **Default:** recent cards sorted by last edit; keyboard: arrows move between cards, Enter opens.
- **Loading:** recent list reads from `Recent.xml` (instant); thumbnails load lazily with a placeholder.
- **Empty (first start):** "Create your first project" with three steps and the primary "New project".
- **Error:** a recent entry whose folder is gone shows "Not found" + "Remove from list".
- **Success:** —
- **Disabled:** "From template…" disabled until templates exist, with the tooltip "Coming soon".

---

### Screen S2: Editor

**Purpose:** configure, program, deploy, monitor.
**Entry from:** S1, opening a file, downloading from a controller.
**Exits to:** S1 (close tab), S3.

#### Layout Wireframe (Regular class)

```
┌───────────────────────────────────────────────────────────────────────────────────────┐
│ Home │ Villa Codru ● │ Office 3F │ +                                (tab strip, 32)     │
├───────────────────────────────────────────────────────────────────────────────────────┤
│ ↶ ↷ │ 💾 │ ▦ Blocks  ⌘ Shortcuts │ ✓ Validate │ ⇪ Upload  ⇩ Download │ ◉ Live │ ⌕ │ ⚙   │ (toolbar, 40)
├──────────────────┬──────────────────────────────────────────────────────┬─────────────┤
│ PROJECT      [⌕] │ Garage │ Lighting │ Climate │ +            (pages)  │ PROPERTIES  │
│ ▾ Villa Codru    │ ┌──────────────────────────────────────────────────┐ │ Garage/Gate │
│   ▾ Main ctrl    │ │                  (paper page)                    │ │ ─────────── │
│     ▸ Digital In │ │  (Gate open ⟩──────┐   ┌───────────────┐         │ │ Title  [  ] │
│     ▸ Digital Out│ │                    └──▶│■ Garage/Gate  │         │ │ Room   [▾ ] │
│     ▸ Virtual In │ │  (Photocell ⟩─────────▶│ Io        Op  ├──▶⟨ Motor│ │ Motor  25 s │
│     ▸ Network    │ │                        │ Spo       Cl  ├──▶⟨ Close│ │ ▸ Inputs    │
│     ▸ Messages   │ │                        └───────────────┘         │ │ ▸ Outputs   │
│   ▸ Rooms        │ │                                    ┌ title block ┐│ │ ▸ Docs      │
│   ▸ Variables    │ │                                    └─────────────┘│ │             │
│   ▸ Users        │ └──────────────────────────────────────────────────┘ │             │
├──────────────────┴──────────────────────────────────────────────────────┴─────────────┤
│ LOG  Problems (0 · 1)  Controller                                           [▾]        │ (dock, 200)
│ ⚠ 21:04 Loop: Lighting › Dimmer A → Dimmer B → Dimmer A                    [Show]     │
├───────────────────────────────────────────────────────────────────────────────────────┤
│ ● Connected · STIx-0042 · Engineer │ 0 errors · 1 warning │ Rev 12 │ 100 %           │ (status, 24)
└───────────────────────────────────────────────────────────────────────────────────────┘
```

Compact class: opening Properties collapses the log dock to its tab bar. Each side panel collapses to a 32 px rail with its icon.

#### Component Hierarchy

1. Tab strip (projects)
2. Top toolbar (`TopToolBar`, `ToolBtn`, `BlocksMenu`, `ShortcutsMenu`)
3. Left panel: Project tree (`ProjectTree`) + the controller connection area (`ControllerConnection`) at its bottom
4. Centre: page tabs + canvas (`PageFrame`, `Page`, `Block`, `Port`, `Signal`, `DisplayConnections`)
5. Right panel: Properties (`Properties`)
6. Bottom dock: Log (`LogTab`): Problems (validator), Controller (runtime log), Faults
7. Status bar

#### Named States

- **Default:** last page open; the selection syncs between the tree, the canvas and Properties.
- **Loading:** opening a big project shows a progress line under the tab strip; the UI stays responsive.
- **Empty:** a project with no pages: "No pages yet" + "Add page". An empty page: "Drag a signal from the tree or press Space to add a block". Properties with nothing selected: "Select an item to see its properties".
- **Error:** validation errors → red count in the status bar; clicking it opens Problems. A broken reference shows a red dashed wire end.
- **Success:** a notice after save, upload or download (auto-hides).
- **Disabled / read-only:** the newer-format banner (J7); edit actions disabled with the reason.
- **Live:** values on the canvas, the toolbar "Live" button active, "Release all" visible when forces exist.
- **Connected but out of sync:** amber "Program differs" in the connection area, monitoring paused.

---

### Screen S3: Library

**Purpose:** see, install and remove packages, locally and on the controller.
**Entry from:** S1, S2 toolbar, "Install from library" links.
**Exits to:** back to the previous screen.

#### Layout Wireframe

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ Library                         [ Search packages ]  Source [All ▾] Kind [▾] │
│ [Install package…]                                                           │
├──────────────────────────────────────────────────────────────────────────────┤
│ ▣ Garage / Gate     Block      1.0.0   Templates · Controller  ✔ signed  ⋯   │
│ ▣ Modbus TCP        Protocol   2.3.1   Local                   ✔ signed  ⋯   │
│ ▣ My Dimmer         Block      0.1.0   Local                   ⚠ unsigned ⋯  │
└──────────────────────────────────────────────────────────────────────────────┘
```

#### Named States

- **Default:** grouped by Kind; the row menu (⋯) holds Install on controller, Remove, Show docs.
- **Loading:** skeleton rows while the controller catalogue loads; local rows appear at once.
- **Empty:** a search with no match → "No package matches '…'".
- **Error:** the controller catalogue fails to load → a banner with Retry; the local list stays.
- **Success:** a notice "Installed Modbus TCP 2.3.1 on the controller".
- **Disabled:** Remove is disabled for template packages and for versions in use (tooltip gives the reason).

---

### Screen S4: Dialogs

| Dialog | Content | Primary / secondary | Notes |
|--------|---------|---------------------|-------|
| Trust controller | serial, model, fingerprint (mono, grouped by 4), "Compare with the label on the device" | Trust / Decline | Escape = Decline |
| Identity changed | old and new fingerprints, a warning | Re-pair… / Cancel | Re-pair requires typing the serial |
| Login | user, password, "Remember session" (if keychain) | Log in / Cancel | inline error |
| Validation errors | the list with "Show" links | Close | upload stays blocked |
| Dependencies | a table: package, needed, on controller, action | Install and upload / Cancel | |
| Upload conflict | explanation, rev numbers | **Merge** / Overwrite / Cancel | Overwrite → second confirmation with the count |
| Merge summary | plain-language list of what was taken and what was kept | Close | wide variant (800) |
| Force value | target, type, current value, new value editor, "Keep after disconnect" | Force / Cancel | |
| Unsaved changes | — | Save / Discard / Cancel | |

---

## Decision Points & Alternative Paths (global)

| Trigger | Where | Display | Recovery |
|---------|-------|---------|---------|
| Network timeout (10 s) | any Link request | error notice "The controller didn't answer" | Retry |
| Session expired | any Link request | the login dialog, keeping the current screen | log in |
| The controller project changed (`ProjectChanged`) | S2 | notice with Download | Download / ignore (merge at the next upload) |
| A fault push | S2 | the block marker + a log entry + a status-bar count | Clear fault |

---

## Interaction & Animation Notes

| Interaction | Behaviour | Timing | Meaningful? |
|-------------|-----------|--------|-------------|
| Hover on rows and buttons | colour change | `motionFast` 100 ms | decorative |
| Panel collapse/expand | width slide | `motionMedium` 180 ms | meaningful (spatial continuity) |
| Notice appear/disappear | slide + fade from the top right | `motionMedium` | meaningful |
| Fit page / zoom to selection | smooth zoom | `motionSlow` 260 ms | meaningful |
| Moving blocks, drawing wires, live values | immediate, no animation | 0 | — |
| Live Bool halo | appears instantly; no pulsing | 0 | meaningful (value label also shown) |
| Spinners | rotate | continuous | meaningful (busy) |

"Reduce motion" (Settings) sets every duration to 0 and replaces spinners with a static "working…" label.

**Keyboard shortcut set (shown in `ShortcutsMenu`)**

| Action | Shortcut |
|--------|----------|
| New / Open / Save / Save as | Ctrl/Cmd + N / O / S / Shift + S |
| Undo / Redo | Ctrl/Cmd + Z / Shift + Z |
| Blocks menu | Space (on the canvas) |
| Connect from selected port | C |
| Delete | Delete / Backspace |
| Duplicate | Ctrl/Cmd + D |
| Rename | F2 |
| Fit page / zoom to selection | F / Shift + F |
| Next / previous page | Ctrl/Cmd + PgDn / PgUp |
| Next region | F6 |
| Validate | Ctrl/Cmd + Shift + V |
| Upload / Download | Ctrl/Cmd + U / Ctrl/Cmd + Shift + U |
| Live monitoring | Ctrl/Cmd + L |
| Release all forces | Ctrl/Cmd + Shift + R |

---

*Part of the BMAD Planning & Orchestrator plugin. Produced by the `bmad-ux` skill (adapted to a Qt Quick desktop application).*
