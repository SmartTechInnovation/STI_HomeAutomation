# cmake/modules

Every `*.cmake` file in this folder is included automatically by the top-level `CMakeLists.txt`, in alphabetical order, **after** the `stix_core` and `appSTIx_Config` targets are defined.

Use one file per concern, so that independent stories don't edit the same build file:

| File (planned) | Story | Purpose |
|----------------|-------|---------|
| `warnings.cmake` | 1.6 | `STIX_WERROR` option (warnings as errors in CI) |
| `translations.cmake` | 1.7 | `qt_add_translations` for EN/RU/RO/ES |
| `zip.cmake` | 4.4 | zip library for `.stixpkg` |
| `crypto.cmake` | 4.5 | Ed25519 signature verification |
| `keychain.cmake` | 6.5 | optional QtKeychain |

A module may use the targets `stix_core` (C++ core, layers L1–L3) and `appSTIx_Config` (the app, QML).
