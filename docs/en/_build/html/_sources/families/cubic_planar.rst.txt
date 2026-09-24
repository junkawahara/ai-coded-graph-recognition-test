Cubic Planar Graph
==================

A **cubic planar graph** is a planar graph where every vertex has degree
exactly 3.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1102.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CubicPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CubicPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class. Both default to ``PLANTRI``.
For each connected component of order n, simple triangulations on n/2+2
vertices seed the edge flips of Brinkmann–McKay (2007), section 2 / Figure 5.
Flips introduce parallel edges, then loops, preserving minimum degree three.
Darts record twins and triangular face walks; only simple duals are emitted.
Embedding codes identify reflections, while canonical breadth-first
labelings identify abstract graphs that have distinct embeddings.
This includes bridges and 2-vertex cuts. Component multisets cover
disconnected graphs; labeled enumeration expands all distinct labelings.

The original vertex-addition searches remain selectable, preserving their
output order and labels: ``REVERSE_SEARCH`` for labeled output and
``CANONICAL_AUGMENTATION`` for unlabeled output. CLI options are
``--algo plantri`` / ``reverse_search`` / ``canonical_augmentation``.
Python's existing unlabeled API accepts ``algorithm="plantri"`` or
``"canonical_augmentation"``. Unlabeled ``connected_only`` / ``--connected``
works with both algorithms. The new method supports n up to 63 in C++/CLI;
the legacy limits remain ``max_vertex_count`` (labeled) and 63 (unlabeled).
Python retains 1 <= n <= 8. Odd n and n < 4 produce no graphs.

There are O(n) flip candidates per map and O(n²) embedding canonicalization
work per candidate. Abstract canonicalization tries breadth-first neighbor
orders with exponential worst-case cost. Embedding codes/queues, component
representatives, output and label-expansion visited sets are retained.
No external plantri executable or recognizer is used; plantri's canonical
reductions, orbit pruning, throughput and space guarantees are not claimed.

.. doxygenenum:: graph_recognition::CubicPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CubicPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

The non-isomorphic enumeration reproduces
`OEIS A005964 <https://oeis.org/A005964>`_ (connected cubic planar
graphs) with ``connected_only`` through ``n = 16`` (133 at n = 14;
681 at n = 16), as well as Table 1's embedding counts through 16 vertices.
Output sets agree with the legacy algorithms through 10 vertices
(unlabeled) and eight vertices (labeled). Tests also cover bridges,
2-vertex cuts and repeated isomorphic components. The static test cases
stop at ``n = 12``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/cubic_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic_planar(g);

       std::cout << std::boolalpha << result.is_cubic_planar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cubic_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cubic_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 4
       return 0;
   }


References
----------

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
