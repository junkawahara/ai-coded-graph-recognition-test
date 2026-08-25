# Graph Recognition Library

Header-only C++11 library of recognition algorithms for graph classes.

## Build

```
make              # build all CLI binaries into bin/
make <type>       # build a single binary (bin/<type>; e.g. make interval)
make gtest_all    # build only the gtest binary
make clean        # remove artifacts (including bin/)
```

CLI targets are derived automatically from `src/*_main.cpp` (the `Makefile` has no hand-written target list).

Compiler: g++ with C++11 (default); override via `CXX` / `CXXFLAGS`. Only the gtest binary needs C++17 (the library itself stays C++11-compatible). Run `git submodule update --init --recursive` once to fetch `third_party/googletest`.

## Project layout

```
include/       header-only library (all algorithms)
  graph.h        graph representation (1-indexed, adjacency list + adjacency set)
  <type>.h       one recognizer per graph class; enumerators in <type>_enum.h
  (others)       core utilities, decomposition structures, and the shared
                 NO-certificate vocabulary/extractors (forbidden_subgraph.h / obstruction_extract.h)
src/           CLI entry points (<type>_main.cpp)
tests/
  <type>/        test cases per graph class (.in / .exp)
  gtest/         gtest sources: helpers/, recognizers/, enumerators/,
                 property/ (random differential tests; excluded by the default filter)
third_party/googletest/  Google Test (git submodule)
docs/          Sphinx + Doxygen documentation
notes/         internal design notes (see "Algorithm design notes" below)
```

## Tests

```
make test           # default filter (tens of seconds once built)
make test-quick     # default filter + property tests (excludes only ultra-slow enumeration cases)
make test-all       # everything (large-n fullerene/cubic_planar/circular_arc cases run for hours)
./gtest_all --gtest_filter='Interval*'   # partial run
```

The default filter is `TEST_DEFAULT_FILTER` in the Makefile. It excludes exactly four patterns (keep in sync with the Makefile comments):

| pattern | why excluded |
| --- | --- |
| `*FullereneEnum*` | enumeration takes hours from n>=20 (all cases excluded) |
| `*CubicPlanarEnum*case6` | n=10 (5,826,240 graphs) takes ~280 s |
| `*/CircularArcEnumTest.*case6` | n=6 takes ~250 s alone vs ~5 s for all the rest; the leading `/` keeps `ProperCircularArcEnumTest` included |
| `*Property*` | random differential tests (run via `make test-quick` / `make test-all`) |

The old Python/Bash test infrastructure (`tests/legacy/`) is deleted; recover it from the git tag `legacy-tests` if needed.

## Adding a new graph class

1. Create `include/<type>.h` providing `<Camel>Result { bool is_<type>; ... }` and `check_<type>(const Graph&)`
2. Optionally create `src/<type>_main.cpp` (gtest alone suffices; the file's mere presence makes `make` produce `bin/<type>`)
3. Put test cases (.in / .exp) under `tests/<type>/`
4. Create `tests/gtest/recognizers/<type>_test.cpp` by copying an existing one
5. Verify with `make test`

## Result struct conventions

When `<Camel>Result` carries structure (certificate / decomposition / model) beyond the recognition bool:

- Structure fields are valid only when `is_<type> == true`, and empty otherwise.
  Exception: vertex-indexed vectors (`side`, `parent`, ...) are returned zero-filled
  at size n+1 so indices `[1, n]` stay in bounds even on precondition violations
  (`enumerate_maximal_cliques` in `clique.h` is the existing model).
- If an algorithm variant does not fill a structure field, state that in the field's Doxygen comment.
- Structure that comes free as a by-product of recognition goes into the Result
  (split's (K,S) partition, threshold's creation sequence, ...). Structure that costs
  more than recognition goes into a separate `build_<structure>(g)` builder so the
  recognition cost is unchanged (`build_clique_tree`, `build_cotree`, `build_pruning_sequence`).

NO-side certificates (forbidden subgraph / hole / AT / ...):

- Use the shared type `Obstruction obstruction;` (`include/forbidden_subgraph.h`).
  Valid only when `is_<type> == false`; `kind == NONE` when true.
- `obstruction.vertices` is a vertex list, not a vertex-indexed vector, so the n+1
  zero-fill convention does NOT apply. Its meaning (ordering) is defined per kind
  in the enum's Doxygen comments.
- Patterns in the complement are expressed via `in_complement` rather than new kinds;
  this covers all co_* classes.
- Certificates obtainable within the recognition bound are filled in by the recognizer.
  More expensive ones go into `build_<type>_obstruction(g)` builders so the default
  recognition cost is unchanged (`build_split_obstruction`, `build_planar_obstruction`, ...).
- Variants that cannot produce a certificate also return `kind == NONE`, documented
  in the field's Doxygen comment (same rule as YES-side structure fields).

## I/O format

stdin: first line `n m` (vertex and edge counts), then one edge `u v` per line; vertices are 1-indexed.
stdout: `YES` / `NO` plus class-specific information.

## Algorithm design notes

Pitfalls for reimplementation are recorded in separate files. Read the relevant one before touching that code, and append new insights there:

- `notes/design_notes.md` — per-class recognition/enumeration design notes (reverse-search parent rules, resource budgets, constructions that turned out to be wrong)
- `notes/decomposition_notes.md` — pitfalls from exposing decomposition structures (SEO / modular decomposition / split / SPQR / DMP planar embedding)
- `notes/obstruction_notes.md` — pitfalls of NO-certificate extraction and why some classes were skipped
- `notes/enum_alg.md` / `notes/recog_alg.md` — surveys of enumeration / recognition algorithms per graph class (references, complexity, implementation status)

## Coding conventions

- C++11-compatible (no `std::iota`; use `size_t` for range-for indices)
- Header-only (`inline` functions)
- Namespace `graph_recognition`; vertices are 1-indexed

## Git workflow

- Commit frequently: small, incremental commits after each feature or fix; do not batch unrelated changes
- Auto-commit after implementation: when a task is complete and tests pass, commit without waiting for user instruction
