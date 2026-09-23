# graph-recognition

Python bindings for the [Graph Recognition Library](https://github.com/junkawahara/ai-coded-graph-recognition-test) — a C++11 header-only library providing recognition algorithms for 81 graph classes.

[Japanese version (README_ja.md)](README_ja.md)

## Installation

```bash
pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
```

Requires a C++11 compiler (g++ or clang++).

### Optional: NetworkX support

```bash
pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
```

## Quick Start

```python
from graph_recognition import is_interval, is_chordal, is_bipartite

# Using (n, edges) format (1-indexed vertices)
print(is_interval(4, [(1, 2), (2, 3), (3, 4)]))    # True
print(is_interval(4, [(1, 2), (2, 3), (3, 4), (4, 1)]))  # False (C4)

# Algorithm selection
print(is_chordal(4, [(1, 2), (2, 3), (3, 4)], algorithm="mcs_peo"))

# Recognize (returns (bool, NO-certificate dict or None))
from graph_recognition import recognize_interval
result, cert = recognize_interval(4, [(1, 2), (2, 3), (3, 4)])
print(result)  # True
```

### With NetworkX

```python
import networkx as nx
from graph_recognition import is_interval, is_planar

G = nx.path_graph(5)
print(is_interval(G))  # True

# Arbitrary node types are supported
G = nx.Graph()
G.add_edges_from([("a", "b"), ("b", "c")])
print(is_interval(G))  # True
```

## Supported Graph Classes

| Graph Class | Functions |
|---|---|
| Apex | `is_apex`, `recognize_apex` |
| Apollonian network | `is_apollonian`, `recognize_apollonian` |
| AT-free | `is_at_free`, `recognize_at_free` |
| Biconnected | `is_biconnected`, `recognize_biconnected` |
| Biconvex bipartite | `is_biconvex_bipartite`, `recognize_biconvex_bipartite` |
| Bipartite | `is_bipartite`, `recognize_bipartite` |
| Bipartite permutation | `is_bipartite_permutation`, `recognize_bipartite_permutation` |
| Block | `is_block`, `recognize_block` |
| Bull-free | `is_bull_free`, `recognize_bull_free` |
| Cactus | `is_cactus`, `recognize_cactus` |
| Caterpillar | `is_caterpillar`, `recognize_caterpillar` |
| Chain | `is_chain`, `recognize_chain` |
| Chordal | `is_chordal`, `recognize_chordal` |
| Chordal bipartite | `is_chordal_bipartite`, `recognize_chordal_bipartite` |
| Circle | `is_circle`, `recognize_circle` |
| Circular-arc | `is_circular_arc`, `recognize_circular_arc` |
| Claw-free | `is_claw_free`, `recognize_claw_free` |
| Cluster | `is_cluster`, `recognize_cluster` |
| Co-chordal | `is_co_chordal`, `recognize_co_chordal` |
| Co-comparability | `is_co_comparability`, `recognize_co_comparability` |
| Co-interval | `is_co_interval`, `recognize_co_interval` |
| Co-chain | `is_cochain`, `recognize_cochain` |
| Cograph | `is_cograph`, `recognize_cograph` |
| Comparability | `is_comparability`, `recognize_comparability` |
| Convex bipartite | `is_convex_bipartite`, `recognize_convex_bipartite` |
| Cubic | `is_cubic`, `recognize_cubic` |
| Cubic planar | `is_cubic_planar`, `recognize_cubic_planar` |
| Diamond-free | `is_diamond_free`, `recognize_diamond_free` |
| Digraph (directed input) | `is_digraph`, `recognize_digraph` |
| Distance-hereditary | `is_distance_hereditary`, `recognize_distance_hereditary` |
| Eulerian | `is_eulerian`, `recognize_eulerian` |
| Even-hole-free | `is_even_hole_free`, `recognize_even_hole_free` |
| Five-leaf power | `is_five_leaf_power`, `recognize_five_leaf_power` |
| Forest | `is_forest`, `recognize_forest` |
| Four-leaf power | `is_four_leaf_power`, `recognize_four_leaf_power` |
| Fullerene | `is_fullerene`, `recognize_fullerene` |
| Gem-free | `is_gem_free`, `recognize_gem_free` |
| Halin | `is_halin`, `recognize_halin` |
| Interval | `is_interval`, `recognize_interval` |
| k-regular | `is_kregular`, `recognize_kregular` |
| k-tree | `is_ktree`, `recognize_ktree` |
| Laman | `is_laman`, `recognize_laman` |
| Line graph | `is_line_graph`, `recognize_line_graph` |
| Maximal outerplanar | `is_maximal_outer_planar`, `recognize_maximal_outer_planar` |
| Maximal planar | `is_maximal_planar`, `recognize_maximal_planar` |
| Meyniel | `is_meyniel`, `recognize_meyniel` |
| Odd-hole-free | `is_odd_hole_free`, `recognize_odd_hole_free` |
| Outerplanar | `is_outer_planar`, `recognize_outer_planar` |
| P5-free | `is_p5_free`, `recognize_p5_free` |
| Parity | `is_parity`, `recognize_parity` |
| Perfect | `is_perfect`, `recognize_perfect` |
| Permutation | `is_permutation`, `recognize_permutation` |
| Planar | `is_planar`, `recognize_planar` |
| Polyhedral | `is_polyhedral`, `recognize_polyhedral` |
| Poset Hasse diagram (directed input) | `is_poset`, `recognize_poset` |
| Proper chordal | `is_proper_chordal`, `recognize_proper_chordal` |
| Proper circular-arc | `is_proper_circular_arc`, `recognize_proper_circular_arc` |
| Proper interval | `is_proper_interval`, `recognize_proper_interval` |
| Ptolemaic | `is_ptolemaic`, `recognize_ptolemaic` |
| Quasi-threshold | `is_quasi_threshold`, `recognize_quasi_threshold` |
| Self-complementary | `is_self_complementary`, `recognize_self_complementary` |
| Series-parallel | `is_series_parallel`, `recognize_series_parallel` |
| Simple quadrangulation | `is_simple_quadrangulation`, `recognize_simple_quadrangulation` |
| Snark | `is_snark`, `recognize_snark` |
| Split | `is_split`, `recognize_split` |
| Strongly chordal | `is_strongly_chordal`, `recognize_strongly_chordal` |
| Strongly regular | `is_strongly_regular`, `recognize_strongly_regular` |
| Three-leaf power | `is_three_leaf_power`, `recognize_three_leaf_power` |
| Threshold | `is_threshold`, `recognize_threshold` |
| Tournament (directed input) | `is_tournament`, `recognize_tournament` |
| Trapezoid | `is_trapezoid`, `recognize_trapezoid` |
| Tree | `is_tree`, `recognize_tree` |
| Triangle-free | `is_triangle_free`, `recognize_triangle_free` |
| Triconnected | `is_triconnected`, `recognize_triconnected` |
| Trivially perfect | `is_trivially_perfect`, `recognize_trivially_perfect` |
| Unicyclic | `is_unicyclic`, `recognize_unicyclic` |
| Unit interval | `is_unit_interval`, `recognize_unit_interval` |
| Weakly chordal | `is_weakly_chordal`, `recognize_weakly_chordal` |

78 of the C++ library's 81 recognizers are bound (all but
`partial_ktree`, `degenerate` and `cage`, whose extra parameters do
not fit the wrapper's uniform signature). Note that `is_digraph`,
`is_poset`, and `is_tournament` interpret the edge list as *directed* arcs
(for `is_poset` the arcs are the covering relation: `(u, v)` means `v`
covers `u`).

### Enumeration Functions

`enumerate_<type>_labeled_graphs(n)` generates all labeled graphs of the
given class on `n` vertices. `n` is capped at 6 (`ENUM_MAX_N`); larger
values raise `ValueError`, because the full result is materialized in
memory. Only a few C++ enumerators stream instead: the chordal, split,
strongly chordal and proper chordal labeled enumerators, the threshold unlabeled
enumerator and the chordal subgraph / chordal bipartite induced subgraph
enumerators have `_cb` callback APIs that their CLIs use, so those CLIs handle
larger `n`; the interval and weakly chordal labeled enumerators have a `_cb`
API too, while every other C++ CLI materializes the result just as this
wrapper does:

`enumerate_at_free_labeled_graphs`,
`enumerate_biconvex_bipartite_labeled_graphs`,
`enumerate_bipartite_labeled_graphs`,
`enumerate_bipartite_permutation_labeled_graphs`,
`enumerate_block_labeled_graphs`, `enumerate_cactus_labeled_graphs`,
`enumerate_chordal_bipartite_labeled_graphs`,
`enumerate_chordal_labeled_graphs`, `enumerate_circular_arc_labeled_graphs`,
`enumerate_claw_free_labeled_graphs`, `enumerate_co_chordal_labeled_graphs`,
`enumerate_co_comparability_labeled_graphs`,
`enumerate_co_interval_labeled_graphs`, `enumerate_cograph_labeled_graphs`,
`enumerate_comparability_labeled_graphs`,
`enumerate_convex_bipartite_labeled_graphs`,
`enumerate_diamond_free_labeled_graphs`,
`enumerate_distance_hereditary_labeled_graphs`,
`enumerate_interval_labeled_graphs`, `enumerate_line_graph_labeled_graphs`,
`enumerate_outer_planar_labeled_graphs`, `enumerate_perfect_labeled_graphs`,
`enumerate_permutation_labeled_graphs`, `enumerate_planar_labeled_graphs`,
`enumerate_proper_interval_labeled_graphs`,
`enumerate_ptolemaic_labeled_graphs`,
`enumerate_series_parallel_labeled_graphs`, `enumerate_split_labeled_graphs`,
`enumerate_strongly_chordal_labeled_graphs`,
`enumerate_three_leaf_power_labeled_graphs`,
`enumerate_trapezoid_labeled_graphs`,
`enumerate_trivially_perfect_labeled_graphs`,
`enumerate_weakly_chordal_labeled_graphs`

`enumerate_co_chordal_labeled_graphs(n, algorithm="kiyomi_uno")` uses the
default complement of the dedicated chordal enumeration. Select
`algorithm="reverse_search"` to use the original vertex-addition search
with co-chordal recognition. Both return the same graphs in different orders.

`enumerate_co_interval_labeled_graphs(n, algorithm="kiyomi_kijima_uno")` uses
the default complement of the dedicated interval enumeration. Select
`algorithm="reverse_search"` to use the original vertex-addition search
with co-interval recognition. Both return the same graphs in different orders.

`enumerate_proper_interval_labeled_graphs(n, algorithm="string_representation")`
uses the default bracket-string generator with expansion into all distinct
labelings, including disconnected graphs. Select `algorithm="reverse_search"`
to use the original chordal-tree search with proper-interval recognition.
Both return the same graphs in different orders. The implementation uses
Dyck-word DFS and visited edge sets for label expansion; it does not have
the paper's O(1) time per unlabeled representation.

`enumerate_<type>_unlabeled_graphs(n)` emits one representative per
isomorphism class instead of every labeling. All of them except
`apollonian`, `biconnected`, `chain`, `cochain`, `maximal_outer_planar`, `maximal_planar`, `polyhedral`,
`self_complementary`, `threshold` and the complement-based enumerators
(`co_chordal`, `co_comparability`, `co_interval`)
also take `connected_only=False`, which keeps only the connected
representatives:

`enumerate_apollonian_unlabeled_graphs`,
`enumerate_biconnected_unlabeled_graphs`,
`enumerate_bipartite_permutation_unlabeled_graphs`,
`enumerate_bipartite_unlabeled_graphs`,
`enumerate_cactus_unlabeled_graphs`, `enumerate_chain_unlabeled_graphs`,
`enumerate_chordal_unlabeled_graphs`, `enumerate_circle_unlabeled_graphs`,
`enumerate_cluster_unlabeled_graphs`,
`enumerate_co_chordal_unlabeled_graphs`,
`enumerate_co_comparability_unlabeled_graphs`,
`enumerate_co_interval_unlabeled_graphs`,
`enumerate_cochain_unlabeled_graphs`,
`enumerate_cograph_unlabeled_graphs`,
`enumerate_comparability_unlabeled_graphs`,
`enumerate_cubic_planar_unlabeled_graphs`,
`enumerate_cubic_unlabeled_graphs`,
`enumerate_distance_hereditary_unlabeled_graphs`,
`enumerate_eulerian_unlabeled_graphs`,
`enumerate_interval_unlabeled_graphs`,
`enumerate_maximal_outer_planar_unlabeled_graphs`,
`enumerate_maximal_planar_unlabeled_graphs`,
`enumerate_outer_planar_unlabeled_graphs`,
`enumerate_permutation_unlabeled_graphs`, `enumerate_planar_unlabeled_graphs`,
`enumerate_polyhedral_unlabeled_graphs`,
`enumerate_proper_interval_unlabeled_graphs`,
`enumerate_ptolemaic_unlabeled_graphs`,
`enumerate_self_complementary_unlabeled_graphs`,
`enumerate_series_parallel_unlabeled_graphs`,
`enumerate_split_unlabeled_graphs`,
`enumerate_three_leaf_power_unlabeled_graphs`,
`enumerate_threshold_unlabeled_graphs`,
`enumerate_triangle_free_unlabeled_graphs`,
`enumerate_trivially_perfect_unlabeled_graphs`

### Subgraph Enumeration Functions

`enumerate_<type>_subgraphs(n_or_graph, edges=None)` takes a *host graph*
instead of a vertex count and generates the subgraphs of it belonging to the
class. The input is the same `(n, edges)` pair or `networkx.Graph` the
recognizers accept:

`enumerate_chordal_subgraphs`

A subgraph here is a spanning subgraph `(V, E')` with `E'` a subset of the
host's edges: the vertex set is fixed and isolated vertices are kept, so the
results are in bijection with the class's edge subsets of the host and the
empty edge set is always among them.

The cap is on the host's edge count, not its vertex count: every subgraph of a
forest is chordal, so a host with `m` edges can have up to `2^m` chordal
subgraphs. Hosts with more than 16 distinct edges (`ENUM_MAX_M`) raise
`ValueError`.

```python
from graph_recognition import enumerate_chordal_subgraphs

# C4: every edge subset but C4 itself is chordal.
len(enumerate_chordal_subgraphs(4, [(1, 2), (2, 3), (3, 4), (4, 1)]))
# 15
```

### Induced Subgraph Enumeration Functions

`enumerate_<type>_induced_subgraphs(n_or_graph, edges=None)` also takes a
**host graph**, but its solutions are the vertex subsets `X` for which `G[X]`
belongs to the class, so it returns lists of vertex lists rather than edge
lists:

`enumerate_chordal_bipartite_induced_subgraphs`

Each vertex list is sorted ascending, and the empty set always comes first.

The cap is on the host's vertex count, not its edge count: these classes are
hereditary, so a host that is itself in the class has all `2^n` vertex subsets
as solutions no matter how sparse it is. Hosts with more than 16 vertices
(`INDUCED_ENUM_MAX_N`) raise `ValueError`.

```python
from graph_recognition import enumerate_chordal_bipartite_induced_subgraphs

# C6: every vertex subset but the whole cycle is chordal bipartite.
len(enumerate_chordal_bipartite_induced_subgraphs(
    6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)]))
# 63
```

## API

### `is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool`

Check if a graph belongs to the specified class.

- `n_or_graph`: Number of vertices (int) or a `networkx.Graph`
- `edges`: List of `(u, v)` tuples (1-indexed). Required when `n_or_graph` is an int.
  Any iterable of length-2 sequences works (a list of lists, a NumPy array of shape
  `(m, 2)`, ...); unordered pairs (`set` / `frozenset`) are rejected with `TypeError`
  because the endpoint order is the arc direction for the directed recognizers.
  `n` and the endpoints may be any `numbers.Integral` (Python `int`, NumPy integer
  scalars); `bool` is rejected.
- `algorithm`: Optional algorithm name (str). See docstrings for available options.

Two recognizers may give up on a hard instance by raising `RuntimeError` instead of returning a bool (the answer is then unknown): `is_circle` / `recognize_circle` when `algorithm="dow_backtracking"` exhausts its step budget (about 2e7 steps; practical up to roughly 9 vertices) or the default `naji_system` exceeds its 1 GiB basis memory cap, and `is_five_leaf_power` / `recognize_five_leaf_power` when the realizability search exhausts its budget (about 5e7 steps, roughly ten seconds; some sparse strongly chordal inputs of a few dozen vertices reach it).

Five more raise `RuntimeError` on inputs that are too large for their recursive search, which would otherwise overflow the default stack and crash the interpreter (the limits sit at about one third of the measured crash point): `is_line_graph` / `recognize_line_graph` with 6000 or more edges, `is_circular_arc` / `recognize_circular_arc` when a connected input has 7000 or more maximal cliques (`mcconnell`) or 7000 or more vertices (`backtracking`, also used as the non-Helly fallback), `is_proper_circular_arc` / `recognize_proper_circular_arc` when a claw-free input has 7000 or more vertices (it runs the same endpoint search after its claw-free filter), `is_meyniel` / `recognize_meyniel` with 17000 or more vertices, and `is_even_hole_free` / `recognize_even_hole_free` with 32000 or more vertices. Each function's docstring lists its condition.

### `recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)`

Same as `is_<type>` but returns a tuple `(bool, dict | None)`. On a `True`
answer the second element is always `None`. On a `False` answer it is a **NO
certificate** for the 38 classes listed by `graph_recognition._core._certified_types()`
(at_free, biconnected, bipartite, block, bull_free, cactus, chain, chordal,
chordal_bipartite, claw_free, cluster, co_chordal, co_comparability, co_interval,
cochain, cograph, comparability, diamond_free, distance_hereditary, even_hole_free,
gem_free, interval, meyniel, odd_hole_free, p5_free, parity, perfect, permutation,
planar, proper_interval, ptolemaic, quasi_threshold, split, threshold, triangle_free,
trivially_perfect, unit_interval, weakly_chordal), and `None` for every other class
(each function's docstring says which). The certificate is a dict with four keys:

- `kind`: the name of the structure that rules the graph out (`'hole'`, `'claw'`,
  `'odd_cycle'`, `'asteroidal_triple'`, ...);
- `in_complement`: `True` when the pattern is induced in the complement (the
  `co_*` classes);
- `vertices`: the 1-indexed vertices of the pattern, ordered as the kind
  prescribes (a hole or odd cycle in cyclic order, for example);
- `vertex_sets`: branch sets, cycles or paths for the few kinds that need
  them; an empty list for most kinds.

```python
recognize_chordal(4, [(1, 2), (2, 3), (3, 4), (4, 1)])
# (False, {'kind': 'hole', 'in_complement': False, 'vertices': [2, 3, 4, 1], 'vertex_sets': []})
recognize_bipartite(3, [(1, 2), (2, 3), (1, 3)])
# (False, {'kind': 'odd_cycle', 'in_complement': False, 'vertices': [2, 1, 3], 'vertex_sets': []})
recognize_tree(3, [(1, 2), (2, 3), (1, 3)])
# (False, None) -- no certificate for this class
```

The certificate does not depend on `algorithm`. Some classes need more work to
produce the witness than to decide membership, so a `False` answer can cost
noticeably more than `is_<type>`. With a `networkx.Graph` input the vertices in
the certificate are the 1-indexed positions in the iteration order of
`G.nodes()` (the numbering `from_networkx` uses), not the original node labels.

### Decompositions

Where `is_<type>` answers whether a graph belongs to a class, these return the
structure behind the answer. Every result is plain Python data -- dicts, lists
and tuples of ints -- and vertex-indexed lists keep the 1-indexed convention,
so they have `n + 1` entries with index 0 unused.

```python
from graph_recognition import modular_decomposition, split_decomposition, spqr_tree

# P4 has no non-trivial module, so its decomposition tree is a single prime node
t = modular_decomposition(4, [(1, 2), (2, 3), (3, 4)])
t["nodes"][t["root"]]["kind"]                      # 'prime'

# Distance-hereditary graphs are exactly those with no prime bag
split_decomposition(4, [(1, 2), (2, 3), (3, 4)])["totally_decomposable"]   # True

# A cycle is a single polygon
[node["kind"] for node in spqr_tree(4, [(1, 2), (2, 3), (3, 4), (4, 1)])["nodes"]]  # ['S']
```

| Function | Returns |
| --- | --- |
| `connected_components` / `co_components` | vertex lists per component |
| `twin_quotient(n, edges, kind='both')` | twin classes and the quotient graph |
| `block_cut_tree` | biconnected components, cut vertices, bridges |
| `modular_decomposition` | the modular decomposition tree |
| `cotree` | the cotree of a cograph (a tree with no prime node) |
| `transitive_orientation` | a transitive orientation of a comparability graph |
| `permutation_realizer` | a permutation diagram |
| `clique_tree` | maximal cliques and a clique tree of a chordal graph |
| `tree_decomposition` | bags, tree and treewidth of a chordal graph |
| `split_decomposition` | Cunningham's canonical split decomposition |
| `spqr_tree` | Tutte's 3-connected components of a biconnected graph |
| `planar_embedding` | a rotation system and its faces |
| `strong_elimination_ordering` | a strong elimination ordering |
| `indifference_tree_layout` | an indifference tree-layout |
| `consecutive_ones(num_columns, rows)` | consecutive ones test of a 0/1 matrix |

## Building

The C++ extension compiles against the library headers: from a repository
checkout it uses `../include`, and an sdist created with
`python -m build --sdist` bundles a copy of the headers, so standalone
sdists build as well.

## License

MIT
