# graph-recognition

Python bindings for the [Graph Recognition Library](https://github.com/junkawahara/ai-coded-graph-recognition-test) — a C++11 header-only library providing recognition algorithms for 30+ graph classes.

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
| AT-free | `is_at_free`, `recognize_at_free` |
| Bipartite | `is_bipartite`, `recognize_bipartite` |
| Bipartite permutation | `is_bipartite_permutation`, `recognize_bipartite_permutation` |
| Block | `is_block`, `recognize_block` |
| Cactus | `is_cactus`, `recognize_cactus` |
| Chain | `is_chain`, `recognize_chain` |
| Chordal | `is_chordal`, `recognize_chordal` |
| Chordal bipartite | `is_chordal_bipartite`, `recognize_chordal_bipartite` |
| Circular-arc | `is_circular_arc`, `recognize_circular_arc` |
| Co-chordal | `is_co_chordal`, `recognize_co_chordal` |
| Co-comparability | `is_co_comparability`, `recognize_co_comparability` |
| Co-interval | `is_co_interval`, `recognize_co_interval` |
| Co-chain | `is_cochain`, `recognize_cochain` |
| Cograph | `is_cograph`, `recognize_cograph` |
| Comparability | `is_comparability`, `recognize_comparability` |
| Distance-hereditary | `is_distance_hereditary`, `recognize_distance_hereditary` |
| Interval | `is_interval`, `recognize_interval` |
| Outerplanar | `is_outer_planar`, `recognize_outer_planar` |
| Permutation | `is_permutation`, `recognize_permutation` |
| Planar | `is_planar`, `recognize_planar` |
| Proper interval | `is_proper_interval`, `recognize_proper_interval` |
| Ptolemaic | `is_ptolemaic`, `recognize_ptolemaic` |
| Quasi-threshold | `is_quasi_threshold`, `recognize_quasi_threshold` |
| Series-parallel | `is_series_parallel`, `recognize_series_parallel` |
| Split | `is_split`, `recognize_split` |
| Strongly chordal | `is_strongly_chordal`, `recognize_strongly_chordal` |
| Threshold | `is_threshold`, `recognize_threshold` |
| Trivially perfect | `is_trivially_perfect`, `recognize_trivially_perfect` |
| Unit interval | `is_unit_interval`, `recognize_unit_interval` |
| Weakly chordal | `is_weakly_chordal`, `recognize_weakly_chordal` |

Additionally: `enumerate_chordal_graphs(n)` for labeled chordal graph enumeration.

## API

### `is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool`

Check if a graph belongs to the specified class.

- `n_or_graph`: Number of vertices (int) or a `networkx.Graph`
- `edges`: List of `(u, v)` tuples (1-indexed). Required when `n_or_graph` is an int.
- `algorithm`: Optional algorithm name (str). See docstrings for available options.

### `recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)`

Same as `is_<type>` but returns a tuple. The second element is reserved for future certificate support.

## License

MIT
