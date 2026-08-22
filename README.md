# ai-coded-graph-recognition-test

[Japanese version (README_ja.md)](README_ja.md)

A C++11 header-only library that provides **recognition and enumeration algorithms for 75+ graph classes** — chordal, interval, planar, permutation, and many more. Given a graph, each recognizer determines whether it belongs to a specific class (with certificates); each enumerator generates all graphs of a class on n vertices (labeled for most classes, non-isomorphic for some).

This library was **entirely written by AI** (Claude 4.6 Opus and Codex 5.2) as an experiment to explore the limits of AI coding. The human role was limited to task instructions and code review — no code was written by hand.

## Motivation

This project investigates how far AI can go in implementing non-trivial algorithms from scratch. Graph class recognition was chosen as the domain because it requires:

- Understanding of formal definitions and theorems from structural graph theory
- Correct implementation of well-known algorithms (MCS, PEO, BFS, DFS, backtracking, etc.)
- Handling of subtle edge cases (empty graphs, disconnected graphs, complement closures)
- Designing and running test infrastructure to validate correctness

The experiment is ongoing. Findings will be documented as they emerge.

## Documentation

Full API documentation: **https://junkawahara.github.io/ai-coded-graph-recognition-test/en**

## Features

- **Header-only**: just `#include` and go — no linking required
- **C++11 compatible**: works with any modern compiler
- **75+ graph classes** with recognition, enumeration, or both
- **76 recognizers** with multiple algorithm variants (YES/NO + certificates)
- **73 enumerators** that generate all graphs of a given class on n vertices (labeled for most classes; some, e.g. tree/forest/caterpillar/halin/fullerene/chain/cochain/threshold/unicyclic/simple quadrangulation, enumerate non-isomorphic graphs)
- **CLI tools** for every recognizer and enumerator
- **Test infrastructure**: static test cases, randomized property tests, differential testing between algorithm variants
- **Python bindings** via pybind11 with NetworkX integration

## Supported Graph Classes

The **Complexity** column gives the time bound of the default recognition
algorithm (n = vertices, m = edges, Δ = maximum degree). Entries marked
*exponential* are worst-case exponential; see
[Performance Notes](#performance-notes) for details and practical limits.

### Chordal Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Chordal | `chordal.h` | Yes | O(n+m) | No induced cycle of length >= 4 |
| Strongly chordal | `strongly_chordal.h` | Yes | O(nmΔ) | Chordal + every even cycle (>= 6) has an odd chord |
| Proper chordal | `proper_chordal.h` | Yes | *exponential* | Chordal + admits indifference tree-layout |
| Split | `split.h` | Yes | O(n) | Vertices partition into a clique and an independent set |
| Threshold | `threshold.h` | Yes | O(n) | Iteratively removable isolated or universal vertices |
| Weakly chordal | `weakly_chordal.h` | Yes | O(n⁶) | No induced cycle of length >= 5 in G or complement(G) |
| Block | `block.h` | Yes | O(n+m) | Every biconnected component is a clique |
| Ptolemaic | `ptolemaic.h` | Yes | O(n³ log n) | Chordal + distance-hereditary |
| Trivially perfect | `trivially_perfect.h` | Yes | O(n(n+m)) | Chordal + cograph (= quasi-threshold) |
| Quasi-threshold | `quasi_threshold.h` | — | O(n(n+m)) | Another name for trivially perfect (thin wrapper) |
| k-tree | `ktree.h` | Yes | O(n² + nk²) | Graphs built from K_{k+1} by repeatedly attaching vertices to k-cliques |

### Interval / Circular-Arc Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Interval | `interval.h` | Yes | O(n³) | Intersection graph of intervals on the real line |
| Proper interval | `proper_interval.h` | Yes | O(n³ + nΔ³) | Interval graph with no containment between intervals |
| Unit interval | `unit_interval.h` | — | O(n³ + nΔ³) | Equal-length intervals (= proper interval) |
| Co-interval | `co_interval.h` | Yes | O(n³) | Complement is an interval graph |
| Circular-arc | `circular_arc.h` | Yes | *exponential* | Intersection graph of arcs on a circle |
| Proper circular-arc | `proper_circular_arc.h` | Yes | *exponential* | Circular-arc with no containment between arcs |

### Permutation / Comparability Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Permutation | `permutation.h` | Yes | O(n³) | Both G and complement(G) are comparability graphs |
| Comparability | `comparability.h` | Yes | O(nm) | Edges admit a transitive orientation |
| Co-comparability | `co_comparability.h` | Yes | O(n³) | Complement is a comparability graph |
| Bipartite permutation | `bipartite_permutation.h` | Yes | O(n³) | Bipartite + permutation |
| Trapezoid | `trapezoid.h` | Yes | O(n⁴) | Intersection graph of trapezoids between two parallel lines (generalises permutation) |

### Bipartite Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Bipartite | `bipartite.h` | Yes | O(n+m) | 2-colorable (no odd cycle) |
| Chordal bipartite | `chordal_bipartite.h` | Yes | O(m²Δ²) | Bipartite + no induced cycle of length >= 6. Default algorithm allocates a Θ(n²) adjacency matrix; use CYCLE_CHECK for large sparse graphs |
| Chain | `chain.h` | Yes | O(n+m) | Bipartite + neighborhoods form a total order by inclusion |
| Co-chain | `cochain.h` | Yes | O(n²) | Complement is a chain graph |
| Convex bipartite | `convex_bipartite.h` | Yes | O(nm) | Bipartite + one side has consecutive neighborhood property |
| Biconvex bipartite | `biconvex_bipartite.h` | Yes | O(nm) | Bipartite + both sides have consecutive neighborhood property |

### Planar Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Planar | `planar.h` | Yes | O(n+m) | No K5 or K3,3 minor |
| Outerplanar | `outer_planar.h` | Yes | O(n+m) | No K4 or K2,3 minor |
| Cactus | `cactus.h` | Yes | O(n+m) | Every biconnected component is a single edge or a simple cycle |
| Series-parallel | `series_parallel.h` | Yes | O(n+m) | No K4 minor (2-degenerate) |
| Apex | `apex.h` | Yes | O(n(n+m)) | Planar after removing one vertex |
| Maximal planar | `maximal_planar.h` | Yes | O(n+m) | Planar graphs where all faces are triangles |
| Cubic planar | `cubic_planar.h` | Yes | O(n+m) | 3-regular planar graphs |
| Polyhedral | `polyhedral.h` | Yes | O(n²(n+m)) | 3-connected planar graphs (Steinitz's theorem) |
| Simple quadrangulation | `simple_quadrangulation.h` | Yes | O(n³) | 3-connected planar graphs with all quadrilateral faces |
| Halin | `halin.h` | Yes | O(n⁴) | Planar graph formed from a tree + outer cycle |
| Fullerene | `fullerene.h` | Yes | O(n⁴) | 3-regular planar graphs with pentagonal and hexagonal faces |

### Perfect / Structural Classes

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Perfect | `perfect.h` | Yes | *exponential* | No odd hole or odd antihole (SPGT) |
| Cograph | `cograph.h` | Yes | O(n(n+m)) | No induced P4 |
| Distance-hereditary | `distance_hereditary.h` | Yes | O(n³) | Distances preserved in all connected induced subgraphs |
| AT-free | `at_free.h` | Yes | O(n³) | No asteroidal triple |
| Co-chordal | `co_chordal.h` | Yes | O(n²) | Complement is a chordal graph |
| Line graph | `line_graph.h` | Yes | *exponential* | Edge-intersection graph of another graph |
| Circle | `circle.h` | Yes | polynomial | Intersection graph of chords of a circle |
| Meyniel | `meyniel.h` | Yes | *exponential* | Every odd cycle of length >= 5 has at least two chords |
| Parity | `parity.h` | Yes | *exponential* | Every two induced paths between same endpoints have same parity |
| Even-hole-free | `even_hole_free.h` | Yes | *exponential* | No induced even cycle of length >= 4 |
| Odd-hole-free | `odd_hole_free.h` | Yes | *exponential* | No induced odd cycle of length >= 5 |
| Cluster | `cluster.h` | Yes | O(n+m) | Disjoint union of complete graphs |
| Self-complementary | `self_complementary.h` | Yes | *exponential* | Isomorphic to own complement |

### Forbidden Induced Subgraph Classes

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Claw-free | `claw_free.h` | Yes | O(nΔ³) | No induced K1,3 |
| Diamond-free | `diamond_free.h` | Yes | O(m²) | No induced K4 minus one edge |
| Triangle-free | `triangle_free.h` | Yes | O(mΔ) | No K3 |
| Bull-free | `bull_free.h` | Yes | O(mΔ²) | No induced bull graph |
| P5-free | `p5_free.h` | Yes | O(nΔ⁴) | No induced path on 5 vertices |
| Gem-free | `gem_free.h` | Yes | O(nΔ⁴) | No induced gem (fan) graph |

### Leaf Power Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| 3-leaf power | `three_leaf_power.h` | Yes | O(n + m log n) | Leaf power with distance threshold 3 |
| 4-leaf power | `four_leaf_power.h` | Yes | *exponential* | Leaf power with distance threshold 4 |
| 5-leaf power | `five_leaf_power.h` | Yes | *exponential* | Leaf power with distance threshold 5 |

### Tree / Forest Family

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Tree | `tree.h` | Yes | O(n+m) | Connected acyclic graph |
| Forest | `forest.h` | Yes | O(n+m) | Acyclic graph (disjoint union of trees) |
| Caterpillar | `caterpillar.h` | Yes | O(n+m) | Tree where all vertices are within distance 1 of a path |
| Unicyclic | `unicyclic.h` | Yes | O(n+m) | Connected graph with exactly one cycle |

### Connectivity / Regularity

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Biconnected | `biconnected.h` | Yes | O(n+m) | 2-connected (at least 3 vertices, no cut vertex) |
| Triconnected | `triconnected.h` | — | O(n²(n+m)) | 3-connected |
| Eulerian | `eulerian.h` | Yes | O(n) | All vertices have even degree |
| k-regular | `kregular.h` | Yes | O(n) | All vertices have degree k |
| Cubic | `cubic.h` | Yes | O(n) | 3-regular graphs |
| Strongly regular | `strongly_regular.h` | Yes | O(n²Δ) | Regular with uniform adjacency counts |
| Snark | `snark.h` | Yes | *exponential* | Cyclically 4-edge-connected cubic graphs of girth >= 5 with chromatic index 4 |
| Laman | `laman.h` | Yes | O(n²) | Minimally rigid graphs in 2D |

### Directed Graph Classes

| Graph Class | Header | Enum | Complexity | Description |
|---|---|---|---|---|
| Tournament | `tournament.h` | Yes | O(n²) | Complete directed graphs (orientations of Kn) |
| Directed graph | `digraph.h` | Yes | O(m log m) | All simple directed graphs |
| Poset | `poset.h` | Yes | O(m(n+m)) | Partially ordered sets (Hasse diagrams) |

## Building

```bash
make          # build all recognizers and enumerators into bin/
make clean    # remove binaries
```

Requires a C++11-capable g++. Customize with `CXX` and `CXXFLAGS`.

## Usage

### Recognition

Each recognizer reads a graph from stdin and prints the result:

```bash
echo "4 4
1 2
2 3
3 4
4 1" | ./bin/chordal
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

### Enumeration

Each enumerator generates all graphs (labeled for most classes) on n vertices for a given class:

```bash
echo "4" | ./bin/chordal_enum
# Output: count on first line, then edge lists
```

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
pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

# with NetworkX support
pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
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

All 76 recognizer classes are available as `is_<type>()` and `recognize_<type>()` functions, and 36 classes as `enumerate_<type>_graphs()` functions. See [python/README.md](python/README.md) for details.

## Performance Notes

Most recognizers run in low-order polynomial time and handle thousands of
vertices. Known exceptions worth planning around:

- **Perfect** (`perfect.h`): the odd-antihole check builds the complement
  explicitly, so even sparse inputs pay for a Θ(n²)-edge search, and the
  induced even-path DFS is exponential in the worst case. Measured: sparse
  chordal n=400 ≈ 1.7 s, K(200,200) ≈ 27 s, K(400,400) > 60 s. A few hundred
  vertices is the practical limit; `odd_hole_free.h` is much cheaper on
  sparse graphs because it skips the complement.
- **Odd-hole-free / even-hole-free** (`odd_hole_free.h`, `even_hole_free.h`):
  DFS-based induced-path search, exponential in the worst case but fast on
  typical inputs. The polynomial algorithms from the literature
  (Chudnovsky–Scott–Seymour–Spirkl) are not implemented.
- **Circle** (`circle.h`): the default Naji linear-system algorithm is
  polynomial (n=300 dense in ≈ 1 s); the optional `DOW_BACKTRACKING`
  certificate algorithm is exponential and practical only up to n ≈ 9.
- **4-leaf / 5-leaf power** (`four_leaf_power.h`, `five_leaf_power.h`): the
  strongly-chordal pre-filter is polynomial, but the subsequent (Steiner)
  root realization search backtracks over center/tree assignments and is
  exponential in the worst case.
- **Self-complementary** (`self_complementary.h`): isomorphism with the
  complement via degree-pruned backtracking; worst case exponential (e.g. on
  regular self-complementary graphs).
- **Snark** (`snark.h`): the chromatic-index step is a 3-edge-coloring
  backtracking search, exponential in the worst case (the girth and cyclic
  edge-connectivity steps are polynomial).
- **Proper chordal** (`proper_chordal.h`): the nested-convexity check
  enumerates vertex orderings of a block by brute force, so the worst case
  is exponential even though the literature algorithm is O(n⁴).
- **Circular-arc / proper circular-arc** (`circular_arc.h`,
  `proper_circular_arc.h`): both rely on backtracking (circular clique
  orderings resp. arc endpoint orderings) and are exponential in the worst
  case; typical inputs stay on the polynomial path.
- **Line graph** (`line_graph.h`): the O(mΔ) filter decides most inputs, but
  ambiguous cases fall back to a Krausz-partition backtracking search that is
  exponential in the worst case.
- **Meyniel / parity** (`meyniel.h`, `parity.h`): both enumerate
  (induced) paths per edge or vertex pair, which is exponential in the worst
  case but fast on typical inputs.

## Testing

```bash
# Run the gtest suite with the default filter (~6 seconds once built)
make test

# Run a subset
./gtest_all --gtest_filter='Interval*'

# Also run the randomized Property tests (only the multi-minute enum cases stay excluded)
make test-quick
```

Recognizer and enumerator tests live under `tests/gtest/recognizers/` and `tests/gtest/enumerators/`, parameterised over the static cases in `tests/<type>/`. Property-based randomized tests live under `tests/gtest/property/`. The earlier Python/Bash test infrastructure (`tests/legacy/`) has been removed; it is still available at the `legacy-tests` git tag.

## Project Structure

```
include/          Header-only library (all algorithms)
  graph.h           Graph representation (1-indexed, adjacency list + set)
  chordal.h         Chordal graph recognition
  interval.h        Interval graph recognition
  ...               (150+ headers)
src/              CLI entry points
python/           Python wrapper (pybind11)
  src/              Package source (graph_recognition)
  tests/            pytest test suite
tests/            Test infrastructure
  <type>/           Static test cases (.in / .exp)
  gtest/            gtest suite (recognizers/, enumerators/, property/, helpers/)
docs/             Sphinx + Doxygen documentation
```

## AI Models Used

- **Claude 4.6 Opus** (Anthropic) — via Claude Code CLI
- **Codex 5.2** (OpenAI)

All code was generated by these models. The human provided task descriptions, reviewed outputs, and directed bug fixes — but wrote zero lines of code.

## License

[MIT](LICENSE)
