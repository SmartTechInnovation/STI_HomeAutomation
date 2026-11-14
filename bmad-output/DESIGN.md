# DESIGN.md — Visual System

> **LOCKED PLANNING ARTIFACT.** The external dev tool reads this document but must
> not edit it. All design changes go through the bmad-ux skill and are recorded in
> `decision-log.md`.

**Project:** STIx Config (desktop editor of the STIx platform)
**Track:** BMad Method (MVP)
**Date:** 2026-11-05
**Version:** 1.0
**Theme:** dark only (a light theme stays possible through the tokens, but is out of scope)
**Brand source:** smartinnovation.md (brand colours and the Barlow font)

**Adaptation note:** the skill's template targets websites. STIx Config is a Qt Quick desktop app, so this document maps the same contract to Qt:
- ARIA becomes the QML `Accessible` attached properties;
- phone breakpoints become window size classes;
- 44 px touch targets become 24 px pointer targets, with documented canvas exceptions.

Every token below has a concrete value, and every text pair was checked with `contrast-check.py`.

---

## 0. Design principles

1. **Library-first, nothing hard-coded.** Blocks, peripherals and protocols come from packages. The UI never assumes a fixed set of block kinds. A block's colour, icon, ports and docs come from its descriptor, so a new package looks right with no app update. This is how the "extensible, decentralised, Arduino-like but graphical" vision appears in the UI.
2. **The page is paper; the tool is dark.** Program pages render as light paper (as they print and export to PDF) inside a dark, calm tool frame. Colour inside the tool marks **state** (live, forced, fault, selection), never decoration.
3. **Dense but readable for hours.** A professional configurator: compact rows, keyboard everywhere, no wasted chrome. Text never drops below 11 px at 100 % zoom.
4. **One accent, used sparingly.** Brand blue marks *the* primary action and selection; brand green marks *healthy / connected / on*. Everything else is neutral.
5. **Guidance for newcomers without slowing experts.** Empty states teach, tooltips show the block docs, templates give a starting point. Experts can ignore all of it through shortcuts.

---

## 1. Design Tokens

Tokens live in `resources/qml/Components/Theme/Theme.qml` as `readonly property` values. The names below are the QML property names. Never hard-code a colour or size in a component.

### 1.1 Colour palette

**Brand (from smartinnovation.md)**

| Token | Value | Usage |
|-------|-------|-------|
| `brandBlack` | `#272727` | main window background |
| `brandSurface` | `#323232` | cards, inputs, menus |
| `brandBlue` | `#3D5ADD` | primary button fill, selection fill |
| `brandGreen` | `#A4D874` | connected / healthy / Bool true / success |
| `brandWhite` | `#F7F7F7` | primary text |
| `brandGrayDark` | `#5A616E` | default borders |
| `brandGray` | `#7D8491` | strong borders, disabled text |
| `brandGrayLight` | `#A8AEB9` | secondary text |

**Primary (blue scale)**

| Token | Value | Usage |
|-------|-------|-------|
| `primary` | `#3D5ADD` | button fill, selected row fill (with white text) |
| `primaryHover` | `#3550C9` | hover on primary fill |
| `primaryPressed` | `#2D44AD` | pressed |
| `primaryText` | `#8095F2` | blue text, links, icons and **focus ring** on dark (5.33 : 1) |
| `primaryTint` | `#3D5ADD33` | selection background behind neutral text, drop targets |

**Semantic**

| Token | Value | Usage |
|-------|-------|-------|
| `success` | `#A4D874` | connected, OK, validation passed |
| `warning` | `#E8C547` | warnings, cycle warnings, "program differs" |
| `error` | `#FF6568` | error text and icons on dark |
| `errorFill` | `#D92430` | destructive button fill, error badge fill |
| `forced` | `#FF8400` | forced values (badge, wire dash) |
| `info` | `#8095F2` | information (same as `primaryText`) |

**Neutral scale (dark surfaces, darkest → lightest)**

| Token | Value | Usage |
|-------|-------|-------|
| `bgChrome` | `#1C1C1C` | project tab strip, status bar |
| `bgCanvas` | `#202020` | editor workspace around the pages |
| `bgWindow` | `#272727` | main background (brand black) |
| `bgPanel` | `#2D2D2D` | side panels, log dock |
| `bgSurface` | `#323232` | inputs, cards, list rows (brand surface) |
| `bgElevated` | `#3A3A3A` | menus, popups, dialogs, tooltips |
| `bgHover` | `#424242` | hover on rows and tool buttons |
| `borderSubtle` | `#3A3A3A` | dividers between regions |
| `borderDefault` | `#5A616E` | input and card borders (UI contrast ≥ 3 : 1 on panels) |
| `borderStrong` | `#7D8491` | hovered inputs, block frame on the canvas |
| `textPrimary` | `#F7F7F7` | main text |
| `textSecondary` | `#A8AEB9` | labels, metadata, placeholders |
| `textDisabled` | `#7D8491` | disabled text only (exempt from contrast rules) |

**Paper (program pages, as printed)**

| Token | Value | Usage |
|-------|-------|-------|
| `paper` | `#F9FBFF` | page background |
| `paperFrame` | `#D9DCE1` | page border, title-block lines, grid dots |
| `ink` | `#272727` | text on paper |
| `inkMuted` | `#5A616E` | secondary text on paper (port names, units) |

**Data types (ports and wires; all ≥ 3.6 : 1 on `paper`, all distinct)**

| Token | Type | Value | On paper |
|-------|------|-------|----------|
| `dtBool` | Bool | `#C62828` | 5.43 |
| `dtInt` | Int | `#00897B` | 4.17 |
| `dtFloat` | Float | `#558B2F` | 3.96 |
| `dtString` | String | `#AD1457` | 6.72 |
| `dtEnum` | Enum | `#6A1B9A` | 9.06 |
| `dtDate` | Date | `#E65100` | 3.66 |
| `dtCustom` | Custom | `#5D4037` | 8.99 |
| `dtVoid` | Void | `#546E7A` | 5.21 |

> These replace the colours in `DataTypes_class` (`#A9C7FF`, `#13DB9D`, `#A0F032`, `#FCD116` measure 1.3–1.7 : 1 on paper and are nearly invisible). `DataTypes_class::color()` stays the single source; it returns these values.

**Canvas states (on paper)**

| Token | Value | Usage |
|-------|-------|-------|
| `selection` | `#3D5ADD` | selected block outline (2 px), selected wire |
| `liveOn` | `#A4D874` | halo behind a wire or port whose Bool is true (decorative; the value label carries the meaning) |
| `forcedMark` | `#E65100` | forced wire dash and badge on paper |
| `faultMark` | `#C62828` | quarantined block outline and badge on paper |

### 1.2 Typography

**Font families (bundled, so every OS renders the same)**

| Role | Family | Files | Licence |
|------|--------|-------|---------|
| UI and canvas | **Barlow** (the brand font) | Regular 400, Medium 500, SemiBold 600 | SIL OFL 1.1 |
| Values, IDs, logs | **JetBrains Mono** | Regular 400 | SIL OFL 1.1 |

The fonts are loaded once at startup (`QFontDatabase::addApplicationFont` from `resources/fonts/`) and exposed as `Theme.fontUi` / `Theme.fontMono`. This also removes today's "Sans Serif" alias warning (a missing family that costs 33 ms at startup).

**Type scale (desktop; px at 100 % interface scale)**

| Token | Size | Weight | Line height | Usage |
|-------|------|--------|-------------|-------|
| `textDisplay` | 24 | 600 | 32 | home page title |
| `textTitle` | 18 | 600 | 24 | dialog titles, page names in headers |
| `textHeading` | 15 | 600 | 20 | panel headers, section titles |
| `textBody` | 14 | 400 | 20 | default text, inputs, tree rows |
| `textLabel` | 13 | 500 | 18 | field labels, buttons, tabs |
| `textSmall` | 12 | 400 | 16 | metadata, status bar, log rows |
| `textMicro` | 11 | 500 | 14 | port short names and values on the canvas at 100 % zoom (they scale with zoom) |
| `textMono` | 13 | 400 | 18 | values, IUuids, log details |

An **interface scale** setting (90 %, 100 %, 115 %, 130 %, 150 %) multiplies every size and spacing token. Canvas zoom is separate.

### 1.3 Spacing scale (4 px base, 8 px rhythm)

| Token | Value | Common use |
|-------|-------|------------|
| `space1` | 2 | icon-to-badge gaps |
| `space2` | 4 | inline gaps, port spacing inside a block |
| `space3` | 8 | padding of compact rows and buttons |
| `space4` | 12 | gap between form fields |
| `space5` | 16 | panel padding |
| `space6` | 24 | dialog padding, section gaps |
| `space7` | 32 | home page margins |
| `space8` | 48 | empty-state blocks |

**Fixed sizes**

| Token | Value |
|-------|-------|
| `rowHeight` | 28 (tree rows, list rows, property rows) |
| `controlHeight` | 28 (inputs, combo boxes, buttons) |
| `toolButtonSize` | 32 (top toolbar) |
| `tabHeight` | 32 (project tabs) |
| `statusBarHeight` | 24 |
| `iconSmall` / `iconMedium` / `iconLarge` | 16 / 20 / 24 |
| `portPitch` | 20 (vertical distance between ports at 100 % zoom; equals the 10-unit `WORKSPACE_SCALE` grid × 2) |

### 1.4 Window size classes (replace web breakpoints)

| Class | Window width | Behaviour |
|-------|--------------|-----------|
| Minimum | 1280 × 720 | the smallest supported window; both side panels may be open, at their minimum widths |
| Compact | 1280–1599 | opening the second side panel collapses the log dock to its tab bar |
| Regular | 1600–2559 | default layout: tree 280, properties 320, log dock 200 |
| Wide | ≥ 2560 | panels keep their widths; the extra space goes to the canvas |

Side panels are resizable (tree 220–480, properties 260–520) and collapsible. Sizes are remembered per user.

### 1.5 Elevation

On dark surfaces, elevation is mostly the surface colour plus a border. Shadows are subtle and only used for floating layers.

| Level | Surface | Border | Shadow | Used by |
|-------|---------|--------|--------|---------|
| 0 | `bgWindow` / `bgPanel` | — | none | regions |
| 1 | `bgSurface` | `borderSubtle` | none | inputs, cards, list rows |
| 2 | `bgElevated` | `borderSubtle` | 0 4 12 `#00000066` | menus, popovers, tooltips, blocks menu |
| 3 | `bgElevated` | `borderDefault` | 0 12 32 `#00000080` | dialogs, notices |
| Canvas | `paper` | `paperFrame` | 0 2 8 `#00000059` | the page on the canvas |

### 1.6 Border radius

| Token | Value | Usage |
|-------|-------|-------|
| `radiusXs` | 2 | ports, small badges |
| `radiusSm` | 4 | inputs, chips, tool buttons |
| `radiusMd` | 6 | buttons, blocks on the canvas, list rows when selected |
| `radiusLg` | 8 | panels' floating parts, menus, dialogs, home cards |
| `radiusFull` | 999 | pills (signal references, connection badge) |

### 1.7 Motion

| Token | Duration | Easing | Usage |
|-------|----------|--------|-------|
| `motionFast` | 100 ms | OutCubic | hover and press colour changes |
| `motionMedium` | 180 ms | OutCubic | panel collapse, menu open, notice slide |
| `motionSlow` | 260 ms | InOutCubic | page change, fit-to-page zoom |

Canvas edits (move, connect, live values) are **never animated**: they follow the pointer or the data immediately. A **"Reduce motion"** setting sets every duration to 0. The OS has no portable reduced-motion flag reachable from Qt, so the setting is in-app.

---

## 2. Component Specifications

States use these rules everywhere, unless a component says otherwise:
- **Hover:** `bgHover` fill or `borderStrong` border.
- **Focus:** a 2 px `primaryText` ring, offset 2 px; never removed.
- **Pressed:** one step darker.
- **Disabled:** `textDisabled` text, no hover, `Accessible.description` says why.
- **Loading:** inline spinner 16 px + label.
- **Error:** `error` border and message.

### 2.1 Buttons

| Variant | Default | Hover | Pressed | Disabled | Use |
|---------|---------|-------|---------|----------|-----|
| **Primary** | `primary` fill, `#FFFFFF` text (5.67 : 1) | `primaryHover` | `primaryPressed` | `bgSurface` fill, `textDisabled` | the one main action of a dialog or area (Upload, Connect, Save) |
| **Secondary** | `bgSurface` fill, `borderDefault` 1 px, `textPrimary` | `bgHover` | `bgElevated` | as above | other actions |
| **Ghost** | transparent, `textSecondary` | `bgHover`, `textPrimary` | `bgElevated` | `textDisabled` | low-emphasis (Cancel, More) |
| **Destructive** | `errorFill` fill, `#FFFFFF` text (4.95 : 1) | `#C01F2A` | `#A61B24` | as above | Delete, Overwrite, Remove package |
| **Tool** (`ToolBtn`) | 32 × 32, icon 20 `textSecondary`, transparent | `bgHover`, icon `textPrimary` | `bgElevated` | icon `textDisabled` | top toolbar; tooltip shows the name + shortcut |

- Height `controlHeight` (28); horizontal padding `space4`; label `textLabel`; icon + label gap `space2`.
- Loading: the label is replaced by a spinner + "Uploading…"; the width is kept.
- `Accessible.role: Accessible.Button`, `Accessible.name` = label (or tooltip for icon-only buttons).

### 2.2 Text, number and unit inputs

- `controlHeight` 28; `bgSurface` fill; `borderDefault` 1 px; `radiusSm`; padding 0 `space3`; text `textBody`; placeholder `textSecondary`.
- Focus: `primaryText` border + focus ring. Error: `error` border, message below in `textSmall` + `error`, with an icon. Read-only (e.g. `ConfigVers`): no border, `textSecondary`, lock icon on hover.
- **Number with unit** (`DarkSpin`): value right-aligned in `fontMono`, unit suffix in `textSecondary`; arrow keys ±1, Shift ±10; wheel only when focused.
- `Accessible.name` = the field label; error text in `Accessible.description`.

### 2.3 Combo box, checkbox, switch, reference picker

- **Combo box (Enum):** like an input, with a chevron; the popup is level 2; type to filter when there are more than 8 items.
- **Checkbox (`CheckBoxCustom`):** 16 × 16, `radiusXs`, `borderDefault`; checked = `primary` fill + white tick; hit area 24 × 24 with the label clickable.
- **Switch (Bool property):** 32 × 16 track; on = `success` track with a `brandBlack` knob.
- **Reference picker (`Type="Ref"`):** a combo box listing the target nodes with their icons, plus "None"; a "go to" icon jumps to the referenced node.

### 2.4 Panels (Project tree, Properties, Log dock)

- Background `bgPanel`; header 32 px with `textHeading`, collapse button and an optional search field; divider `borderSubtle`.
- **Tree rows:** `rowHeight` 28, indent 16 per level, icon 16 + label `textBody`; selected = `primaryTint` fill + `primaryText` left bar 2 px; context menu on right-click and the Menu key.
- **Property rows:** label column (40 %, `textSecondary`, ellipsis + tooltip) and editor column; groups (`Group Title`) are collapsible sections with a `textLabel` header.
- **Log rows:** `textSmall`; severity icon (info `info`, warning `warning`, error `error`); time in `fontMono`; clicking a row with an `iUuid` selects the instance on its page.

### 2.5 Project tabs (`TabStrip` / `Tab`)

- `tabHeight` 32 on `bgChrome`; tab = `textLabel` + a dot when modified + close button on hover; active tab = `bgWindow` fill joined to the toolbar below, with a 2 px `primaryText` top line.
- Drag to reorder; middle-click closes; Ctrl/Cmd+W closes; overflow shows a chevron menu.

### 2.6 Canvas: page, block, port, signal, wire

**Page (paper):** `paper` fill, `paperFrame` border and title block (project, page, author, date, as printed), grid dots every 10 units in `paperFrame`, page shadow (canvas elevation). Zoom 10–400 %; Fit page = F; zoom to selection = Shift+F.

**Block**

| Part | Spec |
|------|------|
| Body | `#FFFFFF` fill, 1 px `borderStrong`, `radiusMd`; width from the descriptor (`Width`, in workspace units) |
| Header | 24 px, filled with the descriptor `Color`; title `textLabel` in `#FFFFFF` or `ink`, whichever has ≥ 4.5 : 1 on that colour (computed); category icon 16 at the left |
| Ports | inputs on the left edge, outputs on the right, `portPitch` 20; short name `textMicro` + `inkMuted` inside the body; hidden ports (`Visible="false"`) appear through "Show all ports" |
| Selected | 2 px `selection` outline, outside the body |
| Fault (quarantined) | 2 px `faultMark` outline + a fault badge in the header; tooltip shows the reason |
| Missing package | hatched header in `inkMuted`, "missing package" badge |

**Port**

| State | Visual |
|-------|--------|
| Default | 8 × 8 square (`radiusXs`) in the data-type colour; hit area 20 × 20 |
| Hover | 10 × 10 + tooltip: title, type, unit, docs |
| Connected | filled; unconnected = 1.5 px outline only |
| Dragging a wire, compatible | grows to 12 × 12 with a `primaryTint` halo |
| Dragging a wire, incompatible | 30 % opacity (from C++ `canConnectTo`) |
| Live (monitoring) | value label next to the port in `fontMono` `textMicro`; Bool shows a filled `liveOn` halo when true |
| Forced | `forcedMark` badge "F" and the value in `forcedMark` |

**Signal reference (SignalInRef / SignalOutRef):** a pill (`radiusFull`, height 20) with the channel or variable name, an In or Out arrow, and its data-type colour as a 3 px left bar. A cross-page source shows a page-link icon.

**Wire:** cubic curve from port to port; 1.5 px in the data-type colour.
- Hover: 2.5 px. Selected: 2.5 px in `selection`.
- Live true: a 5 px `liveOn` halo under the stroke. Forced: dashed 4/3 in `forcedMark`.
- Several wires into one input fan in with a 6 px spread, so each stays selectable.

### 2.7 Blocks menu (palette)

- A level-2 popover from the toolbar or the Space key on the canvas; a search field first (focus on open); categories from the library (`Category` + icon), each listing block tiles: icon, title, source badge (Controller / Local / Templates).
- Enter places the block at the cursor; dragging a tile drops it on the page.
- Empty search shows "No block matches '…' · Browse the library".

### 2.8 Dialogs and notices

- **Dialog:** level 3, `radiusLg`, padding `space6`, max width 560 (wide variant 800 for the merge summary). Title `textTitle`; actions right-aligned (primary last); Escape = cancel, unless the dialog is a security decision (trust fingerprint), where Escape = decline.
- **Notice (non-blocking):** level 3, top-right of the canvas, width 360, with an icon, title, text and up to 2 actions. It auto-hides after 8 s only if it has no action. Used for "Project edited on the controller (rev N)", "Upload finished", "Connection lost".
- **Banner (inside a page area):** full width of the area, `bgElevated`, a 3 px left bar in the semantic colour. Used for "Read-only: this project was made by a newer STIx Config", "Monitoring paused: the project differs from the controller".

### 2.9 Status bar and connection badge

- `statusBarHeight` 24 on `bgChrome`, `textSmall` `textSecondary`. From left to right: connection badge, controller name / serial, role, validation summary (errors and warnings, clickable), "Rev N", zoom %.
- **Connection badge** (pill):
  - disconnected: `textSecondary` outline;
  - connecting: `warning` with a spinner;
  - connected: `success` dot + "Connected";
  - error: `error` dot + a short reason.

  The same badge appears in `ControllerConnection.qml`.

### 2.10 Empty, loading and skeleton states

- **Empty:** a 48 px icon in `textDisabled`, a `textHeading` sentence, a `textBody` hint and one primary action. Examples: "No pages yet: Add page", "No blocks match", "Not connected: Connect to controller".
- **Loading:** a spinner + text for operations under 2 s; a progress bar with a percentage and Cancel for uploads, downloads and installs.
- Skeleton rows are used only in the library list while a controller catalogue loads.

### 2.11 Home page

- Left column 280: "New project", "Open…", "From template…" (post-MVP), Library. Main area: a search field and the **recent projects** as cards (`radiusLg`, `bgSurface`, title `textHeading`, location and dates `textSmall`, a thumbnail of page 1 when available).
- Empty state for a first start: "Create your first project", with three short steps (configure the controller, draw the logic, upload) and the "New project" primary button.

---

## 3. Accessibility contract (WCAG 2.1 AA, mapped to Qt Quick)

### 3.1 Colour contrast: verified pairs (contrast-check.py)

| Pair | Ratio | AA normal | AA large | Notes |
|------|-------|-----------|----------|-------|
| `textPrimary` on `bgWindow` | 13.94 | ✅ | ✅ | |
| `textPrimary` on `bgElevated` | 10.62 | ✅ | ✅ | darkest-contrast surface for main text |
| `textSecondary` on `bgWindow` / `bgPanel` / `bgSurface` / `bgElevated` | 6.70 / 6.18 / 5.75 / 5.10 | ✅ | ✅ | labels, placeholders |
| `textDisabled` (`#7D8491`) on `bgSurface` | 3.41 | ❌ | ✅ | **disabled text only** (WCAG exempt); borders ≥ 3 : 1 ✅ |
| `#FFFFFF` on `primary` (primary button) | 5.67 | ✅ | ✅ | |
| `primaryText` on `bgWindow` / `bgSurface` | 5.33 / 4.58 | ✅ | ✅ | links, focus ring, info |
| `brandBlue` `#3D5ADD` as text on `bgWindow` | 2.64 | ❌ | ❌ | **never** used as text or icon on dark |
| `#FFFFFF` on `errorFill` (destructive) | 4.95 | ✅ | ✅ | |
| `#F7F7F7` on brand red `#FB2C36` | 3.55 | ❌ | ✅ | **not** used for buttons |
| `error` `#FF6568` on `bgWindow` / `bgSurface` | 5.20 / 4.47 | ✅ / ⚠ | ✅ | on `bgSurface` and lighter, use it at ≥ 14 px SemiBold (counts as large text), or on `bgPanel` and darker |
| `success` on `bgWindow` | 9.00 | ✅ | ✅ | |
| `warning` on `bgWindow` | 8.90 | ✅ | ✅ | |
| `brandBlack` on `success` / `warning` / `forced` badges | 9.00 / 8.90 / 6.08 | ✅ | ✅ | |
| `ink` on `paper` | 14.42 | ✅ | ✅ | |
| `inkMuted` on `paper` | 6.02 | ✅ | ✅ | port names |
| data types on `paper` | 3.66–9.06 | — | ✅ | graphics ≥ 3 : 1 ✅ (wires and ports are UI graphics) |

**Colour is never the only signal.** Each state has a second cue:
- data type: the port tooltip and the type in the properties panel;
- live Bool: the value label;
- forced: the "F" badge;
- fault: the badge plus its tooltip;
- connection: the badge text.

### 3.2 Keyboard and focus

- Every action has a keyboard path. Tab order: tab strip → toolbar → tree → canvas → properties → log. F6 cycles between regions.
- Focus is always visible (2 px `primaryText` ring). No component sets `activeFocusOnTab: false` without another keyboard route.
- **Canvas keyboard:**
  - arrows move the selection by one grid step (Shift = 10 steps);
  - Tab cycles the blocks of the page, Enter opens the block's properties;
  - **C** starts a connection from the selected port and lets you choose the target with the arrows;
  - Delete removes the selection; Space opens the blocks menu;
  - Ctrl/Cmd + Z / Shift + Z undo and redo;
  - F fits the page.
- Dialogs trap focus. Escape cancels (or declines, for the trust dialog). Focus returns to the control that opened them.
- **Single-key shortcuts** (Space, C, F, arrows, Delete) are active **only while the canvas has focus** (WCAG 2.1.4). They never fire while typing in a field, and the Shortcuts menu lists them.
- Notices with an action never auto-hide (WCAG 2.2.1); those without an action stay at least 8 s and pause on hover.

### 3.3 Pointer targets and sizing

- Minimum pointer target: **24 × 24** (tool buttons 32 × 32). Gaps between targets ≥ 4 px.
- **Canvas exception:** ports draw at 8 × 8 but have a 20 × 20 hit area at 100 % zoom. Below 60 % zoom, connecting is disabled and the hint "Zoom in to connect" is shown.
- Interface scale up to 150 % without loss of function (§1.2); the minimum window is 1280 × 720 at 100 %.

### 3.4 Semantics (spec for the dev, Qt Quick)

- Each interactive item sets `Accessible.role` (Button, CheckBox, ComboBox, EditableText, List, ListItem, Tree, TreeItem, PageTab, Dialog, AlertMessage…), `Accessible.name` and, where needed, `Accessible.description`.
- Error messages use `Accessible.role: Accessible.AlertMessage`. Notices announce their title.
- Icon-only controls get `Accessible.name` from their tooltip text.
- Blocks expose `Accessible.name: "<title>, <type>, <n> inputs, <m> outputs"`; ports expose `"<title>, <type>, <value if live>"`.
- Every string goes through `qsTr()` (FR-16).

---

## 4. Design Decisions

| Decision | Rationale | Alternatives considered |
|----------|-----------|------------------------|
| Dark tool frame, light paper pages | The pages print and export to PDF unchanged, and the dark frame matches the brand and long working sessions | All dark (pages would need a separate print style); all light (off-brand, more glare) |
| `primaryText` `#8095F2` for blue text and focus on dark | Brand blue measures 2.64 : 1 on dark; the tint keeps the brand hue at 5.33 : 1 | Using brand blue everywhere (fails AA) |
| `errorFill` `#D92430` for destructive buttons | White on brand red `#FB2C36` is 3.55 : 1 | Dark text on brand red (unusual for destructive actions) |
| New data-type palette | Today 4 of the 6 colours measure 1.3–1.7 : 1 on paper, and Enum, Date and Custom share one colour | Keep the colours and add outlines (more visual noise) |
| Bundled Barlow + JetBrains Mono | Brand font, the same rendering on all 3 OSes, removes the missing "Sans Serif" alias; both SIL OFL | System fonts (different per OS, off-brand) |
| Window size classes instead of web breakpoints | A desktop tool resizes continuously; a minimum window and panel rules fit better than phone/tablet breakpoints | Template breakpoints (meaningless for this app) |
| 24 px pointer targets, with a canvas port exception | Desktop with a mouse; 44 px targets would make blocks huge | 44 px everywhere (wastes canvas space) |
| In-app "Reduce motion" | Qt has no portable OS reduced-motion flag | None |
| Block colour and icon from the descriptor | Library-first: new packages look right with no app update | A fixed colour per category in the app |

---

*Part of the BMAD Planning & Orchestrator plugin. Produced by the `bmad-ux` skill (adapted to a Qt Quick desktop application).*
