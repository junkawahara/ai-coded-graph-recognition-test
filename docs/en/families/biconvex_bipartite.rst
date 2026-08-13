Biconvex Bipartite Graph
========================

Determines whether a graph is a biconvex bipartite graph.
A bipartite graph G=(X, Y, E) where both sides can be linearly ordered
so that each vertex's neighbors on the opposite side form a consecutive interval.
Both rows and columns of the biadjacency matrix satisfy the consecutive ones property.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_66.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BiconvexBipartiteAlgorithm``
     - Description
   * - ``BRUTE_FORCE``
     - Tries all permutations of each side independently (X-side C1P and Y-side
       C1P are tested separately, not jointly).
       Complexity: :math:`O((|X|! + |Y|!) \cdot (n + m))`.
   * - ``C1P`` **(default)**
     - PQ-tree (Booth & Lueker 1976) to check C1P on both sides.
       Complexity: :math:`O(n \cdot m)` in the worst case -- this PQ-tree
       implementation omits the BUBBLE pass of the original paper and walks
       from every pertinent leaf up to the tree root on each reduction, so it
       does not attain the O(n + m) bound of Booth & Lueker.

.. doxygenenum:: graph_recognition::BiconvexBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconvexBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconvex_bipartite
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BiconvexBipartiteEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconvexBipartiteEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconvex_bipartite_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "biconvex_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_biconvex_bipartite(g);

       std::cout << std::boolalpha << result.is_biconvex_bipartite << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "biconvex_bipartite_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconvex_bipartite_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* N. Abbas, L. K. Stewart. "Biconvex graphs: ordering and algorithms."
  *Discrete Applied Mathematics*, 103(1--3):1--19, 2000.
  `DOI:10.1016/S0166-218X(99)00217-6 <https://doi.org/10.1016/S0166-218X(99)00217-6>`_

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
