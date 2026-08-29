# Enumeration algorithms per graph class

For each graph class, this file surveys algorithms for enumerating
non-isomorphic graphs, counting formulas, and OEIS sequences.
`[ ]` means not implemented, `[x]` means implemented.

---

## General frameworks

### Canonical Augmentation (McKay, 1998)
- Builds graphs incrementally by adding vertices/edges to smaller graphs. Only the inverse of the canonical deletion is accepted, guaranteeing a unique construction path per isomorphism class
- nauty/geng implements this method; `geng n` generates all non-isomorphic graphs on n vertices
- **Reference**: McKay, "Isomorph-Free Exhaustive Generation," J. Algorithms 26(2), 1998
- **PDF**: `references/mckay1998_isomorph_free_generation.pdf`

### Reverse Search (Avis & Fukuda, 1996)
- Defines a tree structure on the solution space dynamically and enumerates all solutions by DFS; needs only polynomial space
- **Reference**: Avis, Fukuda, "Reverse Search for Enumeration," Discrete Appl. Math. 65(1-3), 1996

### Orderly Generation (Colbourn & Read, 1979)
- Classic framework that extends only canonical forms of partial solutions, pruning isomorphic intermediate states early
- Variants exist for restricted graph classes, applicable to generating classes with local forbidden conditions such as triangle-free
- **References**: Colbourn, Read, "Orderly Algorithms for Graph Generation," Int. J. Comput. Math. 7, 1979; Colbourn, Read, "Orderly algorithms for generating restricted classes of graphs," J. Graph Theory 3(2), 1979

### Proximity Search (Conte & Uno, 2022)
- Specialized for enumerating maximal subgraphs. Achieves polynomial delay by reducing the out-degree of the solution graph while maintaining strong connectivity
- **Reference**: Conte, Grossi, Marino, Uno, Versari, SIAM J. Computing, 2022 (STOC 2019)
- **PDF**: `references/conte2022_proximity_search.pdf`

### Recursive Graph Transformations (Mestre, 2009)
- Framework generating connected / 2-edge-connected graphs without duplicates by recursively applying elementary transformations
- Manages the generation tree with automorphism-group weighting, giving an alternative line of methods for generation with connectivity
- **Reference**: Mestre, "Generating connected and 2-edge connected graphs," JGAA 13(2), 2009

### BDD-based enumeration (Kawahara et al., 2024)
- For intersection graph classes with O(n)-bit string representations, enumerates on BDDs in time polynomial in n
- Targets: proper interval, bipartite permutation, chain, cochain, threshold
- Extends to enumeration constrained by maximum (bi)clique size or edge count
- **Reference**: Kawahara et al., Theoretical Computer Science 1003, 2024
- **PDF**: `references/kawahara2024_bdd_intersection_graphs.pdf`

### Filter approach
- Generate all non-isomorphic graphs with nauty/geng → filter with a recognition algorithm (`geng n | ./recognizer`)
- Practical up to about n ≤ 11-13. geng's built-in filters (`-C` connected, `-T` triangle-free, etc.) can shrink the search space

### Major software
| tool | purpose | URL |
|--------|------|-----|
| **nauty/geng** | general non-isomorphic graph generation | https://users.cecs.anu.edu.au/~bdm/nauty/ |
| **plantri** | planar graphs / triangulations | https://users.cecs.anu.edu.au/~bdm/plantri/ |
| **GENREG** | regular graph generation | https://www.mathe2.uni-bayreuth.de/markus/reggraphs.html |
| **snarkhunter** | cubic graphs / snarks | (Brinkmann, Goedgebeur, McKay) |
| **SageMath** | nauty wrapper + graph theory in general | https://www.sagemath.org/ |

---

## Enumeration per graph class

### [x] Chordal
| item | content |
|------|------|
| OEIS (labeled) | A058862: 1, 2, 8, 61, 822, 18154, ... |
| OEIS (unlabeled) | A048193: 1, 2, 4, 10, 27, 94, 393, 2119, 14524, ... |
| OEIS (connected unlabeled) | A048192: 1, 1, 2, 5, 15, 58, 272, 1614, 11911, ... |
| Counting | labeled count computable in O(n^7) arithmetic operations |
| Enumeration | dedicated Kiyomi--Uno reverse search. In the differential-output model: O(1) amortized time, O(1) delay, O(n^2) working space at K_n. Non-isomorphic enumeration by canonical augmentation with recognizer pruning (the class is hereditary, so a linear-time `check_chordal` per candidate child prunes every level) |
| Implementation | `include/enumerators/chordal_labeled_enum.h` — labeled exhaustive enumeration with the minimum-degree simplicial vertex as parent. Uses a simple O(n^2) state and a full edge list per graph, so the O(1) bounds for the paper's optimized differential-output implementation do not apply. `include/enumerators/chordal_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path with a `check_chordal` call per candidate child (the permutation/circle scheme) |
| Subgraph enumeration | `include/enumerators/chordal_subgraph_enum.h` — the chordal subgraphs of a **given host graph**, which is the problem Kiyomi--Uno actually state (`chordal_labeled_enum.h` is its `G = K_n` case). Same search with child generation filtered by host adjacency; correct because the parent rule only deletes edges, so the subgraphs of a fixed host are closed under it. Output is `2^m` in the worst case (a forest host) |
| References | Hebert-Johnson, Lokshtanov, Vigoda, ESA 2023 (counting); Kiyomi, Uno, IEICE Trans. E89-D(2), 2006 (enumeration) |
| PDF | `references/hebert-johnson2023_counting_chordal.pdf` |
| Notes | uniform random unlabeled generation is possible in expected polynomial time (Hien, Patel, Sah, Sawhney, STACS 2025) |

### [x] Interval
| item | content |
|------|------|
| OEIS (labeled) | A005215: 1, 2, 8, 61, 822, 17914, 571475, ... |
| OEIS (unlabeled) | A005975: 1, 2, 4, 10, 27, 92, 369, 1807, ... |
| OEIS (connected unlabeled) | A005976: 1, 1, 2, 5, 15, 56, 250, 1328, ... |
| Counting | implicit enumeration via generating functions (Hanlon, 1982) |
| Enumeration | labeled interval supergraphs by edge-deletion reverse search in O(n^3) time / O(n^2) space per output (Kiyomi--Kijima--Uno, WG 2006). Non-isomorphic enumeration with O(n^4) delay (Yamazaki et al., WALCOM 2018 / TCS 2020), improved to O(n^3 log n) delay (Mikos, DMTCS 2021) |
| Implementation | `include/enumerators/interval_labeled_enum.h` — **default is the dedicated Kiyomi--Kijima--Uno reverse search**. Rooted at K_n; the parent operation adds the edge from the largest-label non-universal vertex to its closest non-neighbor in the interval model. The old chordal vertex-addition + interval filter remains as `LEGACY_CHORDAL_FILTER` |
| References | Kiyomi, Kijima, Uno, WG 2006; Yamazaki, Saitoh, Kiyomi, Uehara, TCS 806, 2020; Mikos, DMTCS 23(1), 2021; Hanlon, Trans. AMS 272, 1982; Yang, Pippenger, Proc. AMS Ser. B 4, 2017 |

### [x] Proper Interval / Unit Interval
| item | content |
|------|------|
| OEIS | A005217 family (Hanlon's enumeration) |
| OEIS (unlabeled) | A005217: 1, 2, 4, 9, 21, 55, 151, 447, ... |
| OEIS (connected unlabeled) | A007123: 1, 1, 2, 4, 10, 26, 76, 232, ... = (Catalan(n-1) + C(n-1, floor((n-1)/2))) / 2 |
| Enumeration | reverse search, **O(1) amortized** per graph; BDD-based enumeration of non-isomorphic proper interval graphs in time polynomial in n |
| Implementation | `include/enumerators/proper_interval_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/proper_interval_unlabeled_enum.h` — non-isomorphic enumeration via the Saitoh et al. bracket-string representation (one string per class, canonical up to reverse-flip), composed over integer partitions for disconnected graphs; `connected_only` restricts the output to A007123 |
| References | Saitoh, Yamanaka, Kiyomi, Uehara, WALCOM 2009 / IEICE Trans. E93-D(7), 2010; Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

### [x] Split
| item | content |
|------|------|
| OEIS (labeled) | A179534 |
| OEIS (unlabeled) | A048194: 1, 2, 4, 9, 21, 56, 164, 557, 2223, ... |
| OEIS (connected unlabeled) | first differences of A048194 (no own entry): 1, 1, 2, 5, 12, 35, 108, 393, 1666, ... — a disconnected split graph is a smaller split graph plus isolated vertices (two components with an edge each would induce a 2K2) |
| Counting | closed formula exists (a sum over clique sizes k) |
| Enumeration | directly generates S-max KS-partitions, accepting only the partition that puts the smallest label of a clique-forming swing-vertex set on the S side. All candidates are split graphs; no recognition filter needed. Non-isomorphic enumeration by canonical augmentation with recognizer pruning (the class is hereditary, so a `check_split` degree-sequence test per candidate child prunes every level) |
| Implementation | `include/enumerators/split_labeled_enum.h` — default is the dedicated `KS_PARTITION_CANONICAL`. The old chordal subtree + split pruning is `LEGACY_CHORDAL_FILTER`. Streaming via callbacks supported. `include/enumerators/split_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path with a `check_split` call per candidate child (the chordal scheme) |
| References | Bina, Pribil, Comment. Math. Univ. Carolin. 56(2), 2015 (counting); Cheng, Collins, Trenk, Discrete Math. 339(9), 2016 (swing-vertex structure); Troyka, EJC 26(2), 2019 (colored split graphs / S-max partitions); McKay, J. Algorithms 26, 1998 (canonical construction path) |

### [x] Threshold
| item | content |
|------|------|
| OEIS (labeled) | A005840: 1, 1, 2, 8, 46, 332, 2874, ... (restricted to connected, half of that for n ≥ 2: 1, 1, 4, 23, 166, 1437, ...) |
| OEIS (unlabeled) | 2^(n-1) graphs (directly from the binary-string characterization) |
| Counting | closed formula (an expression via Eulerian numbers) |
| Enumeration | exhaustive enumeration of binary strings (each step adds an isolated or dominating vertex); BDD-based enumeration of non-isomorphic threshold graphs in time polynomial in n |
| References | Beissinger, Peled, Graphs and Combinatorics 3, 1987; Galvin, Wesley, Zacovic, JIS 25, 2022; Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |
| PDF | `references/galvin2022_threshold_enum.pdf` |

### [x] Trivially Perfect / Quasi-Threshold
| item | content |
|------|------|
| OEIS (labeled) | 1, 2, 8, 49, 402, ... (n=1,...,5) |
| OEIS (unlabeled) | A000081(n+1): 1, 2, 4, 9, 20, 48, 115, 286, ... (rooted forests on n nodes) |
| OEIS (connected unlabeled) | A000081(n): 1, 1, 2, 4, 9, 20, 48, 115, ... (rooted trees on n nodes) |
| Counting | constructive enumeration via recursive construction of the UVD tree |
| Implementation | `include/enumerators/trivially_perfect_labeled_enum.h` (UVD_TREE). `include/enumerators/trivially_perfect_unlabeled_enum.h` — non-isomorphic enumeration via the rooted-forest bijection (ancestor closure of the canonical rooted trees of `tree_unlabeled_enum.h`, composed over integer partitions); `connected_only` restricts the output to A000081(n) |
| References | Galvin, Wesley, Zacovic, JIS 25, 2022; Wolk, Proc. AMS 13(5), 1962; Beyer, Hedetniemi, SIAM J. Comput. 9(4), 1980 |

### [x] Strongly Chordal
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 61, 822, 18034, ... (n=1,...,6) |
| Enumeration | reverse search (pruned enumeration as a subtree of chordal). Prunable because the class is hereditary |
| Implementation | `include/enumerators/strongly_chordal_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | for n≤5 all chordal graphs are strongly chordal (the 3-sun has 6 vertices). The gap to chordal (18154) appears at n=6 |

### [x] Ptolemaic
| item | content |
|------|------|
| OEIS (connected labeled) | A287886: 1, 1, 4, 35, 481, 9042, ... |
| OEIS (all labeled) | 1, 2, 8, 61, 762, 13534, ... (n=1,...,6) |
| OEIS (connected unlabeled) | A287888: 1, 1, 2, 5, 14, 47, 170, 676, ... |
| Enumeration | reverse search (pruned enumeration as a subtree of chordal). Polynomial delay (Nakano, Uno); full enumeration / random generation via split-decomposition grammars; **O(n^3)**-delay enumeration based on a vertex-incremental characterization |
| Implementation | `include/enumerators/ptolemaic_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/ptolemaic_unlabeled_enum.h` — non-isomorphic enumeration from the vertex-incremental characterization (the distance-hereditary pendant / true twin / false twin extensions of the connected members one level down, the false twin restricted to simplicial vertices so chordality is preserved; isomorph rejection by a canonical-form set per level, no recognizer call), disconnected members composed as multisets of components over the integer partitions of n |
| References | Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023; Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018; Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 |

### [x] Block
| item | content |
|------|------|
| OEIS (connected unlabeled) | A035053: 1, 1, 2, 4, 9, 22, 59, 165, 496, ... |
| OEIS (labeled) | 1, 2, 8, 55, 562, 7739, 134808, ... |
| Enumeration | Nakano-Uno framework: chordal reverse search + block-property pruning; full enumeration via split-decomposition grammars |
| Implementation | `include/enumerators/block_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018 |

### [x] Weakly Chordal
| item | content |
|------|------|
| OEIS (connected unlabeled) | A079457 |
| Enumeration | Kiyomi's edge-addition reverse search. Rooted at the empty graph, with the unique parent defined by the youngest deletable edge |
| Implementation | `include/enumerators/weakly_chordal_labeled_enum.h` — labeled exhaustive enumeration. The conventional method is `GENERIC_VERTEX_AUGMENTATION` |
| References | Kiyomi, Ph.D. thesis, 2006, §4.1.5; Hayward, J. Graph Theory 21, 1996 |

### [x] AT-Free
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 64, 1024, 31748, ... (n=1,...,6) |
| Enumeration | reverse search (vertex addition + AT-free test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/at_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | AT-free is hereditary (closed under induced subgraphs). For n ≤ 5 every graph is AT-free |
| References | Corneil, Olariu, Stewart, SIAM J. Discrete Math. 10(3), 1997 |

---

### [x] Permutation
| item | content |
|------|------|
| OEIS (unlabeled) | A123448: 1, 2, 4, 11, 33, 142, 776, 5699, 50723, ... |
| Enumeration | canonical deletion (Johnston, 2020); polynomial delay (Yamazaki et al., TCS 2019) |
| Implementation | `include/enumerators/permutation_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/permutation_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path / canonical deletion (permutation graphs are hereditary, so the search grows one vertex at a time; with no cheap incremental membership test the pruning is a `check_permutation` call per candidate child, run before the canonicalization of `util/canonical_augmentation.h`); `connected_only` filters at emission |
| Notes | canonical deletion reaches n=13 in about 44 CPU hours |
| References | Yamazaki, Saitoh, Kiyomi, Uehara, TCS 2019 |

### [x] Comparability
| item | content |
|------|------|
| OEIS | asymptotically about half the number of posets (A000112) (Möhring's theorem: almost all comparability graphs are UPO) |
| Enumeration | reverse search (vertex addition + comparability test). Prunable because the property is hereditary |
| Implementation | `include/enumerators/comparability_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Möhring, "Almost all comparability graphs are UPO," Discrete Math. 1984 |

### [x] Co-Comparability
| item | content |
|------|------|
| Enumeration | reverse search by vertex addition + co-comparability test. Prunes by testing whether the complement is a comparability graph |
| Implementation | `include/enumerators/co_comparability_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |

### [x] Co-Chordal
| item | content |
|------|------|
| OEIS (labeled) | A058862: 1, 2, 8, 61, 822, 18154, ... (same as chordal: complementation is a bijection) |
| Enumeration | reverse search by vertex addition + co-chordal test. Prunes by testing whether the complement is chordal |
| Implementation | `include/enumerators/co_chordal_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |

### [x] Co-Interval
| item | content |
|------|------|
| OEIS (labeled) | same as interval (complementation is a bijection): 1, 2, 8, 61, 822, ... |
| Enumeration | reverse search by vertex addition + co-interval test. Prunes by testing whether the complement is interval |
| Implementation | `include/enumerators/co_interval_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |

### [x] Cograph (P4-free)
| item | content |
|------|------|
| OEIS (unlabeled) | A000084: 1, 2, 4, 10, 24, 66, 180, 522, 1532, ... |
| OEIS (connected unlabeled) | A000669: 1, 1, 2, 5, 12, 33, 90, 261, 766, ... (= A000084/2 for n >= 2 by the complement bijection) |
| OEIS (labeled) | A006351: 1, 2, 8, 52, 472, 5504, ... |
| Counting | bijection with cotrees (series-parallel networks). Recursive formula available |
| Enumeration | direct enumeration by recursive cotree construction; an **O(n)**-delay algorithm generating non-isomorphic cotrees directly; maximal cograph subgraphs enumerable via proximity search |
| Implementation | `include/enumerators/cograph_labeled_enum.h` — labeled exhaustive enumeration (cotree construction). `include/enumerators/cograph_unlabeled_enum.h` — non-isomorphic enumeration by recursive cotree construction via the complement duality (connected graphs = complements of the disconnected ones, which are composed of smaller connected graphs over integer partitions); `connected_only` restricts the output to A000669 |
| References | Seinsche, 1974 (P4-free characterization); Jones, Protti, Del-Vecchio, TCS 713, 2018; Conte, Kante, Kurita, Uno, Wasa, DAM 2023 (proximity search) |

### [x] Distance-Hereditary
| item | content |
|------|------|
| OEIS (connected unlabeled) | A277862: 1, 1, 2, 6, 18, 73, 308, 1484, 7492, ... |
| Counting | generating functions + symbolic specification (Chauve, Fusy, Lumbroso, 2017) |
| Enumeration | polynomial delay (Nakano, Uno, ISAAC 2020 / DAM 2023); **O(n^3)**-delay non-isomorphic enumeration based on a vertex-incremental characterization |
| Implementation | `include/enumerators/distance_hereditary_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/distance_hereditary_unlabeled_enum.h` — non-isomorphic enumeration from the vertex-incremental characterization (pendant / true twin / false twin extensions of the connected members one level down, isomorph rejection by a canonical-form set per level; no recognizer call), disconnected members composed as multisets of components over the integer partitions of n |
| References | Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 |
| PDF | `references/chauve2017_distance_hereditary_enum.pdf` |

### [x] Circular-Arc
| item | content |
|------|------|
| OEIS | not found |
| OEIS (labeled) | 1, 2, 8, 64, 999, 28081, ... (n=1,...,6) |
| Enumeration | reverse search (vertex addition + circular-arc test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/circular_arc_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | superclass of interval. Not closed under disjoint union (a disconnected graph is circular-arc only if all components are interval). For n ≤ 4 every graph is circular-arc |

### [x] Proper Circular-Arc
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 60, 754, ... (n=1,...,5) |
| Recognition | endpoint-order backtracking + containment-forbidding 2-SAT constraints (exponential time). Claw-freeness is only a necessary-condition filter ("CA ∩ claw-free" fails as a characterization: the net graph is a counterexample) |
| Enumeration | reverse search (vertex addition + proper circular-arc test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/proper_circular_arc_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | superclass of proper interval, subclass of circular-arc. For n ≤ 3 every graph is proper circular-arc (a claw needs 4 vertices). The gap to circular-arc (64) appears at n=4: the 4 labeled stars K_{1,3} are excluded, giving 60 |
| References | Tucker (1974); Deng, Hell, Huang (1996); Lin, Soulignac, Szwarcfiter (2013) |

### [x] Trapezoid
| item | content |
|------|------|
| OEIS | not found |
| OEIS (labeled) | 1, 2, 8, 64, 1012, ... (n=1,...,5) |
| Enumeration | reverse search (vertex addition + trapezoid test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/trapezoid_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | superclass of permutation, subclass of co-comparability. For n≤4 every graph is trapezoid. At n=5, permutation (1012) = trapezoid (1012) = co-comparability (1012) |

---

### [x] Bipartite
| item | content |
|------|------|
| OEIS (labeled) | A047864 |
| OEIS (unlabeled) | A033995: 1, 1, 2, 3, 7, 13, 35, 88, 303, ... |
| OEIS (connected unlabeled) | A005142 |
| Enumeration | nauty/**genbg** with canonical augmentation. ~O(1) amortized per graph |
| Implementation | `include/enumerators/bipartite_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/bipartite_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path (the pruning is a 2-coloring test: the new vertex's neighborhood must meet each component in only one side of its bipartition; the shared `util/canonical_augmentation.h` supplies the canonical form and the canonical-deletion orbit); `connected_only` filters at emission |
| References | McKay, J. Algorithms 1998; Gainer-Dewar, Gessel, EJC 21(2), 2014 |

### [x] Chordal Bipartite
| item | content |
|------|------|
| OEIS | not registered |
| Enumeration | reverse search (vertex addition + chordal bipartite test). Prunable because the property is hereditary |
| Implementation | `include/enumerators/chordal_bipartite_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Induced subgraph enumeration | `include/enumerators/chordal_bipartite_induced_subgraph_enum.h` — the chordal bipartite **induced** subgraphs of a **given host graph**, i.e. the vertex subsets `X` with `G[X]` chordal bipartite (Kurita--Wasa--Arimura--Uno ECB). Reverse search on CBEO: the parent removes the largest weak-simplicial vertex, so `X ∪ {v}` is a child iff `v` is weak-simplicial in `G[X ∪ {v}]` and is the largest such vertex. Simple variant — weak-simplicial sets are recomputed per candidate rather than maintained differentially, so the paper's amortized O(k t Δ²) bound does not apply; only the delay differs. Output is `2^n` in the worst case (a chordal bipartite host), driven by `n`, not `m` |
| References | Kiyomi, Kanno, Otachi, Saitoh, Yamanaka, COCOON 2019 (fixed-n enumeration); Kurita, Wasa, Arimura, Uno, COCOON 2019, LNCS 11653, 339--351, arXiv:1903.02161 (induced subgraph enumeration) |
| PDF | `references/kurita2019_chordal_bipartite_induced_subgraph_enum.pdf` (the induced-subgraph paper; the fixed-n one is not in `references/`) |

### [x] Bipartite Permutation
| item | content |
|------|------|
| OEIS | not registered (small values computed) |
| Enumeration | reverse search, **O(1) amortized** per graph. Uniform random generation in O(n); BDD-based enumeration of non-isomorphic graphs in time polynomial in n |
| Implementation | `include/enumerators/bipartite_permutation_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Saitoh, Otachi, Yamanaka, Uehara, J. Discrete Algorithms 10, 2012 (ISAAC 2009); Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

### [x] Convex Bipartite
| item | content |
|------|------|
| OEIS | not registered |
| Enumeration | reverse search (vertex addition + C1P test) |
| Implementation | `include/enumerators/convex_bipartite_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | consecutive-ones property on one side of the bipartition. Hereditary, so prunable in reverse search |

### [x] Biconvex Bipartite
| item | content |
|------|------|
| OEIS | not registered |
| Enumeration | reverse search (vertex addition + biconvexity test) |
| Implementation | `include/enumerators/biconvex_bipartite_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | consecutive-ones property on both sides. Hereditary, so prunable in reverse search |

### [x] Chain (difference graphs)
| item | content |
|------|------|
| OEIS (unlabeled) | A005418: 1, 2, 3, 6, 10, 20, 36, 72, 136, ... |
| Counting | **closed formula**: a(n) = 2^(n-2) + 2^(floor(n/2)-1) (n ≥ 2) |
| Enumeration | neighborhoods within each side are linearly ordered by inclusion → direct construction; BDD-based enumeration of non-isomorphic chain graphs in time polynomial in n |
| References | Peled, Sun, Discrete Appl. Math. 60(1-3), 1995; Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

### [x] Cochain
| item | content |
|------|------|
| OEIS (unlabeled) | A005418 (same count as chain graphs) |
| Enumeration | chain-graph enumeration + complementation, using that complementation is a bijection on isomorphism classes; BDD-based enumeration of non-isomorphic cochain graphs in time polynomial in n |
| Implementation | `include/enumerators/cochain_unlabeled_enum.h` — builds complements from the chain enumeration |
| Notes | complements of chain graphs; the non-isomorphic count matches chain |
| References | Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

---

### [x] Planar
| item | content |
|------|------|
| OEIS (unlabeled) | A005470: 1, 2, 4, 11, 33, 142, 822, 6966, 79853, ... |
| OEIS (connected unlabeled) | A003094: 1, 1, 2, 6, 20, 99, 646, 5974, 71885, ... |
| Enumeration | **plantri** (Brinkmann, McKay). Over 2 million graphs per second. Non-isomorphic enumeration by canonical augmentation with recognizer pruning (the class is hereditary, so a linear-time `check_planar` left-right test per candidate child prunes every level) |
| Implementation | `include/enumerators/planar_labeled_enum.h` — reverse search (vertex addition + planarity test). `include/enumerators/planar_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path with a `check_planar` call per candidate child (the chordal scheme) |
| References | Brinkmann, McKay, MATCH 58, 2007; Gimenez, Noy, JAMS 2009 (asymptotic formula); McKay, J. Algorithms 26, 1998 (canonical construction path) |
| PDF | `references/brinkmann2007_plantri.pdf` |

### [x] Outerplanar
| item | content |
|------|------|
| OEIS (unlabeled) | A111564 |
| OEIS (connected) | A111563 |
| OEIS (2-connected) | A001004 |
| Counting | asymptotic formula: g_n ~ 0.00910 * n^(-5/2) * 7.504^n |
| Enumeration | plantri (2-connected); rooted version in O(1) per graph (Wang, Nagamochi, 2010) |
| Implementation | `include/enumerators/outer_planar_labeled_enum.h` — reverse search (vertex addition + outerplanarity test) |
| References | Bodirsky, Fusy, Kang, Vigerske, EJC 14, 2007 |
| PDF | `references/bodirsky2007_outerplanar_enum.pdf` |

### [x] Series-Parallel
| item | content |
|------|------|
| Labeled counts | 1, 2, 8, 63, 913, ... (n=1,...,5; graphs with no K4 minor. A000084 / A006351 count two-terminal series-parallel *networks* by edges and are a different sequence) |
| Enumeration | **O(1) amortized** per graph (Kawano, Nakano, IEICE 2005) |
| Implementation | `include/enumerators/series_parallel_labeled_enum.h` — reverse search (vertex addition + SP test) |
| References | Kawano, Nakano, IEICE Trans. E88-A(5), 2005; Bodirsky, Gimenez, Kang, Noy, EuroComb 2005 |

### [x] Cactus
| item | content |
|------|------|
| OEIS (unlabeled) | A000083: 1, 1, 2, 4, 9, 23, 63, 188, ... |
| OEIS (labeled) | A000314 |
| Counting | generating functions + Lagrange inversion |
| Enumeration | O(1) per graph for the rooted version. Enumeration + random generation via split-decomposition trees; full enumeration via split-decomposition grammars |
| Implementation | `include/enumerators/cactus_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Bahrani, Lumbroso, arXiv:1711.10647, 2017; Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018 |
| PDF | `references/bahrani2017_cactus_enum.pdf` |

### [x] Line Graph
| item | content |
|------|------|
| OEIS (unlabeled) | A132220: 1, 2, 4, 10, 24, 63, 166, 471, ... |
| OEIS (connected) | A003089 |
| Enumeration | reverse search (vertex addition + Krausz-partition test). Prunable because the property is hereditary |
| Implementation | `include/enumerators/line_graph_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |

### [x] Claw-Free
| item | content |
|------|------|
| OEIS (unlabeled) | A086991: 1, 2, 4, 10, 26, 85, 302, 1285, ... |
| OEIS (connected) | A022562 |
| Enumeration | reverse search (vertex addition + claw-free test). Prunable because the property is hereditary |
| Implementation | `include/enumerators/claw_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |

### [x] Diamond-Free
| item | content |
|------|------|
| OEIS | not found |
| Enumeration | reverse search (vertex addition + diamond-free test). Prunable because the property is hereditary |
| Implementation | `include/enumerators/diamond_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |

### [x] Perfect
| item | content |
|------|------|
| OEIS (unlabeled) | A052431: 1, 2, 4, 11, 33, 148, 906, 8887, ... |
| OEIS (connected) | A052433 |
| Enumeration | reverse search (vertex addition + SPGT test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/perfect_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Kawahara et al., WALCOM 2023 / TCS 2024 (subclasses); Chudnovsky et al., Ann. Math. 164, 2006 (SPGT) |

### [x] Three-Leaf Power
| item | content |
|------|------|
| OEIS (labeled) | 1, 2, 8, 61, 642, 8254, ... (n=1,...,6) |
| Enumeration | chordal reverse search + 3-leaf-power filter (test whether the critical clique graph is a forest); **O(n^3)**-delay non-isomorphic enumeration based on a vertex-incremental characterization |
| Implementation | `include/enumerators/three_leaf_power_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Brandstädt & Le, IPL 98, 2006 (characterization: (bull, dart, gem)-free chordal); Chauve, Fusy, Lumbroso, ANALCO 2017 (analytic counting); Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 |
| Notes | exact counting also possible via split decomposition + generating functions (same framework as distance-hereditary) |

---

## Tree- and forest-like graph classes

### [x] Tree (unlabeled)
| item | content |
|------|------|
| OEIS (unrooted unlabeled) | A000055: 1, 1, 1, 2, 3, 6, 11, 23, 47, 106, ... |
| OEIS (rooted unlabeled) | A000081: 1, 1, 2, 4, 9, 20, 48, 115, 286, ... |
| OEIS (labeled unrooted) | A000272: n^(n-2) (Cayley's formula) |
| Counting | Otter's formula (1948): derives the unrooted count from the rooted count. Asymptotic formula available |
| Enumeration (unrooted) | **CAT (constant amortized time)** (Wright, Richmond, Odlyzko, McKay, SIAM J. Comput. 1986) |
| Enumeration (rooted) | **CAT** (Beyer, Hedetniemi, SIAM J. Comput. 1980): generation via level sequences |
| Enumeration (labeled) | exhaustive enumeration of Prüfer sequences: n^(n-2) sequences, each decoded in O(n) |
| Implementation | `include/enumerators/tree_unlabeled_enum.h` — non-isomorphic free-tree enumeration (bottom-up recursive construction + centroid decomposition) |
| References | Otter, Ann. Math. 49(3), 1948; Wright et al., SIAM J. Comput. 15(2), 1986; Beyer, Hedetniemi, SIAM J. Comput. 9(4), 1980 |

### [x] Forest
| item | content |
|------|------|
| OEIS (unlabeled) | A005195: 1, 1, 2, 3, 6, 10, 20, 37, 76, 153, ... |
| OEIS (labeled) | A001858 (rooted forests number (n+1)^(n-1)) |
| Counting | unlabeled: Euler transform of A000055 (unrooted trees). Labeled: exponential generating function exp(T(x)) |
| Enumeration | composition of tree enumeration: enumerate integer partitions + combine tree enumerations per size |
| Implementation | `include/enumerators/forest_unlabeled_enum.h` — non-isomorphic forest enumeration via integer partitions + tree composition |
| References | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973 |

### [x] Caterpillar
| item | content |
|------|------|
| OEIS (unlabeled) | A005418 (caterpillars on n+2 vertices): 1, 1, 1, 2, 3, 6, 10, 20, 36, 72, ... (n=1,...) |
| OEIS (labeled) | A245012 |
| Counting | **closed formula**: generating function by Harary-Schwenk (1973). Characterized by spine + leaf distribution |
| Enumeration | enumerate spine lengths → enumerate leaf assignments (combinations) to each spine vertex, accounting for spine reversal symmetry |
| Implementation | `include/enumerators/caterpillar_unlabeled_enum.h` — constructive enumeration (spine + leaf assignment) |
| Notes | coincides with pathwidth-1 graphs. Trees whose vertices are all within distance 1 of a central path |
| References | Harary, Schwenk, "The number of caterpillars," Discrete Math. 6(4), 1973 |

### [x] k-Tree
| item | content |
|------|------|
| OEIS (labeled 2-tree) | A036361: 1, 1, 6, 70, 1215, ... (n=2,...) |
| OEIS (unlabeled 2-tree) | A054581: 1, 1, 1, 2, 5, 12, 39, 136, 529, ... (n=2,...) |
| OEIS (labeled 3-tree) | A036362 |
| Counting (labeled) | Beineke-Pippert (1969): generalization of Cayley-type formulas. Of the form T_k(n) = C(n,k) · (k(n-k)+1)^(n-k-2) |
| Enumeration | recursive k-clique extension: attach a new vertex to an existing k-clique. A structure well suited to reverse search |
| Implementation | `include/enumerators/ktree_labeled_enum.h` — labeled exhaustive enumeration (reverse search, k-clique restricted) |
| Notes | k=1 gives ordinary trees, k=2 maximal outerplanar (n≥3), k=3 Apollonian networks |
| References | Beineke, Pippert, J. Combin. Theory 6(2), 1969; Harary, Palmer, "Graphical Enumeration," 1973 |

---

## Graph classes defined by structural properties

### [x] Unicyclic
| item | content |
|------|------|
| OEIS (connected unlabeled) | A001429: 1, 2, 5, 13, 33, 89, 240, 657, 1806, ... |
| Counting | derived via generating functions. Connected graphs with n vertices and n edges |
| Enumeration | fix the cycle length → constructive enumeration attaching a rooted tree to each cycle vertex. Dihedral symmetry removed by a bracelet normal form |
| Implementation | `include/enumerators/unicyclic_unlabeled_enum.h` — constructive enumeration (cycle + rooted-tree attachment) |
| Notes | connected graphs with edge count = vertex count; contain exactly one cycle |

### [x] Biconnected
| item | content |
|------|------|
| OEIS (unlabeled) | A002218: 0, 0, 1, 3, 10, 56, 468, 7123, 194066, ... |
| OEIS (labeled) | 0, 0, 1, 10, 238, 11368, ... (n=1,...,6; A013922) |
| Recognition | O(n+m) Tarjan cut-vertex detection |
| Enumeration | reverse search (vertex addition + connectivity pruning + 2-connectivity test at the final step). The class is not hereditary, so intermediate pruning is connectivity-based only. Non-isomorphic enumeration by canonical augmentation with geng `-C` style connectivity constraints (G - v is connected for every v of a biconnected G, so level n-1 generates only connected graphs and the last vertex needs degree >= 2 covering all degree-deficient vertices; full cut-vertex test at emission) |
| Implementation | `include/enumerators/biconnected_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/biconnected_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path (all graphs at intermediate levels — the class is not hereditary — with connectivity/degree constraints on the last two levels) |
| Notes | a fundamental structural property: connected graphs without cut vertices (n ≥ 3). Not hereditary (vertex removal can destroy the property) |

### [x] Maximal Planar / Triangulation
| item | content |
|------|------|
| OEIS (unlabeled) | A000109: 1, 1, 1, 2, 5, 14, 50, 233, 1249, 7595, 49566, ... |
| OEIS (rooted) | A000260 |
| Enumeration | **plantri** (Brinkmann, McKay): over 5 million graphs per second. Canonical construction path method, amortized O(n^2) per graph |
| Implementation | `include/enumerators/maximal_planar_labeled_enum.h` — labeled exhaustive enumeration (reverse search + planarity pruning + edge-count constraint) |
| Notes | planar graphs whose faces are all triangles. The dual of a 3-connected planar graph is a triangulation |
| References | Brinkmann, McKay, MATCH 58, 2007 |

### [x] Triangle-Free
| item | content |
|------|------|
| OEIS (unlabeled) | A006785: 1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172, ... |
| OEIS (connected unlabeled) | A024607: 1, 1, 1, 3, 6, 19, 59, 267, ... |
| OEIS (labeled) | A213434: 1, 2, 7, 41, 388, 5789, ... |
| Enumeration | **geng -t** (nauty): canonical augmentation + triangle-forbidding pruning. Checks whether adding an edge creates a triangle |
| Implementation | `include/enumerators/triangle_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/triangle_free_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path (independent-set neighborhoods are the triangle-forbidding pruning; one branch-and-bound canonicalization per candidate child yields both the canonical form and the canonical-deletion orbit); `connected_only` filters at emission |
| Notes | hereditary property, closely tied to Ramsey theory |
| References | McKay, J. Algorithms 26, 1998; Colbourn, Read, J. Graph Theory 3, 1979 |

### [x] Eulerian
| item | content |
|------|------|
| OEIS (unlabeled) | A002854 |
| OEIS (connected unlabeled) | A003049: 1, 0, 1, 1, 4, 8, 37, 184, 1782, 31026, ... |
| Counting (labeled) | **closed formula**: 2^((n-1)(n-2)/2) (the all-degrees-even condition is n-1 independent linear constraints over GF(2)) |
| Enumeration | cycle-space basis enumeration: enumerate symmetric differences of all subsets of fundamental cycles of a spanning tree of K_n. geng + even-degree filter also works. Unlabeled counting via Polya/Burnside; non-isomorphic enumeration by canonical augmentation with a forced last level (odd-degree vertices of G - v are exactly N_G(v), so every (n-1)-vertex graph has a unique Eulerian extension) |
| Implementation | `include/enumerators/eulerian_labeled_enum.h` — labeled exhaustive enumeration (cycle-space basis enumeration). `include/enumerators/eulerian_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path (all graphs at intermediate levels — the class is not hereditary — and the forced even-degree completion at the last); `connected_only` filters at emission |
| Notes | graphs with all degrees even; if connected, they carry an Euler circuit |
| References | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973; McKay, J. Algorithms 26, 1998 |

### [x] Self-Complementary
| item | content |
|------|------|
| OEIS (unlabeled) | A000171: 1, 0, 0, 1, 2, 0, 0, 10, 36, 0, 0, 720, 5600, ... |
| OEIS (labeled) | 1, 0, 0, 12, 72, 0, 0, 98280, ... (n=1,...,8) |
| Counting | **closed formula** (Read, 1963): enumeration via Polya-type cycle indices. a(n)=0 when n mod 4 ∈ {2,3} |
| Enumeration | constructive enumeration exploiting the structure of the complementing permutation σ: for every permutation of valid cycle type (powers of 2, ≥4), compute the orbits of edge pairs and enumerate the 2 choices per orbit. Non-isomorphic: one σ per cycle type suffices (same-type permutations are conjugate) + isomorph rejection by canonical form |
| Implementation | `include/enumerators/self_complementary_labeled_enum.h` — labeled exhaustive enumeration (complementing permutation). `include/enumerators/self_complementary_unlabeled_enum.h` — non-isomorphic enumeration from one complementing permutation per cycle type, with the 2^(r-1) alternating assignments of its r vertex-pair orbits (flipping all orbits only complements the graph) filtered through a global canonical-form set |
| Notes | nonexistent unless n(n-1)/4 is an integer (only n ≡ 0,1 mod 4) |
| References | Read, J. London Math. Soc. 38, 1963; Farrugia, Ph.D. thesis, Univ. Malta, 1999 |

---

## Regular graph classes

### [x] k-Regular
| item | content |
|------|------|
| OEIS (triangle array) | A051031: number of non-isomorphic k-regular graphs on (n vertices, degree k) |
| OEIS (sum over degrees) | A005176 |
| OEIS (4-regular) | A033301 |
| OEIS (5-regular) | A165626 |
| Enumeration | **GENREG** (Meringer, 1999): orderly generation + fast canonicity test. Handles arbitrary k, n |
| Implementation | `include/enumerators/kregular_labeled_enum.h` — labeled exhaustive enumeration (degree-constrained reverse search) |
| Notes | parallelizable (Rouyer et al., 2019: 4-regular enumerated up to 23 vertices) |
| References | Meringer, J. Graph Theory 30, 1999, pp. 137-146 |

### [x] Cubic (3-regular)
| item | content |
|------|------|
| OEIS (unlabeled) | A005638: 0, 1, 2, 6, 21, 94, 540, 4207, ... |
| OEIS (connected unlabeled) | A002851: 0, 1, 2, 5, 19, 85, 509, 4060, ... |
| OEIS (labeled) | A004109 is **connected** labeled cubic graphs (on 2n vertices). The implementation and .exp files include disconnected ones (e.g. n=8: 19355 = 19320 connected + 35 for K4∪K4) |
| Enumeration | **snarkhunter** (Brinkmann, Goedgebeur, McKay): dedicated cubic-graph generator using canonical deletion. With girth constraints, over 30× faster than geng |
| Implementation | `include/enumerators/cubic_labeled_enum.h` — labeled exhaustive enumeration (reverse search with the degree-3 constraint) |
| Notes | also used to enumerate snarks (bridgeless cubic graphs that are not 3-edge-colorable) |
| References | Brinkmann, Goedgebeur, McKay, J. Graph Theory 86, 2017; Brinkmann, J. Graph Theory 23(2), 1996 |

### [x] Strongly Regular
| item | content |
|------|------|
| OEIS | A088741 (parameter-dependent; no single sequence) |
| OEIS (labeled) | 0, 0, 0, 6, 12, 50, 0, 280, 5600, ... (n=1,...,9) |
| Counting | enumerated per parameter tuple (n,k,λ,μ); completely classified for small n |
| Enumeration | exhaustive search with fixed parameters + pruning by eigenvalue feasibility tests (McKay, Spence) |
| Implementation | `include/enumerators/strongly_regular_labeled_enum.h` — labeled exhaustive enumeration (parameter-constrained backtracking) |
| Notes | Spence's database has the complete list of feasible parameters up to 64 vertices |
| References | McKay, Spence, Australas. J. Combin. 24, 2001; Brouwer's parameter tables |

---

## Intersection / geometric graph classes (additions)

### [x] Circle
| item | content |
|------|------|
| OEIS (unlabeled) | A156809: 1, 2, 4, 11, 34, 154, 978, 9497, 127954, ... |
| OEIS (labeled) | 1, 2, 8, 64, 1024, 32636, ... (n=1,...,6) |
| Recognition | O(n+m) linear time (Paul, Rutter, STACS 2026); O(n^2) (Spinrad, 1994) |
| Enumeration | canonical deletion (Johnston, 2020): computed up to n=13 (22,576,188,846 graphs) |
| Implementation | `include/enumerators/circle_labeled_enum.h` — labeled exhaustive enumeration (reverse search). `include/enumerators/circle_unlabeled_enum.h` — non-isomorphic enumeration by the canonical construction path / canonical deletion (circle graphs are hereditary, so the search grows one vertex at a time; the pruning is a `check_circle` call — polynomial Naji system — per candidate child, run before the canonicalization of `util/canonical_augmentation.h`); `connected_only` filters at emission (A156808) |
| Notes | intersection graphs of chords of a circle; superclass of permutation. For n≤5 every graph is a circle graph |
| References | Spinrad, Discrete Math. 128, 1994; Paul, Rutter, arXiv:2512.23492, 2025; Johnston, 2020 |

### [x] Halin
| item | content |
|------|------|
| OEIS (unlabeled) | A346779: 0, 0, 0, 1, 1, 2, 2, 4, 6, 13, 22, 50, 106, 252, ... |
| Recognition | O(n+m) linear time: planar + 3-connected + outer-face vertex-count condition |
| Enumeration | constructive: enumerate all planar embeddings of non-isomorphic HI-trees, deduplicating with a planar-tree canonical code |
| Implementation | `include/enumerators/halin_unlabeled_enum.h` — non-isomorphic exhaustive enumeration (HI-tree + planar embedding + bracket-code normalization) |
| Notes | a tree without degree-2 vertices plus a cycle through its leaves; related to cubic polyhedral graphs |
| References | Halin, Combinatorial Mathematics and its Applications, 1971 |

### [x] Apex
| item | content |
|------|------|
| OEIS (unlabeled) | A215620: 1, 2, 4, 11, 34, 155, 1026, 11666, ... |
| OEIS (nonplanar apex) | A215621 |
| OEIS (labeled) | 1, 2, 8, 64, 1024, 32767, ... (n=1,...,6) |
| Recognition | O(n(n+m)): remove each vertex and test planarity |
| Enumeration | reverse search (vertex addition + apex test). Prunable because the class is minor-closed (hereditary) |
| Implementation | `include/enumerators/apex_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | graphs that become planar after removing one vertex; minor-closed. For n ≤ 5 every graph is apex (≤ 4 vertices after removal is always planar). At n=6 only K_6 is non-apex |

---

## Forbidden-subgraph classes (additions)

### [x] Cluster (P3-free / disjoint unions of cliques)
| item | content |
|------|------|
| OEIS (labeled) | A000110 (Bell numbers): 1, 1, 2, 5, 15, 52, 203, 877, 4140, ... |
| OEIS (unlabeled) | A000041 (integer partitions): 1, 1, 2, 3, 5, 7, 11, 15, 22, ... |
| Recognition | O(n+m): check that every connected component is a clique |
| Enumeration | recursive enumeration of set partitions → build the graph with each block as a clique. Constructive, no filter. Non-isomorphic: the same construction over *integer* partitions instead of set partitions, since the multiset of clique sizes is a complete isomorphism invariant |
| Implementation | `include/enumerators/cluster_labeled_enum.h` — labeled exhaustive enumeration (set-partition construction). `include/enumerators/cluster_unlabeled_enum.h` — non-isomorphic enumeration via the integer-partition bijection (one clique per part, parts non-increasing); `connected_only` emits the single connected member K_n |
| Notes | P3-free ⟺ disjoint union of cliques. Subclass of cograph (P4-free) and of threshold |
| References | Knuth, "The Art of Computer Programming" Vol. 4A (set-partition and partition enumeration); OEIS A000110, A000041 |

### [x] Bull-Free
| item | content |
|------|------|
| OEIS (connected unlabeled) | A079575: 1, 1, 2, 6, 20, 93, 480, 2960, 19475, ... |
| OEIS (labeled) | 1, 2, 8, 64, 964, ... (n=1,...,5) |
| Recognition | O(m*Δ²) (triangle enumeration + pendant search); O(n^5) brute force |
| Enumeration | reverse search (vertex addition + bull-free test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/bull_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | the bull is a triangle + 2 pendant edges (5 vertices, 5 edges). For n ≤ 4 every graph is bull-free |
| References | Chudnovsky, "The structure of bull-free graphs I-III," JCTB, 2012 |

### [x] Parity
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 64, 892, 18584, 521096, ... (n=1,...,7) |
| Recognition | direct definition check: verify BFS distances and induced-path parities for all vertex pairs by DFS backtracking. O(n+m) linear time also possible (split decomposition) |
| Enumeration | reverse search: pruned enumeration exploiting that the class is hereditary |
| Implementation | `include/enumerators/parity_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | graphs where any two induced paths between the same pair of vertices have the same parity. For n ≤ 4 every graph is parity (C₅ is the smallest non-parity graph) |
| References | Burlet, Uhry, Annals of Discrete Math., 1984; Bouchet, Combinatorica, 1987 |

### [x] Meyniel
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 64, 952, 23744, ... (n=1,...,6) |
| Recognition | O(n^2) (Lévêque, Lin, Maffray, Trotignon, TCS 2009) |
| Enumeration | reverse search (vertex addition + Meyniel test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/meyniel_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | graphs where every odd cycle of length ≥ 5 has at least 2 chords. For n ≤ 4 every graph is Meyniel |
| References | Burlet, Fonlupt, Annals of Discrete Math., 1984; Lévêque et al., TCS, 2009 |

### [x] Even-Hole-Free
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 61, 834, ... (n=1,...,5) |
| Recognition | O(n^9) (Lai, Lu, Thorup, STOC 2020). History: O(n^40) → O(n^31) → O(n^19) → O(n^11) → O(n^9) |
| Enumeration | reverse search (vertex addition + even-hole-free test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/even_hole_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | the smallest even hole is C4 (4 vertices). For n ≤ 3 every graph is even-hole-free |
| References | Conforti et al., JCTB, 2002; Lai, Lu, Thorup, STOC 2020 |

### [x] Odd-Hole-Free
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 64, 1012, ... (n=1,...,5) |
| Recognition | O(n^9) (Chudnovsky, Scott, Seymour, Spirkl, JACM 2020). Resolved a decades-old open problem |
| Enumeration | reverse search (vertex addition + odd-hole-free test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/odd_hole_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | perfect = odd-hole-free ∩ odd-antihole-free (SPGT). For n ≤ 4 every graph is odd-hole-free (C₅ is the smallest odd hole) |
| References | Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020 |

### [x] 4-Leaf Power
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 61, 822, 17194, ... (n=1,...,6) |
| Recognition | O(n+m) linear time (Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008) |
| Enumeration | chordal reverse search + 4-leaf-power filter (same approach as 3-leaf power) |
| Implementation | `include/enumerators/four_leaf_power_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | subclass of strongly chordal; decided via tree-subdivision feasibility of the critical clique graph. For n≤5 every chordal graph is a 4-leaf power. At n=6 the 4 minimal forbidden induced subgraphs appear |
| References | Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008 |

### [x] 5-Leaf Power
| item | content |
|------|------|
| OEIS | not registered |
| OEIS (labeled) | 1, 2, 8, 61, 822, 18034, ... (n=1,...,6) |
| Recognition | O(n+m) linear time (Chang, Ko, 2007) |
| Enumeration | chordal reverse search + 5-leaf-power filter (CC + tree subdivision, thresholds ≤3/≥4) |
| Implementation | `include/enumerators/five_leaf_power_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | k-leaf powers and (k+1)-leaf powers are incomparable for k≥4 (Fellows et al.). For n≤5 every chordal graph is a 5-leaf power. The gap to 4-leaf power (17194) appears at n=6 |
| References | Chang, Ko, 2007; Lafond, ACM Trans. Algorithms, 2023 (polynomial-time recognition for general k) |

### [x] Gem-Free
| item | content |
|------|------|
| OEIS (labeled) | 1, 2, 8, 64, 964, 25376, ... (n=1,...,6) |
| Recognition | O(n*m*Δ) (neighborhood P4 search); O(n^5) brute force |
| Enumeration | reverse search (vertex addition + gem-free test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/gem_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | the gem (fan F_{1,3}) is a P4 + universal vertex (5 vertices, 7 edges). For n ≤ 4 every graph is gem-free |
| References | Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999 |

### [x] P5-Free
| item | content |
|------|------|
| OEIS (labeled) | 1, 2, 8, 64, 964, 24968, ... (n=1,...,6) |
| Recognition | path-extension search (default); O(n^5) brute force |
| Enumeration | reverse search (vertex addition + P5-free test). Prunable because the class is hereditary |
| Implementation | `include/enumerators/p5_free_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| Notes | P5 is the induced path of length 4 (5 vertices, 4 edges). Next in the Pk-free series after P3-free = cluster and P4-free = cograph. For n ≤ 4 every graph is P5-free |
| References | Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999; Bacsó, Tuza, Dominating cliques in P5-free graphs, Period. Math. Hungar. 21, 1990 |

---

## Special planar classes (additions)

### [x] Polyhedral / 3-Connected Planar
| item | content |
|------|------|
| OEIS (unlabeled) | A000944: 0, 0, 0, 1, 2, 7, 34, 257, 2606, 32300, 440564, 6384634, ... |
| OEIS (labeled) | 0, 0, 0, 1, 25, 1227, 131412, ... (n=1,...,7) |
| Definition | 3-vertex-connected planar graphs. By Steinitz's theorem, exactly the edge skeletons of convex polyhedra |
| Enumeration | **plantri** (Brinkmann, McKay): canonical construction path method, over 5 million graphs per second. Computed up to n ≤ 18 |
| Implementation | `include/enumerators/polyhedral_labeled_enum.h` — labeled exhaustive enumeration (reverse search + planarity pruning + 3-connectivity test) |
| References | Duijvestijn, Federico, "The Number of Polyhedral (3-Connected Planar) Graphs," Math. Comp. 37, 1981; Brinkmann, McKay, MATCH 58, 2007 |
| PDF | `references/brinkmann2007_plantri.pdf` |
| Notes | directly generable with plantri's `-p` option; dual to maximal planar (triangulations). 3-connectivity is not hereditary, so intermediate steps prune by planarity (hereditary) + connectivity + degree, with the 3-connectivity test at the final step |

### [x] Simple Quadrangulation
| item | content |
|------|------|
| OEIS (3-connected, min degree 3) | A007022: 1, 0, 1, 1, 3, 3, 11, 18, 58, ... (n=8, 9, 10, 11, ...; n=8 is the cube, n=11 the Herschel graph. Odd n ≥ 11 are also nonempty) |
| Definition | simple quadrangulations of the sphere (all faces 4-gons); no crossings other than shared edges |
| Enumeration | **plantri** (Brinkmann, McKay): generated from base graphs (the octahedron etc.) by local transformations ({C4}; P0, P1). 270,000 graphs per second. Filters for 3-connected / min degree 3 / no non-facial 4-cycle etc. |
| Implementation | `include/enumerators/simple_quadrangulation_unlabeled_enum.h` — non-isomorphic exhaustive enumeration (dual approach: enumerate 4-regular planar 3-connected graphs → extract faces → build the dual) |
| References | Brinkmann, McKay, "Generation of simple quadrangulations of the sphere," Discrete Math. 305, 2005 |
| PDF | `references/brinkmann2005_quadrangulation.pdf` |
| Notes | duals are 4-regular planar graphs. 3-connected quadrangulations include skeletons of Archimedean solids. Characterization: bipartite + planar + 3-connected + m = 2n-4 |

### [x] Cubic Planar
| item | content |
|------|------|
| OEIS (connected unlabeled) | corresponds to A000109 as duals of triangulations. Directly: plantri's `-b` option |
| OEIS (labeled) | 1, 60, 13475, 5826240, ... (n=4, 6, 8, 10) |
| Definition | planar graphs with all degrees 3 (duals of triangulations) |
| Enumeration | **plantri** (Brinkmann, McKay): generate triangulations and dualize, or generate cubic planar directly. 2-connected / 3-connected variants supported |
| Implementation | `include/enumerators/cubic_planar_labeled_enum.h` — labeled exhaustive enumeration (reverse search, degree-3 + planarity pruning) |
| References | Brinkmann, McKay, MATCH 58, 2007 |
| PDF | `references/brinkmann2007_plantri.pdf` |
| Notes | superclass of fullerenes, snarks, Halin graphs, etc. Non-planar cubic graphs exist from n = 6 (K3,3) (n=6: 60 of 70 are planar; n=8: 13475 of 19355) |

### [x] Fullerene
| item | content |
|------|------|
| OEIS (unlabeled) | A007894: 1, 0, 1, 1, 2, 3, 6, 6, 15, 17, 40, 45, 89, ... (n=20, 22, 24, ...) |
| OEIS (with enantiomers) | A057210 |
| Definition | cubic planar graphs whose faces are all pentagons (exactly 12) or hexagons. The vertex count is always even, ≥ 20 |
| Counting | **exact formula**: Engel, Smillie (Duke Math. J. 2025) derived an exact enumeration formula using modular forms. Asymptotics a(n) ~ c · n^9 |
| Enumeration | **fullgen** (Brinkmann): built from triangulations. **buckygen** (Brinkmann, Goedgebeur, McKay): 3.5× faster than fullgen. Supports the IPR (isolated pentagon rule) filter |
| Implementation | `include/enumerators/fullerene_unlabeled_enum.h` — non-isomorphic enumeration (enumerate the dual triangulations by reverse search and dualize; isomorphism removal via a BFS canonical form) |
| References | Brinkmann, Goedgebeur, McKay, J. Chem. Inf. Model. 52, 2012 (buckygen); Engel, Smillie, Duke Math. J. 174(3), 2025 (exact enumeration) |
| PDF | `references/goedgebeur2013_fullerene_generation.pdf`, `references/engel2023_fullerene_enum.pdf` |
| Notes | C60 (the soccer ball) is the canonical example; important in chemistry and materials science. n=20 is the regular dodecahedron, the unique smallest |

---

## Snarks and Hamiltonicity

### [x] Snark
| item | content |
|------|------|
| OEIS (unlabeled) | A130315: 1, 0, 0, 0, 2, 6, 20, 38, 280, 2900, 28399, 293059, ... (n=10, 12, 14, ...) |
| OEIS (labeled) | 0, ..., 0, 30240, 0, 0, 0, ... (0 for n=1,...,9; 30240 at n=10) |
| Definition | bridgeless cubic graphs with chromatic index 4. The definition requiring cyclic 4-edge-connectivity and girth ≥ 5 is also in use |
| Enumeration | **snarkhunter** (Brinkmann, Goedgebeur): dedicated cubic-graph generator with built-in look-ahead 3-edge-colorability testing; supports girth ≥ k filters (k=4,5,6,7). Snarks with girth ≥ 6 fully enumerated up to 38 vertices, girth ≥ 7 up to 42 |
| Implementation | `include/enumerators/snark_labeled_enum.h` — labeled exhaustive enumeration (reverse search, girth ≥ 5 pruning + bridgeless / cyclically 4-edge-connected / non-3-edge-colorable checks) |
| References | Brinkmann, Goedgebeur, J. Combin. Theory Ser. B 103, 2013 (generation and properties); Brinkmann, Goedgebeur, J. Graph Theory 86, 2017 (large girth); Brinkmann, Goedgebeur, Mattiolo, arXiv:2603.17789, 2026 (new algorithms) |
| PDF | `references/goedgebeur2013_snarks_properties.pdf`, `references/brinkmann2017_cubic_snarks.pdf` |
| Notes | historically important as candidate counterexamples to the four-color theorem. The Petersen graph (10 vertices) is the smallest snark. The vertex count is always even |

### [ ] Hypohamiltonian
| item | content |
|------|------|
| OEIS (unlabeled) | A141150: 1, 0, 0, 1, 0, 1, 4, 0, 14, 34 (n=10, 11, ..., 19) |
| Definition | non-Hamiltonian graphs that become Hamiltonian after removing any single vertex |
| Enumeration | Goedgebeur, Zamfirescu (2017): dedicated generation algorithm enumerating all non-isomorphic hypohamiltonian graphs. Complete lists established up to 17 vertices; 14 graphs at n=18, 34 at n=19 |
| References | Goedgebeur, Zamfirescu, Ars Math. Contemp. 13, 2017 (improved bounds); Goedgebeur, Zamfirescu, Discrete Math. 347, 2024 (K₂-hypohamiltonian) |
| PDF | `references/goedgebeur2017_hypohamiltonian.pdf`, `references/goedgebeur2024_k2_hypohamiltonian.pdf` |
| Notes | the Petersen graph (10 vertices) is the smallest hypohamiltonian graph and the only one on 10 vertices. No hypohamiltonian graphs exist on 11, 12, 14, or 17 vertices. The smallest planar hypohamiltonian graph has at least 23 vertices |

---

## Rigidity theory

### [x] Laman Graph (minimally rigid)
| item | content |
|------|------|
| OEIS (unlabeled) | A227117: 1, 1, 1, 1, 3, 13, 70, 608, 7222, 110132, 2039273, 44176717, ... |
| OEIS (labeled) | 1, 1, 1, 6, 100, 3355, 190491, ... (n=1,...,7) |
| Definition | graphs with n vertices, 2n-3 edges, where every k-vertex subgraph has at most 2k-3 edges ((2,3)-tight graphs). Coincides with 2-dimensional minimally rigid graphs |
| Enumeration | **nauty-laman-plugin** (Larsson): geng plugin generating (2,3)-sparse/tight graphs fast. Constructive enumeration via Henneberg moves (vertex addition + edge splitting) is also possible |
| Implementation | `include/enumerators/laman_labeled_enum.h` — labeled exhaustive enumeration (reverse search + incremental (2,3)-sparsity subset checks) |
| References | Laman, J. Engrg. Math. 4, 1970 (characterization); Larsson, GitHub: nauty-laman-plugin; Capco, Gallet, Grasegger, Koutschan, Lubbes, Schicho, SIAM J. Appl. Algebra Geom., 2018 (realization counts) |
| Notes | bases of the rigidity matroid; describe minimally rigid bar-and-joint frameworks in 2D. Planar (non-crossing) Laman graphs are enumerable by reverse search |

---

## Digraphs and partial orders

### [x] Digraph
| item | content |
|------|------|
| OEIS (unlabeled) | A000273: 1, 1, 3, 16, 218, 9608, 1540944, 882033440, ... |
| OEIS (connected) | A003085 |
| OEIS (labeled) | directed analogue of A000088 (2^(n(n-1)) labeled digraphs; A000273 non-isomorphic) |
| Enumeration | nauty/**directg**: orient the edges of undirected graphs in all ways, suppressing isomorphic digraphs. Combined with geng: `geng n | directg` generates all non-isomorphic digraphs |
| Counting | Burnside's lemma + cycle index (Pólya-style) |
| Implementation | `include/enumerators/digraph_labeled_enum.h` — labeled exhaustive enumeration (constructive DFS, 4-way branch per pair) |
| References | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973; McKay, nauty User's Guide |
| Notes | simple digraphs with no self-loops or multi-edges. Each edge is one-directional (a bidirectional pair counts as 2 directed edges) |

### [x] Tournament
| item | content |
|------|------|
| OEIS (unlabeled) | A000568: 1, 1, 1, 2, 4, 12, 56, 456, 6880, 191536, 9733056, ... |
| OEIS (strong) | A051337 (strongly connected, non-isomorphic) / A054946 (strongly connected, labeled) |
| Definition | digraphs obtained by orienting every edge of a complete graph (every vertex pair comparable) |
| Enumeration | nauty/**gentourng**: dedicated non-isomorphic tournament generator with out-degree constraint options |
| Implementation | `include/enumerators/tournament_labeled_enum.h` — labeled exhaustive enumeration (constructive DFS over all orientations of K_n) |
| Counting | Burnside's lemma + cycle index of the symmetric group |
| References | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973; Moon, "Topics on Tournaments," Holt, Rinehart & Winston, 1968 |
| Notes | in bijection with round-robin results. Non-isomorphic classes extracted from n! labeled tournaments |

### [x] Poset / Partial Order
| item | content |
|------|------|
| OEIS (unlabeled) | A000112: 1, 1, 2, 5, 16, 63, 318, 2045, 16999, 183231, 2567284, ... |
| OEIS (labeled) | A001035: 1, 1, 3, 19, 219, 4231, 130023, ... |
| Definition | finite sets with a reflexive, antisymmetric, transitive binary relation. The Hasse diagram is a DAG |
| Enumeration | nauty/**genposetg** (Brinkmann): non-isomorphic generation of Hasse diagrams (transitively reduced DAGs). Enumerated up to 16 vertices (Brinkmann, McKay, 2002) |
| Implementation | `include/enumerators/poset_labeled_enum.h` — labeled exhaustive enumeration (pairwise DFS + incremental transitive closure) |
| Counting | no closed formula known; computationally hard (conjectured #P-hard) |
| References | Brinkmann, McKay, "Posets on up to 16 Points," Order 19(2), 2002; Heitzig, Reinhold, "Counting Finite Lattices," Algebra Universalis 48, 2002 |
| Notes | equals the number of T₀ topological spaces (A000112). Comparability graphs are the undirected graphs built from posets |

### [ ] Lattice
| item | content |
|------|------|
| OEIS (unlabeled) | A006966: 1, 1, 1, 1, 2, 5, 15, 53, 222, 1078, 5994, 37622, ... |
| Definition | posets in which every two elements have a join and a meet |
| Enumeration | poset enumeration + lattice-condition filter. Dedicated algorithms also exist (Heitzig, Reinhold, 2002) |
| References | Heitzig, Reinhold, Algebra Universalis 48, 2002 |
| Notes | an important subclass of posets; computed up to n=18 |

---

## Counting theory of intersection graphs (additions)

### [ ] x-Monotone Curve Intersection Graph
| item | content |
|------|------|
| Definition | intersection graphs of x-monotone curves (curves meeting every vertical line in at most one point). In the pseudo-segment case any two curves cross at most once |
| Counting | labeled: 2^Ω(n^{4/3}) distinct intersection graphs (lower bound), 2^O(n^{4/3} log² n) (upper bound). Fox, Pach, Suk (GD 2024) |
| References | Fox, Pach, Suk, Proc. GD 2024, LIPIcs; Kynčl, Discrete Comput. Geom. 50, 2013 |
| PDF | `references/fox2024_xmonotone_curves.pdf` |
| Notes | subclass of string graphs (general curve intersection). Uses a new upper bound on VC-dimension; better bounds under chromatic-number constraints |

---

## Graph classes defined by symmetry and transitivity

### [ ] Vertex-Transitive
| item | content |
|------|------|
| OEIS (unlabeled) | A006799: 1, 2, 2, 4, 3, 8, 4, 14, 9, ... |
| Definition | graphs whose automorphism group acts transitively on the vertices — every vertex has the same local environment |
| Enumeration | built from the complete list of transitive groups: enumerate transitive permutation groups of degree n → generate the graphs invariant under each group → remove isomorphs. Holt, Royle (2020) built the complete catalogue up to 47 vertices |
| References | McKay, Royle, "The transitive graphs with at most 26 vertices," Ars Combin. 30, 1990; Holt, Royle, "A census of small transitive groups and vertex-transitive graphs," J. Symbolic Comput. 101, 2020 |
| PDF | `references/holt2020_vertex_transitive_census.pdf` |
| Notes | Cayley graphs (built from group generators) are an important subclass. Vertex-transitive non-Cayley graphs exist (e.g. the Petersen graph). Dataset published on Zenodo in graph6 format |

### [ ] Circulant
| item | content |
|------|------|
| OEIS (unlabeled) | A049287: 1, 2, 2, 4, 3, 8, 4, 12, 8, 20, 8, 48, 14, 48, 44, 84, 36, 192, ... |
| Definition | Cayley graphs over the cyclic group Z_n. For a connection set S ⊂ Z_n, vertices i, j are adjacent ⟺ (j-i) mod n ∈ S |
| Counting | closed formulas for prime order; formulas also known for squarefree and prime-squared orders (Liskovets) |
| Enumeration | filter A285620 candidates with nauty. Classification via Ádám's conjecture (characterization of CI groups) is also possible |
| References | Mishna, "Cayley graph enumeration," M.Sc. Thesis, Simon Fraser Univ., 2003; Liskovets, "Identities for enumerators of circulant graphs," J. Combin. Sci. Syst. 31, 2006 |
| PDF | `references/mishna2003_cayley_graph_enum.pdf` |
| Notes | the simplest subclass of vertex-transitive graphs. Integral circulants (all eigenvalues integral) are also studied |

---

## Special planar classes (additions 2)

### [ ] Planar Triangulation with Minimum Degree 5
| item | content |
|------|------|
| OEIS (unlabeled) | A081621: 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 3, 4, 12, 23, 73, 192, 651, 2070, 7290, ... |
| OEIS (min degree ≥ 4) | A000103: 0, 0, 1, 1, 2, 5, 12, 34, 130, 525, 2472, 12400, 65619, ... |
| Definition | simple triangulations of the sphere (all faces triangles) with minimum degree ≥ 5. Equivalent: duals of girth-5 3-connected planar cubic graphs |
| Enumeration | plantri's `-m5` option: canonical construction path method. Min degree ≥ 4 via `-m4`. All graphs generated by recursively applying local transformations (edge expansion, facet splitting) |
| Implementation | not implemented (relies on plantri) |
| References | Brinkmann, McKay, "Construction of planar triangulations with minimum degree 5," Discrete Math. 301, 2005 |
| PDF | `references/brinkmann2005_min_degree5_triangulation.pdf` |
| Notes | the smallest is the icosahedron at 12 vertices (unique). Includes the duals of fullerenes. Also applied to enumerating 5-regular planar graphs (Hasheminezhad, McKay, Reeves, 2009) |

### [ ] Maximal Outerplanar / Simple 2-Tree
| item | content |
|------|------|
| OEIS (unlabeled) | A000207 (maximal outerplanar graphs on n+2 vertices): from n=3: 1, 1, 1, 3, 4, 12, 27, 82, 228, ... |
| Definition | outerplanar graphs to which no edge can be added without breaking outerplanarity. Equivalent: outerplanar graphs whose interior faces are all triangles. Coincides with simple 2-trees for n ≥ 3 |
| Enumeration | via plantri (2-connected outerplanar); constructed as duals of triangulations. O(1)-per-graph enumeration via recursive (fan-decomposition) construction is also possible |
| References | Bodirsky, Fusy, Kang, Vigerske, EJC 14, 2007 |
| Notes | edge count = 2n-3. Related to caterpillars (pathwidth-1 trees); the maximal version of k-tree (k=2) |

### [ ] Apollonian Network / Planar 3-Tree
| item | content |
|------|------|
| OEIS (rooted labeled) | A001764: 1, 1, 3, 12, 55, 273, 1428, 7752, 43263, 246675, ... |
| Definition | graphs built by recursive triangle subdivision (pick a triangle, add an interior vertex joined to its 3 edges). Equivalent: planar 3-trees, maximal planar chordal graphs, graphs of stacked polytopes, uniquely 4-colorable planar graphs |
| Counting | bijection with ternary trees. Generalized Catalan numbers C(3n, n)/(2n+1) |
| Enumeration | reduces to recursive construction of ternary trees. Also possible via plantri with a maximal planar + chordal filter |
| References | Bodlaender, Kloks, Kratsch, "Treewidth and Pathwidth of Permutation Graphs," SIAM J. Discrete Math. 1995 |
| Notes | the rooted (directed) version is studied as a random network model |

### [ ] Disk Triangulation
| item | content |
|------|------|
| Definition | planar graphs with a distinguished outer face; all faces except the outer one are triangles. The outer face size can be specified |
| Enumeration | plantri's `-d` option generates disk triangulations of a given outer-face size, or all possible sizes if unspecified |
| References | Brinkmann, McKay, "Fast generation of planar graphs," MATCH 58, 2007 (expanded version) |
| PDF | `references/brinkmann2007_plantri_full.pdf` |
| Notes | plantri generates a wide range of planar families (triangulations, quadrangulations, disk triangulations, ...) uniformly; supports parallelization (numbered partial generation) |

---

## Graph classes of bounded width parameters

### [ ] Partial k-Tree / Treewidth ≤ k
| item | content |
|------|------|
| OEIS (partial 2-tree, unlabeled) | a proper superset of 2-trees (A054581) (e.g. C4 is a partial 2-tree but not a 2-tree) |
| Definition | subgraphs of k-trees. Equivalent: graphs of treewidth ≤ k. k=1 gives forests, k=2 series-parallel |
| Enumeration | Dinneen (1997): practical enumeration based on algebraic representations of bounded-width graphs, generated in increasing vertex/edge order with canonical representations for isomorph rejection |
| References | Dinneen, "Practical Enumeration Methods for Graphs of Bounded Pathwidth and Treewidth," CDMTCS-055, 1997; Bodlaender, "A partial k-arboretum of graphs with bounded treewidth," TCS 209, 1998 |
| Notes | treewidth is NP-complete in general but FPT. Forbidden minors fully characterized for k=1,2,3. Enumeration via geng + treewidth filter also possible |

### [ ] k-Degenerate
| item | content |
|------|------|
| Definition | graphs where every induced subgraph has a vertex of degree ≤ k. Equivalent: the vertices can be ordered so each is adjacent to at most k "successors" |
| Counting (labeled) | Bauer, Krug, Wagner (ANALCO 2010): exact counting formula for well-ordered k-degenerate graphs; uniform random generation in polynomial time |
| Enumeration | geng + degeneracy filter. Labeled: constructive enumeration based on well-orderings |
| References | Bauer, Krug, Wagner, "Enumerating and Generating Labeled k-degenerate Graphs," ANALCO 2010; Lick, White, "k-Degenerate Graphs," Canad. J. Math. 22, 1970 |
| Notes | planar graphs are 5-degenerate, outerplanar 2-degenerate, treewidth-k graphs k-degenerate. k-degeneracy is hereditary. Many NP-hard problems are FPT on k-degenerate graphs |

---

## Topological / genus-constrained classes

### [ ] Toroidal (genus 1)
| item | content |
|------|------|
| OEIS (unlabeled) | A319114: 0, 0, 0, 0, 1, 14, 222, 5365, ... |
| OEIS (connected unlabeled) | A319115: 0, 0, 0, 0, 1, 13, 207, 5128, ... |
| Definition | graphs embeddable in the torus (genus-1 surface) but not in the plane |
| Enumeration | geng + genus-test filter. For small n: enumerate all graphs → exclude planar ones → test torus embeddability |
| References | Mohar, Thomassen, "Graphs on Surfaces," Johns Hopkins Univ. Press, 2001 |
| Notes | the smallest toroidal graph is K₅ (5 vertices). K₇ is the largest complete graph embeddable in the torus (Heawood). K_{3,3} is also toroidal |

---

## (k,g)-graphs and cages

### [ ] Cage / (k,g)-Graph
| item | content |
|------|------|
| OEIS (cage vertex count) | A000066 (cubic cages): ..., 10, 14, 24, 30, 58, 70, 112, ... (girth 5, 6, 7, 8, 9, 10, 11, ...) |
| OEIS (cubic girth ≥ 4) | A014371; (girth ≥ 5) A014372; (girth ≥ 6) A014374; (girth ≥ 7) A014375 |
| Definition | a (k,g)-cage is a k-regular graph of girth g with the minimum number of vertices; n(k,g) denotes that number |
| Enumeration | **GENREG** (Meringer, 1999): orderly generation of k-regular graphs with girth constraints; also used to find cages. The 5-regular girth-5 cage generates within an hour |
| References | Meringer, "Fast generation of regular graphs and construction of cages," J. Graph Theory 30, 1999; Exoo, Jajcay, "Dynamic cage survey," Electron. J. Combin. DS16 |
| PDF | `references/meringer1999_genreg_cages.pdf` |
| Notes | Petersen graph = (3,5)-cage, Heawood graph = (3,6)-cage, McGee graph = (3,7)-cage, Tutte 8-cage = (3,8)-cage. n(k,g) is unknown for many (k,g); exhaustive search for record graphs is computationally hard |

---

## Additional subclasses of chordal graphs

### [x] Proper Chordal
| item | content |
|------|------|
| OEIS (labeled) | 1, 2, 8, 61, 822, 18034, 595415, ... (n=1,...,7) |
| Definition | chordal graphs forbidding indifference triples in a tree-layout. proper interval ⊂ proper chordal ⊂ chordal |
| Recognition | O(n⁴) (Paul, Protopapas, STACS 2024): block-tree construction + nested-convexity verification |
| Enumeration | chordal reverse search + proper chordal pruning (hereditary class) |
| Implementation | `include/recognizers/proper_chordal.h` — recognition (Algorithms 1 + 2); `include/enumerators/proper_chordal_labeled_enum.h` — labeled exhaustive enumeration (reverse search) |
| References | Paul, Protopapas, "Tree-Layout Based Graph Classes: Proper Chordal Graphs," LIPIcs vol. 289, STACS 2024 |
| PDF | `references/paul2024_proper_chordal.pdf` |
| Notes | isomorphism testing is polynomial (GI-complete for chordal graphs in general). Coincides with strongly chordal for n ≤ 6; the first gap is at n=7 (strongly chordal: 598775, proper chordal: 595415). k-suns (k≥3) are not proper chordal |

### [ ] Dually Chordal
| item | content |
|------|------|
| Definition | graphs with a maximum neighbourhood ordering. Equivalent: clique graphs of chordal graphs. Strongly chordal = hereditarily dually chordal (every induced subgraph dually chordal) |
| Recognition | O(n+m) linear time (Brandstädt, Dragan, Chepoi, Voloshin, 1998) |
| Enumeration | geng + dually chordal filter. Not hereditary, so reverse search does not apply directly |
| References | Brandstädt, Dragan, Chepoi, Voloshin, "Dually Chordal Graphs," SIAM J. Discrete Math. 11(3), 1998 |
| Notes | unlike chordal, not hereditary (induced subgraphs need not be dually chordal); may contain induced cycles of length ≥ 4 |

---

## Hamiltonicity (additions)

### [ ] K₂-Hypohamiltonian
| item | content |
|------|------|
| Definition | non-Hamiltonian graphs that become Hamiltonian after removing any pair of adjacent vertices |
| Enumeration | Goedgebeur, Zamfirescu (2024): generation algorithm for all non-isomorphic K₂-hypohamiltonian graphs; a construction without edge removal preserving planarity and girth |
| References | Goedgebeur, Zamfirescu, "Generation and new infinite families of K₂-hypohamiltonian graphs," Discrete Math. 347, 2024; Goedgebeur, Renders, Wiener, Zamfirescu, "K₂-Hamiltonian graphs: II," J. Graph Theory 105(4), 2024 |
| PDF | `references/goedgebeur2024_k2_hypohamiltonian.pdf` |
| Notes | the Petersen graph is both K₁- and K₂-hypohamiltonian. The smallest planar K₂-hypohamiltonian graph and the smallest planar example of girth 5 are determined. Existence settled for almost all n except n=14, 17 |

### [ ] Hypotraceable
| item | content |
|------|------|
| Definition | graphs without a Hamiltonian path in which removing any single vertex leaves a graph with one |
| Enumeration | constructions from hypohamiltonian graphs. Wiener, Zamfirescu (2018): infinite families via new constructions. Goedgebeur's software **phog** generates planar hypotraceable graphs |
| References | Wiener, Zamfirescu, "New constructions of hypohamiltonian and hypotraceable graphs," J. Graph Theory 87(4), 2018 |
| Notes | hypohamiltonian does not imply hypotraceable (independent notions). The smallest hypotraceable graph has 34 vertices (Horton, 1973) |

---

## Major software (additions)

| tool | purpose | URL / notes |
|--------|------|------------|
| **fullgen** | fullerene generation | bundled with plantri |
| **buckygen** | fast fullerene generation (3.5× fullgen) | Brinkmann, Goedgebeur, McKay |
| **snarkhunter** | snark / cubic graph generation (girth constraints) | Brinkmann, Goedgebeur |
| **directg** | all orientations of undirected graphs | bundled with nauty |
| **gentourng** | tournament generation | bundled with nauty |
| **genposetg** | poset (Hasse diagram) generation | bundled with nauty (Brinkmann) |
| **nauty-laman-plugin** | Laman (minimally rigid) graph generation | geng plugin (Larsson) |
| **House of Graphs** | graph database / catalogue | https://houseofgraphs.org/ (Coolsaet, D'hondt, Goedgebeur) |
| **phog** | planar hypohamiltonian/hypotraceable digraph generation | https://github.com/nvcleemp/phog |
| **Graphsym** | vertex-transitive graph database (≤ 47 vertices) | https://graphsym.net/ |
| **degen** | labeled k-degenerate graph generation (Bauer's implementation) | https://github.com/dbajic/degen |

---

## Reference PDFs (`references/`)

| file | content |
|-----------|------|
| `mckay1998_isomorph_free_generation.pdf` | McKay, canonical augmentation framework |
| `hebert-johnson2023_counting_chordal.pdf` | polynomial-time counting of chordal graphs |
| `galvin2022_threshold_enum.pdf` | enumeration of threshold and quasi-threshold graphs |
| `chauve2017_distance_hereditary_enum.pdf` | exact counting of distance-hereditary graphs |
| `kawahara2024_bdd_intersection_graphs.pdf` | BDD-based intersection-graph enumeration (proper interval, bipartite permutation, chain, cochain, threshold) |
| `kurita2019_chordal_bipartite_induced_subgraph_enum.pdf` | chordal bipartite induced subgraph enumeration (Kurita, Wasa, Arimura, Uno) |
| `conte2022_proximity_search.pdf` | proximity search framework |
| `brinkmann2007_plantri.pdf` | plantri: fast planar graph generation |
| `bodirsky2007_outerplanar_enum.pdf` | counting and asymptotics of outerplanar graphs |
| `bahrani2017_cactus_enum.pdf` | enumeration and random generation of cactus graphs |
| `brinkmann2005_quadrangulation.pdf` | generation of simple quadrangulations of the sphere |
| `brinkmann2017_cubic_snarks.pdf` | generation of cubic graphs and snarks (large girth) |
| `goedgebeur2013_fullerene_generation.pdf` | fast fullerene generation (buckygen) |
| `goedgebeur2013_snarks_properties.pdf` | generation and properties of snarks |
| `goedgebeur2017_hypohamiltonian.pdf` | generation and bounds for hypohamiltonian graphs |
| `goedgebeur2024_k2_hypohamiltonian.pdf` | generation and infinite families of K₂-hypohamiltonian graphs |
| `engel2023_fullerene_enum.pdf` | exact fullerene counting (modular forms) |
| `fox2024_xmonotone_curves.pdf` | counting x-monotone curve intersection graphs |
| `holt2020_vertex_transitive_census.pdf` | complete catalogue of vertex-transitive graphs ≤ 47 vertices |
| `brinkmann2005_min_degree5_triangulation.pdf` | generation of planar triangulations with minimum degree 5 |
| `brinkmann2007_plantri_full.pdf` | plantri: fast planar graph generation (expanded version) |
| `mishna2003_cayley_graph_enum.pdf` | enumeration of Cayley (circulant) graphs |
| `meringer1999_genreg_cages.pdf` | GENREG: fast generation of regular graphs and construction of cages |
| `paul2024_proper_chordal.pdf` | proper chordal graphs: introduction, recognition, isomorphism |

### Additional references (no PDF on file)

| reference | content |
|------|------|
| Kiyomi, Uno, IEICE Trans. E89-D(2), 2006, DOI:10.1093/ietisy/e89-d.2.763 | dedicated reverse search for labeled chordal subgraphs, O(1) amortized / O(1) delay (differential output) |
| Wright, Richmond, Odlyzko, McKay, SIAM J. Comput. 15(2), 1986 | CAT generation of unlabeled trees |
| Beyer, Hedetniemi, SIAM J. Comput. 9(4), 1980 | CAT generation of rooted trees |
| Otter, Ann. Math. 49(3), 1948 | tree counting formulas |
| Harary, Palmer, Academic Press, 1973 | "Graphical Enumeration" textbook |
| Harary, Schwenk, Discrete Math. 6(4), 1973 | counting caterpillars |
| Beineke, Pippert, J. Combin. Theory 6(2), 1969 | counting labeled k-trees |
| Meringer, J. Graph Theory 30, 1999 | GENREG: fast generation of k-regular graphs |
| Brinkmann, Goedgebeur, McKay, J. Graph Theory 86, 2017 | snarkhunter: cubic graph generation |
| Read, J. London Math. Soc. 38, 1963 | counting self-complementary graphs |
| Colbourn, Read, Int. J. Comput. Math. 7, 1979 | classic orderly generation framework |
| Colbourn, Read, J. Graph Theory 3(2), 1979 | orderly generation for restricted graph classes |
| Mestre, JGAA 13(2), 2009 | recursive generation of connected / 2-edge-connected graphs |
| Johnston, 2020 | canonical-deletion enumeration of permutation / circle graphs |
| Jones, Protti, Del-Vecchio, TCS 713, 2018 | linear-delay non-isomorphic enumeration of cographs |
| Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018 | split-decomposition-based full enumeration of block / ptolemaic / cactus variants |
| Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 | BDD enumeration of proper interval / bipartite permutation / chain / cochain / threshold |
| Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 | O(n^3)-delay non-isomorphic enumeration of distance-hereditary, cograph, ptolemaic, 3-leaf power, etc. |
| Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020 | odd-hole detection in O(n^9) |
| Lai, Lu, Thorup, STOC 2020 | even-hole detection in O(n^9) |
| Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008 | linear-time recognition of 4-leaf powers |
| Lafond, ACM Trans. Algorithms, 2023 | polynomial-time recognition of general k-leaf powers |
| Duijvestijn, Federico, Math. Comp. 37, 1981 | counting polyhedral (3-connected planar) graphs |
| Brinkmann, McKay, Discrete Math. 305, 2005 | generation of simple quadrangulations of the sphere |
| Brinkmann, Goedgebeur, McKay, J. Chem. Inf. Model. 52, 2012 | buckygen: fast fullerene generation |
| Engel, Smillie, Duke Math. J. 174(3), 2025 | exact fullerene counting (modular forms) |
| Brinkmann, Goedgebeur, J. Combin. Theory Ser. B 103, 2013 | generation and properties of snarks |
| Brinkmann, Goedgebeur, J. Graph Theory 86, 2017 | generation of large-girth cubic graphs and snarks |
| Brinkmann, Goedgebeur, Mattiolo, arXiv:2603.17789, 2026 | new snark generation algorithms |
| Goedgebeur, Zamfirescu, Ars Math. Contemp. 13, 2017 | generation and bounds for hypohamiltonian graphs |
| Goedgebeur, Zamfirescu, Discrete Math. 347, 2024 | generation and infinite families of K₂-hypohamiltonian graphs |
| Laman, J. Engrg. Math. 4, 1970 | characterization of 2D minimally rigid graphs |
| Larsson, GitHub: nauty-laman-plugin | Laman graph generation as a geng plugin |
| Brinkmann, McKay, Order 19(2), 2002 | enumeration of posets up to 16 points |
| Heitzig, Reinhold, Algebra Universalis 48, 2002 | counting finite lattices |
| Moon, "Topics on Tournaments," 1968 | classic textbook on tournaments |
| Fox, Pach, Suk, Proc. GD 2024, LIPIcs | counting x-monotone curve intersection graphs |
| Coolsaet, D'hondt, Goedgebeur, Discrete Appl. Math. 319, 2022 | House of Graphs 2.0 database |
| McKay, Royle, Ars Combin. 30, 1990 | complete catalogue of vertex-transitive graphs ≤ 26 vertices |
| Holt, Royle, J. Symbolic Comput. 101, 2020 | complete catalogue of vertex-transitive graphs ≤ 47 vertices |
| Mishna, M.Sc. Thesis, Simon Fraser Univ., 2003 | enumeration of Cayley (circulant) graphs |
| Liskovets, J. Combin. Sci. Syst. 31, 2006 | counting identities for circulant graphs |
| Brinkmann, McKay, Discrete Math. 301, 2005 | generation of planar triangulations with minimum degree 5 |
| Hasheminezhad, McKay, Reeves, COCO 2009 | recursive generation of 5-regular planar graphs |
| Dinneen, CDMTCS-055, 1997 | practical enumeration of bounded pathwidth/treewidth graphs |
| Bodlaender, TCS 209, 1998 | partial k-arboretum of bounded-treewidth graphs |
| Bauer, Krug, Wagner, ANALCO 2010 | counting and random generation of labeled k-degenerate graphs |
| Lick, White, Canad. J. Math. 22, 1970 | foundations of k-degenerate graphs |
| Mohar, Thomassen, Johns Hopkins Univ. Press, 2001 | "Graphs on Surfaces" textbook |
| Exoo, Jajcay, Electron. J. Combin. DS16 | dynamic cage survey |
| Meringer, J. Graph Theory 30, 1999 | GENREG: fast generation of regular graphs and construction of cages |
| Paul, Protopapas, LIPIcs vol. 289, STACS 2024 | proper chordal graphs: introduction, recognition, isomorphism |
| Brandstädt, Dragan, Chepoi, Voloshin, SIAM J. Discrete Math. 11(3), 1998 | foundations and recognition of dually chordal graphs |
| Wiener, Zamfirescu, J. Graph Theory 87(4), 2018 | new constructions of hypohamiltonian / hypotraceable graphs |
| Goedgebeur, Renders, Wiener, Zamfirescu, J. Graph Theory 105(4), 2024 | K₂-Hamiltonian graphs II |

---

## Suggested implementation priority

### Implemented classes
1. ~~**Threshold**~~ - direct construction from binary strings, the simplest
2. ~~**Chain**~~ - closed formula, direct construction
3. ~~**Proper Interval**~~ - reverse search, O(1) per graph
4. ~~**Bipartite Permutation**~~ - reverse search, O(1) per graph
5. ~~**Series-Parallel**~~ - O(1) per graph (Kawano-Nakano)
6. ~~**Cograph**~~ - reduces to cotree enumeration
7. ~~**Interval**~~ - BDD-based or polynomial delay
8. ~~**Permutation**~~ - BDD-based or canonical deletion
9. ~~**Planar**~~ - via plantri
10. ~~**Outerplanar**~~ - via plantri

### Unimplemented classes (with efficient dedicated algorithms)

| priority | class | reason |
|--------|--------|------|
| ~~high~~ | ~~**Tree**~~ | ~~CAT (constant amortized time) algorithm exists (Wright et al. 1986)~~ |
| ~~high~~ | ~~**Caterpillar**~~ | ~~closed counting formula + direct construction from the spine structure~~ |
| ~~high~~ | ~~**k-Tree (2-tree)**~~ | ~~Beineke-Pippert formula + recursive clique extension~~ |
| ~~high~~ | ~~**Maximal Planar**~~ | ~~plantri: over 5 million graphs per second~~ |
| ~~high~~ | ~~**4-Leaf Power**~~ | ~~linear-time recognition, chordal reverse search + filter (same approach as 3-leaf power)~~ |
| ~~medium~~ | ~~**Cubic (3-regular)**~~ | ~~fast generation via snarkhunter~~ |
| ~~medium~~ | ~~**k-Regular**~~ | ~~GENREG handles arbitrary k~~ |
| ~~medium~~ | ~~**Circle**~~ | ~~computed up to n=13 by canonical deletion~~ |
| ~~medium~~ | ~~**Triangle-Free**~~ | ~~efficient pruning via geng -t~~ |
| ~~medium~~ | ~~**Unicyclic**~~ | ~~constructive enumeration of cycle + rooted trees~~ |
| ~~medium~~ | ~~**5-Leaf Power**~~ | ~~linear-time recognition, chordal reverse search + filter~~ |
| ~~low~~ | ~~**Parity**~~ | ~~reverse search (hereditary class + direct definition check)~~ |
| ~~low~~ | ~~**Meyniel**~~ | ~~reverse search (hereditary class + direct definition check)~~ |
| ~~low~~ | ~~**Self-Complementary**~~ | ~~closed formula exists (Read 1963)~~ |
| ~~low~~ | ~~**Cluster (P3-free)**~~ | ~~constructive enumeration of set partitions (Bell numbers)~~ |

### Additionally implemented classes
| priority | class | reason |
|--------|--------|------|
| ~~medium~~ | ~~**P5-Free**~~ | ~~reverse search (hereditary class + path-extension recognition). Natural extension of the Pk-free series~~ |

The filter approach (`geng n | ./recognizer`) applies to every class and is practical up to about n ≤ 12.
