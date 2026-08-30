# Implementation plan: unimplemented enumeration algorithms

Result of surveying `notes/enum_alg.md` (entries marked `[ ]`) against the
headers actually present in `include/enumerators/` (2026-08-26).
Check an item off here and flip the corresponding `[ ]` in `notes/enum_alg.md`
when it is implemented.

## Fixed-n graph-class enumerators (unimplemented survey entries)

- [ ] **Hypohamiltonian** — dedicated generation algorithm (Goedgebeur–Zamfirescu 2017); complete lists known up to n=17. PDF: `references/goedgebeur2017_hypohamiltonian.pdf`
- [ ] **K₂-Hypohamiltonian** — generation algorithm for all non-isomorphic K₂-hypohamiltonian graphs (Goedgebeur–Zamfirescu 2024). PDF: `references/goedgebeur2024_k2_hypohamiltonian.pdf`
- [ ] **Hypotraceable** — constructions from hypohamiltonian graphs (Wiener–Zamfirescu 2018); smallest example has 34 vertices (Horton)
- [ ] **Lattice** — poset enumeration + lattice-condition filter, or the dedicated algorithm of Heitzig–Reinhold (2002); OEIS A006966
- [ ] **Vertex-Transitive** — generate graphs invariant under each transitive permutation group, then reject isomorphs (McKay–Royle 1990; Holt–Royle 2020 census up to 47 vertices). PDF: `references/holt2020_vertex_transitive_census.pdf`
- [ ] **Circulant** — enumerate connection sets S ⊂ Z_n + isomorph rejection; closed formulas for prime / squarefree orders (Liskovets 2006; Mishna 2003). PDF: `references/mishna2003_cayley_graph_enum.pdf`
- [ ] **Planar Triangulation with Minimum Degree 5** — canonical construction path via local expansions, as in plantri `-m5` (Brinkmann–McKay 2005); min degree ≥ 4 variant analogous. PDF: `references/brinkmann2005_min_degree5_triangulation.pdf`
- [ ] **Maximal Outerplanar / Simple 2-Tree** — O(1)-per-graph recursive (fan-decomposition) construction, or duals of triangulations (Bodirsky–Fusy–Kang–Vigerske 2007); OEIS A000207
- [ ] **Apollonian Network / Planar 3-Tree** — recursive triangle subdivision; bijection with ternary trees (generalized Catalan numbers C(3n,n)/(2n+1))
- [ ] **Disk Triangulation** — canonical construction with a distinguished outer face, as in plantri `-d` (Brinkmann–McKay 2007). PDF: `references/brinkmann2007_plantri_full.pdf`
- [ ] **Partial k-Tree / Treewidth ≤ k** — constructive enumeration via algebraic representations of bounded-width graphs (Dinneen 1997), or geng + treewidth filter
- [ ] **k-Degenerate** — labeled constructive enumeration via well-orderings with exact counting formula (Bauer–Krug–Wagner, ANALCO 2010); unlabeled via geng + degeneracy filter
- [ ] **Toroidal (genus 1)** — enumerate all graphs, exclude planar ones, test torus embeddability (Mohar–Thomassen); OEIS A319114/A319115
- [ ] **Cage / (k,g)-Graph** — orderly generation of k-regular graphs with girth constraints, as in GENREG (Meringer 1999). PDF: `references/meringer1999_genreg_cages.pdf`
- [ ] **Dually Chordal** — geng + dually-chordal filter; the class is not hereditary, so reverse search does not apply directly (Brandstädt–Dragan–Chepoi–Voloshin 1998)
- [ ] **x-Monotone Curve Intersection Graph** — only counting bounds are known (Fox–Pach–Suk, GD 2024); no practical enumeration route is recorded in the survey. PDF: `references/fox2024_xmonotone_curves.pdf`

## Gaps found outside the survey

- [ ] **Triconnected (3-connected)** — `include/recognizers/triconnected.h` exists but there is no enumerator, and `notes/enum_alg.md` has no section for it (it covers only biconnected and polyhedral = 3-connected planar). Candidate: generation from wheels by Tutte's wheel theorem, or canonical augmentation + 3-connectivity filter. A survey section should be added alongside the implementation.

## Unlabeled (non-isomorphic) enumerator variants

39 classes have one (tree, forest, caterpillar, unicyclic, halin, fullerene,
simple_quadrangulation, chain, cochain, threshold, proper_interval,
trivially_perfect, cograph, cluster, triangle_free, bipartite, permutation,
circle, eulerian, biconnected, chordal, split, planar, self_complementary,
distance_hereditary, ptolemaic, three_leaf_power, co_chordal, cubic,
kregular, snark, laman, interval, co_interval, outer_planar,
series_parallel, cactus, bipartite_permutation, maximal_planar); the other 34 classes with a labeled enumerator do not. Convention: `notes/enum_alg.md`
"Unlabeled (non-isomorphic) enumerators" section in `CLAUDE.md` — separate
`<type>_unlabeled_enum.h` beside the labeled header, plus the canonicalized
labeled↔unlabeled cross-check test (n ≤ 6).

### With a dedicated non-isomorphic algorithm documented in the survey

- [x] **Interval (unlabeled)** — O(n⁴)-delay non-isomorphic enumeration (Yamazaki et al., WALCOM 2018 / TCS 2020), improved to O(n³ log n) delay (Mikos, DMTCS 2021); OEIS A005975. Implemented via the generic canonical-augmentation route (`check_interval` pruning), with the dedicated algorithms referenced in the header; the survey's delay bounds do not apply
- [x] **Cograph (unlabeled)** — O(n)-delay direct generation of non-isomorphic cotrees; A000084
- [x] **Trivially Perfect (unlabeled)** — bijection with rooted forests (A000081); reuse the rooted-tree machinery of `tree_unlabeled_enum.h`
- [x] **Cluster (unlabeled)** — one graph per integer partition (A000041); trivial direct construction
- [x] **Distance-Hereditary (unlabeled)** — O(n³)-delay non-isomorphic enumeration via a vertex-incremental characterization
- [x] **Ptolemaic (unlabeled)** — O(n³)-delay non-isomorphic enumeration via a vertex-incremental characterization
- [x] **3-Leaf Power (unlabeled)** — O(n³)-delay non-isomorphic enumeration via a vertex-incremental characterization
- [x] **Bipartite Permutation (unlabeled)** — BDD-based polynomial-time non-isomorphic enumeration; O(n) uniform random generation known (Saitoh et al.). Implemented via the generic canonical-augmentation route (`check_bipartite_permutation` pruning), with the dedicated algorithms (Saitoh–Otachi–Yamanaka–Uehara 2012; Kawahara–Saitoh–Takeda–Yoshinaka–Yoshioka, TCS 1003, 2024) referenced in the header; the survey's polynomial bounds do not apply. Neither the total counts (1, 2, 3, 7, 13, 34, 81, 239, 693, ...) nor the connected counts (1, 1, 1, 3, 5, 16, 38, 126, 375, ...) are in the OEIS
- [x] **Permutation (unlabeled)** — canonical deletion (Johnston 2020); A123448
- [x] **Circle (unlabeled)** — canonical deletion (Johnston 2020), computed up to n=13; A156809
- [x] **Bipartite (unlabeled)** — canonical augmentation à la nauty genbg; A033995
- [x] **Triangle-Free (unlabeled)** — canonical augmentation with triangle-forbidding pruning à la geng `-t`; A006785
- [x] **Eulerian (unlabeled)** — Polya/Burnside counting documented; enumeration via canonical augmentation + even-degree constraint; A002854
- [x] **Biconnected (unlabeled)** — canonical augmentation with connectivity constraints (geng `-C` style); A002218
- [x] **Maximal Planar (unlabeled)** — canonical construction path à la plantri; A000109. Implemented via the generic canonical-augmentation route over planar graphs (`check_planar` + class-invariant edge-count window pruning, m = 3n-6 filter at the last level), with plantri (Brinkmann–McKay 2007) referenced in the header; the survey's throughput does not apply
- [ ] **Polyhedral (unlabeled)** — plantri-style canonical construction path; A000944
- [ ] **Cubic Planar (unlabeled)** — plantri-style (generate triangulations and dualize)
- [x] **Cubic (unlabeled)** — canonical deletion à la snarkhunter; A005638
- [x] **k-Regular (unlabeled)** — orderly generation à la GENREG (Meringer 1999). C++/CLI only: the extra k parameter does not fit the Python `_ENUM_TYPES` factory signature, matching the labeled kregular/ktree/strongly_regular precedent
- [x] **Snark (unlabeled)** — snarkhunter-style generation with look-ahead 3-edge-colorability; A130315. C++/CLI only: the Python factory's ENUM_MAX_N = 6 cap makes a snark enumerator vacuous (no snark has fewer than 10 vertices), matching the labeled snark precedent
- [ ] **Strongly Regular (unlabeled)** — parameter-constrained backtracking + eigenvalue feasibility with isomorph rejection (McKay–Spence)
- [x] **Laman (unlabeled)** — canonical augmentation under (2,3)-sparsity (nauty-laman-plugin style) or Henneberg moves; A227117. C++/CLI only, matching the labeled Laman enumerator (which is not exposed in Python either)
- [x] **Self-Complementary (unlabeled)** — complementing-permutation construction + isomorph rejection; A000171
- [x] **Cactus (unlabeled)** — split-decomposition grammars; rooted version is O(1)/graph; A000083. Implemented via the generic canonical-augmentation route (`check_cactus` pruning), with the dedicated algorithm referenced in the header; the survey's per-graph bound does not apply. A000083 counts the connected cacti; the total counts (1, 2, 4, 9, 20, 51, 133, 380, 1144, ...) are not in the OEIS
- [x] **Series-Parallel (unlabeled)** — O(1) amortized per graph (Kawano–Nakano, IEICE 2005). Implemented via the generic canonical-augmentation route (`check_series_parallel` pruning), with the dedicated algorithm referenced in the header; the survey's per-graph bound does not apply. Counts (1, 2, 4, 10, 27, 92, 360, 1715, 9356, ...) are not in the OEIS
- [x] **Outerplanar (unlabeled)** — rooted version O(1)/graph (Wang–Nagamochi 2010); A111564. Implemented via the generic canonical-augmentation route (`check_outer_planar` pruning), with the dedicated algorithm referenced in the header; the survey's per-graph bound does not apply
- [x] **Co-Interval (unlabeled)** — complements of unlabeled interval graphs; complementation is a bijection on isomorphism classes (the `cochain_unlabeled_enum.h` model). Depends on Interval (unlabeled)
- [x] **Co-Chordal (unlabeled)** — complements of unlabeled chordal graphs (same complementation route). Depends on Chordal (unlabeled)
- [ ] **Co-Comparability (unlabeled)** — complements of unlabeled comparability graphs (same route). Depends on Comparability (unlabeled)
- [ ] **Digraph (unlabeled)** — directg-style: orient each unlabeled undirected graph in all ways with isomorph suppression; A000273
- [ ] **Tournament (unlabeled)** — gentourng-style dedicated generation; A000568
- [ ] **Poset (unlabeled)** — genposetg-style non-isomorphic Hasse-diagram generation (Brinkmann–McKay 2002); A000112

### Generic route only (no dedicated algorithm in the survey)

Route: McKay-style canonical augmentation with recognizer pruning (hereditary
classes prune every intermediate step; non-hereditary ones only at output),
cross-checked against the canonicalized labeled output for n ≤ 6.

- [x] Chordal (unlabeled) — A048193
- [x] Split (unlabeled) — A048194
- [ ] Strongly Chordal (unlabeled)
- [ ] Proper Chordal (unlabeled)
- [ ] Weakly Chordal (unlabeled)
- [ ] Block (unlabeled)
- [ ] AT-Free (unlabeled)
- [ ] Comparability (unlabeled)
- [ ] Circular-Arc (unlabeled)
- [ ] Proper Circular-Arc (unlabeled)
- [ ] Trapezoid (unlabeled)
- [ ] Chordal Bipartite (unlabeled)
- [ ] Convex Bipartite (unlabeled)
- [ ] Biconvex Bipartite (unlabeled)
- [x] Planar (unlabeled) — A005470
- [ ] Line Graph (unlabeled) — A132220
- [ ] Claw-Free (unlabeled) — A086991
- [ ] Diamond-Free (unlabeled)
- [ ] Bull-Free (unlabeled)
- [ ] Gem-Free (unlabeled)
- [ ] P5-Free (unlabeled)
- [ ] Perfect (unlabeled) — A052431
- [ ] Parity (unlabeled)
- [ ] Meyniel (unlabeled)
- [ ] Even-Hole-Free (unlabeled)
- [ ] Odd-Hole-Free (unlabeled)
- [ ] 4-Leaf Power (unlabeled)
- [ ] 5-Leaf Power (unlabeled)
- [ ] k-Tree (unlabeled) — not hereditary; k-clique-restricted augmentation (as in the labeled enumerator) + isomorph rejection
- [ ] Apex (unlabeled) — A215620

## Subgraph (spanning) enumerator variants

Only `chordal_subgraph_enum.h` exists. Feasibility criterion
(`notes/design_notes.md`): the fixed-n search's parent rule must only *delete*
edges, so `{H in class : H ⊆ G}` is closed under it and child generation can be
filtered by host adjacency without touching the canonicality rule.

### Direct adaptations of existing edge-addition reverse searches

- [ ] **Strongly Chordal subgraphs** — `KIYOMI_EDGE_ADDITION` is rooted at the empty graph and its parent deletes one edge (first SEO vertex → first neighbor); apply the `chordal_subgraph_enum.h` recipe verbatim
- [ ] **Weakly Chordal subgraphs** — same recipe on the default `KIYOMI_EDGE_ADDITION` search (youngest-deletable-edge parent)
- [ ] **Proper Chordal subgraphs** — same recipe on `INDIFFERENCE_EDGE_ADDITION` (max-tree-distance parent edge); note the factorial worst-case recognizer delay

### Monotone classes (closed under edge deletion)

The class's edge subsets of any host form a downward-closed set system:
reverse search with parent = "remove the largest edge", children = "add a host
edge above the current maximum that keeps membership". One recognition per
candidate child, polynomial delay. Guard the CLI on m (output is up to `2^m`).

- [ ] **Bipartite subgraphs**
- [ ] **Triangle-Free subgraphs**
- [ ] **Forest subgraphs** — spanning forests = independent sets of the graphic matroid; forest has no labeled header, so `EnumeratedGraph` needs a home
- [ ] **Planar subgraphs**
- [ ] **Outerplanar subgraphs**
- [ ] **Series-Parallel subgraphs** — K4-minor-free per the recognizer, no connectivity requirement, hence monotone
- [ ] **Cactus subgraphs** — the repo definition (every biconnected component an edge or a cycle) has no connectivity requirement, hence monotone
- [ ] **Apex subgraphs** — minor-closed hence monotone; the apex recognizer per candidate child is expensive, guard tightly

### Dedicated structure

- [ ] **Eulerian subgraphs** — the repo defines eulerian as "all degrees even", so the solutions are exactly the host's cycle space (`2^(m−n+c)` elements); iterate over subsets of a fundamental-cycle basis, the same approach as `CYCLE_SPACE_BASIS` in the labeled enumerator

### Related fifth family: supergraph enumeration

- [ ] **Interval supergraph enumerator** — per the survey, Kiyomi–Kijima–Uno (WG 2006) actually enumerate the labeled interval *supergraphs* of a host: the search is rooted at K_n and the parent adds an edge, so `interval_labeled_enum.h` is the empty-host special case (mirror of the chordal situation). Adaptation: children may delete only non-host edges. Relevant to minimal interval completions. Would need its own `<type>_supergraph_enum.h` conventions (new registry, guard on `binom(n,2) − m`).

Other classes (interval/split/threshold/cograph/... as *subgraphs*) are neither
monotone nor equipped with an edge-deletion search; each would need a per-class
"every nonempty member has a deletable edge" lemma plus a canonical parent
choice before a reverse search applies — research items, not listed here.

## Induced subgraph enumerator variants

Only `chordal_bipartite_induced_subgraph_enum.h` exists. Every **hereditary**
class admits the generic recipe: reverse search over vertex sets with parent
`X ∖ {max X}` (hereditary ⇒ still a solution), children `X ∪ {v}` for
`v > max X` passing the recognizer — polynomial delay, n recognitions per node.
The Kurita–Wasa weak-simplicial ordering used for chordal bipartite is the
delay-optimized special case; analogous elimination-ordering parents (e.g.
simplicial vertices for chordal) can replace the recognizer call per class.
Guard the CLI and Python on n (output is `2^n` on an in-class host). Classes
with expensive or budgeted recognizers (perfect, even/odd-hole-free, 5-leaf
power, circle) need tighter n caps.

- [ ] Apex induced subgraphs
- [ ] AT-Free induced subgraphs
- [ ] Biconvex Bipartite induced subgraphs
- [ ] Bipartite induced subgraphs
- [ ] Bipartite Permutation induced subgraphs
- [ ] Block induced subgraphs
- [ ] Bull-Free induced subgraphs
- [ ] Cactus induced subgraphs
- [ ] Chain induced subgraphs
- [ ] Chordal induced subgraphs — simplicial-vertex parent rule available
- [ ] Circle induced subgraphs
- [ ] Circular-Arc induced subgraphs
- [ ] Claw-Free induced subgraphs
- [ ] Cluster induced subgraphs
- [ ] Co-Chordal induced subgraphs
- [ ] Co-Comparability induced subgraphs
- [ ] Co-Interval induced subgraphs
- [ ] Cochain induced subgraphs
- [ ] Cograph induced subgraphs
- [ ] Comparability induced subgraphs
- [ ] Convex Bipartite induced subgraphs
- [ ] Diamond-Free induced subgraphs
- [ ] Distance-Hereditary induced subgraphs
- [ ] Even-Hole-Free induced subgraphs
- [ ] 5-Leaf Power induced subgraphs — recognizer has a step budget; cap n hard
- [ ] Forest induced subgraphs
- [ ] 4-Leaf Power induced subgraphs
- [ ] Gem-Free induced subgraphs
- [ ] Interval induced subgraphs
- [ ] Line Graph induced subgraphs
- [ ] Meyniel induced subgraphs
- [ ] Odd-Hole-Free induced subgraphs
- [ ] Outerplanar induced subgraphs
- [ ] P5-Free induced subgraphs
- [ ] Parity induced subgraphs
- [ ] Perfect induced subgraphs
- [ ] Permutation induced subgraphs
- [ ] Planar induced subgraphs
- [ ] Proper Chordal induced subgraphs
- [ ] Proper Circular-Arc induced subgraphs
- [ ] Proper Interval induced subgraphs
- [ ] Ptolemaic induced subgraphs
- [ ] Series-Parallel induced subgraphs
- [ ] Split induced subgraphs
- [ ] Strongly Chordal induced subgraphs
- [ ] 3-Leaf Power induced subgraphs
- [ ] Threshold induced subgraphs
- [ ] Trapezoid induced subgraphs
- [ ] Triangle-Free induced subgraphs
- [ ] Trivially Perfect induced subgraphs
- [ ] Weakly Chordal induced subgraphs

Excluded as **not hereditary** (vertex removal breaks the definition):
biconnected, triconnected, eulerian, halin, unicyclic, tree, caterpillar,
maximal planar, polyhedral, cubic, cubic planar, k-regular, strongly regular,
self-complementary, snark, laman, k-tree, fullerene, simple quadrangulation.
Digraph-world classes (digraph, tournament, poset) are hereditary in their own
universe but the host infrastructure is the undirected `Graph`, so they would
need separate digraph-host machinery first.
