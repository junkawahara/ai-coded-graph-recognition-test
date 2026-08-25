# NO-certificate (obstruction) extraction

38 classes return a certificate on the NO side (conventions: the "Result struct
conventions" section of CLAUDE.md). The shared vocabulary is
`include/forbidden_subgraph.h`, the shared extractors are
`include/obstruction_extract.h`, and the definition-level verifiers are
`verify_obstruction` in `tests/gtest/helpers/certificates.cpp` and `verify` in
`python/tests/test_obstructions.py`. Only the pitfalls for reimplementation are
recorded here (moved out of CLAUDE.md).

- **Holes need no shortcut handling**: a shortest path in an induced subgraph is
  itself an induced path. Restricting the BFS to the complement of
  `N[u] ∪ N[v]` means interior vertices have no chord to u or v, and being a
  shortest path they have none among themselves either; `hole_from_bfs_path`
  closes this shape into a hole. Conversely, **ODD_HOLE must not be
  shortcut** (it can collapse below length 5 or into a triangle). The only kind
  that does not require chordlessness is ODD_CYCLE (bipartite).
- **A PEO failure point always lies on a hole**: local extraction from the
  failing triple (v, parent[v], u) of `verify_peo` succeeded on all 4,438,431
  NO instances over all graphs with n<=7 and all variants. The generic
  `find_hole` fallback is insurance, not the expected path.
- **Composite classes map to the composite class's kinds**: a raw C6 is not an
  obstruction for split, nor for trivially perfect. Four consecutive vertices
  of a long hole form an induced P4, and on the complement side the same four
  vertices form a C4 of G (`split_obstruction_from_hole` /
  `tp_obstruction_from_hole`). This is why split certificates always have
  `in_complement == false`.
- **FORCING_CYCLE must not be taken from the solver's propagation trail**: the
  trail mixes in transitivity and already-fixed classes, which the verifier
  cannot replay. BFS over the pure Γ relation instead (`find_forcing_cycle`).
  Γ is symmetric, so one search covers a whole implication class, and a failed
  search removes every reached arc from the candidate set.
- **The parity certificate is two paths**: a single induced path of wrong parity
  cannot tell the verifier which side is wrong. Attach the shortest u-v path
  (induced, with the parity of d(u,v)) as well.
- **Minor-model extraction cannot use memoized trues**: `serialize` keys
  identify isomorphic states, so a cached true belongs to a different
  contraction history (groups) and cannot supply a model. Cached falses are a
  purely structural fact and may be reused (`MinorChecker::find_model` keeps
  its own dead_).
- **Variants that decide from the degree sequence alone cannot carry a
  certificate**: threshold's FAST, split's HAMMER_SIMEONE, and LR planarity
  have no vertices to point at. Handle with `kind == NONE` + a Doxygen note +
  a builder.
- **biconnected with n < 3 has no certificate**: K2 is connected and has no cut
  vertex; there is nothing to point at beyond "too small".
- **Classes skipped, and why**: circular_arc / proper_circular_arc (no usable
  finite forbidden-subgraph characterization; Tucker's families are infinite),
  trapezoid (the 2+2 witness lives in the B(P) of the chosen orientation, so
  independent verification would need a certified poset), strongly_chordal
  (suns have unbounded size), proper_chordal (recognition itself is factorial),
  line_graph (no Beineke detector implemented, and the failure point does not
  identify which of the 9 graphs occurs), circle (Naji's NO is a rank fact
  over GF(2), not a combinatorial witness), five_leaf_power (budget exhaustion
  means "unknown", not NO).
