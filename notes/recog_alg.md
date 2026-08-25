# Recognition algorithms for enumeration-only classes

For the graph classes that used to ship only an enumerator, this file surveys
the existence, complexity, and references of recognition algorithms.
`[ ]` means not implemented, `[x]` means implemented.

---

## Trivial recognition (decidable directly from the definition)

### [x] Tree
| item | content |
|------|------|
| Complexity | O(n + m) |
| Algorithm | connectivity check (BFS/DFS) + verify m = n - 1 |
| Implementation | `include/tree.h` — `check_tree()` |
| Notes | equivalent conditions: connected and acyclic; connected with m = n - 1; exactly one path between any two vertices |

### [x] Forest
| item | content |
|------|------|
| Complexity | O(n + m) |
| Algorithm | verify there is no cycle (DFS), or verify m = n - (number of connected components) |
| Implementation | `include/forest.h` — `check_forest()` |
| Notes | equivalent condition: contains no cycle (every connected component is a tree) |

### [x] Unicyclic
| item | content |
|------|------|
| Complexity | O(n + m) |
| Algorithm | connectivity check + verify m = n |
| Implementation | `include/unicyclic.h` — `check_unicyclic()` |
| Notes | a connected graph has exactly one cycle ⟺ its edge count equals its vertex count |

### [x] k-regular
| item | content |
|------|------|
| Complexity | O(n + m) |
| Algorithm | verify all vertices have the same degree |
| Implementation | `include/kregular.h` — `check_kregular()` |
| Notes | outputs the degree k when regular. Necessary condition: nk is even |

### [x] Cubic
| item | content |
|------|------|
| Complexity | O(n + m) |
| Algorithm | verify every vertex has degree 3 |
| Implementation | `include/cubic.h` — `check_cubic()` |
| Notes | regular with k = 3. Necessary condition: n is even |

### [x] Tournament
| item | content |
|------|------|
| Complexity | O(n²) |
| Algorithm | in the directed graph, verify that every vertex pair (u, v) has exactly one directed edge |
| Implementation | `include/tournament.h` — `check_tournament()` |
| Notes | an orientation of the complete graph Kn. Necessary condition: m = n(n-1)/2 |

### [x] Directed graph
| item | content |
|------|------|
| Complexity | — |
| Algorithm | trivial as a recognition problem (any set of directed edges is a directed graph) |
| Implementation | `include/digraph.h` — `check_digraph()` |
| Notes | enumeration generates all non-isomorphic digraphs; recognition only validates the input format |

---

## Easy recognition (combinations of existing properties)

### [x] Caterpillar
| item | content |
|------|------|
| Complexity | O(n) |
| Algorithm | (1) verify the graph is a tree, (2) remove all leaves (degree-1 vertices), (3) verify the remainder is a path (all degrees ≤ 2) or empty |
| Implementation | `include/caterpillar.h` — `check_caterpillar()` |
| References | Harary, Schwenk, "The number of caterpillars," Discrete Mathematics 6, 1973 |

### [x] Maximal planar
| item | content |
|------|------|
| Complexity | O(n + m) (planarity via left-right planarity, planarity_lr.h) |
| Algorithm | (1) planarity check (left-right criterion), (2) verify m = 3n - 6 (n ≥ 3) |
| Implementation | `include/maximal_planar.h` — `check_maximal_planar()` |
| Notes | planar graphs whose faces are all triangles; n ≤ 2 are trivial cases |

### [x] Cubic planar
| item | content |
|------|------|
| Complexity | O(n + m) (planarity via left-right planarity, planarity_lr.h) |
| Algorithm | (1) verify every vertex has degree 3, (2) planarity check (left-right criterion) |
| Implementation | `include/cubic_planar.h` — `check_cubic_planar()` |
| Notes | intersection of cubic and planar. Necessary condition: n is even |

### [x] Polyhedral
| item | content |
|------|------|
| Complexity | O(n + m) + cost of 3-connectivity (planarity via left-right planarity, planarity_lr.h) |
| Algorithm | (1) planarity check (left-right criterion), (2) 3-connectivity check |
| Implementation | `include/polyhedral.h` — `check_polyhedral()` |
| References | Steinitz's theorem: polyhedral ⟺ 3-connected planar |
| Notes | 3-connectivity is decidable in O(n + m) via Hopcroft-Tarjan SPQR-tree construction |

### [x] Simple quadrangulation
| item | content |
|------|------|
| Complexity | O(n + m) + cost of 3-connectivity (planarity via left-right planarity, planarity_lr.h) |
| Algorithm | (1) verify m = 2n - 4, (2) verify triangle-freeness, (3) 3-connectivity check, (4) planarity check (left-right criterion) |
| Implementation | `include/simple_quadrangulation.h` — `check_simple_quadrangulation()` |
| Notes | 3-connected planar + m = 2n-4 + triangle-free ⟺ all faces are quadrilaterals. By Euler's formula the average face size is 4, and with no triangles every face has size 4 |

---

## Nontrivial polynomial-time recognition

### [x] k-tree
| item | content |
|------|------|
| Complexity | O(n^2 + nk^2) (the implementation builds an adjacency matrix and scans all vertices per removal) |
| Algorithm | **peeling**: repeatedly remove a vertex whose degree is exactly k and whose neighborhood is a clique; the graph is a k-tree iff this reduces it to a single (k+1)-clique |
| Alternative | PEO-based: compute a perfect elimination ordering by MCS → verify chordality → verify all maximal cliques have size k+1 and all minimal separators have size k |
| References | Patil, "The structure of k-trees," 1986; Beineke, Pippert, "The number of labeled k-dimensional trees," J. Combin. Theory 6, 1969; Rose, "On simple characterizations of k-trees," Discrete Mathematics 7, 1974 |
| Notes | necessary condition: m = kn - k(k+1)/2. k = 1 gives trees, k = 2 gives 2-trees |

### [x] Halin
| item | content |
|------|------|
| Complexity | polynomial (planarity via left-right criterion O(n+m); embedding via Tutte coordinates) |
| Algorithm | (1) verify 3-connectivity and planarity, (2) for each face of the planar embedding, check whether removing that face's edges leaves a tree with no degree-2 vertices whose leaves coincide with the vertices of that face cycle (the outer cycle passes only through the leaves of the tree) |
| References | Cornuéjols, Naddef, Pulleyblank, 1983 (structural characterization); Fomin, Golovach, Thilikos, 2009 |
| Notes | necessary conditions: 3-connected, planar, minimum degree 3 |

### [x] Fullerene
| item | content |
|------|------|
| Complexity | polynomial (planarity via left-right criterion O(n+m); deciding C20 takes 3 ms measured) |
| Algorithm | (1) verify cubicity, (2) planarity check + computation of a combinatorial embedding (Tutte coordinates), (3) verify every face is a pentagon or a hexagon |
| References | Brinkmann, Dress, 1997 (fullerene generation); de Fraysseix, Ossona de Mendez, Rosenstiehl, 2006 (left-right planarity); Brandes, 2009 (formulation used by the implementation) |
| Notes | by Euler's formula there are exactly 12 pentagons and n/2 - 10 hexagons. Necessary conditions: n ≥ 20, n even (except that n = 22 is impossible) |

### [x] Strongly regular
| item | content |
|------|------|
| Complexity | O(n³) (matrix product), O(n^ω) (fast matrix multiplication) |
| Algorithm | (1) verify regularity (obtain degree k), (2) compute A² for the adjacency matrix A, (3) check A² = (λ - μ)A + (k - μ)I + μJ, where λ is the number of common neighbors of adjacent pairs and μ that of non-adjacent pairs |
| References | testing parameters (n, k, λ, μ) is standard algebraic graph theory |
| Notes | polynomial even with unknown parameters: read λ, μ off any edge/non-edge and check they are constant over all pairs |

### [x] Laman
| item | content |
|------|------|
| Complexity | O(n²) |
| Algorithm | **pebble game**: place 2 pebbles per vertex and insert edges one by one; at each insertion, check via BFS/DFS whether enough pebbles can be gathered at the endpoints. Laman condition: m = 2n - 3 and \|E(S)\| ≤ 2\|S\| - 3 for every subset S |
| References | Lee, Streinu, "Pebble Game Algorithms and Sparse Graphs," Discrete Mathematics 308(8), 2008; Laman, "On graphs and rigidity of plane skeletal structures," J. Engineering Mathematics 4, 1970 |
| Notes | minimally rigid graphs in 2D. The pebble game avoids enumerating all subsets |

### [x] Poset / Hasse diagram
| item | content |
|------|------|
| Complexity | O(nm) |
| Algorithm | (1) verify the digraph is a DAG (topological sort, O(n + m)), (2) verify it is transitively reduced: for each edge (u, v), verify there is no path of length ≥ 2 from u to v |
| Notes | a Hasse diagram is the transitive reduction of a partial order. Verifying transitive reduction needs a reachability check per edge |

---

## Hard problems (no polynomial-time algorithm known)

### [x] Snark
| item | content |
|------|------|
| Complexity | **NP-hard** (determining the chromatic index is the bottleneck) |
| Algorithm | cubicity and cyclic 4-edge-connectivity are polynomial; but deciding the chromatic index (3 vs 4) is NP-complete |
| References | Holyer, "The NP-completeness of edge-coloring," SIAM J. Comput. 10(4), 1981 (NP-complete even for cubic graphs); Isaacs, "Infinite families of nontrivial trivalent graphs which are not Tait colorable," Amer. Math. Monthly 82, 1975 |
| Notes | by Vizing's theorem a cubic graph has chromatic index 3 or 4. A 3-edge-coloring certificate is polynomially verifiable (NP), but deciding chromatic index 4 is co-NP-hard. In practice, use a SAT solver or similar |

### [x] Self-complementary
| item | content |
|------|------|
| Complexity | **GI-complete** (equivalent to graph isomorphism; quasi-polynomial exp(O((log n)^c))) |
| Algorithm | isomorphism test between G and its complement G̅. Necessary preconditions: n ≡ 0 or 1 (mod 4), m = n(n-1)/4 |
| References | Colbourn, Colbourn, "Graph Isomorphism and Self-Complementary Graphs," SIGACT News, 1978 (GI-completeness); Babai, "Graph Isomorphism in Quasipolynomial Time," STOC 2016 |
| Practical tools | isomorphism via nauty/Traces (McKay, Piperno, 2014) or Bliss |
| Notes | the existence of a polynomial-time algorithm is open |
