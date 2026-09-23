Proper Interval Graph
=====================

Determines whether a graph is a proper interval graph.
An interval graph representable by a family of intervals with no proper containment.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_298.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ProperIntervalAlgorithm``
     - Description
   * - ``TRIPLE_LOOP_CLAW_CHECK``
     - Interval graph + claw (K_{1,3}) detection via triple loop. Complexity: O(n^3 + n * Delta^3)
       (including the O(n^3) interval test it starts with).
   * - ``FAST_CLAW_CHECK`` **(default)**
     - Interval graph + claw detection via edge counting.
       Only performs detailed search when N(c) has fewer than d(d-1)/2 edges.
       The claw filter is O(m * Delta) but the detailed search makes its worst
       case O(n * Delta^3), so together with the O(n^3) interval test the whole
       check is O(n^3 + n * Delta^3).

.. doxygenenum:: graph_recognition::ProperIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalResult
   :project: graph_recognition
   :members:

``ProperIntervalResult`` also reports an indifference ordering -- a vertex
order in which every closed neighbourhood is consecutive. It comes from
sorting the interval model the interval recognizer already builds by (left,
right) endpoint. That does not work for interval graphs in general, but in a
claw-free one no clique range can sit strictly inside another on both sides:
a vertex ending before that range and one starting after it would both be
neighbours of the outer vertex while being non-adjacent to each other and to
the inner vertex, so the outer vertex would be the centre of a claw whose
leaves are those two and the inner vertex. The ordering is checked directly
before being returned.

.. doxygenfunction:: graph_recognition::check_proper_interval
   :project: graph_recognition


Enumeration
-----------

The labeled enumerator defaults to ``STRING_REPRESENTATION``. It reuses the
unlabeled bracket-string generator described below, including disconnected
component composition, and expands each representative into all its distinct
labelings. Swapping adjacent labels generates every permutation. Recording
visited sorted edge lists identifies permutations differing by automorphisms,
so reflection, twin permutations and exchanges of equal components are all
handled. A representative G contributes exactly ``n! / |Aut(G)|`` labeled graphs.

For an orbit of L distinct labelings with m edges and n >= 1, expansion makes
L(n-1) swap attempts, each sorting m edges and looking up an edge vector in a
set of at most L entries. That set takes O(L(m+1)) additional space and is
released after each representative. The unlabeled representatives and all
labeled results are materialized. The bracket generator uses Dyck-word DFS;
this implementation does not implement the paper's optimized reverse search
or claim its O(1) time per unlabeled representation.

``REVERSE_SEARCH`` retains the previous chordal-tree search with
proper-interval recognition, including its original output order. Both choices
produce the same labeled graphs in potentially different orders, and both
return one empty graph for n = 0. The historical C++ function name is retained.
The CLI accepts ``--algo string_representation`` (default) or
``--algo reverse_search``. Python accepts the corresponding
``algorithm="string_representation"`` or ``algorithm="reverse_search"`` argument
to ``enumerate_proper_interval_labeled_graphs``; its existing range is 1 <= n <= 6.

.. doxygenenum:: graph_recognition::ProperIntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_interval_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, using the bracket-string representation
of Saitoh et al.: a connected proper interval graph on n vertices is a string
of n ``[`` and n ``]`` whose every proper non-empty prefix holds strictly more
``[`` than ``]``, and the string is unique up to reversal, so keeping only the
strings that do not exceed their reverse-flip keeps each class exactly once.
Disconnected graphs are composed from connected components over integer
partitions. The counts are OEIS A005217 (1, 2, 4, 9, 21, 55, 151, 447, ...),
or A007123 (1, 1, 2, 4, 10, 26, 76, 232, ...) with ``connected_only`` set.

.. doxygenenum:: graph_recognition::ProperIntervalUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ProperIntervalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_interval_unlabeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/proper_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_interval(g);

       std::cout << std::boolalpha << result.is_proper_interval << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/proper_interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* T. Saitoh, K. Yamanaka, M. Kiyomi, R. Uehara.
  "Random Generation and Enumeration of Proper Interval Graphs."
  *IEICE Transactions on Information and Systems*, E93-D(7):1816--1823, 2010.
  `DOI:10.1587/transinf.E93.D.1816 <https://doi.org/10.1587/transinf.E93.D.1816>`_

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
