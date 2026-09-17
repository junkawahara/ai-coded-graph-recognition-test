Usage Reference
===============

Build
-----

.. code-block:: bash

   make          # Build all targets
   make clean    # Remove binaries

The compiler defaults to ``g++`` with C++11. Override with ``CXX`` and ``CXXFLAGS``.


Input / Output Format
---------------------

**Input** (stdin):

.. code-block:: text

   n m
   u1 v1
   u2 v2
   ...

* ``n``: number of vertices, ``m``: number of edges
* Vertices are 1-indexed

**Output** (stdout): ``YES`` / ``NO`` followed by class-specific information.

**Exit status**: 0 when the answer was printed. 1 with ``error: malformed graph
input`` on stderr when the input does not follow the format (the directed
``digraph`` / ``poset`` / ``tournament`` CLIs included). 2 with
``error: ...; result unknown`` on stderr when an exception left the recognizer
without an answer -- typically an allocation failure (``std::bad_alloc``); nothing
is printed to stdout in that case. Several recognizers use memory quadratic (or
worse) in the vertex count (the project accepts O(n^2), O(n^3) and O(2^n)
algorithms), so a large input can legitimately end with status 2. The
recognizers whose recursion depth grows with the input (line_graph,
circular_arc, meyniel, even_hole_free) end with status 2 as well when they hit
the recursion limit that keeps them from overflowing the stack.

Enumeration CLIs (``bin/<type>_labeled_enum`` / ``bin/<type>_unlabeled_enum``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Input** (stdin): the vertex count ``n`` (followed by ``k``, the girth, ... for
the k-regular, k-tree, k-degenerate, partial k-tree and cage enumerators). Some
non-isomorphic enumerators take ``--connected`` to emit only connected graphs.

**Output** (stdout): the number of graphs on the first line, then one line per
graph, ``m u1 v1 u2 v2 ...`` (the edge count ``m`` followed by the 1-indexed
edges).

**Exit status**: 0 when the output was produced. 1 with ``error: ...`` on stderr
and nothing on stdout when ``n`` (or a parameter) cannot be read, ``n`` is
negative, ``n`` exceeds what the enumerator can represent (63 or 64 for the
enumerators that keep adjacencies in 64-bit words, 33 for the directed
canonical form) or an unknown option is given; an out-of-range ``n`` is never
answered with the count 0, which would be false for every class (each contains
the edgeless graph). The chordal / split / strongly_chordal / proper_chordal
labeled enumerators additionally cap ``n`` at 7 or 8 in the CLI because of the
output size. 2 when an exception left the enumerator without a result: the
search budgets of circle and five_leaf_power, an abandoned cage search, and
the like.


Library Usage (C++)
-------------------

.. code-block:: cpp

   #include "util/graph.h"
   #include "recognizers/interval.h"
   #include <iostream>

   int main() {
       using namespace graph_recognition;
       Graph g = Graph::read(std::cin);
       IntervalResult res = check_interval(g);
       if (res.is_interval) {
           std::cout << "YES" << std::endl;
           for (int v = 1; v <= g.n; ++v) {
               std::cout << v << ": ["
                         << res.intervals[v].first << ", "
                         << res.intervals[v].second << "]"
                         << std::endl;
           }
       } else {
           std::cout << "NO" << std::endl;
       }
       return 0;
   }


Algorithm Selection
-------------------

Each recognition function accepts an ``enum class`` parameter to choose the algorithm.
A default argument is provided, so the parameter can be omitted.

.. code-block:: cpp

   #include "util/graph.h"
   #include "recognizers/interval.h"

   using namespace graph_recognition;
   Graph g = Graph::read(std::cin);

   // Use the default algorithm (AT_FREE)
   IntervalResult r1 = check_interval(g);

   // Explicitly select an algorithm
   IntervalResult r2 = check_interval(g, IntervalAlgorithm::BACKTRACKING);
   IntervalResult r3 = check_interval(g, IntervalAlgorithm::AT_FREE);

More examples:

.. code-block:: cpp

   // Permutation graph
   check_permutation(g, PermutationAlgorithm::BACKTRACKING);
   check_permutation(g, PermutationAlgorithm::CLASS_BASED);  // default

   // Chordal bipartite graph
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::CYCLE_CHECK);
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::BISIMPLICIAL);
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL);  // default

   // Distance-hereditary graph
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::HASH_TWINS);  // default
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::SORTED_TWINS);


CLI Executables
---------------

Each graph class has a corresponding CLI executable in ``src/<type>_main.cpp``.
After building with ``make``, run:

.. code-block:: bash

   printf "4 3\n1 2\n2 3\n3 4\n" | ./bin/interval
   # Output: YES followed by the interval model

Enumeration executables accept a vertex count:

.. code-block:: bash

   echo 5 | ./bin/interval_labeled_enum
   # Output: the number of graphs on the first line, then one edge list per
   # labeled interval graph on 5 vertices


Testing
-------

**Primary test suite** (Google Test):

.. code-block:: bash

   make test                                # default filter, ~6 s once built
   make test-quick                          # also run the property tests (only the multi-minute enum cases stay excluded)
   ./gtest_all --gtest_filter='Interval*'   # run a subset

The legacy per-class checkers, differential testers, and fuzzers that
used to live in ``tests/legacy/`` (``check_<type>.py``, ``compare.py``,
``fuzz.sh``) have been removed; check out the ``legacy-tests`` git tag
if you need them.
