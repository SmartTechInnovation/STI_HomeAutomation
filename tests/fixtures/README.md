# Test fixtures

Files used by the tests: sample projects, descriptors, packages, keys.

Tests find this folder through the compile definition `STIX_FIXTURES_DIR`:

```cpp
const QString path = QStringLiteral(STIX_FIXTURES_DIR) + "/projects/1.0.0/Project.xml";
```

Planned layout:

| Folder | Content | Story |
|--------|---------|-------|
| `projects/<ConfigVers>/` | one golden project per released format version; each must always open | 2.3 |
| `migration/` | projects in older/newer versions for the migration tests | 2.2 |
| `descriptors/` | block descriptors for BlockManager tests | 1.3 |
| `validator/`, `merge/` | crafted projects for the validator and the three-way merge | 3.6, 9.2 |
| `packages/`, `keys/`, `certs/` | library packages, test signing keys, test TLS certificates (test-only, never real keys) | 4.x, 6.4 |

Fixtures are inputs: a test must never write into this folder. Write to a `QTemporaryDir` instead.
