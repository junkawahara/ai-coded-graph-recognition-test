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

## Cross-cutting enumerator-family gaps

The four enumerator families are unevenly populated; each item below is a
family-level gap rather than a single algorithm.

- [ ] **Spanning subgraph enumerators** — only `chordal_subgraph_enum.h` exists. Other classes closed under edge deletion (forest, bipartite, triangle-free, planar, outerplanar, series-parallel, ...) admit the same host-filtered reverse-search construction (see `notes/design_notes.md`).
- [ ] **Induced subgraph enumerators** — only `chordal_bipartite_induced_subgraph_enum.h` exists. Any hereditary class with a recognizer admits one.
- [ ] **Unlabeled (non-isomorphic) enumerators** — only 11 classes have one (tree, forest, caterpillar, unicyclic, halin, fullerene, simple_quadrangulation, chain, cochain, threshold, proper_interval). Classes with a labeled enumerator but no unlabeled counterpart could get one, enabling the labeled↔unlabeled cross-check tests.
