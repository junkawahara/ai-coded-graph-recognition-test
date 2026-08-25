# Algorithm design notes

Pitfalls to know before reimplementing or modifying the recognition/enumeration
algorithms of individual graph classes (moved out of CLAUDE.md). For the
decomposition-structure work see `decomposition_notes.md`; for NO-certificate
extraction see `obstruction_notes.md`.

## Chordal enumeration (chordal_enum.h)
- **Default is the dedicated Kiyomi--Uno reverse search**: the root is the one-edge graph, and the parent is defined by removing the minimum-degree simplicial vertex (ties broken by smallest label). Children attach an unused vertex `v` to a clique `C`, generating only the `|C| < k`, `|C| = k`, `|C| = k+1` cases of Lemmas 1/2 in the paper.
- **Isolated vertices are implicit**: `KiyomiUnoChordalState::alive` holds only the vertices currently incident to at least one edge. A disconnected K2 component adds two unused vertices at once, with `v < w` deduplicating `(v,{w})` / `(w,{v})`. The empty graph is emitted exactly once, outside the search tree.
- **PEO and simplicial status are updated incrementally**: the child's new vertex goes to the front of the PEO. An existing simplicial vertex `u in C` stays simplicial iff `N_G(u) ⊆ C`, so this is updated differentially instead of re-running recognition.
- **The old search is kept for dependent code**: `ChordalEnumState` / `collect_children_reverse_search` are used by the various chordal-subclass enumerators and by the legacy differential checks of interval / strongly chordal. The public chordal API selects it via `LEGACY_VERTEX_REVERSE_SEARCH`.
- The paper's O(1) amortized / O(1) delay bounds apply to an optimized differential-output implementation. The current implementation keeps a simple O(n^2) state and builds the full edge list on every callback, so those bounds do not apply.

## Split enumeration (split_enum.h)
- **Default is a dedicated canonical KS-partition enumeration**: it directly generates S-max partitions `V = K ∪ S`, where `K` is a clique, `S` is an independent set, and every `k in K` is required to have a nonempty neighborhood inside `S`; all candidates are then split graphs and no recognition filter is needed.
- **Swing vertices deduplicate**: multiple S-max partitions exist only when the swing-vertex set `A` is a clique. On the `S` side, `a` is complete to `K` and every vertex of `A-{a}` has `S`-neighborhood exactly `{a}`. Only the partition with `a = min(A)` is accepted.
- **Old search and streaming**: the chordal vertex-addition tree + split recognition is kept as `LEGACY_CHORDAL_FILTER` for differential checks. The callback API keeps only O(n^2) search state and builds the full edge list per output.

## Interval enumeration (interval_enum.h)
- **Default is the dedicated Kiyomi--Kijima--Uno reverse search**: the root is K_n and edges are deleted one at a time. The parent is defined by adding the edge between the largest-label non-universal vertex and its closest non-neighbor in the interval model.
- **Child candidates are limited by a pivot**: deleting an edge between two vertices below the pivot cannot lead back to the current node as parent. Vertices above the pivot are universal true twins, so interval recognition runs once per fixed partner and the result is reused for the corresponding children of every label.
- **The old chordal-filter search remains**: selectable via `LEGACY_CHORDAL_FILTER`, used for set-difference tests against the dedicated search.
- **The original paper's O(n^3)/output time bound does not carry over** (the O(n^2) space bound does): the implementation calls the existing `check_interval` per child candidate and builds the full edge list per callback. The streaming API itself keeps O(n^2) state.

## Strongly chordal enumeration (strongly_chordal_enum.h)
- **Default is Kiyomi's dedicated edge-addition reverse search**: the root is the empty graph, and the parent is defined by deleting the edge between the first non-isolated vertex in the strong elimination ordering and its first neighbor (Kiyomi 2006, Lemma 4.11 / Theorem 4.12). Children add one missing edge and recurse only if that edge is the child's canonical parent edge. Every node is strongly chordal; the search does not filter all chordal graphs.
- **The canonical ordering is Farber's partial-order construction**: at each elimination stage, the relations `N_i[x] ⊂ N_i[y]` are accumulated into the running partial order, and a simple vertex minimal in that order is removed (smallest label on ties). Eliminating an arbitrary simple vertex is fine for recognition but does not necessarily yield a strong elimination ordering, so it must not be used for the parent definition. Simpleness is tested by sorting the neighbors by alive degree and checking consecutive containment of closed neighborhoods.
- **Old search and streaming**: the chordal vertex-addition tree + strongly chordal recognition is kept as `LEGACY_CHORDAL_FILTER` for differential checks. The callback API keeps O(n^2) search state and does not retain all outputs.
- **The original paper's complexity bounds do not carry over**: the paper achieves O(M min(m log n, n^2)) per output and O(n+M) space with a fast strong-ordering construction. This implementation recomputes a naive O(n^4) Farber partial order per candidate edge and uses an adjacency matrix plus full output edge lists.

## Proper chordal enumeration (proper_chordal_enum.h)
- **Default is a dedicated edge-addition reverse search**: the root is the empty graph. On the deterministic indifference tree-layout T(G) built by the recognizer, let e(G) be the edge of maximum tree distance (lexicographically smallest on ties); the parent of a nonempty proper chordal graph G is G-e(G). Children add one missing edge and recurse only if it becomes the child's canonical parent edge. Every search node is proper chordal; the search does not filter all chordal graphs.
- **Why the parent exists**: take the edge e of maximum tree distance in the indifference tree-layout. If deleting e created a new forbidden indifference triple, there would be an edge of strictly larger tree distance requiring e as its middle edge — a contradiction. Hence the same layout works for G-e, and every nonempty graph has a deletable edge. This reverse search is not published in Paul--Protopapas (STACS 2024); it was derived for this implementation from Theorem 6 of that paper.
- **Old search and streaming**: the chordal vertex-addition tree + proper chordal recognition is kept as `LEGACY_CHORDAL_FILTER` for differential checks. The callback API keeps O(n^2) search state and does not retain all outputs.
- **Complexity caveat**: each search node examines O(n^2) edge-addition candidates and picks the canonical parent edge in O(n^2) directly from the layout returned by the recognizer. With a polynomial-time recognizer this gives polynomial delay and O(n^2) search space; the current recognizer checks nested-convexity by trying all block permutations, so the implemented delay is worst-case factorial.

## Circle recognition (circle.h)
- **Default is Naji's linear system** (polynomial time, decision only): G is a circle graph ⟺ the GF(2) system over variables β(u,v) per ordered pair is solvable, with NS1 (edge vw: β(v,w)+β(w,v)=1), NS2 (edge vw and x non-adjacent to both: β(x,v)+β(x,w)=0), NS3 (non-edge {v,w} with common neighbor x: β(v,w)+β(w,v)+β(x,v)+β(x,w)=1) (Naji 1985 / Gasse 1997 / Geelen–Lee 2020, arXiv:1807.10988).
- **Implementation-level reductions**: NS1 is eliminated by substitution, one variable per edge. NS2 is equivalent to "β(x,·) is constant on each connected component of G−N[x]", so it is eliminated by quotienting to one variable per (x, component). Only NS3 (4 variables per equation) goes into incremental bitset Gaussian elimination that maintains RREF; keeping the basis in RREF makes each insertion a XOR pass over 4 coefficients.
- **Measured**: random G(n,1/2) with n=200 takes 0.13 s (NO); a circle graph from a random chord diagram with n=300 (m≈16000) takes 1.2 s (YES). The old DOW backtracking timed out on NO instances at n=10.
- **DOW_BACKTRACKING is kept for certificates** (the only way to return a DOW on YES; its NO proof is exponential, practical limit n≈9). With Naji as the enumeration filter, `CircleEnumTest/case6` went from 20 s to 0.2 s.
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
