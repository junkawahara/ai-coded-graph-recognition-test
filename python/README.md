# graph-recognition

Python bindings for the [Graph Recognition Library](https://github.com/junkawahara/ai-coded-graph-recognition-test) — a C++11 header-only library providing recognition algorithms for 76 graph classes.

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

# Recognize (returns tuple with certificate placeholder)
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

All 76 recognizers of the C++ library are bound. Note that `is_digraph`,
`is_poset`, and `is_tournament` interpret the edge list as *directed* arcs
(for `is_poset` the arcs are the covering relation: `(u, v)` means `v`
covers `u`).

### Enumeration Functions

`enumerate_<type>_graphs(n)` generates all labeled graphs of the given
class on `n` vertices (`chain`, `cochain`, and `threshold` enumerate up
to isomorphism instead):

`enumerate_at_free_graphs`, `enumerate_biconvex_bipartite_graphs`,
`enumerate_bipartite_graphs`,
`enumerate_bipartite_permutation_graphs`, `enumerate_block_graphs`,
`enumerate_cactus_graphs`, `enumerate_chain_graphs`, `enumerate_chordal_graphs`,
`enumerate_chordal_bipartite_graphs`, `enumerate_circular_arc_graphs`,
`enumerate_claw_free_graphs`, `enumerate_co_chordal_graphs`,
`enumerate_co_comparability_graphs`, `enumerate_co_interval_graphs`,
`enumerate_cochain_graphs`,
`enumerate_cograph_graphs`, `enumerate_comparability_graphs`,
`enumerate_convex_bipartite_graphs`, `enumerate_diamond_free_graphs`,
`enumerate_distance_hereditary_graphs`, `enumerate_interval_graphs`,
`enumerate_line_graph_graphs`, `enumerate_outer_planar_graphs`,
`enumerate_perfect_graphs`, `enumerate_permutation_graphs`,
`enumerate_planar_graphs`,
`enumerate_proper_interval_graphs`, `enumerate_ptolemaic_graphs`,
`enumerate_series_parallel_graphs`, `enumerate_split_graphs`,
`enumerate_strongly_chordal_graphs`, `enumerate_three_leaf_power_graphs`,
`enumerate_threshold_graphs`, `enumerate_trapezoid_graphs`,
`enumerate_trivially_perfect_graphs`, `enumerate_weakly_chordal_graphs`

## API

### `is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool`

Check if a graph belongs to the specified class.

- `n_or_graph`: Number of vertices (int) or a `networkx.Graph`
- `edges`: List of `(u, v)` tuples (1-indexed). Required when `n_or_graph` is an int.
- `algorithm`: Optional algorithm name (str). See docstrings for available options.

### `recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)`

Same as `is_<type>` but returns a tuple. The second element is reserved for future certificate support.

## Building

The C++ extension compiles against the library headers: from a repository
checkout it uses `../include`, and an sdist created with
`python -m build --sdist` bundles a copy of the headers, so standalone
sdists build as well.

## License

MIT
