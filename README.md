# ai-coded-graph-recognition-test

[Japanese version (README_ja.md)](README_ja.md)

An experiment to explore the limits of AI coding: a C++11 header-only library for graph class recognition, **entirely written by AI** (Claude 4.6 Opus and Codex 5.2). The human role was limited to task instructions and code review — no code was written by hand.

## Motivation

This project investigates how far AI can go in implementing non-trivial algorithms from scratch. Graph class recognition was chosen as the domain because it requires:

- Understanding of formal definitions and theorems from structural graph theory
- Correct implementation of well-known algorithms (MCS, PEO, BFS, DFS, backtracking, etc.)
- Handling of subtle edge cases (empty graphs, disconnected graphs, complement closures)
- Designing and running test infrastructure to validate correctness

The experiment is ongoing. Findings will be documented as they emerge.

## Documentation

Full API documentation: **https://junkawahara.github.io/ai-coded-graph-recognition-test/**

## Features

- **Header-only**: just `#include` and go — no linking required
- **C++11 compatible**: works with any modern compiler
- **30+ graph classes** recognized with multiple algorithm variants
- **CLI tools** for each recognizer (reads a graph, prints YES/NO)
- **Test infrastructure**: static test cases, Python brute-force checkers, fuzz testing, differential testing between algorithm variants

## Supported Graph Classes

### Chordal Family

| Graph Class | Header | Description |
|---|---|---|
| Chordal | `chordal.h` | No induced cycle of length ≥ 4 |
| Strongly chordal | `strongly_chordal.h` | Chordal + every even cycle (≥ 6) has an odd chord |
| Split | `split.h` | Vertices partition into a clique and an independent set |
| Threshold | `threshold.h` | Iteratively removable isolated or universal vertices |
| Weakly chordal | `weakly_chordal.h` | No induced cycle of length ≥ 5 in G or complement(G) |
| Block | `block.h` | Every biconnected component is a clique |
| Ptolemaic | `ptolemaic.h` | Chordal + distance-hereditary |
| Trivially perfect | `trivially_perfect.h` | Chordal + cograph (= quasi-threshold) |
| Quasi-threshold | `quasi_threshold.h` | Alias for trivially perfect |

### Interval Family

| Graph Class | Header | Description |
|---|---|---|
| Interval | `interval.h` | Intersection graph of intervals on the real line |
| Proper interval | `proper_interval.h` | Interval graph with no containment between intervals |
| Unit interval | `unit_interval.h` | Interval graph with equal-length intervals (= proper interval) |
| Co-interval | `co_interval.h` | Complement is an interval graph |
| Circular-arc | `circular_arc.h` | Intersection graph of arcs on a circle |

### Permutation Family

| Graph Class | Header | Description |
|---|---|---|
| Permutation | `permutation.h` | Both G and complement(G) are comparability graphs |
| Comparability | `comparability.h` | Edges admit a transitive orientation |
| Co-comparability | `co_comparability.h` | Complement is a comparability graph |
| Bipartite permutation | `bipartite_permutation.h` | Bipartite + permutation |

### Bipartite Family

| Graph Class | Header | Description |
|---|---|---|
| Bipartite | `bipartite.h` | 2-colorable (no odd cycle) |
| Chordal bipartite | `chordal_bipartite.h` | Bipartite + no induced cycle of length ≥ 6 |
| Chain | `chain.h` | Bipartite + neighborhoods form a total order by inclusion |
| Co-chain | `cochain.h` | Complement is a chain graph |

### Planar Family

| Graph Class | Header | Description |
|---|---|---|
| Planar | `planar.h` | No K₅ or K₃,₃ minor |
| Outerplanar | `outer_planar.h` | No K₄ or K₂,₃ minor |
| Cactus | `cactus.h` | Every biconnected component is a single edge or a simple cycle |
| Series-parallel | `series_parallel.h` | No K₄ minor (2-degenerate) |

### Other Classes

| Graph Class | Header | Description |
|---|---|---|
| Cograph | `cograph.h` | No induced P₄ |
| Distance-hereditary | `distance_hereditary.h` | Distances preserved in all connected induced subgraphs |
| AT-free | `at_free.h` | No asteroidal triple |
| Co-chordal | `co_chordal.h` | Complement is a chordal graph |

## Building

```bash
make          # build all recognizers
make clean    # remove binaries
```

Requires a C++11-capable g++. Customize with `CXX` and `CXXFLAGS`.

## Usage

Each recognizer reads a graph from stdin and prints the result:

```bash
echo "4 4
1 2
2 3
3 4
4 1" | ./chordal
# Output: NO
```

**Input format:**
```
n m
u1 v1
u2 v2
...
```
where `n` = number of vertices, `m` = number of edges, vertices are 1-indexed.

**Output format:** `YES` or `NO`, followed by class-specific data (e.g., interval models).

### Library Usage

```cpp
#include "interval.h"

graph_recognition::Graph g(4, {{1,2}, {2,3}, {3,4}, {4,1}});
auto result = graph_recognition::check_interval(g);
if (result.is_interval) {
    // result.intervals[v] = {L, R} for each vertex v
}
```

Most recognizers support algorithm selection:

```cpp
// Use backtracking instead of the default AT-free algorithm
auto result = graph_recognition::check_interval(g,
    graph_recognition::IntervalAlgorithm::BACKTRACKING);
```

### Python Wrapper

A Python package is available via pybind11 bindings:

```bash
cd python
pip install .                    # basic install
pip install ".[networkx]"        # with NetworkX support
```

```python
from graph_recognition import is_interval, is_chordal

# Edge list (1-indexed)
is_interval(4, [(1, 2), (2, 3), (3, 4)])  # True
is_chordal(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

# Algorithm selection
is_interval(4, [(1, 2), (2, 3), (3, 4)], algorithm="backtracking")

# NetworkX integration (arbitrary node types supported)
import networkx as nx
is_interval(nx.path_graph(5))  # True
```

All 30 graph classes are available as `is_<type>()` and `recognize_<type>()` functions. See [python/README.md](python/README.md) for details.

## Testing

```bash
# Run static tests for a specific graph class
bash tests/run.sh interval

# Differential testing: compare two binaries on random graphs
python3 tests/compare.py ./interval ./interval_v2 1000

# Fuzz testing
bash tests/fuzz.sh ./binary1 ./binary2 500
```

Python brute-force checkers (`tests/check_*.py`) validate results by exhaustive enumeration for small graphs (n ≤ 9).

## Project Structure

```
include/          Header-only library (all algorithms)
  graph.h           Graph representation (1-indexed, adjacency list + set)
  chordal.h         Chordal graph recognition
  interval.h        Interval graph recognition
  ...               (30+ headers)
src/              CLI entry points
python/           Python wrapper (pybind11)
  src/              Package source (graph_recognition)
  tests/            pytest test suite
tests/            Test infrastructure
  <type>/           Static test cases (.in / .exp)
  check_<type>.py   Brute-force checkers
  run.sh            Test runner
  compare.py        Differential testing
  fuzz.sh           Fuzz testing
docs/             Sphinx + Doxygen documentation
```

## AI Models Used

- **Claude 4.6 Opus** (Anthropic) — via Claude Code CLI
- **Codex 5.2** (OpenAI)

All code was generated by these models. The human provided task descriptions, reviewed outputs, and directed bug fixes — but wrote zero lines of code.

## License

[MIT](LICENSE)
