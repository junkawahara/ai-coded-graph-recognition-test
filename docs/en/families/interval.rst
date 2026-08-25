Interval Graph
==============

Determines whether a graph is an interval graph.
Each vertex corresponds to an interval on the real line,
with edges between overlapping intervals.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_234.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``IntervalAlgorithm``
     - Description
   * - ``BACKTRACKING``
     - After verifying chordality, searches for a clique path on the clique tree
       via backtracking (each vertex's cliques must form a connected subtree).
   * - ``AT_FREE`` **(default)**
     - Verifies that the graph is chordal and AT-free (asteroidal triple-free),
       based on the Lekkerkerker--Boland theorem.
   * - ``PQ_TREE``
     - Verifies chordality and then tests the consecutive ones property of the
       vertex/maximal-clique incidence matrix with the Booth--Lueker PQ-tree,
       based on the Fulkerson--Gross theorem (see ``pq_tree.h`` in
       :doc:`../api/utilities`).

.. doxygenenum:: graph_recognition::IntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_interval
   :project: graph_recognition


Enumeration
-----------

The default ``KIYOMI_KIJIMA_UNO`` algorithm is an interval-specific
edge-deletion reverse search. Its root is ``K_n``. The unique parent adds an
edge between the largest-labeled non-universal vertex and the closest
non-neighbor in an interval model. A one-edge deletion is visited only when
its parent is the current graph. Vertices with larger labels than the pivot
are universal true twins, allowing equivalent deletion candidates to share
one recognition result.

``LEGACY_CHORDAL_FILTER`` selects the former chordal vertex-addition tree with
interval-recognition pruning. ``REVERSE_SEARCH`` is a backward-compatible
alias for ``KIYOMI_KIJIMA_UNO``.

With linear-time interval-model construction, the paper proves ``O(n^3)``
time per output and ``O(n^2)`` space. This implementation reuses the existing
``check_interval`` recognizer for child candidates and constructs a complete
edge list for every callback, so that time bound does not directly apply.

.. doxygenenum:: graph_recognition::IntervalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_interval_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_interval_graphs_reverse_search_cb
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated labeled interval
graphs was verified to match `OEIS A005215 <https://oeis.org/A005215>`_:
``1, 2, 8, 61, 822, 17914``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_interval(g);

       std::cout << std::boolalpha << result.is_interval << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "interval_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_interval_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Kiyomi, S. Kijima, T. Uno. "Listing Chordal Graphs and Interval Graphs."
  *Graph-Theoretic Concepts in Computer Science (WG 2006)*,
  LNCS 4271:68--77, 2006.
  `DOI:10.1007/11917496_7 <https://doi.org/10.1007/11917496_7>`_

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_

* D. Corneil, S. Olariu, L. Stewart. "The LBFS structure and recognition of interval graphs."
  *SIAM Journal on Discrete Mathematics*, 23(4):1905--1953, 2009.
  `DOI:10.1137/S0895480100373455 <https://doi.org/10.1137/S0895480100373455>`_
