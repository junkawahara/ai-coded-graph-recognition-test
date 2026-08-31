# Algorithm design notes

Pitfalls to know before reimplementing or modifying the recognition/enumeration
algorithms of individual graph classes (moved out of CLAUDE.md). For the
decomposition-structure work see `decomposition_notes.md`; for NO-certificate
extraction see `obstruction_notes.md`.

## Chordal enumeration (chordal_labeled_enum.h)
- **Default is the dedicated Kiyomi--Uno reverse search**: the root is the one-edge graph, and the parent is defined by removing the minimum-degree simplicial vertex (ties broken by smallest label). Children attach an unused vertex `v` to a clique `C`, generating only the `|C| < k`, `|C| = k`, `|C| = k+1` cases of Lemmas 1/2 in the paper.
- **Isolated vertices are implicit**: `KiyomiUnoChordalState::alive` holds only the vertices currently incident to at least one edge. A disconnected K2 component adds two unused vertices at once, with `v < w` deduplicating `(v,{w})` / `(w,{v})`. The empty graph is emitted exactly once, outside the search tree.
- **PEO and simplicial status are updated incrementally**: the child's new vertex goes to the front of the PEO. An existing simplicial vertex `u in C` stays simplicial iff `N_G(u) ⊆ C`, so this is updated differentially instead of re-running recognition.
- **The old search is kept for dependent code**: `ChordalLabeledEnumState` / `collect_children_reverse_search` are used by the various chordal-subclass enumerators and by the legacy differential checks of interval / strongly chordal. The public chordal API selects it via `LEGACY_VERTEX_REVERSE_SEARCH`.
- The paper's O(1) amortized / O(1) delay bounds apply to an optimized differential-output implementation. The current implementation keeps a simple O(n^2) state and builds the full edge list on every callback, so those bounds do not apply.

## Chordal subgraph enumeration (chordal_subgraph_enum.h)
- **This is the problem Kiyomi--Uno actually solves**; `chordal_labeled_enum.h` is its `G = K_n` special case. The subgraph enumerator therefore reuses `KiyomiUnoChordalState` and every `kiyomi_uno_*` helper unchanged and only adds a host filter, rather than reimplementing the search.
- **Filtering child generation by host adjacency is sufficient, and no canonicality rule may change.** The parent rule deletes the edges at the minimum-degree simplicial vertex, i.e. it only *deletes* edges, so `{chordal H : H ⊆ G}` is closed under it. Restricting the full search tree to that family therefore leaves a tree whose roots are the one-edge subgraphs = the edges of `G`. Concretely: a clique child needs `v` host-adjacent to all of `C`, a new-component child needs `(v,w) ∈ E(G)`, and the roots iterate over `E(G)`. `kiyomi_uno_is_child_clique()` reads only the current state, never the host, so it is reused verbatim — adding a host condition *inside* it would break the parent/child correspondence and drop subgraphs.
- **Do not try to prune by "is `C` a clique of the host"**: `C` is enumerated from the current subgraph's PEO, and `C` is always a clique of the subgraph, hence of the host. The only host condition that matters is on the *new* vertex's edges.
- **The output is spanning subgraphs, not induced ones**: the vertex set stays `{1, ..., n}` and isolated vertices are kept, so outputs are in bijection with the chordal edge subsets of `E(G)` and the empty edge set is always emitted (once, outside the search tree). An induced-subgraph enumerator would be a different problem with a different (much smaller) output.
- **The output size is `2^m` in the worst case** (every subgraph of a forest is chordal), so the count is driven by `m`, not by `n`: `K_5` gives 822 but the Petersen graph (`n=10, m=15`) gives 22292. The CLI guards on `m`, not `n`, for that reason.

## Chordal bipartite induced subgraph enumeration (chordal_bipartite_induced_subgraph_enum.h)
- **This is induced-subgraph enumeration, a different problem from the spanning-subgraph enumerators.** Solutions are the vertex subsets `X` with `G[X]` chordal bipartite, so the output type is `std::vector<std::vector<int>> vertex_sets`, not `EnumeratedGraph`. Reusing the edge-list output type would misrepresent the result, which is why the Python side needs its own `_INDUCED_SUBGRAPH_ENUM_TYPES` registry.
- **The max-check only ranges over `u ∈ X` with `u > v`.** `WS(X ∪ {v}) ⊆ X ∪ {v}`, so vertices outside the candidate solution are not candidates at all, and vertices below `v` cannot beat it. Since `members` is kept sorted, the check walks it backwards and breaks at the first element `< v`.
- **Set `in_X[v] = 1` before *both* weak-simpliciality tests.** `u`'s weak-simplicial status is relative to `G[X ∪ {v}]`, not `G[X]`: adding `v` can make an existing neighbor pair of `u` incomparable. Testing `u` against `G[X]` silently accepts non-children and produces duplicates.
- **Do not add a `check_chordal_bipartite` call to the child test.** If `X` is a solution and `v` is weak-simplicial in `G[X ∪ {v}]`, then `G[X ∪ {v}]` is chordal bipartite automatically: independence of `N(v)` rules out triangles through `v`, comparability of the neighbors chords every longer cycle through `v`, and a bad cycle avoiding `v` would already contradict `G[X]` being a solution. A recognizer call is redundant, asymptotically dominant, and masks exactly the bugs the differential tests exist to catch.
- **Comparability is two-sided**: `N(a) ⊆ N(b)` failing does not imply `N(b) ⊆ N(a)` — test both directions. The independence check must come first: it is what guarantees `b ∉ N(a)`, so the subset scan never has to ask whether `b ∈ N(b)`.
- **Isolated vertices are vacuously weak-simplicial**, so every singleton is a child of `∅` and disconnected solutions come for free. Do not "optimize" the candidate loop to `N(X)`; that drops every solution with a new component.
- **The output size is `2^n` in the worst case** and reaches it exactly when the host is itself chordal bipartite (the class is hereditary), regardless of how sparse the host is. The CLI and the Python guard are therefore on `n`, not `m` — the opposite of `chordal_subgraph_enum.h`.
- **The vertex order is a free choice.** "Largest weak-simplicial vertex" only shapes the family tree; any fixed total order enumerates the same family. The paper's degeneracy ordering is there for the complexity bound, not for correctness, so do not diagnose an ordering bug from output-order differences alone.
- **Simple vs. paper**: the candidate set `C(X)`, `UpdateWS` / `UpdateAWS` and the degeneracy ordering are delay optimizations only. Recomputing `WS` from scratch per candidate is correct and is what this header does; state that whenever the O(k t Δ²) bound is quoted.

## Split enumeration (split_labeled_enum.h)
- **Default is a dedicated canonical KS-partition enumeration**: it directly generates S-max partitions `V = K ∪ S`, where `K` is a clique, `S` is an independent set, and every `k in K` is required to have a nonempty neighborhood inside `S`; all candidates are then split graphs and no recognition filter is needed.
- **Swing vertices deduplicate**: multiple S-max partitions exist only when the swing-vertex set `A` is a clique. On the `S` side, `a` is complete to `K` and every vertex of `A-{a}` has `S`-neighborhood exactly `{a}`. Only the partition with `a = min(A)` is accepted.
- **Old search and streaming**: the chordal vertex-addition tree + split recognition is kept as `LEGACY_CHORDAL_FILTER` for differential checks. The callback API keeps only O(n^2) search state and builds the full edge list per output.

## Interval enumeration (interval_labeled_enum.h)
- **Default is the dedicated Kiyomi--Kijima--Uno reverse search**: the root is K_n and edges are deleted one at a time. The parent is defined by adding the edge between the largest-label non-universal vertex and its closest non-neighbor in the interval model.
- **Child candidates are limited by a pivot**: deleting an edge between two vertices below the pivot cannot lead back to the current node as parent. Vertices above the pivot are universal true twins, so interval recognition runs once per fixed partner and the result is reused for the corresponding children of every label.
- **The old chordal-filter search remains**: selectable via `LEGACY_CHORDAL_FILTER`, used for set-difference tests against the dedicated search.
- **The original paper's O(n^3)/output time bound does not carry over** (the O(n^2) space bound does): the implementation calls the existing `check_interval` per child candidate and builds the full edge list per callback. The streaming API itself keeps O(n^2) state.

## Proper interval unlabeled enumeration (proper_interval_unlabeled_enum.h)
- **This is not a reverse search**: it generates the Saitoh et al. bracket string of each isomorphism class directly. A connected proper interval graph on n vertices is a string of n `[` and n `]`; scanning left to right, `[` opens a new vertex adjacent to every vertex still open, `]` closes one.
- **`]` closes the vertex that has been open the longest (FIFO)**. This is the main reimplementation trap: closing the most recent one (LIFO, the natural stack reflex) builds nested intervals, which is a valid interval model but never a proper one, so the enumeration silently drifts to a different set of graphs. The recognizer cross-check in the gtest catches it.
- **The strict-prefix condition is exactly connectivity**: `s = '[' + Dyck(n-1) + ']'` forces every proper non-empty prefix to hold strictly more `[` than `]`. A balanced proper prefix would mean no interval spans that boundary, i.e. the graph splits into components there. So enumerating Dyck words of semilength n-1 covers precisely the connected graphs.
- **Canonicity is `s <= reverse_flip(s)`, and the `<=` matters**: the representation is unique up to reversal (Deng--Hell--Huang), so each class has an orbit of size one or two under reverse-flip. Using `<` drops the self-mirror strings, which are the sole representative of their class (n=3 would yield 1 connected graph instead of 2). ASCII puts `[` (0x5B) before `]` (0x5D), so a plain lexicographic comparison is the right one.
- **Disconnected graphs are composed over integer partitions**, copying `forest_combine` / `forest_partition_dfs` from forest_unlabeled_enum.h: non-increasing parts, and for equal-size parts the chosen component index must be non-decreasing. That constraint is what makes the multiset of components duplicate-free; without it the same graph appears once per ordering of its equal-size components.
- **`connected_only` selects between the two OEIS series**: A005217 (1, 2, 4, 9, 21, 55, 151, 447, ...) by default, A007123 (1, 1, 2, 4, 10, 26, 76, 232, ...) when set. n = 0 emits the empty graph in both modes.
- **The paper's O(1)-amortized delay does not carry over**: the Dyck recursion visits all Catalan(n-1) strings and discards roughly half at the canonicity filter, and every output is materialized as a full edge list. Verified against both series through n = 8, and the class set matches the labeled enumerator's canonicalized output through n = 6.


## Trivially perfect unlabeled enumeration (trivially_perfect_unlabeled_enum.h)
- **No isomorph rejection is needed**: the ancestor closure of a rooted tree (join every
  vertex to all of its ancestors) is a bijection between rooted-tree isomorphism classes
  and connected trivially perfect graph classes. The inverse is the universal vertex
  decomposition: in the closure, the universal clique is exactly the chain from the root
  down to the first vertex with zero or >= 2 children, and peeling it and recursing on the
  components of the remainder rebuilds the tree. So the canonical rooted trees of
  `tree_unlabeled_enum.h` (`detail::compute_rooted_trees`) map one-to-one onto the output;
  a wrong closure would be caught by the labeled cross-check in the gtest.
- **The closure needs the root path, not just the parent**: for level-sequence entry
  L[i] = d, vertex i+1 is joined to `path[0..d-1]` where `path[a]` is the most recent
  vertex at depth a. Joining only to the parent gives back the tree itself. Preorder
  numbering makes every ancestor smaller than its descendants, so edges come out with
  first < second without swapping.
- **Disconnected graphs use the same integer-partition composition** as
  proper_interval_unlabeled_enum.h (non-increasing parts, non-decreasing component index on
  equal-size parts). The rooted-tree cache is shared across component sizes, so the whole
  enumeration computes each rooted-tree list once.
- **Counts are pure A000081**: all graphs on n vertices = rooted forests on n nodes =
  A000081(n+1) (1, 2, 4, 9, 20, 48, 115, 286, 719, ...); `connected_only` = rooted trees =
  A000081(n). Verified through n = 9 on both series and against the canonicalized labeled
  enumerator through n = 6.

## Cograph unlabeled enumeration (cograph_unlabeled_enum.h)
- **No isomorph rejection is needed**: for n >= 2, complementation swaps the cotree's root
  type (union <-> join), so it is a bijection between the connected and the disconnected
  cographs on n vertices. Disconnected graphs are determined by their multiset of connected
  components, the integer-partition composition (non-increasing parts, non-decreasing
  component index on equal-size parts) emits each multiset once, and complementation is
  injective on isomorphism classes — so building conn(k) bottom-up as
  conn(k) = complement(disc(k)), disc(k) = compositions of conn(< k), is duplicate-free
  by induction. This is the recursion behind the linear-delay generator of Jones, Protti,
  Del-Vecchio (TCS 713, 2018); this implementation materializes the output instead of
  meeting their delay bound.
- **Capping the part size at k - 1 is what selects the disconnected graphs**: a partition
  of k with every part < k has at least two parts. The full enumeration is then
  conn(n) followed by disc(n) — do not run a second partition DFS with max part n, which
  would recompute disc(n) and emit conn(n) via the trivial partition [n].
- **The complement is generated in lexicographic u < v order**, so its edge list needs no
  re-sort; the composed disconnected graphs do get sorted in the combine step.
- **Counts**: A000084 (1, 2, 4, 10, 24, 66, 180, 522, 1532, 4624, ...); `connected_only`
  = A000669 (1, 1, 2, 5, 12, 33, 90, 261, 766, 2312, ...). Verified through n = 10 on
  both series and against the canonicalized labeled enumerator through n = 6.

## Cluster unlabeled enumeration (cluster_unlabeled_enum.h)
- **Integer partitions, not set partitions**: the labeled enumerator walks the B(n) set
  partitions of {1..n}; the non-isomorphic one must walk the p(n) integer partitions of n.
  The cliques of a cluster graph are exactly its connected components, so the multiset of
  clique sizes is a complete isomorphism invariant and the map partition -> graph is a
  bijection on isomorphism classes. Do not reach for the labeled DFS plus canonicalization
  here — the whole point is that no isomorph rejection is needed.
- **Non-increasing parts are what makes the walk duplicate-free** (each partition is
  generated once), the same constraint the forest/trivially-perfect composition uses; the
  extra non-decreasing component-index rule those enumerators need does not arise, since a
  part of size k admits only one connected cluster graph (K_k).
- **`connected_only` is the single graph K_n**, not an empty output: a cluster graph is
  connected iff it is one clique. Guarding it by the general partition walk would emit the
  trivial partition [n] plus nothing else, so it is short-circuited instead.
- **The edge list comes out sorted for free**: consecutive vertex blocks in part order, and
  lexicographic pairs inside each block, so no final sort is needed (the test asserts it).
- **Counts**: A000041 (1, 2, 3, 5, 7, 11, 15, 22, 30, 42, ...). Verified through n = 20
  against the partition numbers and against the canonicalized labeled enumerator through
  n = 6.

## Canonical augmentation machinery (util/canonical_augmentation.h)
- **Shared by every geng-style unlabeled enumerator** (triangle-free, bipartite, ...):
  `canonicalize_bitmask_graph` returns the lexicographically smallest adjacency-row
  vector *and* the automorphism orbit of the canonically last vertex, plus
  `bitmask_graph_connected` / `bitmask_graph_edges`. Graphs are `vector<unsigned long
  long>` adjacency bitmasks over vertices 0..k-1, so k <= 63 and the enumerators guard
  n < 64.
- **One canonicalization per candidate child does double duty**: the branch-and-bound
  search for the smallest row vector also collects, over all orderings that tie with the
  minimum, the set of vertices placed last. Orderings achieving the canonical form differ
  by an automorphism, so that set is exactly the automorphism orbit of the canonically
  last vertex — the canonical-deletion orbit — and no separate Aut(G) computation is
  needed.
- **The leaf of the branch-and-bound must re-compare in full**: `best` can shrink after a
  branch was flagged strictly smaller, so the strictly-less flag is only a license to skip
  pruning, never a proof of a new minimum. Trusting the flag at the leaf overwrites the
  minimum with a larger vector; the labeled cross-check catches it (this bug existed
  during development of the triangle-free enumerator).
- **Cost**: exact branch-and-bound over all vertex orderings, worst case k! on
  vertex-transitive graphs (empty graph, complete graph, cycles). This is what bounds the
  practical range of everything built on it to around n = 10, not the number of graphs
  emitted.
- **Adding a class on top of it** needs one thing: the class must be hereditary, so
  deleting the canonically last vertex stays inside it. A cheap test for which
  neighborhoods of a new vertex keep membership (triangle-free, bipartite) is an
  optimization on top, not a requirement — permutation just calls the recognizer per
  candidate child.

## Triangle-free unlabeled enumeration (triangle_free_unlabeled_enum.h)
- **This is a McKay canonical construction path, not a reverse search over labeled
  graphs**: graphs grow one vertex at a time, and the candidate neighborhood S of the new
  vertex must be an independent set — that independence test *is* the triangle-forbidding
  pruning (adding a vertex creates a triangle iff S contains an edge), so the search never
  calls the recognizer.
- **Both rejection layers are required**: the orbit test on the added vertex makes each
  class accept exactly one parent class, and the per-parent dedup of accepted children by
  canonical form makes that parent produce the class once (different independent sets of
  the same parent can yield isomorphic accepted children). A child rejected by the orbit
  test is always produced elsewhere — possibly from the same parent via another S.
- **`connected_only` filters at emission**: connectivity is not monotone under the vertex
  growth (later vertices may join components), so unlike hereditary pruning it cannot cut
  the search; the full triangle-free search runs regardless.
- **Counts**: A006785 (1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172, ...); `connected_only`
  = A024607 (1, 1, 1, 3, 6, 19, 59, 267, ...). Verified through n = 10 / n = 8
  respectively (n = 9 about 2 s, n = 10 about 36 s — the exact canonicalization is k! on
  vertex-transitive graphs) and against the canonicalized labeled enumerator through
  n = 6.

## Bipartite unlabeled enumeration (bipartite_unlabeled_enum.h)
- **Same canonical construction path as triangle-free**, on the shared
  `util/canonical_augmentation.h`; only the membership test differs.
- **The membership test is a 2-coloring, not a recognizer call**: a *connected* bipartite
  graph has exactly one 2-coloring up to swapping its two sides, so adding a vertex x with
  neighborhood S keeps the graph bipartite iff, for every connected component C, S ∩ C
  lies inside one side of C's bipartition. Each component's colouring is flipped
  independently (components are joined only through x), and every cycle through x then
  closes at even length. Implemented as per-component side bitmasks computed once per DFS
  node, so the per-candidate test is one AND per component.
- **The per-component quantifier is the easy thing to get wrong**: "S is independent" (the
  triangle-free test) is *not* the bipartite test, and neither is "S lies in one side of a
  global 2-coloring" — a disconnected parent has no global 2-coloring to speak of, and
  requiring one would drop, e.g., every graph built by joining a new vertex to both sides
  of two different components.
- **`connected_only` filters at emission**, exactly as for triangle-free: connectivity is
  not monotone under vertex growth.
- **Counts**: A033995 (1, 2, 3, 7, 13, 35, 88, 303, 1119, 5479, ... for n = 1, 2, ...);
  `connected_only` = A005142 (1, 1, 1, 3, 5, 17, 44, 182, 730, ...). Verified through
  n = 10 (about 30 s; n = 9 about 1.5 s) / n = 9 respectively and against the
  canonicalized labeled enumerator through n = 6.

## Permutation unlabeled enumeration (permutation_unlabeled_enum.h)
- **Same canonical construction path as triangle-free / bipartite**, on the shared
  `util/canonical_augmentation.h`; what is new is that there is no cheap incremental
  membership test, so the pruning is a full `check_permutation` call per candidate child
  (both G and its complement must be comparability graphs — no way to decide that from
  the parent plus the new neighborhood). This is the pattern to copy for the remaining
  hereditary classes that have no dedicated test.
- **Recognize first, canonicalize second**: both costs are within a factor of two of each
  other (n = 9: 1568634 recognizer calls in 7.6 s, 692088 canonicalizations in 10.9 s), so
  running the recognizer first — it rejects a bit over half the candidates — is worth
  roughly a 40% saving on the run. Reversing the order canonicalizes graphs that are about
  to be thrown away.
- **`connected_only` filters at emission**, as in the other two: connectivity is not
  monotone under vertex growth.
- **Counts**: A123448 (1, 2, 4, 11, 33, 142, 776, 5699, 50723, ... for n = 1, 2, ...);
  `connected_only` = 1, 1, 2, 6, 20, 99, 600, 4753, 44068, ... (no OEIS entry found).
  Verified through n = 9 (n = 8 about 1 s, n = 9 about 19 s) both against the survey's
  A123448 values and, independently of the enumerator, against generating all unlabeled
  graphs on n vertices (A000088: 34, 156, 1044, 12346, 274668) and filtering them by
  `check_permutation`; plus the canonicalized labeled enumerator through n = 6. The static
  test cases stop at n = 7 to keep the n! brute-force canonical form in the test cheap.

## Circle unlabeled enumeration (circle_unlabeled_enum.h)
- **A verbatim copy of the permutation scheme** (see the section above): circle graphs are
  hereditary and have no cheap incremental membership test either, so the pruning is a
  `check_circle` call per candidate child, run before the canonicalization. Unlike
  permutation the recognizer (Naji's GF(2) system) is polynomial with a small constant,
  so the canonicalization dominates sooner.
- **Counts**: A156809 (1, 2, 4, 11, 34, 154, 978, 9497, 127954, ... for n = 1, 2, ...);
  every graph on at most 5 vertices is a circle graph, so the first five terms are
  A000088. `connected_only` = A156808 (1, 1, 2, 6, 21, 110, 789, 8336, ...).
  Verified through n = 9 (n = 8 about 2 s, n = 9 about 47 s) against A156809 and,
  independently of the enumerator, against generating all unlabeled graphs on n vertices
  (A000088: 34, 156, 1044, 12346) and filtering them by `check_circle` through n = 8;
  plus the canonicalized labeled enumerator through n = 6. The static test cases stop at
  n = 7 to keep the n! brute-force canonical form in the test cheap.

## Eulerian unlabeled enumeration (eulerian_unlabeled_enum.h)
- **Not hereditary, so the constraint moves to the last level instead of pruning**:
  deleting a vertex of positive degree from an Eulerian graph leaves its neighbors odd,
  so the intermediate levels of the canonical augmentation must generate all graphs on
  up to n - 1 vertices (no recognizer pruning is possible below the top, unlike the
  hereditary enumerators on the same machinery). The even-degree constraint instead
  makes the last level forced: in an Eulerian graph G the odd-degree vertices of G - v
  are exactly N_G(v), so every (n-1)-vertex graph extends to an Eulerian graph in
  exactly one way — join the new vertex to the odd-degree vertices, an even-sized set
  by the handshake lemma (this is the deletion bijection behind the labeled count
  2^((n-1)(n-2)/2)). Graphs reaching level n are Eulerian by construction; no
  recognizer call anywhere.
- **The canonical-parent test is unchanged on the forced level**: the canonical parent
  of an Eulerian graph is G minus a canonical-orbit vertex — an arbitrary (n-1)-vertex
  graph, generated exactly once at the previous level — and its unique extension
  reconstructs G, so McKay's one-parent-per-class argument goes through verbatim.

## Biconnected unlabeled enumeration (biconnected_unlabeled_enum.h)
- **Not hereditary either, but the constraint prunes the last two levels instead of
  forcing them**: deleting a vertex can disconnect a biconnected graph, so the
  intermediate levels generate all graphs (as for Eulerian). What biconnectivity gives
  is geng `-C` style necessary conditions: G - v is connected for *every* vertex v of a
  biconnected G — in particular for the canonical-orbit vertex — so disconnected
  level-(n-1) children are dropped before their canonicalization, and at the last level
  the new vertex needs degree >= 2 with every vertex of degree < 2 among its neighbors
  (the final minimum degree is 2; deleting v only lowers its own neighbors' degrees, so
  s = N_G(v) always satisfies both conditions and no class is lost).
- **The conditions are necessary, not sufficient**: a connected parent plus the degree
  conditions can still leave a cut vertex (e.g. joining the new vertex to two adjacent
  vertices of a path), so unlike Eulerian's forced level a full `check_biconnected` runs
  at emission — the only recognizer call in the search.
- **n < 3 emits nothing**, matching the recognizer's size requirement and the labeled
  enumerator (Eulerian's n = 0 special case does not carry over).
- **Counts**: A002218 (1, 3, 10, 56, 468, 7123, 194066 for n = 3, 4, ...). Verified
  through n = 9 (n = 8 about 0.4 s, n = 9 about 20 s — the last level is not forced, so
  the connected (n-1)-vertex graphs each branch over up to 2^(n-1) candidate
  neighborhoods, making n = 10 impractical unlike Eulerian) and against the
  canonicalized labeled enumerator through n = 6. The static test cases stop at n = 8,
  with the n! brute-force isomorphism check in the test guarded to n <= 7.
- **Counts**: A002854 (1, 1, 2, 3, 7, 16, 54, 243, 2038, 33120, ... for n = 1, 2, ...);
  `connected_only` = A003049 (1, 0, 1, 1, 4, 8, 37, 184, ...). Verified through n = 10
  (n = 9 about 1 s, n = 10 about 40 s — cheaper than the recognizer-pruned enumerators
  at equal n because the forced last level replaces the 2^(n-1)-way branch); plus the
  canonicalized labeled cycle-space enumerator through n = 6. The static test cases
  stop at n = 8: n! brute-force canonicalization in the test stays cheap and the
  default filter stays fast.


## Chordal unlabeled enumeration (chordal_unlabeled_enum.h)
- **A verbatim copy of the permutation/circle scheme**: chordal graphs are hereditary,
  so the pruning is a `check_chordal` call per candidate child, run before the
  canonicalization. The recognizer is linear (bucket MCS + PEO verification), so unlike
  permutation/circle the canonicalization dominates from the start.
- **The obvious shortcut is wrong**: adding a vertex whose neighborhood is a clique
  always preserves chordality, but restricting child generation to clique neighborhoods
  loses classes anyway. The parent in canonical deletion is fixed as the canonically
  last vertex of the child, and that vertex need not be simplicial (every chordal graph
  has *a* simplicial vertex, but the canonical labeling does not get to choose it — in
  P3 grown from 2K1 by joining a new vertex to both, the added center has the
  independent set as its neighborhood). So the per-child `check_chordal` call cannot be
  replaced by a neighborhood-clique test.
- **Counts**: A048193 (1, 2, 4, 10, 27, 94, 393, 2119, 14524, ... for n = 1, 2, ...);
  `connected_only` = A048192 (1, 1, 2, 5, 15, 58, 272, 1614, 11911, ...). Verified
  through n = 9 (n = 8 about 0.5 s, n = 9 about 6.5 s) against both sequences and,
  independently of the enumerator, against generating all unlabeled graphs on n
  vertices and filtering them by `check_chordal` through n = 8; plus the canonicalized
  labeled enumerator through n = 6. The static test cases stop at n = 8 (the gtest's
  n! brute-force canonical-form dedup is guarded to n <= 7).

## Interval unlabeled enumeration (interval_unlabeled_enum.h)
- **A verbatim copy of the chordal scheme**: interval graphs are hereditary, so the
  pruning is a `check_interval` call per candidate child (default AT_FREE variant:
  chordality + asteroidal-triple scan + clique-path construction), run before the
  canonicalization.
- **Not the survey's dedicated algorithm**: the Yamazaki et al. (WALCOM 2018 /
  TCS 806, 2020, O(n^4) delay) and Mikos (DMTCS 23(1), 2021, doi:10.46298/dmtcs.6164,
  O(n^3 log n) delay) enumerators walk MPQ-tree canonical forms and never
  canonicalize a graph; reimplementing one of them is the route if the exact
  canonicalization ever becomes the bottleneck before n = 10. Their delay bounds do
  not apply to this implementation.
- **Counts**: A005975 (1, 2, 4, 10, 27, 92, 369, 1807, 10344, ... for n = 1, 2, ...);
  `connected_only` = A005976 (1, 1, 2, 5, 15, 56, 250, 1328, ...). Verified through
  n = 9 (n = 8 about 0.5 s, n = 9 about 6.5 s) against both sequences and,
  independently of the enumerator, by filtering the chordal unlabeled enumeration
  (interval ⊂ chordal) through `check_interval` for n <= 9; plus the canonicalized
  labeled Kiyomi--Kijima--Uno enumerator through n = 6. The static test cases stop
  at n = 8 (the gtest's n! brute-force canonical-form dedup is guarded to n <= 7).

## Split unlabeled enumeration (split_unlabeled_enum.h)
- **A verbatim copy of the chordal scheme**: split graphs are hereditary, so the
  pruning is a `check_split` call per candidate child (Hammer-Simeone degree-sequence
  condition, near-linear), run before the canonicalization. As with chordal, the
  canonicalization dominates from the start.
- **Connected counts have no OEIS entry**: they are the first differences
  A048194(n) − A048194(n−1) (1, 1, 2, 5, 12, 35, 108, 393, 1666, ...). A disconnected
  split graph has at most one component with an edge (two such components would induce
  a 2K2), so its other components are isolated vertices, and split graphs with an
  isolated vertex biject with the split graphs on n−1 vertices. (The sequence
  coincides with a shift of A055192, bipartite graphs with a distinguished block and
  no isolated vertices, but that entry does not mention split graphs.)
- **Counts**: A048194 (1, 2, 4, 9, 21, 56, 164, 557, 2223, ... for n = 1, 2, ...).
  Verified through n = 9 against the sequence and, independently of the enumerator,
  against generating all unlabeled graphs on n vertices and filtering them by
  `check_split` through n = 8; plus the canonicalized labeled enumerator through
  n = 6. The static test cases stop at n = 8 (the gtest's n! brute-force
  canonical-form dedup is guarded to n <= 7).

## Planar unlabeled enumeration (planar_unlabeled_enum.h)
- **A verbatim copy of the chordal scheme**: planar graphs are hereditary, so the
  pruning is a `check_planar` call per candidate child (linear-time left-right
  planarity test), run before the canonicalization. As with chordal, the
  canonicalization dominates from the start.
- **Counts**: A005470 (1, 2, 4, 11, 33, 142, 822, 6966, 79853, ... for n = 1, 2, ...);
  connected counts are A003094 (1, 1, 2, 6, 20, 99, 646, 5974, ...). Verified through
  n = 9 against A005470 (~23 s) and, independently of the enumerator, against
  generating all unlabeled graphs on n vertices and filtering them by `check_planar`
  through n = 8; plus the canonicalized labeled enumerator through n = 6. The static
  test cases stop at n = 8 (the gtest's n! brute-force canonical-form dedup is guarded
  to n <= 7).

## Outerplanar unlabeled enumeration (outer_planar_unlabeled_enum.h)
- **A verbatim copy of the chordal scheme**: outerplanar graphs are hereditary, so the
  pruning is a `check_outer_planar` call per candidate child (linear-time left-right
  planarity test of G + K1), run before the canonicalization.
- **Not the survey's dedicated algorithm**: Wang--Nagamochi (AAIM 2010,
  doi:10.1007/978-3-642-14355-7_31) generate rooted connected outerplanar graphs in
  O(1) time per graph by growing the embedding directly and never canonicalize;
  reimplementing it (plus unrooting and the disconnected composition) is the route if
  the exact canonicalization ever becomes the bottleneck before n = 11. Its per-graph
  bound does not apply to this implementation.
- **Counts**: A111564 (1, 2, 4, 10, 25, 80, 277, 1150, 5291, ... for n = 1, 2, ...);
  `connected_only` = A111563 (1, 1, 2, 5, 13, 46, 172, 777, 3783, ...). Verified
  through n = 9 (n = 8 about 0.5 s, n = 9 about 5.7 s) against both sequences and,
  independently of the enumerator, by filtering the planar unlabeled enumeration
  (outerplanar ⊂ planar) through `check_outer_planar` for n <= 7; plus the
  canonicalized labeled reverse-search enumerator through n = 6. The static test cases
  stop at n = 8 (the gtest's n! brute-force canonical-form dedup is guarded to n <= 7).

## Series-parallel unlabeled enumeration (series_parallel_unlabeled_enum.h)
- **A verbatim copy of the chordal scheme**: series-parallel graphs are minor-closed,
  hence hereditary, so the pruning is a `check_series_parallel` call per candidate
  child (the queue-based series/parallel reduction), run before the canonicalization.
- **Not the survey's dedicated algorithm**: Kawano--Nakano (IEICE Trans. E88-A(5),
  2005) generate rooted connected series-parallel graphs in O(1) amortized time per
  graph by growing the decomposition tree directly and never canonicalize;
  reimplementing it (plus unrooting and the disconnected composition) is the route if
  the exact canonicalization ever becomes the bottleneck before n = 11. Its per-graph
  bound does not apply to this implementation.
- **Counts are NOT in the OEIS** (checked 2026-08-29, both search-by-terms and
  search-by-name): 1, 2, 4, 10, 27, 92, 360, 1715, 9356, ... for n = 1, 2, ...;
  `connected_only` gives 1, 1, 2, 5, 15, 56, 241, 1245, 7182, ... (Note A000084 /
  A006351 count two-terminal series-parallel *networks* by edges — a different
  object.) Verified through n = 8 independently of the recognizer by enumerating
  **all** unlabeled graphs (unpruned canonical augmentation) and filtering with the
  direct K4 `MinorChecker` of `util/minor.h`, plus the canonicalized labeled
  reverse-search enumerator through n = 6; n = 9 (about 6 s) gives the values above.
  The static test cases stop at n = 8 (the gtest's n! brute-force canonical-form
  dedup is guarded to n <= 7).

## Cactus unlabeled enumeration (cactus_unlabeled_enum.h)
- **A verbatim copy of the chordal scheme**: cactus graphs are hereditary (deleting a
  vertex only shrinks blocks — a cycle block falls apart into edges), so the pruning
  is a `check_cactus` call per candidate child (a DFS biconnected component
  decomposition), run before the canonicalization.
- **Not the survey's dedicated algorithm**: Bahrani--Lumbroso (ANALCO 2018,
  arXiv:1711.10647) derive the class's split-decomposition grammar and generate the
  non-isomorphic cacti directly from it (O(1)/graph for the rooted version);
  reimplementing it is the route if the exact canonicalization ever becomes the
  bottleneck before n = 11. Its per-graph bound does not apply to this implementation.
- **A000083 counts the CONNECTED cacti**, not all of them: its OEIS name is "mixed
  Husimi trees / polygonal cacti with bridges" and its terms (offset 0: 1, 1, 1, 2,
  4, 9, 23, 63, 188, 596, 1979, ...) match `connected_only` exactly. The total
  (possibly disconnected) sequence 1, 2, 4, 9, 20, 51, 133, 380, 1144, 3645, ...
  for n = 1, 2, ... is NOT in the OEIS (checked 2026-08-30 by search-by-terms).
  Verified through n = 8 independently of the enumerator by filtering the planar
  unlabeled enumeration (cactus is planar) through `check_cactus`, plus the
  canonicalized labeled reverse-search enumerator through n = 6; the connected
  counts match A000083 through n = 10 (about 33 s). The static test cases stop at
  n = 8 (the gtest's n! brute-force canonical-form dedup is guarded to n <= 7).

## Bipartite permutation unlabeled enumeration (bipartite_permutation_unlabeled_enum.h)
- **A verbatim copy of the chordal/cactus scheme**: the class is hereditary (vertex
  deletion preserves both bipartiteness and being a permutation graph), so the pruning
  is a `check_bipartite_permutation` call per candidate child (bipartiteness test +
  the generic permutation graph test), run before the canonicalization.
- **Not the survey's dedicated algorithms**: Saitoh--Otachi--Yamanaka--Uehara
  (J. Discrete Algorithms 10, 2012) enumerate the connected members through a
  canonical string encoding without isomorph rejection, and
  Kawahara--Saitoh--Takeda--Yoshinaka--Yoshioka (TCS 1003, 2024,
  DOI:10.1016/j.tcs.2024.114591) enumerate the non-isomorphic members BDD-based in
  polynomial time; reimplementing one of those is the route if the exact
  canonicalization ever becomes the bottleneck before n = 11. Their bounds do not
  apply to this implementation.
- **Neither count sequence is in the OEIS** (checked 2026-08-30 by search-by-terms):
  total 1, 2, 3, 7, 13, 34, 81, 239, 693, ... and connected
  1, 1, 1, 3, 5, 16, 38, 126, 375, ... for n = 1, 2, .... Verified through n = 8
  independently of the enumerator by filtering the bipartite unlabeled enumeration
  (bipartite permutation graphs are bipartite) through `check_bipartite_permutation`
  in both total and connected modes, plus the canonicalized labeled reverse-search
  enumerator through n = 6. The static test cases stop at n = 9 (n = 9 enumerates in
  about 2 s; the gtest's n! brute-force canonical-form dedup is guarded to n <= 7).

## Maximal planar unlabeled enumeration (maximal_planar_unlabeled_enum.h)
- **The planar scheme plus the labeled enumerator's edge bounds**: the class is not
  hereditary (a vertex-deleted triangulation is planar but no longer edge-maximal), so
  the search grows *planar* graphs — vertex deletion preserves planarity — and emits
  the n-vertex graphs with m = 3n-6. Children are pruned by `check_planar` and by the
  labeled enumerator's degree window (the child's edge count must be able to reach
  3n-6 with vertex j adding at most j-1 edges, and must not exceed 3n-6).
- **The window is safe for canonical augmentation because it is class-invariant**: it
  depends only on the child's edge count (and the level), never on which vertex was
  added, and the canonical-deletion chain of every triangulation stays inside it (the
  vertex deleted at level j+1 has degree at most j, exactly the slack the window
  grants). Pruning by a property of the *added vertex* that is not a class invariant
  would break the one-parent-per-class argument.
- **Not plantri**: Brinkmann--McKay (MATCH 58, 2007) generate embedded plane
  triangulations by canonical construction path over vertex splittings at millions of
  graphs per second; reimplementing it is the route if the exact canonicalization
  ever becomes the bottleneck. Counts match A000109 through n = 10 (233 graphs, about
  16 s; n = 9 takes about 0.6 s). The static test cases stop at n = 9 (the gtest's n!
  brute-force canonical-form dedup is guarded to n <= 8, cheap because only 14 graphs
  exist there). No `connected_only` flag: every maximal planar graph is connected, and
  the gtest asserts it.

## Polyhedral unlabeled enumeration (polyhedral_unlabeled_enum.h)
- **The maximal planar scheme with a weaker window and a final full check**: the class
  is not hereditary (vertex deletion can destroy 3-connectivity), so the search grows
  *planar* graphs exactly as in maximal_planar_unlabeled_enum.h; the edge-count window
  is ceil(3n/2) <= m <= 3n-6 (minimum degree 3 and planarity), and because
  3-connectivity cannot be tested along the way, the n-vertex graphs go through the
  full `check_polyhedral` before emission — the labeled enumerator's prune-then-verify
  scheme.
- **Window safety is the same argument as maximal planar**: both bounds depend only on
  the child's edge count, and the canonical-deletion chain of every polyhedral graph
  stays inside them (edges only ever get removed along the chain, and the vertex
  deleted at level j+1 has degree at most j, so the lower bound stays reachable).
  The labeled enumerator's other prunings (endgame connectivity, endgame degree >= 3)
  would also be class-invariant and chain-safe (3-connectivity survives deleting <= 2
  vertices, and a vertex loses at most r edges to r deleted vertices), but were not
  needed: the window alone gives n = 8 (257 graphs) in ~0.6 s and n = 9 (2606) in
  ~14 s, where the exact canonicalization dominates anyway.
- **Counts match A000944 through n = 9**; nothing is emitted below n = 4 (no
  3-connected graph exists), matching `enumerate_polyhedral_labeled_graphs`. The
  static test cases stop at n = 8 (the gtest's n! brute-force canonical-form dedup is
  guarded to n <= 7, 34 graphs). No `connected_only` flag: 3-connected implies
  connected.

## Cubic planar unlabeled enumeration (cubic_planar_unlabeled_enum.h)
- **The cubic scheme with a planarity prune, not the planar scheme with a degree
  filter**: both `cubic_unlabeled_enum.h` (degree-constrained neighborhoods, O(k^3)
  candidate children) and `planar_unlabeled_enum.h` (all subsets) reach the class, but
  starting from the cubic search keeps the branching factor polynomial. The added
  `check_planar` per candidate child is chain-safe because planarity is hereditary
  under vertex deletion, so the intermediates range over the hereditary class of
  planar graphs with maximum degree <= 3 — no edge-count window is worth adding
  (m <= 3n/2 is far below the planar bound 3n - 6 already).
- **The planarity call starts at 6-vertex children**: a graph with maximum degree <= 3
  on at most 5 vertices is always planar (K5 needs degree 4, K3,3 needs 6 vertices),
  so smaller children skip the Graph construction entirely.
- **Verification is differential, not hand-counted**: the connected counts reproduce
  A005964 (1, 1, 3, 9, 32, 133 through n = 14, ~94 s), but the totals allowing
  disconnected graphs (1, 1, 4, 10, 37, 146) are in no OEIS entry, so the gtest
  cross-checks the counts (total and connected, n <= 10) against
  `enumerate_cubic_unlabeled_graphs` filtered by `check_planar` on top of the usual
  labeled cross-check (n <= 6). A first draft of the header guessed 38 and 153 for
  n = 12 and 14; the differential run corrected both — do not trust hand-derived
  counts for composite classes.

## Tournament unlabeled enumeration (tournament_unlabeled_enum.h)
- **The shared undirected canonicalizer works verbatim on out-adjacency bitmasks,
  but only for tournaments**: `canonicalize_bitmask_graph` packs each vertex's row
  against the positions placed *before* it, and in a tournament an unset bit is
  exactly the reversed arc (the underlying graph is complete), so the form
  determines every pair's orientation and is a complete isomorphism invariant,
  with tied orderings differing by automorphisms. For general digraphs this fails
  — an unset bit would conflate non-adjacency with a reversed arc — so a digraph
  or poset unlabeled enumerator needs a genuinely directed canonical form (e.g.
  two bits per pair), not this reuse.
- **No recognizer prune and no connected_only flag**: every intermediate digraph
  of the vertex-by-vertex growth is a tournament by construction (the new
  vertex's in-neighborhood is the complement of the chosen out-neighborhood),
  and every tournament is weakly connected. This makes the enumerator the
  gentourng scheme itself rather than an approximation of it: gentourng is
  McKay's canonical construction path specialized to tournaments.
- **Tournaments have few automorphisms** (the automorphism group of a tournament
  has odd order, so vertex-transitive worst cases like the empty/complete graph
  cannot occur), keeping the exact branch-and-bound canonicalization fast:
  n = 9 (191536 classes) runs in ~9 s.
- **The directed cross-check helper is `canonical_arc_list`** in the gtest
  helpers: the sibling of `canonical_edge_list` without the u < v
  normalization. Same n! brute force, same n <= 8 intent.

## Digraph unlabeled enumeration (digraph_unlabeled_enum.h)
- **The directed canonical form lives in the shared utility, not the enumerator**:
  `canonicalize_bitmask_digraph` in `util/canonical_augmentation.h` is the same
  branch-and-bound DFS as the undirected form with one difference — each row packs
  **two** bits per earlier position (bit 2q = arc to position q, bit 2q+1 = the
  reverse arc), so non-adjacency, either single arc, and a bidirectional pair are
  all distinguished. That widens rows to 2(k-1) bits, so k <= 33 (irrelevant in
  practice: the enumeration dies of graph count long before). A future poset
  unlabeled enumerator should reuse this form rather than invent another.
- **Children are (out, in) subset pairs, 4^k per node**: the new vertex's
  out-neighborhood and in-neighborhood over the existing k vertices are
  independent subsets. No recognizer prune exists (every intermediate digraph is
  simple by construction) and no connected_only flag (the labeled digraph
  enumerator has none either; A003085 would be the weakly connected counts).
- **The 4^k fan-out, not the canonicalization, is the practical bound**: n = 6
  (1540944 classes) costs ~10^7 canonicalizations, ~7 s; n = 7 would cost
  1540944 * 4^6 ~ 6 * 10^9 of them for 882033440 outputs — out of reach, unlike
  tournaments (2^k fan-out, practical to n = 9).
- **Labeled cross-check caps at n = 4, one below the usual 6**: the labeled
  universe is 2^(n(n-1)) (4^10 ~ 10^6 already at n = 5), and each labeled digraph
  is canonicalized by the n!-brute-force `canonical_arc_list`. The in-result
  isomorphic-duplicate check runs to n = 5 (9608 * 5! is fine).

## Poset unlabeled enumeration (poset_unlabeled_enum.h)
- **Grow the transitive closure, not the Hasse diagram**: deleting a vertex of
  the closure digraph is exactly the closure of the induced subposet, so the
  canonical-parent argument goes through; deleting a vertex of a Hasse diagram
  is in general NOT the Hasse diagram of the subposet (removing a middle
  element turns a length-2 chain's endpoints into a new cover), which would
  break reachability. The Hasse diagram is extracted only at the leaves, to
  match `poset_labeled_enum.h` and `check_poset`.
- **The child rule needs no recognizer**: a new element with successor up-set
  s_out (closed under successors) and disjoint predecessor down-set s_in
  (closed under predecessors, every member below every s_out member) keeps the
  digraph a transitively closed DAG by construction. At most 3^k of the 4^k
  subset pairs survive disjointness, and the closure constraints prune far
  below that — n = 8 (16999 classes) ~1.3 s, n = 9 (183231) ~33 s, n = 10
  (2567284) on the order of ten minutes; much further than the digraph
  enumerator's 4^k unconstrained fan-out.
- **`canonicalize_bitmask_digraph` reused as planned**: the directed canonical
  form the digraph enumerator introduced applies verbatim to closure
  out-adjacency rows (poset isomorphism = digraph isomorphism of closures).
- **Labeled cross-check caps at n = 5, one below the usual 6**: A001035(6) =
  130023 labeled posets would each go through the n!-brute-force
  `canonical_arc_list`. Canonicalizing Hasse arc lists is sound for the
  comparison because Hasse-diagram isomorphism coincides with poset
  isomorphism. The in-result isomorphic-duplicate check runs to n = 6
  (318 * 6! is fine).

## Self-complementary unlabeled enumeration (self_complementary_unlabeled_enum.h)
- **Not the canonical-augmentation scheme**: the class is not hereditary (deleting a
  vertex destroys self-complementarity), and it is so sparse that growing all graphs
  vertex by vertex would be hopeless. The construction is the labeled enumerator's
  complementing-permutation one, restricted to one permutation per cycle type.
- **One permutation per cycle type is what makes it cheap**: permutations of the same
  cycle type are conjugate, and the graphs complemented by `tau sigma tau^-1` are the
  `tau`-images of those complemented by `sigma`, so a single representative already
  meets every isomorphism class the type can produce. The labeled enumerator's loops
  over every ordering of the cycle-length multiset and every assignment of vertices to
  cycles (an n!-sized space) collapse to nothing here — that loop is what makes the
  labeled enumerator unusable past n = 8, not the graph count.
- **Cycle lengths are powers of two, not just multiples of 4**: the pair-orbit parity
  argument alone gives "every cycle length is divisible by 4, plus at most one fixed
  point". The stronger statement holds because the antimorphism can be taken of
  2-power order (raise it to its odd part; odd powers of an antimorphism are
  antimorphisms), and then every cycle length divides that order. Enumerating only
  multiples of 4 that are not powers of two would add types that produce nothing new;
  omitting the powers-of-two restriction is safe but wasteful, dropping it the other
  way (allowing 12, say) is not — the completeness argument is the 2-power one.
- **Fixing the first orbit halves the work**: flipping the assignment of every orbit
  complements the graph, and sigma maps the complement back, so mask and ~mask are
  always isomorphic and only 2^(r-1) of the 2^r assignments need to be built.
- **Isomorph rejection must be global**: a graph can have antimorphisms of several
  cycle types (and one type can produce a class more than once), so the canonical-form
  set spans all cycle types rather than being reset per type.
- **Counts**: A000171 (1, 0, 0, 1, 2, 0, 0, 10, 36, 0, 0, 720, 5600, ... for
  n = 1, 2, ...), nonzero only for n = 0, 1 (mod 4). Verified through n = 9 against the
  sequence and, independently of the enumerator, by canonicalizing the labeled
  enumerator's whole output: its 98280 graphs at n = 8 and its 4123728 at n = 9
  collapse to exactly the 10 and 36 classes emitted here. The gtest cross-check
  against the canonicalized labeled enumerator stops at n = 6 (n! brute force); the
  static test cases stop at n = 9.
- **The canonicalization, not the candidate count, is the wall**: cost is 2^(r-1)
  canonicalizations per cycle type, where r is the number of pair orbits — r = 2, 3,
  8/4, 10/5 for n = 4, 5, 8, 9, so n = 9 (528 candidates) takes 0.1 s. At n = 12 the
  type (4,4,4) has r = 18, and one canonicalization of these highly symmetric
  12-vertex graphs costs about 0.17 s, so its 131072 candidates are some six hours
  (n = 13: r = 21, 1050624 candidates). A cheaper exact canonical form, or reducing
  the candidates by the centralizer of sigma before canonicalizing, is what would
  move that. r first reaches the 63-bit mask limit at n = 24, far past the practical
  range.

## Distance-hereditary unlabeled enumeration (distance_hereditary_unlabeled_enum.h)
- **The vertex-incremental characterization replaces the recognizer, not just prunes it**:
  Bandelt & Mulder's three one-vertex extensions (pendant, true twin, false twin) generate
  the whole class, so a graph has 3(k-1) children instead of the 2^(k-1) neighborhoods the
  hereditary reverse search tests, and `check_distance_hereditary` is never called inside
  the search. That is the whole speed argument; the isomorph rejection is what costs.
- **Level-wise canonical-form sets, not the canonical-parent test**: with the children
  restricted to the three extensions, the vertex the shared canonicalization reports (the
  canonically last one) need not be prunable, and a class whose canonically last vertex is
  neither pendant nor a twin would then have every one of its productions rejected — the
  orbit test of `triangle_free_unlabeled_enum.h` is only valid when children range over
  *all* neighborhoods. A memory-free version needs a canonical *deletion* chosen among the
  prunable vertices plus that vertex's automorphism orbit, which
  `canonicalize_bitmask_graph` does not report. Keeping one level in a `std::set` of
  canonical forms is what the class counts (7492 at n = 9) make affordable.
- **Every level stays connected by one guard**: an extension of a connected graph is
  connected except for a false twin of an isolated vertex, which happens only for K1, so
  skipping the empty neighborhood mask is the entire connectivity argument. Removing a
  pendant or a twin from a connected graph also keeps it connected (a path through a
  deleted twin of v reroutes through v), which is why the connected members on k vertices
  are *exactly* the extensions of the connected members on k-1.
- **Disconnected members are composed, not searched**: the class is closed under disjoint
  union and membership is per-component, so the general members are the multisets of
  connected ones over the integer partitions of n (the `forest_unlabeled_enum.h` recipe,
  with the non-decreasing index constraint on equal-size parts).
- **Counts**: `connected_only` = A277862 (1, 1, 2, 6, 18, 73, 308, 1484, 7492, ...); all
  members = its Euler transform (1, 2, 4, 11, 31, 114, 454, 2078, 10168, ...). Verified
  through n = 9 against A277862, and through n = 7 against the canonicalized output of the
  labeled enumerator (498416 labeled graphs at n = 7 collapse to exactly the 454 classes
  emitted here); the gtest cross-check stops at n = 6 (n! brute force), the static cases at
  n = 8.
- **The canonicalization is the wall, as everywhere here**: n = 8 takes 0.3 s, n = 9 about
  11 s, n = 10 several minutes — the class contains K_k and K_{1,k-1}, where the exact
  branch-and-bound canonical form degenerates to k!. A cheaper canonical form (refinement
  + automorphism pruning) is what would move that, not a better child rule.

## Ptolemaic unlabeled enumeration (ptolemaic_unlabeled_enum.h)
- **Ptolemaic = chordal + distance-hereditary, so the class inherits the DH vertex-incremental
  search with exactly one extra restriction**: of Bandelt & Mulder's three extensions, the
  pendant vertex is simplicial and the true twin substitutes v by a clique of size two, both of
  which chordal graphs are closed under; only the false twin can create a hole, and it creates
  exactly the induced C4 `u-a-v-b-u` for any two non-adjacent `a, b` in `N(v)`. So the false
  twin is admissible **iff v is simplicial**, and `ptolemaic_unlabeled_is_simplicial` on the
  parent's neighborhood mask is the whole difference from
  `distance_hereditary_unlabeled_enum.h`.
- **The restriction is also necessary for completeness, not just sound**: a Ptolemaic graph is
  distance-hereditary, so it has a pendant vertex or a twin, and deleting one keeps it
  Ptolemaic (hereditary). If the deleted vertex was a false twin of v then `N(v)` was already a
  clique — otherwise that vertex plus two non-adjacent neighbors of v would be an induced C4 in
  a chordal graph. So the connected members on k vertices are *exactly* the restricted
  extensions of the connected members on k-1, and no class is lost. Filtering the DH search
  with `check_ptolemaic` instead would give the same output at the cost of a recognizer call
  per child; the simpliciality test is the same information for free.
- **Everything else is the DH recipe verbatim**: level-wise canonical-form sets rather than the
  canonical-parent test (the restricted children make the canonically last vertex not
  necessarily prunable — see the distance-hereditary section for why the orbit test is invalid
  here), the empty-mask guard as the entire connectivity argument, and the integer-partition
  composition of the disconnected members.
- **Counts**: `connected_only` = A287888 (1, 1, 2, 5, 14, 47, 170, 676, 2834, ...); all members
  = its Euler transform (1, 2, 4, 10, 26, 82, 278, 1053, 4251, ...). Verified through n = 9
  against A287888, and through n = 7 against the canonicalized output of the labeled enumerator
  (312125 labeled graphs at n = 7 collapse to exactly the 278 classes emitted here); the gtest
  cross-check stops at n = 6 (n! brute force), the static cases at n = 8.
- **Range is one step past distance-hereditary, for the same reason**: the class is a subclass,
  so each level is smaller — n = 8 takes 0.3 s, n = 9 about 9 s, n = 10 about two and a half
  minutes (12471 connected representatives) — but the wall is still the exact branch-and-bound
  canonicalization degenerating to k! on K_k and K_{1,k-1}, both of which are Ptolemaic.

## 3-leaf power unlabeled enumeration (three_leaf_power_unlabeled_enum.h)
- **The critical clique tree cuts the extension set to two**: connected 3-leaf powers are
  exactly the clique substitutions of trees (Brandstädt & Le: the critical clique graph is a
  tree with adjacent cliques joined completely — the repo recognizer's own test). A true twin
  grows one critical clique and always stays in the class. A pendant at v stays **iff v has no
  true twin or the graph is complete**: otherwise {v} splits off its critical clique and both
  halves stay joined to each other and to a former neighbor clique — a triangle in the
  critical clique graph, concretely a bull or dart through the new vertex.
- **The false twin is dropped, not restricted**: a connected member with a non-singleton
  critical clique is a true-twin extension of a smaller member, and one with all critical
  cliques singleton *is* its critical clique tree — a tree — hence a pendant extension at a
  vertex without a true twin in the smaller tree (trees on >= 3 vertices have none; K1 and K2
  are complete, so the pendant rule admits them). Pendant + true twin are therefore already
  complete, and the false twin (admissible exactly at a simplicial vertex with no true twin,
  or in a complete graph) would only regenerate graphs the dedup discards. This is the
  opposite situation from distance-hereditary and Ptolemaic, where all three extensions are
  needed for completeness.
- **Everything else is the DH/Ptolemaic recipe verbatim**: level-wise canonical-form sets
  rather than the canonical-parent test (see the distance-hereditary section), connectivity
  for free (both extensions attach the new vertex to an existing one, so no empty-mask guard
  is even needed), and the integer-partition composition of the disconnected members.
- **Counts**: `connected_only` = A277863 (1, 1, 2, 5, 12, 32, 82, 227, 629, ...); all members
  = its Euler transform (1, 2, 4, 10, 24, 65, 171, 478, 1341, ...). Beware the OEIS entry's
  offset: its term at index k is the count on k + 1 vertices (index 9 = 1840 is n = 10).
  Verified through n = 10 against A277863, and through n = 7 against the canonicalized output
  of the labeled enumerator (127115 labeled graphs at n = 7 collapse to exactly the 171
  classes emitted here); the gtest cross-check stops at n = 6 (n! brute force), the static
  cases at n = 9.
- **Range is one step past Ptolemaic, for the same reason**: the class is a subclass, so each
  level is smaller — n = 9 takes about 1 s, n = 10 about 20 s (1840 connected
  representatives), n = 11 about ten minutes (5456, matching
  the next A277863 term) — but the wall is still the exact
  branch-and-bound canonicalization degenerating to k! on K_k and K_{1,k-1}, both of which
  are 3-leaf powers.

## Strongly chordal enumeration (strongly_chordal_labeled_enum.h)
- **Default is Kiyomi's dedicated edge-addition reverse search**: the root is the empty graph, and the parent is defined by deleting the edge between the first non-isolated vertex in the strong elimination ordering and its first neighbor (Kiyomi 2006, Lemma 4.11 / Theorem 4.12). Children add one missing edge and recurse only if that edge is the child's canonical parent edge. Every node is strongly chordal; the search does not filter all chordal graphs.
- **The canonical ordering is Farber's partial-order construction**: at each elimination stage, the relations `N_i[x] ⊂ N_i[y]` are accumulated into the running partial order, and a simple vertex minimal in that order is removed (smallest label on ties). Eliminating an arbitrary simple vertex is fine for recognition but does not necessarily yield a strong elimination ordering, so it must not be used for the parent definition. Simpleness is tested by sorting the neighbors by alive degree and checking consecutive containment of closed neighborhoods.
- **Old search and streaming**: the chordal vertex-addition tree + strongly chordal recognition is kept as `LEGACY_CHORDAL_FILTER` for differential checks. The callback API keeps O(n^2) search state and does not retain all outputs.
- **The original paper's complexity bounds do not carry over**: the paper achieves O(M min(m log n, n^2)) per output and O(n+M) space with a fast strong-ordering construction. This implementation recomputes a naive O(n^4) Farber partial order per candidate edge and uses an adjacency matrix plus full output edge lists.

## Proper chordal enumeration (proper_chordal_labeled_enum.h)
- **Default is a dedicated edge-addition reverse search**: the root is the empty graph. On the deterministic indifference tree-layout T(G) built by the recognizer, let e(G) be the edge of maximum tree distance (lexicographically smallest on ties); the parent of a nonempty proper chordal graph G is G-e(G). Children add one missing edge and recurse only if it becomes the child's canonical parent edge. Every search node is proper chordal; the search does not filter all chordal graphs.
- **Why the parent exists**: take the edge e of maximum tree distance in the indifference tree-layout. If deleting e created a new forbidden indifference triple, there would be an edge of strictly larger tree distance requiring e as its middle edge — a contradiction. Hence the same layout works for G-e, and every nonempty graph has a deletable edge. This reverse search is not published in Paul--Protopapas (STACS 2024); it was derived for this implementation from Theorem 6 of that paper.
- **Old search and streaming**: the chordal vertex-addition tree + proper chordal recognition is kept as `LEGACY_CHORDAL_FILTER` for differential checks. The callback API keeps O(n^2) search state and does not retain all outputs.
- **Complexity caveat**: each search node examines O(n^2) edge-addition candidates and picks the canonical parent edge in O(n^2) directly from the layout returned by the recognizer. With a polynomial-time recognizer this gives polynomial delay and O(n^2) search space; the current recognizer checks nested-convexity by trying all block permutations, so the implemented delay is worst-case factorial.

## Circle recognition (circle.h)
- **Default is Naji's linear system** (polynomial time, decision only): G is a circle graph ⟺ the GF(2) system over variables β(u,v) per ordered pair is solvable, with NS1 (edge vw: β(v,w)+β(w,v)=1), NS2 (edge vw and x non-adjacent to both: β(x,v)+β(x,w)=0), NS3 (non-edge {v,w} with common neighbor x: β(v,w)+β(w,v)+β(x,v)+β(x,w)=1) (Naji 1985 / Gasse 1997 / Geelen–Lee 2020, arXiv:1807.10988).
- **Implementation-level reductions**: NS1 is eliminated by substitution, one variable per edge. NS2 is equivalent to "β(x,·) is constant on each connected component of G−N[x]", so it is eliminated by quotienting to one variable per (x, component). Only NS3 (4 variables per equation) goes into incremental bitset Gaussian elimination that maintains RREF; keeping the basis in RREF makes each insertion a XOR pass over 4 coefficients.
- **Measured**: random G(n,1/2) with n=200 takes 0.13 s (NO); a circle graph from a random chord diagram with n=300 (m≈16000) takes 1.2 s (YES). The old DOW backtracking timed out on NO instances at n=10.
- **DOW_BACKTRACKING is kept for certificates** (the only way to return a DOW on YES; its NO proof is exponential, practical limit n≈9). With Naji as the enumeration filter, `CircleLabeledEnumTest/case6` went from 20 s to 0.2 s.
- **Resource limits**: circle graphs are closed under adding/removing true and false twins, so twin classes are contracted to one representative before Naji (the star K_{1,n-1} has (n-1)^2 variables raw → 1 vertex after contraction). The basis is stored densely at Θ(rank·V) bits, so the actual allocation is checked against a memory cap (default 1 GiB) and exceeded allocations throw std::runtime_error (explicit refusal instead of an OOM kill). DOW also has a step budget (default 2e7) and throws likewise on excess — never conflate "answer unknown" with NO.

## 5-leaf power recognition (five_leaf_power.h)
- **The 3-Steiner-root search is exponential**, and NO instances may effectively never finish. check_five_leaf_power counts a step budget (default five_leaf_power_default_budget = 5e7; 0 means unlimited) and throws std::runtime_error on excess. Silently answering NO is forbidden — budget exhaustion means the answer is unknown, not NO (the CLI reports to stderr and exits 2).
- **Path masks are multi-word**: the former 64-bit limit silently produced false NOs for quotient graphs with 65+ nodes (P_65 was the first wrong answer). The mask table is O(k^2·k/64), so huge components are rejected explicitly by a 256 MB guard.
- **The smallest strongly chordal graph that is not a 5-leaf power has n=7** (tests/five_leaf_power/case10; verified by an independent brute force written directly from the definition). Every strongly chordal graph with n≤6 is a 5-leaf power.

## Circular-arc recognition (circular_arc.h)
- **Circular-arc graphs are not closed under disjoint union**: the arcs of a non-interval component cover the whole circle, leaving no room for other components. A disconnected graph is circular-arc iff every component is interval.
- **Circular clique ordering**: vertices of the starting clique need the wrap-around ("must continue" exemption). Greedy tie-breaking fails on some instances → backtracking is required.
- **Brute-force verification**: complement + C1P (all permutations) verifies up to n≤8. `check_circular_arc_brute.py` (in `tests/legacy/` at git tag `legacy-tests`).

## Trapezoid recognition (trapezoid.h)
- **The Cogis/PS(P) construction is equivalent to G[K̄₂]**: by properties of the lexicographic product it preserves comparability and collapses to permutation recognition.
- **The incidence-poset Inc(P) construction is also wrong**: dim(Inc(P)) ≠ idim(P) in general.
- **Correct approach**: detect a 2+2 pattern in B(P). An edge (x,y) of B(P) means NOT x<_P y. Two edges are incompatible ⟺ the 4 elements are distinct and x₁<_P y₂, x₂<_P y₁. **Excluding trivial 2K₂ (fewer than 4 distinct elements) is essential.**
- **Orientation independence**: a 2K₂ on 4 distinct elements corresponds to a 2+2, and the existence of a 2+2 depends only on comparability, hence not on the choice of transitive orientation.

## Chordal bipartite recognition (chordal_bipartite.h)
- **DLO + Gamma-free is wrong**: Gamma patterns appear even for trees. The correct method is bisimplicial edge elimination (one edge at a time).
- **Bulk removal (N(y)×N(x)) is also wrong**: a non-bisimplicial edge inside a complete bipartite subgraph can lie on an induced cycle.

## Strongly regular unlabeled enumeration (strongly_regular_unlabeled_enum.h)
- **One canonical-augmentation search per parameter tuple, no cross-tuple isomorph
  rejection**: a strongly regular graph determines (k, lambda, mu) uniquely (k is the
  degree; lambda / mu are read off any adjacent / non-adjacent pair, and both kinds of
  pair exist because the definition excludes complete and empty graphs), so the
  per-tuple searches emit disjoint class sets and their outputs concatenate directly.
  The tuple prefilter is the labeled enumerator's `detail::srg_enumerate_params`
  (counting identity + eigenvalue integrality), reused verbatim.
- **Not hereditary, but the windows make the last level exact**: every induced subgraph
  of an srg(n, k, lambda, mu) satisfies the k-regular deficit conditions *and*, per
  vertex pair, cn <= target and cn + min(def(u), def(v), r) >= target (a future common
  neighbor consumes one unit of both deficits). Pruning on these necessary conditions
  never cuts a canonical-deletion chain, and at r = 0 they pin every degree and every
  cn to its target, so leaves are strongly regular by construction — no recognizer call
  anywhere, unlike the permutation/circle/chordal scheme.
- **The cost is the infeasible tuples, not the graphs**: n = 14 (4 classes, ~53 s) is
  slower than n = 15 (6 classes, ~24 s) because tuples that survive the arithmetic
  prefilter but admit no graph (or few) must still be searched to exhaustion; the
  common-neighbor windows are what keep that affordable. Practical to about n = 15.
- **Counts have no single OEIS sequence** (the survey's A088741 is per-tuple): the
  known classification gives 2, 1, 4, 0, 4, 3, 6, 0, 8, 1, 4, 6 for n = 4..15 —
  imprimitive classes are exactly tK_m and their complements, primitive ones start
  C5, Paley(9), Petersen + complement, Paley(13). Static gtest cases stop at n = 13
  (~1.5 s total); n = 14 and 15 were verified by hand.

## Maximal outerplanar unlabeled enumeration (maximal_outer_planar_unlabeled_enum.h)
- **Canonical augmentation does not apply**: the class is not hereditary (deleting a
  vertex leaves an outerplanar graph that is no longer edge-maximal), and unlike
  maximal planar there is no need to grow a hereditary superclass — the dedicated
  bijection is simpler and recognizer-free.
- **Dihedral orbits ARE the isomorphism classes**: a maximal outerplanar graph on
  n >= 3 vertices is a triangulation of a convex n-gon, and a 2-connected outerplanar
  graph has a *unique* Hamiltonian cycle, so any isomorphism maps outer cycle to outer
  cycle and is an element of D_n. Hence one representative per orbit of the diagonal
  set under the 2n dihedral maps (lexicographic-minimum test) suffices — no
  `canonical_edge_list`-style search anywhere.
- **Triangulation enumeration without duplicates**: pop an open chord (i, j) from a
  work stack and choose the apex k of the triangle resting on it (i < k < j); the apex
  per chord is unique in a fixed triangulation, so each of the Catalan(n-2)
  triangulations is reached exactly once. Sub-chords spanning >= 2 polygon edges
  become both new diagonals and new open chords.
- **A000207 continues 733, 2282, 7528 (not 8168)**: matches the Burnside asymptotic
  Catalan(n-2)/(2n); verified n = 3..16 (n = 16: 83898 classes, ~2 s).

## Apollonian network / planar 3-tree (apollonian.h, apollonian_unlabeled_enum.h)
- **Recognition is just maximal planar + chordal**: a simplicial vertex of a maximal
  planar graph on n >= 5 vertices has degree exactly 3 (degree >= 4 with a complete
  neighborhood embeds K5), so chordality forces the stacking structure; deleting the
  simplicial vertex keeps m = 3n - 6 and induction reaches K4/K3.
- **Not hereditary, but degree-3-deletable**: canonical augmentation still applies
  with the deletion rule restricted to degree-3 vertices (their link in a
  triangulation is a triangle, so deletion stays in the class, and one always
  exists). The canonical parent deletes the vertex at the *last canonical position
  of degree 3*: position degrees are reconstructible from the canonical form, and
  `canonicalize_bitmask_graph_orbits` (new opt-in variant in
  util/canonical_augmentation.h) records the automorphism orbit of every position,
  so the accept test is "new vertex in orbit of that position". Same machinery
  should serve k-tree (unlabeled) later.
- **Child validity is planarity alone**: inserting a vertex on a triangle keeps
  chordality (the new vertex is simplicial) and m = 3k - 6 automatically; insertion
  into a face keeps planarity, insertion into a separating triangle never does. So
  one `check_planar` call per candidate, no recognizer.
- **Canonicalize the COMPLEMENT**: the shared lex-min-rows branch-and-bound prefers
  non-adjacent prefixes, and these graphs have large independent sets whose
  orderings all tie at zero rows — n = 11 took 35 s. On the complement the search
  builds clique-in-G prefixes, which die out after four positions because the class
  has no K5: 0.3 s at n = 11, ~100x. Same automorphism group, same position orbits;
  G-degree = k - complement-degree. Worth remembering for any dense
  bounded-clique class fed to this canonicalizer.
- **A027610, not A007173**: both count simplicial 3-clusters ↔ stacked
  triangulations, but A007173 keeps mirror images distinct (rigid motions);
  graph isomorphism includes reflections, so the graph count is A027610(n-3)
  (1, 1, 1, 3, 7, 24, 93, 434, 2110, 11002 for n = 4..13). The dual tree of cells
  does NOT determine the class (two non-isomorphic clusters share the P4 tree at
  n = 7), so no pure tree bijection — the cluster gluing data matters.

## Partial k-tree / treewidth <= k (partial_ktree.h, partial_ktree_unlabeled_enum.h)

- **k must be an input, so the class sits outside every uniform registry**: every
  graph is a partial (n-1)-tree, so unlike ktree.h (where k is the forced minimum
  degree) nothing can be inferred. Consequences: the recognizer CLI reads k on the
  line *before* the standard `n m` header, the enumerator CLI reads `n k` (the
  kregular model), and there are no Python bindings — the `_TYPES` / `_ENUM_TYPES`
  factories generate fixed `(n_or_graph, edges)` signatures. The README counts now
  read "78 of 79 recognizers bound"; touch those sentences when the next
  parameterized class lands.
- **The recognizer is the O*(2^n) elimination-order DP run lazily as a memoized
  DFS**: the fill graph is determined by the *set* of eliminated vertices alone
  (u ~ w iff joined by a path with eliminated interior), so memoizing failed
  elimination sets on a std::set of bitmask words is exactly the
  Bodlaender-Fomin-Koster-Kratsch-Thilikos DP with only the reachable states.
  Three prunings do almost all the work at library sizes: forced elimination of
  any simplicial vertex of fill degree <= k (safe by tw(G) = max(deg v, tw(G-v))),
  branch death on a simplicial vertex of fill degree > k (closed fill neighborhood
  is a clique on > k+1 vertices), and unconditional success once <= k+1 vertices
  remain. Degeneracy <= treewidth rejects cheap NOs up front. Do NOT try to
  replace the recognizer with "new vertex's neighborhood is completable" local
  tests — treewidth is not determined by any local condition.
- **Certificate = the elimination order itself**: replaying it and taking the max
  fill degree both validates the YES answer in tests (independent 2^n-free code)
  and yields `width`, an upper bound on the treewidth that need not be tight —
  don't advertise it as the exact treewidth. Read backwards the order is a k-tree
  construction order of a chordal completion.
- **The enumerator is the plain hereditary canonical-augmentation clone**
  (chordal_unlabeled_enum.h with a k parameter): minor-closed ⇒ hereditary ⇒
  recognizer-prune every level. k = 1 and k = 2 must reproduce
  forest_unlabeled_enum / series_parallel_unlabeled_enum class-for-class — those
  cross-checks plus k >= n-1 = A000088 pinned three of the four test axes for
  free. The treewidth <= 3 counts (1, 2, 4, 11, 33, 145, 861, 7604 for n = 1..8)
  are not in the OEIS (checked via the search API; oeis.org 403s plain fetches,
  `curl "https://oeis.org/search?q=...&fmt=json"` works and returns `null` for no
  match).
