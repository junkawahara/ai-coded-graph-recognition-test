Cactus Graph
============

A graph is a **cactus** if every biconnected component is either a single edge
or a simple cycle. Equivalently, any two simple cycles share at most one vertex.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_108.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - Biconnected component decomposition via DFS. Each component is verified to be a single edge or a simple cycle. Complexity: O(n + m).

.. doxygenenum:: graph_recognition::CactusAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cactus
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CactusEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cactus_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cactus.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_cactus(g);

       std::cout << std::boolalpha << result.is_cactus << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cactus_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cactus_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary, G. E. Uhlenbeck. "On the number of husimi trees, I."
  *Proceedings of the National Academy of Sciences*, 39(4):315--322, 1953.
  `DOI:10.1073/pnas.39.4.315 <https://doi.org/10.1073/pnas.39.4.315>`_
