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
                 (labeled) and <type>_unlabeled_enum.h (non-isomorphic)
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

## Unlabeled (non-isomorphic) enumerators

An enumerator that emits one representative per isomorphism class lives in its own
header beside the labeled one, never replacing it — keeping both is what makes the
differential test below possible. `include/proper_interval_unlabeled_enum.h` is the model.

- `include/<type>_unlabeled_enum.h`, with its own `<Camel>UnlabeledEnumAlgorithm`,
  `<Camel>UnlabeledEnumeratedGraph`, `<Camel>UnlabeledEnumerationResult`, and
  `enumerate_<type>_unlabeled_graphs(int n, bool connected_only, algo)`.
  Helpers in `namespace detail` need the full `<type>_unlabeled_` prefix (one shared namespace).
- CLI `src/<type>_unlabeled_enum_main.cpp` → `bin/<type>_unlabeled_enum`, taking `--connected`.
- Python: add the key `"<type>_unlabeled"` to `_ENUM_TYPES` and `_NON_ISOMORPHIC_ENUM_TYPES`,
  plus `_CONNECTED_ONLY_ENUM_TYPES` if it takes `connected_only` (the generated wrapper
  passes only `n` otherwise).
- Tests must include a cross-check that canonicalizing the labeled enumerator's output
  yields exactly the unlabeled enumerator's set (`canonical_edge_list` is brute-force
  over n! permutations, so cap it around n <= 6), on top of the usual count/recognizer checks.
- Composing disconnected graphs from connected components: copy the integer-partition DFS
  in `include/forest_enum.h` — the non-decreasing index constraint on equal-size parts is
  what keeps the multiset duplicate-free.

## Subgraph enumerators

An enumerator that takes a **host graph** and emits the subgraphs of it belonging to
the class, rather than taking `n` and emitting all n-vertex members of the class.
`include/chordal_subgraph_enum.h` is the model.

- `include/<type>_subgraph_enum.h`, with its own `<Camel>SubgraphEnumAlgorithm`,
  `<Camel>SubgraphEnumerationResult`, `enumerate_<type>_subgraphs(const Graph&, algo)`
  and a streaming `enumerate_<type>_subgraphs_cb(const Graph&, cb, algo)`.
  Reuse `EnumeratedGraph` from `<type>_enum.h` rather than declaring a new output type.
  Helpers in `namespace detail` need the full `<type>_subgraph_` prefix (one shared namespace).
- The output is **spanning** subgraphs: the vertex set stays `{1, ..., n}` and isolated
  vertices are kept, so outputs are in bijection with the class's edge subsets of `E(G)`
  and the empty edge set is always emitted. (Induced-subgraph enumeration is a different
  problem with a different output type — see "Induced subgraph enumerators" below.)
- CLI `src/<type>_subgraph_enum_main.cpp` → `bin/<type>_subgraph_enum`, reading the
  standard `n m` + edge-list input. Guard on **m, not n**: the output can be `2^m`.
- Python: add the key `"<type>"` to `_SUBGRAPH_ENUM_TYPES` (and `_SUBGRAPH_ENUM_ALGORITHMS`
  for the docstring); the factory generates `enumerate_<type>_subgraphs(n_or_graph, edges=None)`.
  This registry is separate from `_ENUM_TYPES` because the wrapper takes a host graph.
  The pybind11 entry point is `_enumerate_<type>_subgraphs(n, edges)`.
- Tests must include a brute-force cross-check over all `2^m` edge subsets filtered by
  `check_<type>` (cap around m <= 15), plus a check that a **complete host** reproduces
  `enumerate_<type>_graphs_*(n)` exactly — the two enumerations coincide there by definition.
- If the class's fixed-n enumerator is a reverse search whose parent rule only *deletes*
  edges, the subgraphs of a host are closed under it, so the subgraph enumerator is the
  same search with child generation filtered by host adjacency; the canonicality test must
  stay host-independent. See `notes/design_notes.md` before adapting one.

## Induced subgraph enumerators

An enumerator that takes a **host graph** and emits the vertex subsets `X` for which
`G[X]` belongs to the class. Distinct from the spanning-subgraph enumerators above:
the solutions are vertex sets, not edge sets. `include/chordal_bipartite_induced_subgraph_enum.h`
is the model.

- `include/<type>_induced_subgraph_enum.h`, with its own
  `<Camel>InducedSubgraphEnumAlgorithm`, `<Camel>InducedSubgraphEnumerationResult`,
  `enumerate_<type>_induced_subgraphs(const Graph&, algo)` and a streaming
  `enumerate_<type>_induced_subgraphs_cb(const Graph&, cb, algo)`.
  Helpers in `namespace detail` need the full `<type>_induced_subgraph_` prefix.
- The result field is `std::vector<std::vector<int>> vertex_sets`, each entry sorted
  ascending in the host's labels. Do **not** reuse `EnumeratedGraph`: an induced
  subgraph is determined by its vertex set, and carrying an edge list would
  misrepresent the output. The empty set is always emitted first.
- The streaming callback takes `const std::vector<int>&` and receives the search's own
  buffer, so document that it must be copied to be kept.
- CLI `src/<type>_induced_subgraph_enum_main.cpp` → `bin/<type>_induced_subgraph_enum`.
  Guard on **n, not m**: hereditary classes make every one of the `2^n` vertex subsets
  a solution when the host is itself in the class, however sparse it is.
- Python: add the key `"<type>"` to `_INDUCED_SUBGRAPH_ENUM_TYPES` (and
  `_INDUCED_SUBGRAPH_ENUM_ALGORITHMS` for the docstring); the factory generates
  `enumerate_<type>_induced_subgraphs(n_or_graph, edges=None)` guarded by
  `INDUCED_ENUM_MAX_N`. The pybind11 entry point is
  `_enumerate_<type>_induced_subgraphs(n, edges)` returning `list[list[int]]`.
  `python/tests/test_recognize.py::test_core_bindings_match_registry` enumerates every
  `_enumerate_*` binding, so it has to learn the new registry too.
- Tests must include a brute-force cross-check over all `2^n` vertex subsets filtered by
  `check_<type>` (cap around n <= 12), plus a check that a **host in the class** yields
  exactly `2^n` sets — hereditary classes coincide there by definition.

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
