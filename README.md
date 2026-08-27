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
- **82 enumerators** that generate all graphs of a given class on n vertices (labeled for most classes; some, e.g. tree/forest/caterpillar/halin/fullerene/chain/cochain/threshold/unicyclic/simple quadrangulation/proper interval/trivially perfect/cograph/cluster/triangle-free/bipartite/permutation/circle/eulerian (unlabeled), enumerate non-isomorphic graphs)
- **CLI tools** for every recognizer and enumerator
- **Graph decompositions** as first-class components: modular decomposition, split decomposition (Cunningham), SPQR trees, cotrees, clique trees and tree decompositions, block-cut trees, PQ-trees, transitive orientations, planar embeddings, and the elimination orderings and layouts the recognizers are built on
- **Test infrastructure**: static test cases, randomized property tests, differential testing between algorithm variants
- **Python bindings** via pybind11 with NetworkX integration

## Supported Graph Classes

The **Recognition Complexity** column gives the time bound of the *recognition*
algorithm that runs by default (n = vertices, m = edges, Δ = maximum degree);
it says nothing about the cost of enumeration. Entries marked *exponential* are
worst-case exponential; see [Performance Notes](#performance-notes) for details
and practical limits.

The **References** column cites the characterization or algorithm each
implementation follows — `Rec:` for recognition, and one `Enum` line per
enumerator the class has, split into four categories: `Enum (labeled):` (all
labeled graphs of the class on n vertices), `Enum (unlabeled):` (one
representative per isomorphism class), `Enum (subgraph):` (the spanning
subgraphs of a host graph that belong to the class), and
`Enum (induced subgraph):` (the vertex subsets of a host graph that induce a
member of the class). Where an implementation follows the definition of the
class directly, the paper that established the characterization (or the
standard reference for the problem) is cited instead. `—` means no specific
publication applies (the construction is folklore); a bare `Enum: —` with no
category label means the class has no enumerator. Full bibliographic entries
are listed under [References](#references).

### Chordal Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Chordal | `chordal.h` | O(n+m) | Rec: [Fulkerson+ 65], [Rose+ 76], [Tarjan+ 84]<br>Enum (labeled): [Kiyomi+ 06]<br>Enum (subgraph): [Kiyomi+ 06] | No induced cycle of length >= 4 |
| Strongly chordal | `strongly_chordal.h` | O(nmΔ) | Rec: [Farber 83], [Dahlhaus+ 87]<br>Enum (labeled): [Avis+ 96] | Chordal + every even cycle (>= 6) has an odd chord |
| Proper chordal | `proper_chordal.h` | *exponential* | Rec: [Paul+ 24]<br>Enum (labeled): [Avis+ 96] | Chordal + admits indifference tree-layout |
| Split | `split.h` | O(n) | Rec: [Földes+ 77], [Hammer+ 81]<br>Enum (labeled): [Cheng+ 16], [Troyka 19] | Vertices partition into a clique and an independent set |
| Threshold | `threshold.h` | O(n) | Rec: [Chvátal+ 77], [Mahadev+ 95]<br>Enum (unlabeled): [Chvátal+ 77] | Iteratively removable isolated or universal vertices |
| Weakly chordal | `weakly_chordal.h` | O(n⁶) | Rec: [Hayward 85], [Spinrad+ 95]<br>Enum (labeled): [Kiyomi thesis 06] | No induced cycle of length >= 5 in G or complement(G) |
| Block | `block.h` | O(n+m) | Rec: [Harary 63], [Tarjan 72]<br>Enum (labeled): [Avis+ 96] | Every biconnected component is a clique |
| Ptolemaic | `ptolemaic.h` | O(n³ log n) | Rec: [Howorka 81]<br>Enum (labeled): [Avis+ 96] | Chordal + distance-hereditary |
| Trivially perfect | `trivially_perfect.h` | O(n(n+m)) | Rec: [Wolk 62], [Golumbic 78]<br>Enum (labeled): [Golumbic 78]<br>Enum (unlabeled): [Wolk 62], [Beyer+ 80] | Chordal + cograph (= quasi-threshold) |
| Quasi-threshold | `quasi_threshold.h` | O(n(n+m)) | Rec: [Wolk 62], [Yan+ 96]<br>Enum: — | Another name for trivially perfect (thin wrapper) |
| k-tree | `ktree.h` | O(n² + nk²) | Rec: [Rose 74]<br>Enum (labeled): [Beineke+ 69], [Avis+ 96] | Graphs built from K_{k+1} by repeatedly attaching vertices to k-cliques |

### Interval / Circular-Arc Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Interval | `interval.h` | O(n³) | Rec: [Lekkerkerker+ 62]<br>Enum (labeled): [Kiyomi-Kijima+ 06] | Intersection graph of intervals on the real line |
| Proper interval | `proper_interval.h` | O(n³ + nΔ³) | Rec: [Roberts 69]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [Saitoh+ 10] | Interval graph with no containment between intervals |
| Unit interval | `unit_interval.h` | O(n³ + nΔ³) | Rec: [Roberts 69]<br>Enum: — | Equal-length intervals (= proper interval) |
| Co-interval | `co_interval.h` | O(n³) | Rec: [Lekkerkerker+ 62]<br>Enum (labeled): [Avis+ 96] | Complement is an interval graph |
| Circular-arc | `circular_arc.h` | *exponential* | Rec: [Tucker 80], [McConnell 03]<br>Enum (labeled): [Avis+ 96] | Intersection graph of arcs on a circle |
| Proper circular-arc | `proper_circular_arc.h` | *exponential* | Rec: [Tucker 74]<br>Enum (labeled): [Avis+ 96] | Circular-arc with no containment between arcs |

### Permutation / Comparability Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Permutation | `permutation.h` | O(n³) | Rec: [Pnueli+ 71], [Gallai 67]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | Both G and complement(G) are comparability graphs |
| Comparability | `comparability.h` | O(nm) | Rec: [Gallai 67], [Golumbic 80]<br>Enum (labeled): [Avis+ 96] | Edges admit a transitive orientation |
| Co-comparability | `co_comparability.h` | O(n³) | Rec: [Gallai 67]<br>Enum (labeled): [Avis+ 96] | Complement is a comparability graph |
| Bipartite permutation | `bipartite_permutation.h` | O(n³) | Rec: [Spinrad+ 87]<br>Enum (labeled): [Avis+ 96] | Bipartite + permutation |
| Trapezoid | `trapezoid.h` | O(n⁴) | Rec: [Dagan+ 88], [Cogis 82]<br>Enum (labeled): [Avis+ 96] | Intersection graph of trapezoids between two parallel lines (generalises permutation) |

### Bipartite Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Bipartite | `bipartite.h` | O(n+m) | Rec: [König 36]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | 2-colorable (no odd cycle) |
| Chordal bipartite | `chordal_bipartite.h` | O(m²Δ²) | Rec: [Golumbic+ 78]<br>Enum (labeled): [Avis+ 96]<br>Enum (induced subgraph): [Kurita+ 19] | Bipartite + no induced cycle of length >= 6. Default algorithm allocates a Θ(n²) adjacency matrix; use CYCLE_CHECK for large sparse graphs |
| Chain | `chain.h` | O(n+m) | Rec: [Yannakakis 82]<br>Enum (unlabeled): — | Bipartite + neighborhoods form a total order by inclusion |
| Co-chain | `cochain.h` | O(n²) | Rec: [Yannakakis 82]<br>Enum (unlabeled): — | Complement is a chain graph |
| Convex bipartite | `convex_bipartite.h` | O(nm) | Rec: [Booth+ 76]<br>Enum (labeled): [Avis+ 96] | Bipartite + one side has consecutive neighborhood property |
| Biconvex bipartite | `biconvex_bipartite.h` | O(nm) | Rec: [Abbas+ 00], [Booth+ 76]<br>Enum (labeled): [Avis+ 96] | Bipartite + both sides have consecutive neighborhood property |

### Planar Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Planar | `planar.h` | O(n+m) | Rec: [Kuratowski 30], [de Fraysseix+ 06], [Brandes 09]<br>Enum (labeled): [Avis+ 96] | No K5 or K3,3 minor |
| Outerplanar | `outer_planar.h` | O(n+m) | Rec: [Chartrand+ 67], [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96] | No K4 or K2,3 minor |
| Cactus | `cactus.h` | O(n+m) | Rec: [Harary+ 53], [Tarjan 72]<br>Enum (labeled): [Avis+ 96] | Every biconnected component is a single edge or a simple cycle |
| Series-parallel | `series_parallel.h` | O(n+m) | Rec: [Duffin 65], [Valdes+ 82]<br>Enum (labeled): [Avis+ 96] | No K4 minor (2-degenerate) |
| Apex | `apex.h` | O(n(n+m)) | Rec: [Robertson+ 95], [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96] | Planar after removing one vertex |
| Maximal planar | `maximal_planar.h` | O(n+m) | Rec: [Kuratowski 30], [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96] | Planar graphs where all faces are triangles |
| Cubic planar | `cubic_planar.h` | O(n+m) | Rec: [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96] | 3-regular planar graphs |
| Polyhedral | `polyhedral.h` | O(n²(n+m)) | Rec: [Steinitz 22], [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96] | 3-connected planar graphs (Steinitz's theorem) |
| Simple quadrangulation | `simple_quadrangulation.h` | O(n³) | Rec: [Steinitz 22], [de Fraysseix+ 06]<br>Enum (unlabeled): [Avis+ 96] | 3-connected planar graphs with all quadrilateral faces |
| Halin | `halin.h` | O(n⁴) | Rec: [Halin 71]<br>Enum (unlabeled): [Halin 71], [Wright+ 86] | Planar graph formed from a tree + outer cycle |
| Fullerene | `fullerene.h` | O(n⁴) | Rec: [de Fraysseix+ 06]<br>Enum (unlabeled): [Avis+ 96] | 3-regular planar graphs with pentagonal and hexagonal faces |

### Perfect / Structural Classes

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Perfect | `perfect.h` | *exponential* | Rec: [Chudnovsky+ 06]<br>Enum (labeled): [Avis+ 96] | No odd hole or odd antihole (SPGT) |
| Cograph | `cograph.h` | O(n(n+m)) | Rec: [Seinsche 74], [Corneil+ 81], [Corneil+ 85]<br>Enum (labeled): [Corneil+ 81]<br>Enum (unlabeled): [Jones+ 18] | No induced P4 |
| Distance-hereditary | `distance_hereditary.h` | O(n³) | Rec: [Howorka 77], [Bandelt+ 86]<br>Enum (labeled): [Avis+ 96] | Distances preserved in all connected induced subgraphs |
| AT-free | `at_free.h` | O(n³) | Rec: [Lekkerkerker+ 62], [Corneil+ 97]<br>Enum (labeled): [Avis+ 96] | No asteroidal triple |
| Co-chordal | `co_chordal.h` | O(n²) | Rec: [Fulkerson+ 65], [Tarjan+ 84]<br>Enum (labeled): [Avis+ 96] | Complement is a chordal graph |
| Line graph | `line_graph.h` | *exponential* | Rec: [Whitney 32], [Krausz 43]<br>Enum (labeled): [Avis+ 96] | Edge-intersection graph of another graph |
| Circle | `circle.h` | polynomial | Rec: [Naji 85], [Gasse 97], [Geelen+ 20]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | Intersection graph of chords of a circle |
| Meyniel | `meyniel.h` | *exponential* | Rec: [Meyniel 76], [Burlet+ 84a]<br>Enum (labeled): [Avis+ 96] | Every odd cycle of length >= 5 has at least two chords |
| Parity | `parity.h` | *exponential* | Rec: [Burlet+ 84b]<br>Enum (labeled): [Avis+ 96] | Every two induced paths between same endpoints have same parity |
| Even-hole-free | `even_hole_free.h` | *exponential* | Rec: [Conforti+ 02]<br>Enum (labeled): [Avis+ 96] | No induced even cycle of length >= 4 |
| Odd-hole-free | `odd_hole_free.h` | *exponential* | Rec: [Chudnovsky+ 20]<br>Enum (labeled): [Avis+ 96] | No induced odd cycle of length >= 5 |
| Cluster | `cluster.h` | O(n+m) | Rec: [Brandstädt+ 99]<br>Enum (labeled): —<br>Enum (unlabeled): — | Disjoint union of complete graphs |
| Self-complementary | `self_complementary.h` | *exponential* | Rec: [Sachs 62], [Ringel 63]<br>Enum (labeled): [Sachs 62], [Ringel 63] | Isomorphic to own complement |

### Forbidden Induced Subgraph Classes

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Claw-free | `claw_free.h` | O(nΔ³) | Rec: [Faudree+ 97]<br>Enum (labeled): [Avis+ 96] | No induced K1,3 |
| Diamond-free | `diamond_free.h` | O(m²) | Rec: [Brandstädt+ 99]<br>Enum (labeled): [Avis+ 96] | No induced K4 minus one edge |
| Triangle-free | `triangle_free.h` | O(mΔ) | Rec: [Itai+ 78]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | No K3 |
| Bull-free | `bull_free.h` | O(mΔ²) | Rec: [Chudnovsky 12]<br>Enum (labeled): [Avis+ 96] | No induced bull graph |
| P5-free | `p5_free.h` | O(nΔ⁴) | Rec: [Brandstädt+ 99]<br>Enum (labeled): [Avis+ 96] | No induced path on 5 vertices |
| Gem-free | `gem_free.h` | O(nΔ⁴) | Rec: [Brandstädt+ 99]<br>Enum (labeled): [Avis+ 96] | No induced gem (fan) graph |

### Leaf Power Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| 3-leaf power | `three_leaf_power.h` | O(n + m log n) | Rec: [Nishimura+ 02], [Brandstädt+ 06], [Dom+ 06]<br>Enum (labeled): [Avis+ 96] | Leaf power with distance threshold 3 |
| 4-leaf power | `four_leaf_power.h` | *exponential* | Rec: [Brandstädt+ 08]<br>Enum (labeled): [Avis+ 96] | Leaf power with distance threshold 4 |
| 5-leaf power | `five_leaf_power.h` | *exponential* | Rec: [Chang+ 07], [Lafond 22]<br>Enum (labeled): [Avis+ 96] | Leaf power with distance threshold 5 |

### Tree / Forest Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Tree | `tree.h` | O(n+m) | Rec: —<br>Enum (unlabeled): [Beyer+ 80], [Wright+ 86] | Connected acyclic graph |
| Forest | `forest.h` | O(n+m) | Rec: —<br>Enum (unlabeled): [Wright+ 86] | Acyclic graph (disjoint union of trees) |
| Caterpillar | `caterpillar.h` | O(n+m) | Rec: [Harary+ 73]<br>Enum (unlabeled): [Harary+ 73] | Tree where all vertices are within distance 1 of a path |
| Unicyclic | `unicyclic.h` | O(n+m) | Rec: —<br>Enum (unlabeled): [Beyer+ 80] | Connected graph with exactly one cycle |

### Connectivity / Regularity

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Biconnected | `biconnected.h` | O(n+m) | Rec: [Tarjan 72]<br>Enum (labeled): [Avis+ 96] | 2-connected (at least 3 vertices, no cut vertex) |
| Triconnected | `triconnected.h` | O(n²(n+m)) | Rec: [Hopcroft+ 73]<br>Enum: — | 3-connected |
| Eulerian | `eulerian.h` | O(n) | Rec: [Euler 1741]<br>Enum (labeled): —<br>Enum (unlabeled): [McKay 98] | All vertices have even degree |
| k-regular | `kregular.h` | O(n) | Rec: —<br>Enum (labeled): [Meringer 99] | All vertices have degree k |
| Cubic | `cubic.h` | O(n) | Rec: [Petersen 1891]<br>Enum (labeled): [Avis+ 96] | 3-regular graphs |
| Strongly regular | `strongly_regular.h` | O(n²Δ) | Rec: [Bose 63]<br>Enum (labeled): [Bose 63] | Regular with uniform adjacency counts |
| Snark | `snark.h` | *exponential* | Rec: [Isaacs 75]<br>Enum (labeled): [Avis+ 96] | Cyclically 4-edge-connected cubic graphs of girth >= 5 with chromatic index 4 |
| Laman | `laman.h` | O(n²) | Rec: [Laman 70], [Jacobs+ 97]<br>Enum (labeled): [Avis+ 96] | Minimally rigid graphs in 2D |

### Directed Graph Classes

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Tournament | `tournament.h` | O(n²) | Rec: [Moon 68]<br>Enum (labeled): — | Complete directed graphs (orientations of Kn) |
| Directed graph | `digraph.h` | O(m log m) | Rec: —<br>Enum (labeled): — | All simple directed graphs |
| Poset | `poset.h` | O(m(n+m)) | Rec: —<br>Enum (labeled): [Brinkmann+ 02] | Partially ordered sets (Hasse diagrams) |

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
echo "4" | ./bin/chordal_labeled_enum
# Output: count on first line, then edge lists
```

The chordal enumerator uses the Kiyomi--Uno chordal-specific reverse search by
default. Its search tree removes a minimum-degree simplicial vertex (using the
smallest label to break ties), and generates children by attaching a new vertex
to a clique. The previous largest-label vertex search remains available as
`ChordalLabeledEnumAlgorithm::LEGACY_VERTEX_REVERSE_SEARCH`.

For counting, filtering, or writing large enumerations, use the callback API so
graphs are not retained in memory:

```cpp
#include "chordal_labeled_enum.h"

std::size_t count = 0;
graph_recognition::enumerate_chordal_labeled_graphs_reverse_search_cb(
    7,
    [&count](const graph_recognition::EnumeratedGraph&) { ++count; });
// count == 617675
```

Kiyomi and Uno's O(1) amortized-time and O(1)-delay bounds apply to their
optimized difference-output implementation. This library maintains
straightforward O(n²) state and constructs a complete edge list for each
callback, so those bounds do not apply to this API.

The interval enumerator likewise defaults to the class-specific reverse
search of Kiyomi, Kijima, and Uno. Starting from K_n, it deletes one edge at a
time and accepts only candidates whose canonical edge-addition parent is the
current graph. `IntervalLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER` retains the
former chordal-tree filtering implementation. A streaming counterpart is
available as `enumerate_interval_labeled_graphs_reverse_search_cb`.

The weakly chordal enumerator now defaults to Kiyomi's class-specific
edge-addition reverse search. It starts at the empty spanning graph of K_n and
accepts a child exactly when the added edge is the youngest edge whose deletion
preserves weak chordality. The former hereditary largest-label vertex search is
available as
`WeaklyChordalLabeledEnumAlgorithm::GENERIC_VERTEX_AUGMENTATION`; the streaming API is
`enumerate_weakly_chordal_labeled_graphs_reverse_search_cb`.

### Subgraph Enumeration

A subgraph enumerator takes a *host graph* rather than a vertex count and
generates the subgraphs of it that belong to the class:

```bash
# C4: every edge subset but C4 itself is chordal, so 15
printf '4 4\n1 2\n2 3\n3 4\n4 1\n' | ./bin/chordal_subgraph_enum
# Output: count on first line, then edge lists
```

This is the problem Kiyomi and Uno actually state; `chordal_labeled_enum` is its
`G = K_n` case. It is the same reverse search with child generation filtered by
host adjacency, which is correct because the parent rule only deletes edges, so
the subgraphs of a fixed host are closed under it.

Outputs are spanning subgraphs: the vertex set is fixed and isolated vertices
are kept, so they are in bijection with the chordal edge subsets of the host
and the empty edge set is always among them. The count is driven by the edge
count rather than the vertex count -- every subgraph of a forest is chordal, so
a host with `m` edges can reach `2^m` -- and
`enumerate_chordal_subgraphs_cb` is the streaming counterpart.

```cpp
#include "chordal_subgraph_enum.h"

graph_recognition::Graph host(4, {{1,2}, {2,3}, {3,4}, {4,1}});
auto result = graph_recognition::enumerate_chordal_subgraphs(host);
// result.graphs.size() == 15
```

### Induced Subgraph Enumeration

An induced subgraph enumerator also takes a *host graph*, but its solutions are
the vertex subsets `X` for which `G[X]` belongs to the class:

```bash
# C6: every vertex subset but the whole cycle is chordal bipartite, so 63
printf '6 6\n1 2\n2 3\n3 4\n4 5\n5 6\n6 1\n' \
  | ./bin/chordal_bipartite_induced_subgraph_enum
# Output: count on first line, then vertex lists
```

The search is the ECB reverse search of Kurita, Wasa, Arimura and Uno, built on
their characterization: a graph is chordal bipartite exactly when it can be
emptied by repeatedly removing a *weak-simplicial* vertex, one whose
neighborhood is independent and totally ordered by inclusion.

An induced subgraph is determined by its vertex set, so the output is a family
of sorted vertex lists rather than edge lists, and the empty set always comes
first. The count is driven by the vertex count rather than the edge count --
the class is hereditary, so a host that is itself chordal bipartite reaches
`2^n` however sparse it is -- and
`enumerate_chordal_bipartite_induced_subgraphs_cb` is the streaming
counterpart.

```cpp
#include "chordal_bipartite_induced_subgraph_enum.h"

graph_recognition::Graph host(6, {{1,2}, {2,3}, {3,4}, {4,5}, {5,6}, {6,1}});
auto result =
    graph_recognition::enumerate_chordal_bipartite_induced_subgraphs(host);
// result.vertex_sets.size() == 63
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

### Decompositions

The structures the recognizers compute are available on their own, not only
as a yes/no answer. See the utilities section of the API documentation for
the full list.

```cpp
#include "modular_decomposition.h"
#include "split_decomposition.h"

graph_recognition::Graph g(4, {{1,2}, {2,3}, {3,4}});

// P4 has no non-trivial module: one PRIME node over four leaves
auto tree = graph_recognition::modular_decomposition(g);

// distance-hereditary iff no bag of the split decomposition is prime
auto sd = graph_recognition::split_decomposition(g);
if (sd.totally_decomposable) { /* ... */ }
```

Recognizers that build a structure on the way to their answer now report it:
`SplitResult::side` is the (K, S) partition, `ThresholdResult::creation_order`
the creation sequence, `LineGraphResult::root_graph` a graph whose line graph
is the input, and so on. Where building the structure costs more than the
recognition does, it lives in a separate builder instead -- `build_cotree`,
`build_pruning_sequence`, `build_clique_tree`.

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

## References

Citation keys used in the [Supported Graph Classes](#supported-graph-classes)
tables. `Rec:` entries are the characterization or algorithm the recognizer
follows; `Enum:` entries are the same for the enumerator. Most enumerators are
reverse-search enumerators in the sense of [Avis+ 96], driven by the
corresponding recognizer as the membership filter. Per-class discussion is in
the [class documentation](https://junkawahara.github.io/ai-coded-graph-recognition-test/en).

- **[Abbas+ 00]** N. Abbas, L. K. Stewart. "Biconvex graphs: ordering and algorithms." *Discrete Applied Mathematics*, 103(1–3):1–19, 2000. [DOI:10.1016/S0166-218X(99)00217-6](https://doi.org/10.1016/S0166-218X(99)00217-6)
- **[Avis+ 96]** D. Avis, K. Fukuda. "Reverse search for enumeration." *Discrete Applied Mathematics*, 65(1–3):21–46, 1996. [DOI:10.1016/0166-218X(95)00026-N](https://doi.org/10.1016/0166-218X(95)00026-N)
- **[Bandelt+ 86]** H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs." *Journal of Combinatorial Theory, Series B*, 41(2):182–208, 1986. [DOI:10.1016/0095-8956(86)90043-2](https://doi.org/10.1016/0095-8956(86)90043-2)
- **[Beineke+ 69]** L. W. Beineke, R. E. Pippert. "The number of labeled k-dimensional trees." *Journal of Combinatorial Theory*, 6(2):200–205, 1969. [DOI:10.1016/S0021-9800(69)80120-1](https://doi.org/10.1016/S0021-9800(69)80120-1)
- **[Beyer+ 80]** T. Beyer, S. M. Hedetniemi. "Constant time generation of rooted trees." *SIAM Journal on Computing*, 9(4):706–712, 1980. [DOI:10.1137/0209055](https://doi.org/10.1137/0209055)
- **[Booth+ 76]** K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms." *Journal of Computer and System Sciences*, 13(3):335–379, 1976. [DOI:10.1016/S0022-0000(76)80045-1](https://doi.org/10.1016/S0022-0000(76)80045-1)
- **[Bose 63]** R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs." *Pacific Journal of Mathematics*, 13(2):389–419, 1963. [DOI:10.2140/pjm.1963.13.389](https://doi.org/10.2140/pjm.1963.13.389)
- **[Brandes 09]** U. Brandes. "The left-right planarity test." Manuscript, University of Konstanz, 2009.
- **[Brandstädt+ 99]** A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.* SIAM Monographs on Discrete Mathematics and Applications, 1999. [DOI:10.1137/1.9780898719796](https://doi.org/10.1137/1.9780898719796)
- **[Brandstädt+ 06]** A. Brandstädt, V. B. Le. "Structure and linear time recognition of 3-leaf powers." *Information Processing Letters*, 98(4):133–138, 2006. [DOI:10.1016/j.ipl.2006.01.004](https://doi.org/10.1016/j.ipl.2006.01.004)
- **[Brandstädt+ 08]** A. Brandstädt, V. B. Le, R. Sritharan. "Structure and linear-time recognition of 4-leaf powers." *ACM Transactions on Algorithms*, 5(1):11:1–11:22, 2008. [DOI:10.1145/1435375.1435386](https://doi.org/10.1145/1435375.1435386)
- **[Brinkmann+ 02]** G. Brinkmann, B. D. McKay. "Posets on up to 16 points." *Order*, 19(2):147–179, 2002. [DOI:10.1023/A:1016543307592](https://doi.org/10.1023/A:1016543307592)
- **[Burlet+ 84a]** M. Burlet, J. Fonlupt. "Polynomial algorithm to recognize a Meyniel graph." *Annals of Discrete Mathematics*, 21:225–252, 1984. [DOI:10.1016/S0304-0208(08)72938-4](https://doi.org/10.1016/S0304-0208(08)72938-4)
- **[Burlet+ 84b]** M. Burlet, J. P. Uhry. "Parity graphs." *Annals of Discrete Mathematics*, 21:253–277, 1984. [DOI:10.1016/S0304-0208(08)72939-6](https://doi.org/10.1016/S0304-0208(08)72939-6)
- **[Chang+ 07]** M.-S. Chang, M.-T. Ko. "The 3-Steiner root problem." *Proceedings of WG 2007*, Lecture Notes in Computer Science 4769, pp. 109–120, 2007. [DOI:10.1007/978-3-540-74839-7_11](https://doi.org/10.1007/978-3-540-74839-7_11)
- **[Chartrand+ 67]** G. Chartrand, F. Harary. "Planar permutation graphs." *Annales de l'Institut Henri Poincaré B*, 3(4):433–438, 1967.
- **[Cheng+ 16]** C. Cheng, K. L. Collins, A. N. Trenk. "Split graphs and Nordhaus–Gaddum graphs." *Discrete Mathematics*, 339(9):2345–2356, 2016. [DOI:10.1016/j.disc.2016.04.001](https://doi.org/10.1016/j.disc.2016.04.001)
- **[Chudnovsky 12]** M. Chudnovsky. "The structure of bull-free graphs I — Three-edge-paths with centers and anticenters." *Journal of Combinatorial Theory, Series B*, 102(1):233–251, 2012. [DOI:10.1016/j.jctb.2011.07.003](https://doi.org/10.1016/j.jctb.2011.07.003)
- **[Chudnovsky+ 06]** M. Chudnovsky, N. Robertson, P. Seymour, R. Thomas. "The strong perfect graph theorem." *Annals of Mathematics*, 164(1):51–229, 2006. [DOI:10.4007/annals.2006.164.51](https://doi.org/10.4007/annals.2006.164.51)
- **[Chudnovsky+ 20]** M. Chudnovsky, A. Scott, P. Seymour, S. Spirkl. "Detecting an odd hole." *Journal of the ACM*, 67(1):5:1–5:12, 2020. [DOI:10.1145/3375720](https://doi.org/10.1145/3375720)
- **[Chvátal+ 77]** V. Chvátal, P. L. Hammer. "Aggregation of inequalities in integer programming." *Annals of Discrete Mathematics*, 1:145–162, 1977. [DOI:10.1016/S0167-5060(08)70731-3](https://doi.org/10.1016/S0167-5060(08)70731-3)
- **[Cogis 82]** O. Cogis. "On the Ferrers dimension of a digraph." *Discrete Mathematics*, 38(1):47–52, 1982. [DOI:10.1016/0012-365X(82)90167-4](https://doi.org/10.1016/0012-365X(82)90167-4)
- **[Conforti+ 02]** M. Conforti, G. Cornuéjols, A. Kapoor, K. Vušković. "Even-hole-free graphs, Part I: Decomposition theorem." *Journal of Graph Theory*, 39(1):6–49, 2002. [DOI:10.1002/jgt.10006](https://doi.org/10.1002/jgt.10006)
- **[Corneil+ 81]** D. G. Corneil, H. Lerchs, L. Stewart Burlingham. "Complement reducible graphs." *Discrete Applied Mathematics*, 3(3):163–174, 1981. [DOI:10.1016/0166-218X(81)90013-5](https://doi.org/10.1016/0166-218X(81)90013-5)
- **[Corneil+ 85]** D. G. Corneil, Y. Perl, L. K. Stewart. "A linear recognition algorithm for cographs." *SIAM Journal on Computing*, 14(4):926–934, 1985. [DOI:10.1137/0214065](https://doi.org/10.1137/0214065)
- **[Corneil+ 97]** D. G. Corneil, S. Olariu, L. Stewart. "Asteroidal triple-free graphs." *SIAM Journal on Discrete Mathematics*, 10(3):399–430, 1997. [DOI:10.1137/S0895480193250125](https://doi.org/10.1137/S0895480193250125)
- **[Dagan+ 88]** I. Dagan, M. C. Golumbic, R. Y. Pinter. "Trapezoid graphs and their coloring." *Discrete Applied Mathematics*, 21(1):35–46, 1988. [DOI:10.1016/0166-218X(88)90032-7](https://doi.org/10.1016/0166-218X(88)90032-7)
- **[Dahlhaus+ 87]** E. Dahlhaus, P. Duchet. "On strongly chordal graphs." *Ars Combinatoria*, 24B:23–30, 1987.
- **[de Fraysseix+ 06]** H. de Fraysseix, P. Ossona de Mendez, P. Rosenstiehl. "Trémaux trees and planarity." *International Journal of Foundations of Computer Science*, 17(5):1017–1029, 2006. [DOI:10.1142/S0129054106004248](https://doi.org/10.1142/S0129054106004248)
- **[Dom+ 06]** M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems." *Algorithmica*, 44(4):363–381, 2006. [DOI:10.1007/s00453-005-1180-z](https://doi.org/10.1007/s00453-005-1180-z)
- **[Duffin 65]** R. J. Duffin. "Topology of series-parallel networks." *Journal of Mathematical Analysis and Applications*, 10(2):303–318, 1965. [DOI:10.1016/0022-247X(65)90125-3](https://doi.org/10.1016/0022-247X(65)90125-3)
- **[Euler 1741]** L. Euler. "Solutio problematis ad geometriam situs pertinentis." *Commentarii Academiae Scientiarum Petropolitanae*, 8:128–140, 1741 (presented 1736).
- **[Farber 83]** M. Farber. "Characterizations of strongly chordal graphs." *Discrete Mathematics*, 43(2–3):173–189, 1983. [DOI:10.1016/0012-365X(83)90154-1](https://doi.org/10.1016/0012-365X(83)90154-1)
- **[Faudree+ 97]** R. Faudree, E. Flandrin, Z. Ryjáček. "Claw-free graphs — A survey." *Discrete Mathematics*, 164(1–3):87–147, 1997. [DOI:10.1016/S0012-365X(96)00045-3](https://doi.org/10.1016/S0012-365X(96)00045-3)
- **[Földes+ 77]** S. Földes, P. L. Hammer. "Split graphs." *Congressus Numerantium*, 19:311–315, 1977.
- **[Fulkerson+ 65]** D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs." *Pacific Journal of Mathematics*, 15(3):835–855, 1965. [DOI:10.2140/pjm.1965.15.835](https://doi.org/10.2140/pjm.1965.15.835)
- **[Gallai 67]** T. Gallai. "Transitiv orientierbare Graphen." *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1–2):25–66, 1967. [DOI:10.1007/BF02020961](https://doi.org/10.1007/BF02020961)
- **[Gasse 97]** E. Gasse. "A proof of a circle graph characterization." *Discrete Mathematics*, 173(1–3):277–283, 1997. [DOI:10.1016/S0012-365X(97)00068-X](https://doi.org/10.1016/S0012-365X(97)00068-X)
- **[Geelen+ 20]** J. Geelen, E. Lee. "Naji's characterization of circle graphs." *Journal of Graph Theory*, 93(1):21–33, 2020. [DOI:10.1002/jgt.22466](https://doi.org/10.1002/jgt.22466)
- **[Golumbic 78]** M. C. Golumbic. "Trivially perfect graphs." *Discrete Mathematics*, 24(1):105–107, 1978. [DOI:10.1016/0012-365X(78)90178-4](https://doi.org/10.1016/0012-365X(78)90178-4)
- **[Golumbic 80]** M. C. Golumbic. *Algorithmic Graph Theory and Perfect Graphs.* Academic Press, 1980; 2nd edition, Annals of Discrete Mathematics 57, Elsevier, 2004. [DOI:10.1016/S0167-5060(04)80053-0](https://doi.org/10.1016/S0167-5060(04)80053-0)
- **[Golumbic+ 78]** M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs." *Journal of Graph Theory*, 2(2):155–163, 1978. [DOI:10.1002/jgt.3190020209](https://doi.org/10.1002/jgt.3190020209)
- **[Halin 71]** R. Halin. "Studies on minimally n-connected graphs." In D. J. A. Welsh (ed.), *Combinatorial Mathematics and its Applications*, Academic Press, pp. 129–136, 1971.
- **[Hammer+ 81]** P. L. Hammer, B. Simeone. "The splittance of a graph." *Combinatorica*, 1(3):275–284, 1981. [DOI:10.1007/BF02579333](https://doi.org/10.1007/BF02579333)
- **[Harary 63]** F. Harary. "A characterization of block-graphs." *Canadian Mathematical Bulletin*, 6(1):1–6, 1963. [DOI:10.4153/CMB-1963-001-x](https://doi.org/10.4153/CMB-1963-001-x)
- **[Harary+ 53]** F. Harary, G. E. Uhlenbeck. "On the number of Husimi trees, I." *Proceedings of the National Academy of Sciences*, 39(4):315–322, 1953. [DOI:10.1073/pnas.39.4.315](https://doi.org/10.1073/pnas.39.4.315)
- **[Harary+ 73]** F. Harary, A. J. Schwenk. "The number of caterpillars." *Discrete Mathematics*, 6(4):359–365, 1973. [DOI:10.1016/0012-365X(73)90067-8](https://doi.org/10.1016/0012-365X(73)90067-8)
- **[Hayward 85]** R. B. Hayward. "Weakly triangulated graphs." *Journal of Combinatorial Theory, Series B*, 39(3):200–208, 1985. [DOI:10.1016/0095-8956(85)90050-4](https://doi.org/10.1016/0095-8956(85)90050-4)
- **[Hayward 96]** R. B. Hayward. "Generating weakly triangulated graphs." *Journal of Graph Theory*, 21(1):67–69, 1996. [DOI:10.1002/(SICI)1097-0118(199601)21:1%3C67::AID-JGT9%3E3.0.CO;2-K](https://doi.org/10.1002/(SICI)1097-0118(199601)21:1%3C67::AID-JGT9%3E3.0.CO;2-K)
- **[Hopcroft+ 73]** J. Hopcroft, R. Tarjan. "Dividing a graph into triconnected components." *SIAM Journal on Computing*, 2(3):135–158, 1973. [DOI:10.1137/0202012](https://doi.org/10.1137/0202012)
- **[Howorka 77]** E. Howorka. "A characterization of distance-hereditary graphs." *The Quarterly Journal of Mathematics*, 28(4):417–420, 1977. [DOI:10.1093/qmath/28.4.417](https://doi.org/10.1093/qmath/28.4.417)
- **[Howorka 81]** E. Howorka. "A characterization of Ptolemaic graphs." *Journal of Graph Theory*, 5(3):323–331, 1981. [DOI:10.1002/jgt.3190050314](https://doi.org/10.1002/jgt.3190050314)
- **[Isaacs 75]** R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable." *The American Mathematical Monthly*, 82(3):221–239, 1975. [DOI:10.2307/2319844](https://doi.org/10.2307/2319844)
- **[Itai+ 78]** A. Itai, M. Rodeh. "Finding a minimum circuit in a graph." *SIAM Journal on Computing*, 7(4):413–423, 1978. [DOI:10.1137/0207033](https://doi.org/10.1137/0207033)
- **[Jacobs+ 97]** D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game." *Journal of Computational Physics*, 137(2):346–365, 1997. [DOI:10.1006/jcph.1997.5809](https://doi.org/10.1006/jcph.1997.5809)
- **[Jones+ 18]** Á. A. Jones, F. Protti, R. R. Del-Vecchio. "Cograph generation with linear delay." *Theoretical Computer Science*, 713:1–10, 2018. [DOI:10.1016/j.tcs.2017.12.037](https://doi.org/10.1016/j.tcs.2017.12.037)
- **[Kiyomi+ 06]** M. Kiyomi, T. Uno. "Generating chordal graphs included in given graphs." *IEICE Transactions on Information and Systems*, E89-D(2):763–770, 2006. [DOI:10.1093/ietisy/e89-d.2.763](https://doi.org/10.1093/ietisy/e89-d.2.763)
- **[Kiyomi-Kijima+ 06]** M. Kiyomi, S. Kijima, T. Uno. "Listing chordal graphs and interval graphs." *Graph-Theoretic Concepts in Computer Science (WG 2006)*, LNCS 4271:68–77, 2006. [DOI:10.1007/11917496_7](https://doi.org/10.1007/11917496_7)
- **[Kiyomi thesis 06]** M. Kiyomi. *Studies on Subgraph and Supergraph Enumeration Algorithms.* Ph.D. thesis, The Graduate University for Advanced Studies, 2006, Section 4.1.5. [PDF](https://www.nii.ac.jp/graduate/wp-content/themes/nii_original/assets/pdf/students_thesis/18/kiyomi_Dr_thesis.pdf)
- **[König 36]** D. König. *Theorie der endlichen und unendlichen Graphen.* Akademische Verlagsgesellschaft, Leipzig, 1936.
- **[Krausz 43]** J. Krausz. "Démonstration nouvelle d'un théorème de Whitney sur les réseaux." *Matematikai és Fizikai Lapok*, 50:75–85, 1943.
- **[Kuratowski 30]** K. Kuratowski. "Sur le problème des courbes gauches en topologie." *Fundamenta Mathematicae*, 15(1):271–283, 1930. [DOI:10.4064/fm-15-1-271-283](https://doi.org/10.4064/fm-15-1-271-283)
- **[Kurita+ 19]** K. Kurita, K. Wasa, H. Arimura, T. Uno. "An Efficient Algorithm for Enumerating Chordal Bipartite Induced Subgraphs in Sparse Graphs." *Computing and Combinatorics (COCOON 2019)*, LNCS 11653:339–351, 2019. [DOI:10.1007/978-3-030-26176-4_28](https://doi.org/10.1007/978-3-030-26176-4_28)
- **[Lafond 22]** M. Lafond. "Recognizing k-leaf powers in polynomial time, for constant k." *Proceedings of SODA 2022*, pp. 1384–1410, 2022. [DOI:10.1137/1.9781611977073.58](https://doi.org/10.1137/1.9781611977073.58)
- **[Laman 70]** G. Laman. "On graphs and rigidity of plane skeletal structures." *Journal of Engineering Mathematics*, 4(4):331–340, 1970. [DOI:10.1007/BF01534980](https://doi.org/10.1007/BF01534980)
- **[Lekkerkerker+ 62]** C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line." *Fundamenta Mathematicae*, 51(1):45–64, 1962. [DOI:10.4064/fm-51-1-45-64](https://doi.org/10.4064/fm-51-1-45-64)
- **[Mahadev+ 95]** N. V. R. Mahadev, U. N. Peled. *Threshold Graphs and Related Topics.* Annals of Discrete Mathematics 56, North-Holland, 1995.
- **[McConnell 03]** R. M. McConnell. "Linear-time recognition of circular-arc graphs." *Algorithmica*, 37(2):93–147, 2003. [DOI:10.1007/s00453-003-1032-7](https://doi.org/10.1007/s00453-003-1032-7)
- **[McKay 98]** B. D. McKay. "Isomorph-free exhaustive generation." *Journal of Algorithms*, 26(2):306–324, 1998. [DOI:10.1006/jagm.1997.0898](https://doi.org/10.1006/jagm.1997.0898)
- **[Meringer 99]** M. Meringer. "Fast generation of regular graphs and construction of cages." *Journal of Graph Theory*, 30(2):137–146, 1999. [DOI:10.1002/(SICI)1097-0118(199902)30:2<137::AID-JGT7>3.0.CO;2-G](https://doi.org/10.1002/(SICI)1097-0118(199902)30:2%3C137::AID-JGT7%3E3.0.CO;2-G)
- **[Meyniel 76]** H. Meyniel. "On the perfect graph conjecture." *Discrete Mathematics*, 16(4):339–342, 1976. [DOI:10.1016/S0012-365X(76)80008-8](https://doi.org/10.1016/S0012-365X(76)80008-8)
- **[Moon 68]** J. W. Moon. *Topics on Tournaments.* Holt, Rinehart and Winston, New York, 1968.
- **[Naji 85]** W. Naji. "Reconnaissance des graphes de cordes." *Discrete Mathematics*, 54(3):329–337, 1985. [DOI:10.1016/0012-365X(85)90117-7](https://doi.org/10.1016/0012-365X(85)90117-7)
- **[Nishimura+ 02]** N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees." *Journal of Algorithms*, 42(1):69–108, 2002. [DOI:10.1006/jagm.2001.1195](https://doi.org/10.1006/jagm.2001.1195)
- **[Paul+ 24]** C. Paul, E. Protopapas. "Proper chordal graphs." *Proceedings of STACS 2024*, LIPIcs 289, 53:1–53:17, 2024. [DOI:10.4230/LIPIcs.STACS.2024.53](https://doi.org/10.4230/LIPIcs.STACS.2024.53)
- **[Petersen 1891]** J. Petersen. "Die Theorie der regulären Graphs." *Acta Mathematica*, 15:193–220, 1891. [DOI:10.1007/BF02392606](https://doi.org/10.1007/BF02392606)
- **[Pnueli+ 71]** A. Pnueli, A. Lempel, S. Even. "Transitive orientation of graphs and identification of permutation graphs." *Canadian Journal of Mathematics*, 23(1):160–175, 1971. [DOI:10.4153/CJM-1971-016-5](https://doi.org/10.4153/CJM-1971-016-5)
- **[Ringel 63]** G. Ringel. "Selbstkomplementäre Graphen." *Archiv der Mathematik*, 14(1):354–358, 1963. [DOI:10.1007/BF01234967](https://doi.org/10.1007/BF01234967)
- **[Roberts 69]** F. S. Roberts. "Indifference graphs." In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139–146, 1969.
- **[Robertson+ 95]** N. Robertson, P. D. Seymour. "Graph minors. XIII. The disjoint paths problem." *Journal of Combinatorial Theory, Series B*, 63(1):65–110, 1995. [DOI:10.1006/jctb.1995.1006](https://doi.org/10.1006/jctb.1995.1006)
- **[Rose 74]** D. J. Rose. "On simple characterizations of k-trees." *Discrete Mathematics*, 7(3–4):317–322, 1974. [DOI:10.1016/0012-365X(74)90042-9](https://doi.org/10.1016/0012-365X(74)90042-9)
- **[Rose+ 76]** D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs." *SIAM Journal on Computing*, 5(2):266–283, 1976. [DOI:10.1137/0205021](https://doi.org/10.1137/0205021)
- **[Sachs 62]** H. Sachs. "Über selbstkomplementäre Graphen." *Publicationes Mathematicae Debrecen*, 9:270–288, 1962.
- **[Saitoh+ 10]** T. Saitoh, K. Yamanaka, M. Kiyomi, R. Uehara. "Random Generation and Enumeration of Proper Interval Graphs." *IEICE Transactions on Information and Systems*, E93-D(7):1816–1823, 2010. [DOI:10.1587/transinf.E93.D.1816](https://doi.org/10.1587/transinf.E93.D.1816)
- **[Seinsche 74]** D. Seinsche. "On a property of the class of n-colorable graphs." *Journal of Combinatorial Theory, Series B*, 16(2):191–193, 1974. [DOI:10.1016/0095-8956(74)90063-X](https://doi.org/10.1016/0095-8956(74)90063-X)
- **[Spinrad+ 87]** J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs." *Discrete Applied Mathematics*, 18(3):279–292, 1987. [DOI:10.1016/S0166-218X(87)80003-3](https://doi.org/10.1016/S0166-218X(87)80003-3)
- **[Spinrad+ 95]** J. P. Spinrad, R. Sritharan. "Algorithms for weakly triangulated graphs." *Discrete Applied Mathematics*, 59(2):181–191, 1995. [DOI:10.1016/0166-218X(93)E0161-Q](https://doi.org/10.1016/0166-218X(93)E0161-Q)
- **[Steinitz 22]** E. Steinitz. "Polyeder und Raumeinteilungen." *Encyklopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.
- **[Tarjan 72]** R. Tarjan. "Depth-first search and linear graph algorithms." *SIAM Journal on Computing*, 1(2):146–160, 1972. [DOI:10.1137/0201010](https://doi.org/10.1137/0201010)
- **[Tarjan+ 84]** R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs." *SIAM Journal on Computing*, 13(3):566–579, 1984. [DOI:10.1137/0213035](https://doi.org/10.1137/0213035)
- **[Troyka 19]** J. M. Troyka. "Split graphs: combinatorial species and asymptotics." *Electronic Journal of Combinatorics*, 26(2):P2.42, 2019. [arXiv:1803.07248](https://arxiv.org/abs/1803.07248)
- **[Tucker 74]** A. Tucker. "Structure theorems for some circular-arc graphs." *Discrete Mathematics*, 7(1–2):167–195, 1974. [DOI:10.1016/S0012-365X(74)80027-0](https://doi.org/10.1016/S0012-365X(74)80027-0)
- **[Tucker 80]** A. Tucker. "An efficient test for circular-arc graphs." *SIAM Journal on Computing*, 9(1):1–24, 1980. [DOI:10.1137/0209001](https://doi.org/10.1137/0209001)
- **[Valdes+ 82]** J. Valdes, R. E. Tarjan, E. L. Lawler. "The recognition of series parallel digraphs." *SIAM Journal on Computing*, 11(2):298–313, 1982. [DOI:10.1137/0211023](https://doi.org/10.1137/0211023)
- **[Whitney 32]** H. Whitney. "Congruent graphs and the connectivity of graphs." *American Journal of Mathematics*, 54(1):150–168, 1932. [DOI:10.2307/2371086](https://doi.org/10.2307/2371086)
- **[Wolk 62]** E. S. Wolk. "The comparability graph of a tree." *Proceedings of the American Mathematical Society*, 13(5):789–795, 1962. [DOI:10.1090/S0002-9939-1962-0172273-0](https://doi.org/10.1090/S0002-9939-1962-0172273-0)
- **[Wright+ 86]** R. A. Wright, B. Richmond, A. Odlyzko, B. D. McKay. "Constant time generation of free trees." *SIAM Journal on Computing*, 15(2):540–548, 1986. [DOI:10.1137/0215039](https://doi.org/10.1137/0215039)
- **[Yan+ 96]** J.-H. Yan, J.-J. Chen, G. J. Chang. "Quasi-threshold graphs." *Discrete Applied Mathematics*, 69(3):247–255, 1996. [DOI:10.1016/0166-218X(96)00094-7](https://doi.org/10.1016/0166-218X(96)00094-7)
- **[Yannakakis 82]** M. Yannakakis. "The complexity of the partial order dimension problem." *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351–358, 1982. [DOI:10.1137/0603036](https://doi.org/10.1137/0603036)

## License

[MIT](LICENSE)
