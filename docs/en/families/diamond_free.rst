Diamond-Free Graph
==================

Determines whether a graph is diamond-free (K_4-e free).
No induced subgraph isomorphic to diamond (K_4 minus one edge).
Equivalently, any two triangles sharing an edge must form a K_4.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_441.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DiamondFreeAlgorithm``
     - Description
   * - ``BRUTE``
     - Enumerates all 4-vertex subsets to find a diamond. Complexity: O(n^4).
   * - ``EDGE_PAIR`` **(default)**
     - For each edge, checks whether common neighbors form a clique via edge counting.
       Complexity: O(nm).

.. doxygenenum:: graph_recognition::DiamondFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DiamondFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_diamond_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::DiamondFreeEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DiamondFreeEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_diamond_free_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "diamond_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_diamond_free(g);

       std::cout << std::boolalpha << result.is_diamond_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "diamond_free_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_diamond_free_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
