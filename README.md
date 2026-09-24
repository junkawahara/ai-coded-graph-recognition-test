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
- **81 recognizers** with multiple algorithm variants (YES/NO + certificates)
- **116 enumerators** that generate all graphs of a given class on n vertices (labeled for most classes; some, e.g. tree/forest/caterpillar/halin/fullerene/chain/cochain/threshold/unicyclic/simple quadrangulation/proper interval/trivially perfect/cograph/cluster/triangle-free/bipartite/permutation/circle/eulerian/biconnected/chordal/co-chordal/cubic/k-regular/snark/Laman/split/planar/self-complementary/distance-hereditary/Ptolemaic/3-leaf power/interval/co-interval/outerplanar/series-parallel/cactus/bipartite permutation/maximal outerplanar/Apollonian network/partial k-tree/k-degenerate/cage/maximal planar/polyhedral/cubic planar/tournament/digraph/poset/strongly regular/comparability/co-comparability/block (unlabeled), enumerate non-isomorphic graphs)
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
| Chordal | `chordal.h` | O(n+m) | Rec: [Fulkerson+ 65], [Rose+ 76], [Tarjan+ 84]<br>Enum (labeled): [Kiyomi+ 06]<br>Enum (unlabeled): [McKay 98]<br>Enum (subgraph): [Kiyomi+ 06] | No induced cycle of length >= 4 |
| Strongly chordal | `strongly_chordal.h` | O(n² + nmΔ) | Rec: [Farber 83], [Dahlhaus+ 87]<br>Enum (labeled): [Kiyomi thesis 06] | Chordal + every even cycle (>= 6) has an odd chord |
| Proper chordal | `proper_chordal.h` | *exponential* | Rec: [Paul+ 24]<br>Enum (labeled): indifference-layout edge reverse search (based on [Paul+ 24]) | Chordal + admits indifference tree-layout |
| Split | `split.h` | O(n+m) | Rec: [Földes+ 77], [Hammer+ 81]<br>Enum (labeled): [Cheng+ 16], [Troyka 19]<br>Enum (unlabeled): [McKay 98] | Vertices partition into a clique and an independent set |
| Threshold | `threshold.h` | O(n+m) | Rec: [Chvátal+ 77], [Mahadev+ 95]<br>Enum (unlabeled): [Chvátal+ 77] | Iteratively removable isolated or universal vertices |
| Weakly chordal | `weakly_chordal.h` | O(n⁶) | Rec: [Hayward 85], [Spinrad+ 95]<br>Enum (labeled): [Kiyomi thesis 06] | No induced cycle of length >= 5 in G or complement(G) |
| Block | `block.h` | O(n+m) | Rec: [Harary 63], [Tarjan 72]<br>Enum (labeled): [Ekim+ 24] (weighted block trees + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Ekim+ 24] | Every biconnected component is a clique |
| Ptolemaic | `ptolemaic.h` | O(n³ log n) | Rec: [Howorka 81]<br>Enum (labeled): [Bandelt+ 86] (chordality-preserving pendant/twin extensions + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Bandelt+ 86] | Chordal + distance-hereditary |
| Trivially perfect | `trivially_perfect.h` | O(n(n+m)) | Rec: [Wolk 62], [Golumbic 78]<br>Enum (labeled): [Golumbic 78]<br>Enum (unlabeled): [Wolk 62], [Beyer+ 80] | Chordal + cograph (= quasi-threshold) |
| Quasi-threshold | `quasi_threshold.h` | O(n(n+m)) | Rec: [Wolk 62], [Yan+ 96]<br>Enum: — | Another name for trivially perfect (thin wrapper) |
| k-tree | `ktree.h` | O(n² + nk²) | Rec: [Rose 74]<br>Enum (labeled): [Beineke+ 69], [Avis+ 96] | Graphs built from K_{k+1} by repeatedly attaching vertices to k-cliques |
| Partial k-tree | `partial_ktree.h` | *exponential* | Rec: [Arnborg+ 87], [Bodlaender+ 12]<br>Enum (unlabeled): [Dinneen 97], [McKay 98] | Subgraphs of k-trees = treewidth <= k (not chordal in general; k is an input; NP-complete for k in the input) |
| k-degenerate | `degenerate.h` | O(n + m) | Rec: [Lick+ 70], [Matula+ 83]<br>Enum (unlabeled): [Bauer+ 10], [McKay 98] | Every induced subgraph has a vertex of degree <= k (k is an input; the exact degeneracy is reported either way, with the (k+1)-core as NO certificate) |

### Interval / Circular-Arc Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Interval | `interval.h` | O(n³) | Rec: [Lekkerkerker+ 62]<br>Enum (labeled): [Kiyomi-Kijima+ 06]<br>Enum (unlabeled): [Mikos 21] (MPQ-tree edge deletion; legacy [McKay 98] selectable) | Intersection graph of intervals on the real line |
| Proper interval | `proper_interval.h` | O(n³ + nΔ³) | Rec: [Roberts 69]<br>Enum (labeled): [Saitoh+ 10] (bracket strings + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Saitoh+ 10] | Interval graph with no containment between intervals |
| Unit interval | `unit_interval.h` | O(n³ + nΔ³) | Rec: [Roberts 69]<br>Enum: — | Equal-length intervals (= proper interval) |
| Co-interval | `co_interval.h` | O(n³) | Rec: [Lekkerkerker+ 62]<br>Enum (labeled): [Kiyomi-Kijima+ 06] (complements; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [McKay 98] | Complement is an interval graph |
| Circular-arc | `circular_arc.h` | *exponential* | Rec: [Tucker 80], [McConnell 03]<br>Enum (labeled): [Avis+ 96] | Intersection graph of arcs on a circle |
| Proper circular-arc | `proper_circular_arc.h` | *exponential* | Rec: [Tucker 74]<br>Enum (labeled): [Avis+ 96] | Circular-arc with no containment between arcs |

### Permutation / Comparability Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Permutation | `permutation.h` | O(n³) | Rec: [Pnueli+ 71], [Gallai 67]<br>Enum (labeled): [Yamazaki+ 20] (modular edge search + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Yamazaki+ 20] (legacy [McKay 98] selectable) | Both G and complement(G) are comparability graphs |
| Comparability | `comparability.h` | O(nm) | Rec: [Gallai 67], [Golumbic 80]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | Edges admit a transitive orientation |
| Co-comparability | `co_comparability.h` | O(n³) | Rec: [Gallai 67]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | Complement is a comparability graph |
| Bipartite permutation | `bipartite_permutation.h` | O(n³) | Rec: [Spinrad+ 87]<br>Enum (labeled): [Saitoh+ 12] (line strings + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Saitoh+ 12] (legacy [McKay 98] selectable) | Bipartite + permutation |
| Trapezoid | `trapezoid.h` | O(n⁴) | Rec: [Dagan+ 88], [Cogis 82]<br>Enum (labeled): [Avis+ 96] | Intersection graph of trapezoids between two parallel lines (generalises permutation) |

### Bipartite Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Bipartite | `bipartite.h` | O(n+m) | Rec: [König 36]<br>Enum (labeled): color-compatible vertex additions (legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [McKay 98] with color-compatible neighborhoods | 2-colorable (no odd cycle) |
| Chordal bipartite | `chordal_bipartite.h` | O(m²Δ²) | Rec: [Golumbic+ 78]<br>Enum (labeled): [Avis+ 96]<br>Enum (induced subgraph): [Kurita+ 19] | Bipartite + no induced cycle of length >= 6. Default algorithm allocates a Θ(n²) adjacency matrix; use CYCLE_CHECK for large sparse graphs |
| Chain | `chain.h` | O(n²) | Rec: [Yannakakis 82]<br>Enum (unlabeled): — | Bipartite + neighborhoods form a total order by inclusion |
| Co-chain | `cochain.h` | O(n³) | Rec: [Yannakakis 82]<br>Enum (unlabeled): — | Complement is a chain graph |
| Convex bipartite | `convex_bipartite.h` | O(nm) | Rec: [Booth+ 76]<br>Enum (labeled): [Avis+ 96] | Bipartite + one side has consecutive neighborhood property |
| Biconvex bipartite | `biconvex_bipartite.h` | O(nm) | Rec: [Abbas+ 00], [Booth+ 76]<br>Enum (labeled): [Avis+ 96] | Bipartite + both sides have consecutive neighborhood property |

### Planar Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Planar | `planar.h` | O(n + m log Δ) | Rec: [Kuratowski 30], [de Fraysseix+ 06], [Brandes 09]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | No K5 or K3,3 minor |
| Outerplanar | `outer_planar.h` | O(n+m) | Rec: [Chartrand+ 67], [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | No K4 or K2,3 minor |
| Maximal outerplanar | `maximal_outer_planar.h` | O(n+m) | Rec: [Chartrand+ 67], [de Fraysseix+ 06]<br>Enum (unlabeled): [Bodirsky+ 07] | Outerplanar graphs where no edge can be added (polygon triangulations; simple 2-trees) |
| Cactus | `cactus.h` | O(n+m) | Rec: [Harary+ 53], [Tarjan 72]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | Every biconnected component is a single edge or a simple cycle |
| Series-parallel | `series_parallel.h` | O(n+m) | Rec: [Duffin 65], [Valdes+ 82]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | No K4 minor (2-degenerate) |
| Apex | `apex.h` | O(n(n+m)) | Rec: [Robertson+ 95], [de Fraysseix+ 06]<br>Enum (labeled): [Avis+ 96] | Planar after removing one vertex |
| Maximal planar | `maximal_planar.h` | O(n+m) | Rec: [Kuratowski 30], [de Fraysseix+ 06]<br>Enum: [Brinkmann+ 07] (triangulation expansions; label expansion for labeled output; legacy [Avis+ 96] / [McKay 98] selectable) | Planar graphs where all faces are triangles |
| Apollonian network | `apollonian.h` | O(n+m) | Rec: [Kuratowski 30], [Tarjan+ 84]<br>Enum (unlabeled): [McKay 98], [Hering+ 82] | Chordal maximal planar graphs (planar 3-trees; stacked triangulations) |
| Cubic planar | `cubic_planar.h` | O(n+m) | Rec: [de Fraysseix+ 06]<br>Enum: [Brinkmann+ 07] (dual triangulations, component multisets and label expansion; legacy [Avis+ 96] / [McKay 98] selectable) | 3-regular planar graphs, including bridges and disconnected graphs |
| Polyhedral | `polyhedral.h` | O(n²(n+m)) | Rec: [Steinitz 22], [de Fraysseix+ 06]<br>Enum: [Brinkmann+ 07] (canonical edge deletions from triangulations; label expansion for labeled output; legacy [Avis+ 96] / [McKay 98] selectable) | 3-connected planar graphs (Steinitz's theorem) |
| Simple quadrangulation | `simple_quadrangulation.h` | O(n³) | Rec: [Steinitz 22], [de Fraysseix+ 06]<br>Enum (unlabeled): [Avis+ 96] | 3-connected planar graphs with all quadrilateral faces |
| Halin | `halin.h` | O(n⁴) | Rec: [Halin 71]<br>Enum (unlabeled): [Halin 71], [Wright+ 86] | Planar graph formed from a tree + outer cycle |
| Fullerene | `fullerene.h` | O(n⁴) | Rec: [de Fraysseix+ 06]<br>Enum (unlabeled): [Avis+ 96] | 3-regular planar graphs with pentagonal and hexagonal faces |

### Perfect / Structural Classes

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Perfect | `perfect.h` | *exponential* | Rec: [Chudnovsky+ 06]<br>Enum (labeled): [Avis+ 96] | No odd hole or odd antihole (SPGT) |
| Cograph | `cograph.h` | O(n(n+m)) | Rec: [Seinsche 74], [Corneil+ 81], [Corneil+ 85]<br>Enum (labeled): [Corneil+ 81]<br>Enum (unlabeled): [Jones+ 18] | No induced P4 |
| Distance-hereditary | `distance_hereditary.h` | O(n³) | Rec: [Howorka 77], [Bandelt+ 86]<br>Enum (labeled): [Bandelt+ 86] (pendant/twin extensions + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Bandelt+ 86] | Distances preserved in all connected induced subgraphs |
| AT-free | `at_free.h` | O(n³) | Rec: [Lekkerkerker+ 62], [Corneil+ 97]<br>Enum (labeled): [Avis+ 96] | No asteroidal triple |
| Co-chordal | `co_chordal.h` | O(n²) | Rec: [Fulkerson+ 65], [Tarjan+ 84]<br>Enum (labeled): [Kiyomi+ 06] (complements; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [McKay 98] | Complement is a chordal graph |
| Line graph | `line_graph.h` | *exponential* | Rec: [Whitney 32], [Krausz 43]<br>Enum (labeled): [Avis+ 96] | Edge-intersection graph of another graph |
| Circle | `circle.h` | polynomial | Rec: [Naji 85], [Gasse 97], [Geelen+ 20]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | Intersection graph of chords of a circle |
| Meyniel | `meyniel.h` | *exponential* | Rec: [Meyniel 76], [Burlet+ 84a]<br>Enum (labeled): [Avis+ 96] | Every odd cycle of length >= 5 has at least two chords |
| Parity | `parity.h` | *exponential* | Rec: [Burlet+ 84b]<br>Enum (labeled): [Avis+ 96] | Every two induced paths between same endpoints have same parity |
| Even-hole-free | `even_hole_free.h` | *exponential* | Rec: [Conforti+ 02]<br>Enum (labeled): [Avis+ 96] | No induced even cycle of length >= 4 |
| Odd-hole-free | `odd_hole_free.h` | *exponential* | Rec: [Chudnovsky+ 20]<br>Enum (labeled): [Avis+ 96] | No induced odd cycle of length >= 5 |
| Cluster | `cluster.h` | O(n+m) | Rec: [Brandstädt+ 99]<br>Enum (labeled): —<br>Enum (unlabeled): — | Disjoint union of complete graphs |
| Self-complementary | `self_complementary.h` | *exponential* | Rec: [Sachs 62], [Ringel 63]<br>Enum (labeled): [Sachs 62], [Ringel 63]<br>Enum (unlabeled): [Sachs 62], [Ringel 63] | Isomorphic to own complement |

### Forbidden Induced Subgraph Classes

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Claw-free | `claw_free.h` | O(nΔ³) | Rec: [Faudree+ 97]<br>Enum (labeled): [Avis+ 96] | No induced K1,3 |
| Diamond-free | `diamond_free.h` | O(n + m²) | Rec: [Brandstädt+ 99]<br>Enum (labeled): [Avis+ 96] | No induced K4 minus one edge |
| Triangle-free | `triangle_free.h` | O(mΔ) | Rec: [Itai+ 78]<br>Enum: [McKay 98] (independent-set additions; labeled adaptation) | No K3 |
| Bull-free | `bull_free.h` | O(mΔ³) | Rec: [Chudnovsky 12]<br>Enum (labeled): [Avis+ 96] | No induced bull graph |
| P5-free | `p5_free.h` | O(nΔ⁴) | Rec: [Brandstädt+ 99]<br>Enum (labeled): [Avis+ 96] | No induced path on 5 vertices |
| Gem-free | `gem_free.h` | O(nΔ⁴) | Rec: [Brandstädt+ 99]<br>Enum (labeled): [Avis+ 96] | No induced gem (fan) graph |

### Leaf Power Family

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| 3-leaf power | `three_leaf_power.h` | O(n + m log n) | Rec: [Nishimura+ 02], [Brandstädt+ 06], [Dom+ 06]<br>Enum (labeled): [Brandstädt+ 06], [Bandelt+ 86] (true-twin/restricted-pendant extensions + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Bandelt+ 86] | Leaf power with distance threshold 3 |
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
| Biconnected | `biconnected.h` | O(n+m) | Rec: [Tarjan 72]<br>Enum (labeled): [Stolee 11] (ear augmentation + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Stolee 11] (legacy [McKay 98] selectable) | 2-connected (at least 3 vertices, no cut vertex) |
| Triconnected | `triconnected.h` | O(n²(n+m)) | Rec: [Hopcroft+ 73]<br>Enum: — | 3-connected |
| Eulerian | `eulerian.h` | O(n) | Rec: [Euler 1741]<br>Enum (labeled): —<br>Enum (unlabeled): [McKay 98] | All vertices have even degree |
| k-regular | `kregular.h` | O(n) | Rec: —<br>Enum (labeled): [Meringer 99]<br>Enum (unlabeled): [Meringer 99], [McKay 98] | All vertices have degree k |
| Cubic | `cubic.h` | O(n) | Rec: [Petersen 1891]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98] | 3-regular graphs |
| (k,g)-graph / cage | `cage.h` | O(n(n+m)) (cage check *exponential*) | Rec: [Exoo+ 08], [Fu+ 97]<br>Enum (unlabeled): [Meringer 99], [McKay 98] | k-regular with girth >= g (k, g are inputs; YES = is a (k,g)-cage, verified minimal from the Moore bound; throws rather than returning a false negative if proof needs a search at order >=64) |
| Strongly regular | `strongly_regular.h` | O(n²Δ) | Rec: [Bose 63]<br>Enum (labeled): [Bose 63]<br>Enum (unlabeled): [McKay+ 01], [McKay 98] | Regular with uniform adjacency counts |
| Snark | `snark.h` | *exponential* | Rec: [Isaacs 75]<br>Enum (labeled): [Avis+ 96]<br>Enum (unlabeled): [McKay 98], [Brinkmann+ 13] | Cyclically 4-edge-connected cubic graphs of girth >= 5 with chromatic index 4 |
| Laman | `laman.h` | O(n²) | Rec: [Laman 70], [Jacobs+ 97]<br>Enum (labeled): [Taufiqurrachman+ Henneberg] (Henneberg I/II + label expansion; legacy [Avis+ 96] selectable)<br>Enum (unlabeled): [Taufiqurrachman+ Henneberg] (legacy [McKay 98] selectable) | Minimally rigid graphs in 2D |

### Directed Graph Classes

| Graph Class | Header | Recognition Complexity | References | Description |
|---|---|---|---|---|
| Tournament | `tournament.h` | O(n²) | Rec: [Moon 68]<br>Enum (labeled): —<br>Enum (unlabeled): [McKay 98] | Complete directed graphs (orientations of Kn) |
| Directed graph | `digraph.h` | O(m log m) | Rec: —<br>Enum (labeled): —<br>Enum (unlabeled): [McKay 98] | All simple directed graphs |
| Poset | `poset.h` | O(m(n+m)) | Rec: —<br>Enum (labeled): [Brinkmann+ 02]<br>Enum (unlabeled): [Brinkmann+ 02], [McKay 98] | Partially ordered sets (Hasse diagrams) |

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
#include "enumerators/chordal_labeled_enum.h"

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

The labeled **co-chordal** enumerator defaults to
`CoChordalLabeledEnumAlgorithm::KIYOMI_UNO`: it complements each graph from the
dedicated chordal enumeration on the same vertex labels. Complementation is a
bijection, so no additional recognition or duplicate elimination is needed.
Each complement takes O(n²) time to construct. Conversion happens inside the
chordal callback; the co-chordal API and CLI still retain all output graphs.

The previous vertex-addition search with co-chordal recognition remains
available as `CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH`. Both algorithms
produce the same labeled graphs, but their enumeration order differs:

```cpp
#include "enumerators/co_chordal_labeled_enum.h"

auto graphs = graph_recognition::enumerate_co_chordal_labeled_graphs_reverse_search(4);
auto legacy = graph_recognition::enumerate_co_chordal_labeled_graphs_reverse_search(
    4, graph_recognition::CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH);
// Both results contain 61 graphs.
```

The CLI accepts `--algo kiyomi_uno` (default) or `--algo reverse_search`:

```bash
echo "4" | ./bin/co_chordal_labeled_enum
echo "4" | ./bin/co_chordal_labeled_enum --algo reverse_search
```

In Python, use `enumerate_co_chordal_labeled_graphs(4)` for the default, or
`enumerate_co_chordal_labeled_graphs(4, algorithm="reverse_search")` for the
previous algorithm. See the [co-chordal documentation](docs/en/families/co_chordal.rst)
for details.

The interval enumerator likewise defaults to the class-specific reverse
search of Kiyomi, Kijima, and Uno. Starting from K_n, it deletes one edge at a
time and accepts only candidates whose canonical edge-addition parent is the
current graph. `IntervalLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER` retains the
former chordal-tree filtering implementation. A streaming counterpart is
available as `enumerate_interval_labeled_graphs_reverse_search_cb`.

The labeled **co-interval** enumerator defaults to
`CoIntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO`: it complements each graph
from that dedicated interval enumeration on the same vertex labels. This
bijection needs no additional recognition or duplicate elimination. Each
complement adds O(n²) time per output and is built inside the interval callback;
the co-interval API and CLI retain all resulting graphs. The interval paper's
per-output time bound does not carry over to this implementation.

The previous vertex-addition search with co-interval recognition remains
available as `REVERSE_SEARCH`. The two algorithms produce the same set of
labeled graphs in different orders:

```cpp
#include "enumerators/co_interval_labeled_enum.h"

auto graphs = graph_recognition::enumerate_co_interval_labeled_graphs_reverse_search(4);
auto legacy = graph_recognition::enumerate_co_interval_labeled_graphs_reverse_search(
    4, graph_recognition::CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
```

Both C++ choices accept `0 <= n <= 64`; `n = 0` yields one empty graph.
The CLI selects them with `--algo kiyomi_kijima_uno` (default) or `--algo reverse_search`:

```bash
echo "4" | ./bin/co_interval_labeled_enum
echo "4" | ./bin/co_interval_labeled_enum --algo reverse_search
```

In Python, use `enumerate_co_interval_labeled_graphs(4)` for the default, or
`enumerate_co_interval_labeled_graphs(4, algorithm="reverse_search")` for the
previous algorithm. See the [co-interval documentation](docs/en/families/co_interval.rst)
for details.

The labeled **proper interval** enumerator defaults to
`ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION`. It reuses the
Saitoh et al. bracket-string representation from the unlabeled enumerator,
including reflection symmetry and disconnected component composition, then
expands each representative into all distinct labelings. Adjacent label swaps
reach every permutation; a set of visited edge lists removes duplicates from
automorphisms, including twins and interchangeable components. Each
representative contributes exactly `n! / |Aut(G)|` labeled graphs.

The generator uses Dyck-word DFS, and the label expansion sorts edges and
checks the visited set for each swap. It retains the current representative's
distinct labelings in that set and materializes all output graphs. The
[paper's](https://doi.org/10.1587/transinf.E93.D.1816) optimized reverse search
and O(1) time per unlabeled representation are not implemented here.

The previous chordal-tree search with proper-interval recognition remains
available as `REVERSE_SEARCH`. The historical C++ function name is unchanged;
both choices return the same labeled graphs, including disconnected ones,
in different orders. `n = 0` yields one empty graph in C++ and the CLI.

```cpp
#include "enumerators/proper_interval_labeled_enum.h"

auto graphs = graph_recognition::enumerate_proper_interval_labeled_graphs_reverse_search(4);
auto legacy = graph_recognition::enumerate_proper_interval_labeled_graphs_reverse_search(
    4, graph_recognition::ProperIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
// Both results contain 57 graphs.
```

The CLI accepts `--algo string_representation` (default) or `--algo reverse_search`:

```bash
echo "4" | ./bin/proper_interval_labeled_enum
echo "4" | ./bin/proper_interval_labeled_enum --algo reverse_search
```

In Python, use `enumerate_proper_interval_labeled_graphs(4)` for the default,
or `enumerate_proper_interval_labeled_graphs(4, algorithm="reverse_search")`
for the previous algorithm. The existing Python range is `1 <= n <= 6`.
See the [proper interval documentation](docs/en/families/proper_interval.rst) for details.

The labeled **distance-hereditary** enumerator defaults to
`DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL`. It reuses the
unlabeled generator's pendant, true-twin and false-twin extensions, canonical
forms per size, and disconnected component composition. Each representative
is expanded into its `n! / |Aut(G)|` distinct labelings using the same adjacent
label swaps as proper interval enumeration. The default needs no recognition
calls. Fixing the extension order to increasing labels would miss graphs
whose largest-labeled vertex is neither pendant nor a twin.

```cpp
#include "enumerators/distance_hereditary_labeled_enum.h"

auto graphs = graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search(5);
auto legacy = graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search(
    5, graph_recognition::DistanceHereditaryLabeledEnumAlgorithm::REVERSE_SEARCH);
// Both results contain 892 graphs, in different orders.
```

The historical C++ function name is unchanged. C++ and the CLI accept
`0 <= n <= 63` for the default, or `0 <= n <= 64` for `REVERSE_SEARCH`, matching
their respective bitmask limits; `n = 0` yields one empty graph. The CLI accepts
`--algo vertex_incremental` (default) or `--algo reverse_search`:

```bash
echo "5" | ./bin/distance_hereditary_labeled_enum
echo "5" | ./bin/distance_hereditary_labeled_enum --algo reverse_search
```

In Python, use `enumerate_distance_hereditary_labeled_graphs(5)` for the default,
or pass `algorithm="vertex_incremental"` / `algorithm="reverse_search"` explicitly.
The existing Python range is `1 <= n <= 6`. The generator retains connected
representatives by size, a visited edge-set collection for the current label
orbit, and all output graphs. The DH-tree parent search and O(n³) delay for
non-isomorphic graphs with at most n vertices from
[Yamazaki–Qian–Uehara (2024)](https://doi.org/10.1016/j.dam.2023.09.002) are not
implemented here. See the [distance-hereditary documentation](docs/en/families/distance_hereditary.rst).

The labeled **Ptolemaic** enumerator defaults to
`PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL`. It reuses the structural
unlabeled generator: pendant and true-twin additions, and false-twin additions
only at simplicial vertices, preserving both chordality and distance heredity.
Disconnected graphs are composed from connected representatives. The same
label-expansion utility as distance-hereditary enumeration generates all
`n! / |Aut(G)|` distinct labelings of each representative, removing duplicates
from twins and interchangeable components. Restricting the additions to
increasing labels would miss graphs whose largest-labeled vertex is neither
pendant nor a twin.

The previous chordal-tree search with Ptolemaic recognition remains available
as `REVERSE_SEARCH`; the historical C++ function name is retained:

```cpp
#include "enumerators/ptolemaic_labeled_enum.h"

auto graphs = graph_recognition::enumerate_ptolemaic_labeled_graphs_reverse_search(5);
auto legacy = graph_recognition::enumerate_ptolemaic_labeled_graphs_reverse_search(
    5, graph_recognition::PtolemaicLabeledEnumAlgorithm::REVERSE_SEARCH);
// Both results contain 762 graphs, in different orders.
```

The CLI accepts `--algo vertex_incremental` (default) or `--algo reverse_search`:

```bash
echo "5" | ./bin/ptolemaic_labeled_enum
echo "5" | ./bin/ptolemaic_labeled_enum --algo reverse_search
```

In Python, use `enumerate_ptolemaic_labeled_graphs(5)` for the default, or
pass `algorithm="vertex_incremental"` / `algorithm="reverse_search"` explicitly.
The Python range stays `1 <= n <= 6`. C++ and the CLI accept `0 <= n <= 63`
for the structural method (the unlabeled generator's bitmask limit); the
legacy method retains `max_vertex_count`. Both emit one empty graph at n = 0.
Canonical-form sets, representatives, visited labelings and all output graphs
are kept in memory. The specialized canonical-parent search and delay bound
from [Yamazaki–Qian–Uehara (2024)](https://doi.org/10.1016/j.dam.2023.09.002)
are not implemented here. See the [Ptolemaic documentation](docs/en/families/ptolemaic.rst).

The labeled **3-leaf power** enumerator defaults to
`ThreeLeafPowerLabeledEnumAlgorithm::VERTEX_INCREMENTAL`. It reuses the structural
unlabeled generator: true-twin additions, and pendant additions only at vertices
without a true twin or in a complete graph. These operations keep the critical
clique graph a tree. Canonical forms remove isomorphic duplicates, including
critical clique tree symmetries, and disconnected graphs are composed from
connected representatives. The shared label-expansion utility emits all
`n! / |Aut(G)|` distinct labelings of each representative, removing duplicates
from twins and identical components. Fixing the addition order to increasing
labels would miss graphs whose largest-labeled vertex is neither pendant nor a twin.

The previous chordal-tree search with 3-leaf power recognition remains available
as `REVERSE_SEARCH`; the historical C++ function name is retained:

```cpp
#include "enumerators/three_leaf_power_labeled_enum.h"

auto graphs = graph_recognition::enumerate_three_leaf_power_labeled_graphs_reverse_search(5);
auto legacy = graph_recognition::enumerate_three_leaf_power_labeled_graphs_reverse_search(
    5, graph_recognition::ThreeLeafPowerLabeledEnumAlgorithm::REVERSE_SEARCH);
// Both results contain 642 graphs, in different orders.
```

The CLI accepts `--algo vertex_incremental` (default) or `--algo reverse_search`:

```bash
echo "5" | ./bin/three_leaf_power_labeled_enum
echo "5" | ./bin/three_leaf_power_labeled_enum --algo reverse_search
```

In Python, use `enumerate_three_leaf_power_labeled_graphs(5)` for the default, or
pass `algorithm="vertex_incremental"` / `algorithm="reverse_search"` explicitly.
The Python range stays `1 <= n <= 6`. C++ and the CLI accept `0 <= n <= 63`
for the structural method (the unlabeled generator's bitmask limit); the
legacy method retains `max_vertex_count`. Both emit one empty graph at n = 0.
Canonical-form sets, representatives, visited labelings and all output graphs
are kept in memory. The specialized canonical-parent search and delay bound
from [Yamazaki–Qian–Uehara (2024)](https://doi.org/10.1016/j.dam.2023.09.002)
are not implemented here. See the [3-leaf power documentation](docs/en/families/three_leaf_power.rst).

Both bipartite enumerators default to `BIPARTITE_COLORING`. Before adding a
vertex, they 2-color each current connected component. The new neighborhood
chooses a subset of one side per component; the empty subset is generated once.
Taking the Cartesian product of those choices generates exactly the valid
neighborhoods, including additions that join components with independently
flipped colorings. For component sides `(L_i, R_i)`, this visits
`product_i (2^|L_i| + 2^|R_i| - 1)` neighborhoods instead of scanning all `2^k`
subsets on the current `k` vertices. No bipartite recognizer is called in this
mode. Labels are added in increasing order, so labeled graphs need no label
expansion or deduplication. The unlabeled enumerator applies the existing
[McKay canonical construction path](https://users.cecs.anu.edu.au/~bdm/papers/orderly.pdf)
test to these children to emit one representative per isomorphism class.

```cpp
#include "enumerators/bipartite_labeled_enum.h"
#include "enumerators/bipartite_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search(5);
auto unlabeled = graph_recognition::enumerate_bipartite_unlabeled_graphs(5);
// 376 labeled graphs and 13 non-isomorphic representatives.
auto legacy_labeled = graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search(
    5, graph_recognition::BipartiteLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_bipartite_unlabeled_graphs(
    5, false, graph_recognition::BipartiteUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

The previous all-subsets algorithms remain selectable: `REVERSE_SEARCH` for
labeled graphs (with recognition) and `CANONICAL_AUGMENTATION` for unlabeled
graphs (with a color-compatibility filter). They preserve their former output
order. The new mode can change the order and the labelings of unlabeled
representatives. The CLI accepts `--algo bipartite_coloring` (default), plus:

```bash
echo "5" | ./bin/bipartite_labeled_enum --algo reverse_search
echo "5" | ./bin/bipartite_unlabeled_enum --algo canonical_augmentation
echo "5" | ./bin/bipartite_unlabeled_enum --connected --algo bipartite_coloring
```

Python uses `enumerate_bipartite_labeled_graphs(n, algorithm="bipartite_coloring")`
and `enumerate_bipartite_unlabeled_graphs(n, connected_only=False,
algorithm="bipartite_coloring")`; `algorithm` can also be `"reverse_search"`
or `"canonical_augmentation"`, respectively. Omitting it selects the new mode.
Python keeps its limits of `1 <= n <= 6` for labeled graphs and `1 <= n <= 8`
for unlabeled graphs. Both C++ and CLI algorithms accept `0 <= n <= 64`
(labeled) or `0 <= n <= 63` (unlabeled). At `n = 0` they emit one empty graph,
including with `connected_only`. All outputs are retained in memory, and the
unlabeled canonicalization and child-form sets still limit practical sizes.

Both **triangle-free** enumerators default to `INDEPENDENT_SETS`. A new vertex
preserves triangle-freeness exactly when its neighborhood is an independent
set of the current graph. The shared generator branches on including or
excluding a vertex; inclusion removes its neighbors from the remaining
candidates. It emits every independent set once, including the empty set
and non-maximal sets, without scanning all `2^k` subsets or calling a recognizer.
For example, a `K16,16` parent has only `2^17 - 1` valid neighborhoods among
`2^32` possible subsets. Neighborhood generation uses O(k) stack space and
O(k) work per emitted mask, excluding the child search and output construction.

The labeled version adds labels in increasing order, so each graph has a unique
parent obtained by deleting its largest label. The unlabeled version combines
the same neighborhood generator with the existing canonical-parent orbit test
and child-form deduplication, following the triangle-free example in
[McKay (1998), §2](https://users.cecs.anu.edu.au/~bdm/papers/orderly.pdf).
Both include disconnected graphs; unlabeled `connected_only` filters at emission.

```cpp
#include "enumerators/triangle_free_labeled_enum.h"
#include "enumerators/triangle_free_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search(5);
auto unlabeled = graph_recognition::enumerate_triangle_free_unlabeled_graphs(5);
// 388 labeled graphs and 14 non-isomorphic representatives.
auto legacy_labeled = graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search(
    5, graph_recognition::TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_triangle_free_unlabeled_graphs(
    5, false, graph_recognition::TriangleFreeUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

The previous all-subsets algorithms remain selectable: `REVERSE_SEARCH` for
labeled graphs (with recognition) and `CANONICAL_AUGMENTATION` for unlabeled
graphs (with an independence test). The new generator emits masks in ascending
order, preserving both output order and the labelings of unlabeled representatives.
The CLI accepts `--algo independent_sets` (default), plus:

```bash
echo "5" | ./bin/triangle_free_labeled_enum --algo reverse_search
echo "5" | ./bin/triangle_free_unlabeled_enum --algo canonical_augmentation
echo "5" | ./bin/triangle_free_unlabeled_enum --connected --algo independent_sets
```

The existing Python API is
`enumerate_triangle_free_unlabeled_graphs(n, connected_only=False,
algorithm="independent_sets")`; use `algorithm="canonical_augmentation"` for
the previous method. Omitting `algorithm` selects the new mode. Python keeps
its `1 <= n <= 8` limit. C++ and the CLI retain `0 <= n <= 64` for labeled
graphs and `0 <= n <= 63` for unlabeled graphs. Both modes emit one empty graph
at `n = 0`, including with `connected_only`. All outputs remain in memory;
the unlabeled canonicalization and child-form sets still limit practical sizes.

Both **bipartite permutation** enumerators default to `STRING_REPRESENTATION`,
using the line representations and endpoint-swap reverse search of
[Saitoh et al. (2012), §4](https://dspace.jaist.ac.jp/dspace/bitstream/10119/10720/1/17647.pdf).
For each bipartition size `p >= q`, the connected generator starts at `K_{p,q}`
and swaps adjacent endpoints at up to three nominated positions. Prefix
heights test connectivity; canonical strings identify reflections, exchange
of the two lines, and rotations. Canonicity filters output while connected
noncanonical orientations remain searchable, so their canonical descendants
are retained. This mode calls neither a graph recognizer nor a general graph
canonicalizer. Disconnected graphs are composed from component multisets;
the labeled version uses the shared label-expansion helper to emit every
distinct labeling, including graphs with twins and repeated components.

```cpp
#include "enumerators/bipartite_permutation_labeled_enum.h"
#include "enumerators/bipartite_permutation_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_bipartite_permutation_labeled_graphs_reverse_search(6);
auto unlabeled = graph_recognition::enumerate_bipartite_permutation_unlabeled_graphs(6);
// 5117 labeled graphs and 34 non-isomorphic representatives (16 connected).
auto legacy_labeled = graph_recognition::enumerate_bipartite_permutation_labeled_graphs_reverse_search(
    6, graph_recognition::BipartitePermutationLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_bipartite_permutation_unlabeled_graphs(
    6, false, graph_recognition::BipartitePermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

The CLI accepts `--algo string_representation` (default), or the previous
recognition-based algorithms:

```bash
echo "6" | ./bin/bipartite_permutation_labeled_enum --algo reverse_search
echo "6" | ./bin/bipartite_permutation_unlabeled_enum --algo canonical_augmentation
echo "6" | ./bin/bipartite_permutation_unlabeled_enum --connected --algo string_representation
```

Python exposes `enumerate_bipartite_permutation_labeled_graphs(n,
algorithm="string_representation")` and
`enumerate_bipartite_permutation_unlabeled_graphs(n, connected_only=False,
algorithm="string_representation")`; the legacy names are `"reverse_search"`
and `"canonical_augmentation"`, respectively. Existing limits remain:
C++/CLI accept `0 <= n <= 64` labeled and `0 <= n <= 63` unlabeled, while
Python accepts `1 <= n <= 6` and `1 <= n <= 8`. C++/CLI emit one empty graph
at `n = 0`, also with `connected_only`. Legacy output order is preserved;
the new mode can change order and unlabeled representative labelings.

The connected generator scans strings in O(n) time and constructs edge lists
in O(n²), for O(n²) amortized time per connected representative and O(n²)
search space excluding results. Component representatives, visited labelings,
and all outputs are retained in memory. Constant-time string updates,
prepostorder traversal, and jumps between bipartition sizes are not implemented;
the paper's O(1) delay guarantee does not apply to this API.

Both **permutation** enumerators default to `MODULAR_DECOMPOSITION`, using
the canonical edge-parent framework of
[Yamazaki et al. (2020), §§3, 5](https://doi.org/10.1016/j.tcs.2019.04.017).
The search starts at the complete graph on the requested vertices and deletes
one edge at a time. Modular decomposition supplies an isomorphism-invariant
permutation: series/parallel children are sorted by size and canonical
permutation, and prime quotients compare all four diagrams obtained by
exchanging lines and reversing both lines, including their substituted child
permutations. This implementation uses its own canonical child ordering.

A child's parent adds the first missing edge in lexicographic order on its
canonical labels that preserves permutation membership. Recognition checks
these local edge candidates; canonical strings remove isomorphic siblings
and verify their parent. Disconnected graphs are included in the same search;
`connected_only` prunes branches as soon as an edge deletion disconnects them.
The labeled version expands each representative into all distinct labelings.

```cpp
#include "enumerators/permutation_labeled_enum.h"
#include "enumerators/permutation_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_permutation_labeled_graphs_reverse_search(6);
auto unlabeled = graph_recognition::enumerate_permutation_unlabeled_graphs(6);
// 29744 labeled graphs and 142 non-isomorphic representatives (99 connected).
auto legacy_labeled = graph_recognition::enumerate_permutation_labeled_graphs_reverse_search(
    6, graph_recognition::PermutationLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_permutation_unlabeled_graphs(
    6, false, graph_recognition::PermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

The CLI accepts `--algo modular_decomposition` (default), or the previous
vertex-addition algorithms:

```bash
echo "6" | ./bin/permutation_labeled_enum --algo reverse_search
echo "6" | ./bin/permutation_unlabeled_enum --algo canonical_augmentation
echo "6" | ./bin/permutation_unlabeled_enum --connected --algo modular_decomposition
```

Python exposes `enumerate_permutation_labeled_graphs(n,
algorithm="modular_decomposition")` and
`enumerate_permutation_unlabeled_graphs(n, connected_only=False,
algorithm="modular_decomposition")`. Use `algorithm="reverse_search"` and
`algorithm="canonical_augmentation"`, respectively, for the legacy methods.
Existing limits remain: C++/CLI accept `0 <= n <= 64` labeled and
`0 <= n <= 63` unlabeled; Python accepts `1 <= n <= 6` and `1 <= n <= 8`.
C++/CLI emit one empty graph at `n = 0`, also with `connected_only`.
Legacy output order is preserved; the new default changes order and may change
unlabeled representative labelings. The historical C++ function name remains.

The default uses the existing closure-based modular decomposition and
matrix-based recognition/realizer, so the paper's delay bound does not apply.
Canonicalization and edge search use polynomial time per representative and
polynomial working space, including per-parent sibling sets. All outputs and
the label-expansion helper's visited labelings are additionally held in memory.

The **unlabeled interval** enumerator defaults to `MPQ_TREE`, based on
[Mikos (2021), §§3–4, 6](https://dmtcs.episciences.org/7246).
It starts at `K_n` and deletes edges that preserve the interval property.
MPQ-tree sections and rotatable paths determine which deletions are valid
and construct their interval endpoint strings, without calling the interval
recognizer. Canonical MPQ strings remove isomorphic siblings; a child is
accepted only when extending its first non-universal interval adds the
canonical parent edge back to the current class. The search includes
disconnected graphs, and `connected_only` prunes deletions that disconnect them.

```cpp
#include "enumerators/interval_unlabeled_enum.h"

auto graphs = graph_recognition::enumerate_interval_unlabeled_graphs(6);
// 92 non-isomorphic interval graphs (56 with connected_only = true).
auto legacy = graph_recognition::enumerate_interval_unlabeled_graphs(
    6, false, graph_recognition::IntervalUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

```bash
echo "6" | ./bin/interval_unlabeled_enum --algo mpq_tree
echo "6" | ./bin/interval_unlabeled_enum --connected --algo mpq_tree
echo "6" | ./bin/interval_unlabeled_enum --algo canonical_augmentation
```

Python accepts `enumerate_interval_unlabeled_graphs(n, connected_only=False,
algorithm="mpq_tree")`; use `algorithm="canonical_augmentation"` for the
previous vertex-addition and recognition algorithm. Omitting `algorithm`
selects `mpq_tree`. The C++/CLI range remains `0 <= n <= 63`, and Python
accepts `1 <= n <= 8`. C++/CLI return one empty graph at `n = 0` in both
connectivity modes. The legacy option preserves its output order and
representative labels; the new default may change both.

This implementation builds MPQ-trees through the existing clique PQ-tree
reductions, compares full subtree strings for canonicalization, and tests
section inclusions directly. It does not implement the paper's linear-time
MPQ construction, compact canonical ranks, or heap-based batch edge listing,
so its `O(n³ log n)` delay bound does not apply. Tree construction and edge
search take polynomial time per representative, with polynomial working
space including sibling sets and an explicit DFS stack. All outputs are
also retained in memory. Labeled interval enumeration continues to use
Kiyomi–Kijima–Uno; the co-interval unlabeled wrapper retains its previous
canonical-augmentation backend and output order.

Both **block graph** enumerators default to `BLOCK_GRAPH_GEN`, based on
Ekim–Shalom–Yirik's weighted block trees. Cut nodes have weight 1; block
nodes carry the number of non-cut vertices in their clique. Alternating
colors, sorted multisets of branches, and the one-, two-, and three-centroid
cases generate each connected isomorphism class once. Zero-weight internal
blocks are included. Each tree is expanded into clique edges, and multisets
of connected components cover disconnected graphs, without recognition or
general graph canonization. The labeled API then uses the shared label
expansion to produce all `n! / |Aut(G)|` distinct labelings per representative.

```cpp
#include "enumerators/block_labeled_enum.h"
#include "enumerators/block_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_block_labeled_graphs_reverse_search(6);
// 7739 labeled graphs, including disconnected graphs.
auto unlabeled = graph_recognition::enumerate_block_unlabeled_graphs(6);
// 50 isomorphism classes (22 with connected_only = true).
auto legacy = graph_recognition::enumerate_block_labeled_graphs_reverse_search(
    6, graph_recognition::BlockLabeledEnumAlgorithm::REVERSE_SEARCH);
```

The previous labeled chordal search with block recognition remains selectable
as `REVERSE_SEARCH`, preserving its output order. CLI examples:

```bash
echo "6" | ./bin/block_labeled_enum --algo block_graph_gen
echo "6" | ./bin/block_labeled_enum --algo reverse_search
echo "6" | ./bin/block_unlabeled_enum --connected --algo block_graph_gen
```

Python provides `enumerate_block_labeled_graphs(n, algorithm="block_graph_gen")`
and `enumerate_block_unlabeled_graphs(n, connected_only=False,
algorithm="block_graph_gen")`. The labeled function also accepts
`algorithm="reverse_search"`. Python's limits remain 6 vertices for labeled
results and 8 for unlabeled results. Both C++ APIs and CLIs accept
`0 <= n <= max_vertex_count`; the new construction uses no adjacency bitmask.
At `n = 0` they emit one empty graph, or none with `connected_only` / `--connected`.

The implementation caches rooted branches up to weight `floor(n/2)` and
connected representatives for component composition. Reconstructing and
sorting an edge list costs `O(n + m log(m+1))`, separately from tree generation
and label expansion. All results and the current representative's visited
labelings are retained in memory; this API does not claim the paper's linear
delay for tree representations. See the [block documentation](docs/en/families/block.rst).

Both **biconnected graph** enumerators default to `EAR_AUGMENTATION`, based
on [Stolee's canonical ear search](https://arxiv.org/abs/1104.5261).
Starting with cycles `C_3, ..., C_n`, the search adds paths between distinct
existing vertices, including missing edges as paths with no internal vertices.
Every intermediate graph is biconnected. A shortest removable ear, with ties
broken by canonically labeled endpoints, determines the parent class; sibling
canonical forms remove duplicate augmentations. Only graphs with exactly `n`
vertices are emitted. The labeled version expands each representative into
all `n! / |Aut(G)|` distinct labelings.

```cpp
#include "enumerators/biconnected_labeled_enum.h"
#include "enumerators/biconnected_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_biconnected_labeled_graphs(6); // 11368
auto unlabeled = graph_recognition::enumerate_biconnected_unlabeled_graphs(6); // 56
auto legacy = graph_recognition::enumerate_biconnected_unlabeled_graphs(
    6, graph_recognition::BiconnectedUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
auto cycles = graph_recognition::enumerate_biconnected_unlabeled_graphs(
    6, graph_recognition::BiconnectedUnlabeledEnumAlgorithm::EAR_AUGMENTATION, 6); // C6
```

The previous vertex-addition algorithms remain available as `REVERSE_SEARCH`
(labeled) and `CANONICAL_AUGMENTATION` (unlabeled), preserving their output
order and representative labels. Both algorithms accept an optional inclusive
`max_edges` bound (third C++ argument, `-1` for unrestricted). Ear search prunes
a partial graph with `k < n` vertices and `m` edges when `m + n - k + 1`
exceeds that bound; the legacy searches use edge and minimum-degree bounds.

```bash
echo "6" | ./bin/biconnected_labeled_enum --algo ear_augmentation
echo "6" | ./bin/biconnected_labeled_enum --algo reverse_search
echo "8" | ./bin/biconnected_unlabeled_enum --max-edges 10
echo "8" | ./bin/biconnected_unlabeled_enum --algo canonical_augmentation --max-edges 10
```

Python provides `enumerate_biconnected_labeled_graphs(n, algorithm=None,
max_edges=None)` and `enumerate_biconnected_unlabeled_graphs(n, algorithm=None,
max_edges=None)`. `None` selects `ear_augmentation` and no edge bound; legacy
names are `reverse_search` and `canonical_augmentation`, respectively. Python
accepts `1 <= n <= 6` for labeled results and `1 <= n <= 8` for unlabeled
results. C++/CLI accept `0 <= n <= 63` for ear search and unlabeled legacy
search; labeled `REVERSE_SEARCH` retains its limit of 64. All modes return
no graphs for `n < 3`. Biconnected graphs are connected by definition, so
there is no `connected_only` / `--connected` option.

This implementation uses the existing worst-case factorial canonicalizer and
per-parent sibling sets in place of EarSearch's nauty and vertex-pair orbits.
Recognition is used to validate ear deletions. Search retains its DFS path and
sibling sets; the public APIs also retain all outputs, and label expansion
keeps visited labelings for the current representative. No polynomial delay
or general speedup over the legacy search is claimed; sparse edge bounds
benefit most. See [design notes and measurements](notes/design_notes.md#biconnected-ear-augmentation).

Both Laman enumerators default to `HENNEBERG`, following the forward
construction in [Taufiqurrachman et al., Sections 2.4 and 3.1](https://www.ipsj-kyushu.jp/page/ronbun/hinokuni/1001/B-1/B-1-3.pdf).
Starting from `K2`, Henneberg I adds a vertex adjacent to any two distinct
vertices; Henneberg II subdivides an edge and joins the new vertex to a
third existing vertex. Both operations preserve Laman tightness, so the
new search requires no recognition or subset-sparsity checks. A canonical-form
set at each size removes isomorphic duplicates across all parents and moves.
The labeled version expands every representative into all `n! / |Aut(G)|`
distinct labelings. This expansion is necessary because the largest label
need not be removable by an inverse Henneberg move.

```cpp
#include "enumerators/laman_labeled_enum.h"
#include "enumerators/laman_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_laman_labeled_graphs(6); // 3355
auto unlabeled = graph_recognition::enumerate_laman_unlabeled_graphs(6); // 13
auto legacy_labeled = graph_recognition::enumerate_laman_labeled_graphs(
    6, graph_recognition::LamanLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_laman_unlabeled_graphs(
    6, graph_recognition::LamanUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

The CLI accepts `--algo henneberg` (default), `--algo reverse_search`
(labeled), or `--algo canonical_augmentation` (unlabeled):

```bash
echo "6" | ./bin/laman_labeled_enum --algo henneberg
echo "6" | ./bin/laman_labeled_enum --algo reverse_search
echo "8" | ./bin/laman_unlabeled_enum --algo henneberg
echo "8" | ./bin/laman_unlabeled_enum --algo canonical_augmentation
```

Both variants retain the existing conventions: no graphs for `n = 0`,
one `K1` for `n = 1`, and one `K2` for `n = 2`. Laman graphs are connected,
so there is no `connected_only` / `--connected` option. C++/CLI accept
`0 <= n <= 63` for Henneberg construction and unlabeled legacy search;
labeled `REVERSE_SEARCH` retains its limit of 64. The legacy options also
preserve their output order and representative labels. The two algorithms
can output graphs in different orders and use different unlabeled representatives.
Laman enumeration remains available in C++/CLI; Python exposes recognition only.

Each k-vertex parent has `k(k-1)/2 + (2k-3)(k-2)` extension candidates.
The shared exact canonicalizer has factorial worst-case cost. Construction
retains the current and next levels and a canonical-form set; label expansion
retains the target representatives and the current representative's visited
labelings. Both APIs retain all output graphs. No polynomial delay or general
speedup over the legacy algorithms is claimed.

Both maximal planar enumerators default to `PLANTRI`, implementing the
E3/E4/E5 triangulation expansions of [Brinkmann+ 07, Sections 1.2–1.3].
Starting at `K4`, each operation replaces a disk of one, two, or three
triangular faces with a new vertex of degree 3, 4, or 5. Cyclic neighbor
orders preserve the embedding, so candidates need no planarity recognition
or final edge-count filter. Rooted directed-edge BFS in both orientations
gives canonical forms, identifying mirror images. A canonical inverse
expansion selects each child's parent class; sibling codes remove repeated
expansions. The labeled API expands each representative into its
`n! / |Aut(G)|` distinct labelings.

```cpp
#include "enumerators/maximal_planar_labeled_enum.h"
#include "enumerators/maximal_planar_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_maximal_planar_labeled_graphs_reverse_search(6);
auto unlabeled = graph_recognition::enumerate_maximal_planar_unlabeled_graphs(8);
auto legacy_labeled = graph_recognition::enumerate_maximal_planar_labeled_graphs_reverse_search(
    6, graph_recognition::MaximalPlanarLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_maximal_planar_unlabeled_graphs(
    8, graph_recognition::MaximalPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

```bash
echo "6" | ./bin/maximal_planar_labeled_enum --algo plantri
echo "6" | ./bin/maximal_planar_labeled_enum --algo reverse_search
echo "8" | ./bin/maximal_planar_unlabeled_enum --algo plantri
echo "8" | ./bin/maximal_planar_unlabeled_enum --algo canonical_augmentation
```

Python's existing `enumerate_maximal_planar_unlabeled_graphs(n,
algorithm="plantri")` accepts `algorithm="canonical_augmentation"` for the
legacy method; the labeled API remains C++/CLI only. Omitting the algorithm
selects `plantri`. C++/CLI retain `0 <= n <= 64` (labeled) and
`0 <= n <= 63` (unlabeled), with one `K_n` for `n = 0..3`; Python retains
`1 <= n <= 8`. There is no `connected_only` option. Legacy modes preserve
their output order and representative labels; the new mode may use a
different order and different representatives.

This is an in-library implementation, with no external plantri dependency.
A k-vertex parent has O(k) candidate patches and O(k²) work per candidate;
rotation copies and sibling sets along the DFS path take O(n³) space.
All output is retained, and label expansion also stores visited edge sets
for the current representative. It does not implement plantri's in-place
updates or expansion-orbit pruning, and does not claim that program's
throughput or a polynomial delay bound. Tests match the published counts
through `n = 12` (7,595 classes, including the degree-five icosahedron),
as well as the legacy output sets at small sizes.

Both polyhedral enumerators default to `PLANTRI`, following the
triangulation-to-plane-graph construction of [Brinkmann+ 07, Section 4].
They reuse the maximal planar generator's embedded triangulations as roots
and delete edges incident with a triangle. A deletion is accepted only
when the merged face meets every other face in at most one vertex or one
common boundary edge, preserving 3-connectivity without recognition calls.
On each canonical embedding, adding the first missing ear diagonal selects
the parent class. Parent comparisons and sibling codes remove duplicates
across different deletion paths and triangulation roots, including mirror
images. Labeled output expands each representative into all distinct labelings.

```cpp
#include "enumerators/polyhedral_labeled_enum.h"
#include "enumerators/polyhedral_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_polyhedral_labeled_graphs(6);  // 1227
auto unlabeled = graph_recognition::enumerate_polyhedral_unlabeled_graphs(8);  // 257
auto legacy_labeled = graph_recognition::enumerate_polyhedral_labeled_graphs(
    6, graph_recognition::PolyhedralLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_polyhedral_unlabeled_graphs(
    8, graph_recognition::PolyhedralUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

```bash
echo "6" | ./bin/polyhedral_labeled_enum --algo plantri
echo "6" | ./bin/polyhedral_labeled_enum --algo reverse_search
echo "8" | ./bin/polyhedral_unlabeled_enum --algo plantri
echo "8" | ./bin/polyhedral_unlabeled_enum --algo canonical_augmentation
```

Python's `enumerate_polyhedral_unlabeled_graphs(n, algorithm="plantri")`
also accepts `algorithm="canonical_augmentation"`; omitting the argument
selects `plantri`. Labeled enumeration remains C++/CLI only. The existing
limits remain: C++/CLI `0 <= n <= 64` (labeled), `0 <= n <= 63` (unlabeled),
and Python `1 <= n <= 8`. Sizes below four yield no graphs, and there is no
`connected_only` option. Legacy modes retain their output order and labels;
the default uses its own order and representatives.

Beyond generating the triangulation roots, the edge search examines O(n)
candidates per n-vertex output, costing O(n²) per candidate and O(n³)
search storage. All outputs and the current label-expansion orbit are
stored separately. This internal implementation does not require plantri
or claim its throughput, in-place updates, or orbit-pruning optimizations.
Tests compare both legacy output sets at small sizes, check every edge's
deletion condition against vertex-pair connectivity tests through eight
vertices, and reproduce the published counts through `n = 10` (32,300 classes).

Both cubic planar enumerators default to `PLANTRI`, using the dual
triangulation construction of [Brinkmann+ 07, Section 2 and Figure 5].
For each connected component of order n, the maximal planar generator
provides simple triangulations on `n/2 + 2` vertices. Edge flips first
introduce parallel edges, then loops, while preserving triangular faces
and minimum degree three. Darts distinguish all edge incidences, including
loops. Each face becomes a cubic vertex; duals with loops or parallel edges
are rejected. This covers connectivity one and two as well as three.
Embedding codes identify mirror images, and canonical breadth-first
labelings remove abstract duplicates caused by distinct embeddings.
Unordered multisets of connected components cover disconnected graphs;
labeled output expands every representative into all distinct labelings.

```cpp
#include "enumerators/cubic_planar_labeled_enum.h"
#include "enumerators/cubic_planar_unlabeled_enum.h"

auto labeled = graph_recognition::enumerate_cubic_planar_labeled_graphs(6);  // 60
auto unlabeled = graph_recognition::enumerate_cubic_planar_unlabeled_graphs(8);  // 4
auto connected = graph_recognition::enumerate_cubic_planar_unlabeled_graphs(8, true);  // 3
auto legacy_labeled = graph_recognition::enumerate_cubic_planar_labeled_graphs(
    6, graph_recognition::CubicPlanarLabeledEnumAlgorithm::REVERSE_SEARCH);
auto legacy_unlabeled = graph_recognition::enumerate_cubic_planar_unlabeled_graphs(
    8, false, graph_recognition::CubicPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
```

```bash
echo "6" | ./bin/cubic_planar_labeled_enum --algo plantri
echo "6" | ./bin/cubic_planar_labeled_enum --algo reverse_search
echo "8" | ./bin/cubic_planar_unlabeled_enum --algo plantri --connected
echo "8" | ./bin/cubic_planar_unlabeled_enum --algo canonical_augmentation
```

Python's `enumerate_cubic_planar_unlabeled_graphs(n, connected_only=False,
algorithm="plantri")` also accepts `algorithm="canonical_augmentation"`.
Omitting `algorithm` selects `plantri`; existing positional `connected_only`
calls remain valid. Labeled enumeration remains C++/CLI only. The new
method supports `0 <= n <= 63` in C++/CLI; the legacy methods retain their
limits (`max_vertex_count` for labeled, 63 for unlabeled), output order and
representative labels. Python retains `1 <= n <= 8`. Odd n and n below four
produce no graphs. Only the unlabeled API supports `connected_only`.

This implementation uses no external plantri executable or recognition
calls. It retains embedding codes and a queue at each component size,
connected representatives, all outputs and the current label-expansion
visited set. Each map has O(n) candidate flips with O(n²) embedding
canonicalization per candidate. Abstract canonicalization tries all
breadth-first neighbor orders and has exponential worst-case cost.
Visited sets replace plantri's canonical reductions and orbit pruning;
its delay, throughput and memory bounds are not claimed. Tests match
Table 1's embedding counts and the abstract connected counts through
16 vertices (681 classes), compare both legacy output sets at small sizes,
and cover bridges, 2-vertex cuts and repeated disconnected components.

The weakly chordal enumerator now defaults to Kiyomi's class-specific
edge-addition reverse search. It starts at the empty spanning graph of K_n and
accepts a child exactly when the added edge is the youngest edge whose deletion
preserves weak chordality. The former hereditary largest-label vertex search is
available as
`WeaklyChordalLabeledEnumAlgorithm::GENERIC_VERTEX_AUGMENTATION`; the streaming API is
`enumerate_weakly_chordal_labeled_graphs_reverse_search_cb`.

Strongly chordal and proper chordal enumeration also default to dedicated
edge-addition searches: `KIYOMI_EDGE_ADDITION` and
`INDIFFERENCE_EDGE_ADDITION`, respectively. Their previous chordal-tree
filtering searches remain available as `LEGACY_CHORDAL_FILTER`. The proper
chordal search uses the tree-layout characterization of Paul and Protopapas;
their paper supplies the characterization and recognition algorithm, not this
enumeration algorithm. The factorial nested-convex subroutine in the current
recognizer also limits the enumeration implementation.

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
#include "enumerators/chordal_subgraph_enum.h"

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
#include "enumerators/chordal_bipartite_induced_subgraph_enum.h"

graph_recognition::Graph host(6, {{1,2}, {2,3}, {3,4}, {4,5}, {5,6}, {6,1}});
auto result =
    graph_recognition::enumerate_chordal_bipartite_induced_subgraphs(host);
// result.vertex_sets.size() == 63
```

### Library Usage

```cpp
#include "recognizers/interval.h"

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
#include "decompositions/modular_decomposition.h"
#include "decompositions/split_decomposition.h"

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

78 of the 81 recognizer classes are available as `is_<type>()` and `recognize_<type>()` functions (all but partial k-tree, k-degenerate and cage, whose extra parameters do not fit the wrapper's uniform signature), and 48 classes as `enumerate_<type>_graphs()` functions. See [python/README.md](python/README.md) for details.

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
  case; typical inputs stay on the polynomial path. The searches recurse once
  per maximal clique resp. per vertex, so a connected input with 7000 or more
  maximal cliques (MCCONNELL) or vertices (BACKTRACKING) is refused with
  `std::runtime_error` (`circular_arc_recursion_limit`).
- **Line graph** (`line_graph.h`): the O(mΔ) filter decides most inputs, but
  ambiguous cases fall back to a Krausz-partition backtracking search that is
  exponential in the worst case. The search recurses once per edge, so inputs
  with 6000 or more edges are refused with `std::runtime_error`
  (`line_graph_recursion_limit`).
- **Meyniel / parity** (`meyniel.h`, `parity.h`): both enumerate
  (induced) paths per edge or vertex pair, which is exponential in the worst
  case but fast on typical inputs. The Meyniel DFS recurses once per vertex,
  so inputs with 17000 or more vertices are refused with `std::runtime_error`
  (`meyniel_recursion_limit`); `even_hole_free.h` has the same guard at
  32000 vertices (`even_hole_free_recursion_limit`).

These recursion limits are the one exception to the complexity policy in
CLAUDE.md: a stack overflow ends the process with SIGSEGV, which no exception
boundary can report, so the depth is capped at about one third of the
measured crash point under the default 8 MiB stack.

## Testing

```bash
# Run the gtest suite with the default filter (tens of seconds once built)
make test

# Run a subset
./gtest_all --gtest_filter='Interval*'

# Also run the randomized Property tests (only the multi-minute enum cases stay excluded)
make test-quick
```

Recognizer and enumerator tests live under `tests/gtest/recognizers/` and `tests/gtest/enumerators/`, parameterised over the static cases in `tests/<type>/`. Property-based randomized tests live under `tests/gtest/property/`. The earlier Python/Bash test infrastructure (`tests/legacy/`) has been removed; it is still available at the `legacy-tests` git tag.

## Project Structure

```
include/          Header-only library in five role buckets; every #include names
                  the bucket (`#include "recognizers/chordal.h"`), -Iinclude is the only root
  util/             graph.h (1-indexed graph representation), graph_utils.h, search
                    primitives (lexbfs, mcs), dsu, minor, planarity_lr
  recognizers/      one <type>.h per graph class, providing check_<type>
  enumerators/      <type>_labeled_enum.h, <type>_unlabeled_enum.h (non-isomorphic),
                    <type>_subgraph_enum.h, <type>_induced_subgraph_enum.h
  decompositions/   clique / clique tree, components, block-cut tree, MD / split / SPQR,
                    PQ tree, orderings, tree decomposition / layout, twins, embeddings
  certificates/     shared NO-certificate vocabulary and extractors
                    (forbidden_subgraph.h / obstruction_extract.h)
src/              CLI entry points (<type>_main.cpp)
python/           Python wrapper (pybind11)
  src/              Package source (graph_recognition)
  tests/            pytest test suite
tests/            Test infrastructure
  <type>/           Static test cases (.in / .exp)
  gtest/            gtest suite (recognizers/, enumerators/, property/, helpers/)
third_party/googletest/  Google Test (submodule)
docs/             Sphinx + Doxygen documentation
notes/            Internal design notes
```

## AI Models Used

- **Claude 4.6 Opus** (Anthropic) — via Claude Code CLI
- **Codex 5.2** (OpenAI)

All code was generated by these models. The human provided task descriptions, reviewed outputs, and directed bug fixes — but wrote zero lines of code.

## References

Citation keys used in the [Supported Graph Classes](#supported-graph-classes)
tables. `Rec:` entries are the characterization or algorithm the recognizer
follows; `Enum:` entries describe the enumerator's current default.
[Avis+ 96] identifies a general search framework, not a guarantee that the
implementation generates arbitrary candidates and filters them. Several
classes have dedicated structural searches; others still use recognizer
pruning, and some retain the previous filter-based algorithm as an option.
Per-class implementation details are in [notes/enum_alg.md](notes/enum_alg.md)
and the [class documentation](https://junkawahara.github.io/ai-coded-graph-recognition-test/en).

- **[Abbas+ 00]** N. Abbas, L. K. Stewart. "Biconvex graphs: ordering and algorithms." *Discrete Applied Mathematics*, 103(1–3):1–19, 2000. [DOI:10.1016/S0166-218X(99)00217-6](https://doi.org/10.1016/S0166-218X(99)00217-6)
- **[Arnborg+ 87]** S. Arnborg, D. G. Corneil, A. Proskurowski. "Complexity of finding embeddings in a k-tree." *SIAM Journal on Algebraic and Discrete Methods*, 8(2):277–284, 1987. [DOI:10.1137/0608024](https://doi.org/10.1137/0608024)
- **[Avis+ 96]** D. Avis, K. Fukuda. "Reverse search for enumeration." *Discrete Applied Mathematics*, 65(1–3):21–46, 1996. [DOI:10.1016/0166-218X(95)00026-N](https://doi.org/10.1016/0166-218X(95)00026-N)
- **[Bandelt+ 86]** H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs." *Journal of Combinatorial Theory, Series B*, 41(2):182–208, 1986. [DOI:10.1016/0095-8956(86)90043-2](https://doi.org/10.1016/0095-8956(86)90043-2)
- **[Bauer+ 10]** R. Bauer, M. Krug, D. Wagner. "Enumerating and generating labeled k-degenerate graphs." *Proceedings of ANALCO 2010*, pp. 90–98, 2010. [DOI:10.1137/1.9781611973006.12](https://doi.org/10.1137/1.9781611973006.12)
- **[Beineke+ 69]** L. W. Beineke, R. E. Pippert. "The number of labeled k-dimensional trees." *Journal of Combinatorial Theory*, 6(2):200–205, 1969. [DOI:10.1016/S0021-9800(69)80120-1](https://doi.org/10.1016/S0021-9800(69)80120-1)
- **[Beyer+ 80]** T. Beyer, S. M. Hedetniemi. "Constant time generation of rooted trees." *SIAM Journal on Computing*, 9(4):706–712, 1980. [DOI:10.1137/0209055](https://doi.org/10.1137/0209055)
- **[Bodirsky+ 07]** M. Bodirsky, É. Fusy, M. Kang, S. Vigerske. "Enumeration and asymptotic properties of unlabeled outerplanar graphs." *Electronic Journal of Combinatorics*, 14(1):R66, 2007.
- **[Bodlaender+ 12]** H. L. Bodlaender, F. V. Fomin, A. M. C. A. Koster, D. Kratsch, D. M. Thilikos. "On exact algorithms for treewidth." *ACM Transactions on Algorithms*, 9(1):12:1–12:23, 2012. [DOI:10.1145/2390176.2390188](https://doi.org/10.1145/2390176.2390188)
- **[Booth+ 76]** K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms." *Journal of Computer and System Sciences*, 13(3):335–379, 1976. [DOI:10.1016/S0022-0000(76)80045-1](https://doi.org/10.1016/S0022-0000(76)80045-1)
- **[Bose 63]** R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs." *Pacific Journal of Mathematics*, 13(2):389–419, 1963. [DOI:10.2140/pjm.1963.13.389](https://doi.org/10.2140/pjm.1963.13.389)
- **[Brandes 09]** U. Brandes. "The left-right planarity test." Manuscript, University of Konstanz, 2009.
- **[Brandstädt+ 99]** A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.* SIAM Monographs on Discrete Mathematics and Applications, 1999. [DOI:10.1137/1.9780898719796](https://doi.org/10.1137/1.9780898719796)
- **[Brandstädt+ 06]** A. Brandstädt, V. B. Le. "Structure and linear time recognition of 3-leaf powers." *Information Processing Letters*, 98(4):133–138, 2006. [DOI:10.1016/j.ipl.2006.01.004](https://doi.org/10.1016/j.ipl.2006.01.004)
- **[Brandstädt+ 08]** A. Brandstädt, V. B. Le, R. Sritharan. "Structure and linear-time recognition of 4-leaf powers." *ACM Transactions on Algorithms*, 5(1):11:1–11:22, 2008. [DOI:10.1145/1435375.1435386](https://doi.org/10.1145/1435375.1435386)
- **[Brinkmann+ 02]** G. Brinkmann, B. D. McKay. "Posets on up to 16 points." *Order*, 19(2):147–179, 2002. [DOI:10.1023/A:1016543307592](https://doi.org/10.1023/A:1016543307592)
- **[Brinkmann+ 07]** G. Brinkmann, B. D. McKay. "Fast generation of planar graphs." *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323–357, 2007. Sections 1.2–1.3 (simple triangulations and canonical construction paths), 2 / Figure 5 (parallel edges, loops and cubic duals), and 4 (edge deletions for general plane graphs); Tables 1 and 9 (counts). [Official paper and program](https://users.cecs.anu.edu.au/~bdm/plantri/)
- **[Brinkmann+ 13]** G. Brinkmann, J. Goedgebeur, J. Hägglund, K. Markström. "Generation and properties of snarks." *Journal of Combinatorial Theory, Series B*, 103(4):468–488, 2013. [DOI:10.1016/j.jctb.2013.05.001](https://doi.org/10.1016/j.jctb.2013.05.001)
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
- **[Dinneen 97]** M. J. Dinneen. "Practical enumeration methods for graphs of bounded pathwidth and treewidth." CDMTCS Research Report CDMTCS-055, University of Auckland, 1997.
- **[Dom+ 06]** M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems." *Algorithmica*, 44(4):363–381, 2006. [DOI:10.1007/s00453-005-1180-z](https://doi.org/10.1007/s00453-005-1180-z)
- **[Duffin 65]** R. J. Duffin. "Topology of series-parallel networks." *Journal of Mathematical Analysis and Applications*, 10(2):303–318, 1965. [DOI:10.1016/0022-247X(65)90125-3](https://doi.org/10.1016/0022-247X(65)90125-3)
- **[Ekim+ 24]** T. Ekim, M. Shalom, M. A. Yirik. "Generation of weighted trees, block trees and block graphs." 2024. [arXiv:2401.09764](https://arxiv.org/abs/2401.09764)
- **[Euler 1741]** L. Euler. "Solutio problematis ad geometriam situs pertinentis." *Commentarii Academiae Scientiarum Petropolitanae*, 8:128–140, 1741 (presented 1736).
- **[Exoo+ 08]** G. Exoo, R. Jajcay. "Dynamic cage survey." *Electronic Journal of Combinatorics*, Dynamic Survey DS16, 2008 (revised 2013). [DOI:10.37236/37](https://doi.org/10.37236/37)
- **[Farber 83]** M. Farber. "Characterizations of strongly chordal graphs." *Discrete Mathematics*, 43(2–3):173–189, 1983. [DOI:10.1016/0012-365X(83)90154-1](https://doi.org/10.1016/0012-365X(83)90154-1)
- **[Faudree+ 97]** R. Faudree, E. Flandrin, Z. Ryjáček. "Claw-free graphs — A survey." *Discrete Mathematics*, 164(1–3):87–147, 1997. [DOI:10.1016/S0012-365X(96)00045-3](https://doi.org/10.1016/S0012-365X(96)00045-3)
- **[Földes+ 77]** S. Földes, P. L. Hammer. "Split graphs." *Congressus Numerantium*, 19:311–315, 1977.
- **[Fu+ 97]** H.-L. Fu, K.-C. Huang, C. A. Rodger. "Connectivity of cages." *Journal of Graph Theory*, 24(2):187–191, 1997. [DOI:10.1002/(SICI)1097-0118(199702)24:2<187::AID-JGT6>3.0.CO;2-M](https://doi.org/10.1002/(SICI)1097-0118(199702)24:2%3C187::AID-JGT6%3E3.0.CO;2-M)
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
- **[Hering+ 82]** F. Hering, R. Hoede, H. Fleischner. "The enumeration of stack polytopes and simplicial clusters." *Discrete Mathematics*, 40(2–3):203–217, 1982. [DOI:10.1016/0012-365X(82)90121-2](https://doi.org/10.1016/0012-365X(82)90121-2)
- **[Howorka 77]** E. Howorka. "A characterization of distance-hereditary graphs." *The Quarterly Journal of Mathematics*, 28(4):417–420, 1977. [DOI:10.1093/qmath/28.4.417](https://doi.org/10.1093/qmath/28.4.417)
- **[Howorka 81]** E. Howorka. "A characterization of Ptolemaic graphs." *Journal of Graph Theory*, 5(3):323–331, 1981. [DOI:10.1002/jgt.3190050314](https://doi.org/10.1002/jgt.3190050314)
- **[Isaacs 75]** R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable." *The American Mathematical Monthly*, 82(3):221–239, 1975. [DOI:10.2307/2319844](https://doi.org/10.2307/2319844)
- **[Itai+ 78]** A. Itai, M. Rodeh. "Finding a minimum circuit in a graph." *SIAM Journal on Computing*, 7(4):413–423, 1978. [DOI:10.1137/0207033](https://doi.org/10.1137/0207033)
- **[Jacobs+ 97]** D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game." *Journal of Computational Physics*, 137(2):346–365, 1997. [DOI:10.1006/jcph.1997.5809](https://doi.org/10.1006/jcph.1997.5809)
- **[Jones+ 18]** Á. A. Jones, F. Protti, R. R. Del-Vecchio. "Cograph generation with linear delay." *Theoretical Computer Science*, 713:1–10, 2018. [DOI:10.1016/j.tcs.2017.12.037](https://doi.org/10.1016/j.tcs.2017.12.037)
- **[Kiyomi+ 06]** M. Kiyomi, T. Uno. "Generating chordal graphs included in given graphs." *IEICE Transactions on Information and Systems*, E89-D(2):763–770, 2006. [DOI:10.1093/ietisy/e89-d.2.763](https://doi.org/10.1093/ietisy/e89-d.2.763)
- **[Kiyomi-Kijima+ 06]** M. Kiyomi, S. Kijima, T. Uno. "Listing chordal graphs and interval graphs." *Graph-Theoretic Concepts in Computer Science (WG 2006)*, LNCS 4271:68–77, 2006. [DOI:10.1007/11917496_7](https://doi.org/10.1007/11917496_7)
- **[Kiyomi thesis 06]** M. Kiyomi. *Studies on Subgraph and Supergraph Enumeration Algorithms.* Ph.D. thesis, The Graduate University for Advanced Studies, 2006, Sections 4.1.3 (strongly chordal) and 4.1.5 (weakly chordal). [PDF](https://www.nii.ac.jp/graduate/wp-content/themes/nii_original/assets/pdf/students_thesis/18/kiyomi_Dr_thesis.pdf)
- **[König 36]** D. König. *Theorie der endlichen und unendlichen Graphen.* Akademische Verlagsgesellschaft, Leipzig, 1936.
- **[Krausz 43]** J. Krausz. "Démonstration nouvelle d'un théorème de Whitney sur les réseaux." *Matematikai és Fizikai Lapok*, 50:75–85, 1943.
- **[Kuratowski 30]** K. Kuratowski. "Sur le problème des courbes gauches en topologie." *Fundamenta Mathematicae*, 15(1):271–283, 1930. [DOI:10.4064/fm-15-1-271-283](https://doi.org/10.4064/fm-15-1-271-283)
- **[Kurita+ 19]** K. Kurita, K. Wasa, H. Arimura, T. Uno. "An Efficient Algorithm for Enumerating Chordal Bipartite Induced Subgraphs in Sparse Graphs." *Combinatorial Algorithms (IWOCA 2019)*, LNCS 11638:339–351, 2019. [arXiv:1903.02161](https://arxiv.org/abs/1903.02161)
- **[Lafond 22]** M. Lafond. "Recognizing k-leaf powers in polynomial time, for constant k." *Proceedings of SODA 2022*, pp. 1384–1410, 2022. [DOI:10.1137/1.9781611977073.58](https://doi.org/10.1137/1.9781611977073.58)
- **[Laman 70]** G. Laman. "On graphs and rigidity of plane skeletal structures." *Journal of Engineering Mathematics*, 4(4):331–340, 1970. [DOI:10.1007/BF01534980](https://doi.org/10.1007/BF01534980)
- **[Lekkerkerker+ 62]** C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line." *Fundamenta Mathematicae*, 51(1):45–64, 1962. [DOI:10.4064/fm-51-1-45-64](https://doi.org/10.4064/fm-51-1-45-64)
- **[Lick+ 70]** D. R. Lick, A. T. White. "k-degenerate graphs." *Canadian Journal of Mathematics*, 22(5):1082–1096, 1970. [DOI:10.4153/CJM-1970-125-1](https://doi.org/10.4153/CJM-1970-125-1)
- **[Mahadev+ 95]** N. V. R. Mahadev, U. N. Peled. *Threshold Graphs and Related Topics.* Annals of Discrete Mathematics 56, North-Holland, 1995.
- **[Matula+ 83]** D. W. Matula, L. L. Beck. "Smallest-last ordering and clustering and graph coloring algorithms." *Journal of the ACM*, 30(3):417–427, 1983. [DOI:10.1145/2402.322385](https://doi.org/10.1145/2402.322385)
- **[McConnell 03]** R. M. McConnell. "Linear-time recognition of circular-arc graphs." *Algorithmica*, 37(2):93–147, 2003. [DOI:10.1007/s00453-003-1032-7](https://doi.org/10.1007/s00453-003-1032-7)
- **[McKay 98]** B. D. McKay. "Isomorph-free exhaustive generation." *Journal of Algorithms*, 26(2):306–324, 1998. [DOI:10.1006/jagm.1997.0898](https://doi.org/10.1006/jagm.1997.0898)
- **[McKay+ 01]** B. D. McKay, E. Spence. "Classification of regular two-graphs on 36 and 38 vertices." *Australasian Journal of Combinatorics*, 24:293–300, 2001.
- **[Meringer 99]** M. Meringer. "Fast generation of regular graphs and construction of cages." *Journal of Graph Theory*, 30(2):137–146, 1999. [DOI:10.1002/(SICI)1097-0118(199902)30:2<137::AID-JGT7>3.0.CO;2-G](https://doi.org/10.1002/(SICI)1097-0118(199902)30:2%3C137::AID-JGT7%3E3.0.CO;2-G)
- **[Meyniel 76]** H. Meyniel. "On the perfect graph conjecture." *Discrete Mathematics*, 16(4):339–342, 1976. [DOI:10.1016/S0012-365X(76)80008-8](https://doi.org/10.1016/S0012-365X(76)80008-8)
- **[Mikos 21]** P. Mikos. "Efficient enumeration of non-isomorphic interval graphs." *Discrete Mathematics & Theoretical Computer Science*, 23(1), #2, 2021. [DOI:10.46298/dmtcs.6164](https://doi.org/10.46298/dmtcs.6164)
- **[Moon 68]** J. W. Moon. *Topics on Tournaments.* Holt, Rinehart and Winston, New York, 1968.
- **[Naji 85]** W. Naji. "Reconnaissance des graphes de cordes." *Discrete Mathematics*, 54(3):329–337, 1985. [DOI:10.1016/0012-365X(85)90117-7](https://doi.org/10.1016/0012-365X(85)90117-7)
- **[Nishimura+ 02]** N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees." *Journal of Algorithms*, 42(1):69–108, 2002. [DOI:10.1006/jagm.2001.1195](https://doi.org/10.1006/jagm.2001.1195)
- **[Paul+ 24]** C. Paul, E. Protopapas. "Tree-Layout Based Graph Classes: Proper Chordal Graphs." *Proceedings of STACS 2024*, LIPIcs 289, 55:1–55:18, 2024. [DOI:10.4230/LIPIcs.STACS.2024.55](https://doi.org/10.4230/LIPIcs.STACS.2024.55)
- **[Petersen 1891]** J. Petersen. "Die Theorie der regulären Graphs." *Acta Mathematica*, 15:193–220, 1891. [DOI:10.1007/BF02392606](https://doi.org/10.1007/BF02392606)
- **[Pnueli+ 71]** A. Pnueli, A. Lempel, S. Even. "Transitive orientation of graphs and identification of permutation graphs." *Canadian Journal of Mathematics*, 23(1):160–175, 1971. [DOI:10.4153/CJM-1971-016-5](https://doi.org/10.4153/CJM-1971-016-5)
- **[Ringel 63]** G. Ringel. "Selbstkomplementäre Graphen." *Archiv der Mathematik*, 14(1):354–358, 1963. [DOI:10.1007/BF01234967](https://doi.org/10.1007/BF01234967)
- **[Roberts 69]** F. S. Roberts. "Indifference graphs." In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139–146, 1969.
- **[Robertson+ 95]** N. Robertson, P. D. Seymour. "Graph minors. XIII. The disjoint paths problem." *Journal of Combinatorial Theory, Series B*, 63(1):65–110, 1995. [DOI:10.1006/jctb.1995.1006](https://doi.org/10.1006/jctb.1995.1006)
- **[Rose 74]** D. J. Rose. "On simple characterizations of k-trees." *Discrete Mathematics*, 7(3–4):317–322, 1974. [DOI:10.1016/0012-365X(74)90042-9](https://doi.org/10.1016/0012-365X(74)90042-9)
- **[Rose+ 76]** D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs." *SIAM Journal on Computing*, 5(2):266–283, 1976. [DOI:10.1137/0205021](https://doi.org/10.1137/0205021)
- **[Sachs 62]** H. Sachs. "Über selbstkomplementäre Graphen." *Publicationes Mathematicae Debrecen*, 9:270–288, 1962.
- **[Saitoh+ 10]** T. Saitoh, K. Yamanaka, M. Kiyomi, R. Uehara. "Random Generation and Enumeration of Proper Interval Graphs." *IEICE Transactions on Information and Systems*, E93-D(7):1816–1823, 2010. [DOI:10.1587/transinf.E93.D.1816](https://doi.org/10.1587/transinf.E93.D.1816)

- **[Saitoh+ 12]** T. Saitoh, Y. Otachi, K. Yamanaka, R. Uehara. "Random Generation and Enumeration of Bipartite Permutation Graphs." *Journal of Discrete Algorithms*, 10:84–97, 2012. [DOI:10.1016/j.jda.2011.11.001](https://doi.org/10.1016/j.jda.2011.11.001)
- **[Seinsche 74]** D. Seinsche. "On a property of the class of n-colorable graphs." *Journal of Combinatorial Theory, Series B*, 16(2):191–193, 1974. [DOI:10.1016/0095-8956(74)90063-X](https://doi.org/10.1016/0095-8956(74)90063-X)
- **[Spinrad+ 87]** J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs." *Discrete Applied Mathematics*, 18(3):279–292, 1987. [DOI:10.1016/S0166-218X(87)80003-3](https://doi.org/10.1016/S0166-218X(87)80003-3)
- **[Spinrad+ 95]** J. P. Spinrad, R. Sritharan. "Algorithms for weakly triangulated graphs." *Discrete Applied Mathematics*, 59(2):181–191, 1995. [DOI:10.1016/0166-218X(93)E0161-Q](https://doi.org/10.1016/0166-218X(93)E0161-Q)
- **[Steinitz 22]** E. Steinitz. "Polyeder und Raumeinteilungen." *Encyklopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.
- **[Stolee 11]** D. Stolee. "Isomorph-free generation of 2-connected graphs with applications." 2011. [arXiv:1104.5261](https://arxiv.org/abs/1104.5261)
- **[Tarjan 72]** R. Tarjan. "Depth-first search and linear graph algorithms." *SIAM Journal on Computing*, 1(2):146–160, 1972. [DOI:10.1137/0201010](https://doi.org/10.1137/0201010)
- **[Tarjan+ 84]** R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs." *SIAM Journal on Computing*, 13(3):566–579, 1984. [DOI:10.1137/0213035](https://doi.org/10.1137/0213035)
- **[Taufiqurrachman+ Henneberg]** Taufiqurrachman, Y. Yamauchi, S. Kijima, M. Yamashita. "Enumeration of Henneberg decompositions of Laman graphs and M-Circuits." Sections 2.4 and 3.1 (Henneberg I/II construction and non-isomorphic generation). [Author manuscript](https://www.ipsj-kyushu.jp/page/ronbun/hinokuni/1001/B-1/B-1-3.pdf)
- **[Troyka 19]** J. M. Troyka. "Split graphs: combinatorial species and asymptotics." *Electronic Journal of Combinatorics*, 26(2):P2.42, 2019. [arXiv:1803.07248](https://arxiv.org/abs/1803.07248)
- **[Tucker 74]** A. Tucker. "Structure theorems for some circular-arc graphs." *Discrete Mathematics*, 7(1–2):167–195, 1974. [DOI:10.1016/S0012-365X(74)80027-0](https://doi.org/10.1016/S0012-365X(74)80027-0)
- **[Tucker 80]** A. Tucker. "An efficient test for circular-arc graphs." *SIAM Journal on Computing*, 9(1):1–24, 1980. [DOI:10.1137/0209001](https://doi.org/10.1137/0209001)
- **[Valdes+ 82]** J. Valdes, R. E. Tarjan, E. L. Lawler. "The recognition of series parallel digraphs." *SIAM Journal on Computing*, 11(2):298–313, 1982. [DOI:10.1137/0211023](https://doi.org/10.1137/0211023)
- **[Whitney 32]** H. Whitney. "Congruent graphs and the connectivity of graphs." *American Journal of Mathematics*, 54(1):150–168, 1932. [DOI:10.2307/2371086](https://doi.org/10.2307/2371086)
- **[Wolk 62]** E. S. Wolk. "The comparability graph of a tree." *Proceedings of the American Mathematical Society*, 13(5):789–795, 1962. [DOI:10.1090/S0002-9939-1962-0172273-0](https://doi.org/10.1090/S0002-9939-1962-0172273-0)
- **[Wright+ 86]** R. A. Wright, B. Richmond, A. Odlyzko, B. D. McKay. "Constant time generation of free trees." *SIAM Journal on Computing*, 15(2):540–548, 1986. [DOI:10.1137/0215039](https://doi.org/10.1137/0215039)
- **[Yan+ 96]** J.-H. Yan, J.-J. Chen, G. J. Chang. "Quasi-threshold graphs." *Discrete Applied Mathematics*, 69(3):247–255, 1996. [DOI:10.1016/0166-218X(96)00094-7](https://doi.org/10.1016/0166-218X(96)00094-7)
- **[Yamazaki+ 20]** K. Yamazaki, T. Saitoh, M. Kiyomi, R. Uehara. "Enumeration of nonisomorphic interval graphs and nonisomorphic permutation graphs." *Theoretical Computer Science*, 806:310–322, 2020. [DOI:10.1016/j.tcs.2019.04.017](https://doi.org/10.1016/j.tcs.2019.04.017)
- **[Yannakakis 82]** M. Yannakakis. "The complexity of the partial order dimension problem." *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351–358, 1982. [DOI:10.1137/0603036](https://doi.org/10.1137/0603036)

## License

[MIT](LICENSE)
