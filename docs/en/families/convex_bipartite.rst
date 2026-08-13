Convex Bipartite Graph
======================

Determines whether a graph is a convex bipartite graph.
A bipartite graph G=(X, Y, E) where Y can be linearly ordered
so that each vertex in X has neighbors forming a consecutive interval.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_67.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ConvexBipartiteAlgorithm``
     - Description
   * - ``BRUTE_FORCE``
     - Tries all permutations of Y to check the consecutive ones property (C1P).
       Complexity: :math:`O(|Y|! \cdot (|X| + m))`.
   * - ``C1P`` **(default)**
     - Determines C1P via a PQ-tree. Complexity: O(n + m).

.. doxygenenum:: graph_recognition::ConvexBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ConvexBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_convex_bipartite
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ConvexBipartiteEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ConvexBipartiteEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_convex_bipartite_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "convex_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_convex_bipartite(g);

       std::cout << std::boolalpha << result.is_convex_bipartite << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "convex_bipartite_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_convex_bipartite_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
